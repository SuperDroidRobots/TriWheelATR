//=========================HEADER=============================================================
/*

   xBee Control of a Tri-Wheel Vectoring Robot
   AUTHOR: Jason Traud
   April 23rd, 2014
   
   This code uses an Arduino Uno mounted with a joystick that sends a serial
   signal through an xBee to command a Tri-Wheel Vectoring platform
   TP-251-003. The robot commands three PWM motor controllers (TE-058-000)
   to control three independent omni-directional wheels. 
   
   
   Hardware: Arduino Uno R3      (MCU-050-000)
             Tri-Wheel ATR       (TP-251-003) 

   Connections: 
   
     Arduino A0   -   Joystick Y-Axis (Forward and Back)
     Arduino A1   -   Joystick X-Axis (Left and Right)
     Arduino A2   -   Joystick Z-Axis (Spin)
     
     Arduino 5V   -   Joystick Power
     Arduino Gnd  -   Joystick Ground

     
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
// Analog Input Variables
// *********************
int FB;
int LR;
int Spin;

// *********************
// Temporary variables
// *********************
double tempFB;
double tempLR;
double tempSpin;

bool dirFB;
bool dirLR;
bool dirSpin;
bool halfSpeed;

// *********************
// Transmitted bytes
// *********************
byte byteFB;
byte byteLR;
byte byteSpin;
byte Digital1;
byte checksum;

// Digital 1
// 0 - Half Speed 
// 1 - Reverse FB 
// 2 - Reverse LR
// 3 - Reverse Spin
// 4
// 5
// 6
// 7

// *********************
// Reverse bits
// *********************
bool revA0 = 0;
bool revA1 = 1;
bool revA2 = 1;

// *********************
// Misc variables
// *********************
int deadband = 30;

//******************************************************************************
// Sets up our serial com, hardware pins and initialize misc flags
// RETURNS: Nothing
//******************************************************************************
void setup() {
	Serial.begin(9600);
	halfSpeed = 1;
}

//******************************************************************************
// Main program loop where we transmit our data
// RETURNS: Nothing
//******************************************************************************
void loop() {
	reinitVars();		// nulls out temporary variables
	captureData();		// retrieves new data
	packetizeData();	// assembles data to be sent
	transmitData();
	//debugData();
	
	delay(20);	// Delay needed for consistant communication
}

//******************************************************************************
// Reinitialize temporary variables
// RETURNS: Nothing
//******************************************************************************
void reinitVars() {
  
  // Initialize Temporary Variables
  Digital1 = 0;	
  tempFB = 0;
  tempLR = 0;
  tempSpin = 0;
}

//******************************************************************************
// Retrieve analog input data and conform to specs
// RETURNS: Nothing
//******************************************************************************
void captureData() {
  
  // Read analog inputs
  FB = analogRead(A0);
  LR = analogRead(A1);
  Spin = analogRead(A2);

  // Reverse input analogs if flagged
  if (revA0)	{ FB = 1024 - FB; }
  if (revA1)	{ LR = 1024 - LR; }
  if (revA2)	{ Spin = 1024 - Spin; }

  // Maps input data to valid range
  tempFB = map(FB, 0, 1024, -255, 255);	
  tempLR = map(LR, 0, 1024, -255, 255);
  tempSpin = map(Spin, 0, 1024, -255, 255);
}

//******************************************************************************
// Assembles data to be sent
// RETURNS: Nothing
//******************************************************************************
void packetizeData() {
  
  // Assemble direction bits
  if (tempFB < 0) {	dirFB = false;	tempFB = tempFB * -1;	} 
  else { dirFB = true; }

  if (tempLR < 0) {	dirLR = false;	tempLR = tempLR * -1;	} 
  else { dirLR = true; }

  if (tempSpin < 0) {	dirSpin = false;	tempSpin = tempSpin * -1;	} 
  else { dirSpin = true; }

  // Band limit analog inputs
  if (tempFB > 254)	{ tempFB = 254;		}
  if (tempLR > 254)	{ tempLR = 254;		}
  if (tempSpin > 254)	{ tempSpin = 254;	}

  if (tempFB < deadband)	{ tempFB = 0;		}
  if (tempLR < deadband)	{ tempLR = 0;		}
  if (tempSpin < deadband)	{ tempSpin = 0;		}

  // Assemble the digital packet
  if (halfSpeed)	{ Digital1 = Digital1 + 1;	}	
  if (dirFB)		{ Digital1 = Digital1 + 2;	}
  if (dirLR)		{ Digital1 = Digital1 + 4;	}
  if (dirSpin)	        { Digital1 = Digital1 + 8;	}

  // convert transmitted data to bytes
  byteFB = byte(tempFB);
  byteLR = byte(tempLR);
  byteSpin = byte(tempSpin);

  // calculate checksum
  checksum = byteFB + byteLR + byteSpin + byte(Digital1);
}

//******************************************************************************
// Sends our data out through the xBee
// RETURNS: Nothing
//******************************************************************************
void transmitData() {
  // xBee Communication
  Serial.write("S");
  Serial.write("D");
  Serial.write("R");	
  Serial.write(byteFB);
  Serial.write(byteLR);
  Serial.write(byteSpin);
  Serial.write(byte(Digital1));
  Serial.write(checksum);
}

//******************************************************************************
// General debug information that may be useful when troubleshooting
// RETURNS: Nothing
//******************************************************************************
void debugData() {

  // Print out values for easy debugging
  Serial.print("[FB: "); Serial.print(byteFB, HEX); Serial.print("]");
  Serial.print("[LR: "); Serial.print(byteLR, HEX); Serial.print("]");
  Serial.print("[Sp: "); Serial.print(byteSpin, HEX); Serial.print("]");
  Serial.print("[D1: "); Serial.print(byte(Digital1), BIN); Serial.print("]");

  Serial.print("[dFB: "); Serial.print(dirFB); Serial.print("]");
  Serial.print("[tFB: "); Serial.print(tempFB);	Serial.print("]");

  Serial.print("[dLR: "); Serial.print(dirLR); Serial.print("]");
  Serial.print("[tLR: "); Serial.print(tempLR);	Serial.print("]");
	
  Serial.print("[dSp: "); Serial.print(dirSpin); Serial.print("]");
  Serial.print("[tSp: "); Serial.print(tempSpin); Serial.print("]");

  Serial.println("");
}
