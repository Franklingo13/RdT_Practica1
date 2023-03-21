
#include <DHT.h>
#include <WiFi.h>


// Conexión WiFi
/*const char* SSID = "POCOX3";
const char* SSID_PASSWORD = "pocox5597";*/

//const char* SSID = "Alejate_de_mi_WiFi";
//const char* SSID_PASSWORD = "0107100596nand74ls00";

const char* SSID = "PCJD";
const char* SSID_PASSWORD = "12345678";




// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 15
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float temperature_DHT, humidity_DHT; //variables for DHT

TimerHandle_t wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(SSID, SSID_PASSWORD);
}

// variables para monitorear el tiempo entre lecturas
unsigned long previousMillis = 0;   
const long interval = 10000;  


void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %dn", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}






void setup() {
  Serial.begin(115200);
  Serial.println();

  dht.begin();
  delay(1000);
  
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  connectToWifi();

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    //Read from DHT
    humidity_DHT = dht.readHumidity();
    temperature_DHT = dht.readTemperature();
    
    if (isnan(temperature_DHT) || isnan(humidity_DHT)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    unsigned long currentMillis = millis();

    Serial.printf("Temperatura: %.2f \n", temperature_DHT);


    Serial.printf("Hunedad: %.2f \n", humidity_DHT);
  }

}
