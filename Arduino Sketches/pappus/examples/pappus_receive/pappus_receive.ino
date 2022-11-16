#include <SPI.h>
#include <LoRa.h>
#include <Pappus.h>

int counter = 0;
byte NODE_ID = 0x02;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  Serial.println("Pappus Receiver");
  
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa failed");
    while (1);
  }
}

void loop() {
  if (packetAvailable()){
    Serial.println("Packet Received!");
    String packet = readFullPacket();
    byte header[4];
    getPacketHeader(packet, header);
    
    
    if (header[1] != NODE_ID){
      Serial.println("\tAddressed for node: " + String(header[1]));
      return;
    }
    String message = getPacketMessage(packet);
    Serial.println("\tSender: " + String(header[0]) + ", Message: '" + message + "'");
    
  }

}
