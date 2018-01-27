#include <Servo.h>
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

Servo servoLeft;         
Servo servoRight;

const int pingPin = 7;
void setup() 
{
  Serial.begin(9600);

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  servoLeft.attach(10); 
  servoRight.attach(9);
}
void loop()
{
  

  long duration, inches;
  double mm;

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  mm = microsecondsToMillimeters(duration);

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(mm);
  Serial.print("mm");
  Serial.println();

  int val = mm;
  val = map(val, 0, 175, 255, 0);
  if(val < 0) {
    val = 0;
  }
  Serial.println(val);

  analogWrite(REDPIN, val);
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

double microsecondsToMillimeters(long microseconds) {
  return microseconds / 2.9 / 2;
}

// Motion routines for forward, reverse, turns, and stop
void forward() {
  servoLeft.write(0);
  servoRight.write(180);
}

void reverse() {
  servoLeft.write(180);
  servoRight.write(0);
}

void turnRight() {
  servoLeft.write(180);
  servoRight.write(180);
}
void turnLeft() {
  servoLeft.write(0);
  servoRight.write(0);
}

void stopRobot() {
  servoLeft.write(90);
  servoRight.write(90);
}

