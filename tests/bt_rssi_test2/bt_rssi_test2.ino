/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 1; //In seconds
int rssi0, rssi1, rssi;
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    char btMessage[50];
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //sprintf(btMessage, "%s", advertisedDevice.toString().c_str());
      String m = advertisedDevice.toString().c_str();
    //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString());
      //Serial.println(m);
      //delay(1);
      //Serial.print(" RSSI: ");
      //Serial.println(advertisedDevice.getRSSI());
      //delay(1);
      //sprintf(btMessage, "%s", advertisedDevice.toString().c_str());
      //String m((char*)btMessage);
      //delay(1);
      //Serial.printf("Advertised Device mod: %s \n",m);
      if(m.indexOf("2157a") >= 0) {
        rssi = advertisedDevice.getRSSI();
        Serial.println(rssi);
        
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() { 
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");

  rssi0 = rssi;

  foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");

  rssi1 = rssi;
  Serial.print("av:");
  Serial.println((rssi0+rssi1) / 2);
  
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(1);
}
