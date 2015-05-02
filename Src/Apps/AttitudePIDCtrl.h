/**************************************************************************

Filename    :   AttitudePIDCtrl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

/*
  PIDControl
 */
#ifndef __ATTITUDEPIDCONTROL__
#define __ATTITUDEPIDCONTROL__
#include "PIDControl.h"

class AttitudePIDCtrl: public PIDControllerImpl
{
public:
	AttitudePIDCtrl(){};
	~AttitudePIDCtrl(){};

	virtual void Compute(double* angles, double* angVels, double* output);

	virtual void SetA2RTunings(double A2R_kp, Axis _eAxis);

	virtual void SetTunings(PIDParams& params, Axis _eAxis);

	// Used specifically to set the accumulated error to zero when Ki is changed to prevent sudden jumps in PID output
	virtual void SetErrSum(double val, Axis _eAxis);

	virtual double GetAttitude(Axis _eAxis);

	// Called when user input along a particular axis arrives
	virtual void OnControlInput(double userInput, Axis _eAxis);

	virtual void SetHoverAttitude(double attitude, Axis _eAxis);

	virtual void SetSpeed(int);

	virtual void Reset();

	double ApplySafeCheck(double Output);

	double LastOutput;
};

#endif
