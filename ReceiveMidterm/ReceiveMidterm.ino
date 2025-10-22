#include <esp_now.h>
#include <WiFi.h>

// GLOBAL VARIABLES for Morse Code Timing
// Adjust these to match the sender's timing for accurate decoding
const int DOT_LENGTH = 150;    // The length of the dot in milliseconds (adjusted for quicker visual feedback)
const int DASH_LENGTH = DOT_LENGTH * 3;
const int PAUSE_BETWEEN_SYMBOLS = DOT_LENGTH;
const int PAUSE_BETWEEN_LETTERS = DASH_LENGTH;
const int PAUSE_BETWEEN_WORDS = DOT_LENGTH * 7;

// Light Pin
int lightPin = 1;

uint8_t broadcastAddress[] = {0x98, 0xa3, 0x16, 0xf0, 0x82, 0x00};

typedef struct message{
  char a[32];
} message;

message myMessage;

// --- FUNCTION PROTOTYPES ---
void playDot();
void playDash();
void playReceivedMorse(const char* message);

// Function to play a dot
void playDot() {
  Serial.println("DOT");
  digitalWrite(lightPin, HIGH);
  delay(DOT_LENGTH);
  digitalWrite(lightPin, LOW);
}

// Function to play a dash
void playDash() {
  digitalWrite(lightPin, HIGH);
  delay(DASH_LENGTH);
  digitalWrite(lightPin, LOW);
}

// Function to play the received message as Morse code
void playReceivedMorse(const char* message) {
    Serial.print("Playing Morse: ");
    Serial.println(message);

    for (int i = 0; message[i] != '\0'; i++) {
        char currentLetter = tolower(message[i]);

        // Look up the Morse code for the character
        const char* morseCode = NULL;
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
                delay(PAUSE_BETWEEN_WORDS);
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
                delay(PAUSE_BETWEEN_SYMBOLS);
            }
        }
        
        // Pause between letters
        if (message[i+1] != '\0') {
            delay(PAUSE_BETWEEN_LETTERS);
        }
    }
    Serial.println("Morse playback complete.");
}

void OnDataRecv(const esp_now_recv_info * mac, const unsigned char * incomingData, int len){

  memcpy(&myMessage, incomingData,sizeof(myMessage));


  Serial.print("Message Received: ");
  Serial.println(myMessage.a);

  playReceivedMorse(myMessage.a);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(lightPin, OUTPUT);

  if(esp_now_init() != ESP_OK){
    Serial.println("ERROR initializing ESP-NOW");
    return;
  }

  Serial.println("ESP-NOW initialized. Waiting for messages...");
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}