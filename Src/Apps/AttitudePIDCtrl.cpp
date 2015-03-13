/**************************************************************************

Filename    :   AttitudePIDCtrl.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "AttitudePIDCtrl.h"
#include "arduino.h"

void AttitudePIDCtrl::Reset()
{
	// Set all accumulated variables to 0
	PIDCtrlData[Axis_Yaw].Errsum = 0.0;
	PIDCtrlData[Axis_Pitch].Errsum = 0.0;
	PIDCtrlData[Axis_Roll].Errsum = 0.0;

	// Set target attitude to hover attitude. Only do this for Pitch and Roll
	for (int i = 0; i < 2; i++)
	{
		PIDCtrlData[i].TargetAttitudeBF = PIDCtrlData[i].HoverAttitudeBF;
	}
	// Set the attitude to the current orientation of the quad
	PIDCtrlData[Axis_Yaw].AttitudeBF = QuadState.Yaw;
	PIDCtrlData[Axis_Pitch].AttitudeBF = QuadState.Pitch;
	PIDCtrlData[Axis_Roll].AttitudeBF = QuadState.Roll;

}

void AttitudePIDCtrl::SetSpeed(int speed)
{
	QuadSpeed = speed;
}

void AttitudePIDCtrl::SetTunings(double kp, double ki, double kd, Axis eAxis)
{
	PIDCtrlData[eAxis].Kp = kp;
	PIDCtrlData[eAxis].Ki = ki/1000;
	PIDCtrlData[eAxis].Kd = kd*1000;

}

void AttitudePIDCtrl::SetA2RTunings(double A2R_kp, Axis _eAxis)
{
	UNUSED2(A2R_kp, _eAxis)
}

double AttitudePIDCtrl::GetAttitude(Axis eAxis)
{
	return PIDCtrlData[eAxis].AttitudeBF;
}

void AttitudePIDCtrl::SetHoverAttitude(double attitude, Axis eAxis)
{
	PIDCtrlData[eAxis].HoverAttitudeBF = attitude;
	PIDCtrlData[eAxis].TargetAttitudeBF = attitude;
}

void AttitudePIDCtrl::OnControlInput(double userInput, Axis eAxis)
{
	PIDCtrlData[eAxis].TargetAttitudeBF = PIDCtrlData[eAxis].HoverAttitudeBF + userInput;
}

void AttitudePIDCtrl::Compute(double* angles, double* angVels, double* output)
{
	/*How long since we last calculated*/
	unsigned long now = millis();
	double timeChange = (double) (now - LastTime);

	for (int i = 0; i < 3; i++)
	{
		double stepSize = (PIDCtrlData[i].TargetAttitudeBF - PIDCtrlData[i].AttitudeBF)/100;
		PIDCtrlData[i].AttitudeBF += stepSize;
		/*Compute all the working error variables*/
		double error = (PIDCtrlData[i].AttitudeBF - angles[i]);
		// Start accumulating I error only when the Quad is approach lift off speed. Otherwise there could be a large accumulated
		// I error before motors start leading to erratic (and potentially dangerous behavior)
		if (QuadSpeed > 500)
		{
			PIDCtrlData[i].Errsum += (error) * timeChange;
			// Cap Errsum
			PIDCtrlData[i].Errsum = PIDCtrlData[i].Errsum > 0? min(20000, PIDCtrlData[i].Errsum):max(-20000, PIDCtrlData[i].Errsum);
		}

		double dErr = (error - PIDCtrlData[i].LastErr) / timeChange;
		/*Compute PID Output*/
		double currentKp = max(0, PIDCtrlData[i].Kp * (1));
		PIDCtrlData[i].Output = currentKp * error + PIDCtrlData[i].Ki * PIDCtrlData[i].Errsum + PIDCtrlData[i].Kd * dErr;

		/*Remember some variables for next time*/
		PIDCtrlData[i].LastErr = error;
		LastTime = now;

		output[i] = ApplySafeCheck(PIDCtrlData[i].Output);
	}
}

double AttitudePIDCtrl::ApplySafeCheck(double Output)
{
	if (fabs(LastOutput) > 150 && fabs(Output) > 150)
	{
		return 0;
	}
	LastOutput = Output;
	return Output;
}


void AttitudePIDCtrl::SetErrSum(double val, Axis eAxis)
{
	PIDCtrlData[eAxis].Errsum = val;
}
