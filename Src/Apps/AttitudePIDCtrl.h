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

	double Compute(double angle, double unused);

	virtual void SetTunings(double Kp, double Ki, double Kd);

	virtual void SetSetPoint(double _setPoint);

	virtual void SetNewSetpoint(double _setPoint);

	virtual void SetSpeed(int);

	virtual double GetSetPoint();

	virtual void SetErrorSum(double val);

	virtual void SetLastError(double val);

	virtual double GetErrorSum();

	double ApplySafeCheck(double Output);

	virtual void Reset();

	double LastOutput;
};

#endif
