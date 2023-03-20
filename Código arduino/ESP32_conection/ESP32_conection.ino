#include <ThingerESP32.h>
#include "WiFi.h"
#include <DHT.h>
 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
 
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

//#include "arduino_secrets.h"

/* Conexión con thinger.io */
#define THINGER_SERIAL_DEBUG
#define USERNAME "franklingo13"
#define DEVICE_ID "esp32"
#define DEVICE_CREDENTIAL "W+0%Kud4d8Vp!CGe"

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

// Conexión WiFi
const char* SSID = "POCOX3";
const char* SSID_PASWORD = "pocox5597";
float yaw = 0; // defined as a global variable for thing.io

void setup() {
  // open serial for debugging
  //Serial.begin(115200);
  Serial.begin(9600);
  WiFi.begin(SSID,SSID_PASWORD);


  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("Conectando...");

   }
   Serial.print("Conectado");

  pinMode(16, OUTPUT);

  thing.add_wifi(SSID, SSID_PASWORD);
  
  // Comenzamos el sensor DHT
  dht.begin();
/*
  // digital pin control example (i.e. turning on/off a light, a relay, configuring a parameter, etc)
  thing["GPIO_16"] << digitalPin(16);

  // resource output example (i.e. reading a sensor value)
  thing["millis"] >> outputValue(millis());
*/
  // more details at http://docs.thinger.io/arduino/
}

void loop() {
  //thing.handle();
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  // Leemos la temperatura en grados Fahreheit
  float f = dht.readTemperature(true);
 
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
    yaw = h;
    thing.handle();
    thing["temp"] >> [](pson& out){
      out = yaw;
    };
}
