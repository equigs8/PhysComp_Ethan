//SS4 Pong Game
//2 pots that control each paddle
//1 button that pauses the game
//1 button that resets the game

// BUTTONS
const int resetButtonPin = 1;
const int pauseButtonPin = 2;

//BUTTON VALUES
int resetButtonValue;
int pauseButtonValue;

//POTS
const int paddlePot1Pin = 8;
const int paddlePot2Pin = 3;

//Pot Values
int paddlePot1Value;
int paddlePot2Value;

//TIMER
int lastTime = 0;
int currentTime = 0;
int timerInterval = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(pauseButtonPin, INPUT_PULLUP);

  Serial.begin(9600);     // Start serial communication @ 9600 baud rate
  analogReadResolution(8);

}

void loop() {
  currentTime = millis(); //read current elapsed time
  if (currentTime - lastTime >= timerInterval)  //if we have reached our timer interval...
  {
    paddlePot1Value = analogRead(paddlePot1Pin);
    paddlePot2Value = analogRead(paddlePot2Pin);

    resetButtonValue = digitalRead(resetButtonPin);
    pauseButtonValue = digitalRead(pauseButtonPin);

    //int valueArray[] = {paddlePot1Value, paddlePot2Value, resetButtonValue, pauseButtonValue};
    //Serial.print(convertArrayToString(valueArray));

    
    Serial.print(paddlePot1Value);
    Serial.print(',');
    Serial.print(paddlePot2Value);
    Serial.print(',');
    Serial.print(resetButtonValue);
    Serial.print(',');
    Serial.print(pauseButtonValue);
    Serial.print('\n');
  }
}


//A funtion that takes in an array and returns it as a string separated by commas and ending with a newline
String convertArrayToString(int arr[]) {
  String result = "";
  int size = sizeof(arr) / sizeof(arr[0]);
  for (int i = 0; i < size; i++) {
    result += String(arr[i]);
    if (i < size - 1) {
      result += ",";
    }
  }
  result += "\n";
  return result;
}
