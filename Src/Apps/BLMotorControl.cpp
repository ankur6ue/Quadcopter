/**************************************************************************

Filename    :   BLMotorControl.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

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
	unsigned long before = millis();
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
			MotorFL.Run(QuadState.Speed + QuadState.PID_Alt - QuadState.PID_Pitch  - QuadState.PID_Yaw);
			MotorBL.Run(QuadState.Speed + QuadState.PID_Alt + QuadState.PID_Roll + QuadState.PID_Yaw);
			MotorFR.Run(QuadState.Speed + QuadState.PID_Alt - QuadState.PID_Roll + QuadState.PID_Yaw);
			MotorBR.Run(QuadState.Speed + QuadState.PID_Alt + QuadState.PID_Pitch  - QuadState.PID_Yaw);
		}
	}
	unsigned long now = millis();
	return (now - before);
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

	Motor.write(700);
	Motor.write(ESCSettings.Low);
//	delay(500);
}

void BLMotor::Run(int _speed)
{
	if (bIsRunning)
	{
		int speed = constrain_i(_speed, ESCSettings.Low, MaxMotorInput);
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
