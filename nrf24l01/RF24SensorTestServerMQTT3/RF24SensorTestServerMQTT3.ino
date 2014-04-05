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
//#include <EtherCard.h>



EthernetClient client;
#define PREFIX ""
WebServer httpServer(PREFIX, 80);
IPAddress ip(10, 89, 9, 88);
// ethernet mac address - must be unique on your network
static byte mac[]   = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
byte server_mqtt[] = { 10, 89, 9, 128 };



#define DEBUG;


const char server[] = "api.xively.com"; 
PubSubClient mqtt("api.xively.com", 1883, callback, client);

#define cs   1
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset


#define ENABLE LOW
#define DISABLE HIGH

#define nrf 49
#define tftDevice 1
#define lan 10
#define sd 4
#define devicesSize 4

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


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

//RF24Sensor::RF24Sensor(uint8_t _cePin, uint8_t _csnPin,  byte * receive_addr)
//Mirf.csnPin = 49;
//Mirf.cePin = 48;
RF24Sensor sensor(48, 49, (byte *)"serv1");

//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
float p = 3.1415926;

boolean ethOk = false;
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
	// handle message arrived
}


void onCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
	/* this line sends the standard "we're all OK" headers back to the
	browser */
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

	enableSPIDevice(tftDevice);

	//tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	//tft.fillScreen(ST7735_BLACK);
	//tft.setRotation(3);
	//tft.setTextSize(1);
	//tft.setTextWrap(true);
	//tft.setTextColor(ST7735_RED);
	//tft.setCursor(lineSpacing, lineSpacing);
	//tft.print(F("Initializing...."));
	
	enableSPIDevice(lan);


	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
		// no point in carrying on, so do nothing forevermore:
		// try to congifure using IP address instead of DHCP:
		Ethernet.begin(mac, ip);
	}
	// give the Ethernet shield a second to initialize:
	delay(1000);

	/*if (mqtt.connect("arduinoClient", "radek", "radzio")) {
		mqtt.publish("outTopic", "hello world");
		mqtt.subscribe("inTopic");
	}*/

	if (mqtt.connect("arduinoClient", "YxIFLpYxdj9s1huksqmOwHbvC8cVZcPZvkZINOtTobVyLCzt", "")) {
	
	}


	/* setup our default command that will be run when the user accesses
	* a page NOT on the server */
	//httpServer.setFailureCommand(&my_failCmd);

	/* run the same command if you try to load /index.html, a common
	* default page name */
	httpServer.addCommand("on.html", &onCmd);
	httpServer.addCommand("off.html", &offCmd);
	httpServer.begin();


//	ether.printIp(F("IP:  "), ether.myip);
//	ether.printIp(F("GW:  "), ether.gwip);
//	ether.printIp(F("DNS: "), ether.dnsip);
//	ether.printIp(F("SRV: "), ether.hisip);

	//enableSPIDevice(tftDevice);
	//tft.setTextColor(ST7735_YELLOW);
	//int y = lineHeight + 2* lineSpacing;
	//tft.setCursor(lineSpacing, y);
	////tft.printf(F("IP: %d.%d.%d.%d"), ether.myip[0],  ether.myip[1], ether.myip[2], ether.myip[3]);
	//y += (lineHeight + lineSpacing);
	//tft.setCursor(lineSpacing, y);
	////tft.printf(F("GW: %d.%d.%d.%d"), ether.gwip[0], ether.gwip[1], ether.gwip[2], ether.gwip[3]);
	//y += (lineHeight + lineSpacing);
	//tft.setCursor(lineSpacing, y);
	////tft.printf(F("DNS: %d.%d.%d.%d"), ether.dnsip[0], ether.dnsip[1], ether.dnsip[2], ether.dnsip[3]);
	//y += (lineHeight + lineSpacing);
	//tft.setCursor(lineSpacing, y);
	//tft.setTextColor(ST7735_GREEN);
	//tft.print("Starting radio...");

	//if(!ethOk)
	//{
	//	y += (lineHeight + lineSpacing);
	//	tft.setCursor(lineSpacing, y);
	//	tft.setTextColor(ST7735_RED);
	//	tft.print("Etherned failed...");
	//}

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


	//enableSPIDevice(tftDevice);

	//tft.fillScreen(ST7735_BLACK);
	//tft.setRotation(3);
	//tft.setTextSize(1);
	//tft.setTextWrap(true);
	//tft.setTextColor(ST7735_RED);
	//int yy = lineSpacing;
	//tft.setCursor(lineSpacing, lineSpacing);
	//tft.print(F("Get radio message"));

	//yy = getNextLineY(yy);
	//tft.setCursor(lineSpacing, yy);
	//tft.printf(F("From: %c%c%c%c%c"), message.addr[0], message.addr[1], message.addr[2], message.addr[3], message.addr[4]);

	////drawLine(x_start,y_start,x_ende,y_ende,farbe);
	////tft.drawLine(20, 50, 98, 50, ST7735_GREEN);
	//
	//if (ethOk == true)
	//{
	//	tft.fillCircle(12, 128 - 12, 10, ST7735_GREEN);
	//}
	//else
	//{
	//	tft.fillCircle(12, 128 - 12, 10, ST7735_RED);
	//}

	//switch (message.type)
	//{
	//case 0:

	//break;
	//case 1:

	//	break;
	//case 2:
	//	tft.setCursor(20, 55);
	//	if (((MotionPayload*)message.payload)->motion == 1)
	//	{
	//		tft.setTextColor(ST7735_RED);
	//	}
	//	else
	//	{
	//		tft.setTextColor(ST7735_GREEN);
	//	}
	//	tft.print(((MotionPayload*)message.payload)->motion);
	//	break;
	//case 3:
	//
	//	yy = getNextLineY(yy);
	//	tft.setCursor(lineSpacing, yy);
	//	tft.setTextColor(ST7735_RED);
	//	tft.print("Temperature: ");
	//	tft.print(((DhtPayload*)message.payload)->temperature);
	//	tft.print(" c");
	//	yy = getNextLineY(yy);
	//	tft.setCursor(lineSpacing, yy);
	//	tft.print("Humidity: ");
	//	tft.print(((DhtPayload*)message.payload)->humidity);
	//	tft.print(" %");
	//	yy += 12;
	//	tft.drawLine(lineSpacing, yy, 156 , yy, ST7735_GREEN);
	//	//tft.drawLine(lineSpacing, yy + 4, tft.width - lineSpacing, yy + 4, ST7735_CYAN);

	//	break;
	//case 4:
	//	tft.setCursor(20, 65);
	//	tft.setTextColor(ST7735_YELLOW);
	//	tft.print(((IrPayload*)message.payload)->irResult, HEX);
	//	break;
 //   case 5:
	//	tft.setCursor(20, 65);
	//	tft.setTextColor(ST7735_YELLOW);
 // 
	//	tft.print(((BmpPayload*)message.payload)->pressure);
	//	break;
	//}


	//enableSPIDevice(nrf);

}


void loop()
{
//	enableSPIDevice(lan);
//	handleHttpRequest();
	enableSPIDevice(nrf);
	Message message;
	message = sensor.receive();

	//handleHttpRequest();
	/*enableSPIDevice(lan);
	if (client.available()) {
		char c = client.read();
		Serial.print(c);
	}*/
	enableSPIDevice(lan);
	mqtt.loop();

	char buff[64];
	int len = 64;

	/* process incoming connections one at a time forever */
	httpServer.processConnection(buff, &len);

	if (message.type != 0)
	{
		printMessage(message);
		

		if (message.type == 2)
		{
			//enableSPIDevice(lan);
			sendMotionMessage((MotionPayload*)message.payload);
		}

		if (message.type == 5)
		{
			//enableSPIDevice(lan);
			sendBmpMessage((BmpPayload*)message.payload);
		}


		if (message.type == 3)
		{
			//enableSPIDevice(lan);
			sendDhtMessage((DhtPayload*)message.payload);
		}
	}
	


	enableSPIDevice(nrf);
}

void sendBmpMessage(BmpPayload* payload)
{
		if (!bmp.poll(5000))
		{
			return;
		}
		
	char temp[8];// = "12345";
	char pressure[8];// = "123456";
	char buffer2[40] = { "" };

		dtostrf(payload->temperature, 3, 2, temp);
		dtostrf(payload->pressure, 4, 2, pressure);
		//sprintf(buffer2, "cisnienie,%s\r\ntemperatura2,%s", pressure, temp);

		strcat(buffer2, "cisnienie,");
		strcat(buffer2, pressure);
		strcat(buffer2, "\r\n");
		strcat(buffer2, "temperatura2,");
		strcat(buffer2, temp);
		//DEBUG_PRINT("Buffer: ");
		//DEBUG_PRINTLN(buffer2);


		DEBUG_PRINTFLN("Sending... BMP");


		sendData(buffer2);
		//sendData("bmp");
		DEBUG_PRINTFLN("Sent... BMP");
}

void sendDhtMessage(DhtPayload* payload)
{
	if (!dht11.poll(5000))
	{
		return;
	}

	//DEBUG_PRINTLN(payload->temperature);
	//DEBUG_PRINTLN(payload->humidity);
//
	char temp[8];
	char humidity[8];
	char buffer2[40] = { "" };
	dtostrf(payload->temperature, 3, 2, temp);
	dtostrf(payload->humidity, 3, 2, humidity);
	//sprintf(buffer2, "temperatura,%s\r\nwilgotnosc,%s", temp, humidity);


	strcat(buffer2, "temperatura,");
	strcat(buffer2, temp);
	strcat(buffer2, "\r\n");
	strcat(buffer2, "wilgotnosc,");
	strcat(buffer2, humidity);

	//DEBUG_PRINTLN(temp);
	//DEBUG_PRINTLN(humidity);

	//DEBUG_PRINT("Buffer: ");
	//DEBUG_PRINTLN(buffer2);


	DEBUG_PRINTFLN("Sending... DHT");


	//sendData("dht");
	sendData(buffer2);

	DEBUG_PRINTFLN("Sent... DHT");
}

void sendMotionMessage(MotionPayload* payload)
{
	char buffer2[30] = { "" };
	sprintf(buffer2, "motion,%i\0", payload->motion);


	DEBUG_PRINT("Buffer: ");
	DEBUG_PRINTLN(buffer2);


	DEBUG_PRINTFLN("Sending... Motion");


	sendData(buffer2);

	DEBUG_PRINTFLN("Sent... Motion");
}




void sendData( char* data)
{
	enableSPIDevice(lan);
	//if (client.connect(server, 80)) {
	//	Serial.println("connecting...");
	//	// send the HTTP PUT request:
	//	client.print(F("PUT /v2/feeds/"));
	//	client.print(F("369115982"));
	//	client.println(".csv HTTP/1.1");
	//	client.println(F("Host: api.xivey.com"));
	//	client.print(F("X-ApiKey: "));
	//	client.println(F("YxIFLpYxdj9s1huksqmOwHbvC8cVZcPZvkZINOtTobVyLCzt"));
	//	client.print(F("User-Agent: "));
	//	client.println(F("arduino"));
	//	client.print(F("Content-Length: "));
	//	client.println(strlen(data));

	//	// last pieces of the HTTP PUT request:
	//	client.println(F("Content-Type: text/csv"));
	//	client.println(F("Connection: close"));
	//	client.println();

	//	// here's the actual content of the PUT request:
	//	client.println(data);
	//	//DEBUG_PRINTLN(data);
	//	//delay(10000);

	//	while (client.connected()) {
	//		// the server will close the connection when it is finished sending packets
	//		while (client.available()) {
	//			// ...but not until this buffer is empty
	//			client.read();
	//			//length++;
	//		}
	//	}
	//	// close your end after the server closes its end
	//	client.stop();

	//}
	//else {
	//	// if you couldn't make a connection:
	//	Serial.println("connection failed");
	//	Serial.println();
	//	Serial.println("disconnecting.");
	//	client.stop();
	//}
	DEBUG_PRINTLN(data);

	if (!mqtt.connected())
	{
		if (mqtt.connect("arduinoClient", "YxIFLpYxdj9s1huksqmOwHbvC8cVZcPZvkZINOtTobVyLCzt", "")) {

		}
	}

	mqtt.publish("/v2/feeds/369115982.csv", data);
	DEBUG_PRINTLN(data);
	/*Serial.println(F("Free BEFORE RAM = ")); 
	Serial.println(freeMemory(), DEC);

	aJsonObject* root = aJson.createObject();

	aJsonObject* fmt = aJson.createObject();

	aJson.addItemToObject(root, "data", fmt);

	aJson.addNumberToObject(fmt, "temperature", 14.0);
	aJson.addNumberToObject(fmt, "humidity", 55.0);

	aJson.addStringToObject(root, "sensor", "serv1");

	char* string = aJson.print(root);
	aJson.deleteItem(root);
	Serial.println(string);
	
	mqtt.publish("outTopic", string);

	Serial.println(F("Free BEFORE DELETE RAM = "));
	Serial.println(freeMemory(), DEC);
	free(string);


	Serial.println(F("Free AFTER RAM = "));
	Serial.println(freeMemory(), DEC);*/

}





