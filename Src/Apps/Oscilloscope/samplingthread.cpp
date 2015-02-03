#include "samplingthread.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>
#include "DataParser.h"
#include "commanddef.h"
#include <qlist.h>

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

#define DEGTORAD M_PI/180

#define LOGBUFSIZE 200
enum ERRORS
{
	BUFFER_OVERFLOW = 1
};

SamplingThread::SamplingThread( QObject *parent ):
    QwtSamplingThread( parent ),
    pfrequency( 5.0 ),
    pamplitude( 20.0 ), iDataLength(1500)
{
	SetupSerialPort();
	pDataParser = new DataParser();
	pDataParser->RegisterDataParser(new DataParserImplYpr(this, "ypr", 3));
	pDataParser->RegisterDataParser(new DataParserImplFusion(this, "fusion", 3));
	pDataParser->RegisterDataParser(new DataParserImplPID(this, "PID", 3));
	pDataParser->RegisterDataParser(new DataParserImplCommands(this));
	pDataParser->RegisterDataParser(new DataParserImplBeacon(this));
	pDataParser->RegisterAckParser(new DataParserImplAck(this));

//	fp = fopen("C:\\Qt\\SensorData.txt", "w");
}


// Sets up the serial port on port 16, baud rate = 115200
int SamplingThread::SetupSerialPort()
{
	int success = false;
	Sp = new Serial("\\\\.\\COM19",115200);    // adjust as needed

	if (Sp->IsConnected())
	{
		printf("We're connected");
		success = true;
	}
	return success;
}

void SamplingThread::setFrequency( double frequency )
{
    pfrequency = frequency;
}

void SamplingThread::setAmplitude( double amplitude )
{
    pamplitude = amplitude;
}

double SamplingThread::frequency() const
{
    return pfrequency;
}

double SamplingThread::amplitude() const
{
    return pamplitude;
}

void SamplingThread::sample( double elapsed )
{
    if ( pfrequency > 0.0 )
    {	
		memset(cIncomingData, 0, 2000);
		int bytesRead = Sp->ReadData(cIncomingData,iDataLength);
	//	printf("Bytes read: (-1 means no data available) %i\n",readResult);
		if (bytesRead != -1)
		{
			pDataParser->ParseData(cIncomingData, bytesRead);
			pDataParser->Plot(elapsed);
		}	

		UserCommands* commandInstance = &(UserCommands::Instance());
		if (commandInstance->IsSendBeacon())
		{
			SendBeacon();
		}
		if (commandInstance->IsDirty())
		{
			int numChar = 0;

			// Need to put the commands on the list as there could be more than one flags that were modified.
			// For example when the motors are turned off, speed is also set to zero. 
			QList<CommandDef*> commandList;
			CommandDef* pcommandDef;
			int _speed;
			int motorState;
			bool bmotorToggle;
			PIDFlags PIDflag;
			float kp, ki, kd;

			if (commandInstance->IsDirty(SPEED))
			{
				// We are only reading the shared variables, which is likely to be an atomic operation and locks are likely not required
				// but better be safe than sorry. 
				commandInstance->doLock();
				_speed = commandInstance->GetSpeed();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef( "Speed", (float)_speed);
				
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(MOTORTOGGLE))
			{
				commandInstance->doLock();
				bmotorToggle = commandInstance->GetMotorToggle();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("MotorToggle", (float)bmotorToggle);
				commandList.push_back(pcommandDef);
			}
				
			if (commandInstance->IsDirty(MOTORSTATE))
			{
				commandInstance->doLock();
				motorState = commandInstance->GetMotorState();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("MotorState", motorState);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(DEF_PITCHSETPOINT))
			{
				commandInstance->doLock();
				double defSetPoint = commandInstance->GetDefaultPitchSetpoint()*DEGTORAD;
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("DefPitchSetPt", defSetPoint);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(DEF_ROLLSETPOINT))
			{
				commandInstance->doLock();
				double defSetPoint = commandInstance->GetDefaultRollSetpoint()*DEGTORAD;
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("DefRollSetPt", defSetPoint);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(PITCHSETPOINT))
			{
				commandInstance->doLock();
				double newSetPoint = commandInstance->GetPitchSetpoint()*DEGTORAD;
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("PitchSetPt", newSetPoint);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(ROLLSETPOINT))
			{
				commandInstance->doLock();
				double newSetPoint = commandInstance->GetRollSetpoint()*DEGTORAD;
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("RollSetPt", newSetPoint);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(YAWSETPOINT))
			{
				commandInstance->doLock();
				double newSetPoint = commandInstance->GetYawSetpoint()*DEGTORAD;
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("YawSetPt", newSetPoint);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(PIpPARAMS))
			{
				// Figure out which PID Parameter has been modified
				if (commandInstance->IsPIDFlagDirty(Pitch_Kp))
				{
					commandInstance->doLock();
					kp = commandInstance->GetPitchKp();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("Kp", kp);
					commandList.push_back(pcommandDef);
				}

				if (commandInstance->IsPIDFlagDirty(Pitch_Ki))
				{
					commandInstance->doLock();
					ki = commandInstance->GetPitchKi();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("Ki", ki);
					commandList.push_back(pcommandDef);
				}

				if (commandInstance->IsPIDFlagDirty(Pitch_Kd))
				{
					commandInstance->doLock();
					kd = commandInstance->GetPitchKd();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("Kd", kd);
					commandList.push_back(pcommandDef);
				}

				// Figure out which PID Parameter has been modified
				if (commandInstance->IsPIDFlagDirty(Yaw_Kp))
				{
					commandInstance->doLock();
					kp = commandInstance->GetYawKp();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("Yaw_Kp", kp);
					commandList.push_back(pcommandDef);
				}

				if (commandInstance->IsPIDFlagDirty(Yaw_Ki))
				{
					commandInstance->doLock();
					ki = commandInstance->GetYawKi();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("Yaw_Ki", ki);
					commandList.push_back(pcommandDef);
				}

				if (commandInstance->IsPIDFlagDirty(Yaw_Kd))
				{
					commandInstance->doLock();
					kd = commandInstance->GetYawKd();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("Yaw_Kd", kd);
					commandList.push_back(pcommandDef);
				}
			}
			
			for (int i = 0; i < commandList.length(); i++)
			{
				int numChar = commandList[i]->numChar;
				if (numChar > 0)
				{
					SendCommandAndWaitAck(commandList[i]);
				}
				delete commandList[i];
			}
		}
    }
}

void SamplingThread::SendCommandAndWaitAck(CommandDef* pcommandDef)
{
	bool	receivedAck = false;
	int		tryCount = 0;
	int		numTries = 4;
	int		numChar = pcommandDef->numChar;
	while (!receivedAck && tryCount < numTries)
	{
		Sp->WriteData(pcommandDef->CommandBuffer, numChar);
		Sleep(50);
		receivedAck = BlockTillReply(50, pcommandDef->CommandName);
		tryCount++;
	}
	if (receivedAck)
	{
		printf("ack received after %d tries\n", tryCount);
	}
	else
	{
		printf("command not acknowledged\n");
	}
}

void SamplingThread::SendBeacon()
{
	static unsigned long now = 0;
	static unsigned long before = 0;
	CommandDef commandDef("Beacon", 0);
	now = GetTickCount();
	if ((now - before) > 500)
	{
		SendCommandAndWaitAck(&commandDef);
		before = now;
	}
}

/**
 * Waits for reply from sender or timeout before continuing
 */
bool SamplingThread::BlockTillReply(unsigned long timeout, char* ackCmdId)
{
    unsigned long time  = GetTickCount();
    unsigned long start = time;
    bool receivedAck    = false;
    while( (time - start ) < timeout && !receivedAck) {
		// wait for a few ms before checking again
		Sleep(5);
		time = GetTickCount();
        receivedAck = CheckForAck(ackCmdId);
    }
    return receivedAck;
}

bool SamplingThread::CheckForAck(char* ackCmdId)
{
	memset(cIncomingData, 0, 2000); 
	int bytesRead = Sp->ReadData(cIncomingData,iDataLength);
	if (bytesRead != -1)
	{
		return pDataParser->ParseAck(cIncomingData, bytesRead, ackCmdId);
	}
	return false;
}

double SamplingThread::value( double timeStamp ) const
{
    const double period = 1.0 / pfrequency;

    const double x = ::fmod( timeStamp, period );
    const double v = pamplitude * qFastSin( x / period * 2 * M_PI );

    return v;
}

/*
void SamplingThread::run()
{
   
}
*/