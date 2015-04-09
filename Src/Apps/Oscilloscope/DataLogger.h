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
#include "qstringlist.h"
#include "qregexp.h"
#include "qdatastream.h"

#define NUMSAMPLES 6000 // Enough to store 10 minutes of data sent 10 times per second. 10*60*10; 
class Sample
{
public:
	Sample(int timeSent, int timeReceived, QString& sdata): TimeSent(timeSent), TimeReceived(timeReceived), sData(sdata){};
	virtual ~Sample(){};
	virtual void	Write(QDataStream& out);
	virtual bool	Parse(float* pdata, int timeStamp);
	virtual int		GetTimestamp();
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
		sName			= sname;
		NumSamples		= 0;
		CurrentSample	= 0;
	}
	~Logger()
	{
		for (int i = 0; i < Samples.length(); i++)
		{
			if (Samples[i])
			{
				delete Samples[i];
			}
		}
		Samples.clear();
	}
	// Clear CurrSample
	void Reset()
	{
		Samples.clear();
		NumSamples		= 0;
		CurrentSample	= 0;
	}

	void AddSample(Sample* psample)
	{
		Samples.push_back(psample);
		NumSamples++;
	}
	void IncrementCurrentSampleIndex() { CurrentSample++;  }

	Sample* GetCurrentSample()
	{
		if (CurrentSample < NumSamples)
		{
			return Samples[CurrentSample];	
		}
		return NULL;
	}
	void WriteSamples(QDataStream& out)
	{
		int len = Samples.length();
		for (int i = 0; i < len; i++)
		{
			if (Samples[i])
			{
				out << *Samples[i];
			}
		}
	}

	void ReadSamples(QDataStream& in)
	{
		QString str;
		in >> str;
		QRegExp rx("[ ]");// match a space
		QStringList list = str.split(rx, QString::SkipEmptyParts);
		float data[3];
		for (int i = 0; i < list.size(); i++)
		{
			data[i] = list.at(i).toFloat();
		}
	}

	QList<Sample*>		Samples;
	int					NumSamples;
	QString				sName; // Indicates what's being logged - MPU data, PID data etc. 
	int					CurrentSample;
};

QDataStream& operator<<(QDataStream& out, Logger& logger);
QDataStream& operator>>(QDataStream& in, Logger& logger);

#endif