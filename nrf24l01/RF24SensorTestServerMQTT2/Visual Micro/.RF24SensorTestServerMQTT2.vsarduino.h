#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Nano w/ ATmega328
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

int serial_putc(char c, FILE *);
void printf_begin(void);
static void dhcp_status_cb(int s, const uint16_t *dnsaddr);
static void resolv_found_cb(char *name, uint16_t *addr);
void ___dhcp_status(int s, const uint16_t *);
//
//
void webclient_datahandler(char *data, u16_t len);
void webclient_connected(void);
void webclient_timedout(void);
void webclient_aborted(void);
void webclient_closed(void);

#include "D:\arduino\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "D:\arduino\arduino-1.0.5\hardware\arduino\variants\eightanaloginputs\pins_arduino.h" 
#include "D:\Projects\arduino-sensor-network\nrf24l01\RF24SensorTestServerMQTT2\RF24SensorTestServerMQTT2.ino"
#endif
