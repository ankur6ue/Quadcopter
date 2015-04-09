/**************************************************************************

Filename    :   AccelerationSampler.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef __ACCELSAMPLER__
#define __ACCELSAMPLER__

#include "arduino.h"
#include "Scheduler.h"

class AccelSampler: public Task
{
public:
	AccelSampler(int frequency, const char* name): Task(frequency, name){}
	virtual ~AccelSampler(){};
	virtual unsigned long Run();
	void GetAccelAvg(float& faccX, float& faccY, float& faccZ);

	float faccXn; float faccYn; float faccZn;
	float faccXn_; float faccYn_; float faccZn_; // n-1
	float faccXn__; float faccYn__; float faccZn__; // n-2
};

#endif
