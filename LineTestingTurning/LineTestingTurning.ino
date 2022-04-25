#include <QTRSensors.h>

//First H Bridge

int m1_speed = 9; //Motor 1
int m1_in1 = 23;
int m1_in2 = 22;

int m2_speed = 3; //Motor 2
int m2_in1 = 24;
int m2_in2 = 25;

//Second H Bridge
int m3_speed = 6; // Motor 3
int m3_in1 = 12; // m1_in1 of second H bridge
int m3_in2 = 13; // m2_in2 of second H bridge

int m4_speed = 7; // Motor 4 second H bridge
int m4_in1 = 11; // m2_in2 of second H Bridge
int m4_in2 = 10; //m2_in1 of second H Bridge

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
int flag = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  //Motor1
  pinMode(m1_speed, OUTPUT);
  pinMode(m1_in1, OUTPUT);
  pinMode(m1_in2, OUTPUT);

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

  // Ultrasonic
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  //Line Follower Stuff
  // configure the sensors
  qtr.setTypeRC();
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

void forward_start() {
  int forward_speed = 40;
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
  int right_speed = 40;
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
  int left_speed = 40;
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
  analogWrite(m1_speed, 60);

  digitalWrite(m2_in2, HIGH); //CCW for Motor 2
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, 60);

  digitalWrite(m3_in1, LOW); // CW for Motor3
  digitalWrite(m3_in2, HIGH);
  analogWrite(m3_speed, 60);

  digitalWrite(m4_in1, HIGH); // CW for Motor4
  digitalWrite(m4_in2, LOW);
  analogWrite(m4_speed, 80);
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
void loop() {

  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);
  //  Serial.println(position);
  delay(50);
  int maxVal = sensorValues[0];
  int maxValLoc = 0;
  for (int i = 0; i < 3; i++) {
    if (sensorValues[i] > maxVal) {
      maxVal = sensorValues[i];
      maxValLoc = i;
    }
  }
    for (uint8_t i = 0; i < SensorCount; i++)
    {
      Serial.print(sensorValues[i]);
      Serial.print('\t');
    }
  if (flag == 0) {
    while (sensorValues[1] < 700) {
      position = qtr.readLineBlack(sensorValues);
      forward_start();
    }
    adjLeft();
    delay(100);
    flag = 1;
  }
  else {
    position = qtr.readLineBlack(sensorValues);
    //  Serial.println(position);
    delay(50);
    maxVal = sensorValues[0];
    maxValLoc = 0;
    for (int i = 0; i < 3; i++) {
      if (sensorValues[i] > maxVal) {
        maxVal = sensorValues[i];
        maxValLoc = i;
      }
    }
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

    position = qtr.readLineBlack(sensorValues);
    // Pass a square
    //  if (sensorValues[3] > 700) {
    //    while (sensorValues[3] > 700) {
    //      position = qtr.readLineBlack(sensorValues);
    ////      Serial.println(position);
    //      forward();
    //    }
    //    linePassed = linePassed + 1;
    // ULTRASONIC DISTANCE
    measured_distance = UltrasonicSensor();
    if (measured_distance < 57 && linePassed < 3) {
      linePassed = linePassed + 1;
      forward();
      delay(100);
      stopMotor();
      delay(500);
      right();
      delay(600);
      position = qtr.readLineBlack(sensorValues);
      //    Serial.println(position);
      //      for (uint8_t i = 0; i < 3; i++)
      //        {
      //          Serial.print(sensorValues[i]);
      //          Serial.print('\t');
      //        }
      maxVal = sensorValues[0];
      maxValLoc = 0;
      for (int i = 0; i < 3; i++) {
        if (sensorValues[i] > maxVal) {
          maxVal = sensorValues[i];
          maxValLoc = i;
        }
      }
      //      Serial.println(maxValLoc);
      while (maxValLoc != 0 && sensorValues[0] > 500) {

        //        for (uint8_t i = 0; i < 3; i++)
        //        {
        //          Serial.print(sensorValues[i]);
        //          Serial.print('\t');
        //        }

        right();
        position = qtr.readLineBlack(sensorValues);
        //      Serial.println(position);
        maxVal = sensorValues[0];
        maxValLoc = 0;
        for (int i = 0; i < 3; i++) {
          if (sensorValues[i] > maxVal) {
            maxVal = sensorValues[i];
            maxValLoc = i;
          }
        }
      }
      forward();
      delay(100);
      //        Serial.println(maxValLoc);
    }

   // measured_distance = UltrasonicSensor();
    if (measured_distance < 120 && linePassed == 3) {
      linePassed = linePassed + 1;
      forward();
      delay(100);
      stopMotor();
      delay(500);
      right();
      delay(600);
      position = qtr.readLineBlack(sensorValues);
      //    Serial.println(position);
      //      for (uint8_t i = 0; i < 3; i++)
      //        {
      //          Serial.print(sensorValues[i]);
      //          Serial.print('\t');
      //        }
      maxVal = sensorValues[0];
      maxValLoc = 0;
      for (int i = 0; i < 3; i++) {
        if (sensorValues[i] > maxVal) {
          maxVal = sensorValues[i];
          maxValLoc = i;
        }
      }
      //      Serial.println(maxValLoc);
      while (maxValLoc != 0 && sensorValues[0] > 500) {

        //        for (uint8_t i = 0; i < 3; i++)
        //        {
        //          Serial.print(sensorValues[i]);
        //          Serial.print('\t');
        //        }

        right();
        position = qtr.readLineBlack(sensorValues);
        //      Serial.println(position);
        maxVal = sensorValues[0];
        maxValLoc = 0;
        for (int i = 0; i < 3; i++) {
          if (sensorValues[i] > maxVal) {
            maxVal = sensorValues[i];
            maxValLoc = i;
          }
        }
      }
      forward();
      delay(100);
      //        Serial.println(maxValLoc);
    }
    //Serial.print("Line Passed: ");
    //Serial.println(linePassed);
    //measured_distance = UltrasonicSensor();
    if (measured_distance < 120 && linePassed == 4) {
      linePassed = linePassed + 1;
      forward();
      delay(100);
      stopMotor();
      delay(500);
      right();
      delay(10000);
      position = qtr.readLineBlack(sensorValues);
      //    Serial.println(position);
      //      for (uint8_t i = 0; i < 3; i++)
      //        {
      //          Serial.print(sensorValues[i]);
      //          Serial.print('\t');
      //        }
      maxVal = sensorValues[0];
      maxValLoc = 0;
      for (int i = 0; i < 3; i++) {
        if (sensorValues[i] > maxVal) {
          maxVal = sensorValues[i];
          maxValLoc = i;
        }
      }
      //      Serial.println(maxValLoc);
      while (maxValLoc != 0 && sensorValues[0] > 500) {

        //        for (uint8_t i = 0; i < 3; i++)
        //        {
        //          Serial.print(sensorValues[i]);
        //          Serial.print('\t');
        //        }

        right();
        position = qtr.readLineBlack(sensorValues);
        //      Serial.println(position);
        maxVal = sensorValues[0];
        maxValLoc = 0;
        for (int i = 0; i < 3; i++) {
          if (sensorValues[i] > maxVal) {
            maxVal = sensorValues[i];
            maxValLoc = i;
          }
        }
      }
      forward();
      delay(100);
      //        Serial.println(maxValLoc);
    }
    //Serial.print("Line Passed: ");
    //Serial.println(linePassed);
  }
}
