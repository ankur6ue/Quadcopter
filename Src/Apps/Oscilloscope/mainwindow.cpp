/**************************************************************************

Filename    :   MainWindow.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

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
	// Initialize the Joystick
	SetupCtrlInput();
	ReadPIDParams(YawPIDParams, PitchPIDParams, RollPIDParams);
	CreatePlots();
	CreatePlotControls();
	CreatePIDControls();
	CreateQuadStatePanel();
	CreateQuadControlPanel();
	ManageLayout();

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
		UserCommands::Instance().ClearSendBeaconFlag();
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
		
		//UserCommands::Instance().SetSendBeaconFlag();
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

