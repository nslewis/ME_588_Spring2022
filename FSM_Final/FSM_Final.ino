// Color Sensor Pins and Variables
#include <Wire.h>
#include <QTRSensors.h>
#include"Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_4X);
uint8_t colorList[3] = {0, 0, 0};

// LED and Button Pins
const int RYB_button[3] = {42, 46, 1};
const int RYB_LED[3] = {31, 33, 34};
const int start_button = 1;
const int start_LED = 32;
const int match_LED = 35;
int color_selected;
// Motor Pins

/*
    M1-----Line Follower---------M3  Watch from Here for CW and CCW
    |                             |  CW is forward CCW is backward
    |                             |
    M2---------------------------M4
*/
// Motor 1
const int m1_speed = 9;
const int m1_in1 = 23;
const int m1_in2 = 22;
// Motor 2
const int m2_speed = 3;
const int m2_in1 = 24;
const int m2_in2 = 25;
// Motor 3
const int m3_speed = 6;
const int m3_in1 = 12;
const int m3_in2 = 13;
// Motor 4
const int m4_speed = 7;
const int m4_in1 = 11;
const int m4_in2 = 10;


// Mole Whacker Servo Pins
#include <Servo.h>
const int servo_pin = 1;
Servo myservo;
int drop_flag = 0; // 0 Means Ball Not Dropped, 1 Means Ball Dropped

// Line Follower Stuff
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
int start_flag = 0;


// Global States
int STATE = 1;
const int COLOR_STATE = 1;
const int START_STATE = 2;
const int FORWARD_STATE = 3;
const int TURN_STATE = 4;
const int DROP_STATE = 5;
const int END_GAME_STATE = 6;

/****************/
void setup() {
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
  pinMode(m1_speed, OUTPUT);
  pinMode(m2_speed, OUTPUT);
  pinMode(m3_speed, OUTPUT);
  pinMode(m4_speed, OUTPUT);

  pinMode(m1_in1, OUTPUT);
  pinMode(m2_in1, OUTPUT);
  pinMode(m3_in1, OUTPUT);
  pinMode(m4_in1, OUTPUT);

  pinMode(m1_in2, OUTPUT);
  pinMode(m2_in2, OUTPUT);
  pinMode(m3_in2, OUTPUT);
  pinMode(m4_in2, OUTPUT);

  // Servo Setup
  myservo.attach(servo_pin);

  //Line Follower Stuff
  // configure the sensors
  qtr.setTypeRC();
  // qtr.setSensorPins((const uint8_t[]){3, 4, 5, 6, 7, 8, 9, 10}, SensorCount);
  qtr.setSensorPins((const uint8_t[]) {
    4, 5, 8, 41
  }, SensorCount);
  qtr.setEmitterPin(2);

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
  delay(1000);

}

// Motor Move Function like forwards right left etc
void forward() {
  int forward_speed = 55;
  Serial.println("Going Straight");
  digitalWrite(m1_in1, LOW); //Clockwise for Motor 1
  digitalWrite(m1_in2, HIGH);
  analogWrite(m1_speed, forward_speed);

  digitalWrite(m2_in2, HIGH); //clockwise for Motor 2
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, forward_speed);

  digitalWrite(m3_in1, HIGH); // clockwise for Motor3
  digitalWrite(m3_in2, LOW);
  analogWrite(m3_speed, forward_speed);

  digitalWrite(m4_in1, LOW); // Clockwise for Motor4
  digitalWrite(m4_in2, HIGH);
  analogWrite(m4_speed, forward_speed);
}

void adjRight() {
  Serial.println("Shift Right");
  int right_speed = 55;
  digitalWrite(m1_in1, LOW); //Clockwise for Motor 1
  digitalWrite(m1_in2, HIGH);
  analogWrite(m1_speed, right_speed);

  digitalWrite(m2_in2, HIGH); //clockwise for Motor 2
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, right_speed);

  digitalWrite(m3_in1, LOW); // clockwise for Motor3
  digitalWrite(m3_in2, HIGH);
  analogWrite(m3_speed, right_speed);

  digitalWrite(m4_in1, HIGH); // Connections for Motor4
  digitalWrite(m4_in2, LOW);
  analogWrite(m4_speed, right_speed);
}

void adjLeft() {

  Serial.println("Shift Left");
  int left_speed = 60;
  digitalWrite(m1_in1, HIGH); //Clockwise for Motor 1
  digitalWrite(m1_in2, LOW);
  analogWrite(m1_speed, left_speed);

  digitalWrite(m2_in2, LOW); //clockwise for Motor 2
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_speed, left_speed);

  digitalWrite(m3_in1, HIGH); // clockwise for Motor3
  digitalWrite(m3_in2, LOW);
  analogWrite(m3_speed, left_speed);

  digitalWrite(m4_in1, LOW); // Connections for Motor4
  digitalWrite(m4_in2, HIGH);
  analogWrite(m4_speed, left_speed);
}
void right() {
  Serial.println("Full Right");
  digitalWrite(m1_in1, LOW); //CCW for Motor 1
  digitalWrite(m1_in2, HIGH);
  analogWrite(m1_speed, 80);

  digitalWrite(m2_in2, HIGH); //CCW for Motor 2
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, 80);

  digitalWrite(m3_in1, LOW); // CW for Motor3
  digitalWrite(m3_in2, HIGH);
  analogWrite(m3_speed, 80);

  digitalWrite(m4_in1, HIGH); // CW for Motor4
  digitalWrite(m4_in2, LOW);
  analogWrite(m4_speed, 100);
}
void stopMotor() {
  Serial.println("Stop Motor");
  digitalWrite(m1_in1, LOW); //Stop for Motor 1
  digitalWrite(m1_in2, LOW);

  digitalWrite(m2_in2, LOW); //Stop for Motor 2
  digitalWrite(m2_in1, LOW);


  digitalWrite(m3_in1, LOW); // Stop for Motor3
  digitalWrite(m3_in2, LOW);


  digitalWrite(m4_in1, LOW); // Stop for Motor4
  digitalWrite(m4_in2, LOW);
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

// State Functions
// Max Location of Black Line
int maxValLoc_func(uint16_t sensorValues[]) {
  int maxVal = sensorValues[0];
  int maxValLoc = 0;
  for (int i = 0; i < 3; i++) {
    if (sensorValues[i] > maxVal) {
      maxVal = sensorValues[i];
      maxValLoc = i;
    }
  }
  return maxValLoc;
}
// Move Forward Function
int move_forward(int color_selected) {
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  int pos_distance[3] = {0, 0, 0};
  int position = qtr.readLineBlack(sensorValues);
  delay(50);
  int maxValLoc = maxValLoc_func(sensorValues);
  // Starting From White Square
  if (start_flag == 0) {
    while (sensorValues[2] < 700) {
      position = qtr.readLineBlack(sensorValues);
      forward();
    }
    start_flag = 1;
  }
  else {
    // Necessary Corrections to Maintain Straight Line
    // Line in the right
    if (maxValLoc == 0) {
      adjRight();
    }

    // Line at the middle
    else if (maxValLoc == 1) {
      forward();
    }

    // Line at the left
    else if (maxValLoc == 2) {
      adjLeft();
    }
    position = qtr.readLineBlack(sensorValues); // Update Position
    // ULTRASONIC DISTANCE
    measured_distance = UltrasonicSensor();
    if (measured_distance == pos_distance[0] || measured_distance == pos_distance[1] || measured_distance == pos_distance[2]) {
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

    // First Turn
    if (measured_distance < 57 && linePassed < 3) {
      linePassed = linePassed + 1;
      forward();
      delay(100);
      stopMotor();
      delay(500);
      right();
      delay(600);
      position = qtr.readLineBlack(sensorValues); // Update Position
      maxValLoc = maxValLoc_func(sensorValues);
      while (maxValLoc != 0 && sensorValues[0] > 500) {
        right();
        position = qtr.readLineBlack(sensorValues); // Update Position
        maxValLoc = maxValLoc_func(sensorValues);
      }
    }
    forward();
    delay(100);
    measured_distance = UltrasonicSensor(); // Update Measured Distance
    // Second Turn
    if (measured_distance < 120 && linePassed == 3) {
      linePassed = linePassed + 1;
      forward();
      delay(100);
      stopMotor();
      delay(500);
      right();
      delay(600);
      position = qtr.readLineBlack(sensorValues);
      maxValLoc = maxValLoc_func(sensorValues);
      while (maxValLoc != 0 && sensorValues[0] > 500) {
        right();
        position = qtr.readLineBlack(sensorValues);
        maxValLoc = maxValLoc_func(sensorValues);
      }
      forward();
      delay(100);
    }
    measured_distance = UltrasonicSensor();
    // Third Turn
    if (measured_distance < 120 && linePassed == 4) {
      linePassed = linePassed + 1;
      forward();
      delay(100);
      stopMotor();
      delay(500);
      right();
      delay(600); // Make This 90 Degree Turn
      position = qtr.readLineBlack(sensorValues);
      maxValLoc = maxValLoc_func(sensorValues);
      while (maxValLoc != 0 && sensorValues[0] > 500) {
        right();
        position = qtr.readLineBlack(sensorValues);
        maxValLoc = maxValLoc_func(sensorValues);
      }
      forward();
      delay(100);
    }
    if (measured_distance < 120 && linePassed == 5) {
      linePassed = linePassed + 1;
      forward();
      delay(100);
      stopMotor();
      delay(500);
      right();
      delay(600); // Make This 90 Degree Turn
      position = qtr.readLineBlack(sensorValues);
      maxValLoc = maxValLoc_func(sensorValues);
      while (maxValLoc != 0 && sensorValues[0] > 500) {
        right();
        position = qtr.readLineBlack(sensorValues);
        maxValLoc = maxValLoc_func(sensorValues);
      }
      forward();
      delay(100);
    }
    if (linePassed == 6){
      STATE = END_GAME_STATE;
    }
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
      for (int i = 0; i < 2; i++) {
        if (digitalRead(RYB_button[i] == LOW)) {
          color_selected = i;
          digitalWrite(RYB_LED[i], HIGH);
          STATE = START_STATE;
        }
      }
      break;
    case START_STATE:
      started = digitalRead(start_button);
      if (started == LOW) {
        digitalWrite(start_LED, HIGH);
        STATE = FORWARD_STATE;
      }
      break;
    case FORWARD_STATE:
      STATE = move_forward(color_selected); // Make Function for this
      break;
    case DROP_STATE:
      STATE = drop_ball();
      break;
    case END_GAME_STATE:
      stopMotor();
      break;
  }
}
