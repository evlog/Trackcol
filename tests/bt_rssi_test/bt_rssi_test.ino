//ESP32, Classic Bluetooth, SPP
#include "BluetoothSerial.h"
#include "esp_gap_bt_api.h"
#include <esp_spp_api.h>

BluetoothSerial SerialBT;
byte rssi; //RSSI
byte addr[6] = {0,0,0,0,0,0}; //to keep MAC address of the remote device

//RSSI callback function
void gap_callback (esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
  if (event == ESP_BT_GAP_READ_RSSI_DELTA_EVT) rssi = param->read_rssi_delta.rssi_delta;
}

//SPP service callback function (to get remote MAC address)
void spp_callback (esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  if (event == ESP_SPP_SRV_OPEN_EVT) memcpy(addr, param->srv_open.rem_bda, 6);
}

void setup()
{
  Serial.begin(9600);
  SerialBT.begin("ESP32");
  //register RSSI callback function:
  esp_bt_gap_register_callback(gap_callback);
  //register SPP service callback to get remote address:
  SerialBT.println("");
  SerialBT.register_callback(spp_callback);
}

void loop()
{
  //when we need RSSI call this:
  esp_bt_gap_read_rssi_delta (addr); //now variable rssi contains RSSI level
  byte b = rssi; //etc....
}
