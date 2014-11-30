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
