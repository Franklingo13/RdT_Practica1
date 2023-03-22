
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>



int number = 0;
const int  Trigger = D5;
const int  Echo = D6;
int tiempo;
int distancia;
int indicador=0;
int aux1=0;
int aux2=0;
int aux3=0;
 
void setup() {
  Serial.begin(115200);
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);

  
}

void loop() {

// Ultrasonido
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);
  tiempo = pulseIn(Echo, HIGH);
  distancia = tiempo/59;


 Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
  

  if (distancia < 15 && distancia >= 10){
    Serial.println("Bajo");
    indicador= 1;
    if (aux1 == 0){
       Serial.println("Atencion !!..  Nivel bajo de pruducto");
       aux1=1;
      }
    aux2=0;
    aux3=0;
    
    
    }
  if (distancia < 10 && distancia >= 5){
    Serial.println("Medio");
    indicador= 5;
    aux1=0;
    aux2=0;
    aux3=0;
    }
  if (distancia < 5 && distancia > 0){
    Serial.println("Alto");
    indicador= 10;

    if (aux3 == 0){
       Serial.println("Productos LLENOS :)");
       aux3=1;
      }
    aux1=0;
    aux2=0;
    }

  if (distancia >= 15){
    Serial.println("Vacio");
    indicador =0 ;
    if (aux2 == 0){
      Serial.println("Alerta..!!..  No hay productos..");
       aux2=1;
      }
    aux1=0;
    aux3=0;
    
    }



   

   delay(2000);
  
}
