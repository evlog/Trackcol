/*
  ESP32 eeprom_class example with EEPROM library
  This simple example demonstrates using EEPROM library to store different data in
  ESP32 Flash memory in a multiple user-defined EEPROM class objects.
  
  Created for arduino-esp32 on 25 Dec, 2017
  by Elochukwu Ifediora (fedy0)
  converted to nvs by lbernstone - 06/22/2019
*/

#include "EEPROM.h"

// Instantiate eeprom objects with parameter/argument names and sizes
EEPROMClass  TEST0("eeprom0", 100);
EEPROMClass  TEST1("eeprom1", 100);
EEPROMClass  TEST2("eeprom2", 100);
EEPROMClass  TEST3("eeprom3", 100);
EEPROMClass  TEST4("eeprom4", 100);

void setup() {
  Serial.begin(9600);
  Serial.println("Testing EEPROMClass\n");

  if (!TEST0.begin(TEST0.length())) {
    Serial.println("Failed to initialise TEST0");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!TEST1.begin(TEST1.length())) {
    Serial.println("Failed to initialise TEST1");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!TEST2.begin(TEST2.length())) {
    Serial.println("Failed to initialise TEST2");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!TEST3.begin(TEST3.length())) {
    Serial.println("Failed to initialise TEST3");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!TEST4.begin(TEST4.length())) {
    Serial.println("Failed to initialise TEST4");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }


  uint32_t i;
  
  TEST0.write(99, 30);
  TEST1.write(99, 31);
  TEST2.write(99, 32);
  TEST3.write(99, 33);
  TEST4.write(99, 34);

  Serial.println(byte(TEST0.read(99)));
  Serial.println(byte(TEST1.read(99)));
  Serial.println(byte(TEST2.read(99)));
  Serial.println(byte(TEST3.read(99)));
  Serial.println(byte(TEST4.read(99)));

}

void loop() {
  delay(0xFFFFFFFF);
}
