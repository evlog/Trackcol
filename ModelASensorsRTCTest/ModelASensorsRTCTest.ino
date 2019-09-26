/**
  Trackcol ModelA
  Name: ModelASensorsRTCTest
  Purpose: ESP32 plus sensors and RTC test code

  @author E. Logaras
  @version 1.0 25/09/2019
*/

// Define libraries
//-----------------------------
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
//-----------------------------
//-----------------------------

// Define constants and global variables
//-----------------------------
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
//-----------------------------

//Auxiliary Functions
//-----------------------------

// Read ADXL345 information
void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displayDataRate(void)
{
  Serial.print  ("Data Rate:    "); 
   
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print  ("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print  ("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print  ("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print  ("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print  ("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print  ("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print  ("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print  ("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print  ("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print  ("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print  ("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print  ("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print  ("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print  ("0.10 "); 
      break;
    default:
      Serial.print  ("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}
 
void displayRange(void)
{
  Serial.print  ("Range:         +/- "); 
   
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      Serial.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print  ("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print  ("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print  ("2 "); 
      break;
    default:
      Serial.print  ("?? "); 
      break;
  }  
  Serial.println(" g");  
}
//-----------------------------
//-----------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");
   
  // Initialise the sensor 
  if(!accel.begin())
  {
    // There was a problem detecting the ADXL345 ... check your connections 
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }
 
  // Set the range 
  accel.setRange(ADXL345_RANGE_16_G);
   
  // Display some basic information on this sensor 
  displaySensorDetails();
   
  // Display additional settings (outside the scope of sensor_t) 
  displayDataRate();
  displayRange();
  Serial.println("");
}

void loop() {
  // put your main code here, to run repeatedly:
  /*Read from ADXL345 accelerometer*/
  float X, Y, Z, roll, pitch, rollF, pitchF;
  
  sensors_event_t event; 
  accel.getEvent(&event);

  X = event.acceleration.x;
  Y = event.acceleration.y;
  Z = event.acceleration.z;
  

 
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: "); Serial.print(X); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Z); Serial.print("  ");Serial.println("m/s^2 ");

  // Calculate Roll and Pitch (rotation around X-axis, rotation around Y-axis)
  roll = atan(Y / sqrt(pow(X, 2) + pow(Z, 2))) * 180 / PI;
  pitch = atan(-1 * X / sqrt(pow(Y, 2) + pow(Z, 2))) * 180 / PI;

  // Low-pass filter
  rollF = 0.94 * rollF + 0.06 * roll;
  pitchF = 0.94 * pitchF + 0.06 * pitch;


  Serial.print(roll);
  Serial.print("/");
  Serial.println(pitch);

  Serial.print(rollF);
  Serial.print("/");
  Serial.println(pitchF);

  
  /*Take a one second break*/
  delay(1000);
}
