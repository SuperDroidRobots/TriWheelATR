//=========================HEADER=============================================================
/*

   Tri-Wheel Vectoring Robot Motor Example
   AUTHOR: Jason Traud
   November 5, 2013
   
   This code uses an Arduino Uno mounted on a Tri-Wheel Vectoring platform
   TP-093-003. The robot commands three PWM motor controllers (TE-058-000)
   to control three independent omni-directional wheels. 

   The goal of this firmware is to cycle through the possible motor movements
   to provide a base for future autonomous routines.  
   
   
   Hardware: Arduino Uno R3      (MCU-050-000)
             Tri-Wheel ATR       (TP-251-003) 
             LMD18200 MCU        (TE-058-000)

   Connections: 
   
     Arduino D3   -   Motor Controller A (PWM Input)
     Arduino D2   -   Motor Controller A (Direction Input)

     Arduino D5   -   Motor Controller B (PWM Input)
     Arduino D9   -   Motor Controller B (Direction Input)

     Arduino D6   -   Motor Controller C (PWM Input)
     Arduino D10  -   Motor Controller C (Direction Input)
     
   Support:
     
     Vectoring Robot Support Page:
     http://www.superdroidrobots.com/shop/custom.aspx/vectoring-robots/44/
     
     SuperDroid Robots Forum:
     http://www.sdrobots.com/forums/
	
   License: CCAv3.0 Attribution-ShareAlike (http://creativecommons.org/licenses/by-sa/3.0/)
   You're free to use this code for any venture. Attribution is greatly appreciated. 

//============================================================================================
*/

// ****************************************************
// Libraries
// ****************************************************
#include "Arduino.h"

// ****************************************************
// Motor Controller Ports
// ****************************************************
#define pwmA 3
#define dirA 2

#define pwmB 5
#define dirB 9

#define pwmC 6
#define dirC 10

// ****************************************************
// Motor Controllers
// ****************************************************
typedef struct {
  int pulse;
  bool direction;
} MotorValues;

MotorValues motorA;
MotorValues motorB;
MotorValues motorC;

// Globals
double sideStep = 0.60; // Limiting factor to ensure direct side to side movement

// ****************************************************
// Stops the motors
// RETURNS: none
// ****************************************************
void allStop() {
 analogWrite(pwmA, 0);
 analogWrite(pwmB, 0);
 analogWrite(pwmC, 0);
}

// ****************************************************
// Sets the PWM motor values
// RETURNS: none
// ****************************************************
void commandMotors() {
 analogWrite(pwmA, motorA.pulse);
 analogWrite(pwmB, motorB.pulse);
 analogWrite(pwmC, motorC.pulse);
}

// ****************************************************
// Forward motor movement
// RETURNS: none
// ****************************************************
void forwardMovement() {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, HIGH); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = 0; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }  
  allStop();
}

// ****************************************************
// Backward motor movement
// RETURNS: none
// ****************************************************
void backwardMovement() {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, LOW); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = 0; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Right motor movement
// RETURNS: none
// ****************************************************
void rightMovement () {  
  // Set motor directions
  digitalWrite(dirA, LOW); digitalWrite(dirB, HIGH); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = int (i * sideStep); motorC.pulse = int (i * sideStep); commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Left motor movement
// RETURNS: none
// ****************************************************
void leftMovement () {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, LOW); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = int (i * sideStep); motorC.pulse = int (i * sideStep); commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Spin Clowise motor movement
// RETURNS: none
// ****************************************************
void spinClockwise() {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, HIGH); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Spin Counter Clockwise motor movement
// RETURNS: none
// ****************************************************
void spinCounterClockwise () {
  // Set motor directions
  digitalWrite(dirA, LOW); digitalWrite(dirB, LOW); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Initial setup function, called once
// RETURNS: none
// ****************************************************
void setup() {
  
  // Debug Serial
  Serial.begin(115200);		

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

// ****************************************************
// Main program loop
// RETURNS: none
// ****************************************************
void loop() {
  forwardMovement();
  backwardMovement();
  rightMovement();
  leftMovement();
  spinClockwise();
  spinCounterClockwise();
}
