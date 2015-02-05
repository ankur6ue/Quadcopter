/**************************************************************************

Filename    :   Scheduler.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "Scheduler.h"
#include "arduino.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"

Scheduler::Scheduler()
{
	LastRunTime = 0;
	TimeTaken	= 0;
	NumTasks	= 0;
}

void Scheduler::RegisterTask(Task* task)
{
	// ASSERT (NumTasks < MAX_NUM_TASKS)
	Tasks[NumTasks] = task;
	NumTasks++;
}

void Scheduler::Tick()
{
	LastRunTime  = micros();
	for (int i = 0; i < NumTasks; i++)
	{
		Tasks[i]->Execute();
	}
	TimeTaken = micros() - LastRunTime;
	// Implement other important scheduler behaviour - tasks should have time budgets, scheduler should keep a track
	// of if a task went over allocated budget etc.
}

void Scheduler::RunTask(Task* task)
{
	if (task)
	{
		task->Execute();
	}
}

Task::Task (float frequency, const char* name): IntervalTicks((unsigned long)(1000000.0/frequency))
{
	LastRunTime = 0;
	TimeTaken	= 0;
	strcpy(TaskName, name);
}

void Task::Execute()
{
	unsigned long now = micros();
	if (now - LastRunTime > IntervalTicks)
	{
		LastRunTime = now;
	//	SERIAL.println(TaskName);
		TimeTaken = this->Run();
	}
}
