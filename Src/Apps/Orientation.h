/**************************************************************************

Filename    :   Orientation.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef __ORIENTATION__
#define __ORIENTATION__

#include "arduino.h"
#include "Scheduler.h"

class CalcOrientation: public Task
{
public:
	CalcOrientation(int frequency, const char* name): Task(frequency, name){}
	~CalcOrientation(){};
	virtual unsigned long Run();
};

#endif
