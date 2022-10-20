/*
 * ReceiveImu SendLoraPacket
 * 
 * This node (address 'T' in ascii) reads accelerometer data and sends 
 * it over LoRa every two seconds.
 * 
 * This is a part of a 3-node test, with the three .ino files being
 * "receiveHeaderedLora_WriteSD", "readImu_sendLoraPacket", and 
 * "readImu_sendLoraPacket". 
 * 
 * Peripheral components needed: 915 MHz antenna, DFRobotics BMX160 
 * chip connected via I2C
 * 
 * By: Trent Moca
 */
 
#include <SPI.h>
#include <LoRa.h>
#include <DFRobot_BMX160.h>

byte counter = 0;
DFRobot_BMX160 bmx160;

void setup() {
  if (bmx160.begin() != true){
    while(1);
  }

  if (!LoRa.begin(915E6)) {
    while (1);
  }
  
}

void loop() {
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);
  String data = String(counter) + " [x: " + String(Oaccel.x) + ", y: " + String(Oaccel.y) + ", z: " + String(Oaccel.z) + "]";

  byte header[4];
  header[0] = 0x54;  // Node ID= T
  header[1] = 0x58;  // Target= X
  header[2] = counter;  // counter num
  header[3] = 0x5F;  // _

  sendLoraPacket(header, data);
  
  counter++;

  delay(2000);
}



void sendLoraPacket(byte* header, String message){
  char charHeader[4];
  for (byte i =0; i < 4; i++){
    charHeader[i] = header[i] & 0xFF;
  }
  LoRa.beginPacket();
  LoRa.print(String(charHeader));
  LoRa.print(message);
  LoRa.endPacket();
}
