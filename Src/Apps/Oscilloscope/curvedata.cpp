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
