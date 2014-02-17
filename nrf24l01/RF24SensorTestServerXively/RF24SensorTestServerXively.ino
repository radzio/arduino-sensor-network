#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "RF24Sensor.h"

#include <EtherCard.h>

#define STATIC 0  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// ethernet interface ip address
static byte myip[] = { 10,89,9,80 };
// gateway ip address
static byte gwip[] = { 10,89,9,1 };
#endif

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

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


#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

RF24Sensor sensor(4,3, (byte *)"serv1");

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
float p = 3.1415926;


int devices[3] = {nrf, tftDevice, lan };

void enableSPIDevice(int pin)
{
   for(int i = 0; i< devicesSize; i++)
   {
     if(devices[i] != pin)
     {
       digitalWrite(devices[i], DISABLE);
     }
   }
   digitalWrite(pin, ENABLE);
}





void setup(){
  Serial.begin(9600);
  Serial.print("Message size ");
  Serial.println(sizeof(Message));
  
  pinMode(10, OUTPUT);
  pinMode(5, OUTPUT);
  
  enableSPIDevice(nrf);
  
  sensor.config();
  
  enableSPIDevice(tftDevice);
  
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  
  enableSPIDevice(lan);
  
   if (ether.begin(sizeof Ethernet::buffer, mymac,5) == 0)
   {
    Serial.println( "Failed to access Ethernet controller");
   }
  #if STATIC
    ether.staticSetup(myip, gwip);
  #else
    if (!ether.dhcpSetup())
    {
      Serial.println("DHCP failed");
    }
  #endif

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
  
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
  
  enableSPIDevice(nrf);
}



void printMessage(Message message)
{
  Serial.print("Message from: ");
  Serial.print((char)message.addr[0]); 
  Serial.print((char)message.addr[1]); 
  Serial.print((char)message.addr[2]); 
  Serial.print((char)message.addr[3]); 
  Serial.print((char)message.addr[4]);
  Serial.print(" type: ");
  Serial.print(message.type);
  Serial.print(" ");
  Serial.println((char *) message.payload);
  
  

  enableSPIDevice(tftDevice);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  tft.setTextSize(1);
  tft.setTextWrap(true); 
  
  //drawLine(x_start,y_start,x_ende,y_ende,farbe);
  tft.drawLine(20,50,98,50,ST7735_GREEN);
  // drawRect(x_oben_links,y_oben_links,weite,hoehe,farbe);
  //tft.fillRect(20,20,140,20,ST7735_BLACK);
  tft.setCursor(20,20);
  tft.setTextColor(ST7735_WHITE);
  tft.print((char *) message.payload);
  enableSPIDevice(nrf);
}


void loop()
{
  Message message;

  message = sensor.receive();
  if(message.type == 0)
  {
  //     Serial.println("No message"); 
  //     Serial.println((char *)message.payload); 
  //     Serial.println((char *)message.addr); 
  }
  else
  {
     
     printMessage(message);  
     if(message.type == 2)
     {
         if(strcmp("motion/1", (char *) message.payload) == 0)
         {
            //digitalWrite(led, HIGH);
         }
         if(strcmp("motion/0", (char *) message.payload) == 0)
         {
            //digitalWrite(led, LOW);
         }
     }
  }

  //delay(1000);
   enableSPIDevice(lan);
   if (ether.packetLoop(ether.packetReceive())) {
      memcpy_P(ether.tcpOffset(), page, sizeof page);
      ether.httpServerReply(sizeof page - 1);
      
      enableSPIDevice(tftDevice);
      tft.setTextSize(1);
      tft.setTextWrap(true); 
      tft.setCursor(20,70);
      tft.setTextColor(ST7735_WHITE);
      tft.print("HTTP REQUEST");  
  }
  
   if(message.type == 3)
   {
      byte sd = stash.create();
  stash.print("temperatura,");
  stash.print((char)message.payload[2]);
  stash.print((char)message.payload[3]); 
  stash.print((char)message.payload[4]);
  stash.print((char)message.payload[5]); 
  stash.println((char)message.payload[6]);   
  
  
  stash.print("wilgotnosc,");
  stash.print((char)message.payload[10]);
  stash.print((char)message.payload[11]); 
  stash.print((char)message.payload[12]);
  stash.print((char)message.payload[13]); 
  stash.println((char)message.payload[14]); 

      stash.save();
    
     Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
                        "Host: $F" "\r\n"
                        "X-PachubeApiKey: $F" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "\r\n"
                        "$H"),
            website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    ether.tcpSend();
   }
  
  enableSPIDevice(nrf);
}

