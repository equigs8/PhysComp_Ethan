let x = 0;
let y = 1;

let playerX = 0;
let playerY = 0;

let playerInputX = 0;
let playerInputY = 0;

let moveSpeed = 5;

let isShooting = false;

let objectX = 0;
let objectY = 0;


let colorRed = 0;
let colorGreen = 0;
let colorBlue = 255;
function setup() {
  createCanvas(1000, 600);
}

function draw() {
  background(220);
  
  if(keyIsDown(LEFT_ARROW)){
    playerInputX = -1;
  }if(keyIsDown(RIGHT_ARROW)){
    playerInputX = 1;
  }if(keyIsDown(UP_ARROW)){
    playerInputY = -1;
  }if(keyIsDown(DOWN_ARROW)){
    playerInputY = 1;
  }
  if(keyIsDown(ENTER) && !isShooting){
    isShooting = true;
    objectX = playerX;
    objectY = playerY;
  }
  if(isShooting){
    moveObject();
  }



  drawPlayer();
  movePlayer(playerInputX, playerInputY);
  playerInputX = 0;
  playerInputY = 0;
}


function movePlayer(x, y) {
  playerX += x;
  playerY += y;
}

function drawPlayer(){
  fill(255);
  ellipse(playerX * moveSpeed, playerY * moveSpeed, 50, 50);
}

function moveObject(){
  objectX += playerX - mouseX;
  objectY += playerY - mouseY;
  rect(objectX * moveSpeed, objectY * moveSpeed, 20, 20);
}




