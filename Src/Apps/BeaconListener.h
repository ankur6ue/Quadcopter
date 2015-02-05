/**************************************************************************

Filename    :   BeaconListener.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef __BEACONLISTENER__
#define __BEACONLISTENER__

#include "arduino.h"
#include "Scheduler.h"

class BeaconListener: public Task
{
public:
	BeaconListener(int frequency, const char* name): Task(frequency, name){}
	virtual unsigned long Run();
	void 	SetBeaconReceived() { bBeaconReceived = true; }
	bool	IsBeaconReceived() { return bBeaconReceived; }

	bool 	bBeaconReceived;
};

#endif
