int SPEAKER_PIN = 0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32-S3!");
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {
  tone(SPEAKER_PIN, 262, 250)
  delay(10); // this speeds up the simulation
}
