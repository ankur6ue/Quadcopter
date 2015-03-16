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
	bool AddtoLog(const char* str);
	void Reset();
	bool AddtoLog(const char* prefix, float numbers[], int numElem);
	bool AddtoLog(const char* prefix, int numbers[], int numElem);
	void AddTerminatingNull()
	{
		LogBuf[LogLength] = '\0';
	}
	char 	LogBuf[LOGBUFSIZE+1];
	int 	LogLength;
	bool 	bError;
};

class Logger: public Task
{
public:
	Logger(float frequency, MyLog* _plog, const char* name): Task(frequency, name)
	{
		SetLog(_plog);
	}

	virtual ~Logger(){};

	void Serialize(const char* prefix, const char* name, float val);
	void AttachSentinal();
	void SetLog(MyLog* _log) {pLog = _log; };

	MyLog* pLog;
};

class QuadStateLogger: public Logger
{
public:
	QuadStateLogger(float frequency, MyLog* _plog, const char* _name): Logger(frequency, _plog, _name){};
	virtual unsigned long Run();
	void SendQuadState();
};

class PIDStateLogger: public Logger
{
public:
	PIDStateLogger(float frequency, MyLog* _plog, const char* _name): Logger(frequency, _plog, _name){};
	virtual unsigned long Run();
};

class ExceptionLogger: public Logger
{
public:
	ExceptionLogger(float frequency, MyLog* _plog, const char* _name): Logger(frequency, _plog, _name){};
	virtual unsigned long Run();
};



