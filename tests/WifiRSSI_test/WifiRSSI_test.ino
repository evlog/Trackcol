#include <SPI.h>
#include <WiFi.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

int freq = 2000;
int channel = 0;
int resolution = 8;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//SSID of your network
char ssid[] = "Redmi";
//password of your WPA Network
char pass[] = "spartan3";

void setup()
{



   Serial.begin(9600);

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(15, channel);
  
  WiFi.begin(ssid, pass);
  Serial.print("Wait for WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println(WiFi.localIP());


  //Wire.begin(21, 22);
  Wire.begin(5, 4);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
 
  // Clear the buffer.
  display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);
display.setCursor(0,28);

//ledcWrite(channel, 255);
//ledcWriteTone(channel, 200);
//ledcWrite(channel, 125);
  
}

void loop () {

  int num_of_points = 200;
  int i;
  long rssi = 0;
  long rssi_av = 0;


  /*ledcWriteTone(channel, 2000);
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle=dutyCycle+10){
  
    Serial.println(dutyCycle);
  
    ledcWrite(channel, dutyCycle);
    delay(1000);
  }
  
 ledcWrite(channel, 125);
  
  for (int freq = 255; freq < 10000; freq = freq + 250){
  
     Serial.println(freq);
  
     ledcWriteTone(channel, freq);
     delay(1000);
  }*/

  // Reconnect to WiFi if not connected
  //------
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Reconnecting to WiFi...\n");
    WiFi.begin(ssid, pass);
  }

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  //------
  //------

  if (WiFi.status() == WL_CONNECTED) {
  
    for (i = 0; i < num_of_points;i++) {
      rssi = rssi + WiFi.RSSI();
      delay(1);
    }
  
    rssi_av = rssi / num_of_points;

    Serial.print("RSSI:");
    Serial.println(rssi_av);

    ledcWrite(channel, 250);

    rssi_av = rssi_av * (-1);

    if (rssi_av < 25)
      ledcWriteTone(channel, 3000);

    else if ((rssi_av > 25) & (rssi_av < 30))
      ledcWriteTone(channel, 2800);

    else if ((rssi_av > 30) & (rssi_av < 35))
      ledcWriteTone(channel, 2500);

    else if ((rssi_av > 35) & (rssi_av < 40))
      ledcWriteTone(channel, 1500);

    else if ((rssi_av > 40) & (rssi_av < 45))
      ledcWriteTone(channel, 1000);

    else if ((rssi_av > 45) & (rssi_av < 50))
      ledcWriteTone(channel, 600);

    else if ((rssi_av > 50) & (rssi_av < 55))
      ledcWriteTone(channel, 300);

    else if ((rssi_av > 55) & (rssi_av < 60))
      ledcWriteTone(channel, 100);
      
    else if ((rssi_av > 60))
      ledcWriteTone(channel, 1);


    Serial.println((2500 - (rssi_av*rssi_av*rssi_av)/(-128)));

    Serial.print("Distance:");
    float d = pow(10, (-50 - rssi_av) / 20.0);
    Serial.println(d);

    display.clearDisplay();
    display.setCursor(0,28);
    display.println(d);
    display.println(rssi_av);
    display.display();
  
    //delay(1);
  }

  
  
}
