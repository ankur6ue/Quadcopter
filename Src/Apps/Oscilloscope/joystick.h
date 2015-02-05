/**************************************************************************

Filename    :   Joystick.h
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "qthread.h"
#include <dinput.h>

enum AxisMovedFlag
{
	X_AXIS		= 1,
	Y_AXIS		= 2,
	Z_AXIS		= 4,
	START		= 8,
	SPEEDUP		= 16,
	SPEEDDOWN	= 32
};

enum GamePadKeys
{
	GPApLeft		= 0,
	GPApRight		= 2,
	GPApUp			= 3,
	GPApDown		= 1,
	GPApLBUTTON	= 4,
	GPApRBUTTON	= 5
};

enum QuadToGPad
{
	QuadStart		= GPApLBUTTON,
	QuadSpeedUp		= GPApUp,
	QuadSpeedDown	= GPApDown
};

class Joystick: public QObject
{
	Q_OBJECT
public:
	Joystick(QObject* parent);
	HRESULT Init();
	static BOOL CALLBACK EnumCallback(LPCDIDEVICEINSTANCEA instance, VOID* context);
	static BOOL CALLBACK Joystick::EnumAxesCallback(const DIDEVICEOBJECTINSTANCEA* instance, VOID* context);
	HRESULT InitJoystick();
	HRESULT Poll();
	// returns true if the new joy stick position is different from the previous one
	int HasChanged(DIJOYSTATE2& js);

Q_SIGNALS:
	void AxisMoved(long, long, long, int);
	
public slots:
	void	Run();
public:
	static	LPDIRECTINPUT8A pDi;
	static	LPDIRECTINPUTDEVICE8A pJoystick;
	bool		bRunning; 
	DIJOYSTATE2 prevJoystate; // stores the previous joystick position, used in HasChanged
};