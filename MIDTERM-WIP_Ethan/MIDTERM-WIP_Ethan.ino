#include <string.h>
#include <esp_now.h>
#include <WiFi.h>

// --- FUNCTION PROTOTYPES ---
void playDot();
void playDash();
void readButtonPress();
const char* letterToMorseCode(char letter);
char morseCodeToLetter(String code);
void SwitchModeLight();

// GLOBAL VARIABLES
// Light Pins
int lightPin1 = 1;
int listeningModePin = 10;
int sendingModePin = 11;

// Buzzer Pin
int buzzerPin = 12;

// Button Pin
int buttonPin = 2;

// Potentiometer Pin
int potentiometerPin = 4;

// Morse Code Timing
int dotLength = 500;
int dashLength = dotLength * 3;
int pauseBetweenSymbolsLength = dotLength;
int pauseBetweenLettersLength = dashLength;
int pauseBetweenWordsLength = dotLength * 7;

// Button Morse Code Variables
long pressStartTime = 0;
long lastPressTime = 0;
long pressDuration = 0;
String currentMorseCode = "";
String receivedWord = "";
unsigned int debounceDelay = 50;

bool connectedTo = false;


// Mode Variables
enum Mode {
  LISTENING_MODE,
  SENDING_MODE
};
Mode currentMode;

// --- IMPORTANT: REPLACE WITH YOUR BOARD'S MAC ADDRESSES ---
// You must change these for each board.
uint8_t MY_MAC_ADDRESS[] = {0x98,0xA3,0x16,0xf0,0x82,00};
uint8_t RECEIVER_MAC_ADDRESS[] = {0x8C,0xBF,0xEA,0x1B,0x8A,0x40};

// uint8_t RECEIVER_MAC_ADDRESS[] = {0x98,0xA3,0x16,0xf0,0x82,00};
// uint8_t MY_MAC_ADDRESS[] = {0x8C,0xBF,0xEA,0x1B,0x8A,0x40};



void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi in Station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    //return;
  }

  // Register callback functions
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, RECEIVER_MAC_ADDRESS, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    connectedTo = false;
    //return;
  }
  connectedTo = true;

  pinMode(lightPin1, OUTPUT);
  pinMode(listeningModePin, OUTPUT);
  pinMode(sendingModePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potentiometerPin, INPUT);

  Serial.println("Setup Complete");
}

void loop() {
  //delay(100);
  int potValue = analogRead(potentiometerPin);
  // int potValue = 0;
  //Serial.println(potValue);
  if (potValue < 512) {
    if (currentMode != LISTENING_MODE) {
      currentMode = LISTENING_MODE;
      Serial.println("\nMode: LISTENING_MODE (ESP-NOW Receiver)");
      SwitchModeLight();
    }
    //readButtonPress();
  } else {
    if (currentMode != SENDING_MODE) {
      currentMode = SENDING_MODE;
      Serial.println("\nMode: SENDING_MODE (Button Sender)");
      SwitchModeLight();
    }
    readButtonPress();
  }
}

void SwitchModeLight(){
  if(currentMode == LISTENING_MODE){
    digitalWrite(listeningModePin, HIGH);
    digitalWrite(sendingModePin, LOW);
  }else{
    digitalWrite(listeningModePin, LOW);
    digitalWrite(sendingModePin, HIGH);
  }
}

// Function to read button presses
void readButtonPress() {
  if (digitalRead(buttonPin) == LOW) {
    delay(debounceDelay);
    pressStartTime = millis();
    //Serial.println("before while loop");
    while (digitalRead(buttonPin) == LOW) {
      // Wait for the button to be released
    }
    //Serial.println("After the while loop");
    pressDuration = millis() - pressStartTime;
    if (pressDuration > 0) {
      if (pressDuration < dotLength * 1.5) {
        char symbolToSend = '.';
        // if(connectedTo){
        //   esp_now_send(RECEIVER_MAC_ADDRESS, (uint8_t *)&symbolToSend, 1);
        // }
        currentMorseCode += '.';
        
        Serial.print(".");
      } else {
        char symbolToSend = '-';
        // if(connectedTo){
        //   esp_now_send(RECEIVER_MAC_ADDRESS, (uint8_t *)&symbolToSend, 1);
        // }
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
    
    if(connectedTo){
      esp_now_send(RECEIVER_MAC_ADDRESS, (uint8_t *)&currentMorseCode,currentMorseCode.length());
    }
    currentMorseCode = "";
  }
}



// Callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (currentMode == LISTENING_MODE) {
    char symbol = (char)data[0];
    Serial.print("Received: ");
    Serial.println(symbol);
    if (symbol == '.') {
      playDot();
    } else if (symbol == '-') {
      playDash();
    }
  }
}

// Callback function that will be executed when data is sent
void OnDataSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
// Function to play a dot
void playDot() {
  digitalWrite(lightPin1, HIGH);
  tone(buzzerPin, 1000); // Play a tone at 1000 Hz
  delay(dotLength);
  digitalWrite(lightPin1, LOW);
  noTone(buzzerPin); // Stop the tone
}

// Function to play a dash
void playDash() {
  digitalWrite(lightPin1, HIGH);
  tone(buzzerPin, 1000); // Play a tone at 1000 Hz
  delay(dashLength);
  digitalWrite(lightPin1, LOW);
  noTone(buzzerPin); // Stop the tone
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