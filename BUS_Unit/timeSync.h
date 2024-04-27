#include <WiFi.h>
#include "time.h"
#include <NTPClient.h>

const char* ssid       = "ELP305_T3";
const char* password   = "Qwerty@123";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
int LED_BUILTIN = 2;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
}

String getTimeString(){
    String timeStr;
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Time BT");
    }
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    return timeString;
}


void feedback(){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);

    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);

}

void get_time()
{  
  //connect to WiFi

    Serial.printf("Getting Time from Server \nConnecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED");

    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
    feedback();
    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

// void loop()
// {
//   if ( WiFi.status() != WL_CONNECTED){
//       digitalWrite(LED_BUILTIN, HIGH);
//   }
//   else {
//       digitalWrite(LED_BUILTIN, LOW);
//   }
//   delay(1000);
//   printLocalTime();
// }