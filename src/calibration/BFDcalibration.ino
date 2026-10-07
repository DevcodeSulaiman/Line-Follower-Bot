const int sensorPins[5] = {2, 3, 4, 5, 6};  // D0 to D5 of BFD-1000
const int clpPin = 8;  // Calibration pin
const int calibrateButton = 9;  // Optional: push button for manual calibration


void setup() {
  Serial.begin(9600);

  // Sensor inputs
  for (int i = 0; i < 5; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  // CLP and calibration button
  pinMode(clpPin, OUTPUT);

  digitalWrite(clpPin, LOW);  // default LOW

  pinMode(calibrateButton, INPUT_PULLUP);  // Active LOW button

  // Optional auto calibration at startup:
  Serial.println("Auto calibrating...");
  calibrateSensors();
}

void loop() {
  // Manual calibration via button
  if (digitalRead(calibrateButton) == LOW) {
    Serial.println("Manual calibration triggered...");
    calibrateSensors();
  }

  // Read and print sensor values
  Serial.print("Sensors: ");
  for (int i = 0; i < 5; i++) {
    int val = digitalRead(sensorPins[i]);
    Serial.print(val);
    Serial.print(" ");
  }
  Serial.println();
  delay(100);
}

void calibrateSensors() {
  digitalWrite(clpPin, HIGH);
  delay(500);  // Hold CLP high for 0.5s
  digitalWrite(clpPin, LOW);
  Serial.println("Calibration complete.");
}
