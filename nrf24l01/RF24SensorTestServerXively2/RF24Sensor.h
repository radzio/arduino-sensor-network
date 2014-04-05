#ifndef RF24Sensor_H
#define RF24Sensor_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

typedef struct {
      float temperature;
      float humidity;
    } DhtPayload;
    
typedef struct {
      float temperature;
      float pressure;
    } BmpPayload;
    
typedef struct {
      float temperature;
    } TemperaturePayload;
    
typedef struct {
       uint8_t motion;
    } MotionPayload;
    
typedef struct {
       unsigned long irResult;
    } IrPayload;

typedef struct {
      byte addr[5];
      uint8_t type;
      byte payload[26];
    } Message;
    
    
class RF24Sensor {
 private:
  byte * addr;
  uint8_t cePin;
  uint8_t csnPin;
  Nrf24l Mirf;

 public:
  RF24Sensor(uint8_t cePin, uint8_t csnPin,  byte * addr);
  void config();
  void send(Message message, byte * addr);
  Message receive();
};
    

#endif

