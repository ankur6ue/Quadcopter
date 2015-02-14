/**************************************************************************

Filename    :   PIDTypeMenu.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#include "qmenu.h"

class PIDTypeMenu: public QMenu
{
	Q_OBJECT
public:
	PIDTypeMenu(const QString& title, QWidget* pwidget ): QMenu(title, pwidget){}
public Q_SLOTS:
	void SetAttitudeCtrlPID();
	void SetRateCtrlPID();
};

