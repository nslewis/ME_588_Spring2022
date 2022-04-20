#include <QTRSensors.h>

double kp = 2;
double ki = 5;
double kd = 1;

//First H Bridge

int m1_speed = 9; //Motor 1
int m1_in1 = 22;
int m1_in2 = 23;

int m2_speed = 3; //Motor 2
int m2_in1 = 25;
int m2_in2 = 24;

//Second H Bridge
int m3_speed = 6; // Motor 3
int m3_in1 = 12; // m1_in1 of second H bridge
int m3_in2 = 13; // m2_in2 of second H bridge

int m4_speed = 7; // Motor 4 second H bridge
int m4_in1 = 10; // m2_in2 of second H Bridge
int m4_in2 = 11; //m2_in1 of second H Bridge

// Line Follower Stuff
QTRSensors qtr;
const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];
int linePassed = 0;

void setup() {
  // put your setup code here, to run once:

  //Motor1
  pinMode(m1_speed, OUTPUT);
  pinMode(m1_in1, OUTPUT);
  pinMode(m2_in2, OUTPUT);

  //Motor2
  pinMode(m2_speed, OUTPUT);
  pinMode(m2_in2, OUTPUT);
  pinMode(m2_in1, OUTPUT);

  //Motor3
  pinMode(m3_speed, OUTPUT);
  pinMode(m3_in1, OUTPUT);
  pinMode(m3_in2, OUTPUT);

  //Motor4
  pinMode(m4_speed, OUTPUT);
  pinMode(m4_in1, OUTPUT);
  pinMode(m4_in2, OUTPUT);



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

void loop() {

  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);
  stopMotor();
  delay(50);

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

  //Pass a square
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
}
