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
#include "AttitudePIDCtrl.h"
#include "RatePIDCtrl.h"

PIDControllerImpl::PIDCtrlData::PIDCtrlData()
{
	Input = 0; Output = 0; AttitudeEF = 0; AttitudeBF = 0;
	Errsum = 0; LastErr = 0;
	Kp = 0; Ki = 0; Kd = 0;
	TargetAttitudeEF = 0; TargetAttitudeBF = 0; StepSize = 0;
}

PIDControllerImpl::PIDControllerImpl()
{
	QuadSpeed = 0; LastTime = 0;
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

void PIDController::Compute(double* angles, double* angVels, double* output)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->Compute(angles, angVels, output);
}

void PIDController::SetErrSum(double val, Axis _eAxis)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetErrSum(val, _eAxis);
}

void PIDController::SetSpeed(int speed)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetSpeed(speed);
}
  
void PIDController::SetTunings(double kp, double ki, double kd, Axis _eAxis)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetTunings(kp, ki, kd, _eAxis);
}

void PIDController::Reset()
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->Reset();
}

double PIDController::GetAttitude(Axis _eAxis)
{
	int pidIndex = QuadState.ePIDType;
	return pidCtrlImpl[pidIndex]->GetAttitude(_eAxis);
}

void PIDController::OnControlInput(double userInput, Axis _eAxis)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->OnControlInput(userInput, _eAxis);
}

void PIDController::SetHoverAttitude(double attitude, Axis _eAxis)
{
	int pidIndex = QuadState.ePIDType;
	pidCtrlImpl[pidIndex]->SetHoverAttitude(attitude, _eAxis);
}
