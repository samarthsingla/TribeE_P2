
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 2; //In seconds
bool beaconInRange = 0;
BLEAddress beaconAddress = BLEAddress("ec:64:c9:86:e3:2e");
int strength;
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.getAddress().equals(beaconAddress)) {
        beaconInRange = 1;
        strength = advertisedDevice.getRSSI();
      }
    }
};

void startScan() {
  beaconInRange = 0;
  Serial.println("Scan Started");
  pBLEScan -> start(scanTime, false);
}

bool checkBeaconInRange(BLEAddress beaconAddress) {
  BLEScanResults foundDevices = pBLEScan->getResults();
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  if (beaconInRange) {
    Serial.println("Beacon Found");
    Serial.println("Strength ");
    Serial.println(strength);
  }
  else Serial.println("Beacon Not Found");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  return beaconInRange;
}

void setup_ble() {
  Serial.println("Setting up BLE \n Scanning...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  Serial.println("BLE Setup Done");
}
