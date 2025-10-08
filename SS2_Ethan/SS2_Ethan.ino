/*
Morse Code Communicator - Prototype Added
By Ethan Quigley
*/
#include <string.h>

// --- FUNCTION PROTOTYPES ---
// These lines tell the compiler about the functions so they can be called later
void playDot();
void playDash();
void readButtonPress();
const char* letterToMorseCode(char letter);
char morseCodeToLetter(String code);

// GLOBAL VARIABLES
// Light Pin
int lightPin1 = 1;

// Button Pin
int buttonPin = 2;

// Potentiometer Pin
int potentiometerPin = 4; // Use an analog pin

// Morse Code Timing
int dotLength = 500; // The length of the dot in milliseconds
int dashLength = dotLength * 3; // The length of the dash in milliseconds
int pauseBetweenSymbolsLength = dotLength;
int pauseBetweenLettersLength = dashLength;
int pauseBetweenWordsLength = dotLength * 7;

// Button Morse Code Variables
long pressStartTime = 0;
long lastPressTime = 0;
long pressDuration = 0;
String currentMorseCode = "";
String receivedWord = "";

// Debounce variable for delay-based method
unsigned int debounceDelay = 50; // The time to wait for the button to settle

// Mode Variables
enum Mode {
  LISTENING_MODE,
  SENDING_MODE
};

Mode currentMode = LISTENING_MODE; // Start in listening mode

void setup() {
  Serial.begin(115200);
  pinMode(lightPin1, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potentiometerPin, INPUT);
  Serial.println("Setup Complete");
  Serial.println("Mode: LISTENING_MODE (Button Input)");
}

void loop() {
  // Read the potentiometer to determine the current mode
  int potValue = analogRead(potentiometerPin);
  if (potValue < 512) {
    if (currentMode != LISTENING_MODE) {
      currentMode = LISTENING_MODE;
      Serial.println("\nMode: LISTENING_MODE (Button Input)");
    }
    readButtonPress();
  } else {
    if (currentMode != SENDING_MODE) {
      currentMode = SENDING_MODE;
      Serial.println("\nMode: SENDING_MODE (Serial Input)");
      Serial.println("Type a word and press Enter.");
    }
    readSerialInput();
  }
}

// Function to read button presses
void readButtonPress() {
  // Button logic goes here
  if (digitalRead(buttonPin) == LOW) {
    delay(debounceDelay);
    pressStartTime = millis();
    while (digitalRead(buttonPin) == LOW) {
      // Wait for the button to be released
    }
    pressDuration = millis() - pressStartTime;

    if (pressDuration > 0) {
      if (pressDuration < dotLength * 1.5) {
        currentMorseCode += '.';
        Serial.print(".");
      } else {
        currentMorseCode += '-';
        Serial.print("-");
      }
    }
    lastPressTime = millis();
  }

  // Check for a long pause to indicate the end of the current letter
  if (currentMorseCode.length() > 0 && millis() - lastPressTime > pauseBetweenLettersLength) {
    char decodedLetter = morseCodeToLetter(currentMorseCode);
    if (decodedLetter != ' ') {
      receivedWord += decodedLetter;
      Serial.print(" -> ");
      Serial.println(decodedLetter);
    }
    currentMorseCode = "";
  }
}

// Function to read data from the serial monitor
void readSerialInput() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    message.trim();
    
    if (message.length() > 0) {
      playSerialMessage(message);
    }
  }
}

// Function to play the Morse code for a single letter from a serial input string
void playSerialMessage(String message) {
  if (message.length() > 0) {
    Serial.print("Sending message: ");
    Serial.println(message);
    
    for (int i = 0; i < message.length(); i++) {
      char currentLetter = message.charAt(i);
      const char* morseCode = letterToMorseCode(currentLetter);
      
      if (morseCode != NULL) {
        Serial.print(currentLetter);
        Serial.print(": ");
        for (int j = 0; morseCode[j] != '\0'; j++) {
          if (morseCode[j] == '.') {
            playDot();
            Serial.print(".");
          } else if (morseCode[j] == '-') {
            playDash();
            Serial.print("-");
          }
          if (morseCode[j+1] != '\0') {
            delay(pauseBetweenSymbolsLength);
          }
        }
        Serial.println();
      } else {
        Serial.print("No Morse code found for letter: ");
        Serial.println(currentLetter);
      }
      delay(pauseBetweenLettersLength);
    }
    Serial.println("Message sent.");
    Serial.println("Waiting for next serial input.");
  }
}

// Function to convert a letter to its Morse code string
const char* letterToMorseCode(char letter) {
  letter = tolower(letter);
  switch (letter) {
    case 'a': return ".-"; case 'b': return "-..."; case 'c': return "-.-.";
    case 'd': return "-.."; case 'e': return "."; case 'f': return "..-.";
    case 'g': return "--."; case 'h': return "...."; case 'i': return "..";
    case 'j': return ".---"; case 'k': return "-.-"; case 'l': return ".-..";
    case 'm': return "--"; case 'n': return "-."; case 'o': return "---";
    case 'p': return ".--."; case 'q': return "--.-"; case 'r': return ".-.";
    case 's': return "..."; case 't': return "-"; case 'u': return "..-";
    case 'v': return "...-"; case 'w': return ".--"; case 'x': return "-..-";
    case 'y': return "-.--"; case 'z': return "--..";
    case '1': return ".----"; case '2': return "..---"; case '3': return "...--";
    case '4': return "....-"; case '5': return "....."; case '6': return "-....";
    case '7': return "--..."; case '8': return "---.."; case '9': return "----.";
    case '0': return "-----"; case ' ': return " ";
    default: return NULL;
  }
}

// Function to convert a Morse code string back to a letter
char morseCodeToLetter(String code) {
  if (code == ".-") return 'a';
  if (code == "-...") return 'b';
  if (code == "-.-.") return 'c';
  if (code == "-..") return 'd';
  if (code == ".") return 'e';
  if (code == "..-.") return 'f';
  if (code == "--.") return 'g';
  if (code == "....") return 'h';
  if (code == "..") return 'i';
  if (code == ".---") return 'j';
  if (code == "-.-") return 'k';
  if (code == ".-..") return 'l';
  if (code == "--") return 'm';
  if (code == "-.") return 'n';
  if (code == "---") return 'o';
  if (code == ".--.") return 'p';
  if (code == "--.-") return 'q';
  if (code == ".-.") return 'r';
  if (code == "...") return 's';
  if (code == "-") return 't';
  if (code == "..-") return 'u';
  if (code == "...-") return 'v';
  if (code == ".--") return 'w';
  if (code == "-..-") return 'x';
  if (code == "-.--") return 'y';
  if (code == "--..") return 'z';
  return ' ';
}

// Function to play a dot
void playDot() {
  digitalWrite(lightPin1, HIGH);
  delay(dotLength);
  digitalWrite(lightPin1, LOW);
}

// Function to play a dash
void playDash() {
  digitalWrite(lightPin1, HIGH);
  delay(dashLength);
  digitalWrite(lightPin1, LOW);
}