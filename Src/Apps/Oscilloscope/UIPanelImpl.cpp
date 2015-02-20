/**************************************************************************

Filename    :   UIPanelImpl.cpp
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
#include "PIDTypeMenu.h"

void MainWindow::CreatePlots()
{
	const double intervalLength = 10.0; // seconds
	pamplitude = 150;

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
}

void MainWindow::CreatePlotControls()
{
	const double intervalLength = 10.0; // seconds
	pamplitude = 150;

	pAmplitudeKnob = new Knob( "Amplitude", 0.0, 200.0, this );
	pAmplitudeKnob->setValue( 100.0 );

	pIntervalWheel = new WheelBox( "Displayed [s]", 10.0, 100.0, 1.0, this );
	//	pIntervalWheel->setFixedSize(200, 100);
	pIntervalWheel->setValue( intervalLength );

	pTimerWheel = new WheelBox( "Sample Interval [ms]", 0.0, 50.0, 1, this );
	pTimerWheel->setValue( 20.0 );
}

void MainWindow::CreateCommonPIDControls()
{
	pCommonPIDCtrl	= new CommonPIDControls();

	pCommonPIDCtrl->pPitchKp = new WheelBox( "Kp", 0, 30, 1, this );
	pCommonPIDCtrl->pPitchKi = new WheelBox( "Ki", 0, 3, 0.05, this );
	pCommonPIDCtrl->pPitchKd = new WheelBox( "Kd", 0, 30, 1, this );
	pCommonPIDCtrl->pYawKp = new WheelBox( "Yaw_Kp", 0, 15, 0.5, this );
	pCommonPIDCtrl->pYawKi = new WheelBox( "Yaw_Ki", 0, 1, 0.01, this );
	pCommonPIDCtrl->pYawKd = new WheelBox( "Yaw_Kd", 0, 4, 0.05, this );

	SetPIDParams();
}

void MainWindow::SetPIDParams()
{
	pCommonPIDCtrl->pPitchKp->setValue( pPitchPIDParams->fKp );
	pCommonPIDCtrl->pPitchKi->setValue( pPitchPIDParams->fKi );
	pCommonPIDCtrl->pPitchKd->setValue( pPitchPIDParams->fKd );
	pCommonPIDCtrl->pYawKp->setValue( pYawPIDParams->fKp );
	pCommonPIDCtrl->pYawKi->setValue( pYawPIDParams->fKi );
	pCommonPIDCtrl->pYawKd->setValue( pYawPIDParams->fKd );
}

void MainWindow::CreateAttitudePIDControls()
{
	pAttPIDCtrl		= new AttitudePIDControls();
	pAttPIDCtrl->pCommonPIDCtrl = pCommonPIDCtrl;
}

void MainWindow::CreateRatePIDControls()
{
	pRatePIDCtrl	= new RatePIDControls();
	pRatePIDCtrl->pCommonPIDCtrl = pCommonPIDCtrl;
}

void MainWindow::CreatePIDControls()
{
	CreateCommonPIDControls();
	CreateRatePIDControls();
	CreateAttitudePIDControls();
}

void MainWindow::CreateQuadStatePanel()
{
	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Text,Qt::Key_Green);

	pPIDType = new QLineEdit(this);
	pPIDType->setAlignment(Qt::AlignLeft);
	pPIDType->setPalette(*palette);

	pQuadSpeed = new QLineEdit(this);
	pQuadSpeed->setAlignment(Qt::AlignLeft);
	pQuadSpeed->setPalette(*palette);

	pQuadPower = new QLineEdit(this);
	pQuadPower->setAlignment(Qt::AlignLeft);
	pQuadPower->setPalette(*palette);

	pQuadPKi = new QLineEdit(this);	pQuadPKi->setAlignment(Qt::AlignLeft); pQuadPKi->setPalette(*palette); 
	pQuadPKp = new QLineEdit(this);	pQuadPKp->setAlignment(Qt::AlignLeft); pQuadPKp->setPalette(*palette); 
	pQuadPKd = new QLineEdit(this);	pQuadPKd->setAlignment(Qt::AlignLeft);	pQuadPKd->setPalette(*palette);

	pQuadRKi = new QLineEdit(this);	pQuadRKi->setAlignment(Qt::AlignLeft); pQuadRKi->setPalette(*palette); 
	pQuadRKp = new QLineEdit(this);	pQuadRKp->setAlignment(Qt::AlignLeft); pQuadRKp->setPalette(*palette); 
	pQuadRKd = new QLineEdit(this);	pQuadRKd->setAlignment(Qt::AlignLeft);	pQuadRKd->setPalette(*palette);

	pQuadYKi = new QLineEdit(this);	pQuadYKi->setAlignment(Qt::AlignLeft); pQuadYKi->setPalette(*palette); 
	pQuadYKp = new QLineEdit(this);	pQuadYKp->setAlignment(Qt::AlignLeft); pQuadYKp->setPalette(*palette); 
	pQuadYKd = new QLineEdit(this);	pQuadYKd->setAlignment(Qt::AlignLeft);	pQuadYKd->setPalette(*palette);
}

void MainWindow::CreateQuadControlPanel()
{
	QPalette* palette2 = new QPalette();
	palette2->setColor(QPalette::ButtonText, Qt::Key_Green);
	// Groubox for the Pitch/Yaw Control

	pPitchSetPtWheel = new WheelBox( "Pitch Setpoint", -25, 25, 1, this );
	pRollSetPtWheel = new WheelBox( "Roll Setpoint", -25, 25, 1, this );
	pPitchSetPtWheel->setValue(DefaultPitchSetPoint);
	pRollSetPtWheel->setValue(DefaultRollSetPoint);

	pPitchCtrlWheel	= new WheelBox( "Pitch Ctrl", -PITCH_CTRL_RANGE, PITCH_CTRL_RANGE, 1, this );
	pRollCtrlWheel	= new WheelBox( "Roll Ctrl", -ROLL_CTRL_RANGE, ROLL_CTRL_RANGE, 1, this );
	pYawCtrlWheel	= new WheelBox( "Yaw Ctrl", -YAW_CTRL_RANGE, YAW_CTRL_RANGE, 1, this );

	// User inputs to the quadcopter
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
}

void MainWindow::ManageLayout()
{
	// Three main vertical layouts laid out horizontally
	QHBoxLayout *layout = new QHBoxLayout( this );
	QVBoxLayout* vLayout1 = new QVBoxLayout();
	QVBoxLayout* vLayout2 = new QVBoxLayout();
	QVBoxLayout* vLayout3 = new QVBoxLayout();
	layout->addLayout( vLayout1 );
	layout->addLayout( vLayout2 );
	layout->addLayout( vLayout3);

	// Add the plots to the first VBox
	vLayout1->addWidget( y_plot, 4);
	vLayout1->addWidget( p_plot, 4);
	vLayout1->addWidget( r_plot, 4);

	// Working on the second VBox. Three VBoxes and corresponding group boxes
	QGroupBox* gpBox2a = new QGroupBox("Plot Parameters", this);
	QVBoxLayout* vLayout2a = new QVBoxLayout();
	gpBox2a->setLayout(vLayout2a);

	QGroupBox* gpBox2b = new QGroupBox("Quadcopter PID Control", this);
	QGridLayout* vLayout2b = new QGridLayout();
	gpBox2b->setLayout(vLayout2b);

	QGroupBox* gpBox2c = new QGroupBox("Quadcopter State", this);
	QGridLayout* vLayout2c = new QGridLayout();
	gpBox2c->setLayout(vLayout2c);

	vLayout2->addWidget(gpBox2a);
	vLayout2->addWidget(gpBox2b);
	vLayout2->addWidget(gpBox2c);

	// Add Plot Control Widgets
	vLayout2a->addWidget( pIntervalWheel, 0, Qt::AlignHCenter );
	vLayout2a->addWidget( pTimerWheel, 0, Qt::AlignHCenter );
	vLayout2a->addWidget( pAmplitudeKnob, 0, Qt::AlignHCenter );

	// Quadcopter PID Control Parameters
	vLayout2b->setSpacing(0);
	vLayout2b->setMargin(0);
	
//-	AddRateCtrlPIDWidgets();
//	AddAttitudeCtrlPIDWidgets();

	vLayout2b->addWidget (pCommonPIDCtrl->pPitchKp, 0, 0, 1, 1, Qt::AlignCenter);
	vLayout2b->addWidget (pCommonPIDCtrl->pPitchKi, 0, 1, 1, 1, Qt::AlignCenter);
	vLayout2b->addWidget (pCommonPIDCtrl->pPitchKd, 0, 2, 1, 1, Qt::AlignCenter);
	vLayout2b->addWidget (pCommonPIDCtrl->pYawKp, 1, 0, 1, 1, Qt::AlignCenter);
	vLayout2b->addWidget (pCommonPIDCtrl->pYawKi, 1, 1, 1, 1, Qt::AlignCenter);
	vLayout2b->addWidget (pCommonPIDCtrl->pYawKd, 1, 2, 1, 1, Qt::AlignCenter);
	// UI elements representing Quadcopter state

	vLayout2c->setSpacing(0);
	vLayout2c->setMargin(0);
	vLayout2c->setContentsMargins(0,0,0,50);

	// Create labels here as they are not class member variables
	QLabel* pPIDTypeLabel = new QLabel("PIDType", this);
	QLabel* pQuadSpeedLabel = new QLabel("QuadSpeed", this);
	QLabel* pQuadPowerLabel = new QLabel("QuadPower", this);

	QLabel* pQuadPLabel = new QLabel("Pitch", this);
	QLabel* pQuadRLabel = new QLabel("Roll", this);
	QLabel* pQuadYLabel = new QLabel("Yaw", this);

	QLabel* pQuadKiLabel = new QLabel("Ki", this);
	QLabel* pQuadKpLabel = new QLabel("Kp", this);
	QLabel* pQuadKdLabel = new QLabel("Kd", this);

	vLayout2c->addWidget( pQuadSpeedLabel, 0, 0, 1, 2, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadSpeed, 0, 2, 1, 2, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadPowerLabel, 1, 0, 1, 2, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadPower, 1, 2, 1, 2, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadPLabel, 2, 1, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadRLabel, 2, 2, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadYLabel, 2, 3, 1, 1, Qt::AlignLeft);

	vLayout2c->addWidget( pQuadKpLabel, 3, 0, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadKiLabel, 4, 0, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadKdLabel, 5, 0, 1, 1, Qt::AlignLeft);

	vLayout2c->addWidget( pQuadPKp, 3, 1, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadRKp, 3, 2, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadYKp, 3, 3, 1, 1, Qt::AlignLeft);

	vLayout2c->addWidget( pQuadPKi, 4, 1, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadRKi, 4, 2, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadYKi, 4, 3, 1, 1, Qt::AlignLeft);

	vLayout2c->addWidget( pQuadPKd, 5, 1, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadRKd, 5, 2, 1, 1, Qt::AlignLeft);
	vLayout2c->addWidget( pQuadYKd, 5, 3, 1, 1, Qt::AlignLeft);

	vLayout2c->addWidget( pPIDTypeLabel, 6, 0, 1, 2, Qt::AlignLeft);
	vLayout2c->addWidget( pPIDType, 6, 2, 1, 2, Qt::AlignLeft);

	QGroupBox* gpBox3a = new QGroupBox("Roll/Pitch SetPoints", this);
	QGridLayout* vLayout3a = new QGridLayout();
	gpBox3a->setLayout(vLayout3a);

	QGroupBox* gpBox3b = new QGroupBox("Default Roll/Pitch Control", this);
	QGridLayout* vLayout3b = new QGridLayout();
	gpBox3b->setLayout(vLayout3b);

	QGroupBox* gpBox3c = new QGroupBox("Quadcopter Power Control", this);
	QGridLayout* vLayout3c = new QGridLayout();
	gpBox3c->setLayout(vLayout3c);

	vLayout3->addWidget(gpBox3a);
	vLayout3->addWidget(gpBox3b);
	vLayout3->addWidget(gpBox3c);

	vLayout3a->addWidget( pRollCtrlWheel, 0, 0, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pPitchCtrlWheel, 1, 0, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pYawCtrlWheel, 2, 0, 1, 1, Qt::AlignLeft);

	vLayout3b->addWidget( pRollSetPtWheel, 0, 0, 1, 1, Qt::AlignLeft);
	vLayout3b->addWidget( pPitchSetPtWheel, 1, 0, 1, 1, Qt::AlignLeft);

	vLayout3c->addWidget (pFR, 2, 0, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget (pFL, 2, 1, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget (pBR, 3, 0, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget (pBL, 3, 1, 1, 1, Qt::AlignHCenter);
	vLayout3c->addWidget( pSpeedWheel, 0, 0, 1, 2, Qt::AlignHCenter);
	vLayout3c->addWidget (pMotorToggle, 1, 0, 1, 2, Qt::AlignHCenter);

	layout->update();
}

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