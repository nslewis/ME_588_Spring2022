// Color Sensor Pins and Variables
#include <Wire.h>
#include <QTRSensors.h>
#include<PID_v1.h>
#include"Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_4X);
uint8_t colorList[3] = {0, 0, 0};

// LED and Button Pins
const int RYB_button[3] = {42, 46, 40};
const int RYB_LED[3] = {31, 33, 34};
const int start_button = 38;
const int start_LED = 32;
const int match_LED = 35;
int color_selected;
// Motor Pins
/*          41  8  5  4
    left-----Line Follower---------M3  Watch from Here for CW and CCW
    |                             |  CW is forward CCW is backward
    |                             |
    --------------------------------
*/
// Motor 1
const int left_speed = 9;
const int left_in1 = 23;
const int left_in2 = 22;
// Motor 2
const int right_speed = 7;
const int right_in1 = 12;
const int right_in2 = 13;

// PID Control Stuff
double kp = 0.03;
double kd = 0.05;
double ki = 0;

double setpoint = 2000;
double input, output;
PID myPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);

// Mole Whacker Servo Pins
#include <Servo.h>
const int servo_pin = 1;
Servo myservo;
int drop_flag1 = 0; // 0 Means Ball Not Dropped, 1 Means Ball Dropped
int drop_flag2 = 0; // 0 Means Ball Not Dropped, 1 Means Ball Dropped
int drop_flag3 = 0; // 0 Means Ball Not Dropped, 1 Means Ball Dropped

// Line Follower Stuff

/*          41  8  5  4
    left-----Line Follower---------M4  Watch from Here for CW and CCW
    |                             |  CW is forward CCW is backward
    |                             |
    right---------------------------M3
*/
QTRSensors qtr;
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];
int linePassed = 0;

// Ultrasonic Stuff
#define echoPin 52
#define trigPin 48
// defines variables for Ultrasonic Sensor
long int duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int measured_distance;


// Global States
int STATE = 1;
const int COLOR_STATE = 1;
const int START_STATE = 2;
const int INITIAL_STATE = 3;
const int FORWARD_STATE = 4;
const int DROP_STATE = 5;
const int END_GAME_STATE = 6;

/****************/
void setup() {

  // PID Stuff

  myPID.SetOutputLimits(-20, 20);
  //Ultrasonic Sensor Pin Declarations
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  // Button Setup
  pinMode(RYB_button[0], INPUT_PULLUP);
  pinMode(RYB_button[1], INPUT_PULLUP);
  pinMode(RYB_button[2], INPUT_PULLUP);

  // Start Setup
  pinMode(start_button, INPUT_PULLUP);

  // LED Setup
  pinMode(RYB_LED[0], OUTPUT);
  pinMode(RYB_LED[1], OUTPUT);
  pinMode(RYB_LED[2], OUTPUT);
  pinMode(start_LED, OUTPUT);
  pinMode(match_LED, OUTPUT);

  // Color Sensor Setup
  tcs.begin();
  pinMode(RYB_LED[0], OUTPUT);
  pinMode(RYB_LED[1], OUTPUT);
  pinMode(RYB_LED[2], OUTPUT);

  // Motor Setup
  pinMode(left_speed, OUTPUT);
  pinMode(right_speed, OUTPUT);

  pinMode(left_in1, OUTPUT);
  pinMode(right_in1, OUTPUT);

  pinMode(left_in2, OUTPUT);
  pinMode(right_in2, OUTPUT);

  // Servo Setup
  myservo.attach(servo_pin);

  //Line Follower Stuff
  // configure the sensors
  qtr.setTypeRC();
  // qtr.setSensorPins((const uint8_t[]){3, 4, 5, 6, 7, 8, 9, 10}, SensorCount);
  qtr.setSensorPins((const uint8_t[]) {
    4, 49, 5, 41, 8
  }, SensorCount);

  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode

  // 2.5 ms RC read timeout (default) * 10 reads per calibrate() call
  // = ~25 ms per calibrate() call.
  // Call calibrate() 400 times to make calibration take about 10 seconds.
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration

  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  myPID.SetMode(AUTOMATIC);
  delay(1000);

}

// Motor Move Function like forwards right left etc
void right() {
  Serial.println("Full Right");
  digitalWrite(left_in1, LOW); //CCW for Motor 1
  digitalWrite(left_in2, HIGH);
  analogWrite(left_speed, 80);

  digitalWrite(right_in2, HIGH); //CCW for Motor 2
  digitalWrite(right_in1, LOW);
  analogWrite(right_speed, 80);

}

void forward() {
  Serial.println("Full Straight");
  digitalWrite(left_in1, LOW); //CCW for Motor 1
  digitalWrite(left_in2, HIGH);
  analogWrite(left_speed, 80);

  digitalWrite(right_in2, LOW); //CCW for Motor 2
  digitalWrite(right_in1, HIGH);
  analogWrite(right_speed, 80);

}
void stopMotor() {
  Serial.println("Stop Motor");
  digitalWrite(left_in1, LOW); //Stop for Motor 1
  digitalWrite(left_in2, LOW);

  digitalWrite(right_in2, LOW); //Stop for Motor 2
  digitalWrite(right_in1, LOW);
  drop_flag1 = 0;
  drop_flag2 = 0;
  drop_flag3 = 0;

}
// Ultrasonic Stuff
double UltrasonicSensor()
{
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}


void initial_move() {
  int position;
  while (sensorValues[1] < 700) {
    position = qtr.readLineBlack(sensorValues);
    forward();
  }
  stopMotor();
  Serial.println("Start from White Successful");
  STATE = FORWARD_STATE;
}

void color_check() {
  int lux = tcs.getRGB(&colorList[0], &colorList[1], &colorList[2]);
  if (lux > 700) {
    if (colorList[0] > 60 && color_selected == 0) {
      digitalWrite(match_LED, HIGH);
      STATE = DROP_STATE;
    }
    else if (colorList[0] > 30 && colorList[1] > 12 && colorList[2] < 5 && color_selected == 1) {
      digitalWrite(match_LED, HIGH);
      STATE = DROP_STATE;
    }
    else if (colorList[2] > 30 && color_selected == 2) {
      digitalWrite(match_LED, HIGH);
      STATE = DROP_STATE;
    }
    else {
      digitalWrite(match_LED, LOW);
      STATE = FORWARD_STATE;
    }
  }
  else {
    digitalWrite(match_LED, LOW);
    STATE = FORWARD_STATE;
  }
}

int square_check(int measured_distance) {
  int success = 0;
  if (measured_distance > 55 && measured_distance < 65 && drop_flag1 == 0) {
    success = 1;
    drop_flag1 = 1;
    color_check();
  }
  else if (measured_distance > 125 && measured_distance < 135 && drop_flag2 == 0) {
    success = 1;
    drop_flag2 = 1;
    color_check();
  }
  else if (measured_distance > 165 && measured_distance < 175 && drop_flag3 == 0) {
    success = 1;
    drop_flag3 = 1;
    color_check();
  }
  return success;
}
// Move Forward Function
int move_forward(int color_selected) {
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  int sensors[5];
  int pos_distance[3] = {165, 135, 57};

  int position = qtr.readLineBlack(sensors);
  input = position;

  float left_baseSpeed = 90;
  float right_baseSpeed = 100;

  myPID.Compute();

  float left_totalSpeed = left_baseSpeed + output;
  float right_totalSpeed = right_baseSpeed - output;

  if (left_totalSpeed < 0) {
    left_totalSpeed = 80;
  }
  if (right_totalSpeed < 0) {
    right_totalSpeed = 90;
  }
  if (left_totalSpeed > 255) {
    left_totalSpeed = 150;
  }
  if (right_totalSpeed > 255) {
    right_totalSpeed = 150;
  }

  Serial.print(output);
  Serial.print("       ");
  Serial.print(left_totalSpeed);
  Serial.print("       ");
  Serial.println(right_totalSpeed);

  digitalWrite(left_in2, HIGH);
  digitalWrite(left_in1, LOW);
  //left_totalSpeed = 80;
  analogWrite(left_speed, left_totalSpeed);

  digitalWrite(right_in1, HIGH);
  digitalWrite(right_in2, LOW);
  //right_totalSpeed = 90;
  analogWrite(right_speed, right_totalSpeed);


  // ULTRASONIC DISTANCE
  measured_distance = UltrasonicSensor();

  // Does square check and color drop maybe
  square_check(measured_distance);


  // First Turn
  measured_distance = UltrasonicSensor(); // Update Measured Distance
  if (measured_distance < 57 && linePassed < 3) {
    linePassed = linePassed + 1;
    delay(100);
    stopMotor();
    delay(500);
    right();
    delay(600); // 90 Degree Turn
  }
  measured_distance = UltrasonicSensor(); // Update Measured Distance

  // Second Turn
  if (measured_distance < 120 && linePassed == 3) {
    linePassed = linePassed + 1;
    delay(100);
    stopMotor();
    delay(500);
    right();
    delay(600); // 90 Degree Turn
  }


  measured_distance = UltrasonicSensor(); // Update Measured Distance
  // Third Turn
  if (measured_distance < 120 && linePassed == 4) {
    linePassed = linePassed + 1;
    delay(100);
    stopMotor();
    delay(500);
    right();
    delay(600); // Make This 90 Degree Turn For center
    position = qtr.readLineBlack(sensorValues);
  }
  else if (measured_distance < 120 && linePassed == 5) {
    linePassed = linePassed + 1;
    delay(100);
    stopMotor();
    delay(500);
    right();
    delay(600); // Make This 90 Degree Turn for center
  }
  if (linePassed == 6) {
    STATE = END_GAME_STATE;
  }
  return STATE;
}

int drop_ball() {
  int angle_min = 55;
  int angle_max = 111;
  int pos = 0;    // variable to store the servo position
  //Logic
  delay(1000);
  myservo.write(angle_min);
  for (pos = angle_min; pos < angle_max; pos++) { // goes from 90 to 180 in steps of 15
    delay(50000);
    myservo.write(pos);
    delay(50);
  }
  return FORWARD_STATE;
}
void loop() {
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  int started;
  switch (STATE) {
    case COLOR_STATE:
      color_selected = 0;
      Serial.println("Color State");
      for (int i = 0; i < 2; i++) {
        if (digitalRead(RYB_button[i]) == LOW) {
          color_selected = i;
          digitalWrite(RYB_LED[i], HIGH);
          STATE = START_STATE;
        }
      }
      break;
    case START_STATE:
      Serial.println("Start State");
      started = digitalRead(start_button);
      if (started == LOW) {
        digitalWrite(start_LED, HIGH);
        STATE = INITIAL_STATE;
      }
      break;
    case INITIAL_STATE:
      Serial.println("Initial State");
      initial_move();
      STATE = FORWARD_STATE;
    case FORWARD_STATE:
      Serial.println("Forward State");
      STATE = move_forward(color_selected); // Make Function for this
      break;
    case DROP_STATE:
      Serial.println("Drop State");
      STATE = drop_ball();
      break;
    case END_GAME_STATE: // All LEDS FLASH NEED TO ADD TIMER AS WELL
      Serial.println("End Game State");
      stopMotor();
      break;
  }
}
