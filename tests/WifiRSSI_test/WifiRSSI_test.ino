#include <SPI.h>
#include <WiFi.h>

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



  
}

void loop () {

  int num_of_points = 20;
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
      delay(10);
    }
  
    rssi_av = rssi / num_of_points;

    Serial.print("RSSI:");
    Serial.println(rssi_av);

    Serial.print("Distance:");
    float d = pow(10, (-50 - rssi_av) / 20.0);
    Serial.println(d);
  
    //delay(3000);
  }

  
  
}
