#include "samplingthread.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>
#include "sensordataparser.h"
#include <qlist.h>

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

#define LOGBUFSIZE 200
enum ERRORS
{
	BUFFER_OVERFLOW = 1
};


void SensorDataParserImplCommands::Parse(char* incomingData, int packetLength)
{
	bPrefixFound = false;
	if (PrefixLength == -1) return;
	printf("%s\n", incomingData);
	for (int i = 0; i < max(0, packetLength-PrefixLength); i++)
	{
		if (!strncmp(incomingData + i, Prefix, PrefixLength))
		{
			float val1, val2;
			
			if (sscanf(incomingData + i, "AT Speed %f", &val2))
			{
				EchoCommand* cmd = new EchoCommand("Speed", val2);
				pSamplingThread->signalEchoCommand(cmd);
			}

			if (sscanf(incomingData + i, "AT MotorToggle %f", &val1))
			{
				EchoCommand* cmd = new EchoCommand("MotorToggle", val1);
				pSamplingThread->signalEchoCommand(cmd);
			}
		}
	}
}

SamplingThread::SamplingThread( QObject *parent ):
    QwtSamplingThread( parent ),
    d_frequency( 5.0 ),
    d_amplitude( 20.0 ), iDataLength(1500)
{
	SetupSerialPort();
	pSensorDataParser = new SensorDataParser();
	pSensorDataParser->RegisterDataParser(new SensorDataParserImplYpr(this, "ypr", 3));
	pSensorDataParser->RegisterDataParser(new SensorDataParserImplFusion(this, "fusion", 3));
	pSensorDataParser->RegisterDataParser(new SensorDataParserImplCommands(this));

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
    d_frequency = frequency;
}

void SamplingThread::setAmplitude( double amplitude )
{
    d_amplitude = amplitude;
}

double SamplingThread::frequency() const
{
    return d_frequency;
}

double SamplingThread::amplitude() const
{
    return d_amplitude;
}

void SamplingThread::sample( double elapsed )
{
    if ( d_frequency > 0.0 )
    {
		
		memset(cIncomingData, 0, 2000);
		int bytesRead = Sp->ReadData(cIncomingData,iDataLength);
	//	printf("Bytes read: (-1 means no data available) %i\n",readResult);
		if (bytesRead != -1)
		{
			pSensorDataParser->Parse(cIncomingData, bytesRead);
			pSensorDataParser->Plot(elapsed);
		}	

		UserCommands* commandInstance = &(UserCommands::Instance());
		if (commandInstance->IsDirty())
		{
			int numChar = 0;
			typedef struct
			{
				char	command[32];
				int		numChar;
			} CommandDef;

			// Need to put the commands on the list as there could be more than one flags that were modified.
			// For example when the motors are turned off, speed is also set to zero. 
			QList<CommandDef*> commandList;
			CommandDef* pcommandDef;
			
			if (commandInstance->IsDirty(SPEED))
			{
				// We are only reading the shared variables, which is likely to be an atomic operation and locks are likely not required
				// but better be safe than sorry. 
				commandInstance->doLock();
				int _speed = commandInstance->GetSpeed();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef();
				pcommandDef->numChar  = commandInstance->CreateCommand(pcommandDef->command, "Speed", (float)_speed);
				commandList.push_back(pcommandDef);
			}

			if (commandInstance->IsDirty(MOTORTOGGLE))
			{
				commandInstance->doLock();
				bool bmotorToggle = commandInstance->GetMotorToggle();
				commandInstance->doUnlock();
				pcommandDef = new CommandDef();
				pcommandDef->numChar = commandInstance->CreateCommand(pcommandDef->command, "MotorToggle", (float)bmotorToggle);
				commandList.push_back(pcommandDef);
			}
			
			for (int i = 0; i < commandList.length(); i++)
			{
				int numChar = commandList[i]->numChar;
				if (numChar > 0)
				{
					Sp->WriteData(commandList[i]->command, numChar);
				}
				delete commandList[i];
			}
		}
    }
}

double SamplingThread::value( double timeStamp ) const
{
    const double period = 1.0 / d_frequency;

    const double x = ::fmod( timeStamp, period );
    const double v = d_amplitude * qFastSin( x / period * 2 * M_PI );

    return v;
}

/*
void SamplingThread::run()
{
   
}
*/