const int potPin = 8;
int potValue = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);     // Start serial communication @ 9600 baud rate
  analogReadResolution(8);
}

void loop() {
  // put your main code here, to run repeatedly:
  potValue = analogRead(potPin);


  Serial.write(potValue);
  //Serial.println(potValue);
}
