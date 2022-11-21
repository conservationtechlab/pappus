//For the Arduino on the Beckman center patio

#include <LoRa.h>
#include <SD.h>
#include <SPI.h>
#include <SDLogger.h>
#include <Pappus.h>


long microsStart = 0;
SDLogger logger = SDLogger(4);

void setup() {
  // put your setup code here, to run once:
  if (!logger.beginLogFile("LOG")){
    digitalWrite(LED_BUILTIN, HIGH);
  }
  logger.log("Start!");

  if (!LoRa.begin(915E6)) {
    while(1);
  }

  if (microsStart == 0){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    microsStart = micros();
  }
}

void loop() {
  
  if (packetAvailable()) {
    // read packet
    String packet = readFullPacket();
    byte header[4];
    getPacketHeader(packet, header);

    String log_data = "Received packet from (" + String(header[0]);
    log_data += "), Addressed to (" + String(header[1]) + ")";
    log_data += ", Message ID (" + String(header[2]) + ")";
    log_data += ", RSSI (" + String(LoRa.packetRssi()) + ")";  // not prebuilt function in pappus yet
    log_data += ", Time (" + readableDateTime() + ")";
    log_data += "\n\t Message: '" + getPacketMessage(packet) + "'";
    
    digitalWrite(LED_BUILTIN, HIGH);
    logger.log(log_data);
    digitalWrite(LED_BUILTIN, LOW);
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
