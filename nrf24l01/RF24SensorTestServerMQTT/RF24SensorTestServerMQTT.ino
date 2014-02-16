#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <NanodeMQTT.h>

//---------------------------------------------------
#include <MemoryFree.h>
//#include <Adafruit_ST7735.h>
//#include <Adafruit_GFX.h>

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "RF24Sensor.h"


#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset

#define ENABLE LOW
#define DISABLE HIGH

#define nrf 3
#define tftDevice 10
#define lan 5
#define devicesSize 3

RF24Sensor sensor(4,3, (byte *)"serv1");

//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

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
  
    Serial.print("before freeMemory()=");
  Serial.println(freeMemory());

  //enableSPIDevice(tftDevice);
  //tft.fillScreen(ST7735_BLACK);
//  tft.setRotation(3);
//  tft.setTextSize(1);
//  tft.setTextWrap(true); 
//  
//  //drawLine(x_start,y_start,x_ende,y_ende,farbe);
//  tft.drawLine(20,50,98,50,ST7735_GREEN);
//  // drawRect(x_oben_links,y_oben_links,weite,hoehe,farbe);
//  //tft.fillRect(20,20,140,20,ST7735_BLACK);
//  tft.setCursor(20,20);
//  tft.setTextColor(ST7735_WHITE);
//  tft.print((char *) message.payload);
//  enableSPIDevice(nrf);


    Serial.print("after freeMemory()=");
  Serial.println(freeMemory());

}




//---------------------------------------------------

NanodeMQTT mqtt(&uip);



void setup() {
  
  Serial.begin(9600);
  Serial.print("Message size ");
  Serial.println(sizeof(Message));
  
  pinMode(10, OUTPUT);
  pinMode(5, OUTPUT);
  
  enableSPIDevice(nrf);
  
  sensor.config();
  
  enableSPIDevice(tftDevice);


 // tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
 // tft.fillScreen(ST7735_BLACK);
  
  enableSPIDevice(lan);
  
  byte macaddr[6];
  NanodeUNIO unio(NANODE_MAC_DEVICE);

  Serial.println("MQTT Publish test");
  
  unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
  uip.init(macaddr,5);

  // FIXME: use DHCP instead
  uip.set_ip_addr(10, 89, 9, 88);
  uip.set_netmask(255, 255, 255, 0);

  uip.wait_for_link();
  Serial.println("Link is up");

  // Setup a timer - publish every 5 seconds
  //timer_set(&my_timer, CLOCK_SECOND * 5);

  // FIXME: resolve using DNS instead
  mqtt.set_server_addr(10, 89, 9, 128);
  mqtt.connect();

  Serial.println("setup() done");
  enableSPIDevice(nrf);
}

void loop() {
  
  Message message;

  message = sensor.receive();
  
 
  
  if(message.type == 0)
  {
   //   Serial.println("No message"); 
  //     Serial.println((char *)message.payload); 
  //     Serial.println((char *)message.addr); 
  }
  else
  {
     
     printMessage(message); 
   
//     enableSPIDevice(lan);
//     //uip.poll();
//    if (mqtt.connected()) {
//      Serial.println("Publishing...");
//      mqtt.publish("test", (char *) message.payload);
//      Serial.println("Published.");
//    }
//    else
//    {
//      Serial.println("mqtt not connected.");
//        mqtt.connect();
//         mqtt.publish("test", (char *) message.payload);
//        Serial.println("Published again.");
//    }
    
  }

  //delay(1000);
//   enableSPIDevice(lan);
//   if (ether.packetLoop(ether.packetReceive())) {
//      memcpy_P(ether.tcpOffset(), page, sizeof page);
//      ether.httpServerReply(sizeof page - 1);
//      
//      enableSPIDevice(tftDevice);
//      tft.setTextSize(1);
//      tft.setTextWrap(true); 
//      tft.setCursor(20,70);
//      tft.setTextColor(ST7735_WHITE);
//      tft.print("HTTP REQUEST");
//  }

  enableSPIDevice(lan);
  uip.poll();
 if(message.type > 0)
  {
if (mqtt.connected()) {
      Serial.println("Publishing...");
      mqtt.publish("test", (char *)message.payload);
      Serial.println("Published.");
    }
    else
    {
      Serial.println("Not connected...");
      mqtt.connect();
      mqtt.publish("test", (char *)message.payload);
      Serial.println("Published.");
    }
  }
  enableSPIDevice(nrf);
  
//  Serial.print("freeMemory()=");
//  Serial.println(freeMemory());

}
