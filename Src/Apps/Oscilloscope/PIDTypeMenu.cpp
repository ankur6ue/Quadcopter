/**************************************************************************

Filename    :   PIDTypeMenu.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "PIDTypeMenu.h"
#include "MainWindow.h"
#include "QMenuBar.h"

void PIDTypeMenu::SetAttitudeCtrlPID()
{
	MainWindow* pmainWindow = (MainWindow*)(parentWidget());
	pmainWindow->ePIDType = AttitudePIDControl;
	pmainWindow->PIDCtrlTypeChanged();
}

void PIDTypeMenu::SetRateCtrlPID()
{
	MainWindow* pmainWindow = (MainWindow*)(parentWidget());
	pmainWindow->ePIDType = RatePIDControl;
	pmainWindow->PIDCtrlTypeChanged();
}


void MainWindow::CreateMenuItems()
{
	QMenuBar* pMenuBar = new QMenuBar(this); // No need to do any layout on the menubar
	pPIDTypeMenu = new PIDTypeMenu("&PIDType", this);
	pMenuBar->addMenu(pPIDTypeMenu);
	QAction* pattitudeCtrlAct = new QAction("AttitudeControl", pPIDTypeMenu);
	QObject::connect(pattitudeCtrlAct, SIGNAL(triggered()), pPIDTypeMenu, SLOT(SetAttitudeCtrlPID()));
	pPIDTypeMenu->addAction(pattitudeCtrlAct);
	QAction* prateCtrlAct = new QAction("RateControl", pPIDTypeMenu);
	QObject::connect(prateCtrlAct, SIGNAL(triggered()), pPIDTypeMenu, SLOT(SetRateCtrlPID()));
	pPIDTypeMenu->addAction(prateCtrlAct);
}
