#include <QTRSensors.h>

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


// Ultrasonic Stuff
#define echoPin 52
#define trigPin 48
// defines variables for Ultrasonic Sensor
long int duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int measured_distance;
int start_flag = 0;

void setup() {
  // put your setup code here, to run once:
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
  qtr.setSensorPins((const uint8_t[]) {4,49,5,41,8}, SensorCount);
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
  delay(1000);
}


// Motor Move Function like forwards right left etc

void forward() {
  int forward_speed_left = 140;
  int forward_speed_right = 120;
  Serial.println("Going Straight");
  digitalWrite(left_in1, LOW); //Clockwise for Motor 1
  digitalWrite(left_in2, HIGH);
  analogWrite(left_speed, forward_speed_left);

  digitalWrite(right_in1, HIGH); // clockwise for right
  digitalWrite(right_in2, LOW);
  analogWrite(right_speed, forward_speed_right);
}

void adjRight() {
  Serial.println("Shift Right");
  int right_speed_left = 95;
  int right_speed_right = 75;

  digitalWrite(left_in1, LOW); //Clockwise for Motor 1
  digitalWrite(left_in2, HIGH);
  analogWrite(left_speed, right_speed_left);

  digitalWrite(right_in1, LOW); // clockwise for right
  digitalWrite(right_in2, HIGH);
  analogWrite(right_speed, right_speed_right);

}

void adjLeft() {

  Serial.println("Shift Left");
  int left_speed_left = 95;
  int left_speed_right = 75;
  
  digitalWrite(left_in1, HIGH); //Clockwise for Motor 1
  digitalWrite(left_in2, LOW);
  analogWrite(left_speed, left_speed_left);


  digitalWrite(right_in1, HIGH); // clockwise for right
  digitalWrite(right_in2, LOW);
  analogWrite(right_speed, left_speed_right);

}
void right() {
  int rightturn_speed_left = 95;
  int rightturn_speed_right = 75;
  Serial.println("Full Right");

  digitalWrite(left_in1, LOW); //CCW for Motor 1
  digitalWrite(left_in2, HIGH);

  analogWrite(left_speed, rightturn_speed_left);


  digitalWrite(right_in1, LOW); // CW for right
  digitalWrite(right_in2, HIGH);
  analogWrite(right_speed, rightturn_speed_right);

}
void stopMotor() {
  Serial.println("Stop Motor");
  digitalWrite(left_in1, LOW); //Stop for left
  digitalWrite(left_in2, LOW);

  digitalWrite(right_in1, LOW); // Stop for right
  digitalWrite(right_in2, LOW);
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
  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm");
  return distance;
}

void loop() {

  // put your main code here, to run repeatedly:

  float KP = 0.001;
  float KD = 0; // we are dividing by the time step (
  float KI = KP/10; // If we are using Integral Control

  float elapsedTime;
  float currentTime;
  float previousTime;



  currentTime = millis();
  elapsedTime = currentTime - previousTime;

  float left_baseSpeed = 130;
  float right_baseSpeed = 150;

  float lastError = 0;
  int sensors[5];


  /*  Things that may cause issues, turning may still have to be somewhat hardcoded beceause the line follower might see the black line
      from two corners. Need to test that and ensure that is not a problem. Maybe using 8 sensors is overkill, need to see that too.

  */

  // Currently just 3 right turns
  // First Turn

  measured_distance = UltrasonicSensor();

  //  if (measured_distance < 57 && linePassed < 3) {
  //
  //    linePassed = linePassed + 1;
  //    forward();
  //    delay(100);
  //    stopMotor();
  //    delay(500);
  //    right();
  //    delay(600);
  //
  //  }

  // Get calibrated sensor values returned in the sensors array, along with the
  // line position, which will range from 0 to 2000, with 1000 corresponding to
  // a position under the middle sensor


  int position = qtr.readLineBlack(sensors);

  // Compute our "error" from the line position. We will make it so that the
  // error is zero when the middle sensor is over the line, because this is our
  // goal. Error will range from -1000 to +1000. If we have sensor 0 on the left
  // and sensor 2 on the right, a reading of -1000 means that we see the line on
  // the left and a reading of +1000 means we see the line on the right.

  int error = position - 2000; // Maybe set to 1000 and see for middle
  int cumError;
  cumError = error + (error * elapsedTime);

  Serial.print(error); // Our actual error
  Serial.print("        ");

  // Set the motor speed based on proportional and derivative PID terms:
  // KP is the floating-point proportional constant (maybe start with a value around 0.1)
  // KD is the floating-point derivative constant (maybe start with a value around 5)
  // Note that when doing PID, it is very important you get your signs right, or
  // else the control loop will be unstable.
  float motorSpeed = (KP * error) + (KD * ((error - lastError) / (elapsedTime))) + (KI * cumError);
  lastError = error;


  // left and M2 are base motor speeds (the speeds the motors should run if you
  // are perfectly on the line with no error). If your motors are well matched,
  // left and M2 will be equal. When you start testing your PID loop, it might
  // help to start with small values for left and M2. You can then increase the
  // speed as you fine-tune your PID constants KP and KD.

  float left_totalSpeed = left_baseSpeed - motorSpeed;
  float right_totalSpeed = right_baseSpeed + motorSpeed;


  
  Serial.print(left_totalSpeed);
  Serial.print("        ");
  Serial.println(right_totalSpeed);

  // It might help to keep the speeds positive (this is optional). You might
  // also want to add a similar check to keep the speeds from exceeding a
  // maximum limit.
  if (left_totalSpeed < 0) {
    left_totalSpeed = 0;
  }
  if (right_totalSpeed < 0) {
    right_totalSpeed = 0;
  }
  // set motor speeds using the two motor speed variables above

  digitalWrite(left_in2, HIGH);
  digitalWrite(left_in1, LOW);
  analogWrite(left_speed, left_totalSpeed);

  digitalWrite(right_in1, HIGH);
  digitalWrite(right_in2, LOW);
  analogWrite(right_speed, right_totalSpeed);

  lastError = error;
  previousTime = currentTime;

}
