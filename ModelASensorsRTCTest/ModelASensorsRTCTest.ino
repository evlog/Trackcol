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
//#include <ADXL345.h>         // SparkFun ADXL345 Library
#include "FS.h"                       // File system library
#include "SPIFFS.h"                   // File system library
#include "BluetoothSerial.h"          // Bluetooth library
#include <RTClib.h>                   // RTC library
#include <EEPROM.h>                   // EEPROM library
#include "DHTesp.h"                   

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
//-----------------------------
//-----------------------------

// Define constants and global variables
//-----------------------------
#define FORMAT_SPIFFS_IF_FAILED true
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10       /* Time ESP32 will go to sleep (in seconds) */
#define EEPROM_SIZE 2           // define the number of bytes you want to access
boolean ST_DATA_MEAS = false;

boolean eepromWriteFlag = false;

int ADXL345_INTERRUPT_PIN = 4;   
int LED_STATUS_PIN = 23;  
int DHT22_GPIO_PIN = 15;
int PUSH_BUTTON_INTERRUPT_PIN = 0;        
RTC_DATA_ATTR int bootCount = 0;
hw_timer_t * timer = NULL;            // create a hardware timer 

BluetoothSerial SerialBT;
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION
RTC_DS3231 rtc;                       // RTC object
DHTesp dht;                           // DHT sensor



// Time value to be used for delay
unsigned long measPreviousMillis = 0;
unsigned long statusLedPreviousMillis = 0;

// Real time char value
char t[32];

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

// Interrupt service routine to detect push button changes
void pushButtonIsr () {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  detachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_INTERRUPT_PIN));

  

  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    Serial.println("Button pushed");
    ST_DATA_MEAS = !ST_DATA_MEAS;
    Serial.print("ST_DATA_MEAS: ");
    Serial.println(ST_DATA_MEAS);

//  if (ST_DATA_MEAS ==  true) 
 //   setLedTimer(3000000);
 // else if (ST_DATA_MEAS ==  false) 
//    setLedTimer(10000000);
  
  //    delay(1000);
  //    last_interrupt_time = interrupt_time;
   //   loop();
    //}

  }
  last_interrupt_time = interrupt_time; 

  eepromWriteFlag = true;

  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_INTERRUPT_PIN), pushButtonIsr, FALLING);   // Attach Interrupt 

}

void saveState() {
  // Save STATE value to EEPROM
  //---
  if (ST_DATA_MEAS == false) {
    EEPROM.write(0, 0);
    EEPROM.commit();
  }
  else if (ST_DATA_MEAS == true) {
    EEPROM.write(0, 1);
    EEPROM.commit();    
  }
  //---
  //---
}

void ledTimerIsr(){
      Serial.println("ledTimerIsr");
      //blinkLed();

}

void shortDoubleBlink() {
  digitalWrite(LED_STATUS_PIN, HIGH);   
  delay(100); 
  digitalWrite(LED_STATUS_PIN, LOW);
  delay(500); 
  digitalWrite(LED_STATUS_PIN, HIGH);   
  delay(100); 
  digitalWrite(LED_STATUS_PIN, LOW);
}

void blinkLed (uint32_t blinkDelay) {
  unsigned long statusLedCurrentMillis;
  
  statusLedCurrentMillis = millis();
  if ((statusLedCurrentMillis - statusLedPreviousMillis) >= blinkDelay) {    
    digitalWrite(LED_STATUS_PIN, HIGH);   
    delay(50); 
    digitalWrite(LED_STATUS_PIN, LOW);    
    statusLedPreviousMillis = statusLedCurrentMillis;      
  }
}

 void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("Client Connected");
    //SerialBT.write("Trackcol A device. Ready to receive commands...");
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
  attachInterrupt(digitalPinToInterrupt(ADXL345_INTERRUPT_PIN), adxlIsr, FALLING);   // Attach Interrupt  

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
uint8_t listDir(fs::FS &fs, const char * dirname, uint8_t levels){

    uint8_t flag;
    
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return 2;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return 2;
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
        if (String(file.name()).indexOf("log.txt") != -1) {
          flag = 1;
        }
        else {
          flag = 0;
        }
        file = root.openNextFile();
    }
    return flag;
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

void readFileBT(fs::FS &fs, const char * path){
    SerialBT.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        SerialBT.println("- failed to open file for reading");
        return;
    }

    SerialBT.println("- read from file:");
    while(file.available()){
        SerialBT.write(file.read());
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

// Write angle measurements to log file
void writeAngleToFile(float rollAngle, float pitchAngle) {
  char measChar[8];

  detachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_INTERRUPT_PIN));

  appendFile(SPIFFS, "/log.txt", t);
  appendFile(SPIFFS, "/log.txt", ",");
  dtostrf(rollAngle, 3, 2, measChar);
  appendFile(SPIFFS, "/log.txt", measChar);
  appendFile(SPIFFS, "/log.txt", ",");
  dtostrf(pitchAngle, 3, 2, measChar);
  appendFile(SPIFFS, "/log.txt", measChar);
  appendFile(SPIFFS, "/log.txt", "\r\n");

  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_INTERRUPT_PIN), pushButtonIsr, FALLING);   // Attach Interrupt 
}

// Read temp./hum. from DHT22 sensor
void readTempHum(void) {

  float h = dht.getHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.getTemperature();
  // Check if any reads failed and exit early (to try again).
  Serial.print("Temperature: ");
  Serial.println(t);

  Serial.print("Humidity: ");
  Serial.println(h);


}
void readBtCommand() {

  String btMessage = "";
  
  while(SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (incomingChar!= '\n'){
      btMessage += String(incomingChar);
    }
    else {
      Serial.println(btMessage);
      parseBtCommand(btMessage);     
      btMessage = "";
    }
  }  
}

void parseBtCommand(String btMessage) {
  uint8_t filExistsFlag;
  
  if (btMessage.indexOf("COMM_DELETE_LOG") >= 0) {
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("COMM_DELETE_LOG");
    SerialBT.println("COMM_DELETE_LOG");
    deleteFile(SPIFFS, "/log.txt");
    SerialBT.println("COMM_DELETE_LOG: OK");
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("*****");
    SerialBT.println("*****");
  }

  else if (btMessage.indexOf("COMM_GET_LOG") >= 0) {
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("COMM_GET_LOG");
    SerialBT.println("COMM_GET_LOG");
    filExistsFlag = listDir(SPIFFS, "/", 0);

    // Check if log.txt exists and if not create it
    // ---
    if (filExistsFlag == 1) {
      Serial.println("Log file exists.");
      SerialBT.println("Log file exists.");
      readFile(SPIFFS, "/log.txt"); 
      readFileBT(SPIFFS, "/log.txt");  
    }
    else if (filExistsFlag == 0) {
      writeFile(SPIFFS, "/log.txt", "");
      Serial.println("Log file created.");
      SerialBT.println("Log file created.");
    }
    // ---
    // ---
    Serial.println("COMM_GET_LOG: OK");
    SerialBT.println("COMM_GET_LOG: OK");
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("*****");
    SerialBT.println("*****");
  }

  else if (btMessage.indexOf("COMM_SET_TIME_DATE") >= 0) {
    String timeDate, buf;
    uint32_t i, year_t, month_t, day_t, hours_t, minutes_t, seconds_t;
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("COMM_SET_TIME_DATE");
    SerialBT.println("COMM_SET_TIME_DATE");
    
    timeDate = btMessage.substring(btMessage.indexOf(" "));

    buf = timeDate;

    i = buf.indexOf(',');
    year_t = buf.substring(0,i).toInt();
    buf = buf.substring(i+1, buf.length());

    Serial.println(buf);

    i = buf.indexOf(',');
    month_t = buf.substring(0,i).toInt();
    buf = buf.substring(i+1, buf.length());

    Serial.println(buf);

    i = buf.indexOf(',');
    day_t = buf.substring(0,i).toInt();
    buf = buf.substring(i+1, buf.length());

    Serial.println(buf);

    i = buf.indexOf(',');
    hours_t = buf.substring(0,i).toInt();
    buf = buf.substring(i+1, buf.length());

    Serial.println(buf);

    i = buf.indexOf(',');
    minutes_t = buf.substring(0,i).toInt();
    buf = buf.substring(i+1, buf.length());

    Serial.println(buf);

    i = buf.indexOf(',');
    seconds_t = buf.substring(0,i).toInt();
    buf = buf.substring(i+1, buf.length());
    

 //   for (int i = 0; i < timeDate.length(); i++) {
 //     if (timeDate.substring(i, i+1) == ",") {
 ///       year_t    = timeDate.substring(0, i).toInt();
 //       month_t   = timeDate.substring(i+1).toInt();
 //       day_t     = timeDate.substring(i+2).toInt();
  //      hours_t   = timeDate.substring(i+3).toInt();
  //      minutes_t = timeDate.substring(i+4).toInt();
  //      seconds_t = timeDate.substring(i+5).toInt();
  //      break;
  //    }
  //  }
   

    Serial.println(year_t);
    SerialBT.println(year_t);
    Serial.println(month_t);
    SerialBT.println(month_t);
    Serial.println(day_t);
    SerialBT.println(day_t);
    Serial.println(hours_t);
    SerialBT.println(hours_t);
    Serial.println(minutes_t);
    SerialBT.println(minutes_t);
    Serial.println(seconds_t);
    SerialBT.println(seconds_t);

    rtc.adjust(DateTime(year_t, month_t, day_t, hours_t, minutes_t, seconds_t));

    Serial.println("COMM_SET_TIME_DATE: OK");
    SerialBT.println("COMM_SET_TIME_DATE: OK");
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("*****");
    SerialBT.println("*****");
  }

  else if (btMessage.indexOf("COMM_GET_TIME_DATE") >= 0) {
    char tt[32];
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("COMM_GET_TIME_DATE");
    SerialBT.println("COMM_GET_TIME_DATE");
    
    DateTime now = rtc.now();
    sprintf(tt, "%02d-%02d-%02d %02d:%02d:%02d",  now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());

    SerialBT.println(tt);
    Serial.println(tt);   

    Serial.println("COMM_GET_TIME_DATE: OK");
    SerialBT.println("COMM_GET_TIME_DATE: OK");
    Serial.println("*****");
    SerialBT.println("*****");
    Serial.println("*****");
    SerialBT.println("*****");
  }
}


void initSetLedTimer () {
  Serial.println("initSetLedTimer");
  /* Use 1st timer of 4 */
  /* 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up */
  timer = timerBegin(0, 80, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &ledTimerIsr, true);

  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, 3000000, true);

  /* Start an alarm */
  timerAlarmEnable(timer);
}

void setLedTimer(uint32_t delay) { 
  Serial.println("setLedTimer");

  timer = timerBegin(0, 80, true);

  /* Attach onTimer function to our timer */
  timerAttachInterrupt(timer, &ledTimerIsr, true);

  /* Set alarm to call onTimer function every second 1 tick is 1us
  => 1 second is 1000000us */
  /* Repeat the alarm (third parameter) */
  timerAlarmWrite(timer, delay, true);

  /* Start an alarm */
  timerAlarmEnable(timer);

}


void btInit() {
  unsigned long statusLedCurrentMillis;
  //digitalWrite(LED_STATUS_PIN, HIGH);
  
  while(1) {
    if(ST_DATA_MEAS == true)
      break;

    if (SerialBT.hasClient() == true) {
      Serial.println( "BT client connected");
      SerialBT.println("Trackcol A device. Ready to receive commands...");
      break;
    }
    
    blinkLed(10000);
        
    
  }
    
  while(1) {
    if(ST_DATA_MEAS == true)
      break;
    if (SerialBT.available())
      readBtCommand();

    blinkLed(10000);
        
  }
}

//-----------------------------
//-----------------------------

void setup() {


  uint8_t filExistsFlag;
  byte eepromState;
  
  // put your setup code here, to run once:
  Serial.begin(9600);                 // Start the serial terminal
  SerialBT.begin("ESP32test");        //Bluetooth device name
  //SerialBT.register_callback(callback);
  Serial.println("BlueTooth communication started.");
  Serial.println();

  // File system initialization
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // initialize EEPROM with predefined size
  //---
  EEPROM.begin(EEPROM_SIZE);

  eepromState = EEPROM.read(0);

  if (eepromState == 0)
    ST_DATA_MEAS = false;
  else if (eepromState == 1)
    ST_DATA_MEAS = true;
  else
    ST_DATA_MEAS = false;

  Serial.print("ST_DATA_MEAS:");
  Serial.print(ST_DATA_MEAS);
  //---
  //---

  pinMode(LED_STATUS_PIN, OUTPUT); 
  //initSetLedTimer();
  

  // Set pushbutton interrupt
  pinMode(PUSH_BUTTON_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_INTERRUPT_PIN), pushButtonIsr, FALLING);   // Attach Interrupt 
   
  // Initialize the ADXL345 sensor 
  adxl345Config();

  // Configure DHT22 pin and sensor type
  dht.setup(15, DHTesp::DHT22);

  // Set our ESP32 to wake up every TIME_TO_SLEEP * uS_TO_S_FACTOR seconds
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Check if log.txt exists and if not create it
  // ---
  filExistsFlag = listDir(SPIFFS, "/", 0);

  if (filExistsFlag == 1) {
    Serial.println("Log file exists.");
  }
  else if (filExistsFlag == 0) {
    writeFile(SPIFFS, "/log.txt", "");
    Serial.println("Log file created.");
  }
  // ---
  // ---

  // Initialize time
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  //rtc.adjust(DateTime(2019, 10, 07, 22, 40, 0));
    
  Serial.println( "Test complete" );
 
}

void loop() {
  unsigned long statusLedCurrentMillis = 0;

  if (eepromWriteFlag) {
      shortDoubleBlink();
      saveState();
      eepromWriteFlag = false;
  }

  if (ST_DATA_MEAS == true) {
  
    // put your main code here, to run repeatedly:
    int xAccel, yAccel, zAccel;
    float pitchAngle, rollAngle;
    // Time value to be used for delay 
    unsigned long measCurrentMillis = 0;
    
  

    //digitalWrite(LED_STATUS_PIN, HIGH); 

    blinkLed(3000);

    /*
    statusLedCurrentMillis = millis();
    if ((statusLedCurrentMillis - statusLedPreviousMillis) >= 3000) {    
      digitalWrite(LED_STATUS_PIN, HIGH);   
      delay(50); 
      digitalWrite(LED_STATUS_PIN, LOW);    
      statusLedPreviousMillis = statusLedCurrentMillis;
        
    }*/

   
     //Increment boot number and print it every reboot
    //++bootCount;
    //Serial.println("Boot number: " + String(bootCount));
  
    measCurrentMillis = millis();
    if ((measCurrentMillis - measPreviousMillis) >= 10000) {
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

      DateTime now = rtc.now();

      sprintf(t, "%02d-%02d-%02d %02d:%02d:%02d",  now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
  
      // Write measurements to file
      writeAngleToFile(rollAngle, pitchAngle);  

      readTempHum();

      //---
  
  
      Serial.print(F("Date/Time: "));
      Serial.println(t);
      //---
      //---
      measPreviousMillis = measCurrentMillis;
    }
    
    //Serial.println("Going to sleep now");
    //Serial.flush(); 
  
  
    //if (SerialBT.available())
    //  readBtCommand();
  
    
  
    //delay(10000);
    //esp_deep_sleep_start();
  }

  if (ST_DATA_MEAS ==  false) {
    btInit();
  
  }
}
