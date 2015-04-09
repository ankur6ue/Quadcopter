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

#include "AccelSampler.h"
#include "Quadcopter.h"
#include "IMU.h"

extern IMU Imu;

unsigned long AccelSampler::Run()
{
	unsigned long before = millis();
	faccXn__ = faccXn_;
	faccYn__ = faccYn_;
	faccZn__ = faccZn_;
	faccXn_ = faccXn;
	faccYn_ = faccYn;
	faccZn_ = faccZn;
	Imu.GetAccel(faccXn, faccYn, faccZn);
	unsigned long now = millis();
	return now - before;
}

void AccelSampler::GetAccelAvg(float& faccX, float& faccY, float& faccZ)
{
	faccX = (faccXn + faccXn_ + faccXn__)/3;
	faccY = (faccYn + faccYn_ + faccYn__)/3;
	faccZ = (faccZn + faccZn_ + faccZn__)/3;
}
