/**************************************************************************

Filename    :   Logger.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "Logger.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"
#include "ErrorsDef.h"
#include "quadcopter.h"

extern int	MPUInterruptCounter;
extern ExceptionMgr	cExceptionMgr;

bool MyLog::AddtoLog(const char* str)
{
	if (bError)
		return false;
	if (!str)
		return false;

	int len = strlen(str);
	if (LogLength + len + 1 < LOGBUFSIZE)
	{
		sprintf(LogBuf + LogLength, "%s", str);
		LogLength += len;
		LogBuf[LogLength] = '\t';
		LogLength++;
		return true;
	} else
	{
		bError = true;
		// sprintf(LogBuf, "ERROR=%d", BUFFER_OVERFLOW);
		LogLength = strlen(LogBuf);
		return false;
	}
}

void MyLog::Reset()
{
	LogLength = 0;
	memset(LogBuf, 0, LOGBUFSIZE);
	bError = false;
}

bool MyLog::AddtoLog(const char* prefix, float numbers[], int numElem)
{
	if (bError)
		return false;

	if (AddtoLog(prefix))
	{
		int i = 0;
		char tempBuf[20];
		do
		{
			if (i >= numElem)
				break;
			dtostrf(numbers[i], 1, 2, tempBuf);
			i++;
		} while (AddtoLog(tempBuf));
	}
	return true;
}

bool MyLog::AddtoLog(const char* prefix, int numbers[], int numElem)
{
	if (bError)
		return false;

	if (AddtoLog(prefix))
	{
		int i = 0;
		char tempBuf[20];
		do
		{
			if (i >= numElem)
				break;
			itoa(numbers[i], tempBuf, 10);
			i++;
		} while (AddtoLog(tempBuf));
	}
	return true;
}

void Logger::Serialize(const char* prefix, const char* name, float val)
{
	char tmp1[50];
	char tmp2[10];
	dtostrf(val, 3, 3, tmp2);
	sprintf(tmp1, "%s %s %s", prefix, name, tmp2);
	SERIAL.print(tmp1);
}

void Logger::AttachSentinal()
{
	SERIAL.print("z");
}

unsigned long QuadStateLogger::Run()
{
	unsigned long before = micros();
	SendQuadState();
	unsigned long now = micros();
	return now - before;
}

void QuadStateLogger::SendQuadState()
{
	Serialize("QS", "PKp", QuadState.Kp);
	Serialize("QS", "PKi", QuadState.Ki);
	Serialize("QS", "PKd", QuadState.Kd);

	Serialize("QS", "Yaw_Kp", QuadState.Yaw_Kp);
	Serialize("QS", "Yaw_Ki", QuadState.Yaw_Ki);
	Serialize("QS", "Yaw_Kd", QuadState.Yaw_Kd);

	Serialize("QS", "A2R_PKp", QuadState.A2R_PKp);
	Serialize("QS", "A2R_RKp", QuadState.A2R_RKp);
	Serialize("QS", "A2R_YKp", QuadState.A2R_YKp);

	Serialize("QS", "MotorToggle", QuadState.bMotorToggle);
	Serialize("QS", "Speed", QuadState.Speed);
	Serialize("QS", "PIDType", QuadState.ePIDType);
	AttachSentinal();
}

unsigned long PIDStateLogger::Run()
{
	unsigned long before = micros();
	float angles[3] =
	{ QuadState.Yaw, QuadState.Pitch, QuadState.Roll };
	pLog->AddtoLog("ypr", angles, 3);
	float pidParams[3] =
	{ QuadState.PID_Yaw, QuadState.PID_Pitch, QuadState.PID_Roll };
	pLog->AddtoLog("PID", pidParams, 3);
	SERIAL.print(pLog->LogBuf);
	AttachSentinal();
	float motionParams[6] = {QuadState.YawOmega, QuadState.PitchOmega, QuadState.RollOmega, QuadState.YawAccel,
			QuadState.PitchAccel, QuadState.RollAccel };
	pLog->Reset();
	pLog->AddtoLog("mpr", motionParams, 6);
	SERIAL.print(pLog->LogBuf);
	AttachSentinal();
	pLog->Reset();

	// SERIAL.print("NumInterrupts = "); SERIAL.println(MPUInterruptCounter);
	MPUInterruptCounter = 0;
	pLog->Reset();
	unsigned long now = micros();
	return now - before;
}

unsigned long ExceptionLogger::Run()
{
	unsigned long before = micros();
	if (cExceptionMgr.IsException())
	{
		switch (cExceptionMgr.GetException())
		{
		case NO_BEACON_SIGNAL:
			SERIAL.println("Exception: NoBeaconReceived");
			AttachSentinal();
			break;
		case EXCESSIVE_PID_OUTPUT:
			SERIAL.println("Exception: ExcessivePIDOutput");
			AttachSentinal();
			break;
		case BAD_MPU_DATA:
			SERIAL.println("Exception: BadMPUData");
			AttachSentinal();
			break;
		}
	}
	unsigned long now = micros();
	return now - before;
}

