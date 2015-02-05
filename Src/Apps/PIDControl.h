/**************************************************************************

Filename    :   PIDControl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef PIDCONTROL_h
#define PIDCONTROL_h

class PIDController
{
public:
	PIDController();

	unsigned long lastTime;
	double 	Input, Output, Setpoint;
	double 	Errsum, LastErr;
	double 	Kp, Ki, Kd;
	double 	TargetSetpoint;
	double 	StepSize;
	int 	QuadSpeed;

	virtual double Compute(double input){ return 0; };

	void SetTunings(double Kp, double Ki, double Kd);

	void SetSetPoint(double _setPoint);

	void SetNewSetpoint(double _setPoint);

	void SetSpeed(int);

	double GetSetPoint();

	void SetErrorSum(double val);

	void SetLastError(double val);

	double GetErrorSum();
};

#endif
