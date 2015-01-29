#include "knob.h"
#include <qwt_math.h>
#include <qpen.h>
#include <qwt_knob.h>
#include <qwt_round_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qevent.h>

Knob::Knob( const QString &title, double min, double max, QWidget *parent ):
    QWidget( parent )
{
    QFont font( "Helvetica", 10 );

    pknob = new QwtKnob( this );
    pknob->setFont( font );

    QwtScaleDiv scaleDiv =
        pknob->scaleEngine()->divideScale( min, max, 5, 3 );

    QList<double> ticks = scaleDiv.ticks( QwtScaleDiv::MajorTick );
    if ( ticks.size() > 0 && ticks[0] > min )
    {
        if ( ticks.first() > min )
            ticks.prepend( min );
        if ( ticks.last() < max )
            ticks.append( max );
    }
    scaleDiv.setTicks( QwtScaleDiv::MajorTick, ticks );
    pknob->setScale( scaleDiv );

    pknob->setKnobWidth( 50 );

    font.setBold( true );
    plabel = new QLabel( title, this );
    plabel->setFont( font );
    plabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );

    setSizePolicy( QSizePolicy::MinimumExpanding,
        QSizePolicy::MinimumExpanding );

    connect( pknob, SIGNAL( valueChanged( double ) ),
        this, SIGNAL( valueChanged( double ) ) );
}

QSize Knob::sizeHint() const
{
    QSize sz1 = pknob->sizeHint();
    QSize sz2 = plabel->sizeHint();

    const int w = qMax( sz1.width(), sz2.width() );
    const int h = sz1.height() + sz2.height();

    int off = qCeil( pknob->scaleDraw()->extent( pknob->font() ) );
    off -= 15; // spacing

    return QSize( w, h - off );
}

void Knob::setValue( double value )
{
    pknob->setValue( value );
}

double Knob::value() const
{
    return pknob->value();
}

void Knob::setTheme( const QColor &color )
{
    pknob->setPalette( color );
}

QColor Knob::theme() const
{
    return pknob->palette().color( QPalette::Window );
}

void Knob::resizeEvent( QResizeEvent *event )
{
    const QSize sz = event->size();
    const QSize hint = plabel->sizeHint();

    plabel->setGeometry( 0, sz.height() - hint.height(),
        sz.width(), hint.height() );

    const int knobHeight = pknob->sizeHint().height();

    int off = qCeil( pknob->scaleDraw()->extent( pknob->font() ) );
    off -= 15; // spacing

    pknob->setGeometry( 0, plabel->pos().y() - knobHeight + off,
        sz.width(), knobHeight );
}
