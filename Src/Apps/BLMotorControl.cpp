/*
Copyright (c) <2014> <Ankur Mohan>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Note: Select the mode (program mode/throttle setting mode/normal mode), run this code, 
then power on the ESC. Don't power on the ESC before this code is executing, otherwise
the ESC will see random values on the PWM pin.
*/

// Need the Servo library

#ifndef _BLMOTOR_CONTROL_
#define _BLMOTOR_CONTROL_

#include "Utils.h"
#include "BLMotorControl.h"
#include "QuadCopter.h"
#include "ErrorsDef.h"

extern ExceptionMgr cExceptionMgr;
extern MotorCtrl	cMotorCtrl;

BLMotor MotorFR(11);
BLMotor MotorBL(10);
BLMotor MotorBR(5);
BLMotor MotorFL(6);

ESCSettingsDef ESCSettings(ESC_LOW_DEFAULT, ESC_HIGH_DEFAULT);

MotorCtrl::MotorCtrl(int frequency, const char* name): Task(frequency, name) {};

unsigned long MotorCtrl::Run()
{
	if (cExceptionMgr.IsCriticalException())
	{
	// Check for exceptions
		long e = cExceptionMgr.GetException();

		switch (e)
		{
		case BAD_MPU_DATA:
		case NO_BEACON_SIGNAL:
		case EXCESSIVE_PID_OUTPUT:
			// TODO: Implement proper shutdown mode.
			cMotorCtrl.ResetMotors();
		}
	}
	else // No exceptions, run the motors
	{
		if (QuadState.bMotorToggle)
		{
			MotorFL.Run(QuadState.Speed - QuadState.PID_Roll  + QuadState.PID_Yaw);
			MotorBL.Run(QuadState.Speed + QuadState.PID_Pitch - QuadState.PID_Yaw);
			MotorFR.Run(QuadState.Speed - QuadState.PID_Pitch - QuadState.PID_Yaw);
			MotorBR.Run(QuadState.Speed + QuadState.PID_Roll  + QuadState.PID_Yaw);
		}
	}
}

void MotorCtrl::InitMotors()
{
	MotorFR.Init();
	MotorBR.Init();
	MotorFL.Init();
	MotorBL.Init();
}

void MotorCtrl::ResetMotors()
{
	MotorFR.Reset();
	MotorBR.Reset();
	MotorFL.Reset();
	MotorBL.Reset();
}
BLMotor::BLMotor(int pin)
{
	// Required for I/O from Serial monitor
	// Attach motors to pins
	Pin = pin;
	bIsRunning = true;
}

void BLMotor::Init()
{
	Speed = ESCSettings.Low;
	Step = 1;
	Motor.attach(Pin);
	Reset();
}

void BLMotor::Reset()
{
	Motor.write(ESCSettings.Low);
//	delay(500);
}

void BLMotor::Run(int _speed)
{
	if (bIsRunning)
	{
		int speed = constrain_i(_speed, ESCSettings.Low, 1500);
		Speed = speed;
		Motor.write(speed);
	}
	else
	{
		Speed = 0;
		Motor.write(ESCSettings.Low);
	}
}

#endif
