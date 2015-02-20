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

#define NUMPIDCONTROLLERS 2 // Attitude Controller, Rate Controller

class PIDControllerImpl
{
public:
	PIDControllerImpl();

	~PIDControllerImpl(){};

	virtual double Compute(double, double) {return 0;};

	virtual void SetTunings(double Kp, double Ki, double Kd){};

	virtual void SetSetPoint(double _setPoint){};

	virtual void SetNewSetpoint(double _setPoint){};

	virtual void SetSpeed(int){};

	virtual double GetSetPoint() {return 0.0; };

	virtual void SetErrorSum(double val) {};

	virtual void SetLastError(double val) {};

	virtual double GetErrorSum() { return 0.0; };

	virtual void Reset() {};

	unsigned long lastTime;
	double 	Input, Output, Setpoint;
	double 	Errsum, LastErr;
	double 	Kp, Ki, Kd;
	double 	TargetSetpoint;
	double 	StepSize;
	int 	QuadSpeed;
};

class PIDController
{
public:
	PIDController();

	PIDControllerImpl* pidCtrlImpl[NUMPIDCONTROLLERS];

	void RegisterPIDController(int index, PIDControllerImpl* pImpl);

	void CreateControllers();

	void SetTunings(double Kp, double Ki, double Kd);

	void SetSetPoint(double _setPoint);

	void SetNewSetpoint(double _setPoint);

	void SetSpeed(int);

	double GetSetPoint();

	void SetErrorSum(double val);

	void SetLastError(double val);

	double GetErrorSum();

	double Compute(double, double);

	void Reset();
};


#endif
