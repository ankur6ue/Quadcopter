#include "samplingthread.h"
#include "signaldata.h"
#include "sensordataparser.h"
#include <qwt_math.h>

bool SensorDataParserImplCommands::Parse(char* incomingData, int packetLength, char* commandId)
{
	bPrefixFound = false;
	if (PrefixLength == -1) return false;
	printf("%s\n", incomingData);
	for (int i = 0; i < max(0, packetLength-PrefixLength); i++)
	{
		if (!strncmp(incomingData + i, Prefix, PrefixLength))
		{
			char paramName[50];
			float val;
			sscanf(incomingData+i+PrefixLength, "%s %f", paramName, &val);
			EchoCommand* cmd = new EchoCommand(paramName, val);
			pSamplingThread->signalEchoCommand(cmd);
			bPrefixFound = true;
		}
	}
	return bPrefixFound;
}


bool SensorDataParserImplAck::Parse(char* incomingData, int packetLength, char* commandId)
{
	bPrefixFound = false;
	if (PrefixLength == -1) return false;

	for (int i = 0; i < max(0, packetLength-PrefixLength); i++)
	{
		if (!strncmp(incomingData + i, Prefix, PrefixLength))
		{
			int read = 0;
			float val;
			if (DataLength == 1)
			{
				char ackCommandId[50];
				read = sscanf(incomingData+i+PrefixLength, "\t%s %f", ackCommandId, &val);
				if (!strcmp(commandId, ackCommandId))
				{
					bPrefixFound = true;
					printf("Ack for %s received, param = %f\n", ackCommandId, val);
					break;
				}
			}
		}
	}
	return bPrefixFound;
}

