/*

Viacheslav Osipov

Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp8266-nodemcu/

 

Permission is hereby granted, free of charge, to any person obtaining a copy

of this software and associated documentation files.

 

The above copyright notice and this permission notice shall be included in all

copies or substantial portions of the Software.

*/

 

#include <ESP8266WiFi.h>

#include <espnow.h>

 

#include <Adafruit_Sensor.h>

#include <DHT.h>

 

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

 


#define DHTPIN 5

 

//#define DHTTYPE    DHT11     // DHT 11

#define DHTTYPE    DHT22     // DHT 22 (AM2302)

//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

 

DHT dht(DHTPIN, DHTTYPE);

 

float temperature;

float humidity;

 

float incomingTemp;

float incomingHum;

 

const long interval = 10000;

unsigned long previousMillis = 0;   

 

String success;

 

typedef struct struct_message {

float temp;

float hum;

} struct_message;

 

// Создаем переменную для хранения отправляемого сообщения

struct_message DHTReadings;

 

struct_message incomingReadings;

 

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

Serial.print("Last Packet Send Status: ");

if (sendStatus == 0){

Serial.println("Delivery success");

}

else{

Serial.println("Delivery fail");

}

}

 

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {

memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

Serial.print("Bytes received: ");

Serial.println(len);

incomingTemp = incomingReadings.temp;

incomingHum = incomingReadings.hum;

}

 

void getReadings(){

temperature = dht.readTemperature();

if (isnan(temperature)){

Serial.println("Failed to read from DHT");

temperature = 0.0;

}

humidity = dht.readHumidity();

if (isnan(humidity)){

Serial.println("Failed to read from DHT");

humidity = 0.0;

}

}

 

void printIncomingReadings(){

// Отображаем показания в мониторе порта

Serial.println("INCOMING READINGS");

Serial.print("Temperature: ");

Serial.print(incomingTemp);

Serial.println(" ºC");

Serial.print("Humidity: ");

Serial.print(incomingHum);

Serial.println(" %");

}

 

void setup() {

Serial.begin(115200);

 

dht.begin();

 

WiFi.mode(WIFI_STA);

WiFi.disconnect();

 

if (esp_now_init() != 0) {

Serial.println("Error initializing ESP-NOW");

return;

}

 

esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

 

esp_now_register_send_cb(OnDataSent);

 

esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

 

esp_now_register_recv_cb(OnDataRecv);

}

 

void loop() {

unsigned long currentMillis = millis();

if (currentMillis - previousMillis >= interval) {

previousMillis = currentMillis;

 

getReadings();

 


DHTReadings.temp = temperature;

DHTReadings.hum = humidity;

 

esp_now_send(broadcastAddress, (uint8_t *) &DHTReadings, sizeof(DHTReadings));

 

printIncomingReadings();

}

}