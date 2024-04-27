#include "espnow_peer.h"

#include "dispLib.h"

int LED = 2;
int lastTime;
void setup() {

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  setup_espnow();
  // setup_arduino_cloud();
  Serial.flush();
  setup_ble();
  setup_disp();

  lastTime = millis();
}


void loop() {
  // Your code here 
  int timeElapsed = millis()-lastTime;
  lastTime = millis();

  eta = max(eta - timeElapsed, 0);

  Serial.printf("ETA: %d", eta/1000);
  //Try updating synced variables
  // Serial.println("Updated bus_start");

  send_message();
  displayTime((int) eta/60000);

  delay(2000); //2 sec delay


  if(OOS){
    Serial.println("OOS State");
    OOS_Scroll(1, 8500*10);
  }
}


