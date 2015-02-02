// Executes a callback function every given interval
#include "arduino.h"
#include "Scheduler.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"

#define LOGBUFSIZE 70
extern int MPUInterruptCounter;

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
	bool AddtoLog(const char* str)
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
	};

	void Reset()
	{
		LogLength = 0;
		memset(LogBuf, 0, LOGBUFSIZE);
		bError = false;
	};

	bool AddtoLog(const char* prefix, float numbers[], int numElem)
	{
		if (bError) return false;

		if (AddtoLog(prefix))
		{
			int i = 0;
			char tempBuf[20];
			do
			{
				if (i >= numElem) break;
				dtostrf(numbers[i], 1,2,tempBuf);
				i++;
			} while(AddtoLog(tempBuf));
		}
		return true;
	};

	bool AddtoLog(const char* prefix, int numbers[], int numElem)
	{
		if (bError) return false;

		if (AddtoLog(prefix))
		{
			int i = 0;
			char tempBuf[20];
			do
			{
				if (i >= numElem) break;
				itoa(numbers[i], tempBuf, 10);
				i++;
			} while(AddtoLog(tempBuf));
		}
		return true;
	};

	void AddTerminatingNull()
	{
		LogBuf[LogLength] = '\0';
	}

	char 	LogBuf[LOGBUFSIZE+1];
	int 	LogLength;
	bool 	bError;
};

class Pinger: public Task
{
public:
	Pinger(int frequency, MyLog* _plog, const char* name): Task(frequency, name)
	{
		SetLog(_plog);
	}

	virtual ~Pinger(){};

	void Serialize(const char* prefix, const char* name, float val)
	{
		char tmp1[50];
		char tmp2[10];
		dtostrf(val, 3,3,tmp2);
		sprintf(tmp1, "%s %s %s", prefix, name, tmp2);
		SERIAL.print(tmp1);
	}

	void SetLog(MyLog* _log) {pLog = _log; };

	MyLog* pLog;
};

class QuadStatePinger: public Pinger
{
public:
	QuadStatePinger(int frequency, MyLog* _plog, const char* _name): Pinger(frequency, _plog, _name){};

	virtual unsigned long Run()
	{
		unsigned long before = micros();
		SendQuadState();
		unsigned long now = micros();
		return now - before;
	}

	void SendQuadState()
	{
		Serialize("QuadState", "PKp", QuadState.Kp);
		Serialize("QuadState", "PKi", QuadState.Ki);
		Serialize("QuadState", "PKd", QuadState.Kd);

		Serialize("QuadState", "Yaw_Kp", QuadState.Yaw_Kp);
		Serialize("QuadState", "Yaw_Ki", QuadState.Yaw_Ki);
		Serialize("QuadState", "Yaw_Kd", QuadState.Yaw_Kd);

		Serialize("QuadState", "MotorToggle", QuadState.bMotorToggle);
		Serialize("QuadState", "Speed", QuadState.Speed);
	}
};

class PIDStatePinger: public Pinger
{
public:
	PIDStatePinger(double _interval, MyLog* _plog, const char* _name): Pinger(_interval, _plog, _name){};

	virtual unsigned long Run()
	{
		unsigned long before = micros();
		float angles[3] = {QuadState.Yaw, QuadState.Pitch, QuadState.Roll};
		pLog->AddtoLog("ypr", angles, 3);
		float pidParams[3] = {QuadState.PID_Yaw, QuadState.PID_Pitch, QuadState.PID_Roll};
		pLog->AddtoLog("PID", pidParams , 3);
		pLog->AddTerminatingNull();
		SERIAL.print(pLog->LogBuf);
		// SERIAL.print("NumInterrupts = "); SERIAL.println(MPUInterruptCounter);
		MPUInterruptCounter = 0;
		pLog->Reset();
		unsigned long now = micros();
		return now - before;
	}

};

