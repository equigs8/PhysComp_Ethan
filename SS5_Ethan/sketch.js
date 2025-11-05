let serial;
let portName = 'COM3';  // fill in your serial port name here
let options = { baudRate: 9600}; // change the baud rate to match your Arduino code

let outByte;

let buttonWidth = 200;
let buttonHeight = 100;
let button = false;


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
  //only sending data in this sketch
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


function p5setup() {
  createCanvas(400, 400);
  textAlign(CENTER,CENTER);
  rectMode(CENTER);
  textSize(20);
  noStroke();
}

function draw() {
  background(0);

  for(let i = 0; i < height/2;i++){
    let c = map(i,0,height/2,0,255);
    stroke(c);
    line(0,i,width,i);
  }
  for(let i = height; i >= height/2;i--){
    let c = map(i,height/2,height,255,0);
    stroke(c);
    line(0,i,width,i);
  }

  //center line
  stroke(127);
  line(0, height/2, width, height/2);
  //button
  fill(64);
  rect(width/2,height/2,buttonWidth,buttonHeight);
  //text
  fill(255);
  text("THIS WAY",width/2,100);
  text("THIS WAY",width/2,height-100);
  text("CLICK HERE", width/2,height/2);

  if(button == false){
    let posY = constrain(mouseY,0,height);
    outByte = map(posY,0,width,0,180);
    outByte = floor(outByte);
    serial.write(outByte);
  }

}

function mousePressed() {
  if(mouseX > width/2-buttonWidth/2 && mouseX < width/2+buttonWidth/2 && mouseY > height/2-buttonHeight/2 && mouseY < height/2+buttonHeight/2){
    button = true;
    serial.write(255);
  }
  button = false;
}
