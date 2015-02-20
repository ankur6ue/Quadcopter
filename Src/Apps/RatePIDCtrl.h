/**************************************************************************

Filename    :   RatePIDCtrl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#ifndef __RATEPIDCONTROL__
#define __RATEPIDCONTROL__
#include "PIDControl.h"

class RatePIDCtrl: public PIDControllerImpl
{
public:
	RatePIDCtrl(){};
	~RatePIDCtrl(){};
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

	void Reset();

	double LastOutput;
};

#endif
