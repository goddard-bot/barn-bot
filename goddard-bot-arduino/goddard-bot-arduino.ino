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
const int pinCE = 48; 
const int pinCSN = 53;

int buttonState = 0; 
int motorSpeedA = 0;
int motorSpeedB = 0;
int redVal;
int greenVal;
int blueVal;
int redMax;
int greenMax;
int blueMax;
int pCount = 0; 
int flag = 0;
int red8;
int green8;
int blue8;

byte gotByte = 0; 
volatile int count = 0; 
RF24 wirelessSPI(pinCE, pinCSN); 
const uint64_t pAddress = 0xB00B1E5000LL;  
uint8_t buffer [6];

double mm;
unsigned long lastCommand;
unsigned long lastEmotion;
unsigned long time;

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
  red(3);
  green(1);
  blue(1);
}
void loop()
{
  time = millis();
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

  inches = microsecondsToInches(duration);
  mm = microsecondsToMillimeters(duration);

  pulse();

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    demo();
  }

  if(time - lastCommand > 30000) {
    fidget();
    Serial.println(lastCommand);
    lastCommand = millis();
  } else if(time - lastEmotion > 5000) {
    showEmotion();
    lastEmotion = millis();
  }
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

void spin() {
  digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    motorSpeedA = 255;
    motorSpeedB = 255;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(1000);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void forwardSlow() {
  digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    motorSpeedA = 80;
    motorSpeedB = 80;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(20);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void backwardsSlow() {
  digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    motorSpeedA = 80;
    motorSpeedB = 80;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(20);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void leftSlow() {
  digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    motorSpeedA = 80;
    motorSpeedB = 80;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(20);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void rightSlow() {
  digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    motorSpeedA = 80;
    motorSpeedB = 80;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
    delay(20);
    
    motorSpeedA = 0;
    motorSpeedB = 0;
    analogWrite(enA, motorSpeedA); 
    analogWrite(enB, motorSpeedB);
}

void fidget() {
  if(random(0, 10) < 5) {
    for(int i = 0; i < random(1, 3); i++) {
      forwardSlow();
    }
    for(int i = 0; i < random(0, 3); i++) {
      leftSlow();
    }
    for(int i = 0; i < random(0, 3); i++) {
      rightSlow();
    }
    for(int i = 0; i < random(0, 3); i++) {
      backwardsSlow();
    }
  }
  else {
    for(int i = 0; i < random(0, 3); i++) {
      backwardsSlow();
    }
    for(int i = 0; i < random(0, 3); i++) {
      rightSlow();
    }
    for(int i = 0; i < random(1, 3); i++) {
      leftSlow();
    }
    for(int i = 0; i < random(0, 3); i++) {
      forwardSlow();
    }
  }
  lastCommand = millis();
  Serial.println(millis());
}

void demo() {
  for(int i = 0; i < 10; i++) {
    forward();
  }
  for(int i = 0; i < 2; i++) {
    left();
  }
  for(int i = 0; i < 10; i++) {
    forward();
  }
  yes();
  yes();
  no();
  rainbow();
  rainbow();
  spin();
}

void rainbow() {
  for(int i = 0; i < 10; i++) {
    red(i);
    green(9 - i);
    blue(0);
    delay(80);
  }
  for(int i = 0; i < 10; i++) {
    red(9 - i);
    green(0);
    blue(i);
    delay(80);
  }
  for(int i = 0; i < 10; i++) {
    red(0);
    green(i);
    blue(9 - i);
    delay(80);
  }
}
void red(int red) {
  redVal = map(red, 0, 9, 0, 255);
  analogWrite(REDPIN, redVal);
  redMax = redVal;
  red8 = red;
}

void green(int green) {
  greenVal = map(green, 0, 9, 0, 175);
  analogWrite(GREENPIN, greenVal);
  greenMax = greenVal;
  green8 = green;
}

void blue(int blue) {
  blueVal = map(blue, 0, 9, 0, 80);
  analogWrite(BLUEPIN, blueVal);
  blueMax = blueVal;
  blue8 = blue;
}

void pulse() {
  
  int newRedVal = mm;
  newRedVal = map(newRedVal, 0, 300, redMax, 0);
  if(newRedVal < 20) {
    newRedVal = 20;
  }
  int newGreenVal = mm;
  newGreenVal = map(newGreenVal, 0, 300, greenMax, 0);
  if(newGreenVal < 5) {
    newGreenVal = 5;
  }
  int newBlueVal = mm;
  newBlueVal = map(newBlueVal, 0, 300, blueMax, 0);
  if(newBlueVal < 1) {
    newBlueVal = 1;
  }

  analogWrite(REDPIN, newRedVal);
  analogWrite(GREENPIN, newGreenVal);
  analogWrite(BLUEPIN, newBlueVal);
}

void showEmotion() {
  analogWrite(REDPIN, redMax);
  analogWrite(GREENPIN, greenMax);
  analogWrite(BLUEPIN, blueMax);
  delay(200);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);
  delay(40);
  analogWrite(REDPIN, redMax);
  analogWrite(GREENPIN, greenMax);
  analogWrite(BLUEPIN, blueMax);
  delay(200);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);
  delay(40);
  red(red8);
  green(green8);
  blue(blue8);
}

void no() {
  analogWrite(REDPIN, 150);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);
  delay(100);
  analogWrite(REDPIN, 0);
  delay(40);
  analogWrite(REDPIN, 150);
  delay(100);
  analogWrite(REDPIN, 0);
  delay(40);
  red(red8);
  green(green8);
  blue(blue8);
}

void yes() {
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 150);
  analogWrite(BLUEPIN, 0);
  delay(100);
  analogWrite(GREENPIN, 0);
  delay(40);
  analogWrite(GREENPIN, 150);
  delay(100);
  analogWrite(GREENPIN, 0);
  delay(40);
  red(red8);
  green(green8);
  blue(blue8);
}

void sleep() {
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  for(int i = 0; i < 6; i++) {
    analogWrite(BLUEPIN, 20);
    delay(2000);
    analogWrite(BLUEPIN, 0);
    delay(1000);
  }
  red(red8);
  green(green8);
  blue(blue8);
}

void interruptFunction() {
 while(wirelessSPI.available()) { 
       wirelessSPI.read( buffer, 5 );
 }
 flag = 1;
 detachInterrupt(1);
 lastCommand = millis();
}

void bufferUpdated() {
  if(buffer[0] == 'B') {
    switch(buffer[4]) {
      case 'A'  :
        Serial.print('A');
         break;
      case 'F'  :
        if(mm > 200) {
          forward();
        } else {
          no();
          if(random(0, 10) < 5) {
            right();
          }
          else {
            left();
          }
          backwards();
        }
         break;
      case 'R'  :
        backwards();
         break;
      case 'C'  :
        right();
         break;
      case 'S'  :
        spin();
         break;
      case 'W'  :
        left();
         break;
      case 'N'  :
        no();
         break;
      case 'Y'  :
        yes();
         break;
      case 'L'  :
        sleep();
         break;
      case 'D'  :
        demo();
         break;
      case 'O'  :
        rainbow();
         break;
      case 'Z'  :
         break;
    }
    int redIn = buffer[1] - '0';
    red(redIn);
    int greenIn = buffer[2] - '0';
    green(greenIn);
    int blueIn = buffer[3] - '0';
    blue(blueIn);
  }
  
  Serial.write((char*)buffer);
  Serial.println();
  flag = 0;
  attachInterrupt(1, interruptFunction, FALLING);
  lastCommand = millis();
}
