#include <ThingerESP32.h>
#include "WiFi.h"
#include <DHT.h>
 
/* Conexión con thinger.io */
#define THINGER_SERIAL_DEBUG
#define USERNAME "franklingo13"
#define DEVICE_ID "esp32_01"
//#define DEVICE_CREDENTIAL "W+0%Kud4d8Vp!CGe"
#define DEVICE_CREDENTIAL "k%9b&m68ctdCJxA#"
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

// Conexión WiFi
const char* SSID = "POCOX3";
const char* SSID_PASSWORD = "pocox5597";


void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  
  thing.add_wifi(SSID,SSID_PASSWORD);   

  thing["GPIO_2"] << digitalPin(2);

}

void loop() {
  thing.handle();
  // put your main code here, to run repeatedly:

}
