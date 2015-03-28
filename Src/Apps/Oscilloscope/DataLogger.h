/**************************************************************************

Filename    :   DataLogger.h
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#ifndef DATALOGGER_H
#define DATALOGGER_H

#include "qlist.h"
#include "QString.h"
#include "qdatastream.h"

#define NUMSAMPLES 6000 // Enough to store 10 minutes of data sent 10 times per second. 10*60*10; 
class Sample
{
public:
	Sample(int timeSent, int timeReceived, QString& sdata): TimeSent(timeSent), TimeReceived(timeReceived), sData(sdata){};
	virtual void	Write(QDataStream& out);
	//	virtual void	Read() {};
	// Its the responsibility of the system creating the sample to convert the data into a string.
	QString			sData;
	unsigned int	TimeSent;
	unsigned int	TimeReceived;
	
};

QDataStream &operator << (QDataStream& out, Sample& sample);

class Logger
{
public:
	// Name describes what we are logging. MPU data, PID data, exceptions etc. 
	Logger(QString sname)
	{
		sName		= sname;
		CurrSample	= 0;
	}
	~Logger()
	{
		Samples.clear();
	}
	// Clear CurrSample
	void Reset()
	{
		CurrSample = 0;
	}

	void AddSample(Sample* psample)
	{
		Samples.push_back(psample);
		NumSamples++;
	}

	QList<Sample*>		Samples;
	int					NumSamples;
	int					CurrSample;
	QString				sName; // Indicates what's being logged - MPU data, PID data etc. 
};

QDataStream& operator<<(QDataStream& out, Logger& logger);

#endif