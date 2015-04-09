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
#include "qdatetime.h"
#include <windows.h>

bool DataParserImplCommands::Parse(char* incomingData, int packetLength, char* commandId)
{
	bPrefixFound = false;
	if (PrefixLength == -1) return false;
//	printf("%s\n", incomingData);
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
	DataLength	= dataLength;
	pLogger		= new Logger(Prefix);
	Data = NULL;
	if (DataLength > 0)
		Data = new float[DataLength];
	RadToDegree = 180/M_PI;
	pSamplingThread = samplingThread;
}

void DataParserImpl::ClearLogs()
{
	if (pLogger)
	{
		pLogger->Reset();
	}
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
			float timeStamp;
			QString str;
			if (DataLength == 1)
			{
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t", &timeStamp, Data);
				str.sprintf("%d %f ", DataLength, Data[0]);
			}
			if (DataLength == 2)
			{
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f", &timeStamp, Data, Data+1);
				str.sprintf("%d %f %f ", DataLength, Data[0], Data[1]);
			}
			if (DataLength == 3)
			{
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f\t%f", &timeStamp, Data, Data+1, Data+2);
				str.sprintf("%d %f %f %f", DataLength, Data[0], Data[1], Data[2]);
			}
			if (DataLength == 4)
			{
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f\t%f\t%f", &timeStamp, Data, Data+1, Data+2, Data+3);
				str.sprintf("%d %f %f %f %f", DataLength, Data[0], Data[1], Data[2], Data[3]);
			}
			if (DataLength == 5)
			{
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f\t%f\t%f\t%f", &timeStamp, Data, Data+1, Data+2, Data+3, Data+4);
				str.sprintf("%d %f %f %f %f %f", DataLength, Data[0], Data[1], Data[2], Data[3], Data[4]);
			}
			if (DataLength == 6)
			{
				read = sscanf(incomingData+i+PrefixLength, "\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &timeStamp, Data, Data+1, Data+2, Data+3, Data+4, Data+5);
				str.sprintf("%d %f %f %f %f %f %f", DataLength, Data[0], Data[1], Data[2], Data[3], Data[4], Data[5]);
			}
			if (pSamplingThread->IsRecording())
			{
				Sample* psample = new Sample((int)timeStamp, 0, str);
				pLogger->AddSample(psample);
			}
			bPrefixFound = true;
			break;
		}
	}
	return bPrefixFound;
}

int  DataParserImpl::GetCurrentTimestamp()
{
	Sample* psample = pLogger->GetCurrentSample();
	if (psample)
	{
		return psample->GetTimestamp();
	}
	return 0;
}
bool DataParserImpl::ParsePlaybackData(double elapsed, int timeStamp)
{
	Sample* psample = pLogger->GetCurrentSample();
	pLogger->IncrementCurrentSampleIndex();
	bPrefixFound = false;
	if (psample)
	{
		if (psample->Parse(Data, timeStamp))
		{
			bPrefixFound = true;
			return true;
		}
	}
	else
	{
		for (int i = 0; i < DataLength; i++)
		{
			Data[i] = 0;
		}
	}
	return false;
}

bool DataParser::ParsePlaybackData(double elapsed)
{
	bool ret = false;
	// The idea behind time stamp is as follows: Each data sample stored in the log carries the time at which it was 
	// sent from the embedded system. Some of the samples are lost in transmission or get corrupted during transmission. 
	// This means that samples belong to different data streams (PID, MPU etc) might not exactly correspond. For long logs, 
	// these offsets will accumulate so that points on plots of different data streams with the same x no longer correspond
	// to the same time instant. In order to avoid these discrepancies, we get the timestamp for the first data stream 
	// and ignore those samples from other data streams that don't have a corresponding timestamp
	int timeStamp = DataParsers[0]->GetCurrentTimestamp();
	for (int i = 0; i < DataParsers.length(); i++)
	{
		ret |= DataParsers[i]->ParsePlaybackData(elapsed, timeStamp);
	}
	return ret;
}

bool DataParserImplException::Parse(char* incomingData, int packetLength, char* commandId)
{
	bPrefixFound = false;
	if (PrefixLength == -1) return false;

	for (int i = 0; i < max(0, packetLength-PrefixLength); i++)
	{
		int read = 0;
		char exceptionName[50];
		if (!strncmp(incomingData + i, Prefix, PrefixLength))
		{
			read = sscanf(incomingData+i+PrefixLength, "\t%s\t", exceptionName);
			EchoCommand* cmd = new EchoCommand(Prefix, exceptionName);
			pSamplingThread->signalEchoCommand(cmd);
			bPrefixFound = true;
		}
	}
	return bPrefixFound;
}

// The quadcopter hardware sends each packet of data with a sentinal character ('z') in the end. 
// --data1--z--data2--z--data3--z...
// The sentinal helps us determine if a packet received is a complete data packet and also to separate out
// different data packets. However a packet could be split during data transmission so the sequence of characters
// read doesn't have a sentinal in the end. To handle this case, we split the character string received into substrings
// separated by the sentinal character. The last substring could be either a full packet or could have been split up
// during transmission. If it was split up during transmission, it wouldn't have a terminating sentinal character. Such
// partial substrings are stored in the LastSnippet array and preprended to the next burst of characters received. This
// mechanism allows us to process data packets in a contiguous manner. 
// A key assumption made here is there is no data loss during transmission. This assumption could be easily violated, 
// particularly as the quadcopter travels away from the receiver. We should look into making this algorithm robust to data
// loss during transmission
// Note that technically we should also adjust the "elapsed" parameter to factor in the different times
// the packets are received. For now this is not a problem, but is a good TBD.

void DataParser::ParseData(char* incomingData, int bytesRead, double elapsed)
{
	char* next_token = NULL;
	char* token;
	//	printf("b%d\n", BytesRead);
	printf(incomingData);
	// Prepend new characters read with lastSnippet
	int lastSnippetSize = strlen(cLastSnippet);
	if (lastSnippetSize)
	{
		char tmp[MAX_INCOMING_DATA];
		memcpy(tmp, incomingData, bytesRead);
		memcpy(incomingData, cLastSnippet, lastSnippetSize);
		memcpy(incomingData + lastSnippetSize, tmp, bytesRead);
		// We used this snippet, so put a terminating null in the beginning
		memset(cLastSnippet, 0, MAX_INCOMING_DATA);
		memset(tmp, 0, MAX_INCOMING_DATA);
	}
	// We check for the last character in the incoming data here and not after passing incomingData to
	// strtok as strtok modifies the data passed to it.
	char lastChar = '\0';
	int len = strlen(incomingData);
	if (len >= 1)
	{
		lastChar = incomingData[len - 1];
	}
	token = strtok_s(incomingData, "z", &next_token);
	while (*next_token != '\0')
	{
		// If any parser succeeded, call plot
		if (ParseToken(token, strlen(token), elapsed))
		{ 
			Plot(elapsed);
		}
		
		token = strtok_s(NULL, "z", &next_token);
	}
	// we are at the last snippet. Check if it has a sentinal in the end.
	if (lastChar == 'z')
	{
		// Sentinal was found, this means that the last snippet is a full command packet. Go ahead and parse it
		if (token)
		{
			if(ParseToken(token, strlen(token), elapsed))
			{
				Plot(elapsed);
			}
		}
	}
	else
	{
		// Sentinal was missing, store the lastSnippet and prepend to the next burst of data received, which should 
		// start with the missing characters of this snippet. 
		strcpy(cLastSnippet, token);
	}
	memset(incomingData, 0, MAX_INCOMING_DATA);
}

bool DataParser::ParseToken(char* token, int length, double elapsed)
{
	bool isParsed = false;
	for (int i = 0; i < DataParsers.length(); i++)
	{
		// If any parser returns true, return true
		isParsed |= DataParsers[i]->Parse(token, length);
	}
	return isParsed;
}

bool DataParserImpl::WriteToLog(QDataStream& out)
{
	if (pLogger)
	{
		out << *pLogger;
		return true;
	}
	return false;
}

bool DataParserImpl::ReadFromLog(QDataStream& in)
{
	// Clear any previous data
	pLogger->Reset();
	QString prefix;
	in >> prefix;
	// Must match the prefix for this data parser
	if (prefix.compare(Prefix) != 0) return false;
	// Read number of samples
	QString str;
	in >> str;
	bool ok;
	int numSamples = str.toInt(&ok);
	for (int i = 0; i < numSamples; i++)
	{
		// Read next string. Should be a data string
		in >> str;
		Sample* pSample = new Sample(0, 0, str);
		pLogger->AddSample(pSample);
	}
	return true;
}

void DataParser::ClearLogs()
{
	for (int i = 0; i < DataParsers.length(); i++)
	{
		DataParsers[i]->ClearLogs();
	}
}

bool DataParser::WriteToLog()
{
	// Create log file
	// Get time of the day to create unique log file names
	uint seconds;
	// Can't be dd:hh:mm:ss as filenames can't contain :
	QString fileName = QDateTime::fromTime_t(seconds).toUTC().toString("dd.hh.mm.ss");
	fileName = "C:\\Embedded\\Logs\\" + fileName + ".txt";
	// If file is already open, close it.
	if (File.isOpen())
	{
		File.close();
	}
	File.setFileName(fileName);
	if (!File.open(QIODevice::ReadWrite)) return false;
	QDataStream out(&File);
	
	for (int i = 0; i < DataParsers.length(); i++)
	{
		DataParsers[i]->WriteToLog(out);
	}
	File.close();	
	return true;
}

bool DataParser::ReadFromLog(QString& fileName)
{
	if (File.isOpen())
	{
		File.close();
	}
	File.setFileName(fileName);
	if (!File.open(QIODevice::ReadOnly)) return false;
	QDataStream in(&File);
	bool ret = false;
	for (int i = 0; i < DataParsers.length(); i++)
	{
		if (!DataParsers[i]->ReadFromLog(in))
		{
			File.close();
			return false;
		}
	}
	File.close();
	return true;
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
		const QPointF y1( elapsed, /*RadToDegree*/Data[0]);
		SignalData::instance(yaw, MPU)->append( y1 );

		const QPointF p1( elapsed, /*RadToDegree*/Data[1]);
		SignalData::instance(pitch, MPU)->append( p1);

		const QPointF r1( elapsed, /*RadToDegree*/Data[2]);
		SignalData::instance(roll, MPU)->append( r1 );
	}
}

void DataParserImplMpr::Plot(double elapsed)
{
	if (bPrefixFound)
	{
		/*
		float faccX		= Data[4];
		float faccZ		= Data[3];
		float faccY		= Data[5];

		float accelAngleY = atan(-faccX / sqrt(faccZ*faccZ + faccY*faccY)) * RadToDegree; //atan(-1*accX/sqrt(pow(accY,2) + pow(accZ,2)))*RADIANS_TO_DEGREES;
		float accelAngleX = atan(faccY / faccZ) * RadToDegree; // atan(accY/sqrt(pow(accX,2) + pow(accZ,2)))*RADIANS_TO_DEGREES;


		const QPointF y1( elapsed, 0);
		SignalData::instance(yaw, MPR)->append( y1);

		const QPointF p1( elapsed, accelAngleY);
		SignalData::instance(pitch, MPR)->append( p1);

		const QPointF r1( elapsed, accelAngleX);
		SignalData::instance(roll, MPR)->append( r1 );
		*/
		const QPointF y1(elapsed, 50*Data[0]);
		SignalData::instance(yaw, MPR)->append(y1);

		const QPointF p1(elapsed, 50*Data[1]);
		SignalData::instance(pitch, MPR)->append(p1);

		const QPointF r1(elapsed, 50*Data[2]);
		SignalData::instance(roll, MPR)->append(r1);
	}
}

void DataParserImplPID::Plot(double elapsed)
{
	if (bPrefixFound)
	{
		int SCALE_FACTOR = 1;
		const QPointF y1( elapsed, SCALE_FACTOR*Data[0]);
		SignalData::instance(yaw, PID)->append( y1 );

		const QPointF p1( elapsed, SCALE_FACTOR*Data[1]);
		SignalData::instance(pitch, PID)->append( p1);

		const QPointF r1( elapsed, SCALE_FACTOR*Data[2]);
		SignalData::instance(roll, PID)->append( r1 );
	}
}

void DataParserImplFusion::Plot(double elapsed)
{
	if (bPrefixFound)
	{
		const QPointF r2( elapsed, RadToDegree*Data[0]);
		SignalData::instance(roll, SensorFusion)->append( r2 );

		const QPointF p2( elapsed, RadToDegree*Data[1]);
		SignalData::instance(pitch, SensorFusion)->append( p2);

		const QPointF y2( elapsed, RadToDegree*Data[2]);
		SignalData::instance(yaw, SensorFusion)->append( y2 );
	}
}
