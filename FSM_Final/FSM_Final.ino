// Color Sensor Pins and Variables
#include <Wire.h>
#include <QTRSensors.h>
#include<PID_v1.h>
#include"Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_4X);
uint8_t colorList[3] = {0, 0, 0};

// LED and Button Pins
const int RYB_button[3] = {42, 46, 36};
const int RYB_LED[3] = {31, 33, 34};
const int start_button = 38;
const int start_LED = 32;
const int match_LED = 35;
int color_selected;
unsigned long timeEnd;
unsigned long timeStart;
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
const int right_in2 = 12;
const int right_in1 = 13;

// PID Control Stuff
double kp = 0.03;
double kd = 0.055;
double ki = 0;

double setpoint = 2000;
double input, output;
PID myPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);

// Mole Whacker Servo Pins
#include <Servo.h>
const int servo_pin = 2;
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

  
  Serial.begin(9600);
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

  //  // 2.5 ms RC read timeout (default) * 10 reads per calibrate() call
  //  // = ~25 ms per calibrate() call.
  //  // Call calibrate() 400 times to make calibration take about 10 seconds.
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  //  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration
  //
  //  // print the calibration minimum values measured when emitters were on
  //  Serial.begin(9600);
  //  for (uint8_t i = 0; i < SensorCount; i++)
  //  {
  //    Serial.print(qtr.calibrationOn.minimum[i]);
  //    Serial.print(' ');
  //  }
  //  Serial.println();
  //
  //  // print the calibration maximum values measured when emitters were on
  //  for (uint8_t i = 0; i < SensorCount; i++)
  //  {
  //    Serial.print(qtr.calibrationOn.maximum[i]);
  //    Serial.print(' ');
  //  }
  qtr.calibrationOn.minimum[0] = 534;
  qtr.calibrationOn.minimum[1] = 412;
  qtr.calibrationOn.minimum[2] = 412;
  qtr.calibrationOn.minimum[3] = 412;
  qtr.calibrationOn.minimum[4] = 412;

  qtr.calibrationOn.maximum[0] = 2500;
  qtr.calibrationOn.maximum[1] = 2500;
  qtr.calibrationOn.maximum[2] = 2500;
  qtr.calibrationOn.maximum[3] = 2500;
  qtr.calibrationOn.maximum[4] = 2500;
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
  analogWrite(left_speed, 60);

  digitalWrite(right_in2, HIGH); //CCW for Motor 2
  digitalWrite(right_in1, LOW);
  analogWrite(right_speed, 60);

}

void forward() {
  Serial.println("Full Straight");
  digitalWrite(left_in1, LOW); //CCW for Motor 1
  digitalWrite(left_in2, HIGH);
  analogWrite(left_speed, 60);

  digitalWrite(right_in2, LOW); //CCW for Motor 2
  digitalWrite(right_in1, HIGH);
  analogWrite(right_speed, 60);

}
void stopMotor() {
  Serial.println("Stop Motor");
  digitalWrite(left_in1, LOW); //Stop for Motor 1
  digitalWrite(left_in2, LOW);

  digitalWrite(right_in2, LOW); //Stop for Motor 2
  digitalWrite(right_in1, LOW);

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
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");
  return distance;
}


void initial_move() {
  int position;
  timeEnd = millis();
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
  Serial.print("   Color Selected:      ");
  Serial.print(color_selected);
    Serial.print("   Red:      ");
    Serial.print(colorList[0]);
    Serial.print("   Yellow:      ");
    Serial.print(colorList[1]);
    Serial.print("   Blue:      ");
    Serial.println(colorList[2]);
  if (colorList[0] > 60 && color_selected == 0) {
    digitalWrite(match_LED, HIGH);
    STATE = DROP_STATE;
  }
  else if (colorList[0] > 30 && colorList[1] > 12 && colorList[2] < 15 && color_selected == 1) {
    digitalWrite(match_LED, HIGH);
    STATE = DROP_STATE;
  }
  else if (colorList[2] > 39 && color_selected == 2) {
    Serial.println("SUCCESS");
    digitalWrite(match_LED, HIGH);
    STATE = DROP_STATE;
  }
  else {
    digitalWrite(match_LED, LOW);
    STATE = FORWARD_STATE;
  }
}

int square_check(int measured_distance) {
  int success = 0;
  if (measured_distance > 20 && measured_distance < 48 && drop_flag1 == 0) {
    digitalWrite(RYB_LED[0], HIGH);
    success = 1;
    drop_flag1 = 1;
    color_check();
    digitalWrite(RYB_LED[0], LOW);
  }
  else if (measured_distance > 95 && measured_distance < 106 && drop_flag2 == 0) {
    digitalWrite(RYB_LED[0], HIGH);
    success = 1;
    drop_flag2 = 1;
    color_check();
    digitalWrite(RYB_LED[0], LOW);
  }
  else if (measured_distance > 135 && measured_distance < 150 && drop_flag3 == 0) {
    digitalWrite(RYB_LED[0], HIGH);
    success = 1;
    drop_flag3 = 1;
    color_check();
    digitalWrite(RYB_LED[0], LOW);
  }
  return success;
}
// Move Forward Function
int move_forward(int color_selected) {
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t sensors[5];

  int position = qtr.readLineBlack(sensors);
  input = position;

  float left_baseSpeed = 80;
  float right_baseSpeed = 80;

  myPID.Compute();

  float left_totalSpeed = left_baseSpeed + output;
  float right_totalSpeed = right_baseSpeed - output;

  if (left_totalSpeed < 0) {
    left_totalSpeed = left_baseSpeed;
  }
  if (right_totalSpeed < 0) {
    right_totalSpeed = right_baseSpeed;
  }
  if (left_totalSpeed > 255) {
    left_totalSpeed = left_baseSpeed;
  }
  if (right_totalSpeed > 255) {
    right_totalSpeed = right_baseSpeed;
  }

//  Serial.print(output);
//  Serial.print("       ");
//  Serial.print(left_totalSpeed);
//  Serial.print("       ");
//  Serial.println(right_totalSpeed);

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
  if (measured_distance < 50 && linePassed < 3) {
    linePassed = linePassed + 1;
    delay(100);
    stopMotor();
    delay(500);
    right();
    delay(2000); // 90 Degree Turn
    drop_flag1 = 0;
    drop_flag2 = 0;
    drop_flag3 = 0;
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
  int angle_min = 88;
  int angle_max = 150;
  int pos = 0;    // variable to store the servo position
  stopMotor();
  delay(200);
  myservo.write(angle_min);
  delay(1000);
  for (pos = angle_min; pos < angle_max; pos++) { // goes from 90 to 180 in steps of 15
    myservo.write(pos);
    delay(50);
  }
  for (pos = angle_max; pos > angle_min; pos--) { // goes from 90 to 180 in steps of 15
    myservo.write(pos);
    delay(50);
  }

  delay(100);
  digitalWrite(match_LED, LOW);

  return FORWARD_STATE;
}
void loop() {
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  timeEnd = millis();
  Serial.println(timeEnd);
  Serial.println(timeStart);
  int started;
  measured_distance = UltrasonicSensor();
  switch (STATE) {
    case COLOR_STATE:
      color_selected = 0;
      timeEnd = millis();
      timeStart = timeEnd;
      //Serial.println("Color State");
      for (int i = 0; i < 3; i++) {
        if (digitalRead(RYB_button[i]) == LOW) {
          color_selected = i;
          digitalWrite(RYB_LED[i], HIGH);
          STATE = START_STATE;
        }
      }
      break;
    case START_STATE:
      //Serial.println("Start State");
      timeEnd = millis();
      timeStart = timeEnd;
      started = digitalRead(start_button);
      if (started == LOW) {
        digitalWrite(start_LED, HIGH);
        STATE = INITIAL_STATE;
        timeStart = millis();
      }
      break;
    case INITIAL_STATE:
      //Serial.println("Initial State");
      initial_move();
      STATE = FORWARD_STATE;
    case FORWARD_STATE:
      //Serial.println("Forward State");
      STATE = move_forward(color_selected); // Make Function for this
      timeEnd = millis();
      break;
    case DROP_STATE:
      //Serial.println("Drop State");
      STATE = drop_ball();
      timeEnd = millis();
      break;
    case END_GAME_STATE: // All LEDS FLASH NEED TO ADD TIMER AS WELL
      timeEnd = millis();
      Serial.println("End Game State");
      stopMotor();
      digitalWrite(start_LED, HIGH);
      delay(100);
      digitalWrite(start_LED, LOW);
      delay(100);
      digitalWrite(start_LED, HIGH);
      digitalWrite(start_LED, LOW);
      break;
  }
  if ((timeEnd - timeStart) > 120000 ){
    STATE = END_GAME_STATE;
  }
  Serial.println(timeEnd);
}
