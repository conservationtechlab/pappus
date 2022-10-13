/*
 * ReceiveHeaderedLora WriteSD
 * 
 * This node (address 'R' in ascii) receives a LoRa packet, then 
 * writes the message to an SD card if its addressed to itself. It
 * shaves the header off the packet when it writes tothe .
 * 
 * This is a part of a 3-node test, with the three .ino files being
 * "receiveHeaderedLora_WriteSD", "readImu_sendLoraPacket", and 
 * "readImu_sendLoraPacket".  
 * 
 * Peripheral components needed: 915 MHz antenna, Arduino MKR SD 
 * Shield, and microSD card.
 * 
 * By: Trent Moca
 */


#include <SD.h>
#include <SPI.h>
#include <LoRa.h>


#define CHIP_SELECT               4
#define FILE_NAME                 "NR_LOG.txt"

void setup() {
  if (!SD.begin(CHIP_SELECT)) {
    while(1);
  }

    uploadString("New Log:\n");
  

   if (!LoRa.begin(915E6)) {
    while (1);
  }
  
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    digitalWrite(LED_BUILTIN, HIGH);
    
    String packet = readFullPacket();
    
    byte oldHeader[4];
    getPacketHeader(packet, oldHeader);
    if (oldHeader[1] ^ 0x58) {  // check if target isn't 0x58 ('R')
      digitalWrite(LED_BUILTIN, LOW);
      return;
    }

    uploadString(getPacketMessage(packet)+": RSSI = " + LoRa.packetRssi() + "\n");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void uploadString(String data) {
  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    dataFile.print(data);
    dataFile.close();
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
