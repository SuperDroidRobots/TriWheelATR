/*********************************************************

Tri-Wheel Vectoring Robot Example
Jason Traud
SuperDroid Robots
November 5, 2013

This code uses an Arduino Uno mounted on a Tri-Wheel Vectoring platform
TP-093-003. The robot commands three PWM motor controllers (TE-058-000)
to control three independent omni-directional wheels. 

The goal of this firmware is to cycle through the possible motor movements
to provide a base for future autonomous routines. 

Platform:
http://www.superdroidrobots.com/shop/item.aspx/ig32-triangular-omni-wheel-vectoring-robot-platform/1458/

Motor Controller:
http://www.superdroidrobots.com/shop/item.aspx/pwm-motor-controller-3a-12-55v/583/

Vectoring Robot Support Page:
http://www.superdroidrobots.com/shop/custom.aspx/vectoring-robots/44/

***********************************************************/

// Define pins for PWM motor controller
#define pwmA 3
#define dirA 2
#define brkA 7

#define pwmB 5
#define dirB 9
#define brkB 8

#define pwmC 6
#define dirC 10
#define brkC 11

// Include necessary header files
#include "Arduino.h"

// Command struct for motor controllers
typedef struct {
	int pulse;
	bool direction;
	bool brake;
}	MotorValues;

// Defines structs for each motor
MotorValues motorA;
MotorValues motorB;
MotorValues motorC;

// Globals
double sideStep = 0.60; // Limiting factor to ensure direct side to side movement


void setup() {
  
  // Uncomment the serial command to enable debugging through serial monitor
  // Serial.begin(9600);		// initialize serial communication at 9600 bps

  // Set motor controller communication pins as outputs
  pinMode(dirA, OUTPUT);
  pinMode(brkA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(brkB, OUTPUT);
  pinMode(dirC, OUTPUT);
  pinMode(brkC, OUTPUT);
  
  // Command all motors to stop
  allStop();
 }

// Stops the Robot
void allStop() {
	analogWrite(pwmA, 0);
	analogWrite(pwmB, 0);
	analogWrite(pwmC, 0);
}

void commandMotors() {
  	analogWrite(pwmA, motorA.pulse);
	analogWrite(pwmB, motorB.pulse);
	analogWrite(pwmC, motorC.pulse);
}

void forwardMovement() {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, HIGH); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = 0; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }  
  allStop();
}

void backwardMovement() {
  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, LOW); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = 0; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }
  allStop();
}

void rightMovement () {
  
  // Set motor directions
  digitalWrite(dirA, LOW); digitalWrite(dirB, HIGH); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = int (i * sideStep); motorC.pulse = int (i * sideStep); commandMotors(); delay(25); }
  allStop();
}

void leftMovement () {
  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, LOW); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = int (i * sideStep); motorC.pulse = int (i * sideStep); commandMotors(); delay(25); }
  allStop();
}

void spinClockwise() {
  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, HIGH); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }
  allStop();
}

void spinCounterClockwise () {

  // Set motor directions
  digitalWrite(dirA, LOW); digitalWrite(dirB, LOW); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }
  allStop();
}

void loop() 
{
  forwardMovement();
  backwardMovement();
  rightMovement();
  leftMovement();
  spinClockwise();
  spinCounterClockwise();
}
