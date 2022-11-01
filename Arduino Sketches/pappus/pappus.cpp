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

String readableDateTime(long microsStart) {
  double totalSeconds = (micros() - microsStart)/1000000.0;
  int totalMinutes = floor(totalSeconds)/60;
  int totalHours = floor(totalSeconds)/3600;
  int totalDays = 1 + floor(totalSeconds)/86400;
  double secondsDouble = totalSeconds - totalMinutes*60;
  int seconds = floor(secondsDouble);
  int minutes = totalMinutes - totalHours*60;
  int hours = totalHours - totalDays*24;
  String yearMonth = " 1970-01-";
  char dateTimeCharArray[19];
  int deciseconds = round((secondsDouble - seconds)*1000000);
  sprintf(dateTimeCharArray, "%02d %02d:%02d:%02d.%06d", totalDays, hours, minutes, seconds, deciseconds);
  
  return yearMonth + String(dateTimeCharArray);
