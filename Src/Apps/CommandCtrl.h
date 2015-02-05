/**************************************************************************

Filename    :   CommandCtrl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

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
