
/*
  PWMMotorControl
  
 */

// the setup routine runs once when you press reset:
#include <Arduino.h>
#include "PWMMotorControl.h"
void setup();
void loop();

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  PWMMotorCtrlInit();
}
#ifdef Serial_
#pragma message("Serial_ defined")
#else
#pragma message("Serial_ not defined")
#endif

char currentChar	= 0;
int currentSpeed	= 140;
int currentMotor	= 0;
int currentDir		= 0;
int combo			= 0;
// the loop routine runs over and over again forever:
void loop() {
	// read the input on analog pin 0:
	int sensorValue = analogRead(A0);
 
  #if 0
  
	digitalWrite(I1, HIGH);
	digitalWrite(I2, LOW);
	digitalWrite(I3, HIGH);
	digitalWrite(I4, LOW);
	
	// 85% duty cycle for PWM control
	int counter = 0;
	while (counter < 10)
	{
		digitalWrite(E1, 255);  
		digitalWrite(E2, 255); 
		counter++;
	}
  
	counter = 0;
	while (counter < 50)
	{
		digitalWrite(E1, 0);
		digitalWrite(E2, 0);
		counter++;
	}
 #else
	char msg[100];
	while (Serial.available() > 0) 
	{
		// Read current character
		currentChar = Serial.read();
		Serial.print(currentChar);
		switch (currentChar)
		{
			case 's': // Start, turn clockwise
				Serial.print("Starting\n");
				Serial.print(SetMotorSpeed(0, currentSpeed));
				break;
			case 'f': // forward
				currentDir = 1;
				break;
			case 'b': // backward
				currentDir = 0;
				break;
			case 'u': // speed up
				sprintf(msg, "Speeding up, current speed = %d\n", currentSpeed);
				Serial.print(msg);
				currentSpeed = min(255, currentSpeed + 10);
				break;
			case 'd': // slow down
				currentSpeed = max(0, currentSpeed - 10);
				break;
			case 'e': // Turn off
				currentSpeed = 0;
				break;
			case 'l':
				currentMotor = 0;
				combo = 0;
				break;
			case 'r':
				currentMotor = 1;
				combo = 0;
				break;		
			case 'x':
				combo = 1;
				break;	
		}
		SetMotorDirection(currentMotor, currentDir);
		SetMotorSpeed(currentMotor, currentSpeed);
		if (combo)
		{
			SetMotorDirection(currentMotor?0:1, currentDir);
			SetMotorSpeed(currentMotor?0:1, currentSpeed);
		}
	}	
	
 #endif
}

int main(void)
{
	init();

	#if defined(USBCON)
	USBDevice.attach();
	#endif
	
	setup();
	
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
	
	return 0;
}