/**
  Trackcol ModelA
  Name: ModelASensorsRTCTest
  Purpose: ESP32 plus sensors and RTC test code

  @author E. Logaras
  @version 1.0 25/09/2019
*/

// Define libraries
//-----------------------------
#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library
#include "FS.h"                       // File system library
#include "SPIFFS.h"                   // File system library
//-----------------------------
//-----------------------------

// Define constants and global variables
//-----------------------------
#define FORMAT_SPIFFS_IF_FAILED true
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION
int ADXL345_INTERRUPT_PIN = 4;                 // Setup pin 2 to be the interrupt pin (for most Arduino Boards)

RTC_DATA_ATTR int bootCount = 0;

//-----------------------------

//Auxiliary Functions
//-----------------------------
// Look for ADXL345 interrupts and Triggered Action 
void adxlIsr() {
  
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
  
  // Free Fall Detection
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL)){
    Serial.println("*** FREE FALL ***");
    //add code here to do when free fall is sensed
  } 
  
  // Inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    Serial.println("*** INACTIVITY ***");
     //add code here to do when inactivity is sensed
  }
  
  // Activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("*** ACTIVITY ***"); 
     //add code here to do when activity is sensed
  }
  
  // Double Tap Detection
  if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
    Serial.println("*** DOUBLE TAP ***");
     //add code here to do when a 2X tap is sensed
  }
  
  // Tap Detection
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("*** TAP ***");
     //add code here to do when a tap is sensed
  } 
}

// Configure ADXL345 sensor
void adxl345Config() {
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(16);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity

  adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                                      // Default: Set to 1
                                      // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library 
   
  adxl.setActivityXYZ(0, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  adxl.setInactivityXYZ(0, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(1, 1, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(255);           // 62.5 mg per increment
  adxl.setTapDuration(255);            // 625 μs per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment
 
  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment
 
  // Setting all interupts to take place on INT1 pin
  adxl.setImportantInterruptMapping(1, 0, 0, 0, 0);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                                                        // This library may have a problem using INT2 pin. Default to INT1 pin.
  
  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(0);
  adxl.ActivityINT(0);
  adxl.FreeFallINT(0);
  adxl.doubleTapINT(0);
  adxl.singleTapINT(1);

  pinMode(ADXL345_INTERRUPT_PIN, OUTPUT);
  attachInterrupt(ADXL345_INTERRUPT_PIN, adxlIsr, RISING);   // Attach Interrupt  
}

// Calculate roll angle
float calculateRollAngle (int xAccel, int yAccel, int zAccel) {
  float rollAngle;

  rollAngle = atan((float)yAccel / sqrt(pow((float)xAccel, 2) + pow((float)zAccel, 2))) * 180 / PI;

  return rollAngle;
}

// Calculate pitch angle
float calculatePitchAngle (int xAccel, int yAccel, int zAccel) {
  float pitchAngle;

  pitchAngle = atan(-1 * (float)xAccel / sqrt(pow((float)yAccel, 2) + pow((float)zAccel, 2))) * 180 / PI;

  return pitchAngle;
}

// List files in a directory
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        if (String(file.name()).indexOf("hello.txt") != -1)
          Serial.println("File exists");
        else
          Serial.println("File does not exist");
        file = root.openNextFile();
    }
}

// Open and read a file
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
}

// Open and write to file
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- frite failed");
    }
}

// Append data to existing file
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
}

// Rename a file
void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

// Delete a file
void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}


//-----------------------------
//-----------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                 // Start the serial terminal
  Serial.println("SparkFun ADXL345 Accelerometer Hook Up Guide Example");
  Serial.println();

  // File system initialization
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }
   
  // Initialize the ADXL345 sensor 
  adxl345Config();

  // Set our ESP32 to wake up every TIME_TO_SLEEP * uS_TO_S_FACTOR seconds
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  listDir(SPIFFS, "/", 0);

  readFile(SPIFFS, "/hello.txt");

  Serial.println( "Test complete" );
}

void loop() {
  // put your main code here, to run repeatedly:
  int xAccel, yAccel, zAccel;
  char measChar[8];
  float pitchAngle, rollAngle;
 
   //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  
  // Read from ADXL345 accelerometer
  adxl.readAccel(&xAccel, &yAccel, &zAccel);

  // Calculate roll angle
  rollAngle = calculateRollAngle(xAccel, yAccel, zAccel);
  Serial.print(rollAngle);
  Serial.print("/");
  
  // Calculate pitch angle
  pitchAngle = calculatePitchAngle(xAccel, yAccel, zAccel);  
  Serial.println(pitchAngle);

  Serial.println(zAccel);

  adxlIsr();

  // Write measurements to file
  dtostrf(rollAngle, 3, 2, measChar);
  appendFile(SPIFFS, "/hello.txt", measChar);
  appendFile(SPIFFS, "/hello.txt", ", ");
  dtostrf(pitchAngle, 3, 2, measChar);
  appendFile(SPIFFS, "/hello.txt", measChar);
  appendFile(SPIFFS, "/hello.txt", "\r\n");

  Serial.println("Going to sleep now");
  delay(3000);
  Serial.flush(); 
  esp_deep_sleep_start();

}
