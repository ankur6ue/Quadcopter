/**************************************************************************

Filename    :   BLMotorControl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef BLMOTORCONTROL
#define BLMOTORCONTROL

// Need the Servo library
#include <Arduino.h>
#include <Servo.h>
#include "Scheduler.h"
#define NUMMOTORS 4

enum MotorId
{
	FR = 1,
	BL = 2,
	BR = 4,
	FL = 8
};


#define ESC_HIGH_DEFAULT 2000
#define ESC_LOW_DEFAULT 700
//#define ESC_HIGH_DEFAULT MAX_PULSE_WIDTH // Note that the servo lib maxes out at 200, so any value higher is capped at 180
//#define ESC_LOW_DEFAULT MIN_PULSE_WIDTH
// Stores the settings for all ESC. This can be made specific to each ESC, but that's not needed
// for a quadcopter project

typedef struct ESCSettingsDef
{
	ESCSettingsDef(int low, int high) { Low = low; High = high; };
	int Low;
	int High;
};

class BLMotor
{
public:
	BLMotor(int);
	void Init();
	void Reset();
	void Test(int speed);
	void Run( int speed);
	Servo   Motor;
	int     Pin;   // Indicates the Pin this motor is connected to
	int 	Speed;
	int 	Step;
	bool	bIsRunning;
};

extern BLMotor MotorFR;
extern BLMotor MotorBL;
extern BLMotor MotorBR;
extern BLMotor MotorFL;

void InitMotors();
void ResetMotors();

class MotorCtrl: public Task
{
public:
	MotorCtrl(int frequency, const char* name);
	unsigned long	Run();
	void InitMotors();
	void ResetMotors();
};

#endif




