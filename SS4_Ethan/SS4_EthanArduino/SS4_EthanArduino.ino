//SS4 Pong Game
//2 pots that control each paddle
//1 button that pauses the game
//1 button that resets the game

// BUTTONS
const int resetButtonPin = 0;
const int pauseButtonPin = 0;

//POTS
const int paddlePot1Pin = 8;
const int paddlePot2Pin = 2;

//Pot Values
int paddlePot1Value;
int paddlePot2Value;

//TIMER
int lastTime = 0;
int currentTime = 0;
int timerInterval = 2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);     // Start serial communication @ 9600 baud rate
  analogReadResolution(8);
}

void loop() {
  currentTime = millis(); //read current elapsed time
  if (currentTime - lastTime >= timerInterval)  //if we have reached our timer interval...
  {
    paddlePot1Value = analogRead(paddlePot1Pin);
    paddlePot2Value = analogRead(paddlePot2Pin);


    Serial.print(paddlePot1Value);
    Serial.print(',');
    Serial.print(paddlePot2Value);
    Serial.print('\n');
  }
}
