/**************************************************************************

Filename    :   Scheduler.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef __SCHEDULER__
#define __SCHEDULER__

#define MAX_NUM_TASKS 10

class Task
{
public:
	Task(){};
	//Frequency is in Hertz
	Task(float frequency, const char* name);

	void 	Execute();
	virtual unsigned long	Run() { return 0; }
	void 			Start();
	void 			Stop();
	unsigned long 	GetRunCount() { return RunCount; }
	unsigned long 	GetAggregateTaskTime() { return AggregateTaskTime; }
	void			ResetRunCount() { RunCount = 0; }
	void			ResetAggregateTaskTime() { AggregateTaskTime = 0; }
	char*			GetTaskName() {return TaskName; }
	unsigned long	LastRunTime; // records when the task was last run
	unsigned long	TaskTime; // records the time taken to run the task
	unsigned long	AggregateTaskTime; // Accumulates time taken by successive runs of this task
	unsigned long	RunCount;
	unsigned long	IntervalTicks; // In microseconds
	char			TaskName[50];
	bool			bIsRunning;
};

class Scheduler
{
public:
	Scheduler();
	void 	RegisterTask(Task* task);
	void 	Tick();
	void	RunTask(Task* task); // Runs a specific task
	// Calculates system performance based on time taken by various tasks and stores it in the buffer
	bool 	GetPerformance(char* buffer, unsigned long size);
	// Frequency at which performance is reported;
	void	SetPerfReportFrequency(unsigned int freq) {PerfReportFreq = freq; }
	Task* 	Tasks[MAX_NUM_TASKS];
	unsigned long 	LastRunTime; 	// records the last time scheduler was ticked
	unsigned long 	AggregateTickTime; 	// records the time taken to execute tasks
	unsigned int 	NumTasks;
	unsigned int	PerfReportFreq;
};

#endif
