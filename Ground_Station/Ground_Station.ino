#include <SoftwareSerial.h>
#define HC12_TX 10
#define HC12_RX 11
SoftwareSerial HC12(HC12_TX, HC12_RX);
void setup() {
  Serial.begin(9600); // Serial port to computer
  HC12.begin(9600);   // Serial port to HC12
}
void loop() {
  while (HC12.available()) {    // If HC-12 has data
    Serial.write(HC12.read());  // Send the data to Serial monitor
  }
  while (Serial.available()) {  // If Serial monitor has data
    HC12.write(Serial.read());  // Send that data to HC-12
  }
  delay(100);
  HC12.write("H\n"); // send heartbeat
}