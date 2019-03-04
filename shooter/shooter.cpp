#include <Arduino.h>
#include <AccelStepper.h>
#include <Servo.h>

/**ball gating code
 * */

void setHigh(void);
void setLow(void);
void calculateDutyCycle(void);
void openshutGate(void);

int pwmOutPin = 10; //Motor 1 Enable Pin 
int directionPin = 9; //Motor 1 Direction Pin
int directionState = HIGH; //Motor Direction State
int pwmOutPin_2 = 4; //Motor 2 Enable Pin
int directionPin2 = 3; //Motor 2 Direction State

int servoPin = 7; 
Servo ballGater;
volatile int servoState = 0;
int ballInterval = 1000000;
int shutInterval = 90000;
IntervalTimer ballTimer;

float timePerPeriod = 1./10000; // For 1kHz
float highTimerInterval = timePerPeriod*pow(10,6); // For 1 kHz frequency
IntervalTimer lowTimer;
volatile float lowTimerInterval = timePerPeriod/2*pow(10,6); // microseconds

void setup() {
  Serial.begin(9600);
  pinMode(pwmOutPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(pwmOutPin_2, OUTPUT);
  pinMode(directionPin2,OUTPUT);
  pinMode(servoPin, OUTPUT);
  
  digitalWrite(directionPin, HIGH);
  digitalWrite(directionPin2, LOW);
  analogWrite(pwmOutPin, 255);

  ballGater.attach(servoPin);
  ballGater.write(20);
  ballTimer.begin(openshutGate, ballInterval);
}

void loop() {
  int dutyCycle = 255;
  analogWrite(pwmOutPin, dutyCycle);
  analogWrite(pwmOutPin_2, dutyCycle);
}

void openshutGate() {
  if (servoState == 0) {
    ballGater.write(15);
    servoState = 1;
    ballTimer.update(shutInterval);
  } else {
    ballGater.write(35);
    servoState = 0;
    ballTimer.update(ballInterval);
  }
}