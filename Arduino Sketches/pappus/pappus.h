#ifndef PAPPUS_H
#define PAPPUS_H

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

String readFullPacket();

void getPacketHeader(String packet, byte* header);

String getPacketMessage(String packet);

void sendLoraPacket(byte* header, String message);

String getRSSI();

bool packetAvailable();

String readableTimestamp(long mircosStart);

#endif