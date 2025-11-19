/*
<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
|||||||||||||||||||||||||||||||||||||||||||
||        "SS6 - 5 lights and away we go, Serial HandShaking"           ||
||||||||||||||||||||||||||||||||||||||||||| 
  - Ethan Quigley, 2025
  - When reciving "A" from serial port play lights out game
  - When reciving "B" from serial port Rev lights from potentiometer
<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
*/
#include <Arduino.h>

// Potentiometer
const int potPin = A0; 
int potVal = 0;

// 5 Red lights
const int lightPin_1 = 7;
const int lightPin_2 = 6;
const int lightPin_3 = 5;
const int lightPin_4 = 3;
const int lightPin_5 = 2;

// Correct array declaration
const int lights[] = {lightPin_1, lightPin_2, lightPin_3, lightPin_4, lightPin_5};
const int NUM_LIGHTS = 5;

// Go Button
const int buttonPin = 41; 

// --- Global State Variables ---

// Mode control
bool lightsOutMode = false; // True when 'A' mode is active
bool revLightsMode = false; // True when 'B' mode is active
bool gameIsActive = false;  // True when the lights-out sequence is running

// Timing variables for the Lights Out Game (Mode 'A')
unsigned long previousMillis = 0;   // General timing reference
unsigned long interval = 500;       // Interval for sequential light on (500ms)
unsigned long randomWaitTime = 0;   // Random time before lights go out
unsigned long lightsOnStartTime = 0;// Time when the first light turned on
unsigned long lightsOutTime = 0;    // The moment the lights actually went out
int reactionTimeMS = 0;             // Calculated reaction time
bool buttonPressed = false;         // State to track if the button has been pressed
int currentLightIndex = 0;          // Which light in the sequence is currently active

// Timing variable for Rev Lights Update (Mode 'B')
unsigned long revLightPreviousMillis = 0;
const long revLightInterval = 50;   // Update pot/lights every 50ms

int inByte = 0;

// --- Lights Out Game Functions (Mode 'A') ---

/**
 * @brief Initializes the lights sequence parameters.
 */
void initLightsSequence() {
  // 1. Reset state
  gameIsActive = true;
  buttonPressed = false;
  reactionTimeMS = 0;
  currentLightIndex = 0;
  
  // Turn all lights off
  for (int i = 0; i < NUM_LIGHTS; i++) {
    digitalWrite(lights[i], LOW);
  }

  // 2. Set timing parameters
  previousMillis = millis(); 
  lightsOnStartTime = millis(); 
  randomWaitTime = random(1000, 3001); // 1 to 3 seconds wait time
}

/**
 * @brief Manages the sequential turn-on, random wait, and turn-off sequence.
 */
void updateLightsSequence() {
  unsigned long currentMillis = millis();

  // STAGE 1: Sequential Lights On (every 'interval' ms)
  if (currentLightIndex < NUM_LIGHTS) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; 
      
      digitalWrite(lights[currentLightIndex], HIGH); // Turn the next light ON
      currentLightIndex++;
      
      // If the last light just turned on, reset the timer for the random wait
      if (currentLightIndex == NUM_LIGHTS) {
        previousMillis = currentMillis; // Reset timer for random wait stage
      }
    }
    return; // Stay in this stage until all lights are on
  }

  // STAGE 2: Random Wait then Lights Off
  // Check if the random wait time has elapsed since the last light turned on
  if (currentLightIndex == NUM_LIGHTS && currentMillis - previousMillis >= randomWaitTime) {
    
    // Turn lights off
    for (int i = 0; i < NUM_LIGHTS; i++) {
      digitalWrite(lights[i], LOW);
    }
    
    // Record the precise moment the lights went out
    lightsOutTime = currentMillis; 
    Serial.println("GO!");    
    
    // Move to the final stage (waiting for button press)
    currentLightIndex++; // currentLightIndex will now be 6
  }
}


/**
 * @brief Checks for button press and calculates reaction time.
 */
void checkGameResult() {
  // Read the button state (INPUT_PULLUP: LOW means pressed)
  int currentButtonState = digitalRead(buttonPin);

  // Check for button press *after* lights have gone out (currentLightIndex > NUM_LIGHTS)
  if (currentLightIndex > NUM_LIGHTS && (currentButtonState == LOW) && !buttonPressed) {
    buttonPressed = true; // Mark as pressed to only register once

    // Calculate time elapsed since lights went out
    reactionTimeMS = millis() - lightsOutTime;

    // Send the reaction time result back over Serial
    Serial.print("Reaction Time: ");
    Serial.print(reactionTimeMS); 
    Serial.print(" ms");
    Serial.print('\n'); 

    gameIsActive = false; // End the game round
    currentLightIndex = 0; // Reset index
  }
}

// --- Rev Lights/Potentiometer Functions (Mode 'B') ---

/**
 * @brief Reads the potentiometer and lights up LEDs sequentially based on its value.
 */
void updateRevLights() {
  unsigned long currentMillis = millis();

  // Run only when the interval has passed
  if (currentMillis - revLightPreviousMillis >= revLightInterval) {
    revLightPreviousMillis = currentMillis;

    // 1. Read the potentiometer value (0-1023)
    potVal = analogRead(potPin);

    // 2. Map the pot value (0-1023) to the number of lights (0-5)
    int lightsToTurnOn = map(potVal, 0, 1023, 0, NUM_LIGHTS); 
    
    // 3. Control the LEDs and send feedback
    Serial.print("Pot Value: ");
    Serial.print(potVal);
    Serial.print(" -> Lights ON: ");
    Serial.println(lightsToTurnOn);

    for (int i = 0; i < NUM_LIGHTS; i++) {
      if (i < lightsToTurnOn) {
        digitalWrite(lights[i], HIGH); 
      } else {
        digitalWrite(lights[i], LOW); 
      }
    }
  }
}

// --- Setup and Loop ---

void setup() 
{
  // Pin setup
  pinMode(buttonPin, INPUT_PULLUP); 
  for (int i = 0; i < NUM_LIGHTS; i++) {
    pinMode(lights[i], OUTPUT);
    digitalWrite(lights[i], LOW); 
  }
  pinMode(potPin, INPUT); 

  // Serial setup
  Serial.begin(9600);       
  randomSeed(2); // Better seed generation using an unconnected analog pin
  
}

void loop()
{
  // 1. Handle incoming Serial Commands
  if (Serial.available()) 
  {
    inByte = Serial.read(); 

    if (inByte == 'A') // Lights Out Game
    {
      if (!lightsOutMode) {
        
        revLightsMode = false;
        lightsOutMode = true;
      }
      // Note: We don't trigger the game here, the button press does it in the next section.
    }
    else if (inByte == 'B') // Rev Lights/Potentiometer
    {
      if (!revLightsMode) {
        // Reset lights and mode state
        for (int i = 0; i < NUM_LIGHTS; i++) {
          digitalWrite(lights[i], LOW);
        }
        lightsOutMode = false;
        gameIsActive = false;
        currentLightIndex = 0;
        revLightsMode = true;
        Serial.println("Mode B: Rev Lights/Potentiometer Activated.");
      }
    }
    else 
    {
      Serial.println("Rx Byte is neither 'A' nor 'B'. Please send 'A' or 'B'.");
    }
  }

  // 2. Handle Continuous Logic based on Active Mode
  
  if (lightsOutMode) {
    
    // --- Mode A: Lights Out Game ---
    
    if (!gameIsActive) {
       initLightsSequence(); // Initialize timing and state
    } 
    
    if (gameIsActive && currentLightIndex <= NUM_LIGHTS) {
      // Run the non-blocking sequence until the lights turn off
      updateLightsSequence();
    }
    
    if (gameIsActive && currentLightIndex > NUM_LIGHTS) {
      // Run the non-blocking check for reaction time after lights turn off
      checkGameResult();
    }
  } 
  else if (revLightsMode) {
    
    // --- Mode B: Rev Lights/Potentiometer ---
    
    // Update the rev lights using millis() for periodic updates
    updateRevLights();
  }
}