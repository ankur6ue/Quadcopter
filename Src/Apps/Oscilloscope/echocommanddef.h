#ifndef __ECHOCOMMANDDEF
#define __ECHOCOMMANDDEF

struct EchoCommand
{
	EchoCommand(char* _command, float _val)
	{
		strcpy(Command, _command);
		Val = _val;
	}

	EchoCommand(char* _command, char* _commandArgs)
	{
		strcpy(Command, _command);
		strcpy(CommandArgs, _commandArgs);
	}
	char Command[50];
	char CommandArgs[50];
	float Val;
};

#endif