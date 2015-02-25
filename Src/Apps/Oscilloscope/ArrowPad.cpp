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
	PitchDisplacement	= 0;
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
	PitchDisplacement += 5;
	UserCommands::Instance().SetPitchDisplacement(PitchDisplacement);
}

void ArrowPadDef::ButtonDownClicked()
{
	PitchDisplacement -= 5;
	UserCommands::Instance().SetPitchDisplacement(PitchDisplacement);
}

void ArrowPadDef::ButtonLeftClicked()
{
	RollSetPoint += 5;
	UserCommands::Instance().SetRollDisplacement(RollSetPoint);
}

void ArrowPadDef::ButtonRightClicked()
{
	RollSetPoint -= 5;
	UserCommands::Instance().SetRollDisplacement(RollSetPoint);
}

void ArrowPadDef::ResetSetPoint()
{
	UserCommands::Instance().SetRollHoverAttitude(pMainWindow->RollHoverAttitude);
	UserCommands::Instance().SetPitchHoverAttitude(pMainWindow->PitchHoverAttitude);
	RollSetPoint = pMainWindow->RollHoverAttitude;
	PitchDisplacement = pMainWindow->PitchHoverAttitude;
	UserCommands::Instance().SetRollDisplacement(RollSetPoint);
	UserCommands::Instance().SetPitchDisplacement(PitchDisplacement);

}

