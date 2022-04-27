#include <QTRSensors.h>
#include <PID_v1.h>
// Motor Pins
/*          41  8  5  4
    left-----Line Follower---------right  Watch from Here for CW and CCW
    |                             |  CW is forward CCW is backward
    |                             |
    -------------------------------
*/


// Motor 1
const int left_speed = 9;
const int left_in1 = 23;
const int left_in2 = 22;

// Motor 3
const int right_speed = 7;
const int right_in2 = 12;
const int right_in1 = 13;

// Line Follower Stuff
QTRSensors qtr;
const uint8_t SensorCount = 5;
uint16_t sensorValues[SensorCount];
int linePassed = 0;
int start_flag = 0;


// PID Stuff
double KP = 0.03;
double KD = 0.05; // we are dividing by the time step (
double KI = 0; // If we are using Integral Control

double setpoint = 2000;
double input, output;
PID myPID(&input, &output, &setpoint, KP, KI, KD, DIRECT);

// Ultrasonic Stuff
// Ultrasonic Stuff
#define echoPin 52
#define trigPin 48
// defines variables for Ultrasonic Sensor
long int duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int measured_distance;
void stopMotor() {
  Serial.println("Stop Motor");
  digitalWrite(left_in1, LOW); //Stop for Motor 1
  digitalWrite(left_in2, LOW);

  digitalWrite(right_in2, LOW); //Stop for Motor 2
  digitalWrite(right_in1, LOW);

}

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
  start_flag = 1;
}

void setup() {
  // put your setup code here, to run once:

  // put your main code here, to run repeatedly:
  myPID.SetOutputLimits(-20, 20);

  //Ultrasonic Sensor Pin Declarations
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  // Motor Setup
  pinMode(left_speed, OUTPUT);
  pinMode(right_speed, OUTPUT);

  pinMode(left_in1, OUTPUT);
  pinMode(right_in1, OUTPUT);

  pinMode(left_in2, OUTPUT);
  pinMode(right_in2, OUTPUT);


  //Line Follower Stuff
  // configure the sensors
  qtr.setTypeRC();
  // qtr.setSensorPins((const uint8_t[]){3, 4, 5, 6, 7, 8, 9, 10}, SensorCount);

  // Line Sensor
  // Line 1 = 4
  // Line 2 = 49
  // Line 3 = 5
  // Line 4 = 41
  // Line 5 = 8
  qtr.setSensorPins((const uint8_t[]) {
    4, 49, 5, 41, 8
  }, SensorCount);
  // qtr.setEmitterPin(2);

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
void loop() {

  if (start_flag == 1) {
    int sensors[5];
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


    // ULTRASONIC DISTANCE
    measured_distance = UltrasonicSensor();

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
  }
  else {
    initial_move();
  }
}
