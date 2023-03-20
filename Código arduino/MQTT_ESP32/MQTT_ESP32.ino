
#include <DHT.h>
#include <WiFi.h>
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

// Conexión WiFi
/*const char* SSID = "POCOX3";
const char* SSID_PASSWORD = "pocox5597";*/

const char* SSID = "Alejate_de_mi_WiFi";
const char* SSID_PASSWORD = "0107100596nand74ls00";

// Conexión con mosquitto Broker
//#define MQTT_HOST IPAddress(192, 168, 0, 53) 
#define MQTT_HOST "192.168.0.53" 
#define MQTT_PORT 1883

//MQTT Topics
#define MQTT_PUB_TEMP_DHT "esp32/dht/temperature"
#define MQTT_PUB_HUM_DHT  "esp32/dht/humidity"

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 15
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float temperature_DHT, humidity_DHT; //variables for DHT



//Creación de un objeto AsyncMqttClient
AsyncMqttClient mqttClient;
//temporizadores para reconexión 
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

// variables para monitorear el tiempo entre lecturas
unsigned long previousMillis = 0;   
const long interval = 10000;  

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(SSID, SSID_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %dn", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); 
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
//  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  dht.begin();
  delay(1000);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
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

    // Publish an MQTT message on topic esp32/dht/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP_DHT, 0, true, String(temperature_DHT).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_TEMP_DHT, packetIdPub1);
    Serial.printf("Message: %.2f \n", temperature_DHT);

    // Publish an MQTT message on topic esp32/dht/humidity
    uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_HUM_DHT, 1, true, String(humidity_DHT).c_str());                            
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_HUM_DHT, packetIdPub2);
    Serial.printf("Message: %.2f \n", humidity_DHT);
  }

}
