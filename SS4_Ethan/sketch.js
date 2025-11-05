// A p5.js sketch that implemments the game of pong using controls from an esp32 board

let screenHeight = 500;
let screenWidth = 500;
let centerX = screenWidth/2;
let centerY = screenHeight/2;

let serial;
let portName = 'COM3';  // fill in your serial port name here
let options = { baudRate: 9600}; // change the baud rate to match your Arduino code
let isConnectedToDivice = false;

let inData;
let sensors = [];

let paddle1Position = 20;
let paddle2Position = screenWidth - 20;

let paddle1Xposition = 0;
let paddle2Xposition = 0;

let paddleSpeed = 5;

let ballX = 0;
let ballY = 0;

let ballVelocityX = 1;
let ballVelocityY = -.25;

let ballSpeed = 2.5;

let pointScored = false;

let player1Score = 0;
let player2Score = 0;

let gamePaused = false;
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
function serialEvent() //gets called when new data arrives
{
  let inString = serial.readStringUntil('\n'); // read until newline character
  if (inString.length > 0) //if there's data in the string
  {
    sensors = split(inString, ','); // split the string at commas and store in array
    
    for (let i = 0; i < sensors.length; i++) 
    {
      // Number() function converts string to number
      sensors[i] = Number(sensors[i]); // convert every element in array to numbers

      // You can also use parseInt() function, which takes a second argument for the base (radix).
      // A base of 10 is for decimal numbers, base of 16 is for hexadecimal, base of 2 is for binary.
      // sensors[i] = parseInt(sensors[i], 10); // converts every element in array to decimal number
    }
    if(sensors[2] == 1){
      sensors[2] = true;
    }
    else if(sensors[2] == 0){
      sensors[2] = false;
    }
    if(sensors[3] == 1){
      sensors[3] = true;
    }
    else if(sensors[3] == 0){
      sensors[3] = false;
    }
    checkForReset(sensors[2]);
    checkForPause(sensors[3]);
    
    //print(sensors);
  } 
}

function p5setup() {
  // put p5 setup code here
  let cnv = createCanvas(screenHeight, screenWidth);
  cnv.position((windowWidth - width) / 2, (windowHeight - height) / 2);
  textSize(72);
  textAlign(CENTER, CENTER);
  ballX = centerX;
  ballY = centerY;
  paddle1Xposition = centerY;
  paddle2Xposition = centerY;
  gamePaused = false;
}
function draw() {
  background(0,0,0);

  fill(255,255,255);
  text(player1Score,100,100);
  text(player2Score,400,100);
  if(!gamePaused){
    drawPlayer("player1",paddle1Position);
    drawPlayer("player2",paddle2Position);
    drawBall();
  }

  

  updateScore();

}

function updateScore() {
  if (player1Score == 5) {
    fill(255, 255, 255);
    text("Player 1 wins!", centerX, centerY);
    gamePaused = true;
  }
  if (player2Score == 5) {
    fill(255, 255, 255);
    text("Player 2 wins!", centerX, centerY);
    gamePaused = true;
  }
}

function drawPlayer(playerName,playerPosition){
  fill(255,255,255);
  if(playerName == "player1"){
    if(isConnectedToDivice){
      paddle1Xposition = map(sensors[0],0,255,0,screenHeight);
    }else {
      if(keyIsDown(87)){
      paddle1Xposition -= paddleSpeed;
      }
      if(keyIsDown(83)){
      paddle1Xposition += paddleSpeed;
      }
    }
    rectMode(CENTER);
    rect(playerPosition,paddle1Xposition,10,100);
    
  }
  else{
    if(isConnectedToDivice){
      paddle2Xposition = map(sensors[1],0,255,0,screenHeight);
    }
    else {
      if(keyIsDown(UP_ARROW)){
      paddle2Xposition -= paddleSpeed;
      }
      if(keyIsDown(DOWN_ARROW)){
      paddle2Xposition += paddleSpeed;
      }
    }
    rectMode(CENTER);
    rect(playerPosition,paddle2Xposition,10,100);
  }
  
}

function drawBall(){

  fill(255,255,255);
  rect(ballX,ballY,10,10);

  ballX += ballVelocityX * ballSpeed;
  ballY += ballVelocityY * ballSpeed;
  //console.log("ballX: " + ballX + " ballY: " + ballY);

  if(ballX - 5 <= paddle1Position + 5 && ballX - 5 > 0 && (paddle1Xposition < ballY + 50 && paddle1Xposition > ballY - 50)){
    ballVelocityX = ballVelocityX * -1;
    if(ballY > paddle1Xposition){
      ballVelocityY = ballVelocityY * -1;
    }
    if(ballY < paddle1Xposition){
      ballVelocityY = ballVelocityY * -1;
    }
  }

  // Collision with Paddle 2 (Right):
  // Check if the ball's horizontal position is at or past the paddle's left edge (480 - 5)
  if(ballX + 5 >= paddle2Position - 5 && ballX + 5 < screenWidth && (paddle2Xposition < ballY + 50 && paddle2Xposition > ballY - 50)){
    ballVelocityX = ballVelocityX * -1;
  }

  if(ballX == 0 ){
    pointScored = true;
    player2Score++;
    returnBallToCenter();
  }
  if(ballX == screenWidth){
    pointScored = true;
    player1Score++;
    returnBallToCenter();
  }

}

function returnBallToCenter(){
  ballX = centerX;
  ballY = centerY;
  ballVelocityX = ballVelocityX * -1;
  pointScored = false;
}

function checkForPause(buttonValue){
  if(buttonValue){
    gamePaused = !gamePaused;
  }
}

function checkForReset(buttonValue){
  if(buttonValue){
    resetGame();
  }
}

function printList(portList){
  print("List of Available Serial Ports: ");
  for (var i = 0; i < portList.length; i++) 
  {
    print(i + portList[i]); //print list of available serial ports to console
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

function inDataReceived(data) {
  inData = data;
  //console.log(data);
}

function serialError(err) {
  console.log('Something went wrong with the serial port. ' + err);
  isConnectedToDivice = false;
}

function serialClosed() {
  console.log('The serial port closed.');
  isConnectedToDivice = false;
}