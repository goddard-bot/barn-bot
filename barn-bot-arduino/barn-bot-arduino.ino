#include <SPI.h>
#include <RH_NRF24.h>
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

RH_NRF24 nrf24(48, 53);

const int buttonPin = 2;
const int pingPin = 7;

int buttonState = 0; 
int i = 0;
int motorSpeedA = 0;
int motorSpeedB = 0;

void setup() 
{
  Serial.begin(9600);

  if (!nrf24.init())
    Serial.println("init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

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
//  Serial.println("Sending to nrf24_server");
//  // Send a message to nrf24_server
//  uint8_t data[16];
//  itoa(i, data, 10);
//  nrf24.send(data, sizeof(data));
//  
//  nrf24.waitPacketSent();
//  // Now wait for a reply
//  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
//  uint8_t len = sizeof(buf);
//  if (nrf24.waitAvailableTimeout(500))
//  { 
//    // Should be a reply message for us now   
//    if (nrf24.recv(buf, &len))
//    {
//      Serial.print("got reply: ");
//      Serial.println((char*)buf);
//    }
//    else
//    {
//      Serial.println("recv failed");
//    }
//  }
//  else
//  {
//    Serial.println("No reply, is nrf24_server running?");
//  }
//  delay(400);
   
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
  val = map(val, 0, 500, 255, 0);
  if(val < 0) {
    val = 0;
  }
  Serial.println(val);

  analogWrite(REDPIN, val);

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

