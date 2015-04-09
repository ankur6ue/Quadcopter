/**************************************************************************

Filename    :   SamplingThread.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "samplingthread.h"
#include "Serial.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>
#include "DataParser.h"
#include "commanddef.h"
#include <qlist.h>
#include "../MatrixOps.h"
#include "QPushButton.h"

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
	pamplitude( 20.0 ), iDataLength(4000), BytesRead(0)
{
	SetupSerialPort();
	pDataParser = new DataParser();
	pDataParser_Playback = new DataParser();
	RegisterDataParsers(pDataParser);
	RegisterDataParsers(pDataParser_Playback);
	bRunFlightLog = false;
}

SamplingThread::~SamplingThread()
{
	if (pDataParser)
		delete pDataParser;
	if (pDataParser_Playback)
		delete pDataParser_Playback;
}

void SamplingThread::RegisterDataParsers(DataParser* pParser)
{
	pParser->RegisterDataParser(new DataParserImplYpr(this, "ypr", 3));
	pParser->RegisterDataParser(new DataParserImplFusion(this, "fusion", 3));
	pParser->RegisterDataParser(new DataParserImplPID(this, "PID", 3));
	pParser->RegisterDataParser(new DataParserImplMpr(this, "mpr", 6));
	pParser->RegisterDataParser(new DataParserImplException(this));
	pParser->RegisterDataParser(new DataParserImplCommands(this));
	//	pParser->RegisterDataParser(new DataParserImplBeacon(this));
	pParser->RegisterAckParser(new DataParserImplAck(this));
}
// Sets up the serial port on port 16, baud rate = 115200
int SamplingThread::SetupSerialPort()
{
	int success		= false;
	bIsRecording	= false;
	Sp = new Serial("\\\\.\\COM4",115200);    // adjust as needed
//	Sp = new Serial("\\\\.\\COM20",115200);
//	Sp = new Serial("\\\\.\\COM23",115200);

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

void SamplingThread::recordToggleClicked()
{
	bIsRecording = !bIsRecording;
	if (bIsRecording == false)
	{
		pDataParser->WriteToLog();
		pDataParser->ClearLogs();
	}
}

double SamplingThread::frequency() const
{
	return pfrequency;
}

double SamplingThread::amplitude() const
{
	return pamplitude;
}

void SamplingThread::onPlayToggled()
{
	QPushButton* button = (QPushButton*)(sender());
	bIsPlaying = !bIsPlaying;
	button->setText(bIsPlaying ? "Stop" : "Play");
}
// Called when user selects a log file from the log file listview
void SamplingThread::onLogFileSelected(QString fileName)
{
	QString fullName = "C:\\Embedded\\Logs\\" + fileName;
	pDataParser_Playback->ReadFromLog(fullName);
	bRunFlightLog	= true;
	bIsPlaying		= true;
}

void SamplingThread::sample( double elapsed )
{
	if (UserCommands::Instance().GetOnInitFlag())
	{
		MotorsOff();
		UserCommands::Instance().SetOnInitFlag(false);
	}

	if ( pfrequency > 0.0 )
	{
		// If we want to run the flight log and play log is set to true
		if (bRunFlightLog && bIsPlaying)
		{
			if (pDataParser_Playback->ParsePlaybackData(elapsed))
			{
				pDataParser_Playback->Plot(elapsed);
			}
			else // If all parsers failed, that means there is no more data in the log. Set runflightlog to false
			{
				bRunFlightLog = false;
				emit logPlaybackOver();
			}
		}
		if (!bRunFlightLog)
		{
			BytesRead = Sp->ReadData(cIncomingData, iDataLength);
			if (BytesRead != -1)
			{
				pDataParser->ParseData(cIncomingData, BytesRead, elapsed);
			}
			else
			{
			//	printf("missed\n");
			}
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

			if (commandInstance->IsDirty(PITCHHOVERATTITUDE))
			{
				commandInstance->doLock();
				double pitchHoverAtt = commandInstance->GetPitchHoverAttitude();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("PitchHoverAtt", pitchHoverAtt);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(ROLLHOVERATTITUDE))
			{
				commandInstance->doLock();
				double rollHoverAtt = commandInstance->GetRollHoverAttitude();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("RollHoverAtt", rollHoverAtt);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(PITCHDISPLACEMENT))
			{
				commandInstance->doLock();
				double disp = commandInstance->GetPitchDisplacement();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("PitchDisp", disp);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(ROLLDISPLACEMENT))
			{
				commandInstance->doLock();
				double disp = commandInstance->GetRollDisplacement();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("RollDisp", disp);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(YAWDISPLACEMENT))
			{
				commandInstance->doLock();
				double disp = commandInstance->GetYawDisplacement();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("YawDisp", disp);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(ALPHA))
			{
				commandInstance->doLock();
				double alpha = commandInstance->GetDCMAlpha();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("DCMAlpha", alpha);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(PIDTYPE))
			{
				commandInstance->doLock();
				int pidType = commandInstance->GetPIDType();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef("PIDType", (float)pidType);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(PIDPARAMS))
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
				if (commandInstance->IsPIDFlagDirty(A2RPitch))
				{
					commandInstance->doLock();
					kp = commandInstance->GetA2RPitch();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("A2R_PKp", kp);
					commandList.push_back(pcommandDef);
				}
				if (commandInstance->IsPIDFlagDirty(A2RRoll))
				{
					commandInstance->doLock();
					kp = commandInstance->GetA2RRoll();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("A2R_RKp", kp);
					commandList.push_back(pcommandDef);
				}
				if (commandInstance->IsPIDFlagDirty(A2RYaw))
				{
					commandInstance->doLock();
					kp = commandInstance->GetA2RYaw();
					commandInstance->doUnlock();
					pcommandDef = new CommandDef("A2R_YKp", kp);
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
	memset(cIncomingDataAck, 0, 2000); 
	int bytesRead = Sp->ReadData(cIncomingDataAck,iDataLength);
	if (bytesRead != -1)
	{
		return pDataParser->ParseAck(cIncomingDataAck, bytesRead, ackCmdId);
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