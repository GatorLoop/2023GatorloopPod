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

uint32_t bufferSize = 0;
uint32_t millis_t = 0;
uint64_t tick = 0;

uint32_t LastHeartbeat = millis();

String Command = "";
uint16_t CommandLen = 0;

void setup() {

  // USB Debug
  Serial.begin(9600);
  while (!Serial);

  // HC-12 Serial
  Serial2.begin(9600);
  while (!Serial2);

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

void loop() {
  millis_t = millis();

  while (Serial2.available()) {    // If HC-12 has data
    Command += (char)Serial2.read();  // Send the data to Serial monitor
    CommandLen++;
  }

  String dataString = "";
  dataString += String(millis());
  dataString += " : ";
  dataString += State;
  dataString += " : ";
  dataString += bufferSize;

  if (CommandLen && Command[CommandLen-1] == '\n') { // process command if done
    if (CommandLen > 1 && Command.substring(0,1) == "H") {
      if (State == STANDBY) {
        State = CONNECTED;
        LastHeartbeat = millis_t;
      } else if (State != ERROR && State != BATTERY_ERROR) {
        LastHeartbeat = millis_t;
      }
    }
    if (State == CONNECTED && CommandLen > 2 && Command.substring(0,2) == "Go") {
      State = ACTIVE;
    }
    if (State == ACTIVE && CommandLen > 4 && Command.substring(0,4) == "Stop") {
      State = CONNECTED;
    }
    if (State == ERROR && CommandLen > 7 && Command.substring(0,7) == "Recover") {
      State = STANDBY;
    }
    uint16_t end_index = Command.indexOf('\n') + 1;
    Command = Command.substring(end_index,CommandLen);
    CommandLen -= end_index;
  }

  switch (State) {
    case STANDBY: // Brakes, Blinking Yellow

      if (millis_t%1000 <= 500){
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

      if (millis_t > LastHeartbeat + 2000) { // 2s since last contact
        State = ERROR;
      }
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, HIGH);
      break;
    case ACTIVE: // Control, Green

      if (millis_t > LastHeartbeat + 2000) { // 2s since last contact
        State = ERROR;
      }
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

      if (millis_t%1000 <= 500){
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
  // Serial.println(dataString);

  bufferSize += dataString.length();
  if (bufferSize > 2048) {
    bufferSize = 0;
    dataFile.flush();
  }
  tick++;
}
