/**************************************************************************

Filename    :   CurveData.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "curvedata.h"
#include "signaldata.h"

SignalData& CurveData::GetSignalInstance() const
{
	return SignalData::instance(ePlotId, eCurveId);
}

const SignalData &CurveData::values() const
{
    return GetSignalInstance();
}

SignalData &CurveData::values()
{
    return GetSignalInstance();
}

QPointF CurveData::sample( size_t i ) const
{
    return GetSignalInstance().value( i );
}

size_t CurveData::size() const
{
    return GetSignalInstance().size();
}

QRectF CurveData::boundingRect() const
{
    return GetSignalInstance().boundingRect();
}
