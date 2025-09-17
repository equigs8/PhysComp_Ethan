int buttonPin = 9;

int pressedLength = 0;
int lengthBetweenPauses = 0;

bool buttonState = 0;
bool lastButtonState = 0;
int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("SETUP DONE");
}

void loop() {
  // put your main code here, to run repeatedly:
  bool buttonPressed = !digitalRead(buttonPin);
  //Serial.println(buttonPressed);

  while(buttonPressed && !lastButtonState){
    buttonPressed = !digitalRead(buttonPin);
    pressedLength++;
    //Serial.println(pressedLength);
  }
  if(pressedLength > 0)
  {
    Serial.print(pressedLength);
    if (pressedLength <= 1500000){
      
      Serial.println("DOT");
    } else{
      Serial.println("DASH");
    }
  }
  lastButtonState = buttonPressed;
  pressedLength = 0;


  // buttonState = !digitalRead(buttonPin);
  

  // if(buttonState && !lastButtonState){
  //   counter++;
  // }
  

  // lastButtonState = buttonState;

  // Serial.printf("ButtonState = %i, Counter = %i \n", buttonState,counter);
  

}
