#include <SPI.h>
#include <LoRa.h>
#include <pappus.h>

int counter = 0;
byte NODE_ID = 0x01;
byte TARGET = 0x02;


void setup() {
  Serial.begin(9600);
  while(!Serial);

  Serial.println("Pappus Sender");
  
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa failed");
    while (1);
  }
  
}

void loop() {
  byte packetHeader[4];

  packetHeader[0] = NODE_ID;
  packetHeader[1] = TARGET;
  packetHeader[2] = (byte)(counter & 0xFF);
  packetHeader[3] = 0x95;  // underscore for flags bit : temporary value

  String message = "Hello world! #" + String(counter);
  sendLoraPacket(packetHeader, message);

  delay(2000);

}
