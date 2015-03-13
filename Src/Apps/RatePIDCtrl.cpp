
/**************************************************************************

Filename    :   RatePIDCtrl.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "RatePIDCtrl.h"
#include "arduino.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"
#include "ErrorsDef.h"

extern ExceptionMgr cExceptionMgr;

/* We might want the controller to maintain a certain roll/pitch angle and a yaw rate or certain roll/pitch/yaw angles
 * or any combination thereof. The implementation for the various combinations are in AC_AttitudeControl.cpp in the
 * Ardupilot codebase.
 */
void RatePIDCtrl::Reset()
{
	// Set all accumulated variables to 0
	PIDCtrlData[Axis_Yaw].Errsum = 0.0;
	PIDCtrlData[Axis_Pitch].Errsum = 0.0;
	PIDCtrlData[Axis_Roll].Errsum = 0.0;

	PIDCtrlData[Axis_Yaw].last_ang_vel = 0.0;
	PIDCtrlData[Axis_Pitch].last_ang_vel = 0.0;
	PIDCtrlData[Axis_Roll].last_ang_vel = 0.0;

	PIDCtrlData[Axis_Yaw].LastDerivative = 0.0;
	PIDCtrlData[Axis_Pitch].LastDerivative = 0.0;
	PIDCtrlData[Axis_Roll].LastDerivative = 0.0;


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

void RatePIDCtrl::SetSpeed(int speed)
{
	QuadSpeed = speed;
}

void RatePIDCtrl::SetA2RTunings(double A2R_kp, Axis eAxis)
{
	SERIAL.println("In set a2rtunings"); SERIAL.print("z");
	PIDCtrlData[eAxis].A2R_Kp = A2R_kp;
}

void RatePIDCtrl::SetTunings(double kp, double ki, double kd, Axis eAxis)
{
	PIDCtrlData[eAxis].Kp = kp/40;
	PIDCtrlData[eAxis].Ki = ki/10000;
	PIDCtrlData[eAxis].Kd = kd*10;
}

double RatePIDCtrl::GetAttitude(Axis eAxis)
{
	return PIDCtrlData[eAxis].AttitudeBF;
}

void RatePIDCtrl::SetHoverAttitude(double attitude, Axis eAxis)
{
	PIDCtrlData[eAxis].HoverAttitudeBF = attitude;
}

void RatePIDCtrl::OnControlInput(double userInput, Axis eAxis)
{
	PIDCtrlData[eAxis].TargetAttitudeBF = PIDCtrlData[eAxis].HoverAttitudeBF + userInput;
}

void RatePIDCtrl::Compute(double* angles, double* angVels, double* output)
{
	/*How long since we last calculated*/
	unsigned long now = millis();
	double timeChange = (double) (now - LastTime);
	LastTime = now;
	for (int axis = 0; axis < 3; axis++)
	{
		// Step 1: Constrain target angle to valid range
		// TODO
		// Step 2: Calculate discrepancy from desired angle
		float angle_error = PIDCtrlData[axis].TargetAttitudeBF - angles[axis];
		// TODO: wrap around to 180 degrees
		// Step 3: Convert to angle difference to angular velocity
		float ang_vel_target = angle_error*PIDCtrlData[axis].A2R_Kp;
		// TODO: constrain ang_vel_error
		float curr_ang_vel = angVels[axis];
		float rate_error = curr_ang_vel - ang_vel_target;
		// Step 4: Get p term
		float p = PIDCtrlData[axis].Kp*rate_error;
		// Store p (could be useful for logging)
		PIDCtrlData[axis].P = p;
		// Step 5: Get i term
		float i = 0;
		if (QuadSpeed > LiftOffSpeed)
		{
			// Update accumulated if:
			// 1. i term lies within Windup range
			// 2. i term will definitely decrease as the rate_error has a different sign
			i = PIDCtrlData[axis].I;
			if (((i >= RateWindUp) && (rate_error < 0)) ||
				((i < -RateWindUp) && (rate_error > 0)) ||
				((i < RateWindUp) && (i > -RateWindUp)))
			{
				PIDCtrlData[axis].Errsum += (rate_error) * timeChange;
				i = PIDCtrlData[axis].Errsum*PIDCtrlData[axis].Ki;
			}
		}
		PIDCtrlData[axis].I = i;
		// TODO: Check if motors are maxed out. In this case only add i term if it's going to reduce motor output

		// Step 6: Get d term
		//double dErr = (rate_error - PIDCtrlData[axis].LastErr) / timeChange;
		double dErr = (curr_ang_vel - PIDCtrlData[axis].last_ang_vel) / timeChange;
		// Do some filtering on the derivative term to minimize noise
		dErr = PIDCtrlData[axis].LastDerivative + (dErr - PIDCtrlData[axis].LastDerivative)*0.5;
		PIDCtrlData[axis].LastDerivative = dErr;
		float d = dErr*PIDCtrlData[axis].Kd;

		// Step 7: Add up PID components
		PIDCtrlData[axis].Output = p+i+d;

		if (PIDCtrlData[axis].Output > MaxPIDOutput)
		{
			cExceptionMgr.SetException(EXCESSIVE_PID_OUTPUT);
		}
		output[axis] = PIDCtrlData[axis].Output;
		/*Remember some variables for next time*/
		PIDCtrlData[axis].LastErr = rate_error;
		PIDCtrlData[axis].last_ang_vel = curr_ang_vel;
	}
}

double RatePIDCtrl::ApplySafeCheck(double Output)
{
	if (fabs(LastOutput) > 150 && fabs(Output) > 150)
	{
		return 0;
	}
	LastOutput = Output;
	return Output;
}


void RatePIDCtrl::SetErrSum(double val, Axis eAxis)
{
	PIDCtrlData[eAxis].Errsum = val;
}
