//STOP UNIT CODE
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include "ble_scan.h"

//COMMON VARS FOR ALL PEERS (STOPS AND BUSES):
const int BUS = 0;

const int STOPA = 0;
const int STOPB = 1;

const int SENDER_BUS = 0;
const int SENDER_STOP = 1; 

uint8_t stop_addr[][6] = {
  {0xEC, 0x64, 0xC9, 0x82, 0x95, 0x6C},
  {0xEC, 0x64, 0xC9, 0x85, 0x72, 0x8C},//replace with stop B
};

uint8_t bus_addr[] = {0xEC, 0x64, 0xC9, 0x86,0xE3,0x2C};


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

struct_message out_msg;
struct_message in_msg;

// COMMON VARS: END


//STOP SPECIFIC VARS
int time_to_me[] = {45*1000, 90*1000};
int state = 0;

int SELF = STOPB; //change for burning on both boards
int OTHER = STOPA; 

int eta; //millis
int default_stoppage = 60*1000;

bool OOS = false;
//STOP VARS END

esp_now_peer_info_t peerInfo;

void panic(String err_msg){
  Serial.println("PANIC");
  Serial.println(err_msg);
  exit(2);
}
// Callback when data is received

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if(status != ESP_NOW_SEND_SUCCESS){
    "Delivery of previous packet failed";
  }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&in_msg, incomingData, sizeof(in_msg));

  //LOGGING
  Serial.print("MESSAGE FROM: ");
  Serial.print(in_msg.sender_type);
  Serial.print(" ID: "); Serial.print(in_msg.stop_id);
  
  //unpack bus vars
  bool mpu_stopped = in_msg.mpu_stopped;
  int curr_target_id = in_msg.curr_target_id;
  OOS = in_msg.OOS;


  //unpack stop vars
  int stop_id = in_msg.stop_id;
  int other_state = in_msg.my_state;

  if(in_msg.sender_type == SENDER_BUS){
    Serial.printf("MPU: %d  CURR_TARGET_ID: %d  OOS: %d", mpu_stopped, curr_target_id, OOS);
  }
  else{
    Serial.printf("STOP_ID: %d  OTHER_STATE: %d", stop_id, other_state);
  }
  //LOGGING END

  if(state = 0){
    if(in_msg.sender_type == SENDER_BUS){
      //bus msg received
      if (mpu_stopped){
        //start ble scan to detect proximity
        int startTime = millis();
        startScan();
        int timeElapsed = millis()-startTime;
        Serial.print("Scan time : ");
        Serial.println(timeElapsed);
        bool in_range = checkBeaconInRange(beaconAddress);
        if(in_range){
          state = 1; 
        }
      }
    }
    else{
      //stop message received
    }
  }
  else if(state = 1){
    if(in_msg.sender_type == SENDER_BUS){
      //bus msg received
      if (!mpu_stopped){
        state = 2; 
      }
    }
  }
  else{
    if(in_msg.sender_type == SENDER_STOP){
      //bus msg received
      if(other_state == 2){
        state = 0; 
        eta = time_to_me[SELF];
      }
    }
  }


}


void setup_espnow(){
  Serial.println("Setting up ESP NOW");
  Serial.flush();
  if(SELF == STOPA){
    state = 0; 
    eta = time_to_me[SELF] + 30*1000;
  }
  else {
    state = 2; 
    eta = time_to_me[SELF] + time_to_me[OTHER] + default_stoppage + 30*1000;
  }


  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Register "Was the data delivered successfully" callback
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_peer_info_t peerInfo;

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA; 

  // Register peer 1 
  memcpy(peerInfo.peer_addr, stop_addr[OTHER], 6);
  //print the error
  int resp = esp_now_add_peer(&peerInfo);
  if (resp != ESP_OK){
    Serial.println("Failed to add peer 1");
    Serial.println(resp);

    return;
  }

  //Register peer 2
  memcpy(peerInfo.peer_addr, bus_addr, 6);
  resp = esp_now_add_peer(&peerInfo);
  if (resp != ESP_OK){
    Serial.println("Failed to add peer 2");
    Serial.println(resp);
    return;
  }

  Serial.println("ESP-NOW Setup Done");
}


void prep_message(){
  out_msg.stop_id = SELF;
  out_msg.my_state = state;
  if(state == 2){
    out_msg.change_target = 1;
    out_msg.next_target_id = OTHER; 
  }
}

void send_message(){
  // Send message via ESP-NOW
  prep_message(); 

  esp_err_t result_stop = esp_now_send(stop_addr[OTHER], (uint8_t *) &out_msg, sizeof(out_msg));
  esp_err_t result_bus = esp_now_send(bus_addr, (uint8_t *) &out_msg, sizeof(out_msg));
  
  if(result_stop != ESP_OK){
    Serial.println("Msg not SENT to stop");
  }
  if(result_bus != ESP_OK){
    Serial.println("Msg not SENT to bus"); 
  }

}




