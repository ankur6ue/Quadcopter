#include "wheelbox.h"
#include <qwt_wheel.h>
#include <qlcdnumber.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qevent.h>
#include <qapplication.h>

class Wheel: public QwtWheel
{
public:
    Wheel( WheelBox *parent ):
        QwtWheel( parent )
    {
        setFocusPolicy( Qt::WheelFocus );
        parent->installEventFilter( this );
    }

    virtual bool eventFilter( QObject *object, QEvent *event )
    {
        if ( event->type() == QEvent::Wheel )
        {
            const QWheelEvent *we = static_cast<QWheelEvent *>( event );

            QWheelEvent wheelEvent( QPoint( 5, 5 ), we->delta(),
                we->buttons(), we->modifiers(),
                we->orientation() );

            QApplication::sendEvent( this, &wheelEvent );
            return true;
        }
        return QwtWheel::eventFilter( object, event );
    }
};

WheelBox::WheelBox( const QString &title,
        double min, double max, double stepSize, QWidget *parent ):
    QWidget( parent )
{

    pnumber = new QLCDNumber( this );
    pnumber->setSegmentStyle( QLCDNumber::Filled );
    pnumber->setAutoFillBackground( true );
    pnumber->setFixedHeight( pnumber->sizeHint().height() * 2 );
    pnumber->setFocusPolicy( Qt::WheelFocus );

    QPalette pal( Qt::black );
    pal.setColor( QPalette::WindowText, Qt::green );
    pnumber->setPalette( pal );

    pwheel = new Wheel( this );
    pwheel->setOrientation( Qt::Vertical );
    pwheel->setInverted( true );
    pwheel->setRange( min, max );
    pwheel->setSingleStep( stepSize );
    pwheel->setPageStepCount( 5 );
    pwheel->setFixedHeight( pnumber->height() );

    pnumber->setFocusProxy( pwheel );

    QFont font( "Helvetica", 10 );
    font.setBold( true );

    plabel = new QLabel( title, this );
    plabel->setFont( font );

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins( 0, 0, 0, 0 );
    hLayout->setSpacing( 2 );
    hLayout->addWidget( pnumber, 10 );
    hLayout->addWidget( pwheel );

    QVBoxLayout *vLayout = new QVBoxLayout( this );
    vLayout->addLayout( hLayout, 10 );
    vLayout->addWidget( plabel, 0, Qt::AlignTop | Qt::AlignHCenter );

    connect( pwheel, SIGNAL( valueChanged( double ) ),
        pnumber, SLOT( display( double ) ) );
    connect( pwheel, SIGNAL( valueChanged( double ) ),
        this, SIGNAL( valueChanged( double ) ) );
}

void WheelBox::setTheme( const QColor &color )
{
    pwheel->setPalette( color );
}

QColor WheelBox::theme() const
{
    return pwheel->palette().color( QPalette::Window );
}

void WheelBox::setValue( double value )
{
    pwheel->setValue( value );
    pnumber->display( value );
}

double WheelBox::value() const
{
    return pwheel->value();
}
