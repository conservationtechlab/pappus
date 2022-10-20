//For the Arduino on the Beckman center patio

#include <LoRa.h>
#include <SD.h>
#include <SPI.h>

const int chipSelect = 4;

const String FILE_NAME = "loralog.txt";

long microsStart = 0;

void setup() {
  // put your setup code here, to run once:
  if (!SD.begin(chipSelect)){
    while(1);
  }

  if (!LoRa.begin(915E6)) {
    while(1);
  }

  if (microsStart == 0){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    microsStart = micros();
  }

  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    dataFile.println("\nNew Log Start Here:");
    dataFile.close();
  }
  
}

void loop() {
  int packetSize = LoRa.parsePacket();
  String log_data = "Received packet '";
  if (packetSize) {
    // read packet
    while (LoRa.available()) {
      log_data += (char)LoRa.read();
    }
    log_data += "' at time " + readableDateTime();

    // print RSSI of packet
    log_data += " with RSSI " + String(LoRa.packetRssi());

    digitalWrite(LED_BUILTIN, HIGH);
    uploadString(log_data);
    digitalWrite(LED_BUILTIN, LOW);
  }

}

void uploadString(String data) {
  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  }
}


String readableDateTime() {
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
}
