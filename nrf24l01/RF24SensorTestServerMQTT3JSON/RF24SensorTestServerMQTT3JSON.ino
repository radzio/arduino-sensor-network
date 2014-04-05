//#include <Adafruit_ST7735.h>
//#include <Adafruit_GFX.h>

#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#define DEBUG
#include "RF24Sensor.h"
#include "DebugUtils.h"
#include <MemoryFree.h>
#include "MilliTimer.h"
#include <Ethernet.h>
#include <PubSubClient.h>
#include <aJSON.h>
#include <WebServer.h>
#include <Util.h>
//#include <EtherCard.h>



EthernetClient client;
#define PREFIX ""
WebServer httpServer(PREFIX, 80);
IPAddress ip(10, 89, 9, 89);

// ethernet mac address - must be unique on your network
static byte mac[]   = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x30 };
byte server_mqtt[] = { 10, 89, 9, 128 };

#define DEBUG;


//PubSubClient mqtt("api.xively.com", 1883, callback, client);
PubSubClient mqtt(server_mqtt, 1883, callback, client);

#define cs   1
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset


#define ENABLE LOW
#define DISABLE HIGH

#define nrf 7
#define tftDevice 1
#define lan 10
#define sd 4
#define devicesSize 4


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

//RF24Sensor::RF24Sensor(uint8_t _cePin, uint8_t _csnPin,  byte * receive_addr)
//Mirf.csnPin = 49;
//Mirf.cePin = 48;
RF24Sensor sensor(6, 7, (byte *)"serv1");


int devices[4] = { nrf, tftDevice, lan,sd };

void enableSPIDevice(int pin)
{
	for (int i = 0; i< devicesSize; i++)
	{
		if (devices[i] != pin)
		{
			digitalWrite(devices[i], DISABLE);
		}
	}
	digitalWrite(pin, ENABLE);
}

void callback(char* topic, byte* payload, unsigned int length) {
	if (strcmp(topic, "/arduinoProxyListener") == 0)
	{
		byte buffer[37];

		for (int i = 0; i < length; i++)
		{
			buffer[i] = payload[i];
		}

		Envelope* env = (Envelope* ) &buffer;
		printMessage(env->message);
		enableSPIDevice(nrf);
		sensor.send(env->message, (byte *)"lamp1");
		enableSPIDevice(lan);
	}
}


void onCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
	
	server.httpSuccess();

	enableSPIDevice(nrf);

	Message message;
	message.type = 6;
	RelayPayload relay;
	relay.onOff = 1;
	memcpy(message.payload, &relay, sizeof(RelayPayload));

	sensor.send(message, (byte *)"lamp1");
	enableSPIDevice(lan);
	server.print(F("ON"));

}


void offCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
	/* this line sends the standard "we're all OK" headers back to the
	browser */
	server.httpSuccess();

	enableSPIDevice(nrf);

	Message message;
	message.type = 6;
	RelayPayload relay;
	relay.onOff = 0;
	memcpy(message.payload, &relay, sizeof(RelayPayload));

	sensor.send(message, (byte *)"lamp1");

	enableSPIDevice(lan);


	server.print(F("OFF"));
}


void setup()
{
#ifdef DEBUG
	Serial.begin(9600);
#endif
	pinMode(10, OUTPUT);
	pinMode(4, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
	enableSPIDevice(nrf);

	sensor.config();


	
	enableSPIDevice(lan);


	if (Ethernet.begin(mac) == 0) {
		
		Ethernet.begin(mac, ip);
	}
	delay(1000);

	
	httpServer.addCommand("on.html", &onCmd);
	httpServer.addCommand("off.html", &offCmd);
	httpServer.begin();

	delay(3000);
	enableSPIDevice(nrf);
	
}

void mqttConnect()
{
	if (mqtt.connected() == true)
		return;
	DEBUG_PRINTF("Connecting...");
	if (mqtt.connect("arduinoClient")) {
		mqtt.subscribe("/arduinoProxyListener");
		DEBUG_PRINTFLN(" Connected");
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


bool isRegistered = false;

void loop()
{
	enableSPIDevice(nrf);
	Message message;
	message = sensor.receive();
	enableSPIDevice(lan);
	mqtt.loop();

	char buff[32];
	int len = 32;

	if (isRegistered == false)
	{
		Message registerMessage;
		registerMessage.type = (uint8_t) Register;

		RegisterPayLoad registerPayload;
		byte guid[] = { 0x04, 0x2b, 0xcf, 0x20, 0xa2, 0xa2, 0xbf, 0x49, 0x83, 0x86, 0x37, 0x47, 0x5c, 0x95, 0x7d, 0x8c };
		byte deviceType[] = { B11000000, 0x00, 0x00, 0x00 };

		memcpy(registerPayload.guid, &guid, 16);
		memcpy(registerPayload.deviceType, &deviceType, 4);

		/*for (int i = 0; i < 16; i++)
		{
			Serial.println(registerPayload.guid[i], HEX);
		}

		for (int i = 0; i < 4; i++)
		{
			Serial.println(registerPayload.deviceType[i], BIN);
		}*/

		memcpy(registerMessage.payload, &registerPayload, sizeof(RegisterPayLoad));
		memcpy(registerMessage.addr, (byte *) "test1", 5);
		mqttConnect();
		mqtt.publish("/v2/test", (byte *)&registerMessage, 32);

		isRegistered = true;
	}


	httpServer.processConnection(buff, &len);


	if (message.type != 0)
	{
		printMessage(message);
		
		mqttConnect();
		mqtt.publish("/v2/test", (byte *) &message, 32);
	}
	enableSPIDevice(nrf);
	Serial.print(".");
}


//float ReverseFloat(const float inFloat)
//{
//	float retVal;
//	char *floatToConvert = (char*)& inFloat;
//	char *returnFloat = (char*)& retVal;
//
//	// swap the bytes into a temporary buffer
//	returnFloat[0] = floatToConvert[3];
//	returnFloat[1] = floatToConvert[2];
//	returnFloat[2] = floatToConvert[1];
//	returnFloat[3] = floatToConvert[0];
//
//	return retVal;
//}
//
//void serialFloatPrint(float f) {
//	byte * b = (byte *)&f;
//	Serial.println();
//	Serial.print(b[0], HEX);
//	Serial.print(b[1], HEX);
//	Serial.print(b[2], HEX);
//	Serial.println(b[3], BIN);
//}