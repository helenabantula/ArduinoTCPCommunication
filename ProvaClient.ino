#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <TimeLib.h>


EthernetClient client;
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xCB, 0x2D};
IPAddress ip(192, 168, 1, 177);
IPAddress server(192, 168, 1, 120);
unsigned int  serverPort = 11999;


bool persona = false;
pinMode(9, OUTPUT);

volatile unsigned int timeCount=0;


ISR(TIMER1_COMPA_vect){ //Ep, he arribat a 40, incremento una variable
  timeCount++;
}

void setup() {
  
  Ethernet.begin(mac, ip);
  
  Serial.begin(9600);

  // set up Timer 1
  TCCR1A = 0;          // normal operation
  TCCR1B = bit(WGM12) | bit(CS10) | bit (CS12);   // CTC, scale to clock / 1024
  OCR1A =  3125;       //compare A register value (clock speed / 1024 * 3125) -> Aprox 5Hz
  TIMSK1 = bit (OCIE1A);  //interrupt on Compare A Match
  

    // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }

  delay(1000);

  Serial.println("Connecting...");

  if (client.connect(server, serverPort)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed");
  }
}


void loop() {
  if (!client.connected()){
    if(timeCount>=50){  //This will be 10 seconds
       noInterrupts();
       timeCount=0;
       interrupts();        
       Serial.println("Re-connecting");
       client.flush();
       client.stop();
       delay(50);
       client.connect(server, serverPort);
    }
  }
  else{

    //Serial.println(persona);
    int heartRate = 60;
    String ID = "R";

    if(persona){
       char readedData = client.read();
      if (readedData =='A')
          digitalWrite(9, HIGH);     
      else
          digitalWrite(9, LOW);
    }

    
    if(timeCount>=25 && !persona){
       client.println("Y0");
       Serial.println("Y0"); 
       client.println(ID+heartRate);
       persona = true;
       noInterrupts();
       timeCount=0;
       interrupts();

    }

     if(timeCount>=25 && persona){
       client.println("Z0");
       Serial.println("Z0"); 
       persona = false;
       noInterrupts();
       timeCount=0;
       interrupts();
    }
  }
}
