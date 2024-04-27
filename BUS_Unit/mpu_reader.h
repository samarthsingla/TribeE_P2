#include <Wire.h>
#include <math.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_sensor.h> 

#include "espnow_peer.h"


const int LED = 2; 
const int OOS_BUTTON = 0;

const int MPU_BUTTON = 5;//

Adafruit_MPU6050 mpu;
bool mpu_found = false; 
float arrNumbers[40] = {0};
int pos = 0;
float newAvg = 0;
float sum = 0;
int len = 40;
long lastTime = millis();
double threshold = 0.3;

bool STOPPED = 0;


float movingAvg(float *ptrArrNumbers, float *ptrSum, int pos, int len, float nextNum){
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}

void setup_mpu(){
  //initialize GPIO0 button for INPUT
  // pinMode(MPU_BUTTON, INPUT);
  pinMode(OOS_BUTTON, INPUT);

  Serial.println("Finding MPU6050 Chip");
  if (!mpu.begin()) {
    mpu_found = false;
    Serial.println("Failed to find MPU6050 chip");
  }
  else{
    mpu_found = true;
    Serial.println("MPU6050 Found!");
      mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
    }
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    // Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      // Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      // Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      // Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      // Serial.println("+- 2000 deg/s");
      break;
    }

    mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
    }   
  }


  
}
// float clip(float val){
//   return min(val, 2);
// }

void mpu_drax(){
  
  int oos_button_state = digitalRead(OOS_BUTTON);
  if (oos_button_state == HIGH){
    OOS = true; 
  }


  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //taking the net accel (g) and then subtracting from result
  float net = pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2);
  net = abs(pow(net,0.5) - 10);
  newAvg = movingAvg(arrNumbers, &sum, pos, len, net);
  pos++;
  if (pos >= len){
    pos = 0;
  }
  long prevchecktime = 0;
  int check = 0;
  prevchecktime = millis ();  
  Serial.println(newAvg);
  if (newAvg <= threshold){
    digitalWrite(LED, HIGH);
    STOPPED = 1;
    state = 1;
    
  }
  else{
    digitalWrite(LED, LOW);
    STOPPED = 0; 
    state = 0; 
    prevchecktime = millis ();
  }
  delay(100);
}
