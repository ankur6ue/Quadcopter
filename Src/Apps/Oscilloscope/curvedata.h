/**************************************************************************

Filename    :   CurveData.h
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include <qwt_series_data.h>
#include <qpointer.h>
#include "plotiddef.h"

class SignalData;

class CurveData: public QwtSeriesData<QPointF>
{
public:
	CurveData (PlotId pid, CurveId cid): QwtSeriesData<QPointF>(), ePlotId(pid), eCurveId(cid) {};
	PlotId ePlotId;
	CurveId eCurveId;
    const SignalData &values() const;
    SignalData &values();

    virtual QPointF sample( size_t i ) const;
    virtual size_t size() const;

    virtual QRectF boundingRect() const;
	SignalData& GetSignalInstance() const;
};
