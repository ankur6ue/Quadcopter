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
	out << logger.Samples; // << operator for Samples will be called automatically
	return out;
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