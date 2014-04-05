#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#define DEBUG
#include "RF24Sensor.h"
#include "DebugUtils.h"
#include "MilliTimer.h"

#include <EtherCard.h>

#define STATIC 1  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 10, 89, 9, 88 };
// gateway ip address
static byte gwip[] = { 10, 89, 9, 1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[]   = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

// change these settings to match your own setup
#define FEED    "369115982"
#define APIKEY  "YxIFLpYxdj9s1huksqmOwHbvC8cVZcPZvkZINOtTobVyLCzt"

char page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
"<head><title>"
"Service Temporarily Unavailable"
"</title></head>"
"<body>"
"<h3>This service is currently unavailable</h3>"
"<p><em>"
"The main server is currently off-line.<br />"
"Please try again later."
"</em></p>"
"</body>"
"</html>"
;
//Program:   20936 bytes (63.9% Full)
//Program:   21222 bytes (64.8% Full)

//Data:       1406 bytes(68.7% Full)
#define DEBUG;




char website[] PROGMEM = "api.xively.com";
byte Ethernet::buffer[700];
Stash stash;


#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset


#define ENABLE LOW
#define DISABLE HIGH

#define nrf 3
#define tftDevice 10
#define lan 5
#define devicesSize 3

const uint8_t lineHeight = 8;
const uint8_t lineSpacing = 4;
const uint8_t charWidth = 2;

MilliTimer dht11;
MilliTimer bmp;

int getNextLineY(int lineY)
{
	return lineY + 10;
}

int lastX ;
int lastY = lineSpacing;

long unsigned int m3Last = 0;   
long unsigned int m5Last = 23000;
long unsigned int pause = 5000;

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

//RF24Sensor::RF24Sensor(uint8_t _cePin, uint8_t _csnPin,  byte * receive_addr)
RF24Sensor sensor(4, 3, (byte *)"serv1");

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
float p = 3.1415926;

boolean ethOk = false;
int devices[3] = { nrf, tftDevice, lan };

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



void setup()
{
#ifdef DEBUG
	Serial.begin(9600);
#endif
	pinMode(10, OUTPUT);
	pinMode(5, OUTPUT);

	enableSPIDevice(nrf);

	sensor.config();

	enableSPIDevice(tftDevice);

	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	tft.fillScreen(ST7735_BLACK);
	tft.setRotation(3);
	tft.setTextSize(1);
	tft.setTextWrap(true);
	tft.setTextColor(ST7735_RED);
	tft.setCursor(lineSpacing, lineSpacing);
	tft.print(F("Initializing...."));
	
	enableSPIDevice(lan);

	/*if (ether.begin(sizeof Ethernet::buffer, mymac, 5) == 0)
	{
		DEBUG_PRINTFLN("Failed to access Ethernet controller");
	}
	else
	{
        ethOk = true;
		DEBUG_PRINTFLN("Ethernet controller initialized ");
	}*/
	ether.persistTcpConnection(true);
#if STATIC
	ether.staticSetup(myip, gwip);
#else
	if (!ether.dhcpSetup())
	{
		DEBUG_PRINTF("DHCP failed");
	}
#endif



	//static byte dnsip[] = {8,8,4,4};
	static byte dnsip[] = { 8, 8, 4, 4 };
	ether.copyIp(ether.dnsip, dnsip);
	static byte hisip[] = { 216, 52, 233, 120 };
	//static byte hisip[] = { 10, 89, 9, 128 };

	  //if (!ether.dnsLookup(website))
	  //{
	  //  Serial.println("DNS failed, doing fallback");
		ether.copyIp(ether.hisip, hisip);
	  //}

	ether.printIp(F("IP:  "), ether.myip);
	ether.printIp(F("GW:  "), ether.gwip);
	ether.printIp(F("DNS: "), ether.dnsip);
	ether.printIp(F("SRV: "), ether.hisip);

	enableSPIDevice(tftDevice);
	tft.setTextColor(ST7735_YELLOW);
	int y = lineHeight + 2* lineSpacing;
	tft.setCursor(lineSpacing, y);
	tft.printf(F("IP: %d.%d.%d.%d"), ether.myip[0],  ether.myip[1], ether.myip[2], ether.myip[3]);
	y += (lineHeight + lineSpacing);
	tft.setCursor(lineSpacing, y);
	tft.printf(F("GW: %d.%d.%d.%d"), ether.gwip[0], ether.gwip[1], ether.gwip[2], ether.gwip[3]);
	y += (lineHeight + lineSpacing);
	tft.setCursor(lineSpacing, y);
	tft.printf(F("DNS: %d.%d.%d.%d"), ether.dnsip[0], ether.dnsip[1], ether.dnsip[2], ether.dnsip[3]);
	y += (lineHeight + lineSpacing);
	tft.setCursor(lineSpacing, y);
	tft.setTextColor(ST7735_GREEN);
	tft.print("Starting radio...");

	if(!ethOk)
	{
		y += (lineHeight + lineSpacing);
		tft.setCursor(lineSpacing, y);
		tft.setTextColor(ST7735_RED);
		tft.print("Etherned failed...");
	}

	delay(3000);
	enableSPIDevice(lan);
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


	enableSPIDevice(tftDevice);

	tft.fillScreen(ST7735_BLACK);
	tft.setRotation(3);
	tft.setTextSize(1);
	tft.setTextWrap(true);
	tft.setTextColor(ST7735_RED);
	int yy = lineSpacing;
	tft.setCursor(lineSpacing, lineSpacing);
	tft.print(F("Get radio message"));

	yy = getNextLineY(yy);
	tft.setCursor(lineSpacing, yy);
	tft.printf(F("From: %c%c%c%c%c"), message.addr[0], message.addr[1], message.addr[2], message.addr[3], message.addr[4]);

	//drawLine(x_start,y_start,x_ende,y_ende,farbe);
	//tft.drawLine(20, 50, 98, 50, ST7735_GREEN);
	
	if (ethOk == true)
	{
		tft.fillCircle(12, 128 - 12, 10, ST7735_GREEN);
	}
	else
	{
		tft.fillCircle(12, 128 - 12, 10, ST7735_RED);
	}

	switch (message.type)
	{
	case 0:

	break;
	case 1:

		break;
	case 2:
		tft.setCursor(20, 55);
		if (((MotionPayload*)message.payload)->motion == 1)
		{
			tft.setTextColor(ST7735_RED);
		}
		else
		{
			tft.setTextColor(ST7735_GREEN);
		}
		tft.print(((MotionPayload*)message.payload)->motion);
		break;
	case 3:
	
		yy = getNextLineY(yy);
		tft.setCursor(lineSpacing, yy);
		tft.setTextColor(ST7735_RED);
		tft.print("Temperature: ");
		tft.print(((DhtPayload*)message.payload)->temperature);
		tft.print(" c");
		yy = getNextLineY(yy);
		tft.setCursor(lineSpacing, yy);
		tft.print("Humidity: ");
		tft.print(((DhtPayload*)message.payload)->humidity);
		tft.print(" %");
		yy += 12;
		tft.drawLine(lineSpacing, yy, 156 , yy, ST7735_GREEN);
		//tft.drawLine(lineSpacing, yy + 4, tft.width - lineSpacing, yy + 4, ST7735_CYAN);

		break;
	case 4:
		tft.setCursor(20, 65);
		tft.setTextColor(ST7735_YELLOW);
		tft.print(((IrPayload*)message.payload)->irResult, HEX);
		break;
    case 5:
		tft.setCursor(20, 65);
		tft.setTextColor(ST7735_YELLOW);
  
		tft.print(((BmpPayload*)message.payload)->pressure);
		break;
	}


	enableSPIDevice(nrf);

}


void loop()
{
	enableSPIDevice(lan);
	handleHttpRequest();
	enableSPIDevice(nrf);
	Message message;
	message = sensor.receive();

	//handleHttpRequest();
	
	if (message.type != 0)
	{
		printMessage(message);
		/*enableSPIDevice(lan);

		if (stash.freeCount() <= 40) {
			DEBUG_PRINTLN(">>>>>>>>>>CLEAN STASH<<<<<<<:");
			Stash::initMap(56);
		}

		if (message.type == 2)
		{
			enableSPIDevice(lan);
			sendMotionMessage((MotionPayload*)message.payload);
		}

		if (message.type == 5)
		{
			enableSPIDevice(lan);
			sendBmpMessage((BmpPayload*)message.payload);
		}


		if (message.type == 3)
		{
			enableSPIDevice(lan);
			sendDhtMessage((DhtPayload*)message.payload);
		}
		delay(700);*/
	}
	


	enableSPIDevice(nrf);
}

void handleHttpRequest()
{
	enableSPIDevice(lan);

	if (Stash::freeCount()<5){ // reset EtherCard stash if running low
		Stash::initMap(56);
		DEBUG_PRINTF("--------------------Cleaning stash-------------------");

	}

	if (ether.packetLoop(ether.packetReceive())) {
		DEBUG_PRINTF("receive packet");
		memcpy_P(ether.tcpOffset(), page, sizeof page);
		ether.httpServerReply(sizeof page - 1);

		enableSPIDevice(tftDevice);
		tft.setTextSize(1);
		tft.setTextWrap(true);
		tft.setCursor(lineSpacing, 70);
		tft.setTextColor(ST7735_WHITE);
		tft.print(F("New HTTP REQUEST"));
		enableSPIDevice(lan);
	}
}

void sendBmpMessage(BmpPayload* payload)
{
		if (!bmp.poll(10000))
		{
			return;
		}
		//ether.packetLoop(ether.packetReceive());
		//ether.packetLoop(ether.packetReceive());
		stash.cleanup();
		byte sd = stash.create();
		stash.print("temperatura2,");
		stash.println(payload->temperature);
		stash.print("cisnienie,");
		stash.println(payload->pressure);
		stash.save();

		DEBUG_PRINTLN(payload->temperature);
		DEBUG_PRINTLN(payload->pressure);

		Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
			"Host: $F" "\r\n"
			"X-ApiKey: $F" "\r\n"
			"Content-Length: $D" "\r\n"
			"\r\n"
			"$H"),
			website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);

		// send the packet - this also releases all stash buffers once done
		DEBUG_PRINTFLN("Sending... BMP");
		/*byte sessionId = ether.tcpSend();
		long last = millis();
		const char * reply;
		do
		{
			if (millis() - last > 3000)
				break;
			
			ether.packetLoop(ether.packetReceive());
			Serial.print(".");
			const char * reply = ether.tcpReply(sessionId);
		} while (reply == 0);
		Serial.println("reply: ");
		Serial.println(reply);*/
		/*while (ether.tcpReply(sessionId) == 0)
		{
			ether.packetLoop(ether.packetReceive());
			Serial.print(".");
		}
		const char* reply = ether.tcpReply(sessionId);
		if (reply > 0) {
			Serial.print("reply: ");
			Serial.println(reply);
		}*/
		ether.packetLoop(ether.packetReceive());
		DEBUG_PRINTFLN("Sent... BMP");
}

void sendDhtMessage(DhtPayload* payload)
{
	if (!dht11.poll(10000))
	{
		return;
	}
	//ether.packetLoop(ether.packetReceive());
	stash.cleanup();
	byte sd = stash.create();
	stash.print("temperatura,");
	stash.println(payload->temperature);
	stash.print("wilgotnosc,");
	stash.println(payload->humidity);
	stash.save();

	DEBUG_PRINTLN(payload->temperature);
	DEBUG_PRINTLN(payload->humidity);

	Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
		"Host: $F" "\r\n"
		"X-ApiKey: $F" "\r\n"
		"Content-Length: $D" "\r\n"
		"\r\n"
		"$H"),
		website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);

	// send the packet - this also releases all stash buffers once done
	DEBUG_PRINTFLN("Sending... DHT");
	ether.tcpSend();
	ether.packetLoop(ether.packetReceive());
	DEBUG_PRINTFLN("Sent... DHT");
}

void sendMotionMessage(MotionPayload* payload)
{
	//ether.packetLoop(ether.packetReceive());
	stash.cleanup();
	byte sd = stash.create();
	stash.print("motion,");
	stash.println(payload->motion);
	stash.save();

	DEBUG_PRINTLN(payload->motion);

	Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
		"Host: $F" "\r\n"
		"X-ApiKey: $F" "\r\n"
		"Content-Length: $D" "\r\n"
		"\r\n"
		"$H"),
		website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);

	// send the packet - this also releases all stash buffers once done
	DEBUG_PRINTFLN("Sending... MOTION");
	ether.tcpSend();
	ether.packetLoop(ether.packetReceive());
	DEBUG_PRINTFLN("Sent... MOTION");
}




