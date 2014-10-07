
/*
  PWMMotorControl
 */

// the setup routine runs once when you press reset:
#include <Arduino.h>
#include "PWMMotorControl.h"

int MotorId = 0;
int Speed	= 0;
MotorCntrl Motors[2];
void PWMMotorCtrlInit() {
  
  pinMode(E1, OUTPUT);
  pinMode(E2, OUTPUT);
  
  pinMode(I1, OUTPUT);
  pinMode(I2, OUTPUT);
  pinMode(I3, OUTPUT);
  pinMode(I4, OUTPUT);
  
  // Enable Interrupts
  sei();
  
  // REfer to section 17, "8 Bit Timer Counter 2 with PWM" in the Atmega328 manual"
  // Also, this article: http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM
  // We are using Fast PWM Mode with Interrupts
  // Clear OC2A on Compare Match, set OC2A at BOTTOM, (non-inverting mode).
  // Clear OC2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
  // Fast PWM Mode
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22); //clk/64
  OCR2A = 0; // Motor Id = 0
  OCR2B = 0; // Motor Id = 1
  TIMSK2 = _BV(TOIE2); // Enable interrupt when Timer overflows. This should be a safe time to set new speed values
  digitalWrite(I1, LOW);
  digitalWrite(I2, LOW);
  digitalWrite(I3, LOW);
  digitalWrite(I4, LOW);
}

int CheckMotorId(int _motorId)
{
	// Motor id can be 0 or 1
	if ((_motorId != 0) && (_motorId != 1)) return INVALID_MOTOR_ID;
	return MOTOR_ID_OK;
}
int SetMotorSpeed(int _motorId, int _speed)
{
	int errorCode;
	if ((errorCode = CheckMotorId(_motorId)) != MOTOR_ID_OK) return errorCode;
	// Make sure speed is in the right range (0-255)
	int speed = _speed < 0? max(_speed, 0): min(_speed, 255);
	// Store motor id and speed in global variables to be set later
	Motors[_motorId].Speed = speed;
}

int StopMotor(int _motorId)
{
	if(_motorId == 0)
	{
		digitalWrite(I1, LOW);
		digitalWrite(I2, LOW);
	}
	if(_motorId == 1)
	{
		digitalWrite(I3, LOW);
		digitalWrite(I4, LOW);
	}
}

int SetMotorDirection(int _motorId, int forward)
{
	// Motor id can be 0 or 1
	int errorCode;
	if ((errorCode = CheckMotorId(_motorId)) != MOTOR_ID_OK) return errorCode;
	
	if(_motorId == 0)
	{
		if (forward)
		{
			digitalWrite(I1, LOW);
			digitalWrite(I2, HIGH);
		}
		else
		{
			digitalWrite(I1, HIGH);
			digitalWrite(I2, LOW);
		}
	}
	else
	{
		if (forward)
		{
			digitalWrite(I3, HIGH);
			digitalWrite(I4, LOW);
		}
		else
		{
			digitalWrite(I3, LOW);
			digitalWrite(I4, HIGH);
		}
	}
}

ISR(TIMER2_OVF_vect) {
	/* Reload the timer */
	OCR2B = Motors[0].Speed;
	OCR2A = Motors[1].Speed;
}

