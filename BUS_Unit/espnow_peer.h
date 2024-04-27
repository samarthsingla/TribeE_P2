//BUS UNIT CODE
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include "bt_beacon.h"

//BT BEACON INCLUDES
// #include "mpu_reader.h"

//COMMON VARS FOR ALL PEERS (STOPS AND BUSES):
const int BUS = 0;

const int STOPA = 0;
const int STOPB = 1;

const int SENDER_BUS = 0;
const int SENDER_STOP = 1; 

uint8_t stop_addr[][6] = {
  {0xEC, 0x64, 0xC9, 0x82, 0x95, 0x6C},
  {0xEC, 0x64, 0xC9, 0x85, 0x72, 0x8C}, //replace with stop B
};

uint8_t bus_addr[] = {0xEC, 0x64, 0xC9, 0x86,0xE3,0x2C};


    //BT VARS
String success;
typedef struct struct_message {
    int sender_type;

    //stop variables
    int stop_id;
    bool change_target; //boolean
    int next_target_id; //change to this target stop id;
    int my_state;

    bool logging;
    String log;

    //bus variables
    bool mpu_stopped;
    int curr_target_id; 
    bool OOS;


} struct_message;

// Create a struct_message called BME280Readings to hold sensor readings
struct_message out_msg;
// Create a struct_message to hold incoming sensor readings
struct_message in_msg;

// COMMON VARS: END


//BUS VARS
int curr_target_id;
int state = 0;
bool OOS = false; 
//BUS VARS END



void panic(String err_msg){
  Serial.println("PANIC");
  Serial.println(err_msg);
  exit(-1);
}
// Callback when data is received

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  memcpy(&in_msg, incomingData, sizeof(in_msg));
  int sender_type = in_msg.sender_type;
  if(sender_type != SENDER_STOP){
    panic("Message from another bus");
  }
  int stop_id = in_msg.stop_id; bool change_target = in_msg.change_target; int next_target_id = in_msg.next_target_id;

  if(stop_id == curr_target_id){
    if(change_target){ 
      curr_target_id = next_target_id;
    }
  }
}


void setup_espnow(){
  curr_target_id = STOPA;
  state = 0;


  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else{
    Serial.println("ESP-NOW Initialized");
  }

  //Register "Was the data delivered successfully" callback
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_peer_info_t peerInfo;

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA; 

  // Register peer 1 
  memcpy(peerInfo.peer_addr, stop_addr[0], 6);
  //print the error
  int resp = esp_now_add_peer(&peerInfo);
  if (resp != ESP_OK){
    Serial.println("Failed to add peer 1");
    Serial.println(resp);

    return;
  }

  //Register peer 2
  memcpy(peerInfo.peer_addr, stop_addr[1], 6);
  resp = esp_now_add_peer(&peerInfo);
  if (resp != ESP_OK){
    Serial.println("Failed to add peer 2");
    Serial.println(resp);
    return;
  }

  //START BT Advertising
  setup_beacon();
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void prep_message(bool mpu_stopped){
  out_msg.sender_type = SENDER_BUS;
  out_msg.mpu_stopped = mpu_stopped;
  out_msg.curr_target_id = curr_target_id;
  out_msg.OOS = OOS;
}

void send_message(bool mpu_stopped){
  
  // Send message via ESP-NOW
  prep_message(mpu_stopped); 

  esp_err_t result = esp_now_send(stop_addr[curr_target_id], (uint8_t *) &out_msg, sizeof(out_msg));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

}




