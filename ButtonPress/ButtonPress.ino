int pin = 9;

int pressedLength = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(pin, INPUT_PULLUP);
  Serial.println("SETTUP DONE");
}

void loop() {
  // put your main code here, to run repeatedly:
  bool isPressed = digitalRead(pin);
  if(!isPressed){
    pressedLength++;
    Serial.print("BUTTON PRESSED ");
    Serial.println(pressedLength);
  }
  
}
