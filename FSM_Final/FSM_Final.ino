// Color Sensor Pins and Variables
#include <Wire.h>
#include <QTRSensors.h>
#include"Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_4X);

int color_flag = 1;
uint8_t color_list[3] = {0, 0, 0};
uint8_t color_lux = 1;

// LED and Button Pins
const int RYB_button[3] = {1, 1, 1};
const int RYB_LED[3] = {1, 1, 1};
const int start_button = 1;
const int start_LED = 1;
const int match_LED = 1;

// Motor Pins

/*
 *  M1-----Line Follower---------M3  Watch from Here for CW and CCW
 *  |                             |  CW is forward CCW is backward
 *  |                             |
 *  M2---------------------------M4 
 */
// Motor 1
const int m1_speed = 1;               
const int m1_in1 = 1;
const int m1_in2 = 1;
// Motor 2
const int m2_speed = 1;
const int m2_in1 = 1;
const int m2_in2 = 1;
// Motor 3
const int m3_speed = 1;
const int m3_in1 = 1;
const int m3_in2 = 1;
// Motor 4
const int m4_speed = 1;
const int m4_in1 = 1;
const int m4_in2 = 1;

// Line Follower Pins and Variables
const int left_pin = 1;
const int mid_pin = 1;
const int right_pin = 1;
const int count_pin  = 1;
int count_line = 0;

// Mole Whacker Servo Pins
#include <Servo.h>
const int servo_pin = 1;
Servo myservo;


// Line Follower Stuff
QTRSensors qtr;
const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];
int linePassed = 0;


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

  // Line Follower Setup
  pinMode(left_pin, INPUT);
  pinMode(mid_pin, INPUT);
  pinMode(right_pin, INPUT);
  pinMode(count_pin, INPUT);

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
    3, 4, 5
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
  int forward_speed = 64;
  Serial.println("Going Straight");
  digitalWrite(m1_in1, LOW); //Clockwise for Motor 1
  digitalWrite(m2_in2, HIGH);
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
  int right_speed = 53;
  digitalWrite(m1_in1, HIGH); //Clockwise for Motor 1
  digitalWrite(m2_in2, LOW);
  analogWrite(m1_speed, right_speed);

  digitalWrite(m2_in2, LOW); //clockwise for Motor 2
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_speed, right_speed);

  digitalWrite(m3_in1, HIGH); // clockwise for Motor3
  digitalWrite(m3_in2, LOW);
  analogWrite(m3_speed, right_speed);

  digitalWrite(m4_in1, LOW); // Connections for Motor4
  digitalWrite(m4_in2, HIGH);
  analogWrite(m4_speed, right_speed);
}

void adjLeft() {
  
  Serial.println("Shift Left");
  int left_speed = 53;
  digitalWrite(m1_in1, LOW); //Clockwise for Motor 1
  digitalWrite(m2_in2, HIGH);
  analogWrite(m1_speed, left_speed);

  digitalWrite(m2_in2, HIGH); //clockwise for Motor 2
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, left_speed);

  digitalWrite(m3_in1, LOW); // clockwise for Motor3
  digitalWrite(m3_in2, HIGH);
  analogWrite(m3_speed, left_speed);

  digitalWrite(m4_in1, HIGH); // Connections for Motor4
  digitalWrite(m4_in2, LOW);
  analogWrite(m4_speed, left_speed);
}

void left() {
  digitalWrite(m1_in1, HIGH); //CCW for Motor 1
  digitalWrite(m2_in2, LOW);
  analogWrite(m1_speed, 90);

  digitalWrite(m2_in2, LOW); //CCW for Motor 2
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_speed, 90);

  digitalWrite(m3_in1, HIGH); // CW for Motor3
  digitalWrite(m3_in2, LOW);
  analogWrite(m3_speed, 90);

  digitalWrite(m4_in1, LOW); // CW for Motor4
  digitalWrite(m4_in2, HIGH);
  analogWrite(m4_speed, 90);
}
void stopMotor() {
  Serial.println("Stop Motor");
  digitalWrite(m1_in1, LOW); //Stop for Motor 1
  digitalWrite(m2_in2, LOW);

  digitalWrite(m2_in2, LOW); //Stop for Motor 2
  digitalWrite(m2_in1, LOW);


  digitalWrite(m3_in1, LOW); // Stop for Motor3
  digitalWrite(m3_in2, LOW);


  digitalWrite(m4_in1, LOW); // Stop for Motor4
  digitalWrite(m4_in2, LOW);
}


// State Functions 
// Move Forward

int move_forward(int color_selected,  uint16_t position, int linePassed){
  //Logic
  
  // Line in the middle
  if (position == 1000) {
    forward();
  }

  // Line at the right
  else if (position == 2000) {
    adjRight();
  }

  // Line at the left
  else if (position == 0) {
    adjLeft();
  }
  if (sensorValues[0] > 500 && sensorValues[1] > 500 && sensorValues[2] > 500 ) {
    stopMotor();
    linePassed = linePassed + 1;
    Serial.println(linePassed);
    delay(500);
    forward();
    delay(40);

    if (linePassed == 3) {
      delay(5000);
      while (position!= 1000)
      {
        left();
        delay(100);
      }
      //Go forward
      forward();
      delay(200);
      stopMotor();
      delay(10000);
    }
  }

  else {
    forward();
    delay(10);
  }

  STATE = FORWARD_STATE;
  // Add statement to change state to Drop Ball State if Color Matches
  return STATE;
}

int move_turn(){
  //Logic
}

int drop_ball(){
  //Logic

}
void loop() {
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);
  int color_selected;
  int started;
  switch (STATE) {
    case COLOR_STATE:
      color_selected = 0;
      for (int i = 0; i < 2; i++) {
        if (digitalRead(RYB_button[i] == LOW)) {
          color_selected = i;
          digitalWrite(RYB_LED[i], HIGH);
          STATE=START_STATE;
        }
      }
      break;
    case START_STATE:
      started = digitalRead(start_button);
      if (started == LOW){
        digitalWrite(start_LED, HIGH);
        STATE = FORWARD_STATE;     
      }
      break;
    case FORWARD_STATE:
      STATE = move_forward(color_selected, position, linePassed); // Make Function for this
      break;
    case TURN_STATE:
      STATE = move_turn();
      break;
    case DROP_STATE:
      STATE = drop_ball();
      break;
    case END_GAME_STATE:
      break;
  }
}
