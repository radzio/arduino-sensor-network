
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "RF24Sensor.h"
#include <DHT.h>
#include <IRremote.h>

int DHT_PIN = 2;
DHT dht(DHT_PIN, DHT11);

int RECV_PIN = 5;
IRrecv irrecv(RECV_PIN);

decode_results results;

int pirPin = 3;  
int calibrationTime = 15;        
long unsigned int lowIn;         
long unsigned int pause = 5000;   
boolean lockLow = true;
boolean takeLowTime;  

char * serverAddr = "serv1";

RF24Sensor sensor(8,7, (byte *)"clie1");

void setup(){
  Serial.begin(9600);
  Serial.println(sizeof(DhtPayload));
  Serial.println(sizeof(TemperaturePayload));
  Serial.println(sizeof(MotionPayload));
  Serial.println(sizeof(IrPayload));
  sensor.config();
  initalizePirSensor();
  dht.begin();
  
  irrecv.enableIRIn(); // Start the receiver
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
  //Serial.println((char *) message.payload);
}


void loop()
{
  checkPirSensor();
  checkDHTSensor();
  checkIrSensor();
}


void initalizePirSensor()
{
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  Serial.print("Calibrating PIR sensor ");
  for(int i = 0; i < calibrationTime; i++)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE"); 
}

long unsigned int lastDHTReading = 0;   
long unsigned int DHTPause = 5000;

void checkDHTSensor()
{
  if(millis() - lastDHTReading < DHTPause)
  return;
  
  float h = dht.readHumidity(); // odczyt wilgotności
  float t = dht.readTemperature(); // odczyt temperatury

  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else 
  {
    Message message;
    message.type = 3;
    DhtPayload dhtPayload;
    dhtPayload.temperature = t;
    dhtPayload.humidity = h;

    memcpy(message.payload, &dhtPayload , sizeof(DhtPayload));
    sensor.send(message, (byte *) serverAddr);
    //printMessage(message);
    //memcpy(&dhtPayload, message.payload, sizeof(dhtPayload));
    //Serial.println( ((DhtPayload*)message.payload)->humidity);
    //Serial.println( ((DhtPayload*)message.payload)->temperature);
    lastDHTReading = millis();
  } 
}

void checkIrSensor()
{
    if (irrecv.decode(&results)) 
    {

      irrecv.resume(); // Receive the next value
      if(results.value != 0xFFFFFFFF)
      {
        Serial.println(results.value, HEX);
        Message message;
        message.type = 4; 
        IrPayload irPayload;
        irPayload.irResult = results.value;
        memcpy(message.payload, &irPayload , sizeof(DhtPayload));
        
        sensor.send(message, (byte *) serverAddr);
      }
  } 
}

void checkPirSensor()
{
  MotionPayload motionPayload;
  if(digitalRead(pirPin) == HIGH)
  {
  
    if(lockLow)
    {  
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;  
      Message message;
      message.type = 2;
      motionPayload.motion = 1;
      memcpy(message.payload, &motionPayload , sizeof(MotionPayload));
      sensor.send(message, (byte *) serverAddr);
      
    }         
    takeLowTime = true;
  }
  
  if(digitalRead(pirPin) == LOW)
  {       
    if(takeLowTime)
    {
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause, 
    //we assume that no more motion is going to happen
    if(!lockLow && millis() - lowIn > pause)
    {  
      lockLow = true;  
      Message message;
      message.type = 2;
      motionPayload.motion = 0;
      memcpy(message.payload, &motionPayload , sizeof(MotionPayload)); 
      sensor.send(message, (byte *) serverAddr);      
    }
  } 
}
