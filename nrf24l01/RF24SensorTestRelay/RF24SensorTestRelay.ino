

#include <FiniteStateMachine.h>
//#include <SPI.h>
//#include <Mirf.h>
//#include <nRF24L01.h>
//#include <MirfHardwareSpiDriver.h>

#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>

#include "RF24Sensor.h"

#define DEBUG_PRINT(x) Serial.print (x);
#define DEBUG_PRINTF(x) Serial.print (F(x));
#define DEBUG_PRINTFLN(x) Serial.println (F(x));
#define DEBUG_PRINTDEC(x) Serial.print (x, DEC);
#define DEBUG_PRINTLN(x) Serial.println (x);



//#define DEBUG_PRINT_DETAILS(str)        \
//	Serial.print(millis());     \
//	Serial.print(": ");         \
//	Serial.print(__PRETTY_FUNCTION__); \
//	Serial.print(' ');          \
//	Serial.print(__FILE__);     \
//	Serial.print(':');          \
//	Serial.print(__LINE__);     \
//	Serial.print(' ');          \
//	Serial.println(str);


RF24Sensor sensor(6,7, (byte *)"lamp1");


const int powerPin = 3; 
const int knockSensor = A2; // the piezo is connected to analog pin 0
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not

int sensorReading = 0;      // variable to store the value read from the sensor pin
int ledState = HIGH;         // variable used to store the last LED status, to toggle the light
//int lampState = LOW;
long lastMillis = 0;

void setup(){
  Serial.begin(9600);
  Serial.print("Message size ");
  Serial.println(sizeof(Message));
  sensor.config();
  pinMode(powerPin, OUTPUT); // declare the ledPin as as OUTPUT\
  digitalWrite(3, HIGH);
}

void loop()
{
//  digitalWrite(3, HIGH);
//  delay(4000);
//  digitalWrite(3, LOW);
//  delay(4000);
  klaskacz();
  Message message;
  message = sensor.receive();
  if(message.type == 0)
  {

  }
  else
  {
    if(message.type == 6)
    {
       printMessage(message);
       DEBUG_PRINT(((RelayPayload *) message.payload)->onOff);
       handlePayload((RelayPayload *) message.payload);
    }
  }
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
int lastReading = 1000;
void klaskacz()
{
   sensorReading = analogRead(knockSensor);
   int diff = lastReading - sensorReading;
   //Serial.println(diff);
  if (diff >= 100) 
  {
      Serial.println(sensorReading);
      ledState = !ledState;  
      digitalWrite(powerPin, ledState);
      Serial.println("Knock!"); 
      delay(2000);    
  }
}

void handlePayload(RelayPayload* payload)
{
   ledState = !payload->onOff; 
   Serial.print("Turn : ");
   Serial.println(ledState == HIGH? "off" : "on");
   digitalWrite(powerPin, ledState);
   delay(2000);
}
