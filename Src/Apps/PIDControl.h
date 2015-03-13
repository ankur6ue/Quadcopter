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

#include "quadcopter.h"
#include "Scheduler.h"

#define NUMPIDCONTROLLERS 2 // Attitude Controller, Rate Controller

class PIDControllerImpl
{
public:
	PIDControllerImpl();

	~PIDControllerImpl(){};

	virtual void Compute(double* angles, double* angVels, double* output) {};

	virtual void SetTunings(double Kp, double Ki, double Kd, Axis _eAxis) {};

	virtual void SetA2RTunings(double A2R_kp, Axis _eAxis) {};

	// Used specifically to set the accumulated error to zero when Ki is changed to prevent sudden jumps in PID output
	virtual void SetErrSum(double val, Axis _eAxis) {};

	virtual double GetAttitude(Axis _eAxis) {return 0.0; };

	// Called when user input along a particular axis arrives
	virtual void OnControlInput(double userInput, Axis _eAxis) {};

	virtual void SetHoverAttitude(double attitude, Axis _eAxis) {};

	virtual void SetSpeed(int) {};

	virtual void Reset() {};

	struct PIDCtrlData
	{
		PIDCtrlData();
		double 	Input, Output, AttitudeBF, AttitudeEF;
		double 	Errsum, LastErr;
		double	last_ang_vel;
		double	LastDerivative;
		double 	Kp, Ki, Kd;
		double  P, I, D;
		double 	A2R_Kp; // Coefficient to convert angle to angular velocity
		double 	TargetAttitudeBF; // Body Frame Target Attitude
		double 	TargetAttitudeEF; // Earth Frame Target Attitude
		double 	HoverAttitudeBF;
		double	HoverAttitudeEF;
		double 	StepSize;
	} PIDCtrlData[3]; // 0: Yaw, 1: Pitch, 2: Roll

	int 			QuadSpeed;
	unsigned long 	LastTime;

};

class PIDController: public Task
{
public:
	PIDController(int frequency, const char* name);

	PIDControllerImpl* pidCtrlImpl[NUMPIDCONTROLLERS];

	virtual unsigned long Run();

	void RegisterPIDController(int index, PIDControllerImpl* pImpl);

	void CreateControllers();

	void SetA2RTunings(double A2R_kp, Axis _eAxis);

	void SetTunings(double Kp, double Ki, double Kd, Axis _eAxis);

	// Used specifically to set the accumulated error to zero when Ki is changed to prevent sudden jumps in PID output
	void SetErrSum(double val, Axis _eAxis);

	double GetAttitude(Axis _eAxis);

	// Called when user input along a particular axis arrives
	void OnControlInput(double userInput, Axis _eAxis);

	void SetHoverAttitude(double attitude, Axis _eAxis);

	void SetSpeed(int);

	void Compute(double* angles, double* angVels, double* output);

	void Reset();
};


#endif
