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
#include "PIDTypeMenu.h"
#include <QFileSystemModel>
#include <QListView>
#include <MySortFilterProxyModel.h>
#include "qtoolbutton.h"
#include "QSizePolicy.h"

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

	pTimerWheel = new WheelBox( "Sample Interval [ms]", 0.0, 80.0, 1, this );
	pTimerWheel->setValue( 40.0 );
}

void MainWindow::CreateFlightLogControls()
{
	pRecordingToggle = new QToolButton(this);
	pPlayToggle = new QToolButton(this);
	pPlayToggle->setIcon(QIcon(":/images/Images/play.jpg"));
	pPlayToggle->setIconSize(QSize(20, 20));
	pPlayToggle->setEnabled(false);
	pRecordingToggle->setIcon(QIcon(":/images/Images/record.jpg"));
	pRecordingToggle->setIconSize(QSize(20, 20));
//	pRecordingToggle->setStyleSheet("background-image: url(:images/record.jpg);");
	pfileModel = new QFileSystemModel(this);
	pfileModel->setRootPath("C:\\Embedded\\Logs\\");
	pLogFilelist = new QListView(this);
	MySortFilterProxyModel *proxyModel = new MySortFilterProxyModel(this);
//	proxyModel->setSortRole(Qt::DateFormat);
	proxyModel->setSourceModel(pfileModel);
	pLogFilelist->setModel(proxyModel);
	pLogFilelist->setRootIndex(proxyModel->mapFromSource(pfileModel->index("C:\\Embedded\\Logs\\")));
	pLogFilelist->setMaximumHeight(100);
	pLogFilelist->show();
	proxyModel->invalidate();
	proxyModel->setSortRole(Qt::UserRole);
	proxyModel->sort(0);
	connect(pLogFilelist, SIGNAL(clicked(QModelIndex)), this, SLOT(onLogFileSelected(const QModelIndex&)));
	connect(pPlayToggle, SIGNAL(clicked()), this, SLOT(onPlayToggled()));
	bRecordToggle = false;
}

void MainWindow::SetPlotState(bool bIsPlaying)
{
	if (bIsPlaying)
	{
		r_plot->bUpdate = true;
		y_plot->bUpdate = true;
		p_plot->bUpdate = true;
	}
	else
	{
		r_plot->bUpdate = false;
		y_plot->bUpdate = false;
		p_plot->bUpdate = false;
	}
}

void MainWindow::onPlayToggled()
{
	bIsPlaying = !bIsPlaying;
	pPlayToggle->setIcon(bIsPlaying ? QIcon(":/images/Images/pause.png") : QIcon(":/images/Images/play.jpg"));
	SetPlotState(bIsPlaying);
}

// Disable play button once log playback is over.
void MainWindow::onLogPlaybackOver()
{
	pPlayToggle->setIcon(QIcon(":/images/Images/play.jpg"));
	pPlayToggle->setEnabled(false);

}

void MainWindow::onLogFileSelected(const QModelIndex& index)
{
	QString fileName = pfileModel->fileName(index);
	// Activate play/stop button
	pPlayToggle->setEnabled(true);
	bIsPlaying = true;
	SetPlotState(bIsPlaying);
	pPlayToggle->setIcon(QIcon(":/images/Images/pause.png"));
	// OnLogFileSelected is not a slot in SamplingThread otherwise FileModel will need to also be declared in SamplingThread, potentially creating
	// threading issues. Haven't thought through this in detail though
	emit(logFileSelected(fileName));
}

void MainWindow::CreateAngle2RateControls()
{
	pA2RPitchWheel	= new WheelBox( "A2R_Pitch", 0, 5, 0.2, this );
	pA2RPitchWheel->setValue(mA2RParams.A2R_Pitch);
	pA2RRollWheel	= new WheelBox( "A2R_Roll", 0, 5, 0.2, this );
	pA2RRollWheel->setValue(mA2RParams.A2R_Roll);
	pA2RYawWheel	= new WheelBox( "A2R_Yaw", 0, 5, 0.2, this );
	pA2RYawWheel->setValue(mA2RParams.A2R_Yaw);
}

void MainWindow::CreateCommonPIDControls()
{
	pCommonPIDCtrl	= new CommonPIDControls();

	pCommonPIDCtrl->pPitchKp = new WheelBox( "Kp", 0.5, 6, 0.5, this );
	pCommonPIDCtrl->pPitchKi = new WheelBox( "Ki", 0, 10, 0.5, this );
	pCommonPIDCtrl->pPitchKd = new WheelBox( "Kd", 0, 1, 0.1, this );
	pCommonPIDCtrl->pYawKp = new WheelBox( "Yaw_Kp", 0, 15, 0.5, this );
	pCommonPIDCtrl->pYawKi = new WheelBox( "Yaw_Ki", 0, 2, 0.1, this );
	pCommonPIDCtrl->pYawKd = new WheelBox( "Yaw_Kd", 0, 4, 0.5, this );

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

void MainWindow::CreateAltitudePIDControls()
{
	pAltPIDCtrl = new AltitudePIDControls();
	pAltPIDCtrl->pAltKp = new WheelBox("Alt_Kp", 0, 2, 0.25, this);
	pAltPIDCtrl->pAltKi = new WheelBox("Alt_Ki", 0, 2, 0.1, this);
	pAltPIDCtrl->pAltKd = new WheelBox("Alt_Kd", 0, 4, 0.5, this);
	
	pAltPIDCtrl->pAltKp->setValue(mAltPIDParams.fKp);
	pAltPIDCtrl->pAltKi->setValue(mAltPIDParams.fKi);
	pAltPIDCtrl->pAltKd->setValue(mAltPIDParams.fKd);
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
	CreateAngle2RateControls();
	CreateCommonPIDControls();
	CreateRatePIDControls();
	CreateAttitudePIDControls();
	CreateAltitudePIDControls();
}

void MainWindow::CreateQuadStatePanel()
{
	QPalette *palette = new QPalette();
	palette->setColor(QPalette::Text,Qt::Key_Green);

	pPIDType = new QLineEdit(this);
	pPIDType->setAlignment(Qt::AlignLeft);
	pPIDType->setPalette(*palette);

	pExceptionType = new QLineEdit(this);
	pExceptionType->setAlignment(Qt::AlignLeft);
	pExceptionType->setPalette(*palette);

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
	// Angle to Rate 
	pQuadA2RPKp = new QLineEdit(this);	pQuadA2RPKp->setAlignment(Qt::AlignLeft);	pQuadA2RPKp->setPalette(*palette);
	pQuadA2RRKp = new QLineEdit(this);	pQuadA2RRKp->setAlignment(Qt::AlignLeft);	pQuadA2RRKp->setPalette(*palette);
	pQuadA2RYKp = new QLineEdit(this);	pQuadA2RYKp->setAlignment(Qt::AlignLeft);	pQuadA2RYKp->setPalette(*palette);

}

void MainWindow::CreateDCMControlPanel()
{
	pAlphaSlider = new QSlider(this);
	pAlphaSlider->setRange(0, 10);
	pAlphaSlider->setSingleStep(1);
	pAlphaSlider->setTickInterval(1);
	pAlphaSlider->setOrientation(Qt::Horizontal);
}

void MainWindow::CreateQuadControlPanel()
{
	QPalette* palette2 = new QPalette();
	palette2->setColor(QPalette::ButtonText, Qt::Key_Green);
	// Groubox for the Pitch/Yaw Control

	pPitchHoverAngleWheel = new WheelBox( "Pitch Hover Angle", -8, 8, 0.2, this );
	pRollHoverAngleWheel = new WheelBox( "Roll Hover Angle", -7, 7, 0.2, this );
	pPitchHoverAngleWheel->setValue(PitchHoverAttitude);
	pRollHoverAngleWheel->setValue(RollHoverAttitude);

	pPitchCtrlWheel	= new WheelBox( "Pitch Ctrl", -PITCH_CTRL_RANGE, PITCH_CTRL_RANGE, 1, this );
	pRollCtrlWheel	= new WheelBox( "Roll Ctrl", -ROLL_CTRL_RANGE, ROLL_CTRL_RANGE, 1, this );
	pYawCtrlWheel	= new WheelBox( "Yaw Ctrl", -YAW_CTRL_RANGE, YAW_CTRL_RANGE, 1, this );

	// User inputs to the quadcopter
	pSpeedWheel = new WheelBox( "Speed", 0.0, 1500.0, 2, this );
	pSpeedWheel->setContentsMargins(0,0,0,0);
	pSpeedWheel->setValue( 0.0 );

	pHoverWheel = new WheelBox("Hover Altitude", 0.0, 300.0, 2, this);
	pHoverWheel->setContentsMargins(0, 0, 0, 0);
	pHoverWheel->setValue(0.0);

	pMotorToggle = new QPushButton("Off", this);
	pMotorToggle->setCheckable(true);
	pMotorToggle->setPalette(*palette2);
//	pMotorToggle->setFixedSize(100, 50);
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

	QGroupBox* gpBox2a = new QGroupBox("Quadcopter Power Control", this);
	QGridLayout* vLayout2a = new QGridLayout();
	gpBox2a->setLayout(vLayout2a);

	QGroupBox* gpBox2b = new QGroupBox("Angle to Rate Parameters", this);
	QGridLayout* vLayout2b = new QGridLayout();
	gpBox2b->setLayout(vLayout2b);

	QGroupBox* gpBox2c = new QGroupBox("Quadcopter PID Control", this);
	QGridLayout* vLayout2c = new QGridLayout();
	gpBox2c->setLayout(vLayout2c);

	QGroupBox* gpBox2d = new QGroupBox("Quadcopter State", this);
	QGridLayout* vLayout2d = new QGridLayout();
	gpBox2d->setLayout(vLayout2d);

	vLayout2->addWidget(gpBox2a);
	vLayout2->addWidget(gpBox2b);
	vLayout2->addWidget(gpBox2c);
	vLayout2->addWidget(gpBox2d);

	vLayout2a->addWidget(pFR, 2, 0, 1, 1, Qt::AlignHCenter);
	vLayout2a->addWidget(pFL, 2, 1, 1, 1, Qt::AlignHCenter);
	vLayout2a->addWidget(pBR, 3, 0, 1, 1, Qt::AlignHCenter);
	vLayout2a->addWidget(pBL, 3, 1, 1, 1, Qt::AlignHCenter);
	vLayout2a->addWidget(pSpeedWheel, 0, 0, 1, 1, Qt::AlignHCenter);
	vLayout2a->addWidget(pHoverWheel, 0, 1, 1, 1, Qt::AlignHCenter);
	vLayout2a->addWidget(pMotorToggle, 1, 0, 1, 2, Qt::AlignHCenter);

	// Angle to Roll controls
	vLayout2b->setSpacing(0);
	vLayout2b->setMargin(0);

	vLayout2b->addWidget (pA2RPitchWheel, 0, 0, 1, 1, Qt::AlignCenter);
	vLayout2b->addWidget (pA2RRollWheel, 0, 1, 1, 1, Qt::AlignCenter);
	vLayout2b->addWidget (pA2RYawWheel, 0, 2, 1, 1, Qt::AlignCenter);

	// Quadcopter PID Control Parameters
	vLayout2c->setSpacing(0);
	vLayout2c->setMargin(0);
	
//-	AddRateCtrlPIDWidgets();
//	AddAttitudeCtrlPIDWidgets();

	vLayout2c->addWidget (pCommonPIDCtrl->pPitchKp, 0, 0, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget (pCommonPIDCtrl->pPitchKi, 0, 1, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget (pCommonPIDCtrl->pPitchKd, 0, 2, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget (pCommonPIDCtrl->pYawKp, 1, 0, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget (pCommonPIDCtrl->pYawKi, 1, 1, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget (pCommonPIDCtrl->pYawKd, 1, 2, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget(pAltPIDCtrl->pAltKp, 2, 0, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget(pAltPIDCtrl->pAltKi, 2, 1, 1, 1, Qt::AlignCenter);
	vLayout2c->addWidget(pAltPIDCtrl->pAltKd, 2, 2, 1, 1, Qt::AlignCenter);

	// UI elements representing Quadcopter state

	vLayout2d->setSpacing(0);
	vLayout2d->setMargin(0);
//	vLayout2d->setContentsMargins(0,0,0,50);

	// Create labels here as they are not class member variables
	QLabel* pPIDTypeLabel = new QLabel("PIDType", this);
	QLabel* pExceptionTypeLabel = new QLabel("ExceptionType", this);
	QLabel* pQuadSpeedLabel = new QLabel("QuadSpeed", this);
	QLabel* pQuadPowerLabel = new QLabel("QuadPower", this);

	QLabel* pQuadPLabel = new QLabel("Pitch", this);
	QLabel* pQuadRLabel = new QLabel("Roll", this);
	QLabel* pQuadYLabel = new QLabel("Yaw", this);

	QLabel* pQuadKiLabel = new QLabel("Ki", this);
	QLabel* pQuadKpLabel = new QLabel("Kp", this);
	QLabel* pQuadKdLabel = new QLabel("Kd", this);

	QLabel* pQuadA2RLabel = new QLabel("A2R", this);

	QLabel* pDCMAlphaLabel = new QLabel("Alpha (*10)", this);

	vLayout2d->addWidget( pQuadSpeedLabel, 0, 0, 1, 2, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadSpeed, 0, 2, 1, 2, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadPowerLabel, 1, 0, 1, 2, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadPower, 1, 2, 1, 2, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadPLabel, 2, 1, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadRLabel, 2, 2, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadYLabel, 2, 3, 1, 1, Qt::AlignLeft);

	vLayout2d->addWidget( pQuadKpLabel, 3, 0, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadKiLabel, 4, 0, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadKdLabel, 5, 0, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadA2RLabel, 6, 0, 1, 1, Qt::AlignLeft);

	vLayout2d->addWidget( pQuadPKp, 3, 1, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadRKp, 3, 2, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadYKp, 3, 3, 1, 1, Qt::AlignLeft);

	vLayout2d->addWidget( pQuadPKi, 4, 1, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadRKi, 4, 2, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadYKi, 4, 3, 1, 1, Qt::AlignLeft);

	vLayout2d->addWidget( pQuadPKd, 5, 1, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadRKd, 5, 2, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadYKd, 5, 3, 1, 1, Qt::AlignLeft);

	vLayout2d->addWidget( pQuadA2RPKp, 6, 1, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadA2RRKp, 6, 2, 1, 1, Qt::AlignLeft);
	vLayout2d->addWidget( pQuadA2RYKp, 6, 3, 1, 1, Qt::AlignLeft);

	vLayout2d->addWidget( pPIDTypeLabel, 7, 0, 1, 2, Qt::AlignLeft);
	vLayout2d->addWidget( pPIDType, 7, 2, 1, 2, Qt::AlignLeft);

	vLayout2d->addWidget( pExceptionTypeLabel, 8, 0, 1, 2, Qt::AlignLeft);
	vLayout2d->addWidget( pExceptionType, 8, 2, 1, 2, Qt::AlignLeft);

	// VBox3
	QGroupBox* gpBox3a = new QGroupBox("Roll/Pitch Displacements", this);
	QGridLayout* vLayout3a = new QGridLayout();
	gpBox3a->setLayout(vLayout3a);

	QGroupBox* gpBox3b = new QGroupBox("Roll/Pitch Hover Attitude", this);
	QGridLayout* vLayout3b = new QGridLayout();
	gpBox3b->setLayout(vLayout3b);

	QGroupBox* gpBox3d = new QGroupBox("Flight Data Recording", this);
	QGridLayout* vLayout3d = new QGridLayout();
	gpBox3d->setLayout(vLayout3d);

	QGroupBox* gpBox3e = new QGroupBox("DCM Parameters", this);
	QGridLayout* vLayout3e = new QGridLayout();
	gpBox3e->setLayout(vLayout3e);

	QGroupBox* gpBox3f = new QGroupBox("Plot Parameters", this);
	QVBoxLayout* vLayout3f = new QVBoxLayout();
	gpBox3f->setLayout(vLayout3f);
	
	vLayout3->addWidget(gpBox3f);
	vLayout3->addWidget(gpBox3a);
	vLayout3->addWidget(gpBox3b);
	vLayout3->addWidget(gpBox3e);
	vLayout3->addWidget(gpBox3d);

	// Add Plot Control Widgets
	vLayout3f->addWidget(pIntervalWheel, 0, Qt::AlignHCenter);
	vLayout3f->addWidget(pTimerWheel, 0, Qt::AlignHCenter);
	vLayout3f->addWidget(pAmplitudeKnob, 0, Qt::AlignHCenter);

	// Add Roll/Pitch Control Widgets
	vLayout3a->addWidget( pRollCtrlWheel, 0, 0, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pPitchCtrlWheel, 0, 1, 1, 1, Qt::AlignLeft);
	vLayout3a->addWidget( pYawCtrlWheel, 0, 2, 1, 1, Qt::AlignLeft);

	vLayout3b->addWidget( pRollHoverAngleWheel, 0, 0, 1, 1, Qt::AlignLeft);
	vLayout3b->addWidget( pPitchHoverAngleWheel, 0, 1, 1, 1, Qt::AlignLeft);

	vLayout3d->setSpacing(0);
	vLayout3d->setMargin(0);
	vLayout3d->addWidget(pRecordingToggle, 0, 0, 1, 1, Qt::AlignHCenter);
	vLayout3d->addWidget(pPlayToggle, 0, 1, 1, 1, Qt::AlignHCenter);
	vLayout3d->addWidget(pLogFilelist, 1, 0, 1, 2, Qt::AlignHCenter);

	vLayout3e->addWidget(pDCMAlphaLabel, 0, 0, 1, 1, Qt::AlignHCenter);
	vLayout3e->addWidget(pAlphaSlider, 0, 1, 1, 1, Qt::AlignHCenter);
	// TODO: the wdith of the button should be a percentage of the container
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