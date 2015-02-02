#ifndef __SCHEDULER__
#define __SCHEDULER__

#define MAX_NUM_TASKS 10

class Task
{
public:
	Task(){};
	//Frequency is in Hertz
	Task(int frequency, const char* name);

	void 	Execute();
	virtual unsigned long	Run() { return 0; }
	unsigned long	LastRunTime; // records when the task was last run
	unsigned long	TimeTaken; // records the time taken to run the task
	unsigned long	IntervalTicks; // In microseconds
	char			TaskName[50];
};

class Scheduler
{
public:
	Scheduler();
	void 	RegisterTask(Task* task);
	void 	Tick();
	void	RunTask(Task* task); // Runs a specific task

	Task* 	Tasks[MAX_NUM_TASKS];
	unsigned long 	LastRunTime; 	// records the last time scheduler was ticked
	unsigned long 	TimeTaken; 	// records the time taken to execute the last tick
	unsigned int 	NumTasks;
};

#endif
