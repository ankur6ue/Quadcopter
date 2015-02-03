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
