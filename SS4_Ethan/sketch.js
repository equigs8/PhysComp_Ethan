// A p5.js sketch that implemments the game of pong using controls from an esp32 board

let screenHeight = 500;
let screenWidth = 500;
let centerX = screenWidth/2;
let centerY = screenHeight/2;

let serial;
let portName = 'COM3';  // fill in your serial port name here
let options = { baudRate: 9600}; // change the baud rate to match your Arduino code

let inData;
let sensors = [];

let paddle1Position = 20;
let paddle2Position = screenWidth - 20;

let paddle1Xposition = 0;
let paddle2Xposition = 0;

let ballX = 0;
let ballY = 0;

let ballVelocityX = 1;
let ballVelocityY = -.25;

let ballSpeed = 5;

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
    //print(sensors);
  } 
}

function p5setup() {
  // put p5 setup code here
  createCanvas(screenHeight, screenWidth);
  textSize(72);
  textAlign(CENTER, CENTER);
  ballX = centerX;
  ballY = centerY;
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

  if(player1Score == 5){
    fill(255,255,255);
    text("Player 1 wins!",centerX,centerY);
    gamePaused = true;
  }
  if(player2Score == 5){
    fill(255,255,255);
    text("Player 2 wins!",centerX,centerY);
    gamePaused = true;
  }

}

function drawPlayer(playerName,playerPosition){
  fill(255,255,255);
  if(playerName == "player1"){
    paddle1Xposition = sensors[0];
    rectMode(CENTER);
    rect(playerPosition,paddle1Xposition,10,100);
    
  }
  else{
    paddle2XPosition = sensors[1];
    rect(playerPosition,paddle2XPosition,10,100);
  }
  
}

function drawBall(){

  fill(255,255,255);
  rect(ballX,ballY,10,10);

  ballX += ballVelocityX;
  ballY += ballVelocityY;
  //console.log("ballX: " + ballX + " ballY: " + ballY);



  
  if(ballX == paddle1Position && (paddle1Xposition < ballY + 100 && paddle1Xposition > ballY - 100)){
    ballVelocityX = ballVelocityX * -1;
    console.log("ballX: " + ballX + " ballY: " + ballY);
    console.log("paddle1Position: " + paddle1Position + " paddle1Xposition: " + paddle1Xposition);

    if(ballY > paddle1Xposition){
      ballVelocityY = ballVelocityY * -1;
    }
    if(ballY < paddle1Xposition){
      ballVelocityY = ballVelocityY * -1;
    }
  }
  if(ballX == paddle2Position && (paddle2XPosition < ballY + 100 && paddle2XPosition > ballY - 100)){
    ballVelocityX = ballVelocityX * -1;
    console.log("ballX: " + ballX + " ballY: " + ballY);
    console.log("paddle2Position: " + paddle2Position + " paddle2XPosition: " + paddle2XPosition);
  }

  if(ballX == 0 ){
    pointScored = true;
    player2Score++;
    returnBall();
  }
  if(ballX == screenWidth){
    pointScored = true;
    player1Score++;
    returnBall();
  }

}

function returnBall(){
  ballX = centerX;
  ballY = centerY;
  ballVelocityX = ballVelocityX * -1;
  pointScored = false;
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
}

function serialOpened() {
  console.log('Serial Port Opened');
}

function inDataReceived(data) {
  inData = data;
  //console.log(data);
}

function serialError(err) {
  console.log('Something went wrong with the serial port. ' + err);
}

function serialClosed() {
  console.log('The serial port closed.');
}