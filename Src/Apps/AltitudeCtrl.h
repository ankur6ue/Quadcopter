/**************************************************************************

Filename    :   AltitudeControl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef __ALTITUDECTRL__
#define __ALTITUDECTRL__

#include "arduino.h"
#include "Scheduler.h"
#include "Quadcopter.h"

#define ALT_DEAD_ZONE 100 // Units of 1/10 of a mm

class AltitudeCtrl: public Task
{
public:
	AltitudeCtrl(int frequency, const char* name);
	~AltitudeCtrl(){};
	virtual unsigned long 	Run();
	long	 				GetCurrentAltitude();
	void					SetTargetAltitude(long _targetAlt) { TargetAltitude = _targetAlt; }
	long					GetTargetAltitude() { return TargetAltitude; }
	float 					MicrosecondsToCentimeters(unsigned long);
	void 					SendPing();
	long					Filter(long newVal, long oldVal, long maxDiff);
	int						UpdatePID(long currentAltTarget, long currentAltitude);
	void 					SetTunings(PIDParams& params);
	void 					SetHoverAltitude(float hoverAlt);
	void					Reset();
	void 					SetErrSum(double val) { ErrorSum = 0; }
public:
	long					CurrentAltitude;
	long	 				PrevAltitude;
	long					TargetAltitude;
	long 					CurrentAltTarget;
	long					RestGroundSonarDistance;
	// Maximum difference allowed between current and new altitude readings. Used to prevent sudden jumps in altitude
	long					MaxAltDiff;
	long					ClimbRate;
	float					Vz_Sonar;
	// records the last time this task was run
	unsigned long 			LastRun;
	float					ErrorSum;
	unsigned long 			TimeElapsed;
	int						ThrottleAdjustment;
	PIDParams				mPIDParams;
};

#endif
