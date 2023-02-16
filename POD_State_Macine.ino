#pragma once
#include <SPI.h>
#include <SD.h>
#include "openNextFile.h"

#define STANDBY 0
#define CONNECTED 1
#define ACTIVE 2
#define ERROR 3
#define BATTERY_ERROR 4

static uint8_t State = STANDBY;

const static uint16_t SDchipSelect = BUILTIN_SDCARD;

File dataFile;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);
  if (!SD.begin(SDchipSelect)) {
    Serial.println("ERORR: Card not found!");
    //TODO hop to error state
    while (1);
  }
  Serial.println("card initialized.");

  openNextFile(dataFile);

  if (!dataFile) {
    Serial.println("error opening file");
    //TODO jump to error state
    while (1);
  }

}

uint32_t bufferSize = 0;

void loop() {
  String dataString = "";
  dataString += String(millis());
  dataString += ":";
  dataString += State;
  dataString += ":";
  dataString += bufferSize;

  switch (State) {
    case STANDBY: // Brakes, Blinking Yellow
      break;
    case CONNECTED: // Brakes, Solid Yellow
      break;
    case ACTIVE: // Control, Green
      break;
    case ERROR: // Brakes, Solid Red
      break;
    case BATTERY_ERROR: // Disengage, Blinking Red
      break;
    default:
      State = ERROR;
  }

  dataFile.println(dataString);
  Serial.println(dataString);

  bufferSize += dataString.length();
  if (bufferSize > 2048) {
    bufferSize = 0;
    dataFile.flush();
  }
}
