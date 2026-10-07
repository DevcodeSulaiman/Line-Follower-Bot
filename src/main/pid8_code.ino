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
// float lastPosition = 3500;  // for smoothing
float kp = 0.092;  
float ki = 0.01;
float kd = 0.8;
float integral = 0;
float error = 0;
float lastError = 0;

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

  // Print calibration results
  Serial.println("Min values:");
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print('\t');
  }
  Serial.println();
  Serial.println("Max values:");
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print('\t');
  }
  Serial.println();
  Serial.println("---- Calibration Finished ----");
  delay(2000);
}

void loop() {
  // Read sensors
  qtr.readCalibrated(sensorValues);
  for (int i = 0; i < SensorCount; i++) {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }

  // Get raw line position
  int position = qtr.readLineBlack(sensorValues);

  // ================= Smooth sudden jumps =================
  // float position = 0.8 * lastPosition + 0.2 * rawPosition;
  // lastPosition = position;

  // int error = position - 3500;  // center

  error = position - 3500;

  // ================= PID =================
  integral += error;
  int derivative = error - lastError;
  int correction = kp * error + ki * integral + kd * derivative;

  // ================= Limit extreme correction =================
  correction = constrain(correction, -225, 225);

  int leftSpeed = baseSpeed + correction;
  int rightSpeed = baseSpeed - correction;

  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // Drive motors
  setMotor(AIN1, AIN2, PWMA, leftSpeed);
  setMotor(BIN1, BIN2, PWMB, rightSpeed);

  lastError = error;

  Serial.print(" | Pos: "); Serial.print(position);
  Serial.print(" Err: "); Serial.print(error);
  Serial.print(" L: "); Serial.print(leftSpeed);
  Serial.print(" R: "); Serial.println(rightSpeed);

  delay(20);  // smaller delay for stability
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

// void stopMotors() {
//   analogWrite(PWMA, 0);
//   analogWrite(PWMB, 0);
// }
