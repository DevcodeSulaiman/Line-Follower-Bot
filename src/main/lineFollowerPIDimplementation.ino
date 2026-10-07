// TB6612 pins
const int IN1 = 2, IN2 = 3;      // Left dir
const int IN3 = 4, IN4 = 7;      // Right dir
const int PWMA = 5, PWMB = 6;      // PWM
const int STBY = 8;

// BFD-1000 pins
const int SENS[5] = {A0, A1, A2, A3, A4};
// Weights for position estimate (left negative, right positive)
int W[5] = {-2, -1, 0, +1, +2};

// Tuning vals
int   BASE_SPEED = 120;   // 0..255
float Kp = 25.0;
float Ki = 0.0;
float Kd = 7.0;

// PID vals
float prevErr = 0.0f, integ = 0.0f;
const float dt = 0.01f;     // 10 ms loop
const float integLimit = 200.0f;
const float kdLPF = 0.25f;  // derivative LPF (0..1)

// Interruptions Feedback
unsigned long lastSeenMs = 0;
const unsigned long lostTimeoutMs = 120;
int lastNonZeroPos = 0;

inline void setStandby(bool on){ digitalWrite(STBY, on ? HIGH : LOW); }

void setMotor(int in1, int in2, int pwm, int speed) {
  speed = constrain(speed, -255, 255);
  if (speed >= 0) { digitalWrite(in1, HIGH); digitalWrite(in2, LOW);  analogWrite(pwm, speed); }
  else            { digitalWrite(in1, LOW);  digitalWrite(in2, HIGH); analogWrite(pwm, -speed); }
}

void setBoth(int left, int right) {
  setMotor(IN1, IN2, PWMA, left);
  setMotor(IN3, IN4, PWMB, right);
}

// Position using Weights
float readPosition(bool &anyHit) {
  anyHit = false;
  int sumW = 0, sum = 0;

  for (int i = 0; i < 5; i++) {
    int v = !digitalRead(SENS[i]);
    if (v) { anyHit = true; sumW += W[i]; sum++; }
  }

  if (anyHit) {
    float pos = (float)sumW / (float)sum;
    if (pos != 0) lastNonZeroPos = (int)round(pos);
    return pos;
  } else {
    return (float)lastNonZeroPos; // bias toward last side
  }
}

void setup() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(PWMA, OUTPUT); pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  for (int i = 0; i < 5; i++) pinMode(SENS[i], INPUT_PULLUP);

  setStandby(true);
  setBoth(0, 0);
  Serial.begin(9600);
}

void loop() {
  unsigned long t0 = millis();

  bool anyHit = false;
  float pos = readPosition(anyHit);
  float error = -pos; // target is 0 (center)

  bool lost = !anyHit && (millis() - lastSeenMs > lostTimeoutMs);
  if (anyHit) lastSeenMs = millis();

  // PID
  integ += error * dt;
  integ = constrain(integ, -integLimit, +integLimit);

  static float dFilt = 0.0f;
  float deriv = (error - prevErr) / dt;
  dFilt = kdLPF * deriv + (1.0f - kdLPF) * dFilt;
  prevErr = error;

  float pid = Kp * error + Ki * integ + Kd * dFilt;

  int base = lost ? BASE_SPEED / 2 : BASE_SPEED;
  int left  = base + (int)(-pid);
  int right = base + (int)(+pid);

  left  = constrain(left, -255, 255);
  right = constrain(right, -255, 255);

  setBoth(left, right);

  while (millis() - t0 < 10) { /* ~100 Hz loop */ }
}
