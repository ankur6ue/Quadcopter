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
#include "QSlider.h"
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
#include "samplingthread.h"

void MainWindow::ReadPIDParams(AttitudePIDParams& attPIDParams, RatePIDParams& ratePIDParams)
{
	FILE* fp = fopen("C:\\Embedded\\PIDParams.txt", "r");
	if (fp)
	{
		fscanf(fp, "Attitude YawPIDParams %f %f %f\n", &attPIDParams.YawPIDParams.fKp, &attPIDParams.YawPIDParams.fKi, &attPIDParams.YawPIDParams.fKd); 
		fscanf(fp, "Attitude PitchPIDParams %f %f %f\n", &attPIDParams.PitchPIDParams.fKp, &attPIDParams.PitchPIDParams.fKi, &attPIDParams.PitchPIDParams.fKd); 
		fscanf(fp, "Attitude RollPIDParams %f %f %f\n", &attPIDParams.RollPIDParams.fKp, &attPIDParams.RollPIDParams.fKi, &attPIDParams.RollPIDParams.fKd); 

		fscanf(fp, "Rate YawPIDParams %f %f %f\n", &ratePIDParams.YawPIDParams.fKp, &ratePIDParams.YawPIDParams.fKi, &ratePIDParams.YawPIDParams.fKd); 
		fscanf(fp, "Rate PitchPIDParams %f %f %f\n", &ratePIDParams.PitchPIDParams.fKp, &ratePIDParams.PitchPIDParams.fKi, &ratePIDParams.PitchPIDParams.fKd); 
		fscanf(fp, "Rate RollPIDParams %f %f %f\n", &ratePIDParams.RollPIDParams.fKp, &ratePIDParams.RollPIDParams.fKi, &ratePIDParams.RollPIDParams.fKd); 

		fscanf(fp, "Angle2Rate Params %f %f %f\n", &mA2RParams.A2R_Yaw, &mA2RParams.A2R_Pitch, &mA2RParams.A2R_Roll); 
		
		fscanf(fp, "RollHoverAttitude %d\n", &RollHoverAttitude); 
		fscanf(fp, "PitchHoverAttitude %d\n", &PitchHoverAttitude);
		char sPIDType[50];
		fscanf(fp, "PIDType %s\n", sPIDType);
		if (!strcmp(sPIDType, "Rate"))
			ePIDType = RatePIDControl;
		if (!strcmp(sPIDType, "Attitude"))
			ePIDType = AttitudePIDControl;

	}
	else
	{
		attPIDParams.YawPIDParams.fKp = 12; attPIDParams.YawPIDParams.fKi = 0; attPIDParams.YawPIDParams.fKd = 5;
		attPIDParams.PitchPIDParams.fKp = 24; attPIDParams.PitchPIDParams.fKi = 3; attPIDParams.PitchPIDParams.fKd = 10;
		attPIDParams.RollPIDParams.fKp = 24; attPIDParams.RollPIDParams.fKi = 3; attPIDParams.RollPIDParams.fKd = 10;

		ratePIDParams.YawPIDParams.fKp = 12; ratePIDParams.YawPIDParams.fKi = 0; ratePIDParams.YawPIDParams.fKd = 5;
		ratePIDParams.PitchPIDParams.fKp = 24; ratePIDParams.PitchPIDParams.fKi = 3; ratePIDParams.PitchPIDParams.fKd = 10;
		ratePIDParams.RollPIDParams.fKp = 24; ratePIDParams.RollPIDParams.fKi = 3; ratePIDParams.RollPIDParams.fKd = 10;
		PitchHoverAttitude = -4; RollHoverAttitude = -5;
		ePIDType = RatePIDControl;
	}
}

MainWindow::~MainWindow()
{
	if (pAttPIDCtrl) delete pAttPIDCtrl;
	if (pRatePIDCtrl) delete pRatePIDCtrl;
	if (pCommonPIDCtrl) delete pCommonPIDCtrl;
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
	UserCommands::Instance().SetOnInitFlag(true);
	// Initialize the Joystick 
	SetupCtrlInput();
	ReadPIDParams(mAttPIDParams, mRatePIDParams);
	CreatePlots();
	CreatePlotControls();
	CreateFlightLogControls();
	CreateMenuItems();
	CreatePIDControls();
	CreateQuadStatePanel();
	CreateQuadControlPanel();
	CreateDCMControlPanel();
	ManageLayout();
	
    connect( pAmplitudeKnob, SIGNAL( valueChanged( double ) ),
        SIGNAL( amplitudeChanged( double ) ) );

    connect( pTimerWheel, SIGNAL( valueChanged( double ) ),
        SIGNAL( signalIntervalChanged( double ) ) );

    connect( pIntervalWheel, SIGNAL( valueChanged( double ) ),
        y_plot, SLOT( setIntervalLength( double ) ) );

	connect( pQuadSpeed, SIGNAL (textChanged(const QString &)), SLOT(textChanged(const QString &)));

	connect( pSpeedWheel, SIGNAL( valueChanged( double ) ), SLOT( speedChanged( double ) ) );

	connect( pA2RPitchWheel, SIGNAL( valueChanged( double ) ), SLOT( A2RPitchChanged( double ) ) );

	connect( pA2RRollWheel, SIGNAL( valueChanged( double ) ), SLOT( A2RRollChanged( double ) ) );

	connect( pA2RYawWheel, SIGNAL( valueChanged( double ) ), SLOT( A2RYawChanged( double ) ) );

	connect( pCommonPIDCtrl->pPitchKp, SIGNAL( valueChanged( double ) ), SLOT( PitchKpChanged( double ) ) );

	connect( pCommonPIDCtrl->pPitchKi, SIGNAL( valueChanged( double ) ), SLOT( PitchKiChanged( double ) ) );

	connect( pCommonPIDCtrl->pPitchKd, SIGNAL( valueChanged( double ) ), SLOT( PitchKdChanged( double ) ) );

	connect( pCommonPIDCtrl->pYawKp, SIGNAL( valueChanged( double ) ), SLOT( YawKpChanged( double ) ) );

	connect( pCommonPIDCtrl->pYawKi, SIGNAL( valueChanged( double ) ), SLOT( YawKiChanged( double ) ) );

	connect( pCommonPIDCtrl->pYawKd, SIGNAL( valueChanged( double ) ), SLOT( YawKdChanged( double ) ) );

	connect( pPitchHoverAngleWheel, SIGNAL( valueChanged( double ) ), SLOT( PitchHoverAngleChanged( double ) ) );

	connect( pRollHoverAngleWheel, SIGNAL( valueChanged( double ) ), SLOT( RollHoverAngleChanged( double ) ) );

	connect( pPitchCtrlWheel, SIGNAL( valueChanged( double ) ), SLOT( PitchCtrlChanged( double ) ) );

	connect( pRollCtrlWheel, SIGNAL( valueChanged( double ) ), SLOT( RollCtrlChanged( double ) ) );

	connect( pYawCtrlWheel, SIGNAL( valueChanged( double ) ), SLOT( YawCtrlChanged( double ) ) );

	connect( pFR, SIGNAL( stateChanged( int ) ), SLOT( FRStateChanged( int ) ) );

	connect( pBR, SIGNAL( stateChanged( int ) ), SLOT( BRStateChanged( int ) ) );

	connect( pFL, SIGNAL( stateChanged( int ) ), SLOT( FLStateChanged( int ) ) );

	connect( pBL, SIGNAL( stateChanged( int ) ), SLOT( BLStateChanged( int ) ) );

	connect( pMotorToggle, SIGNAL( clicked() ), SLOT( motorToggleClicked() ) );

	connect(pRecordingToggle, SIGNAL(clicked()), SLOT(recordToggleClicked()));

	connect(pAlphaSlider, SIGNAL(valueChanged(int)), SLOT(DCMAlphaSliderMoved(int)));

}

void MainWindow::SetSamplingThread(SamplingThread* pthread)
{
	pSamplingThread = pthread;
	connect(pRecordingToggle, SIGNAL(clicked()), pSamplingThread, SLOT(recordToggleClicked()));
	connect(this, SIGNAL(logFileSelected(QString)), pSamplingThread, SLOT(onLogFileSelected(QString)));
	connect(pPlayToggle, SIGNAL(clicked()), pSamplingThread, SLOT(onPlayToggled()));
	// Once flight logs have finished playing, disable the play button
	connect(pSamplingThread, SIGNAL(logPlaybackOver()), this, SLOT(onLogPlaybackOver()));
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

void MainWindow::DCMAlphaSliderMoved(int sliderVal)
{
	double newVal = sliderVal / 10.0;
	UserCommands::Instance().SetDCMAlpha(newVal);
}

void MainWindow::PIDCtrlTypeChanged()
{
	UserCommands::Instance().SetPIDType(ePIDType);
	if (ePIDType == AttitudePIDControl) 
	{
		pPitchPIDParams = &mAttPIDParams.PitchPIDParams;
		pYawPIDParams	= &mAttPIDParams.YawPIDParams;
	}
	if (ePIDType == RatePIDControl) 
	{
		pPitchPIDParams = &mRatePIDParams.PitchPIDParams;
		pYawPIDParams	= &mRatePIDParams.YawPIDParams;
	}
	SetPIDParams();
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

void MainWindow::A2RPitchChanged(double a2rPitch)
{
	UserCommands::Instance().SetA2RPitch(a2rPitch);
	mA2RParams.A2R_Pitch = a2rPitch;
}

void MainWindow::A2RRollChanged(double a2rRoll)
{
	UserCommands::Instance().SetA2RRoll(a2rRoll);
	mA2RParams.A2R_Roll = a2rRoll;
}

void MainWindow::A2RYawChanged(double a2rYaw)
{
	UserCommands::Instance().SetA2RYaw(a2rYaw);
	mA2RParams.A2R_Yaw = a2rYaw;
}

// Handlers for user input
void MainWindow::PitchHoverAngleChanged(double setPoint)
{
	PitchHoverAttitude = setPoint;
//	UserCommands::Instance().SetPitchDisplacement(setPoint);
}

// Handlers for user input
void MainWindow::RollHoverAngleChanged(double setPoint)
{
	RollHoverAttitude = setPoint;
//	UserCommands::Instance().SetRollDisplacement(setPoint);
}

// Handlers for user input
void MainWindow::PitchCtrlChanged(double setPoint)
{
	UserCommands::Instance().SetPitchDisplacement(setPoint);
}

// Handlers for user input
void MainWindow::RollCtrlChanged(double setPoint)
{
	UserCommands::Instance().SetRollDisplacement(setPoint);
}

// Handlers for user input
void MainWindow::YawCtrlChanged(double setPoint)
{
	UserCommands::Instance().SetYawDisplacement(setPoint);
}

void MainWindow::PitchKiChanged(double ki)
{
	pPitchPIDParams->fKi = ki;
	UserCommands::Instance().SetPitchKi(ki);
}

void MainWindow::PitchKpChanged(double kp)
{
	pPitchPIDParams->fKp = kp;
	UserCommands::Instance().SetPitchKp(kp);
}

void MainWindow::PitchKdChanged(double kd)
{
	pPitchPIDParams->fKd = kd;
	UserCommands::Instance().SetPitchKd(kd);
}

void MainWindow::YawKiChanged(double ki)
{
	pYawPIDParams->fKi = ki;
	UserCommands::Instance().SetYawKi(ki);
}

void MainWindow::YawKpChanged(double kp)
{
	pYawPIDParams->fKp = kp;
	UserCommands::Instance().SetYawKp(kp);
}

void MainWindow::YawKdChanged(double kd)
{
	pYawPIDParams->fKd = kd;
	UserCommands::Instance().SetYawKd(kd);
}

void MainWindow::MotorsOn()
{
	UserCommands::Instance().SetPitchKi(pPitchPIDParams->fKi);
	UserCommands::Instance().SetPitchKp(pPitchPIDParams->fKp);
	UserCommands::Instance().SetPitchKd(pPitchPIDParams->fKd);

	UserCommands::Instance().SetYawKi(pYawPIDParams->fKi);
	UserCommands::Instance().SetYawKp(pYawPIDParams->fKp);
	UserCommands::Instance().SetYawKd(pYawPIDParams->fKd);
	UserCommands::Instance().SetPIDType(ePIDType);

	if (ePIDType == RatePIDControl)
	{
		UserCommands::Instance().SetA2RPitch(mA2RParams.A2R_Pitch);
		UserCommands::Instance().SetA2RRoll(mA2RParams.A2R_Roll);
		UserCommands::Instance().SetA2RYaw(mA2RParams.A2R_Yaw);
	}
	UserCommands::Instance().SetSendBeaconFlag();
	ResetSetPoint();
	pFL->setCheckState(Qt::Checked);
	pBL->setCheckState(Qt::Checked);
	pFR->setCheckState(Qt::Checked);
	pBR->setCheckState(Qt::Checked);
	UserCommands::Instance().ToggleMotors(true);
}

void MainWindow::MotorsOff()
{
	pSpeedWheel->setValue(0);
	pFL->setCheckState(Qt::Unchecked);
	pBL->setCheckState(Qt::Unchecked);
	pFR->setCheckState(Qt::Unchecked);
	pBR->setCheckState(Qt::Unchecked);
	pExceptionType->setText("");
	UserCommands::Instance().ClearSendBeaconFlag();
	UserCommands::Instance().ToggleMotors(false);
	//	pSpeedWheel->valueChanged(0);
}
void MainWindow::motorToggleClicked()
{
	bMotorToggle = !bMotorToggle;
	pMotorToggle->setText(bMotorToggle? "On": "Off");
	if (bMotorToggle == false)
	{
		MotorsOff();
	}
	else
	{
		MotorsOn();
	}
}

void MainWindow::recordToggleClicked()
{
	bRecordToggle = !bRecordToggle;
	pRecordingToggle->setText(bRecordToggle ? "Pause" : "Record");
}

void MainWindow::ResetSetPoint()
{
	UserCommands::Instance().SetRollHoverAttitude(RollHoverAttitude);
	UserCommands::Instance().SetPitchHoverAttitude(PitchHoverAttitude);
	UserCommands::Instance().SetRollDisplacement(RollHoverAttitude);
	UserCommands::Instance().SetPitchDisplacement(PitchHoverAttitude);
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

	char* commandName	= cmd->Command;
	float val			= cmd->Val;
	char* commandArgs	= cmd->CommandArgs;
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

	if (!strcmp("A2R_PKp", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadA2RPKp->setText(tmp);
	}

	if (!strcmp("A2R_RKp", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadA2RRKp->setText(tmp);
	}

	if (!strcmp("A2R_YKp", commandName))
	{
		sprintf_s(tmp, 50, "%f", val);
		pQuadA2RYKp->setText(tmp);
	}

	if (!strcmp("PIDType", commandName))
	{
		pPIDType->setText(val == 0? "Attitude": "Rate");
	}

	if (!strcmp("Exception:", commandName))
	{
		pExceptionType->setText(commandArgs);
		MessageBeep(0xFFFFFFFF);

	}
	delete cmd;
}

