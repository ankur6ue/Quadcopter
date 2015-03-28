/**************************************************************************

Filename    :   Logger.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "arduino.h"
#include "Scheduler.h"

#define LOGBUFSIZE 200

class MyLog
{
public:
	MyLog()
	{
		LogLength = 0;
		bError = false;
	};

/*	bool EchoCommand(const char* str)
	{
		sprintf(LogBuf+LogLength, "%s", str);
		LogLength += strlen(str);
		return true;
	}

	char* CreateCommand(const char* str, int val)
	{
	//	memset(LogBuf, 0, LOGBUFSIZE);
		char tmp[50];
		sprintf(tmp, "AT,%s,%d ", str, val);
		sprintf(LogBuf+LogLength, "%s", tmp);
		LogLength += strlen(tmp);
		return LogBuf;
	}
*/
	bool 	AddtoLog(const char* str);
	void 	Reset();
	int  	GetLogLength();
	bool 	AddtoLog(const char* prefix, float numbers[], int numElem);
	bool 	AddtoLog(const char* prefix, int numbers[], int numElem);
	void 	AddTerminatingNull()
	{
		LogBuf[LogLength] = '\0';
	}
	char 	LogBuf[LOGBUFSIZE+1];
	int 	LogLength;
	bool 	bError;
};

class PrintHelper
{
public:
	PrintHelper(){};
	virtual ~PrintHelper(){};

	static void Serialize(const char* prefix, const char* name, float val);
	static void AttachSentinal();
	static void Print(char* str);
};

class QuadStateLogger: public Task
{
public:
	QuadStateLogger(float frequency, const char* _name): Task(frequency, _name){};
	virtual unsigned long Run();
	void SendQuadState();
	MyLog Log;
};

class PIDStateLogger: public Task
{
public:
	PIDStateLogger(float frequency, const char* _name): Task(frequency, _name){};
	virtual unsigned long Run();
	MyLog Log;
};

class ExceptionLogger: public Task
{
public:
	ExceptionLogger(float frequency, const char* _name): Task(frequency, _name){};
	virtual unsigned long Run();
	MyLog Log;
};



