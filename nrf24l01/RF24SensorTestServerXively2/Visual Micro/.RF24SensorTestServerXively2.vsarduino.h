#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Uno
#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define ARDUINO 105
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

int getNextLineY(int lineY);
void enableSPIDevice(int pin);
void callback(char* topic, byte* payload, unsigned int length);
//
void printMessage(Message message);
//
void handleHttpRequest();
void sendBmpMessage(BmpPayload* payload);
void sendDhtMessage(DhtPayload* payload);
void sendMotionMessage(MotionPayload* payload);
void sendData( char* data);

#include "D:\arduino\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "D:\arduino\arduino-1.0.5\hardware\arduino\variants\standard\pins_arduino.h" 
#include "D:\Projects\arduino-sensor-network\nrf24l01\RF24SensorTestServerXively2\RF24SensorTestServerXively2.ino"
#include "D:\Projects\arduino-sensor-network\nrf24l01\RF24SensorTestServerXively2\DebugUtils.h"
#include "D:\Projects\arduino-sensor-network\nrf24l01\RF24SensorTestServerXively2\MilliTimer.cpp"
#include "D:\Projects\arduino-sensor-network\nrf24l01\RF24SensorTestServerXively2\MilliTimer.h"
#include "D:\Projects\arduino-sensor-network\nrf24l01\RF24SensorTestServerXively2\RF24Sensor.cpp"
#include "D:\Projects\arduino-sensor-network\nrf24l01\RF24SensorTestServerXively2\RF24Sensor.h"
#endif
