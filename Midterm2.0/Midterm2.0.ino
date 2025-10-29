#include <string.h>

// --- FUNCTION PROTOTYPES ---
// These lines tell the compiler about the functions so they can be called later
void playDot();
void playDash();
void readButtonPress();
const char* letterToMorseCode(char letter);
char morseCodeToLetter(String code);

// GLOBAL VARIABLES
// Light Pins
int lightPin1 = 1;
int listeningModePin = 10;
int sendingModePin = 11;

// Button Pin
int buttonPin = 2;

// Potentiometer Pin
int potentiometerPin = 8; // Use an analog pin

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
String sentWord = "";

// Debounce variable for delay-based method
unsigned int debounceDelay = 50; // The time to wait for the button to settle

#include <esp_now.h>
#include <WiFi.h>


uint8_t broadcastAddress[] = {0x80, 0xb5, 0x4e, 0xe4, 0x4a, 0xcc};
uint8_t broadcastAddress2[] = {0x98, 0xa3, 0x16, 0xf0, 0x82, 0x00};

typedef struct message{
  char a[32];
} message;

message myMessage;

esp_now_peer_info_t peerInfo;

void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status){
  Serial.print("PACKET SEND STATUS:");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Mode Variables
enum Mode {
  SENDING_MODE,
  RECEIVING_MODE
};

Mode currentMode = SENDING_MODE; // Start in listening mode

void setup() {
  //Init Serial at 115200
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  ESPNowSetup();

  pinMode(lightPin1, OUTPUT);
  pinMode(listeningModePin, OUTPUT);
  pinMode(sendingModePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potentiometerPin, INPUT);

  Serial.println("Setup Complete");
  Serial.println("Mode: SENDING_MODE (Button Input)");
}

void loop() {
  // Read the potentiometer to determine the current mode
  int potValue = analogRead(potentiometerPin);
  //Serial.println(potValue);
  if (potValue < 2000) {
    if (currentMode != SENDING_MODE) {
      currentMode = SENDING_MODE;
      Serial.println("\nMode: SENDING_MODE (Button Input)");
      SwitchModeLight();
    }
    readButtonPress();
  } else {
    if (currentMode != RECEIVING_MODE) {
      currentMode = RECEIVING_MODE;
      Serial.println("\nMode: RECEIVING_MODE (Serial Input)");
      Serial.println("Type a word and press Enter.");
      SwitchModeLight();
    }
    //readSerialInput();
  }
}

void ESPNowSetup(){
  if (esp_now_init() != ESP_OK){
    Serial.println("ERROR");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress,6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("FAILED TO ADD PEER");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void OnDataRecv(const esp_now_recv_info * mac, const unsigned char * incomingData, int len){

  memcpy(&myMessage, incomingData,sizeof(myMessage));


  Serial.print("Message Received: ");
  Serial.println(myMessage.a);

  playReceivedMorse(myMessage.a);
}





void SwitchModeLight(){
  if(currentMode ==  SENDING_MODE){
    digitalWrite(listeningModePin, HIGH);
    digitalWrite(sendingModePin, LOW);
  }else{
    digitalWrite(listeningModePin, LOW);
    digitalWrite(sendingModePin, HIGH);
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
      // Serial.println("IN THE WHILE LOOP");
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

  if(receivedWord.length() > 0 && millis() - lastPressTime > pauseBetweenWordsLength && sentWord != receivedWord){
    Serial.print("Sending word: ");
    Serial.println(receivedWord);

    strcpy(myMessage.a, receivedWord.c_str());

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myMessage, sizeof(myMessage));

    if (result == ESP_OK){
      Serial.println("Sending Confirmened");
      sentWord = receivedWord;
      receivedWord = "";
    }else{
      Serial.println("Sending Error");
    }
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

void playReceivedMorse(const char* message){
  Serial.print("Playing Morse Code: ");
  Serial.println(message);

  for (int i = 0; message[i] != '\0'; i++){
    char currentLetter = tolower(message[i]);
    const char* moseCode = Null;
    switch (currentLetter) {
            case 'a': morseCode = ".-"; break; case 'b': morseCode = "-..."; break; case 'c': morseCode = "-.-."; break;
            case 'd': morseCode = "-.."; break; case 'e': morseCode = "."; break; case 'f': morseCode = "..-."; break;
            case 'g': morseCode = "--."; break; case 'h': morseCode = "...."; break; case 'i': morseCode = ".."; break;
            case 'j': morseCode = ".---"; break; case 'k': morseCode = "-.-"; break; case 'l': morseCode = ".-.."; break;
            case 'm': morseCode = "--"; break; case 'n': morseCode = "-."; break; case 'o': morseCode = "---"; break;
            case 'p': morseCode = ".--."; break; case 'q': morseCode = "--.-"; break; case 'r': morseCode = ".-."; break;
            case 's': morseCode = "..."; break; case 't': morseCode = "-"; break; case 'u': morseCode = "..-"; break;
            case 'v': morseCode = "...-"; break; case 'w': morseCode = ".--"; break; case 'x': morseCode = "-..-"; break;
            case 'y': morseCode = "-.--"; break; case 'z': morseCode = "--.."; break;
            case '1': morseCode = ".----"; break; case '2': morseCode = "..---"; break; case '3': morseCode = "...--"; break;
            case '4': morseCode = "....-"; break; case '5': morseCode = "....."; break; case '6': morseCode = "-...."; break;
            case '7': morseCode = "--..."; break; case '8': morseCode = "---.."; break; case '9': morseCode = "----."; break;
            case '0': morseCode = "-----"; break;
            case ' ': // Handle space/pause between words
                delay(pauseBetweenWordsLength);
                continue; // Skip the rest of the loop for this character
            default:
                Serial.print("Skipping unknown character: ");
                Serial.println(currentLetter);
                continue;
        }


    // Play the Morse code symbols for the letter
    for (int j = 0; morseCode[j] != '\0'; j++) {
        if (morseCode[j] == '.') {
            playDot();
        } else if (morseCode[j] == '-') {
            playDash();
        }
        // Pause between symbols within a letter
        if (morseCode[j+1] != '\0') {
            delay(pauseBetweenSympbolsLength);
        }
    }
    
    // Pause between letters
    if (message[i+1] != '\0') {
        delay(pauseBetweenLettersLength);
    }
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
