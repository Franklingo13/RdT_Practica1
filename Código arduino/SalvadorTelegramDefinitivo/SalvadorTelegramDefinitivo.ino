
//#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  // telegram
#include <ArduinoJson.h>           // telegram
#include <DHT.h>                   // sensor de humedad y temperatura
#include <Wire.h>                  // RFID
#include <SPI.h>                   //Librería para comunicación SPI
#include <UNIT_PN532.h>            //Librería Modificada de SPI 
#include "CTBot.h"


#define DHTTYPE DHT11              // tipo de sensor de humed, y temp
// para el RFID
//Conexiones SPI del ESP32
#define PN532_SS   5              // Pin SS del PN532
#define PN532_SCK  18              // Pin SCK del PN532
#define PN532_MOSI 23              // Pin MOSI del PN532
#define PN532_MISO 19              // Pin MISO del PN532

//

/// sensor humedad 
float temperature_DHT, humidity_DHT; //variables for DHT
unsigned long previousMillis = 0;   
const long interval = 10000;  

// Ingresamos el SSID y Password de nuestra red WI-FI
const char* ssid = "PCJD";
const char* password = "12345678";

// iniciamos el Bot de Telegram ingresando el Token que recibimos de Botfather
#define BOTtoken "6042291014:AAEiV0oQmc3ZTUxGbeFl7bHPHMV7FQz7d1I"  

// Ingresamos el ID que recibimos del Chat con IDBot
#define CHAT_ID "1105961015"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);
const int  puerta = 0; // puerta abierta - cerrada
// RFID
uint8_t DatoRecibido[4];          //Para almacenar los datos
UNIT_PN532 nfc(PN532_SS);        // Línea enfocada para la comunicación por SPI
// Funcion auxiliar para mostrar el buffer
void printArray(byte *buffer, byte bufferSize) {
   for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
   }
}
int number = 0;
const int  Trigger = 4;
const int  Echo = 16;
const int  DHTPIN = 15; // Definimos el pin digital donde se conecta el sensor
byte  refrigeracion = 22;
int tiempo;
int distancia;
int indicador=0;
int aux1=0;
int aux2=0;
int aux3=0;
int frio =0;
DHT dht(DHTPIN, DHTTYPE);



// ============== Mensaje obtenidos en telegram =====================
// tiempo que va ha estar esperando respuesta en telegram          ==
int botRequestDelay = 2000;
unsigned long lastTimeBotRan;
///
// Maneja lo que Sucede Cuando se recibe un nuevo mensaje          ==
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);   
    // Imprime el mensaje recibido
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    if (text == "Si") {
      Serial.println("Si es el frnak");
      String perrito= "Ingreso verificado\n◥------◥\n|● ▄ ◉ |\n| ‿/ʊ\‿ |\n|══o══|";
      bot.sendMessage("386743637", perrito ,"");
    }else{
      Serial.println("NO es");  // Notifica al Master
      bot.sendMessage(CHAT_ID, "  ALERTA de intruso en la bodega..!!  \(°-°)/");
      }
      text = "";
  }
}
////////////////////////=================================================================

void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      // obtenemos la Hora UTC via NTP
  client.setTrustAnchors(&cert);         // Agregamos el certificado Raiz de api.telegram.org
  // conexion con la red WI-FI:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");

  pinMode (refrigeracion,OUTPUT);
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);

  dht.begin();

//-----------RFID------------
pinMode (puerta,OUTPUT); // puerta
// pinMode (led,OUTPUT);
//_____ lectura de RFID_________________________________
  Serial.begin(115200); //Inicio de puerto Serial a 115200 baudios
  nfc.begin(); //Comienza la comunicación del PN532
  uint32_t versiondata = nfc.getFirmwareVersion();//Obtiene información de la placa
  if (! versiondata) { //Si no se encuentra comunicación con la placa --->
    Serial.print("No se encontró la placa PN53x");
    while (1); // Detener
  }
  Serial.print("Chip encontrado PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX); //Imprime en el serial que version de Chip es el lector
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC); // Imprime que versión de firmware tiene la placa
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);
  //Establezca el número máximo de reintentos para leer de una tarjeta.
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig(); //Configura la placa para realizar la lectura
  Serial.println("Esperando una tarjeta ISO14443A ...");
//-----------------------  
}
uint8_t validUID[4] = {0x32, 0x1F, 0xFA, 0x21 };  // UID:JD Master
uint8_t validUID2[4] = {0x39, 0xB3, 0x43, 0xE8 };  // UID:Frank  
//Función para comparar dos vectores
bool isEqualArray(uint8_t* arrayA, uint8_t* arrayB, uint8_t length)
{
  for (uint8_t index = 0; index < length; index++)
  {
    if (arrayA[index] != arrayB[index]) return false;
  }
  return true;
}

void loop() {
TBMessage msg;
//---------- Ultrasonido-------------
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);
  tiempo = pulseIn(Echo, HIGH);
  distancia = tiempo/59;


 // Serial.print("Distancia: ");
 // Serial.print(distancia);
 // Serial.println(" cm");
  

  if (distancia < 15 && distancia >= 10){
    Serial.println("Bajo");
    indicador= 1;
    if (aux1 == 0){
       bot.sendMessage(CHAT_ID, "Atencion !!..  Nivel bajo de pruducto");
       aux1=1;
      }
    aux2=0;
    aux3=0;
    
    
    }
  if (distancia < 10 && distancia >6){
    Serial.println("Medio");
    indicador= 5;
    aux1=0;
    aux2=0;
    aux3=0;
    }
  if (distancia <= 6 && distancia > 0){
    Serial.println("Alto");
    indicador= 10;

    if (aux3 == 0){
       bot.sendMessage(CHAT_ID, "Productos LLENOS :)");
       aux3=1;
      }
    aux1=0;
    aux2=0;
    }

  if (distancia >= 15){
    Serial.println("Vacio");
    indicador =0 ;
    if (aux2 == 0){
       bot.sendMessage(CHAT_ID, "Alerta..!!..  No hay productos..");
       aux2=1;
      }
    aux1=0;
    aux3=0;
    
    }
//------------------------------------------------------------
// ---------------------sensor de temperatura y humedad--------
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

     if (temperature_DHT >= 23.5){
      digitalWrite(refrigeracion, LOW);
      if (frio == 0){
        frio =1;
       bot.sendMessage(CHAT_ID, "Atención..!! Temperatura ALTA... Sistema de refrigeracion ACTIVO....");
       }          
      }
      if (temperature_DHT < 22.5){
        digitalWrite(refrigeracion, HIGH);
        if (frio == 1){
        frio =0;
       bot.sendMessage(CHAT_ID, "Temperatura estable, sistema de  refrigeracion DESACTIVADO....");
       }         
        }
  }  

//======================RFID=========================================
  boolean LeeTarjeta; //Variable para almacenar la detección de una tarjeta
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Búfer para almacenar el UID devuelto
  uint8_t LongitudUID; //Variable para almacenar la longitud del UID de la tarjeta
  //Recepción y detección de los datos de la tarjeta y almacenamiento en la variable "LeeTarjeta"
  LeeTarjeta = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &LongitudUID,500);
  //Se detecto un tarjeta RFID
  if (LeeTarjeta) {
    Serial.println("Tarjeta encontrada!");
    Serial.print("Longitud del UID: ");
    Serial.print(LongitudUID, DEC); //Imprime la longitud de los datos de la tarjeta en decimal
    Serial.println(" bytes");
    Serial.print("Valor del UID: ");
    // Imprime los datos almacenados en la tarjeta en Hexadecimal
    Serial.print("UID: "); printArray(uid, LongitudUID);
    Serial.println("");
    if (isEqualArray(uid, validUID, LongitudUID))
    {
      Serial.println("Bodega abierta por: JD \(^-^)/");
      digitalWrite(puerta, HIGH);
      bot.sendMessage(CHAT_ID, "Bodega abierta por: JD \(^-^)/");
      delay(1000);
      digitalWrite(puerta, LOW);
    }
    else if (isEqualArray(uid, validUID2, LongitudUID)){
      Serial.println("Bodega abierta por: FRANK \(°-°)/");
      digitalWrite(puerta, HIGH);
      bot.sendMessage(CHAT_ID, "Bodega abierta por: FRANK \(°-°)/");
      bot.sendMessage("386743637", "ATENCION.. ¡¡¡ Bodega abierta por ti, fueste tu?");
 // =============== RESPUESTA DE TELEGRAM ===============================
 if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  // ==================================================       
      delay(1000);
      digitalWrite(puerta, LOW);
      } else 
    {
      Serial.println("Tarjeta invalida");
    Serial.println("");
   delay(2000);
    }
  }
}
