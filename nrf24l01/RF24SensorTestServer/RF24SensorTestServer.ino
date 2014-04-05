
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "RF24Sensor.h"

#define DEBUG_PRINT(x) Serial.print (x);
#define DEBUG_PRINTF(x) Serial.print (F(x));
#define DEBUG_PRINTFLN(x) Serial.println (F(x));
#define DEBUG_PRINTDEC(x) Serial.print (x, DEC);
#define DEBUG_PRINTLN(x) Serial.println (x);



#define DEBUG_PRINT_DETAILS(str)        \
	Serial.print(millis());     \
	Serial.print(": ");         \
	Serial.print(__PRETTY_FUNCTION__); \
	Serial.print(' ');          \
	Serial.print(__FILE__);     \
	Serial.print(':');          \
	Serial.print(__LINE__);     \
	Serial.print(' ');          \
	Serial.println(str);

int led = 3;

RF24Sensor sensor(6,7, (byte *)"serv1");

void setup(){
  Serial.begin(9600);
  Serial.print("Message size ");
  Serial.println(sizeof(Message));
  
  //pinMode(led, OUTPUT); 
  
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

	DEBUG_PRINTF("Message from: ");
	DEBUG_PRINT((char)message.addr[0]);
	DEBUG_PRINT((char)message.addr[1]);
	DEBUG_PRINT((char)message.addr[2]);
	DEBUG_PRINT((char)message.addr[3]);
	DEBUG_PRINT((char)message.addr[4]);
	DEBUG_PRINTF(" type: ");
	DEBUG_PRINT(message.type);
	DEBUG_PRINTF(" ");
	DEBUG_PRINTLN((char *)message.payload);
 
  
}
