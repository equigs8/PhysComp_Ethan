const int pin = 9;

void setup() {
  // put your setup code here, to run once:

  analogReadResolution(12); //12 is the default

  //only need pinMode() for digital.
  Serial.begin(115200);
  Serial.println("SETUPDONE");
}

void loop() {
  // put your main code here, to run repeatedly:
  int adcVal = analogRead(pin);
  Serial.println(adcVal);
}
