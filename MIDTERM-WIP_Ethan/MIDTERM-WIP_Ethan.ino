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

// Mode Variables
enum Mode {
  LISTENING_MODE,
  SENDING_MODE
};
Mode currentMode;

// --- IMPORTANT: REPLACE WITH YOUR BOARD'S MAC ADDRESSES ---
// You must change these for each board.
uint8_t MY_MAC_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t RECEIVER_MAC_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi in Station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
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
    return;
  }

  pinMode(lightPin1, OUTPUT);
  pinMode(listeningModePin, OUTPUT);
  pinMode(sendingModePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potentiometerPin, INPUT);

  Serial.println("Setup Complete");
}

void loop() {
  delay(100);
  int potValue = analogRead(potentiometerPin);

  if (potValue < 512) {
    if (currentMode != LISTENING_MODE) {
      currentMode = LISTENING_MODE;
      Serial.println("\nMode: LISTENING_MODE (ESP-NOW Receiver)");
      SwitchModeLight();
    }
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
    while (digitalRead(buttonPin) == LOW) {
      // Wait for the button to be released
    }
    pressDuration = millis() - pressStartTime;
    if (pressDuration > 0) {
      if (pressDuration < dotLength * 1.5) {
        char symbolToSend = '.';
        esp_now_send(RECEIVER_MAC_ADDRESS, (uint8_t *)&symbolToSend, 1);
        Serial.print(".");
      } else {
        char symbolToSend = '-';
        esp_now_send(RECEIVER_MAC_ADDRESS, (uint8_t *)&symbolToSend, 1);
        Serial.print("-");
      }
    }
  }
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