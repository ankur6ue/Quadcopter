#include "mainwindow.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include "signaldata.h"
#include "commanddef.h"
#include "echocommanddef.h"
#include "joystick.h"

void MainWindow::ReadPIDParams(PIDParamsDef& yparams, PIDParamsDef& pparams, PIDParamsDef& rparams)
{
	FILE* fp = fopen("C:\\Embedded\\PIDParams.txt", "r");
	if (fp)
	{
		fscanf(fp, "YawPIDParams %f %f %f\n", &yparams.fKp, &yparams.fKi, &yparams.fKd); 
		fscanf(fp, "PitchPIDParams %f %f %f\n", &pparams.fKp, &pparams.fKi, &pparams.fKd); 
		fscanf(fp, "RollPIDParams %f %f %f\n", &rparams.fKp, &rparams.fKi, &rparams.fKd); 
		fscanf(fp, "DefaultRollSetPoint %d\n", &DefaultRollSetPoint); 
		fscanf(fp, "DefaultPitchSetPoint %d\n", &DefaultPitchSetPoint); 
	}
	else
	{
		yparams.fKp = 12; yparams.fKi = 0; yparams.fKd = 5;
		pparams.fKp = 24; pparams.fKi = 3; pparams.fKd = 10;
		rparams.fKp = 24; rparams.fKi = 3; rparams.fKd = 10;
		DefaultPitchSetPoint = -4; DefaultRollSetPoint = -5;
	}
}

void MainWindow::SetupCtrlInput()
{
	pThread = new QThread;
	pJoystick = new Joystick(NULL);
	pJoystick->Init();
	//obj is a pointer to a QObject that will trigger the work to start. It could just be this
	connect(this, SIGNAL(startCtrlInputThread()), pJoystick, SLOT(Run()));
	connect(pJoystick, SIGNAL(AxisMoved(long, long, long, int)), this, SLOT(AxisMoved(long, long, long, int)));
	pJoystick->moveToThread(pThread);
	pThread->start();
	startCtrlInputThread();
}

MainWindow::MainWindow( QWidget *parent ):
    QWidget( parent )
{
    const double intervalLength = 10.0; // seconds
	pamplitude = 150;
	// Initialize the Joystick
	SetupCtrlInput();
    y_plot = new Plot( this, yaw );
//	QwtLegend* plegend = new QwtLegend();
//	plegend->setWindowTitle("Legend");
//	y_plot->insertLegend(plegend);

    y_plot->setIntervalLength( intervalLength );
//	y_plot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	y_plot->setMaximumSize(600, 300);
	y_plot->setAxisScale( QwtPlot::yLeft, -pamplitude, pamplitude);
	y_plot->setTitle("Yaw");

	p_plot = new Plot( this, pitch );
    p_plot->setIntervalLength( intervalLength );
//	p_plot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	p_plot->setMaximumSize(600, 300);
	p_plot->setAxisScale( QwtPlot::yLeft, -pamplitude, pamplitude);
	p_plot->setTitle("Pitch");

	r_plot = new Plot( this, roll );
    r_plot->setIntervalLength( intervalLength );
//	r_plot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	r_plot->setMaximumSize(600, 300);
	r_plot->setAxisScale( QwtPlot::yLeft, -pamplitude, pamplitude);
	r_plot->setTitle("Roll");

	QGroupBox* gpBox1 = new QGroupBox("Plot Parameters", this);

    pAmplitudeKnob = new Knob( "Amplitude", 0.0, 200.0, this );
    pAmplitudeKnob->setValue( 100.0 );

    pIntervalWheel = new WheelBox( "Displayed [s]", 10.0, 100.0, 1.0, this );
//	pIntervalWheel->setFixedSize(200, 100);
    pIntervalWheel->setValue( intervalLength );

	pTimerWheel = new WheelBox( "Sample Interval [ms]", 0.0, 50.0, 1, this );
    pTimerWheel->setValue( 20.0 );

	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Text,Qt::Key_Green);

	QPalette* palette2 = new QPalette();
	palette2->setColor(QPalette::ButtonText, Qt::Key_Green);
	QVBoxLayout* vLayout1 = new QVBoxLayout();
	QVBoxLayout* vLayout1a = new QVBoxLayout();
	gpBox1->setLayout(vLayout1a);
//	vLayout1a->setContentsMargins(0,0,0,50);
//	vLayout1a->setSpacing(0);
//	vLayout1->setMargin(0);

    vLayout1a->addWidget( pIntervalWheel, 0, Qt::AlignHCenter );
    vLayout1a->addWidget( pTimerWheel, 0, Qt::AlignHCenter );
    vLayout1a->addWidget( pAmplitudeKnob, 0, Qt::AlignHCenter );

	// Quadcopter PID Control Parameters

	QGroupBox* gpBox3 = new QGroupBox("Quadcopter PID Control", this);
	QGridLayout* vLayout1c = new QGridLayout();
	vLayout1c->setSpacing(0);
	vLayout1c->setMargin(0);

	ReadPIDParams(YawPIDParams, PitchPIDParams, RollPIDParams);

	pPitchKp = new WheelBox( "Kp", 0, 30, 1, this );
    pPitchKp->setValue( PitchPIDParams.fKp );

	pPitchKi = new WheelBox( "Ki", 0, 3, 0.05, this );
    pPitchKi->setValue( PitchPIDParams.fKi );

	pPitchKd = new WheelBox( "Kd", 0, 30, 1, this );
    pPitchKd->setValue( PitchPIDParams.fKd );

	pYawKp = new WheelBox( "Yaw_Kp", 0, 15, 0.5, this );
    pYawKp->setValue( YawPIDParams.fKp );

	pYawKi = new WheelBox( "Yaw_Ki", 0, 1, 0.01, this );
    pYawKi->setValue( YawPIDParams.fKi );

	pYawKd = new WheelBox( "Yaw_Kd", 0, 4, 0.05, this );
    pYawKd->setValue( YawPIDParams.fKd );

	vLayout1c->addWidget (pPitchKp, 0, 0, 1, 1, Qt::AlignCenter);
	vLayout1c->addWidget (pPitchKi, 0, 1, 1, 1, Qt::AlignCenter);
	vLayout1c->addWidget (pPitchKd, 0, 2, 1, 1, Qt::AlignCenter);
	vLayout1c->addWidget (pYawKp, 1, 0, 1, 1, Qt::AlignCenter);
	vLayout1c->addWidget (pYawKi, 1, 1, 1, 1, Qt::AlignCenter);
	vLayout1c->addWidget (pYawKd, 1, 2, 1, 1, Qt::AlignCenter);

	gpBox3->setLayout(vLayout1c);

	// UI elements representing Quadcopter state

	QGroupBox* gpBox4 = new QGroupBox("Quadcopter State", this);

	QLabel* pQuadSpeedLabel = new QLabel("QuadSpeed", this);
	pQuadSpeed = new QLineEdit(this);
	pQuadSpeed->setAlignment(Qt::AlignLeft);
	pQuadSpeed->setPalette(*palette);

	QLabel* pQuadPowerLabel = new QLabel("QuadPower", this);
	pQuadPower = new QLineEdit(this);
	pQuadPower->setAlignment(Qt::AlignLeft);
	pQuadPower->setPalette(*palette);

	QLabel* pQuadPLabel = new QLabel("Pitch", this);
	QLabel* pQuadRLabel = new QLabel("Roll", this);
	QLabel* pQuadYLabel = new QLabel("Yaw", this);

	QLabel* pQuadKiLabel = new QLabel("Ki", this);
	QLabel* pQuadKpLabel = new QLabel("Kp", this);
	QLabel* pQuadKdLabel = new QLabel("Kd", this);

	pQuadPKi = new QLineEdit(this);	pQuadPKi->setAlignment(Qt::AlignLeft); pQuadPKi->setPalette(*palette); 
	pQuadPKp = new QLineEdit(this);	pQuadPKp->setAlignment(Qt::AlignLeft); pQuadPKp->setPalette(*palette); 
	pQuadPKd = new QLineEdit(this);	pQuadPKd->setAlignment(Qt::AlignLeft);	pQuadPKd->setPalette(*palette);

	pQuadRKi = new QLineEdit(this);	pQuadRKi->setAlignment(Qt::AlignLeft); pQuadRKi->setPalette(*palette); 
	pQuadRKp = new QLineEdit(this);	pQuadRKp->setAlignment(Qt::AlignLeft); pQuadRKp->setPalette(*palette); 
	pQuadRKd = new QLineEdit(this);	pQuadRKd->setAlignment(Qt::AlignLeft);	pQuadRKd->setPalette(*palette);

	pQuadYKi = new QLineEdit(this);	pQuadYKi->setAlignment(Qt::AlignLeft); pQuadYKi->setPalette(*palette); 
	pQuadYKp = new QLineEdit(this);	pQuadYKp->setAlignment(Qt::AlignLeft); pQuadYKp->setPalette(*palette); 
	pQuadYKd = new QLineEdit(this);	pQuadYKd->setAlignment(Qt::AlignLeft);	pQuadYKd->setPalette(*palette);
	
	QGridLayout* vLayout1d = new QGridLayout();
	vLayout1d->setSpacing(0);
	vLayout1d->setMargin(0);
	vLayout1d->setContentsMargins(0,0,0,50);
	vLayout1d->addWidget( pQuadSpeedLabel, 0, 0, 1, 2, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadSpeed, 0, 2, 1, 2, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadPowerLabel, 1, 0, 1, 2, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadPower, 1, 2, 1, 2, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadPLabel, 2, 1, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadRLabel, 2, 2, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadYLabel, 2, 3, 1, 1, Qt::AlignLeft);

	vLayout1d->addWidget( pQuadKpLabel, 3, 0, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadKiLabel, 4, 0, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadKdLabel, 5, 0, 1, 1, Qt::AlignLeft);

	vLayout1d->addWidget( pQuadPKp, 3, 1, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadRKp, 3, 2, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadYKp, 3, 3, 1, 1, Qt::AlignLeft);

	vLayout1d->addWidget( pQuadPKi, 4, 1, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadRKi, 4, 2, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadYKi, 4, 3, 1, 1, Qt::AlignLeft);

	vLayout1d->addWidget( pQuadPKd, 5, 1, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadRKd, 5, 2, 1, 1, Qt::AlignLeft);
	vLayout1d->addWidget( pQuadYKd, 5, 3, 1, 1, Qt::AlignLeft);
	
	gpBox4->setLayout(vLayout1d);

	vLayout1->addWidget(gpBox1);
	vLayout1->addWidget(gpBox3);
	vLayout1->addWidget(gpBox4);
	//vLayout1->addLayout(vLayout1c, 0);

	QVBoxLayout* vLayout2 = new QVBoxLayout();
	vLayout2->addWidget( y_plot, 4);
	vLayout2->addWidget( p_plot, 4);
	vLayout2->addWidget( r_plot, 4);

	// Groubox for the Pitch/Yaw Control

	QGroupBox* gpBox6= new QGroupBox("Roll/Pitch SetPoints", this);
	QGridLayout* vLayout3b = new QGridLayout();
	gpBox6->setLayout(vLayout3b);

	pPitchSetPtWheel = new WheelBox( "Pitch Setpoint", -25, 25, 1, this );
	pRollSetPtWheel = new WheelBox( "Roll Setpoint", -25, 25, 1, this );
	pPitchSetPtWheel->setValue(DefaultPitchSetPoint);
	pRollSetPtWheel->setValue(DefaultRollSetPoint);

	vLayout3b->addWidget( pPitchSetPtWheel, 0, 0, 1, 1, Qt::AlignLeft);
	vLayout3b->addWidget( pRollSetPtWheel, 1, 0, 1, 1, Qt::AlignLeft);

	QVBoxLayout* vLayout3 = new QVBoxLayout();
	QGroupBox* gpBox5 = new QGroupBox("Pitch/Roll Control", this);
	QGridLayout* vLayout3a = new QGridLayout();

	pPitchCtrlWheel	= new WheelBox( "Pitch Ctrl", -PITCH_CTRL_RANGE, PITCH_CTRL_RANGE, 1, this );
	pRollCtrlWheel	= new WheelBox( "Roll Ctrl", -ROLL_CTRL_RANGE, ROLL_CTRL_RANGE, 1, this );
	pYawCtrlWheel	= new WheelBox( "Yaw Ctrl", -YAW_CTRL_RANGE, YAW_CTRL_RANGE, 1, this );

	/*
	pArrowPad = new ArrowPadDef(this);
	pArrowPad->pButtonUp = new QPushButton("Up", this);
	pArrowPad->pButtonDown = new QPushButton("Down", this);
	pArrowPad->pButtonLeft = new QPushButton("Left", this);
	pArrowPad->pButtonRight = new QPushButton("Right", this);
	pArrowPad->ConnectSignals();
	vLayout3a->addWidget( pArrowPad->pButtonUp, 0, 1, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pArrowPad->pButtonLeft, 1, 0, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pArrowPad->pButtonRight, 1, 2, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pArrowPad->pButtonDown, 2, 1, 1, 1, Qt::AlignLeft);
	*/
	vLayout3a->addWidget( pRollCtrlWheel, 0, 0, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pPitchCtrlWheel, 1, 0, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pYawCtrlWheel, 2, 0, 1, 1, Qt::AlignLeft);

	gpBox5->setLayout(vLayout3a);

	// User inputs to the quadcopter

	QGroupBox* gpBox2 = new QGroupBox("Quadcopter Power Control", this);

	pSpeedWheel = new WheelBox( "Speed", 0.0, 1500.0, 2, this );
	pSpeedWheel->setContentsMargins(0,0,0,0);
    pSpeedWheel->setValue( 0.0 );

	pMotorToggle = new QPushButton("Off", this);
	pMotorToggle->setCheckable(true);
	pMotorToggle->setPalette(*palette2);
	pMotorToggle->setFixedSize(100, 50);
	pMotorToggle->setContentsMargins(0,0,0,0);

	bMotorToggle = false;

	pFR = new QCheckBox("FR", this);
	pBR = new QCheckBox("BR", this);
	pFL = new QCheckBox("FL", this);
	pBL = new QCheckBox("BL", this);

	QGridLayout* vLayout3c = new QGridLayout();

	vLayout3c->addWidget (pFR, 2, 0, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget (pFL, 2, 1, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget (pBR, 3, 0, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget (pBL, 3, 1, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget( pSpeedWheel, 0, 0, 1, 2, Qt::AlignHCenter);
	vLayout3c->addWidget (pMotorToggle, 1, 0, 1, 2, Qt::AlignHCenter);
	
	gpBox2->setLayout(vLayout3c);
	vLayout3->addWidget(gpBox5, Qt::AlignTop);
	vLayout3->addWidget(gpBox6, Qt::AlignTop);
	vLayout3->addWidget(gpBox2, Qt::AlignTop);
	
    QHBoxLayout *layout = new QHBoxLayout( this );
	layout->addLayout( vLayout2 );
    layout->addLayout( vLayout1 );
	layout->addLayout( vLayout3);

    connect( pAmplitudeKnob, SIGNAL( valueChanged( double ) ),
        SIGNAL( amplitudeChanged( double ) ) );

    connect( pTimerWheel, SIGNAL( valueChanged( double ) ),
        SIGNAL( signalIntervalChanged( double ) ) );

    connect( pIntervalWheel, SIGNAL( valueChanged( double ) ),
        y_plot, SLOT( setIntervalLength( double ) ) );

	connect( pQuadSpeed, SIGNAL (textChanged(const QString &)), SLOT(textChanged(const QString &)));

	connect( pSpeedWheel, SIGNAL( valueChanged( double ) ), SLOT( speedChanged( double ) ) );

	connect( pPitchKp, SIGNAL( valueChanged( double ) ), SLOT( PitchKpChanged( double ) ) );

	connect( pPitchKi, SIGNAL( valueChanged( double ) ), SLOT( PitchKiChanged( double ) ) );

	connect( pPitchKd, SIGNAL( valueChanged( double ) ), SLOT( PitchKdChanged( double ) ) );

	connect( pYawKp, SIGNAL( valueChanged( double ) ), SLOT( YawKpChanged( double ) ) );

	connect( pYawKi, SIGNAL( valueChanged( double ) ), SLOT( YawKiChanged( double ) ) );

	connect( pYawKd, SIGNAL( valueChanged( double ) ), SLOT( YawKdChanged( double ) ) );

	connect( pPitchSetPtWheel, SIGNAL( valueChanged( double ) ), SLOT( DefaultPitchSetPtChanged( double ) ) );

	connect( pRollSetPtWheel, SIGNAL( valueChanged( double ) ), SLOT( DefaultRollSetPtChanged( double ) ) );

	connect( pPitchCtrlWheel, SIGNAL( valueChanged( double ) ), SLOT( PitchCtrlChanged( double ) ) );

	connect( pRollCtrlWheel, SIGNAL( valueChanged( double ) ), SLOT( RollCtrlChanged( double ) ) );

	connect( pYawCtrlWheel, SIGNAL( valueChanged( double ) ), SLOT( YawCtrlChanged( double ) ) );

	connect( pFR, SIGNAL( stateChanged( int ) ), SLOT( FRStateChanged( int ) ) );

	connect( pBR, SIGNAL( stateChanged( int ) ), SLOT( BRStateChanged( int ) ) );

	connect( pFL, SIGNAL( stateChanged( int ) ), SLOT( FLStateChanged( int ) ) );

	connect( pBL, SIGNAL( stateChanged( int ) ), SLOT( BLStateChanged( int ) ) );

	connect( pMotorToggle, SIGNAL( clicked() ), SLOT( motorToggleClicked() ) );

}

void MainWindow::start()
{
    y_plot->start();
	p_plot->start();
	r_plot->start();
}

double MainWindow::frequency() const
{
    return pFrequencyKnob->value();
}

double MainWindow::amplitude() const
{
    return pAmplitudeKnob->value();
}

double MainWindow::signalInterval() const
{
    return pTimerWheel->value();
}

void MainWindow::setAmplitude( double amplitude )
{
    pamplitude = amplitude;
	y_plot->setAxisScale( QwtPlot::yLeft, -pamplitude, pamplitude);
	p_plot->setAxisScale( QwtPlot::yLeft, -pamplitude, pamplitude);
	r_plot->setAxisScale( QwtPlot::yLeft, -pamplitude, pamplitude);
	y_plot->replot();
	p_plot->replot();
	r_plot->replot();
}

void MainWindow::AxisMoved(long lX, long lY, long lZ, int moveFlag)
{
	printf("lX:%d, lY:%d, lZ:%d\n", lX, lY, lZ);
	if (moveFlag & X_AXIS)
		pPitchCtrlWheel->setValue(lX/1000.0*PITCH_CTRL_RANGE);
	if (moveFlag & Y_AXIS)
		pRollCtrlWheel->setValue(lY/1000.0*ROLL_CTRL_RANGE);
	if (moveFlag & Z_AXIS);
		pYawCtrlWheel->setValue(lZ/1000.0*YAW_CTRL_RANGE);
	if (moveFlag & START)
	{
		bool bval = pMotorToggle->isChecked();
		pMotorToggle->setChecked(!bval);
		pMotorToggle->clicked(); // emits the clicked() signal
	}

	if (moveFlag & SPEEDUP)
	{
		double speed = pSpeedWheel->value();
		pSpeedWheel->setValue(++speed);
	}

	if (moveFlag & SPEEDDOWN)
	{
		double speed = pSpeedWheel->value();
		pSpeedWheel->setValue(--speed);
	}
}

void MainWindow::textChanged(const QString & newText)
{
	int k = 0;
	pQuadSpeed->setText(newText);
}

// Handlers for user input
void MainWindow::speedChanged(double speed)
{
	UserCommands::Instance().SetSpeed(speed);
}

// Handlers for user input
void MainWindow::DefaultPitchSetPtChanged(double setPoint)
{
	DefaultPitchSetPoint = setPoint;
//	UserCommands::Instance().SetPitchSetpoint(setPoint);
}

// Handlers for user input
void MainWindow::DefaultRollSetPtChanged(double setPoint)
{
	DefaultRollSetPoint = setPoint;
//	UserCommands::Instance().SetRollSetpoint(setPoint);
}

// Handlers for user input
void MainWindow::PitchCtrlChanged(double setPoint)
{
	UserCommands::Instance().SetPitchSetpoint(setPoint);
}

// Handlers for user input
void MainWindow::RollCtrlChanged(double setPoint)
{
	UserCommands::Instance().SetRollSetpoint(setPoint);
}

// Handlers for user input
void MainWindow::YawCtrlChanged(double setPoint)
{
	UserCommands::Instance().SetYawSetpoint(setPoint);
}

void MainWindow::PitchKiChanged(double ki)
{
	PitchPIDParams.fKi = ki;
	UserCommands::Instance().SetPitchKi(ki);
}

void MainWindow::PitchKpChanged(double kp)
{
	PitchPIDParams.fKp = kp;
	UserCommands::Instance().SetPitchKp(kp);
}

void MainWindow::PitchKdChanged(double kd)
{
	PitchPIDParams.fKd = kd;
	UserCommands::Instance().SetPitchKd(kd);
}

void MainWindow::YawKiChanged(double ki)
{
	YawPIDParams.fKi = ki;
	UserCommands::Instance().SetYawKi(ki);
}

void MainWindow::YawKpChanged(double kp)
{
	YawPIDParams.fKp = kp;
	UserCommands::Instance().SetYawKp(kp);
}

void MainWindow::YawKdChanged(double kd)
{
	YawPIDParams.fKd = kd;
	UserCommands::Instance().SetYawKd(kd);
}

void MainWindow::motorToggleClicked()
{
	bMotorToggle = !bMotorToggle;
	pMotorToggle->setText(bMotorToggle? "On": "Off");
	if (bMotorToggle == false) 
	{
		pSpeedWheel->setValue(0);
		pFL->setCheckState(Qt::Unchecked);
		pBL->setCheckState(Qt::Unchecked);
		pFR->setCheckState(Qt::Unchecked);
		pBR->setCheckState(Qt::Unchecked);
	//	pSpeedWheel->valueChanged(0);
	}

	else
	{
		UserCommands::Instance().SetPitchKi(PitchPIDParams.fKi);
		UserCommands::Instance().SetPitchKp(PitchPIDParams.fKp);
		UserCommands::Instance().SetPitchKd(PitchPIDParams.fKd);

		UserCommands::Instance().SetYawKi(YawPIDParams.fKi);
		UserCommands::Instance().SetYawKp(YawPIDParams.fKp);
		UserCommands::Instance().SetYawKd(YawPIDParams.fKd);
		ResetSetPoint();
		pFL->setCheckState(Qt::Checked);
		pBL->setCheckState(Qt::Checked);
		pFR->setCheckState(Qt::Checked);
		pBR->setCheckState(Qt::Checked);
	}

	
	UserCommands::Instance().ToggleMotors(bMotorToggle);
}

void MainWindow::ResetSetPoint()
{
	UserCommands::Instance().SetDefaultRollSetpoint(DefaultRollSetPoint);
	UserCommands::Instance().SetDefaultPitchSetpoint(DefaultPitchSetPoint);
	UserCommands::Instance().SetRollSetpoint(DefaultRollSetPoint);
	UserCommands::Instance().SetPitchSetpoint(DefaultPitchSetPoint);
}
void MainWindow::FRStateChanged( int state)
{
	UserCommands::Instance().SetMotorsState(FR, state);
}

void MainWindow::BRStateChanged( int state)
{
	UserCommands::Instance().SetMotorsState(BR, state);
}

void MainWindow::FLStateChanged( int state )
{
	UserCommands::Instance().SetMotorsState(FL, state);
}

void MainWindow::BLStateChanged( int state)
{
	UserCommands::Instance().SetMotorsState(BL, state);
}

void MainWindow::echoCommand(EchoCommand* cmd)
{

	char* commandName = cmd->Command;
	float val = cmd->Val;

	char tmp[50];
	if (!strcmp("Speed", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadSpeed->setText(tmp);
	}
	if (!strcmp("MotorToggle", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadPower->setText(tmp);
	}

	if (!strcmp("PKp", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadPKp->setText(tmp);
	}

	if (!strcmp("PKd", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadPKd->setText(tmp);
	}

	if (!strcmp("PKi", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadPKi->setText(tmp);
	}

	if (!strcmp("Yaw_Kp", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadYKp->setText(tmp);
	}

	if (!strcmp("Yaw_Kd", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadYKd->setText(tmp);
	}

	if (!strcmp("Yaw_Ki", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadYKi->setText(tmp);
	}
	delete cmd;
}

