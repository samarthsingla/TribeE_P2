#include "mpu_reader.h"

void setup() {

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  setup_espnow();
  // setup_arduino_cloud();

  //currently not using mpu
  setup_mpu();

  Serial.println("Setup Done");
}

void loop() {

  // send_message(6969);
  mpu_drax(); // includes delay of 100ms
  // delay(100);
}

