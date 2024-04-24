// Define pins for L298N motor driver
const byte ENA = 10; // Enable Pin for Motor A
const byte IN1 = 11; // Motor A Input 1
const byte IN2 = 12; // Motor A Input 2
const byte ENB = 5;  // Enable Pin for Motor B
const byte IN3 = 8;  // Motor B Input 1
const byte IN4 = 9;  // Motor B Input 2

// Define pins for analog IR sensor array
const byte sensorPins[] = {A0, A1, A2, A3, A4}; // Sensor pins
const int numSensors = sizeof(sensorPins) / sizeof(sensorPins[0]); // Number of sensors

// PID parameters
const float Kp = 0.5;  // Proportional gain
const float Ki = 0.02; // Integral gain
const float Kd = 0.2;  // Derivative gain

// Setpoint (target sensor value)                                                                   
const int setpoint = 512; // Adjust this value as needed

// Variables for PID control
int lastError = 0;
float integral = 0;

void setup() {
  // Motor control pins setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // IR sensor pins setup
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  // Set motor speed to maximum
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Read sensor values
  int sensorValues[numSensors];
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
  }

  // Compute error
  int error = 0;
  int totalWeight = 0;
  for (int i = 0; i < numSensors; i++) {
    error += (i - (numSensors - 1) / 2) * sensorValues[i];
    totalWeight += sensorValues[i];
  }
  error /= totalWeight;

  // PID control
  float pidTerm = Kp * error + Ki * integral + Kd * (error - lastError);
  integral += error;
  lastError = error;

  // Adjust motor speeds based on PID output
  int motorSpeedA = constrain(setpoint + pidTerm, 0, 1023);
  int motorSpeedB = constrain(setpoint - pidTerm, 0, 1023);

  // Set motor directions based on PID output
  if (motorSpeedA > setpoint) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  if (motorSpeedB > setpoint) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  // Set motor speeds
  analogWrite(ENA, motorSpeedA);
  analogWrite(ENB, motorSpeedB);

  // Print sensor values and PID output to serial monitor
  Serial.print("Sensor Values: ");
  for (int i = 0; i < numSensors; i++) {
    Serial.print(sensorValues[i]);
    Serial.print("\t");
  }
  Serial.print("\tPID Output: ");
  Serial.println(pidTerm);

  // Wait for a short delay before the next iteration
  delay(10);
}
