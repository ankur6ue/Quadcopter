/**************************************************************************

Filename    :   BeaconListener.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "Orientation.h"
#include "Quadcopter.h"
#include "IMU.h"

extern IMU Imu;

unsigned long CalcOrientation::Run()
{
	unsigned long before = millis();
	float yaw, pitch, roll, yaw_omega, pitch_omega, roll_omega, pitch_accel, roll_accel, yaw_accel;
	float Vx, Vy, Vz;
	if (Imu.IntegrateGyro(yaw, pitch, roll, yaw_omega, pitch_omega, roll_omega, yaw_accel,
			pitch_accel, roll_accel))
	{
		QuadState.Yaw = yaw; QuadState.Pitch = pitch; QuadState.Roll= roll;
		QuadState.YawOmega 		= yaw_omega;
		QuadState.PitchOmega 	= pitch_omega;
		QuadState.RollOmega 	= roll_omega;
		QuadState.YawAccel 		= yaw_accel;
		QuadState.PitchAccel 	= pitch_accel;
		QuadState.RollAccel 	= roll_accel;
	}
	unsigned long now = millis();
	return now - before;
}
