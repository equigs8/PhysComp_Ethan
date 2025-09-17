/*
Four LED Homework
By Ethan Quigley
*/

// GLOBAL VARIABLES
// Light Pins. Assign to the pin number on microcontroller

int lightPin1 = 9;
int lightPin2 = 8;
int lightPin3 = 15;
int lightPin4 = 4;

// Word to translate into morse code
char myWord[] = "ethan";
int currentLetterPosition = 0;
bool morseCodeDone = false;

int dotLength = 500; // The length of the dot in milliseconds
int dashLength = dotLength * 3; // The length of the dash in milliseconds
int pauseBetweenSymbolsLength = dotLength;
int pauseBetweenLettersLength = dashLength;
int pauseBetweenWordsLength = dotLength * 7;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Standard baud rate is 9600 for Serial Monitor
  pinMode(lightPin1, OUTPUT);
  pinMode(lightPin2, OUTPUT);
  pinMode(lightPin3, OUTPUT);
  pinMode(lightPin4, OUTPUT);
  Serial.println("Setup Complete");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (morseCodeDone){
    trafficLights();
  }else{
    morseCode();
  }
}

void trafficLights(){
  int redLight = lightPin2;
  int yellowLight = lightPin3;
  int greenLight = lightPin4;

  playLight(redLight, 4000);
  playLight(yellowLight, 1000);
  playLight(greenLight, 5000);
  
  morseCodeDone = false;
}

void playLight(int light, int duration){
  digitalWrite(light, HIGH);
  delay(duration);
  digitalWrite(light, LOW);
}


void morseCode(){
  char currentLetter = myWord[currentLetterPosition];
  playLetter(currentLetter);
  
  if (checkIfEndOfWord()) {
    currentLetterPosition = 0;
    Serial.println("END OF WORD");
    delay(pauseBetweenWordsLength); // Pause before repeating the word
    morseCodeDone = true;
  } else {
    currentLetterPosition++;
    delay(pauseBetweenLettersLength); // Pause between letters
  }
}

// Function to play the Morse code for a single letter
void playLetter(char currentLetter) {
  const char* morseCode = letterToMorseCode(currentLetter);
  
  if (morseCode != NULL) {
    Serial.print("Playing Morse code for letter: ");
    Serial.println(currentLetter);
    for (int i = 0; morseCode[i] != '\0'; i++) {
      if (morseCode[i] == '.') {
        playDot();
      } else if (morseCode[i] == '-') {
        playDash();
      }
      if (morseCode[i+1] != '\0') {
          delay(pauseBetweenSymbolsLength); // Pause between dots and dashes
      }
    }
  } else {
    Serial.print("No Morse code found for letter: ");
    Serial.println(currentLetter);
  }
}

// Function to convert a letter to its Morse code string
const char* letterToMorseCode(char letter) {
  
  letter = tolower(letter);

  switch (letter) {
    case 'a': return ".-";
    case 'b': return "-...";
    case 'c': return "-.-.";
    case 'd': return "-..";
    case 'e': return ".";
    case 'f': return "..-.";
    case 'g': return "--.";
    case 'h': return "....";
    case 'i': return "..";
    case 'j': return ".---";
    case 'k': return "-.-";
    case 'l': return ".-..";
    case 'm': return "--";
    case 'n': return "-.";
    case 'o': return "---";
    case 'p': return ".--.";
    case 'q': return "--.-";
    case 'r': return ".-.";
    case 's': return "...";
    case 't': return "-";
    case 'u': return "..-";
    case 'v': return "...-";
    case 'w': return ".--";
    case 'x': return "-..-";
    case 'y': return "-.--";
    case 'z': return "--..";
    
    // Numbers
    case '1': return ".----";
    case '2': return "..---";
    case '3': return "...--";
    case '4': return "....-";
    case '5': return ".....";
    case '6': return "-....";
    case '7': return "--...";
    case '8': return "---..";
    case '9': return "----.";
    case '0': return "-----";

    // Space character
    case ' ': return " ";
    
    default: return NULL; // Return NULL if character not found in the alphabet
  }
}

// Function to play a dot
void playDot() {
  digitalWrite(lightPin1, HIGH); // Use HIGH/LOW instead of 1/0
  delay(dotLength);
  digitalWrite(lightPin1, LOW);
}

// Function to play a dash
void playDash() {
  digitalWrite(lightPin1, HIGH);
  delay(dashLength);
  digitalWrite(lightPin1, LOW);
}

// Function to check if we are at the end of the word
bool checkIfEndOfWord() {
  return (currentLetterPosition >= sizeof(myWord) - 2); // -2 because of null terminator and 0-based index
}