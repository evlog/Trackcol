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
EEPROMClass  NAMES("eeprom0", 100);
EEPROMClass  HEIGHT("eeprom1", 100);
EEPROMClass  AGE("eeprom2", 100);
EEPROMClass  TEST0("eeprom3", 100);
EEPROMClass  TEST1("eeprom4", 100);
EEPROMClass  TEST2("eeprom5", 100);
EEPROMClass  TEST3("eeprom5", 100);
EEPROMClass  TEST4("eeprom5", 100);
EEPROMClass  TEST5("eeprom5", 100);

EEPROMClass  FLAG("eeprom6", 4);

void setup() {

  uint32_t flag;



  const char* name = "Teo Swee Ann";
  char rname[32];
  double height = 5.8;
  uint32_t age = 47;
  uint32_t test0 = 32;
  uint32_t test1 = 33;
  uint32_t test2 = 34;
   uint32_t test3 = 35;

  byte val = 45;


  
  

  Serial.println("Testing EEPROMClass\n");
  if (!NAMES.begin(NAMES.length())) {
    Serial.println("Failed to initialise NAMES");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!HEIGHT.begin(HEIGHT.length())) {
    Serial.println("Failed to initialise HEIGHT");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!AGE.begin(AGE.length())) {
    Serial.println("Failed to initialise AGE");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
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
  if (!TEST5.begin(TEST5.length())) {
    Serial.println("Failed to initialise TEST5");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!FLAG.begin(FLAG.length())) {
    Serial.println("Failed to initialise FLAG");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  Serial.begin(115200);
  TEST2.get(4, test2);
   Serial.print("test2: ");
  Serial.println(test2);
  if (test2 != 35) {
    Serial.println("Memory init");

    test2 = 35;
  
  // Write: Variables ---> EEPROM stores
  NAMES.put(0, name);
  HEIGHT.put(0, height);
  AGE.put(0, age);
  TEST0.put(0, test0);
  TEST1.put(0, test1);
  TEST2.put(0, test2);
  TEST2.put(4, test3);
  Serial.print("name: ");   Serial.println(name);
  Serial.print("height: "); Serial.println(height);
  Serial.print("age: ");    Serial.println(age);
  Serial.println("------------------------------------\n");

  // Clear variables
  name = '\0';
  height = 0;
  age = 0;
  Serial.print("name: ");   Serial.println(name);
  Serial.print("height: "); Serial.println(height);
  Serial.print("age: ");    Serial.println(age);
  Serial.println("------------------------------------\n");
  }
  // Read: Variables <--- EEPROM stores
  NAMES.get(0, rname);
  HEIGHT.get(0, height);
  AGE.get(0, age);
  TEST0.get(0, test0);
  TEST1.get(0, test1);
  TEST2.get(0, test2);
  Serial.print("name: ");   Serial.println(rname);
  Serial.print("height: "); Serial.println(height);
  Serial.print("age: ");    Serial.println(age);
  Serial.print("test0: ");    Serial.println(test0);
  Serial.print("test1: ");    Serial.println(test1);
  Serial.print("test2: ");    Serial.println(test2);

  TEST2.get(4, test2);
  Serial.print("test2: ");    Serial.println(test2);  
  Serial.println("Done!");

  //TEST5.write(999, val);

  //Serial.println(byte(TEST5.read(999)));
  flag = 33;
  FLAG.put(0, flag);

  FLAG.get(0, flag);
   Serial.print("flag: ");
  Serial.println(flag);
}

void loop() {
  delay(0xFFFFFFFF);
}
