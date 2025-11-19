let serial;
let portName = 'COM3';  // fill in your serial port name here
let options = { baudRate: 9600}; // change the baud rate to match your Arduino code
let isConnectedToDivice = false;

// --- UI & State Variables ---
let currentMode = 'A'; // 'A' for Lights Out, 'B' for Rev Lights
let modeToggle;        // p5.Element for the toggle button
let gameStartButton;   // p5.Element for the game start button

// --- Game/Data Variables ---
let reactionTime = '---'; // Holds the result from Mode 'A'
let potLightCount = 0;    // Holds the number of lights ON from Mode 'B'
let feedbackMessage = 'Press the button below to start the game.'; // Dynamic message display
let modeMessage = 'Current Mode: Lights Out (A)';
let gameStarted = false;
let timeRecived = 0;

let trimmedData = [0,0,0,0];

// --- P5 Setup ---
function setup() {
    createCanvas(700, 400);
    textSize(20);
    textAlign(CENTER, CENTER);
    
    // Initialize P5 SerialPort
    serial = new p5.SerialPort(); 
    serial.on('list', printList); 
    serial.on('connected', serverConnected); 
    serial.on('open', portOpen); 
    serial.on('data', serialEvent); 
    serial.on('error', serialError); 
    serial.on('close', portClose); 
    serial.list(); 
    serial.open(portName, options);

    // --- UI Element Setup ---

    // 1. Mode Toggle Button
    modeToggle = createButton('Switch to Rev Lights (B)');
    modeToggle.position(width / 2 - 100, height - 70);
    modeToggle.size(200, 40);
    modeToggle.mousePressed(toggleMode);
    
    // 2. Game Start Button (Starts the 'A' mode sequence)
    gameStartButton = createButton('START Lights Out Game');
    gameStartButton.position(width / 2 - 150, height / 2 + 50);
    gameStartButton.size(300, 60);
    gameStartButton.mousePressed(startGame);
}

// --- P5 Draw Loop ---
function draw() {
    background(220);
    
    // Connection Status
    fill(isConnected ? color(0, 150, 0) : color(150, 0, 0));
    rect(10, 10, 15, 15);
    fill(0);
    textSize(14);
    textAlign(LEFT);
    text('Serial Status', 30, 17);
    
    // --- Display Mode and Data ---
    textAlign(CENTER);
    textSize(24);
    text(modeMessage, width / 2, 50);
    
    if (currentMode === 'A') {
        // Mode A: Lights Out Game Display
        textSize(32);
        fill(0);
        text('Reaction Time:', width / 2, 120);
        
        textSize(64);
        fill(50, 50, 200); // Blue for time
        text(reactionTime + ' ms', width / 2, 200);
        
        textSize(18);
        fill(50);
        text(feedbackMessage, width / 2, 300);
        
    } else {
        // Mode B: Rev Lights Display
        textSize(32);
        fill(0);
        text('Potentiometer Value:', width / 2, 120);
        
        textSize(64);
        fill(200, 50, 50); // Red for lights
        text(potLightCount + ' / 5 Lights ON', width / 2, 200);

        textSize(18);
        fill(50);
        text('Rotate the physical potentiometer to change the lights.', width / 2, 300);
    }
    
    // Update button visibility based on mode
    gameStartButton.style('display', currentMode === 'A' ? 'block' : 'none');
}

// --- UI Interaction Functions ---

function toggleMode() {
    if (currentMode === 'A') {
        currentMode = 'B';
        modeMessage = 'Current Mode: Rev Lights (B)';
        modeToggle.html('Switch to Lights Out (A)');
        // Send 'B' to Arduino to activate Rev Lights mode
        if (isConnected) serial.write('B');
    } else {
        currentMode = 'A';
        modeMessage = 'Current Mode: Lights Out (A)';
        modeToggle.html('Switch to Rev Lights (B)');
        // Send 'A' to Arduino to activate Lights Out mode
        
    }
    reactionTime = '---'; // Clear display on mode switch
    potLightCount = 0;
}

function startGame() {
    if (currentMode === 'A' && isConnected && !gameStarted) {
        gameStarted = true;
        // Send 'A' to Arduino to confirm the mode and start listening for the button press
        serial.write('A'); 
        
        // This button press corresponds to the physical button press logic in the Arduino sketch
        // which triggers initLightsSequence().
        feedbackMessage = 'Sequence started! Waiting for "GO!"...';
    } else if (currentMode === 'B') {
        alert("Game Start button is only active in Lights Out Mode (A).");
    } else {
        alert("Not connected to Arduino. Please check your port name and connection.");
    }
}

// --- Serial Event Functions ---

function serialEvent() {
    let inData = serial.readStringUntil('\n'); // Read the most recent data line
    if (inData.length > 0) {
        print("INCOMING DATA: " + inData);
        trimmedData = inData.split(',');
        if (currentMode === 'A') {
            // Data for Lights Out Game
            if (trimmedData[0] === "Reaction Time:") {
                timeRecived = trimmedData[1];
                reactionTime = timeRecived;
            } else if (trimmedData === "GO!") {
                feedbackMessage = "GO! Press the physical button NOW!";
            }
        } 
        
        else if (currentMode === 'B') {
            // Data for Rev Lights
            if (trimmedData.startsWith(" -> Lights ON:")) {
                let parts = trimmedData.split(" -> Lights ON: ");
                if (parts.length > 1) {
                    // Extract the last number (the count)
                    potLightCount = parseInt(parts[1].trim());
                }
            }
        }
    }
}

function serverConnected() {
    print("CONNECTED TO SERIAL SERVER");
}

function portOpen() {
    print("SERIAL PORT OPEN");
    isConnected = true;
    // Send initial mode command
    serial.write(currentMode);
}

function portClose() {
    print("SERIAL PORT CLOSED");
    isConnected = false;
}

function serialError(err) {
    print('SERIAL ERROR: ' + err);
}

function printList(portList) {
    print("List of Available Serial Ports: ");
    for (let i = 0; i < portList.length; i++) {
        print(i + portList[i]); 
    }
}