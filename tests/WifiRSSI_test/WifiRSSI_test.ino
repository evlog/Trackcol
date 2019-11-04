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
  long rssi0 = WiFi.RSSI();
  delay(20);

  long rssi1 = WiFi.RSSI();
  delay(20);

  long rssi2 = WiFi.RSSI();
  delay(20);

  long rssi3 = WiFi.RSSI();
  delay(20);

  long rssi4 = WiFi.RSSI();
  delay(20);

  long rssi5 = WiFi.RSSI();
  delay(20);

  long rssi6 = WiFi.RSSI();
  delay(20);


  long rssi = (rssi0 + rssi1 + rssi2 + rssi3 + rssi4 + rssi5 + rssi6) / 7;

  Serial.println(rssi);

  delay(3000);

  
  
}
