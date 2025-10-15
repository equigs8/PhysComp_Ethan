/*
A short script to find the MAC address of the ESP32 board
*/


#include <WiFi.h">

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

void loop(){
}