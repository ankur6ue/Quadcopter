#include "mainwindow.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include "signaldata.h"
#include "echocommanddef.h"


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

	d_timerWheel = new WheelBox( "Sample Interval [ms]", 0.0, 50.0, 1, this );
    d_timerWheel->setValue( 20.0 );

	d_speedWheel = new WheelBox( "Motor Speed", 0.0, 80.0, 1, this );
    d_speedWheel->setValue( 0.0 );
	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Text,Qt::Key_Green);

	QPalette* palette2 = new QPalette();
	palette2->setColor(QPalette::ButtonText, Qt::Key_Green);
	d_motorToggle = new QPushButton(this);
	d_motorToggle->setPalette(*palette2);
	d_motorToggle->setText("Off");
	bMotorToggle = false;

	d_inputTextField = new QLineEdit(this);
	d_inputTextField->setAlignment(Qt::AlignLeft);
	d_inputTextField->setPalette(*palette);

	d_escState = new QLineEdit(this);
	d_escState->setAlignment(Qt::AlignLeft);
	d_escState->setPalette(*palette);

    QVBoxLayout* vLayout1 = new QVBoxLayout();
	vLayout1->setContentsMargins(0, 0, 0, 0);
    vLayout1->addWidget( d_intervalWheel );
    vLayout1->addWidget( d_timerWheel );
	vLayout1->addWidget( d_speedWheel );
	vLayout1->addWidget( d_inputTextField);
	vLayout1->addWidget( d_escState);
	vLayout1->addWidget (d_motorToggle);

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

	connect( d_inputTextField, SIGNAL (textChanged(const QString &)), SLOT(textChanged(const QString &)));

	connect( d_speedWheel, SIGNAL( valueChanged( double ) ), SLOT( speedChanged( double ) ) );

	connect( d_motorToggle, SIGNAL( clicked() ), SLOT( motorToggleClicked() ) );

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

void MainWindow::textChanged(const QString & newText)
{
	int k = 0;
	d_inputTextField->setText(newText);
}

// Handlers for user input
void MainWindow::speedChanged(double speed)
{
	UserCommands::Instance().SetSpeed(speed);
}

void MainWindow::motorToggleClicked()
{
	bMotorToggle = !bMotorToggle;
	d_motorToggle->setText(bMotorToggle? "On": "Off");
	if (bMotorToggle == false) 
	{
		d_speedWheel->setValue(0);
	//	d_speedWheel->valueChanged(0);
	}
	UserCommands::Instance().ToggleMotors(bMotorToggle);
}

void MainWindow::echoCommand(EchoCommand* cmd)
{

	char* commandName = cmd->Command;
	float val = cmd->Val;

	char tmp[50];
	if (!strcmp("Speed", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		d_inputTextField->setText(tmp);
	}
	if (!strcmp("MotorToggle", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		d_escState->setText(tmp);
	}
	delete cmd;
}

