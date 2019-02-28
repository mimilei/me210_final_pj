#include <Arduino.h>
#include <Metro.h>

/**
 * ME210 Final Project Powertrain Software
 * */

// Method headers
void driveNorth(void);
void driveEast(void);
void driveSouth(void);
void driveWest(void);
void driveStop(void);
uint8_t motorTimerExpired(void);
void nextState(void);

// State Definitions
typedef enum {
  DRIVE_NORTH, DRIVE_EAST, DRIVE_SOUTH, DRIVE_WEST, STOP // TO FINISH FILLING OUT
} States_t;

// Global variables
int pwmNorthSouth = 16;
int pwmEastWest = 17;
// On the L298N motor driver, there are two direction pins per motor and they
// must have opposing polarity for the motor to run.
int dirPin1 = 3;
int dirPin2 = 4;
int maxSpeedMotor = 255;
int stopSpeedMotor = 0;
int motorTestInterval = 5000;  // Milliseconds.

States_t driveState;
static Metro motorTimer = Metro(motorTestInterval);

void setup() {
  Serial.begin(9600);
  pinMode(pwmNorthSouth, OUTPUT);
  pinMode(pwmEastWest, OUTPUT);
  analogWrite(pwmNorthSouth, maxSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor);
  pinMode(dirPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  
  driveState = STOP;
}

void loop() {
  if (motorTimerExpired()) nextState();
}

void nextState() {
  switch (driveState) {
    case STOP:
      driveState = DRIVE_NORTH;
      break;
    case DRIVE_NORTH:
      driveState = DRIVE_EAST;
      break;
    case DRIVE_EAST:
      driveState = DRIVE_SOUTH;
      break;
    case DRIVE_SOUTH:
      driveState = STOP;
      break;
    default:
      break;
  }
  Serial.println(driveState);
}

void driveNorth() {
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  analogWrite(pwmNorthSouth, maxSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor); 
}

void driveEast() {
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, maxSpeedMotor); 
}

void driveSouth() {
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  analogWrite(pwmNorthSouth, maxSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor); 
}

void driveWest() {
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, maxSpeedMotor); 
}

void driveStop() {
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor);
}

uint8_t motorTimerExpired() {
  return (uint8_t) motorTimer.check();
}
