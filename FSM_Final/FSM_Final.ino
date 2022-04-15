// Color Sensor Pins and Variables
#include <Wire.h>
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

}

void loop() {
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
      STATE = move_forward(color_selected); // Make Function for this
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
