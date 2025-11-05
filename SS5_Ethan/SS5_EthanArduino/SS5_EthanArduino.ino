#include <ESP32Servo.h>

Servo servo;
const int LED1 = 4;
const int LED2 = 5;
const int servoPin = 6;
//byte val; //variable for storing incoming data

int angle = 0;
int val = 0;
int speed = 5;

void setup() {
  // put your setup code here, to run once:
  servo.attach(servoPin);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(9600);

}

void loop() {


  if (Serial.available()){
    val = Serial.read();
  }

  if(val == 255){
    //sweep motor
    val = 0;

    for(angle = 0; angle <= 180; angle++){
      servo.write(angle);
      delay(speed);
    }
    for(angle = 180; angle >= 0; angle--){
      servo.write(angle);
      delay(speed);
    }
  }else{
    servo.write(val);
  }
  
}
