// --- HARDWARE CONFIGURATION ---
const int BUZZER_PIN = 4;
const int BUZZER_CHANNEL = 0;
const int BAUD_RATE = 9600; 


const int STOP_NOTE = 255; 

// --- FREQUENCY MAPPING (C4 to B4) ---
// Maps Note ID (0-11) to Frequency (Hz)
const int piano_frequencies[] = {
    262, // 0: C4
    277, // 1: C#4
    294, // 2: D4
    311, // 3: D#4
    330, // 4: E4
    349, // 5: F4
    370, // 6: F#4
    392, // 7: G4
    415, // 8: G#4
    440, // 9: A4
    466, // 10: A#4
    494  // 11: B4
};

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Serial Piano Ready. Awaiting data from p5.js...");

  // Initialize the buzzer pin with the tone function's LEDC settings
  // This is required for tone() on ESP32 to work correctly
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

void loop() {
  // Check if new data is available on the serial port
  if (Serial.available() > 0) {
    // Read the single incoming byte (Note ID or STOP_NOTE)
    int incomingByte = Serial.read(); 

    if (incomingByte == STOP_NOTE) {
      // Received the STOP signal (255)
      ledcDetachPin(BUZZER_PIN); // Stops the PWM signal cleanly
      // Serial.println("Note OFF"); // Optional feedback
    } 
    else if (incomingByte >= 0 && incomingByte <= 11) {
      // Received a valid Note ID (0-11)
      int noteID = incomingByte;
      int frequency = piano_frequencies[noteID];
      
      // Start the tone using the specified channel and frequency
      // The duration argument is typically 0 to play indefinitely until stopped
      tone(BUZZER_PIN, frequency); 
      // Serial.print("Playing Note ID: "); // Optional feedback
      // Serial.println(noteID);
    }
  }
}