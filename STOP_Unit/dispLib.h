#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
// #define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

#define MAX_DEVICES 1
#define CS_PIN 5


// unsigned long startTime;
// unsigned long duration = 15000;
MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup_disp() {
  // startTime = millis();
  Serial.println("Setting Up Display");
  Display.begin();
  Display.setIntensity(0);
  Display.displayClear();
  Serial.println("Display SetUp Done");
}

void OOS_Scroll(int isOn, int dur) {
  Display.displayScroll("Out of Service!", PA_CENTER, PA_SCROLL_LEFT, 100);
  Serial.println("Is called");
  unsigned long startTime = millis();
  Display.displayClear();
  while (isOn) {
    if (millis() - startTime >= dur) {
      // Exit the loop after the specified duration
      Display.displayClear();
      delay(500);
      return;
    }
    if (Display.displayAnimate()) {
      Display.displayReset();
    }
  }
}

void starttimer(int waitTime) {
  Display.setTextAlignment(PA_CENTER);

  Display.print("eta");
  delay(1000);

  Display.displayClear();
  for (int i_sec = waitTime; i_sec >= 0; i_sec--) {
    int i = i_sec%60;
    Display.setTextAlignment(PA_CENTER);  // Assuming Display is an object with setTextAlignment and print methods
    if (i<10) {
    Display.print("0"+String(i));
    } else {
      Display.print(String(i));
    }              
    delay(1000);                          // Delay for 1 second
    Display.displayClear();               // Clear the display after printing
  }
  Display.displayReset();
  return;
}

void displayTime(int waitTime){ //minutes
  Display.displayClear();
  Display.setTextAlignment(PA_CENTER);
  if (waitTime<10) {
    Display.print("0"+String(waitTime));
  } 
  else if (waitTime>99) {
    Display.print("99");
  } 
  else {
    Display.print(String(waitTime));
  }
}
