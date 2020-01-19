#include <SPI.h>
#include <WiFi.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//SSID of your network
char ssid[] = "Redmi";
//password of your WPA Network
char pass[] = "spartan3";

void setup()
{



   Serial.begin(9600);
  
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

  
}

void loop () {

  int num_of_points = 300;
  int i;
  long rssi = 0;
  long rssi_av = 0;



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
      delay(5);
    }
  
    rssi_av = rssi / num_of_points;

    Serial.print("RSSI:");
    Serial.println(rssi_av);

    Serial.print("Distance:");
    float d = pow(10, (-50 - rssi_av) / 20.0);
    Serial.println(d);

    display.clearDisplay();
    display.setCursor(0,28);
    display.println(d);
    display.println(rssi_av);
    display.display();
  
    delay(1);
  }

  
  
}
