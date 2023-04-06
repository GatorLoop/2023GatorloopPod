#pragma once
#include <SPI.h>
#include <SD.h>
#include "openNextFile.h"

#define LED_R 30
#define LED_G 31
#define LED_B 32

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

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

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
uint32_t milis = 0;
uint64_t tick = 0;

void loop() {
  milis = millis();

  String dataString = "";
  dataString += String(millis());
  dataString += ":";
  dataString += State;
  dataString += ":";
  dataString += bufferSize;

  switch (State) {
    case STANDBY: // Brakes, Blinking Yellow

      if (milis%1000 <= 500){
        // Yellow
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, LOW);
        digitalWrite(LED_B, HIGH);
      } else {
        // Off
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, HIGH);
        digitalWrite(LED_B, HIGH);
      }
      break;
    case CONNECTED: // Brakes, Solid Yellow
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, HIGH);
      break;
    case ACTIVE: // Control, Green
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, HIGH);
      break;
    case ERROR: // Brakes, Solid Red
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, HIGH);
      break;
    case BATTERY_ERROR: // Disengage, Blinking Red

      if (milis%1000 <= 500){
        // Red
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_G, HIGH);
        digitalWrite(LED_B, HIGH);
      } else {
        // Off
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, HIGH);
        digitalWrite(LED_B, HIGH);
      }
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
  tick++;
}
