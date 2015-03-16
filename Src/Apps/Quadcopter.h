/**************************************************************************

Filename    :   Quadcopter.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#ifndef __QUADCOPTER__
#define __QUADCOPTER__

#define UNUSED1(p1) p1;
#define UNUSED2(p1, p2) p1; p2;
#define UNUSED3(p1, p2, p3) p1; p2; p3;

enum PIDType
{
	AttitudePIDControl = 0,
	RatePIDControl
};

enum Axis
{
	Axis_Yaw = 0,
	Axis_Pitch,
	Axis_Roll
};

struct QuadStateDef
{
	QuadStateDef()
	{
		bIsPIDSetup = false;
		bIsKpSet 	= false;
		bIsKiSet 	= false;
		bIsKdSet 	= false;
		bIsYawKpSet = false;
		bIsYawKiSet = false;
		bIsYawKdSet = false;
		bIsPIDTypeSet 	= false;
		bIsA2R_PKpSet	= false;
		bIsA2R_RKpSet	= false;
		bIsA2R_YKpSet	= false;
		ePIDType		= AttitudePIDControl;
	}
	float 	Yaw;
	float 	Pitch;
	float 	Roll;
	float 	YawOmega; 	// Angular velocity along Yaw axis
	float	PitchOmega;
	float	RollOmega;
	float	YawAccel;	// Acceleration along Yaw
	float 	PitchAccel;
	float 	RollAccel;
	float 	Kp;
	float 	Ki;
	float 	Kd;
	float 	Yaw_Kp;
	float 	Yaw_Ki;
	float 	Yaw_Kd;
	float 	PID_Yaw; // Denotes output of the PID Controller
	float 	PID_Roll;
	float 	PID_Pitch;
	float 	A2R_PKp; // Used to convert angle to angular velocity
	float	A2R_RKp;
	float	A2R_YKp;
	bool  	bMotorToggle;
	int		Speed;
	int		QuadStateFlag;
	PIDType	ePIDType;

	bool	bIsPIDSetup;
	bool	bIsKpSet;
	bool	bIsKiSet;
	bool	bIsKdSet;
	bool	bIsYawKpSet;
	bool	bIsYawKiSet;
	bool	bIsYawKdSet;
	bool	bIsPIDTypeSet;
	bool	bIsA2R_PKpSet;
	bool	bIsA2R_RKpSet;
	bool	bIsA2R_YKpSet;

	void Reset()
	{
		bIsPIDSetup 	= false;
		bIsKpSet 		= false;
		bIsKiSet 		= false;
		bIsKdSet 		= false;
		bIsYawKpSet 	= false;
		bIsYawKiSet 	= false;
		bIsYawKdSet 	= false;
		bIsPIDTypeSet 	= false;
		bIsA2R_PKpSet	= false;
		bIsA2R_RKpSet	= false;
		bIsA2R_YKpSet	= false;
	};
	bool IsPIDControlReady()
	{
		return bIsKpSet & bIsKiSet & bIsKdSet & bIsYawKpSet & bIsYawKiSet & bIsYawKdSet & bIsA2R_PKpSet & bIsA2R_RKpSet
		& bIsA2R_YKpSet;
	}
};

enum
{
	Kp_Set 		= 1,
	Kd_Set 		= 2,
	Ki_Set 		= 4,
	YawKp_Set 	= 8,
	YawKi_Set	= 16,
	YawKd_Set	= 32,
	Def_PitchSet= 64,
	Def_RollSet = 128
};

extern QuadStateDef QuadState;

// Global Variables
extern int				StartupTime;
extern int				ESCPoweredTime;
extern unsigned long	Now;
extern unsigned long	Before;
// Thresholds
// Controls how far the I term for the rate controller is allowed to go
extern int				RateWindUp;
// The PID output is not allowed to exceed this
extern int				MaxPIDOutput;
// To prevent damage to the motor, the motor input is capped
extern int				MaxMotorInput;
// Speed at which life off occurs. Depends on the weight of the quad, type of motors etc.
extern int				LiftOffSpeed;
#endif
