#include "signaldata.h"
#include <qvector.h>
#include <qmutex.h>

class SignalData::PrivateData
{
public:
    PrivateData():
        boundingRect( 1.0, 1.0, -2.0, -2.0 ) // invalid
    {
        values.reserve( 1000 );
    }

    inline void append( const QPointF &sample )
    {
        values.append( sample );

        // adjust the bounding rectangle

        if ( boundingRect.width() < 0 || boundingRect.height() < 0 )
        {
            boundingRect.setRect( sample.x(), sample.y(), 0.0, 0.0 );
        }
        else
        {
            boundingRect.setRight( sample.x() );

            if ( sample.y() > boundingRect.bottom() )
                boundingRect.setBottom( sample.y() );

            if ( sample.y() < boundingRect.top() )
                boundingRect.setTop( sample.y() );
        }
    }

    QReadWriteLock lock;
	
    QVector<QPointF> values;
    QRectF boundingRect;

    QMutex mutex; // protecting pendingValues
    QVector<QPointF> pendingValues;
};

SignalData::SignalData()
{
    pdata = new PrivateData();
}

SignalData::~SignalData()
{
    delete pdata;
}

int SignalData::size() const
{
    return pdata->values.size();
}

QPointF SignalData::value( int index ) const
{
    return pdata->values[index];
}

QRectF SignalData::boundingRect() const
{
    return pdata->boundingRect;
}

void SignalData::lock()
{
    pdata->lock.lockForRead();
}

void SignalData::unlock()
{
    pdata->lock.unlock();
}

void SignalData::append( const QPointF &sample )
{
    pdata->mutex.lock();
    pdata->pendingValues += sample;

    const bool isLocked = pdata->lock.tryLockForWrite();
    if ( isLocked )
    {
        const int numValues = pdata->pendingValues.size();
        const QPointF *pendingValues = pdata->pendingValues.data();

        for ( int i = 0; i < numValues; i++ )
            pdata->append( pendingValues[i] );

        pdata->pendingValues.clear();

        pdata->lock.unlock();
    }

    pdata->mutex.unlock();
}

void SignalData::clearStaleValues( double limit )
{
    pdata->lock.lockForWrite();

    pdata->boundingRect = QRectF( 1.0, 1.0, -2.0, -2.0 ); // invalid

    const QVector<QPointF> values = pdata->values;
    pdata->values.clear();
    pdata->values.reserve( values.size() );

    int index;
    for ( index = values.size() - 1; index >= 0; index-- )
    {
        if ( values[index].x() < limit )
            break;
    }

    if ( index > 0 )
        pdata->append( values[index++] );

    while ( index < values.size() - 1 )
        pdata->append( values[index++] );

    pdata->lock.unlock();
}

SignalData* SignalData::instance(PlotId pid, CurveId cid)
{
	static SignalData valueVector[NUM_PLOTS*NUM_CURVES]; // NumPlots*NumCurves Per Plot
	return &valueVector[cid*NUM_PLOTS + pid];
}