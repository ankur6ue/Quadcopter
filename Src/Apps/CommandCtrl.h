/*
 Command Control
 */
#ifndef __COMMANDCONTROL__
#define __COMMANDCONTROL__

#include "Scheduler.h"

struct Command;

class CommandCtrl: public Task
{
public:
	CommandCtrl(int frequency, char* _name): Task(frequency, _name){}

	virtual ~CommandCtrl(){};

	virtual unsigned long Run();

	bool CommandReceived(Command* cmd);

	void ProcessCommands(Command* cmd);
};
#endif
