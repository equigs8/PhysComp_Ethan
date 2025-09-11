/*
Four LED Homework
By Ethan Quigley
*/

//GLOBAL VARIABLES
//Light Pins. Assign to the pin number on microcontroller
int lightPin1 = 0;
int lightPin2 = 0;
int lightPin3  = 0;
int lightPin4 = 0;

//Current Light State
int light1On = false;
int light2On = false;
int light3On = false;
int light4On = false;

//Word to translate into morse code
char word[] = "test";
int currentLetterPosition = 0;

int dotLength = "500"; //The length of the dot in miliseconds
int dashLength = dotLength*3; //The length of the dash in miliseconds
int gapeBetweenLetterLength = dashlength;

void setup() {
  // put your setup code here, to run once:
  Signal.begin(150200);
  pinMode(lightPin1,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  char currentLetter[1] = getNextLettter();
  playLetter(currentLetter);
  pauseBetweenLetters();
  //Check if at end of word
  if (checkIfEndOfWord()){
    currentLetterPosition = 0;
    Signal.println("END OF WORD");
  }

}

void playLetter(char currentLetter[]){

  morseCodeLength = getMorseCodeLength(currentLetter)
}

int[] letterToMorseCode(char letter)
{
  
}

void playDot(){
  digitalWrite(lightPin1,1);
  delay(dotLength);
  digitalWrite(lightPin1,0);
}
void playDash(){
  digitalWrite(lightPin1,1);
  delay(dashLength);
  digitalWrite(lightPin1,0);
}

char getNextletter(){
  currentLetterPosition;
  return word[currentLetterPosition];
}

// for(int letter = 0; letter < strlen(word); letter++){

// }
