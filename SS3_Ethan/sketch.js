let serial;
let portName = 'COM3';  // fill in your serial port name here
let options = { baudRate: 9600}; // change the baud rate to match your Arduino code

let serialData;

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
  serialData = Number(serial.read()); //Store incoming data as a number
  console.log(serial.read());
}

function p5setup() {
  // put p5 setup code here
  createCanvas(500, 420);
}
function draw() {
  background(220);
  fill(serialData, 0, 0);
  rect(100, 100, 100, 100);
  fill(0, 237, 0);
  rect(300, 100, serialData, serialData);
  circle(serialData, 200, 100);
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

function serialDataReceived(data) {
  serialData = data;
  //console.log(data);
}

function serialError(err) {
  console.log('Something went wrong with the serial port. ' + err);
}

function serialClosed() {
  console.log('The serial port closed.');
}