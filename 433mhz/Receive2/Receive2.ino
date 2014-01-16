/*
SimpleReceive
This sketch displays text strings received using VirtualWire
Connect the Receiver data pin to Arduino pin 11
*/
#include <VirtualWire.h>
byte message[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming messages
byte messageLength = VW_MAX_MESSAGE_LEN; // the size of the message
void setup()
{
Serial.begin(9600);
Serial.println("Device is ready");
// Initialize the IO and ISR
vw_setup(2000); // Bits per sec
vw_rx_start(); // Start the receiver
}
void loop()
{
if (vw_get_message(message, &messageLength)) // Non-blocking
{
Serial.print("Received: ");
for (int i = 0; i < messageLength; i++)
{
Serial.write(message[i]);
}
Serial.println();
}
}
