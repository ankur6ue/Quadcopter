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
#include "Quadcopter.h"
#include "AttitudePIDCtrl.h"
#include "RatePIDCtrl.h"

PIDControllerImpl::PIDControllerImpl()
{
	lastTime = 0; Input = 0; Output = 0; Setpoint = 0;
	Errsum = 0; LastErr = 0;
	Kp = 0; Ki = 0; Kd = 0;
	TargetSetpoint = 0; StepSize = 0; QuadSpeed = 0;
}

PIDController::PIDController()
{

}

void PIDController::CreateControllers()
{
	AttitudePIDCtrl* attPIDCtrl = new AttitudePIDCtrl();
	RegisterPIDController(0, attPIDCtrl);
	RatePIDCtrl* ratePIDCtrl = new RatePIDCtrl();
	RegisterPIDController(1, ratePIDCtrl);
}

void PIDController::RegisterPIDController(int index, PIDControllerImpl* pImpl)
{
	if (index < NUMPIDCONTROLLERS)
		pidCtrlImpl[index] = pImpl;
	// else raise exception
}

double PIDController::Compute(double angle, double angVel)
{
	int pidIndex = QuadState.ePIDType;
	return pidCtrlImpl[pidIndex]->Compute(angle, angVel);
}

void PIDController::SetSpeed(int speed)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetSpeed(speed);
}
  
void PIDController::SetTunings(double kp, double ki, double kd)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetTunings(kp, ki, kd);
}

void PIDController::SetErrorSum(double val)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetErrorSum(val);
}

void PIDController::SetLastError(double val)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetLastError(val);
}

void PIDController::Reset()
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->Reset();
}

double PIDController::GetErrorSum()
{
	int pidIndex = QuadState.ePIDType;
	return pidCtrlImpl[pidIndex]->GetErrorSum();
}

double PIDController::GetSetPoint()
{
	int pidIndex = QuadState.ePIDType;
	return pidCtrlImpl[pidIndex]->GetSetPoint();
}

void PIDController::SetSetPoint(double _setPoint)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetSetPoint(_setPoint);
}

void PIDController::SetNewSetpoint(double _setPoint)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetNewSetpoint(_setPoint);
}
