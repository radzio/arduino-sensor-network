
#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>

#include "RF24Sensor.h"


RF24Sensor::RF24Sensor(uint8_t _cePin, uint8_t _csnPin, byte * receive_addr)
{
	addr = receive_addr;
	cePin = _cePin;
	csnPin = _csnPin;
}

void RF24Sensor::config()
{

	Mirf.cePin = cePin;
	Mirf.csnPin = csnPin;

	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
	Serial.println((char*)addr);
	Mirf.setRADDR(addr);
	Mirf.payload = sizeof(Message);
	Mirf.config();
}


void RF24Sensor::send(Message message, byte * toAddress)
{
	Mirf.setTADDR(toAddress);

	memcpy(message.addr, addr, 5);

	Mirf.send((byte *)&message);

	while (Mirf.isSending()){
		//Serial.println("sending");
	}
}
Message RF24Sensor::receive()
{
	Message message;
	if (!Mirf.isSending() && Mirf.dataReady()){
		Mirf.getData((byte *)&message);
		return message;
	}
	message.type = 0;
	return message;
}
