#include <QTRSensors.h>


// Motor Pins
/*          41  8  5  4
    ------Line Follower-----------  Watch from Here for CW and CCW
    |                             |  CW is forward CCW is backward
    |                             |
    M2---------------------------M3
*/
// Motor 2
const int m2_speed = 3;
const int m2_in1 = 24;
const int m2_in2 = 25;
// Motor 3
const int m3_speed = 6;
const int m3_in1 = 12;
const int m3_in2 = 13;


// Line Follower Stuff 
QTRSensors qtr;
const uint8_t SensorCount = 8;
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
  pinMode(m2_speed, OUTPUT);
  pinMode(m3_speed, OUTPUT);

  pinMode(m2_in1, OUTPUT);
  pinMode(m3_in1, OUTPUT);
  
  pinMode(m2_in2, OUTPUT);
  pinMode(m3_in2, OUTPUT);


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
  int forward_speed = 50;
  Serial.println("Going Straight");

  digitalWrite(m2_in2, HIGH); //clockwise for Motor 2
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, forward_speed);

  digitalWrite(m3_in1, HIGH); // clockwise for Motor3
  digitalWrite(m3_in2, LOW);
  analogWrite(m3_speed, forward_speed);

}
void right() {
  Serial.println("Full Right");

  digitalWrite(m2_in2, HIGH); //CCW for Motor 2
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, 80);

  digitalWrite(m3_in1, LOW); // CW for Motor3
  digitalWrite(m3_in2, HIGH);
  analogWrite(m3_speed, 80);
}
void stopMotor() {
  Serial.println("Stop Motor");
  
  digitalWrite(m2_in2, LOW); //Stop for Motor 2
  digitalWrite(m2_in1, LOW);

  digitalWrite(m3_in1, LOW); // Stop for Motor3
  digitalWrite(m3_in2, LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
  int KP = 0;
  int KD = 0;
  int M1 = 0;
  int M2 = 0;
  static uint16_t lastError = 0;
  uint16_t sensors[8];
  

  /*  Things that may cause issues, turning may still have to be somewhat hardcoded beceause the line follower might see the black line 
   *  from two corners. Need to test that and ensure that is not a problem. Maybe using 8 sensors is overkill, need to see that too. 
   *  
   */

   // Currently just 3 right turns
   // First Turn
  if (measured_distance < 57 && linePassed < 3) {
    linePassed = linePassed + 1;
    forward();
    delay(100);
    stopMotor();
    delay(500);
    right();
    delay(600);
  }

  // Get calibrated sensor values returned in the sensors array, along with the
  // line position, which will range from 0 to 2000, with 1000 corresponding to
  // a position under the middle sensor
  int16_t position = qtr.readLineBlack(sensors);
  // Compute our "error" from the line position. We will make it so that the
  // error is zero when the middle sensor is over the line, because this is our
  // goal. Error will range from -1000 to +1000. If we have sensor 0 on the left
  // and sensor 2 on the right, a reading of -1000 means that we see the line on
  // the left and a reading of +1000 means we see the line on the right.
  
  int16_t error = position - 4000; // Maybe set to 5000 and see for middle? 
  
  // Set the motor speed based on proportional and derivative PID terms:
  // KP is the floating-point proportional constant (maybe start with a value around 0.1)
  // KD is the floating-point derivative constant (maybe start with a value around 5)
  // Note that when doing PID, it is very important you get your signs right, or
  // else the control loop will be unstable.
  int16_t motorSpeed = KP * error + KD * (error - lastError);
  lastError = error;
  // M1 and M2 are base motor speeds (the speeds the motors should run if you
  // are perfectly on the line with no error). If your motors are well matched,
  // M1 and M2 will be equal. When you start testing your PID loop, it might
  // help to start with small values for M1 and M2. You can then increase the
  // speed as you fine-tune your PID constants KP and KD.
  int16_t m1Speed = M1 + motorSpeed;
  int16_t m2Speed = M2 - motorSpeed;
  
  digitalWrite(m2_in2, HIGH); 
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_speed, m1Speed);

  digitalWrite(m3_in1, HIGH); 
  digitalWrite(m3_in2, LOW);
  analogWrite(m3_speed, m2Speed);
  
  // It might help to keep the speeds positive (this is optional). You might
  // also want to add a similar check to keep the speeds from exceeding a
  // maximum limit.
  if (m1Speed < 0) { m1Speed = 0; }
  if (m2Speed < 0) { m2Speed = 0; }
  // set motor speeds using the two motor speed variables above

}
