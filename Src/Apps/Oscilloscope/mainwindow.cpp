#include "mainwindow.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>

MainWindow::MainWindow( QWidget *parent ):
    QWidget( parent )
{
    const double intervalLength = 10.0; // seconds
	d_amplitude = 150;
    y_plot = new Plot( this, yaw );
	QwtLegend* plegend = new QwtLegend();
	plegend->setWindowTitle("Legend");
	y_plot->insertLegend(plegend);

    y_plot->setIntervalLength( intervalLength );
	y_plot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	y_plot->setMaximumSize(600, 300);
	y_plot->setAxisScale( QwtPlot::yLeft, -d_amplitude, d_amplitude);
	y_plot->setTitle("Yaw");

	p_plot = new Plot( this, pitch );
    p_plot->setIntervalLength( intervalLength );
	p_plot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	p_plot->setMaximumSize(600, 300);
	p_plot->setAxisScale( QwtPlot::yLeft, -d_amplitude, d_amplitude);
	p_plot->setTitle("Pitch");

	r_plot = new Plot( this, roll );
    r_plot->setIntervalLength( intervalLength );
	r_plot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	r_plot->setMaximumSize(600, 300);
	r_plot->setAxisScale( QwtPlot::yLeft, -d_amplitude, d_amplitude);
	r_plot->setTitle("Roll");

    d_amplitudeKnob = new Knob( "Amplitude", 0.0, 200.0, this );
    d_amplitudeKnob->setValue( 150.0 );

    d_intervalWheel = new WheelBox( "Displayed [s]", 10.0, 100.0, 1.0, this );
    d_intervalWheel->setValue( intervalLength );

    d_timerWheel = new WheelBox( "Sample Interval [ms]", 0.0, 20.0, 0.1, this );
    d_timerWheel->setValue( 20.0 );

    QVBoxLayout* vLayout1 = new QVBoxLayout();
	vLayout1->setContentsMargins(0, 0, 0, 0);
    vLayout1->addWidget( d_intervalWheel );
    vLayout1->addWidget( d_timerWheel );
    vLayout1->addStretch( 10 );
    vLayout1->addWidget( d_amplitudeKnob );
   
	QVBoxLayout* vLayout2 = new QVBoxLayout();
	vLayout1->setContentsMargins(0, 0, 0, 0);
	vLayout2->addWidget( y_plot, 4);
	vLayout2->addWidget( p_plot, 4);
	vLayout2->addWidget( r_plot, 4);

    QHBoxLayout *layout = new QHBoxLayout( this );
	layout->addLayout( vLayout2 );
    layout->addLayout( vLayout1 );

    connect( d_amplitudeKnob, SIGNAL( valueChanged( double ) ),
        SIGNAL( amplitudeChanged( double ) ) );

    connect( d_timerWheel, SIGNAL( valueChanged( double ) ),
        SIGNAL( signalIntervalChanged( double ) ) );

    connect( d_intervalWheel, SIGNAL( valueChanged( double ) ),
        y_plot, SLOT( setIntervalLength( double ) ) );

}

void MainWindow::start()
{
    y_plot->start();
	p_plot->start();
	r_plot->start();
}

double MainWindow::frequency() const
{
    return d_frequencyKnob->value();
}

double MainWindow::amplitude() const
{
    return d_amplitudeKnob->value();
}

double MainWindow::signalInterval() const
{
    return d_timerWheel->value();
}

void MainWindow::setAmplitude( double amplitude )
{
    d_amplitude = amplitude;
	y_plot->setAxisScale( QwtPlot::yLeft, -d_amplitude, d_amplitude);
	p_plot->setAxisScale( QwtPlot::yLeft, -d_amplitude, d_amplitude);
	r_plot->setAxisScale( QwtPlot::yLeft, -d_amplitude, d_amplitude);
	y_plot->replot();
	p_plot->replot();
	r_plot->replot();
}