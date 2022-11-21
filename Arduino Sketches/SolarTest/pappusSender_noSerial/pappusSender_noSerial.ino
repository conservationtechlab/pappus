#include <SPI.h>
#include <LoRa.h>
#include <Pappus.h>

long counter = 0;
byte NODE_ID = 0x01;
byte TARGET = 0x02;


void setup() {
  
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

  String message = "Hello world! Seconds = " + String(counter);

  digitalWrite(LED_BUILTIN, HIGH);
  sendLoraPacket(packetHeader, message);
  counter++;
  digitalWrite(LED_BUILTIN, LOW);


  delay(1000);

}
