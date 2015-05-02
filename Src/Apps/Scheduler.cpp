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
#include "Logger.h"

Scheduler::Scheduler()
{
	LastRunTime 		= 0;
	AggregateTickTime	= 0;
	NumTasks			= 0;
}

void Scheduler::RegisterTask(Task* task)
{
	// ASSERT (NumTasks < MAX_NUM_TASKS)
	Tasks[NumTasks] = task;
	NumTasks++;
}

void Scheduler::Tick()
{
	LastRunTime  = millis();
	for (int i = 0; i < NumTasks; i++)
	{
		Tasks[i]->Execute();
	}
	AggregateTickTime += millis() - LastRunTime;
	if (AggregateTickTime > PerfReportFreq)
	{
		char buffer[600];
		memset(buffer, 0, 600);
		if (GetPerformance(buffer, 600))
		{
			SERIAL.print(buffer);
			PrintHelper::AttachSentinal();
		}
		AggregateTickTime = 0;
	}
	// Implement other important scheduler behaviour - tasks should have time budgets, scheduler should keep a track
	// of if a task went over allocated budget etc.
}

bool Scheduler::GetPerformance(char* buffer, unsigned long bufSize)
{
	unsigned long aggregateTaskTime = 0; // accumulates the time taken to run different tasks
	char tmp[100];
	int strSize 				= 0;
	int insertPos 				= 0;
	unsigned long numTaskRuns 	= 0;
	unsigned long taskTime		= 0;

	for (int i = 0; i < NumTasks; i++)
	{
		numTaskRuns 	= Tasks[i]->GetRunCount();
		taskTime		= Tasks[i]->GetAggregateTaskTime();
		Tasks[i]->ResetAggregateTaskTime();
		Tasks[i]->ResetRunCount();
		unsigned long taskFrequency = (unsigned long)(numTaskRuns*1000/taskTime); // In Hz
		memset(tmp, 0, 100);
		sprintf(tmp, "%s %lu\n", Tasks[i]->GetTaskName(), taskFrequency);
		strSize = strlen(tmp);
		// ensure we have space left in the buffer
		if (bufSize - insertPos < strSize) return false;
		memcpy(buffer+insertPos, tmp, strSize);
		aggregateTaskTime += taskTime;
		insertPos += strSize;
	}
	// Calculate portion of total ticks taken by running tasks. This is a measure of processor utilization
	// by tasks
	int procUtilization = (int)(aggregateTaskTime/AggregateTickTime)*100;
	memset(tmp, 0, 100);
	sprintf(tmp, "ProcUtil = %d\n", procUtilization);
	strSize = strlen(tmp);
	if (bufSize - insertPos < strSize) return false;
	memcpy(buffer+insertPos, tmp, strSize);
	return true;
}

void Scheduler::RunTask(Task* task)
{
	if (task)
	{
		task->Execute();
	}
}

Task::Task (int frequency, const char* name): IntervalTicks((unsigned long)(1000.0/frequency))
{
	LastRunTime = 0;
	TaskTime	= 0;
	bIsRunning	= true;
	RunCount	= 0;
	AggregateTaskTime = 0;
	strcpy(TaskName, name);
}

void Task::Start()
{
	bIsRunning = true;
}

void Task::Stop()
{
	bIsRunning = false;
}

void Task::Execute()
{
	if (!bIsRunning) return;

	unsigned long now = millis();
	if (now - LastRunTime > IntervalTicks)
	{
		LastRunTime = now;
	//	SERIAL.println(TaskName);
		TaskTime = this->Run();
		RunCount++;
		AggregateTaskTime += TaskTime;
	}
}
