/**************************************************************************

Filename    :   PIDControl.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#include "arduino.h"
#include "PIDControl.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"

PIDController::PIDController()
{
	Errsum 			= 0;
	LastErr 		= 0;
	Setpoint		= 0;
	TargetSetpoint 	= 0;
}

void PIDController::SetSpeed(int speed)
{
	QuadSpeed = speed;
}
  
void PIDController::SetTunings(double kp, double ki, double kd)
{
   Kp = kp;
   Ki = ki/1000;
   Kd = kd*1000;
}

void PIDController::SetErrorSum(double val)
{
	Errsum = val;
}

void PIDController::SetLastError(double val)
{
	LastErr = val;
}

double PIDController::GetErrorSum()
{
	return Errsum;
}

double PIDController::GetSetPoint()
{
	return Setpoint;
}

void PIDController::SetSetPoint(double _setPoint)
{
	Setpoint = _setPoint;
}

void PIDController::SetNewSetpoint(double _setPoint)
{
	TargetSetpoint = _setPoint;
	StepSize = (TargetSetpoint - Setpoint)/100;
}
