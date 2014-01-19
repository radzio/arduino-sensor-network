
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "RF24Sensor.h"

int led = 3;

RF24Sensor sensor(48,49, (byte *)"serv1");

void setup(){
  Serial.begin(9600);
  Serial.print("Message size ");
  Serial.println(sizeof(Message));
  
  pinMode(led, OUTPUT); 
  
  sensor.config();
}

void loop(){
Message message;

  message = sensor.receive();
  if(message.type == 0)
  {
//     Serial.println("No message"); 
//     Serial.println((char *)message.payload); 
//     Serial.println((char *)message.addr); 
  }
  else
  {
     
     printMessage(message);  
     if(message.type == 2)
     {
         if(strcmp("motion/1", (char *) message.payload) == 0)
         {
            digitalWrite(led, HIGH);
         }
         if(strcmp("motion/0", (char *) message.payload) == 0)
         {
            digitalWrite(led, LOW);
         }
     }
  }

  delay(1000);
}

void printMessage(Message message)
{
  Serial.print("Message from: ");
  Serial.print((char)message.addr[0]); 
  Serial.print((char)message.addr[1]); 
  Serial.print((char)message.addr[2]); 
  Serial.print((char)message.addr[3]); 
  Serial.print((char)message.addr[4]);
  Serial.print(" type: ");
  Serial.print(message.type);
  Serial.print(" ");
  Serial.println((char *) message.payload);
 
  
}
