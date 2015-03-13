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

void PIDTypeMenu::OnCtrlPIDChanged(QAction* pactiveAction)
{
	MainWindow* pmainWindow = (MainWindow*)(parentWidget());
	QString actionName = pactiveAction->objectName();
	if (actionName == "Attitude")
		pmainWindow->ePIDType = AttitudePIDControl;
	if (actionName == "Rate")
		pmainWindow->ePIDType = RatePIDControl;

	QList<QAction*> actionsList = this->actions();
	for (int i = 0; i < actionsList.length(); i++)
	{
		if (actionsList[i] != pactiveAction)
		{
			actionsList[i]->setChecked(false);
		}
	}
	pmainWindow->PIDCtrlTypeChanged();
}

void MainWindow::CreateMenuItems()
{
	QMenuBar* pMenuBar = new QMenuBar(this); // No need to do any layout on the menubar
	pPIDTypeMenu = new PIDTypeMenu("&PIDType", this);
	pMenuBar->addMenu(pPIDTypeMenu);
	QAction* pattitudeCtrlAct = new QAction("AttitudeControl", pPIDTypeMenu);
	pattitudeCtrlAct->setObjectName("Attitude");
	pattitudeCtrlAct->setCheckable(true);
	QObject::connect(pPIDTypeMenu, SIGNAL(triggered(QAction*)), pPIDTypeMenu, SLOT(OnCtrlPIDChanged(QAction*)));
	pPIDTypeMenu->addAction(pattitudeCtrlAct);
	QAction* prateCtrlAct = new QAction("RateControl", pPIDTypeMenu);
	prateCtrlAct->setObjectName("Rate");
	prateCtrlAct->setCheckable(true);
	pPIDTypeMenu->addAction(prateCtrlAct);
	
	if (ePIDType == AttitudePIDControl) 
	{
		pPitchPIDParams = &mAttPIDParams.PitchPIDParams;
		pYawPIDParams	= &mAttPIDParams.YawPIDParams;
		pPIDTypeMenu->setActiveAction(pattitudeCtrlAct);
		pattitudeCtrlAct->setChecked(true);
	}
	if (ePIDType == RatePIDControl) 
	{
		pPitchPIDParams = &mRatePIDParams.PitchPIDParams;
		pYawPIDParams	= &mRatePIDParams.YawPIDParams;
		pPIDTypeMenu->setActiveAction(prateCtrlAct);
		prateCtrlAct->setChecked(true);
	}
}
