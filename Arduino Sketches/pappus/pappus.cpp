#include "pappus.h"

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

String getRSSI() {
    return String(LoRa.packetRssi());
}

bool packetAvailable(){
    if (LoRa.parsePacket()){
        return true;
    }
    return false;
}