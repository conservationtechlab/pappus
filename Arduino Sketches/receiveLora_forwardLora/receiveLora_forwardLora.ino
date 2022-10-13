/*
 * ReceiveLora ForwardLora
 * 
 * This node (address 'X' in ascii) receives a LoRa packet, then 
 * forwards the message to node 'R' after 200 ms if the message is
 * addressed to itself. The node_id and target id are changed from 
 * the header, but the rest of the packet is retained.
 * 
 * This is a part of a 3-node test, with the three .ino files being
 * "receiveHeaderedLora_WriteSD", "readImu_sendLoraPacket", and 
 * "readImu_sendLoraPacket". 
 * 
 * Peripheral components needed: 915 MHz antenna
 * 
 * By: Trent Moca
 */


#include <SPI.h>
#include <LoRa.h>

void setup() {
  if (!LoRa.begin(915E6)) {
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packet = readFullPacket();
    
    byte oldHeader[4];
    getPacketHeader(packet, oldHeader);
    if (oldHeader[1] ^ 0x58) {  // check if target isn't 0x58 ('X')
      return;
    }
    byte newHeader[4];
    newHeader[0] = 0x58;
    newHeader[1] = 0x52;
    newHeader[2] = oldHeader[2];
    newHeader[3] = oldHeader[3];
  
    
  
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    sendLoraPacket(newHeader, getPacketMessage(packet));
  }

  

}

String readFullPacket(){
  String packet = "";
  byte i = 0;
  while (LoRa.available()){
    packet += (char) LoRa.read();
  }
  return String(packet);
}

void getPacketHeader(String packet, byte* header){
  if (packet.length() <= 4){
    return;
  }
  for (byte i=0; i < 4; i++){
    header[i] = packet.charAt(i) & 0xFF;
  }
}

String getPacketMessage(String packet){
  if (packet.length() <= 4){
    return "";
  }
  return packet.substring(4);
}

void sendLoraPacket(byte* header, String message){
  char charHeader[4];
  for (byte i =0; i < 4; i++){
    charHeader[i] = header[i] & 0xFF;
  }
  LoRa.beginPacket();
  LoRa.print(String(charHeader)+message);
  LoRa.endPacket();
}
