int potPin = 1;
const int pinRGB = 38; //built-in rgb led
int potValue = 0;

const int numReads = 16; //Number of readings in rolling avarage
int readings[numReads];
int count = 0;

void setup() {
  // put your setup code here, to run once:
  analogReadResolution(11);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

  readings[count] = analogRead(potPin);
  count++;
  if (count >= numReads){
    count = 0;
  }
  int sum = 0;

  for (int i=0; i < numReads; i++){
    sum += readings[i];
  }
  int analogValue = sum / numReads;
  Serial.println(analogValue);
  
}
