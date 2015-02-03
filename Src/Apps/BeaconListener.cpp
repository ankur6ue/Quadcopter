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
