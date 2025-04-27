#include <Arduino.h>
#include <Servo.h>
#include <Pixy2.h>
#include <Encoder.h>
#include <Serial_Plotting.h>
#define IN1 1 // right
#define IN2 0 // right
#define IN3 3 // left
#define IN4 2 // left
#define ENCODER1_A 4
#define ENCODER1_B 5
#define ENCODER2_A 6
#define ENCODER2_B 7
#define STEERING_SERVO_PIN 22
#define CAMERA_SERVO_PIN 23
#define angle_Horizental 2
#define MAX_ERROR_SUM 1000
#define STEERING_SERVO_MAX_LEFT 40
#define STEERING_SERVO_MAX_RIGHT 128
#define CAMERA_SERVO_MAX_LEFT 0
#define CAMERA_SERVO_MAX_RIGHT 165
#define MOTOR_SPEED_MAX 240
#define MOTOR_SPEED_MIN 130
#define LINE_VECTOR_SIZE 20
#define HORIZONTAL_LINE_SIZE 1
#define KI 0.0
#define KD 0
#define ANGLE_SETPOINT 0
#define buttonPin 23
#define initi 10
#define angleArraySize 5
#define TRIGGER_PIN 9
#define ECHO_PIN 8
typedef struct vectorPixy
{
  double longueur;
  int m_x1;
  int m_x0;
  int m_y1;
  int m_y0;
} vectorPixy;
Servo myServo1;
Servo myServo2;
Pixy2 pixy;
vectorPixy leftVectors[LINE_VECTOR_SIZE];
vectorPixy rightVectors[LINE_VECTOR_SIZE];
vectorPixy horizontalLines[HORIZONTAL_LINE_SIZE];
IntervalTimer myTimer;
Encoder enc1(5, 4);                      // left encoder
Encoder enc2(6, 7);                      // right encoder
const float wheel_circumference = 0.065; // meters (example: 21cm wheel perimeter)
const int left_encoder_ticks_per_rev = 430;
const int right_encoder_ticks_per_rev = 459;
const long interval = 16.7;
int finishline = 0;
unsigned long last_time = 0;
unsigned long currentMillisCheckHorizentalLines;
unsigned long previousMillis = 0;
float KP = 8;
long last_left_ticks = 0;
long last_right_ticks = 0;
long left_speed_cm_s = 0;
long right_speed_cm_s = 0;
long oldPosition2 = -999;
long oldPosition1 = -999;
int rightVectorsIndex = 0;
int leftVectorsIndex = 0;
int horizontalLinesIndex = 0;
int i = initi;
int pressed = 0;
int x = 0;
int firstAngleArray = 0;
int finalAngleArray[angleArraySize];
int defaultServoAngle = 78;
int backwardPWM = -120;
int distance;

void softwareReset()
{
  SCB_AIRCR = 0x05FA0004;
}
unsigned long startTime;
unsigned long lastCallTime = 0;
void readPixy()
{
  pixy.line.getAllFeatures();
}
void setup()
{
  pinMode(buttonPin, INPUT_PULLDOWN);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(100); // Stabilize sensor
  myServo1.attach(STEERING_SERVO_PIN);
  myServo2.attach(CAMERA_SERVO_PIN);
  myServo1.write(defaultServoAngle);
  myServo2.write(50);
  pixy.init();
  pixy.changeProg("line");
  pixy.setLamp(1, 1);
  Serial.begin(115200);
  // myTimer.begin(readPixy, 33000); // 33000 µs = 33 ms
}

void moveCar(int pwmLeft, int pwmRight)
{
  if (pwmLeft > 0)
  {
    analogWrite(IN3, pwmLeft);
    digitalWrite(IN4, LOW);
  }
  else
  {
    analogWrite(IN4, -pwmLeft);
    digitalWrite(IN3, LOW);
  }

  if (pwmRight > 0)
  {
    analogWrite(IN1, pwmRight);
    analogWrite(IN2, 0);
  }
  else
  {
    analogWrite(IN2, -pwmRight);
    digitalWrite(IN1, LOW);
  }
}
void checkBox()
{
  long duration;
  float distance;

  // Send 10us HIGH pulse to trigger pin
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Read the echo pin: duration in microseconds
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout: 30ms (max ~5m)

  // Calculate distance in cm
  distance = duration * 0.0343 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if (duration == 0)
  {
    Serial.println("Out of range");
  }
  else
  {

    if (distance < 20)
    {
      moveCar(-150, -150);
      delay(500);
      moveCar(0, 0);
      delay(5000);
    }
  }
}
void setSteeringServo(int angle)
{
  angle = constrain(angle, STEERING_SERVO_MAX_LEFT, STEERING_SERVO_MAX_RIGHT);
  myServo1.write(angle);
}
void setCameraServo(int angle)
{
  angle = constrain(angle, CAMERA_SERVO_MAX_LEFT, CAMERA_SERVO_MAX_RIGHT);
  Serial.println(angle);
  myServo2.write(angle);
}
// sorting vectors with the y coordinate to determine the nearest
void sortVectorsByProximity(vectorPixy vectors[], int size)
{
  for (int i = 0; i < size - 1; i++)
  {
    for (int j = 0; j < size - i - 1; j++)
    {
      if (vectors[j].m_y0 > vectors[j + 1].m_y0)
      {
        vectorPixy temp = vectors[j];
        vectors[j] = vectors[j + 1];
        vectors[j + 1] = temp;
      }
    }
  }
}
void filterLines()
{
  pixy.line.getAllFeatures();

  leftVectorsIndex = 0;
  rightVectorsIndex = 0;
  horizontalLinesIndex = 0;
  int aux_x0 = 0;
  int aux_x1 = 0;
  int aux_y0 = 0;
  int aux_y1 = 0;

  for (int i = 0; i < pixy.line.numVectors; i++)
  {
    double horizontalAngle = atan2(abs(pixy.line.vectors[i].m_y1 - pixy.line.vectors[i].m_y0),
                                   abs(pixy.line.vectors[i].m_x1 - pixy.line.vectors[i].m_x0)) *
                             180.00 / PI;

    // Serial.println("horizontal angle:");
    // Serial.println(horizontalAngle);
    // this is to detect the arrival line (yelzem neprogramiwha bch matakrach l starting line )
    if (horizontalAngle < 20 && horizontalAngle > 0 && horizontalLinesIndex < HORIZONTAL_LINE_SIZE)
    {
      horizontalLines[horizontalLinesIndex].m_x0 = pixy.line.vectors[i].m_x0;
      horizontalLines[horizontalLinesIndex].m_x1 = pixy.line.vectors[i].m_x1;
      horizontalLines[horizontalLinesIndex].m_y0 = pixy.line.vectors[i].m_y0;
      horizontalLines[horizontalLinesIndex].m_y1 = pixy.line.vectors[i].m_y1;
      horizontalLinesIndex++;
    }

    if (pixy.line.vectors[i].m_y0 > pixy.line.vectors[i].m_y1)
    {
      aux_x0 = pixy.line.vectors[i].m_x0;
      aux_x1 = pixy.line.vectors[i].m_x1;
      aux_y0 = pixy.line.vectors[i].m_y0;
      aux_y1 = pixy.line.vectors[i].m_y1;
    }
    else
    {
      aux_x0 = pixy.line.vectors[i].m_x1;
      aux_x1 = pixy.line.vectors[i].m_x0;
      aux_y0 = pixy.line.vectors[i].m_y1;
      aux_y1 = pixy.line.vectors[i].m_y0;
    }

    double aux_longueur = sqrt(pow(aux_y1 - aux_y0, 2) + pow(aux_x1 - aux_x0, 2));

    if (aux_x0 < pixy.frameWidth / 2)
    {
      leftVectors[leftVectorsIndex].longueur = aux_longueur;
      leftVectors[leftVectorsIndex].m_x0 = aux_x0;
      leftVectors[leftVectorsIndex].m_x1 = aux_x1;
      leftVectors[leftVectorsIndex].m_y0 = aux_y0;
      leftVectors[leftVectorsIndex].m_y1 = aux_y1;
      leftVectorsIndex++;
    }
    else
    {
      rightVectors[rightVectorsIndex].longueur = aux_longueur;
      rightVectors[rightVectorsIndex].m_x0 = aux_x0;
      rightVectors[rightVectorsIndex].m_x1 = aux_x1;
      rightVectors[rightVectorsIndex].m_y0 = aux_y0;
      rightVectors[rightVectorsIndex].m_y1 = aux_y1;
      rightVectorsIndex++;
    }
  }

  sortVectorsByProximity(leftVectors, leftVectorsIndex);
  sortVectorsByProximity(rightVectors, rightVectorsIndex);
}
int calculateAngle()
{
  static int checkTwoLines = 1;
  filterLines();
  static int calculatedAngle = 0;
  int middleLine_x0 = 0;
  int middleLine_x1 = 0;
  int middleLine_y0 = 0;
  int middleLine_y1 = 0;
  if (leftVectorsIndex >= 1 && rightVectorsIndex >= 1)
  {
    // Case 1: The car sees two lines
    // Checking to see if the lines are not connected with each others
    i = initi;
    if ((abs(leftVectors[0].m_x1 - rightVectors[0].m_x0) > 2 && abs(leftVectors[0].m_y1 - rightVectors[0].m_y0) > 2) &&
        (abs(leftVectors[0].m_x0 - rightVectors[0].m_x1) > 2 && abs(leftVectors[0].m_y0 - rightVectors[0].m_y1) > 2))
    {
      checkTwoLines = 1;
      middleLine_x0 = (leftVectors[0].m_x0 + rightVectors[0].m_x0) / 2;
      middleLine_x1 = (leftVectors[0].m_x1 + rightVectors[0].m_x1) / 2;
      middleLine_y0 = (leftVectors[0].m_y0 + rightVectors[0].m_y0) / 2;
      middleLine_y1 = (leftVectors[0].m_y1 + rightVectors[0].m_y1) / 2;
      calculatedAngle = ((middleLine_x0 + middleLine_x1) / 2) - ((int)pixy.frameWidth / 2);
    }
  }
  else if (leftVectorsIndex >= 1 && rightVectorsIndex == 0 && checkTwoLines)
  {
    // Case 2: The Car sees only the left line, the right line will be the margin of the screen
    i = initi;
    checkTwoLines = 0;
    middleLine_x0 = (leftVectors[0].m_x0 + pixy.frameWidth) / 2;
    middleLine_x1 = (leftVectors[0].m_x1 + pixy.frameWidth) / 2;
    middleLine_y0 = (leftVectors[0].m_y0 + pixy.frameHeight) / 2;
    middleLine_y1 = (leftVectors[0].m_y1 + 0) / 2;
    calculatedAngle = middleLine_x0 + middleLine_x1 - (int)pixy.frameWidth;
  }
  else if (leftVectorsIndex == 0 && rightVectorsIndex >= 1 && checkTwoLines)
  {
    // Case 3: The Car sees only the right line, the left line will be the margin of the screen
    i = initi;
    checkTwoLines = 0;
    middleLine_x0 = (0 + rightVectors[0].m_x0) / 2;
    middleLine_x1 = (0 + rightVectors[0].m_x1) / 2;
    middleLine_y0 = (pixy.frameHeight + rightVectors[0].m_y0) / 2;
    middleLine_y1 = (0 + rightVectors[0].m_y1) / 2;
    calculatedAngle = middleLine_x0 + middleLine_x1 - (int)pixy.frameWidth;
  }
  else if (leftVectorsIndex == 0 && rightVectorsIndex == 0)
  {
    // i++;
    // if (calculatedAngle > 0)
    // {
    //   calculatedAngle += i;
    // }
    // else if (calculatedAngle < 0)
    // {
    //   calculatedAngle -= i;
    // }
  }

  return calculatedAngle;
}
double pidControl(int angle)
{
  static double errorSum = 0.0;
  static double prevError = 0.0;
  static double output = 0.0;
  double error = 0.0;
  double derivative = 0.0;
  double errorforsum = angle;
  constrain(errorforsum, -25, 25);
  error = (double)(ANGLE_SETPOINT - errorforsum);
  if (errorSum > MAX_ERROR_SUM)
  {
    errorSum = MAX_ERROR_SUM;
    // moveCar(0, 0);
    // Serial.println("errorSum >= MAX_ERROR_SUM");
  }
  else if (errorSum < -MAX_ERROR_SUM)
  {
    errorSum = -1 * MAX_ERROR_SUM;
    // moveCar(0, 0);
  }
  else
  {
    errorSum += error;
  }

  derivative = (error - prevError);
  output = (double)KP * error + (double)KI * errorSum + (double)KD * derivative;
  prevError = error;
  return output;
}
void checkForAlignedLines()
{
  int k = 0;
  for (int i = 0; i < horizontalLinesIndex; i++)
  {
    if (horizontalLines[i].m_y0 >= pixy.frameHeight - 20 && horizontalLines[i].m_y1 >= pixy.frameHeight - 20)
    {
      k++;
    }
  }
  if (k >= 2)
  {
    finishline = 1;
  }
}
int angleArrayIndex = 0;
// this is for intersections need to test without it
bool checkfinalAngle(int angle)
{
  int k = 0;
  int threshold = 10; // Acceptable variation
  for (int i = 1; i < angleArraySize; i++)
  {
    if (abs(angle - finalAngleArray[i - 1]) > threshold)
    {
      if (i < angleArraySize / 3)
      {
        k += 1;
      }
      else
      {
        k++;
      }
    }
  }
  if (k > 2)
  {
    return false;
  }
  return true;
}
void calculate_speed()
{
  unsigned long current_time = millis();
  long current_left_ticks = enc2.read();
  long current_right_ticks = enc1.read();
  float delta_time = (current_time - last_time) / 1000.0; // seconds
  if (delta_time == 0)
    delta_time = 0.001; // avoid division by zero

  long delta_left_ticks = current_left_ticks - last_left_ticks;
  long delta_right_ticks = current_right_ticks - last_right_ticks;

  float left_ticks_per_sec = delta_left_ticks / delta_time;
  float right_ticks_per_sec = delta_right_ticks / delta_time;

  left_speed_cm_s = (left_ticks_per_sec / left_encoder_ticks_per_rev) * wheel_circumference * 100;    // centimetre
  right_speed_cm_s = (right_ticks_per_sec / right_encoder_ticks_per_rev) * wheel_circumference * 100; // centimetre

  last_time = current_time;
  last_left_ticks = current_left_ticks;
  last_right_ticks = current_right_ticks;
}
// PID parameters
float kp = 10.0; // Proportional gain
float ki = 0.5;  // Integral gain
float kd = 10;   // Derivative gain

// Target speeds (setpoints)
float target_left_speed_cm_s = 20.0; // example 20 cm/s
float target_right_speed_cm_s = 20.0;

// PID variables
float left_error = 0, right_error = 0;
float last_left_error = 0, last_right_error = 0;
float left_integral = 0, right_integral = 0;

// Motor commands
int left_pwm = 0;
int right_pwm = 0;
void update_speed_pid()
{
  calculate_speed(); // Step 1: update current speed

  // LEFT MOTOR PID
  float left_error = target_left_speed_cm_s - left_speed_cm_s;
  left_integral += left_error;
  float left_derivative = left_error - last_left_error;

  float left_output = (kp * left_error) + (ki * left_integral) + (kd * left_derivative);

  left_pwm += left_output; // Add PID output to PWM
  left_pwm = constrain(left_pwm, 0, 255);

  // RIGHT MOTOR PID
  float right_error = target_right_speed_cm_s - right_speed_cm_s;
  right_integral += right_error;
  float right_derivative = right_error - last_right_error;

  float right_output = (kp * right_error) + (ki * right_integral) + (kd * right_derivative);

  right_pwm += right_output;
  right_pwm = constrain(right_pwm, 0, 255);

  // Apply to motors
  moveCar(left_pwm, right_pwm);

  // Save last errors for next derivative calculation
  last_left_error = left_error;
  last_right_error = right_error;
}
void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillisCheckHorizentalLines - currentMillis >= 10000)
  {
    checkForAlignedLines();
  }

  previousMillis = currentMillis;
  pixy.setLamp(0, 0);
  int angle = calculateAngle();
  checkForAlignedLines();
  int output = (int)pidControl(angle);
  output = constrain(output, -255, 255);
  angle = constrain(angle, -25, 25);
  int servoAngle = map(output, -255, 255, 50, -50);

  int testservoAngle = map(angle, -25, 25, defaultServoAngle - 45, defaultServoAngle + 45);
  testservoAngle = constrain(testservoAngle, STEERING_SERVO_MAX_LEFT, STEERING_SERVO_MAX_RIGHT);
  int finalAngle = (int)(testservoAngle);
  sendData("angle", angle);
  sendData("finalAngle", finalAngle);
  int motorMapping = map(abs(angle), 0, 35, MOTOR_SPEED_MAX, 80);
  motorMapping = constrain(motorMapping, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
  setSteeringServo(finalAngle);
  int motorOutput = 150;
  moveCar(motorOutput, motorOutput);
}