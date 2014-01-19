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
      char addr[5];
      uint8_t type;
      
    } Message;
    

#endif