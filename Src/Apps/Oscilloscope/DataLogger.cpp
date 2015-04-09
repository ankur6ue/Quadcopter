/**************************************************************************

Filename    :   DataLogger.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#include "DataLogger.h"

QDataStream &operator<<(QDataStream &out, Logger& logger)
{
	out << logger.sName;
	QString str; str.sprintf("%d", logger.NumSamples);
	out << str;
	logger.WriteSamples(out);
	return out;
}

QDataStream &operator>>(QDataStream &in, Logger& logger)
{
	in >> logger.sName;
	logger.ReadSamples(in);
//	logger.WriteSamples(out);
	return in;
}

QDataStream &operator<<(QDataStream &out, Sample& sample)
{
	sample.Write(out);
	return out;
}

void Sample::Write(QDataStream& out)
{
	out << sData; 
}

bool Sample::Parse(float* pdata, int timeStamp)
{
	if (TimeSent != timeStamp) return false;
	QRegExp rx("[ ]");// match a space
	QStringList list = sData.split(rx, QString::SkipEmptyParts);
	// read number of data points in each sample
	int numDataPoints = list[0].toUInt();
	for (int i = 0; i < numDataPoints; i++)
	{
		pdata[i] = list.at(i + 1).toFloat();
	}
	return true;
}

int Sample::GetTimestamp()
{
	return TimeSent;
}