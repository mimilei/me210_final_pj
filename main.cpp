#include <Arduino.h>
#include <AccelStepper.h>

// /**
//  * Driving the shooter motors
//  * Last updated: 2/23/2019 at 18:00 by Amanda Steffy
//  * */

// // Method headers
void setHigh(void);
void setLow(void);
void calculateDutyCycle(void);

// // Global variables
int pwmOutPin = 10; //Motor 1 Enable Pin 
//int potInPin = 16;
int directionPin = 9; //Motor 1 Direction Pin
int directionState = HIGH; //Motor Direction State
int pwmOutPin_2 = 4; //Motor 2 Enable Pin
int directionPin2 = 3; //Motor 2 Direction State
bool keyPressed = false;

// IntervalTimer highTimer;
float timePerPeriod = 1./10000; // For 1kHz
float highTimerInterval = timePerPeriod*pow(10,6); // For 1 kHz frequency
IntervalTimer lowTimer;
volatile float lowTimerInterval = timePerPeriod/2*pow(10,6); // microseconds

uint8_t TestForKey(void) {
 uint8_t KeyEventOccurred;
 KeyEventOccurred = Serial.available();
 return KeyEventOccurred;
}

void KeyPressed(void) {
 uint8_t theKey;
 Serial.println("key pressed");
 theKey = Serial.read();
 if (directionState == HIGH) {
   directionState = LOW;
 } else {
   directionState = HIGH;
 }
}

void setup() {
  Serial.begin(9600);
  pinMode(pwmOutPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(pwmOutPin_2, OUTPUT);
  pinMode(directionPin2,OUTPUT);
  digitalWrite(directionPin, HIGH);
  digitalWrite(directionPin2, HIGH);
  analogWrite(pwmOutPin, 255);
}

void loop() {
  //Serial.println("In loop");
  //int potIn = analogRead(potInPin);
  //Set duty cycle to 50%
  int dutyCycle = 255;
  analogWrite(pwmOutPin, dutyCycle);
  analogWrite(pwmOutPin_2, dutyCycle);
  if (TestForKey()) KeyPressed();
  /*if (Serial.available() > 0) {
    Serial.println("Inverting direction");
    digitalWrite(directionPin, directionState);
    if (directionState == HIGH) {
      directionState = LOW;
    } else {
      directionState = HIGH;
    }
    Serial.read();
  }*/
}