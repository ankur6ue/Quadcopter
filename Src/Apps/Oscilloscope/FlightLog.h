/**************************************************************************

Filename    :   FlightLog.h
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "signaldata.h"
#include "qmath.h"

class FlightLog
{
public:
	FlightLog();
	SetLogDirectory();
	CreatNewLog();
	WriteToLog();

	File*	pFp;
	char	DirName[100];
};

class FlightLogInMemory
{
public:
	FlightLogInMemory();
	float YPR[INMEMLOGSIZE][3];
	float PID[INMEMLOGSIZE][3];
	float Gyro[INMEMLOGSIZE][3];
	float Accel[INMEMLOGSIZE][3];
	int iSamplingRate;
	int YPRIndex;
	int PIDIndex;
	int GyroIndex;
	int AccelIndex;
};
