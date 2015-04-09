#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>
#include "plotiddef.h"
#include <qlist.h>
#include <qwt_legend.h>

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;
class CurveDrawData;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget * = NULL, PlotId pid = yaw );
    virtual ~Plot();

    void start();
    virtual void replot();

    virtual bool eventFilter( QObject *, QEvent * );
	PlotId	ePlotId;
	bool	bUpdate;
	QwtSystemClock	pclock;
public Q_SLOTS:
    void setIntervalLength( double );

protected:
    virtual void showEvent( QShowEvent * );
    virtual void resizeEvent( QResizeEvent * );
    virtual void timerEvent( QTimerEvent * );

private:
    void updateCurve();
    void incrementInterval();
	void updateCurve(CurveDrawData* pcurveDrawData);
    void incrementInterval(CurveDrawData* pcurveDrawData);
	void replot(CurveDrawData* pcurveDrawData);

    QwtPlotMarker *porigin;
	QList<CurveDrawData*> CurveDrawDataList;
 
    QwtInterval		pinterval;
    int				ptimerId;
};
