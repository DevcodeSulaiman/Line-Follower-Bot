#include <QTRSensors.h>

QTRSensors qtr;   
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

int AIN1 = 2;
int AIN2 = 3;
int PWMA = 5;
int BIN1 = 4;
int BIN2 = 7;
int PWMB = 6;
int STBY = 8;

// ================= PID VARIABLES =================
int baseSpeed = 120;   
float kp = 0.092;  
float ki = 0.01;
float kd = 0.8;
float integral = 0;
float error = 0;
float lastError = 0;

int s[SensorCount];
int threshold[SensorCount]; // Move threshold outside loop

void setup() {
  // Motor pins
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  Serial.begin(9600);

  // QTR sensor setup
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A7, A6, A5, A4, A3, A2, A1, A0}, SensorCount);
  qtr.setEmitterPin(12);

  // Calibration
  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("Calibrating on battery...");
  for (uint16_t i = 0; i < 450; i++) {
    qtr.calibrate();
    delay(20);
  }
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Calibration Done!");

  // Set thresholds per sensor
  for (int i = 0; i < SensorCount; i++) {
    threshold[i] = (qtr.calibrationOn.minimum[i] + qtr.calibrationOn.maximum[i]) / 2;
  }
}

void loop() {
  qtr.readCalibrated(sensorValues);

  // Convert to binary
  bool lineDetected = false;
  for (int i = 0; i < SensorCount; i++) {
    s[i] = (sensorValues[i] > threshold[i]) ? 1 : 0;
    if (s[i] == 1) lineDetected = true;
  }

  // ================= Line-lost handling =================
  if (!lineDetected) {
  setMotor(AIN1, AIN2, PWMA, 45);
  setMotor(BIN1, BIN2, PWMB, -45);
    return; // skip PID until line is found
  }

  else if (s[3] ==1 && s[4] ==1){
    setMotor(AIN1, AIN2, PWMA, baseSpeed);
    setMotor(BIN1, BIN2, PWMB, baseSpeed);
  }
  
  // ================= PID Line Following =================
  int position = qtr.readLineBlack(sensorValues);
  error = float(position) - 3500.0;
  integral += error;
  float derivative = error - lastError;
  float correction = kp*error + ki*integral + kd*derivative;
  correction = constrain(correction, -225, 225);

  int leftSpeed = baseSpeed + correction;
  int rightSpeed = baseSpeed - correction;

  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  setMotor(AIN1, AIN2, PWMA, leftSpeed);
  setMotor(BIN1, BIN2, PWMB, rightSpeed);

  lastError = error;

  Serial.print(" | Pos: "); Serial.print(position);
  Serial.print(" Err: "); Serial.print(error);
  Serial.print(" L: "); Serial.print(leftSpeed);
  Serial.print(" R: "); Serial.println(rightSpeed);

  delay(20);
}

void setMotor(int in1, int in2, int pwm, int speed) {
  if (speed >= 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwm, speed);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwm, -speed);
  }
}
