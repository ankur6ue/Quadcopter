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

#include "BeaconListener.h"
#include "ErrorsDef.h"

extern ExceptionMgr cExceptionMgr;

unsigned long BeaconListener::Run()
{
	unsigned long before = micros();
	if (bBeaconReceived != true)
	{
		cExceptionMgr.SetException(NO_BEACON_SIGNAL);
	}
	bBeaconReceived = false; // Will be toggled when the next beacon signal is received.
	unsigned long now = micros();
	return now - before;
}
