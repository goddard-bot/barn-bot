#include <SPI.h> 
#include <nRF24L01.h> 
#include <RF24.h> 
#include <avr/sleep.h>
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 4
#define enA 9
#define in1 22
#define in2 23
#define enB 8
#define in3 24
#define in4 25

const int buttonPin = 2;
const int pingPin = 7;

int buttonState = 0; 
int i = 0;
int motorSpeedA = 0;
int motorSpeedB = 0;
int redVal;
int greenVal;
int blueVal;

const int pinCE = 48; 
const int pinCSN = 53; 
byte gotByte = 0; 
volatile int count = 0; 
int pCount = 0; 
RF24 wirelessSPI(pinCE, pinCSN); 
const uint64_t pAddress = 0xB00B1E5000LL;  
uint8_t buffer [6];
int flag = 0;
double mm;
int redMax;
int greenMax;
int blueMax;

void setup() 
{
  Serial.begin(9600); 
  wirelessSPI.begin();  //Start the nRF24 module
  wirelessSPI.setAutoAck(1);                    // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
  wirelessSPI.enableAckPayload();         //allows you to include payload on ack packet
  wirelessSPI.maskIRQ(1,1,0);               //mask all IRQ triggers except for receive (1 is mask, 0 is no mask)
  wirelessSPI.setPALevel(RF24_PA_LOW); //Set power level to low, won't work well at higher levels (interfer with receiver)
  wirelessSPI.setDataRate(RF24_250KBPS);
  wirelessSPI.openReadingPipe(1,pAddress);      //open pipe o for recieving meassages with pipe address
  wirelessSPI.startListening();                 // Start listening for messages
  attachInterrupt(1, interruptFunction, FALLING);

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}
void loop()
{
  
  while(flag == 1) {
    bufferUpdated();
  }   
  
  long duration, inches;
  

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

//  Serial.print(inches);
//  Serial.print("in, ");
//  Serial.print(mm);
//  Serial.print("mm");
//  Serial.println();

//  int val = mm;
//  val = map(val, 0, 500, 255, 0);
//  if(val < 0) {
//    val = 0;
//  }
//  Serial.println(val);

//  analogWrite(REDPIN, val);

  pulse();

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH && mm > 200) {
    forward();
    Serial.println("on");
  } else {
    motorSpeedA = 0;
    motorSpeedB = 0;
  }
  analogWrite(enA, motorSpeedA); 
  analogWrite(enB, motorSpeedB); 
  i++;
  
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

double microsecondsToMillimeters(long microseconds) {
  return microseconds / 2.9 / 2;
}

void forward() {
  digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    motorSpeedA = 255;
    motorSpeedB = 255;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(100);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void backwards() {
  digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    motorSpeedA = 255;
    motorSpeedB = 255;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(100);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void left() {
  digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    motorSpeedA = 255;
    motorSpeedB = 255;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(100);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void right() {
  digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    motorSpeedA = 255;
    motorSpeedB = 255;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(100);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void red(int red) {
  redVal = map(red, 0, 9, 0, 100);
  analogWrite(REDPIN, redVal);
  redMax = redVal;
  Serial.println(red);
}

void green(int green) {
  greenVal = map(green, 0, 9, 0, 100);
  analogWrite(GREENPIN, greenVal);
  greenMax = greenVal;
}

void blue(int blue) {
  blueVal = map(blue, 0, 9, 0, 100);
  analogWrite(BLUEPIN, blueVal);
  blueMax = blueVal;
}

void pulse() {

  int newRedVal = mm;
  newRedVal = map(newRedVal, 0, 500, redMax, 0);
  if(newRedVal < 0) {
    newRedVal = 0;
  }
  int newGreenVal = mm;
  newGreenVal = map(newGreenVal, 0, 500, greenMax, 0);
  if(newGreenVal < 0) {
    newGreenVal = 0;
  }
  int newBlueVal = mm;
  newBlueVal = map(newBlueVal, 0, 500, blueMax, 0);
  if(newBlueVal < 0) {
    newBlueVal = 0;
  }

  analogWrite(REDPIN, newRedVal);
  analogWrite(GREENPIN, newGreenVal);
  analogWrite(BLUEPIN, newBlueVal);
}

void interruptFunction() {
 while(wirelessSPI.available()) { 
       wirelessSPI.read( buffer, 5 );
 }
 flag = 1;
 detachInterrupt(1);
}

void bufferUpdated() {
  if(buffer[0] == 'B') {
    int redIn = buffer[1] - '0';
    red(redIn);
    int greenIn = buffer[2] - '0';
    green(greenIn);
    int blueIn = buffer[3] - '0';
    blue(blueIn);
    
    switch(buffer[4]) {
      case 'A'  :
        Serial.print('A');
         break;
      case 'F'  :
        if(mm > 200) {
          forward();
        } else {
          backwards();
          if(random(0, 10) < 5) {
            right();
          }
          else {
            left();
          }
        }
         break;
      case 'R'  :
        backwards();
         break;
      case 'C'  :
        right();
         break;
      case 'W'  :
        left();
         break;
    }

    Serial.println("i got it");
  }
  
  Serial.write((char*)buffer);
  Serial.println();
  flag = 0;
  attachInterrupt(1, interruptFunction, FALLING);
}
