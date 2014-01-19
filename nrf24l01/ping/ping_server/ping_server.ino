/**
 * An Mirf example which copies back the data it recives.
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
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

int led = 3;
int led2 = 13;
void setup(){
  Serial.begin(9600);
  
    pinMode(led, OUTPUT);     
  pinMode(led2, OUTPUT);
  
  /*
   * Set the SPI Driver.
   */
   
Mirf.csnPin = 49;
Mirf.cePin = 48; 

  Mirf.spi = &MirfHardwareSpi;
  
  /*
   * Setup pins / SPI.
   */
   
  Mirf.init();
  
  /*
   * Configure reciving address.
   */
   
  Mirf.setRADDR((byte *)"serv1");
  
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
   
  Mirf.config();
  
  Serial.println("Listening..."); 
}

void loop(){
  digitalWrite(led2, LOW); 
  byte data[Mirf.payload];
  

   
  if(!Mirf.isSending() && Mirf.dataReady()){
    Mirf.getData(data);
   
    if((unsigned long)data > 0)
    {
      Serial.println("HIGH");
      digitalWrite(led, HIGH);
    }
    else
    {
      Serial.println("LOW");
      digitalWrite(led, LOW);
    }
  }
  
}
