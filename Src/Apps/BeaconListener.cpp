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
#include "SoftwareSerial.h"
#include "SerialDef.h"


extern ExceptionMgr cExceptionMgr;

void 	BeaconListener::SetBeaconReceived()
{
	bBeaconReceived = true;
}

unsigned long BeaconListener::Run()
{
	unsigned long before = micros();
	// Raise exception if we miss a beacon ( or if you want to be more permissive, two beacons in a row)
	if ((bBeaconReceived != true) /* && (bLastBeaconReceived != true )*/)
	{
		cExceptionMgr.SetException(NO_BEACON_SIGNAL);
		SERIAL.print("Setting exception flag\n");
		SERIAL.print("z");
	}
	bBeaconReceived = false; // Will be toggled when the next beacon signal is received.
	bLastBeaconReceived = bBeaconReceived;
	unsigned long now = micros();
	return now - before;
}
