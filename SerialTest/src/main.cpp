#include <Arduino.h>

int inByte = 0;
int lightPin = 4;

void setup() {
  Serial.begin(9600);
  pinMode(lightPin, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    inByte = Serial.read();

    if(inByte == 'A') {
      digitalWrite(lightPin, HIGH);
    } else {
      digitalWrite(lightPin, LOW);
    }
  }
}