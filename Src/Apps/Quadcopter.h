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
	float 	Yaw;
	float 	Pitch;
	float 	Roll;
	float 	Kp;
	float 	Ki;
	float 	Kd;
	float 	Yaw_Kp;
	float 	Yaw_Ki;
	float 	Yaw_Kd;
	float 	PID_Yaw; // Denotes output of the PID Controller
	float 	PID_Roll;
	float 	PID_Pitch;
	bool  	bMotorToggle;
	int		Speed;
	int		QuadStateFlag;
	PIDType	ePIDType;
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

extern int 				StartupTime;
extern bool				bIsPIDSetup;
extern bool				bIsKpSet;
extern bool				bIsKiSet;
extern bool				bIsKdSet;
extern bool				bIsYawKpSet;
extern bool				bIsYawKiSet;
extern bool				bIsYawKdSet;
extern bool				bIsPIDTypeSet;
extern int				ESCPoweredTime;
extern unsigned long	Now;
extern unsigned long	Before;

#endif
