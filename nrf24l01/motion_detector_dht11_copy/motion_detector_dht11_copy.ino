/**
 * A Mirf example to test the latency between two Ardunio.
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 7
 *
 * Note: To see best case latency comment out all Serial.println
 * statements not displaying the result and load 
 * 'ping_server_interupt' on the server.
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>


#include <IRremote.h>

#include "DHT.h"

#define DHTPIN 2    // pin do którego podłączyliśmy dane z czujnika

#define DHTTYPE DHT11   // typ czujnika

DHT dht(DHTPIN, DHTTYPE);


int RECV_PIN = 5;

IRrecv irrecv(RECV_PIN);

decode_results results;


int calibrationTime = 15;        

long unsigned int lowIn;         

long unsigned int pause = 5000;  

int pirPin = 3;    


boolean lockLow = true;
boolean takeLowTime;  

void setup(){
  Serial.begin(9600);

  irrecv.enableIRIn(); // Start the receiver

  /*
   * Setup pins / SPI.
   */

  /* To change CE / CSN Pins:
   * 
   * Mirf.csnPin = 9;
   * Mirf.cePin = 7;
   */
  /*
  Mirf.cePin = 7;
   Mirf.csnPin = 8;
   */
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();

  /*
   * Configure reciving address.
   */

  Mirf.setRADDR((byte *)"clie1");

  /*
   * Set the payload length to sizeof(unsigned long) the
   * return type of millis().
   *
   * NB: payload on client and server must be the same.
   */

  Mirf.payload = sizeof(unsigned long);

  /*
   * Write channel and payload config then power up reciver.
   */

  /*
   * To change channel:
   * 
   * Mirf.channel = 10;
   *
   * NB: Make sure channel is legal in your area.
   */

  Mirf.config();

  Serial.println("Beginning ... "); 


  //pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    //delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  
  dht.begin();



}
unsigned long data = 0;
void loop(){


  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
    if(results.value != 0xFFFFFFFF)
    {
      Mirf.setTADDR((byte *)"serv1");
      data = data == 0 ? 1 :0;
      Serial.println(data);
      Mirf.setTADDR((byte *)"serv1");
      Mirf.send((byte *) &data);
      while(Mirf.isSending()){
      }
    }

  }



  if(digitalRead(pirPin) == HIGH){

    if(lockLow){  
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;            
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis()/1000);
      Serial.println(" sec"); 
      Mirf.setTADDR((byte *)"serv1");
      data = 1;
      Mirf.send((byte *) &data);
      while(Mirf.isSending()){
      }
      //Serial.println("Finished sending");


    }         
    takeLowTime = true;
  }

  if(digitalRead(pirPin) == LOW){       

    if(takeLowTime){
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause, 
    //we assume that no more motion is going to happen
    if(!lockLow && millis() - lowIn > pause){  
      //makes sure this block of code is only executed again after 
      //a new motion sequence has been detected
      lockLow = true;                        
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause)/1000);
      Serial.println(" sec");

      Mirf.setTADDR((byte *)"serv1");
      data = 0;
      Mirf.send((byte *) &data);
      while(Mirf.isSending()){
      }
      //Serial.println("Finished sending");

    }
  }




  float h = dht.readHumidity(); // odczyt wilgotności
  float t = dht.readTemperature(); // odczyt temperatury

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } 
  else {
    Serial.println(t); // Wyświetlenie linijki z temperaturą
    Serial.println(h); // Wyświetlenie linijki z wilgotnością
  }

  delay(2000);
} 




