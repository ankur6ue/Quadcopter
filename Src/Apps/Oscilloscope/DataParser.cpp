/**************************************************************************

Filename    :   DataParser.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "samplingthread.h"
#include "signaldata.h"
#include "DataParser.h"
#include <qwt_math.h>

bool DataParserImplCommands::Parse(char* incomingData, int packetLength, char* commandId)
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

bool DataParserImplBeacon::Parse(char* incomingData, int packetLength, char* commandId)
{
	bPrefixFound = false;
	if (PrefixLength == -1) return false;
	for (int i = 0; i < max(0, packetLength-PrefixLength); i++)
	{
		// Beacon Parser simply echoes that a Beacon has been received
		if (!strncmp(incomingData + i, Prefix, PrefixLength))
		{
			printf("Beacon Ack'ed\n");
			EchoCommand* cmd = new EchoCommand(Prefix, 0.0);
			pSamplingThread->signalEchoCommand(cmd);
			bPrefixFound = true;
		}
	}
	return bPrefixFound;
}

bool DataParserImplAck::Parse(char* incomingData, int packetLength, char* commandId)
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

DataParserImpl::DataParserImpl(SamplingThread* samplingThread, const char* _prefix, int dataLength)
{
	if (_prefix)
	{
		strcpy_s(Prefix, 100, _prefix);
		PrefixLength = strlen(Prefix);
	}
	else
	{
		PrefixLength = -1;
	}
	DataLength = dataLength;
	RadToDegree = 180/3.14;
	pSamplingThread = samplingThread;
}

bool DataParserImpl::Parse(char* incomingData, int packetLength, char* commandId)
{
	bPrefixFound = false;
	if (PrefixLength == -1) return false;

	for (int i = 0; i < max(0, packetLength-PrefixLength); i++)
	{
		if (!strncmp(incomingData + i, Prefix, PrefixLength))
		{
			int read = 0;
			if (DataLength == 1)
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t", Data);
			if (DataLength == 2)
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f", Data, Data+1);
			if (DataLength == 3)
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f", Data, Data+1, Data+2);
			if (DataLength == 4)
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f\t%f", Data, Data+1, Data+2, Data+3);

			bPrefixFound = true;
			break;
		}
	}
	return bPrefixFound;
}


bool DataParser::ParseData(char* incomingData, int packetLength)
{
	bool isParsed = false;
	for (int i = 0; i < DataParsers.length(); i++)
	{
		isParsed = DataParsers[i]->Parse(incomingData, packetLength);
	}
	return isParsed;
}

bool DataParser::ParseAck(char* incomingData, int packetLength, char* commandId)
{
	bool isParsed = false;
	for (int i = 0; i < AckParsers.length(); i++)
	{
		isParsed = AckParsers[i]->Parse(incomingData, packetLength, commandId);
	}
	return isParsed;
}

void DataParser::Plot(double elapsed)
{
	for (int i = 0; i < DataParsers.length(); i++)
	{
		DataParsers[i]->Plot(elapsed);
	}
}

void DataParserImplYpr::Plot(double elapsed)
{
	if (bPrefixFound)
	{
		const QPointF y1( elapsed, RadToDegree*Data[0]);
		SignalData::instance(yaw, MPU).append( y1 );

		const QPointF p1( elapsed, RadToDegree*Data[1]);
		SignalData::instance(pitch, MPU).append( p1);

		const QPointF r1( elapsed, RadToDegree*Data[2]);
		SignalData::instance(roll, MPU).append( r1 );
	}
}

void DataParserImplPID::Plot(double elapsed)
{
	if (bPrefixFound)
	{
		const QPointF y1( elapsed, 10*Data[0]);
		SignalData::instance(yaw, PID).append( y1 );

		const QPointF p1( elapsed, 10*Data[1]);
		SignalData::instance(pitch, PID).append( p1);

		const QPointF r1( elapsed, 10*Data[2]);
		SignalData::instance(roll, PID).append( r1 );
	}
}

void DataParserImplFusion::Plot(double elapsed)
{
	if (bPrefixFound)
	{
		const QPointF r2( elapsed, RadToDegree*Data[0]);
		SignalData::instance(roll, SensorFusion).append( r2 );

		const QPointF p2( elapsed, RadToDegree*Data[1]);
		SignalData::instance(pitch, SensorFusion).append( p2);

		const QPointF y2( elapsed, RadToDegree*Data[2]);
		SignalData::instance(yaw, SensorFusion).append( y2 );
	}
}
