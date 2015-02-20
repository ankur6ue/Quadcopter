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
#ifndef __PIDTYPEMENU__
#define __PIDTYPEMENU__

#include "qmenu.h"

enum PIDType
{
	AttitudePIDControl = 0,
	RatePIDControl
};

class PIDTypeMenu: public QMenu
{
	Q_OBJECT
public:
	PIDTypeMenu(const QString& title, QWidget* pwidget ): QMenu(title, pwidget){}
public Q_SLOTS:
	void OnCtrlPIDChanged(QAction*);
};
#endif