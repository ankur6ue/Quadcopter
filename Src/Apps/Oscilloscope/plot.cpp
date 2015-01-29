#include "plot.h"
#include "curvedata.h"
#include "signaldata.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>

// Stores the state for each curve such as the Painter, paintedPoints etc. 
class CurveDrawData
{
public:
	CurveDrawData()
	{
		pPlotCurve = new QwtPlotCurve();
		pDirectPainter = new QwtPlotDirectPainter(); 
		ppaintedPoints = 0;
	}
	~CurveDrawData()
	{
		if (pPlotCurve) delete pPlotCurve;
		if (pDirectPainter) delete pDirectPainter;
	}
	QwtPlotCurve* pPlotCurve;
	int ppaintedPoints;
    QwtPlotDirectPainter *pDirectPainter;
};

class Canvas: public QwtPlotCanvas
{
public:
    Canvas( QwtPlot *plot = NULL ):
        QwtPlotCanvas( plot )
    {
        // The backing store is important, when working with widget
        // overlays ( f.e rubberbands for zooming ).
        // Here we don't have them and the internal
        // backing store of QWidget is good enough.

        setPaintAttribute( QwtPlotCanvas::BackingStore, false );
        setBorderRadius( 10 );

        if ( QwtPainter::isX11GraphicsSystem() )
        {
#if QT_VERSION < 0x050000
            // Even if not liked by the Qt development, Qt::WA_PaintOutsidePaintEvent
            // works on X11. This has a nice effect on the performance.

            setAttribute( Qt::WA_PaintOutsidePaintEvent, true );
#endif

            // Disabling the backing store of Qt improves the performance
            // for the direct painter even more, but the canvas becomes
            // a native window of the window system, receiving paint events
            // for resize and expose operations. Those might be expensive
            // when there are many points and the backing store of
            // the canvas is disabled. So in this application
            // we better don't disable both backing stores.

            if ( testPaintAttribute( QwtPlotCanvas::BackingStore ) )
            {
                setAttribute( Qt::WA_PaintOnScreen, true );
                setAttribute( Qt::WA_NoSystemBackground, true );
            }
        }

        setupPalette();
    }

private:
    void setupPalette()
    {
        QPalette pal = palette();

#if QT_VERSION >= 0x040400
        QLinearGradient gradient;
        gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
        gradient.setColorAt( 0.0, QColor( 0, 49, 110 ) );
        gradient.setColorAt( 1.0, QColor( 0, 87, 174 ) );

        pal.setBrush( QPalette::Window, QBrush( gradient ) );
#else
        pal.setBrush( QPalette::Window, QBrush( color ) );
#endif

        // QPalette::WindowText is used for the curve color
        pal.setColor( QPalette::WindowText, Qt::green );

        setPalette( pal );
    }
};

Plot::Plot( QWidget *parent, PlotId pid ):
    QwtPlot( parent ),
    pinterval( 0.0, 10.0 ),
    ptimerId( -1 ),
	ePlotId (pid)
{
    setAutoReplot( false );
    setCanvas( new Canvas() );
    plotLayout()->setAlignCanvasToScales( true );

    //setAxisTitle( QwtPlot::xBottom, "Time [s]" );
    setAxisScale( QwtPlot::xBottom, pinterval.minValue(), pinterval.maxValue() );
    setAxisScale( QwtPlot::yLeft, -50.0, 50.0 );
	//setFrameRect(QRect(0,0, 500, 80));
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );

    porigin = new QwtPlotMarker();
    porigin->setLineStyle( QwtPlotMarker::Cross );
    porigin->setValue( pinterval.minValue() + pinterval.width() / 2.0, 0.0 );
    porigin->setLinePen( Qt::gray, 0.0, Qt::DashLine );
    porigin->attach( this );

	for (int i = 0; i < 2; i++)
	{
		CurveDrawData *curveDrawData = new CurveDrawData();
		QwtPlotCurve* pcurve1 = curveDrawData->pPlotCurve;
		pcurve1->setStyle( QwtPlotCurve::Lines );
		if (i == 0)
		{
			pcurve1->setPen( canvas()->palette().color(QPalette::Light ) );
			pcurve1->setTitle("MPU");
		}
		if (i == 1)
		{
			pcurve1->setPen( canvas()->palette().color(QPalette::WindowText) );
			pcurve1->setTitle("PID");
		}
		pcurve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );
		pcurve1->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
		pcurve1->setData( new CurveData(pid, (CurveId)i) );
		pcurve1->attach( this );
		CurveDrawDataList.push_back(curveDrawData);

	}
}

Plot::~Plot()
{
	for (int i = 0; i < CurveDrawDataList.length(); i++)
	{
		delete CurveDrawDataList[i];
	}
//    delete pdirectPainter;
}

void Plot::start()
{
    pclock.start();
    ptimerId = startTimer( 10 );
}

void Plot::replot(CurveDrawData* pcurveDrawData)
{
	CurveData *data = static_cast<CurveData *>( pcurveDrawData->pPlotCurve->data() );
    data->values().lock();

    QwtPlot::replot();
	pcurveDrawData->ppaintedPoints = data->size();

    data->values().unlock();
}

void Plot::replot()
{
	for (int i = 0; i < CurveDrawDataList.length(); i++)
	{
		replot(CurveDrawDataList[i]);
	}
}

void Plot::setIntervalLength( double interval )
{
    if ( interval > 0.0 && interval != pinterval.width() )
    {
        pinterval.setMaxValue( pinterval.minValue() + interval );
        setAxisScale( QwtPlot::xBottom,
            pinterval.minValue(), pinterval.maxValue() );

        replot();
    }
}

void Plot::updateCurve(CurveDrawData* pcurveDrawData)
{
	QwtPlotCurve* pcurve = pcurveDrawData->pPlotCurve;
	CurveData *data = static_cast<CurveData *>( pcurve->data() );
    data->values().lock();

    const int numPoints = data->size();
    if ( numPoints > pcurveDrawData->ppaintedPoints )
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip )
        {
            /*
                Depending on the platform setting a clip might be an important
                performance issue. F.e. for Qt Embedded this reduces the
                part of the backing store that has to be copied out - maybe
                to an unaccelerated frame buffer device.
            */

            const QwtScaleMap xMap = canvasMap( pcurve->xAxis() );
            const QwtScaleMap yMap = canvasMap( pcurve->yAxis() );

            QRectF br = qwtBoundingRect( *data,
                pcurveDrawData->ppaintedPoints - 1, numPoints - 1 );

            const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br ).toRect();
            pcurveDrawData->pDirectPainter->setClipRegion( clipRect );
        }

        pcurveDrawData->pDirectPainter->drawSeries( pcurve,
            pcurveDrawData->ppaintedPoints - 1, numPoints - 1 );
        pcurveDrawData->ppaintedPoints = numPoints;
    }

    data->values().unlock();
}

void Plot::updateCurve()
{
	for (int i = 0; i < CurveDrawDataList.length(); i++)
	{
		updateCurve(CurveDrawDataList[i]);
	}
}

void Plot::incrementInterval(CurveDrawData* pcurveDrawData)
{
    pinterval = QwtInterval( pinterval.maxValue(),
        pinterval.maxValue() + pinterval.width() );

    
    // To avoid, that the grid is jumping, we disable
    // the autocalculation of the ticks and shift them
    // manually instead.

    QwtScaleDiv scaleDiv = axisScaleDiv( QwtPlot::xBottom );
    scaleDiv.setInterval( pinterval );

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        QList<double> ticks = scaleDiv.ticks( i );
        for ( int j = 0; j < ticks.size(); j++ )
            ticks[j] += pinterval.width();
        scaleDiv.setTicks( i, ticks );
    }
    setAxisScaleDiv( QwtPlot::xBottom, scaleDiv );

    porigin->setValue( pinterval.minValue() + pinterval.width() / 2.0, 0.0 );

    pcurveDrawData->ppaintedPoints = 0;
    replot();
}

void Plot::incrementInterval()
{
	for (int i = 0; i < CurveDrawDataList.length(); i++)
	{
		QwtPlotCurve* pcurve = CurveDrawDataList[i]->pPlotCurve;

		CurveData *data = static_cast<CurveData *>( pcurve->data() );
	//	data->values().clearStaleValues( pinterval.minValue() );
	}
	incrementInterval(CurveDrawDataList[0]);
}

void Plot::timerEvent( QTimerEvent *event )
{
    if ( event->timerId() == ptimerId )
    {
        updateCurve();

        const double elapsed = pclock.elapsed() / 1000.0;
        if ( elapsed > pinterval.maxValue() )
            incrementInterval();

        return;
    }

    QwtPlot::timerEvent( event );
}

void Plot::resizeEvent( QResizeEvent *event )
{
	for (int i = 0; i < CurveDrawDataList.length(); i++)
	{
		CurveDrawDataList[i]->pDirectPainter->reset();
	}
    QwtPlot::resizeEvent( event );
}

void Plot::showEvent( QShowEvent * )
{
    replot();
}

bool Plot::eventFilter( QObject *object, QEvent *event )
{
    if ( object == canvas() && 
        event->type() == QEvent::PaletteChange )
    {
		for (int i = 0; i < CurveDrawDataList.length(); i++)
		{
			CurveDrawDataList[i]->pPlotCurve->setPen( canvas()->palette().color( QPalette::WindowText ) );
		}
    }

    return QwtPlot::eventFilter( object, event );
}
