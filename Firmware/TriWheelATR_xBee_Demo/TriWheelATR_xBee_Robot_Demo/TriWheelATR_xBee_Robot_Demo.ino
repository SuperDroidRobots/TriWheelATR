//=========================HEADER=============================================================
/*

   Tri-Wheel Vectoring Robot Motor Example
   AUTHOR: Jason Traud
   April 23rd, 2014
   
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
// *********************
// Define hardware pins
// *********************
#define pwmA 3
#define dirA 2

#define pwmB 5
#define dirB 9

#define pwmC 6
#define dirC 10

#include "Arduino.h"	// Standard Arduino header file

// Standard struct to hold the values that will be assigned
// to our motors
typedef struct {
	int pulse;
	bool direction;
}	MotorValues;

MotorValues motorA;
MotorValues motorB;
MotorValues motorC;

// *********************
// Internal RAM
// *********************
int deadband = 30;
int tempFB;
int tempLR;
int tempSpin;
double tempPulseA;
double tempPulseB;
double tempPulseC;
int tempMax;
double tempScale;
int nFB = 0;
int nLR = 0;
int nSpin = 0;
double sideStep = 0.60;

bool revFB;
bool revLR;
bool revSpin;

bool sdrFound = false;		// Master Flag for SDR
bool sFound = false;		// Flag for SDR
bool dFound = false;		// Flag for SDR
bool rFound = false;		// Flag for SDR

// *********************
// Recieved RAM
// *********************
byte byteFB;
byte byteLR;
byte byteSpin;
byte Digital1;
byte checksum;
byte checksum1;
int notReady = 0;

// *********************
// Communication Related RAM
// *********************
byte Bad_Tx;
word No_Tx = 0;
word chksum1;
byte chksumTest;
byte mask;
int count;

// *********************
// Networking Buffer
// *********************
unsigned char inputBuffer[60];				// Buffer to hold the incoming data
unsigned char head;							// oldest written byte
unsigned char tail;							// newest written byte
unsigned char inputBufferTemp;

//******************************************************************************
// Sets up our serial com, hardware pins and initialize misc flags
// RETURNS: Nothing
//******************************************************************************
void setup() {

  // initialize serial communication at 9600 bps
  Serial.begin(9600);      
                          
  pinMode(dirA, OUTPUT);  
  pinMode(dirB, OUTPUT);  
  pinMode(dirC, OUTPUT);  

  // Make sure all motors are stopped for safety
  allStop();              
  
  // Initialize networking variables
  head = 0;
  tail = 0;
}

//******************************************************************************
// Main program loop where we check for incomming packets and read them. 
// Also handles timeouts
// RETURNS: Nothing
//******************************************************************************
void loop() {

  // Wait for serial
  if (Serial.available() > 0) {

  if (!sFound) {
    inputBufferTemp = Serial.read();
    if(inputBufferTemp == 0x53) { sFound = true; } 
    else { sFound = false; dFound = false; rFound = false; sdrFound = false; }
  }
    
  if (!dFound) {
    inputBufferTemp = Serial.read();
    if(inputBufferTemp == 0x44) { dFound = true; } 
    else { sFound = false; dFound = false; rFound = false; sdrFound = false; }
  }
    
  if (!rFound) {
    inputBufferTemp = Serial.read();
    if(inputBufferTemp == 0x52) { rFound = true; sdrFound = true;} 
    else { sFound = false; dFound = false; rFound = false; sdrFound = false; }
  }

  if (sdrFound && (Serial.available()  > 4 )) {
      
    // store bytes into the appropriate variables
    byteFB    = Serial.read();
    byteLR    = Serial.read();
    byteSpin  = Serial.read();
    Digital1  = Serial.read();
    checksum  = Serial.read();
      
    // Clear flags
    sdrFound = false;
    sFound = false; 
    dFound = false; 
    rFound = false;
    
    chksumTest = byteFB + byteLR + byteSpin + Digital1;
    
    // Checksum validation
    if (chksumTest != checksum) {	
      return;	
    }
    
    // Since our Checksum passed, we can clear our counter  
    No_Tx = 0;
      
      processMotors();
    } 
    else { rxTimeout(); }
  } else { rxTimeout(); }
}

//******************************************************************************
// Stops the robot when called
// RETURNS: Nothing
//******************************************************************************
void allStop() {
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);
  analogWrite(pwmC, 0);
}

//******************************************************************************
// Processes Motor commands from input serial signal
// RETURNS: Nothing
//******************************************************************************
void processMotors() {
  
  // Retrive reverse bits and apply
  mask = B00000010;
  if(Digital1 & mask) { nFB = (int)byteFB; }
  else { nFB = -1 * (int)byteFB; }
  
  mask = B00000100;
  if(Digital1 & mask) { nLR = (int)byteLR; }
  else { nLR = -1 * (int)byteLR; }

  mask = B00001000;
  if(Digital1 & mask) { nSpin = (int)byteSpin; }
  else { nSpin = -1 * (int)byteSpin; }

  // null temporary variables
  tempPulseA = 0; 
  tempPulseB = 0; 
  tempPulseC = 0;
  
  // Apply speed values for FB movement
  tempPulseB = tempPulseB - nFB;
  tempPulseC = tempPulseC + nFB;
  
  // Apply speed values for LR movement
  tempPulseA = tempPulseA + nLR;
  tempPulseB = tempPulseB - sideStep * (double)nLR;
  tempPulseC = tempPulseC - sideStep * (double)nLR;
  
  // Apply speed values for spin movement
  tempPulseA = tempPulseA + nSpin;
  tempPulseB = tempPulseB + nSpin;
  tempPulseC = tempPulseC + nSpin;
  
  // Extract motor directions
  if (tempPulseA < 0) {
    motorA.direction = false;
    digitalWrite(dirA, LOW);
    tempPulseA = -1 * tempPulseA;
  } else {
    motorA.direction = true;
    digitalWrite(dirA, HIGH);
  }
  
  if (tempPulseB < 0) {
    motorB.direction = false;
    digitalWrite(dirB, LOW);
    tempPulseB = -1 * tempPulseB;
  } else {
    motorB.direction = true;
    digitalWrite(dirB, HIGH);
  }
  
  if (tempPulseC < 0) {
    motorC.direction = false;
    digitalWrite(dirC, LOW);
    tempPulseC = -1 * tempPulseC;
  } else {
    motorC.direction = true;
    digitalWrite(dirC, HIGH);
  }
  
  // Scale based on maximum values
  tempMax = max(tempPulseA, tempPulseB);
  tempMax = max(tempPulseC, tempMax);
  
  if (tempMax > 253) {
    tempScale = (double)253 / (double)tempMax;
    tempPulseA = tempScale * (double)tempPulseA;
    tempPulseB = tempScale * (double)tempPulseB;
    tempPulseC = tempScale * (double)tempPulseC;
  }
  
  // Bound pulse values to maximum safe inputs
  if (tempPulseA > 253)         { tempPulseA = 254;    }
  if (tempPulseA < deadband)    { tempPulseA = 0;      }
  
  // Bound pulse values to maximum safe inputs
  if (tempPulseB > 253)        { tempPulseB = 254;      }
  if (tempPulseB < deadband)   { tempPulseB = 0;        }

  // Bound pulse values to maximum safe inputs
  if (tempPulseC > 253)        { tempPulseC = 254;	}
  if (tempPulseC < deadband)   { tempPulseC = 0;	}

  // Apply temporary values to motor structs
  motorA.pulse = (int)tempPulseA;
  motorB.pulse = (int)tempPulseB;
  motorC.pulse = (int)tempPulseC;

  // Output pulse values to SDR PWMs
  analogWrite(pwmA, motorA.pulse);
  analogWrite(pwmB, motorB.pulse);
  analogWrite(pwmC, motorC.pulse);
}

//******************************************************************************
// Processes the checksum
// RETURNS: TRUE when the checksum is validadted
//******************************************************************************
bool processCheckSum() {
  
  checksum1 = byteFB + byteLR + byteSpin + Digital1;
  
  if (checksum1 != checksum) {
    Bad_Tx = Bad_Tx + 1;
    if(Bad_Tx > 3) {
      Bad_Tx = Bad_Tx -1;
    }
    
    return false;
  } else {
    No_Tx = 0;				// Reinitialize counter timeout counter since a packet was recieved and read
    Bad_Tx = 0;				// Clear counter since we read a packet
    return true;
  }
}

//******************************************************************************
// Cuts outputs if a transmission hasn't been recieved
// RETURNS: Nothing
//******************************************************************************
void rxTimeout () {
  
  No_Tx = No_Tx + 1;			// Increment counter
  delay(10);
  
  // After a second without transmission, stop motors
  if (No_Tx > 100)  {
    allStop();	
  }

  if (No_Tx > 150)  {
    No_Tx = 0;
    sdrFound = false;
  }
}


