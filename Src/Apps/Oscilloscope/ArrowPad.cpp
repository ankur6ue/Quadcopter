/**************************************************************************

Filename    :   ArrowPad.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "mainwindow.h"
#include "qpushbutton.h"
#include "commanddef.h"

ArrowPadDef::ArrowPadDef(MainWindow* pmainWindow) : QWidget(pmainWindow)
{
	pMainWindow		= pmainWindow;
	PitchSetPoint	= 0;
	RollSetPoint	= 0;
};

void ArrowPadDef::ConnectSignals()
{
	connect( pButtonUp, SIGNAL( clicked() ), SLOT( ButtonUpClicked() ) );
	connect( pButtonDown, SIGNAL( clicked() ), SLOT( ButtonDownClicked() ) );
	connect( pButtonLeft, SIGNAL( clicked() ), SLOT( ButtonLeftClicked() ) );
	connect( pButtonRight, SIGNAL( clicked() ), SLOT( ButtonRightClicked() ) );
}

void ArrowPadDef::ButtonUpClicked()
{
	PitchSetPoint += 5;
	UserCommands::Instance().SetPitchSetpoint(PitchSetPoint);
}

void ArrowPadDef::ButtonDownClicked()
{
	PitchSetPoint -= 5;
	UserCommands::Instance().SetPitchSetpoint(PitchSetPoint);
}

void ArrowPadDef::ButtonLeftClicked()
{
	RollSetPoint += 5;
	UserCommands::Instance().SetRollSetpoint(RollSetPoint);
}

void ArrowPadDef::ButtonRightClicked()
{
	RollSetPoint -= 5;
	UserCommands::Instance().SetRollSetpoint(RollSetPoint);
}

void ArrowPadDef::ResetSetPoint()
{
	UserCommands::Instance().SetDefaultRollSetpoint(pMainWindow->DefaultRollSetPoint);
	UserCommands::Instance().SetDefaultPitchSetpoint(pMainWindow->DefaultPitchSetPoint);
	RollSetPoint = pMainWindow->DefaultRollSetPoint;
	PitchSetPoint = pMainWindow->DefaultPitchSetPoint;
	UserCommands::Instance().SetRollSetpoint(RollSetPoint);
	UserCommands::Instance().SetPitchSetpoint(PitchSetPoint);

}

