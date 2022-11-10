#include <SPI.h>
#include <LoRa.h>
#include <DFRobot_BMX160.h>

int counter = 0;
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

  // send packet
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
