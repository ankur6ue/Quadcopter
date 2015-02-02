#ifndef __ECHOCOMMANDDEF
#define __ECHOCOMMANDDEF

struct EchoCommand
{
	EchoCommand(char* _command, float _val)
	{
		strcpy(Command, _command);
		Val = _val;
	}
	char Command[50];
	float Val;
};

#endif