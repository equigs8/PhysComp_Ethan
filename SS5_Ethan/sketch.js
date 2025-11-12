// --- SERIAL SETUP (Keep as is) ---
let serial;
let portName = 'COM3';  // fill in your serial port name here
let options = { baudRate: 9600}; 

let isConnectedToDivice = false; // Added missing global declaration

function setup() {
  serial = new p5.SerialPort();
  
  serial.on('list', printList);
  serial.on('connected', serialConnected);
  serial.on('open', serialOpened);
  serial.on('data', serialEvent);
  serial.on('error', serialError);
  serial.on('close', serialClosed);


  serial.list();
  serial.open(portName, options);

  p5setup();
}

function serialEvent() { /* only sending data in this sketch */ }

function printList(portList){
  print("List of Available Serial Ports: ");
  for (var i = 0; i < portList.length; i++) {
    print(i + portList[i]);
  }
}

function serialConnected(port) {
  console.log('Connected to ' + port);
  isConnectedToDivice = true;
}

function serialOpened() {
  console.log('Serial Port Opened');
  isConnectedToDivice = true;
}

function serialError(err) {
  console.log('Something went wrong with the serial port. ' + err);
  isConnectedToDivice = false;
}

function serialClosed() {
  console.log('The serial port closed.');
  isConnectedToDivice = false;
}

// --- GLOBAL VARIABLES & MAPPINGS ---
let whiteKeyWidth;
let whiteKeyHeight;
let blackKeyWidth;
let blackKeyHeight;
let toggleSwitch;
let showMusicalNotes = false;
let toggleXPos;
let targetToggleX;

let whiteKeyStates = [];
let blackKeyStates = [];

// Note names for mapping
const whiteNotes = ["C", "D", "E", "F", "G", "A", "B"];
const blackNotes = ["C#/Db", "D#/Eb", "F#/Gb", "G#/Ab", "A#/Bb"];
const STOP_NOTE = 255;

let isMouseKeyDown = false;

// KeyCode (A=65, S=83, J=74, W=87, U=85, etc.) to Note ID (0-11)
const keyCodeToNoteID = {
  65: 0, 83: 2, 68: 4, 70: 5, 71: 7, 72: 9, 74: 11, // A, S, D, F, G, H, J (White Keys)
  87: 1, 69: 3, 84: 6, 89: 8, 85: 10 // W, E, T, Y, U (Black Keys)
};

// KeyCodes needed for the continuous keyIsDown check (for visual updates)
const allKeyCodes = [
    65, 83, 68, 70, 71, 72, 74, 
    87, 69, 84, 89, 85        
];

// Mapping Note ID to Visual Array Index (0-6 for white, 0-4 for black)
const noteIDToWhiteIndex = { 0: 0, 2: 1, 4: 2, 5: 3, 7: 4, 9: 5, 11: 6 };
const noteIDToBlackIndex = { 1: 0, 3: 1, 6: 2, 8: 3, 10: 4 };

// Mapping for the continuous visual check (needed for handleVisuals)
const visualMap = {
    65: [true, 0], 83: [true, 1], 68: [true, 2], 70: [true, 3], 71: [true, 4], 72: [true, 5], 74: [true, 6],
    87: [false, 0], 69: [false, 1], 84: [false, 2], 89: [false, 3], 85: [false, 4]
};


// --- CENTRAL SERIAL HANDLER (NEW FUNCTION) ---
function sendSerialNote(noteID, isPress) {
    console.log(noteID);
    if (isConnectedToDivice) { // Only send if the serial port is open
        if (isPress) {
            // Send the Note ID (0-11)
            serial.write(noteID);
        } else {
            // Send the STOP signal (255)
            serial.write(STOP_NOTE);
        }
    }
}


function p5setup() {
  createCanvas(700, 400);

  const numWhiteKeys = 7;
  whiteKeyWidth = width / numWhiteKeys;
  whiteKeyHeight = height - 40;
  blackKeyWidth = whiteKeyWidth * 0.6;
  blackKeyHeight = whiteKeyHeight * 0.6;

  // Initialize 7 white key states
  for (let i = 0; i < numWhiteKeys; i++) {
    whiteKeyStates[i] = { offset: 0, color: 255 }; 
  }
  // Initialize 5 black key states
  for (let i = 0; i < 5; i++) {
    blackKeyStates[i] = { offset: 0, color: 0 }; 
  }

  toggleSwitch = {
    x: 50, y: height - 30, w: 600, h: 20
  };

  let boxW = toggleSwitch.w / 2;
  targetToggleX = toggleSwitch.x + boxW; 
  toggleXPos = targetToggleX;
}


// --- VISUAL HANDLER (Handles continuous keyIsDown check) ---
function handleVisuals() {
    if (isMouseKeyDown) return;

    for (const code of allKeyCodes) {
        const mapping = visualMap[code];
        if (!mapping) continue; 

        const isWhite = mapping[0];
        const index = mapping[1];
        
        const pressedColor = isWhite ? color(200, 200, 255) : color(50, 50, 50);
        const releasedColor = isWhite ? 255 : 0;
        const stateArray = isWhite ? whiteKeyStates : blackKeyStates;

        // Check if key is DOWN
        if (keyIsDown(code)) {
            if (stateArray[index].offset === 0) {
                stateArray[index].offset = 5;
                stateArray[index].color = pressedColor;
            }
        } else {
            // Key is NOT down: reset visual state
            if (stateArray[index].offset === 5) {
                stateArray[index].offset = 0;
                stateArray[index].color = releasedColor;
            }
        }
    }
}


function draw(){
  background(240);
  toggleXPos = lerp(toggleXPos, targetToggleX, 0.2);
  
  // Update key visuals every frame
  handleVisuals(); 

  drawKeyboard();
  drawToggleSwitch();
}

// --- DRAWING FUNCTIONS ---

function drawKeyboard() {
  // Key Index to Key Char Map (for display only)
  const whiteIndexToKey = { 0: 'A', 1: 'S', 2: 'D', 3: 'F', 4: 'G', 5: 'H', 6: 'J' };
  const blackIndexToKey = { 0: 'W', 1: 'E', 2: 'T', 3: 'Y', 4: 'U' };
  
  // 1. Draw White Keys
  for (let i = 0; i < whiteNotes.length; i++) {
    let x = i * whiteKeyWidth;
    let y = 0 + whiteKeyStates[i].offset;

    fill(whiteKeyStates[i].color);
    stroke(0);
    strokeWeight(1);
    rect(x, y, whiteKeyWidth, whiteKeyHeight - whiteKeyStates[i].offset);

    fill(0);
    textAlign(CENTER, BOTTOM);
    textSize(18);
    let label = showMusicalNotes ? whiteNotes[i] : whiteIndexToKey[i];
    text(label, x + whiteKeyWidth / 2, whiteKeyHeight - 10);
  }

  // 2. Draw Black Keys
  const whiteKeyIndexForPosition = [0, 1, 3, 4, 5];
  for (let i = 0; i < 5; i++) {
    let whiteIndex = whiteKeyIndexForPosition[i];
    let centerOfSpace = (whiteIndex + 0.5) * whiteKeyWidth;
    let blackX = centerOfSpace - blackKeyWidth / 2;
    let blackY = 0 + blackKeyStates[i].offset;

    fill(blackKeyStates[i].color);
    noStroke();
    rect(blackX, blackY, blackKeyWidth, blackKeyHeight - blackKeyStates[i].offset);

    fill(255);
    textAlign(CENTER, TOP);
    textSize(14);
    let label = showMusicalNotes ? blackNotes[i] : blackIndexToKey[i];
    text(label, blackX + blackKeyWidth / 2, 5 + blackKeyStates[i].offset);
  }
}

function drawToggleSwitch() {
    let { x, y, w, h } = toggleSwitch;
    let textY = y + h / 2 + 2;
    let boxW = w / 2; 
    let highlightX = toggleXPos;

    noFill();
    stroke(100);
    rect(x, y, w, h, 5); 

    fill(125,206,250);

    // Dynamic corner rounding for the sliding animation effect
    let cornerTL = (highlightX < x + 5) ? 5 : 0;
    let cornerTR = (highlightX > x + boxW - 5) ? 5 : 0;
    let cornerBL  = (highlightX < x + 5) ? 5 : 0;
    let cornerBR = (highlightX > x + boxW - 5) ? 5 : 0;

    rect(highlightX, y, boxW, h, cornerTL, cornerTR, cornerBR, cornerBL);

    fill(0);
    textSize(16);
    textAlign(CENTER, CENTER);
    text("Musical Notes", x + boxW / 2, textY);
    text("Key Bindings", x + boxW + boxW / 2, textY);
}


// --- INTERACTION HANDLERS (Simplified for serial) ---

function keyPressed() {
    const code = keyCode;

    if (code in keyCodeToNoteID) {
        const noteID = keyCodeToNoteID[code];
        sendSerialNote(noteID, true); // Send ON signal via central handler
        return false;
    }
}

function keyReleased() {
    const code = keyCode;

    if (code in keyCodeToNoteID) {
        const noteID = keyCodeToNoteID[code];
        sendSerialNote(noteID, false); // Send OFF signal via central handler
        return false;
    }
}

function mousePressed() {
  // 1. TOGGLE SWITCH LOGIC
  let { x, y, w, h } = toggleSwitch;
  let boxW = w / 2;
  if (mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h) {
    showMusicalNotes = !showMusicalNotes; 
    
    if (showMusicalNotes) {
      targetToggleX = x;
    } else {
      targetToggleX = x + boxW;
    }
    return;
  }
  
  // 2. KEY PRESS LOGIC (Visuals and Serial)
  
  const blackKeyNoteIDs = [1, 3, 6, 8, 10];
  const whiteKeyIndexForPosition = [0, 1, 3, 4, 5];
  
  // Check Black Keys
  for (let i = 0; i < 5; i++) {
    let whiteKeyIndex = whiteKeyIndexForPosition[i];
    let centerOfSpace = (whiteKeyIndex + 0.5) * whiteKeyWidth;
    let kX = centerOfSpace - blackKeyWidth / 2;
    let kY = 0;

    if (mouseX >= kX && mouseX <= kX + blackKeyWidth && mouseY >= kY && mouseY <= kY + blackKeyHeight) {
      // MANUAL VISUAL STATE CHANGE (FIX)
      blackKeyStates[i].offset = 5; 
      blackKeyStates[i].color = color(50, 50, 50); 
      
      // Send serial signal
      sendSerialNote(blackKeyNoteIDs[i], true);
      isMouseKeyDown = true;
      return; 
    }
  }

  // Check White Keys
  const whiteKeyNoteIDs = [0, 2, 4, 5, 7, 9, 11];
  for (let i = 0; i < whiteNotes.length; i++) {
    let kX = i * whiteKeyWidth;
    
    if (mouseX >= kX && mouseX <= kX + whiteKeyWidth && mouseY >= 0 && mouseY <= whiteKeyHeight) {
      // MANUAL VISUAL STATE CHANGE (FIX)
      whiteKeyStates[i].offset = 5;
      whiteKeyStates[i].color = color(200, 200, 255); 
      
      // Send serial signal
      sendSerialNote(whiteKeyNoteIDs[i], true);
      isMouseKeyDown = true;
      return;
    }
  }
}

function mouseReleased() {
  // Send STOP signal for sound (Correct)
  sendSerialNote(STOP_NOTE, false); 

  isMouseKeyDown = false;
  
  // MANUAL VISUAL RESET (FIX)
  // We need to reset the visuals of ALL keys because the mouse doesn't know 
  // which key it released. This ensures the click animation instantly disappears.
  
  // Reset White keys
  for (let i = 0; i < whiteKeyStates.length; i++) {
    whiteKeyStates[i].offset = 0;
    whiteKeyStates[i].color = 255;
  }
  // Reset Black keys
  for (let i = 0; i < blackKeyStates.length; i++) {
    blackKeyStates[i].offset = 0;
    blackKeyStates[i].color = 0;
  }
}