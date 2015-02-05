/**************************************************************************

Filename    :   Joystick.cpp
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include <dinput.h>
#include "joystick.h"
#include "stdio.h"
IDirectInput8A * Joystick::pDi = NULL;
IDirectInputDevice8A * Joystick::pJoystick = NULL;

Joystick::Joystick(QObject* parent): QObject(parent) {};


HRESULT Joystick::Init()
{
	HRESULT hr;

	// Create a DirectInput device
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (VOID**)&pDi, NULL))) {
			return hr;
	}

	// Look for the first simple pJoystick we can find.
	if (FAILED(hr = pDi->EnumDevices(DI8DEVCLASS_GAMECTRL, &Joystick::EnumCallback,
		NULL, DIEDFL_ATTACHEDONLY))) {
			return hr;
	}

	// Make sure we got a pJoystick
	if (pJoystick == NULL) {
		printf("pJoystick not found.\n");
		return E_FAIL;
	}

	DIDEVCAPS capabilities;

	// Set the data format to "simple joystick" - a predefined data format 
	//
	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState

	if (FAILED(hr = pJoystick->SetDataFormat(&c_dfDIJoystick2))) {
		return hr;
	}

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	/*
	if (FAILED(hr = pJoystick->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE |
												  DISCL_FOREGROUND))) {
		return hr;
	}
	*/
	// Determine how many axis the pJoystick has (so we don't error out setting
	// properties for unavailable axis)
	capabilities.dwSize = sizeof(DIDEVCAPS);
	if (FAILED(hr = pJoystick->GetCapabilities(&capabilities))) {
		return hr;
	}

	// Enumerate the axes of the joyctick and set the range of each axis. Note:
	// we could just use the defaults, but we're just trying to show an example
	// of enumerating device objects (axes, buttons, etc.).
	if (FAILED(hr = pJoystick->EnumObjects(EnumAxesCallback, NULL, DIDFT_AXIS))) {
		return hr;
	}
}

BOOL CALLBACK Joystick::EnumAxesCallback(const DIDEVICEOBJECTINSTANCEA* instance, VOID* context)
{
    HWND hDlg = (HWND)context;

    DIPROPRANGE propRange; 
    propRange.diph.dwSize       = sizeof(DIPROPRANGE); 
    propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    propRange.diph.dwHow        = DIPH_BYID; 
    propRange.diph.dwObj        = instance->dwType;
    propRange.lMin              = -1000; 
    propRange.lMax              = +1000; 
    
    // Set the range for the axis
    if (FAILED(pJoystick->SetProperty(DIPROP_RANGE, &propRange.diph))) {
        return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}
	
BOOL CALLBACK Joystick::EnumCallback(LPCDIDEVICEINSTANCEA instance, VOID* context)
{
	HRESULT hr;

	// Obtain an interface to the enumerated pJoystick.
	hr = pDi->CreateDevice(instance->guidInstance, &pJoystick, NULL);

	// If it failed, then we can't use this pJoystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if (FAILED(hr)) { 
		return DIENUM_CONTINUE;
	}

	// Stop enumeration. Note: we're just taking the first pJoystick we get. You
	// could store all the enumerated pJoysticks and let the user pick.

	return DIENUM_STOP;
}

void Joystick::Run()
{
	bRunning = true;
	while(bRunning)
	{
		Sleep(100);
		Poll();
	}
}

HRESULT Joystick::Poll()
{
	HRESULT     hr;

    if (pJoystick == NULL) {
        return S_OK;
    }
    // Poll the device to read the current state
    hr = pJoystick->Poll(); 
    if (FAILED(hr)) {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = pJoystick->Acquire();
        while (hr == DIERR_INPUTLOST) {
            hr = pJoystick->Acquire();
        }

        // If we encounter a fatal error, return failure.
        if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) {
            return E_FAIL;
        }

        // If another application has control of this device, return successfully.
        // We'll just have to wait our turn to use the pJoystick.
        if (hr == DIERR_OTHERAPPHASPRIO) {
            return S_OK;
        }
    }

	DIJOYSTATE2 js;
    // Get the input's device state
    if (FAILED(hr = pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &js))) {
        return hr; // The device should have been acquired during the Poll()
    }
	int movedFlag;
	if (movedFlag = HasChanged(js))
	{
		AxisMoved(js.lX, js.lY, js.lZ, movedFlag);
	}
	
//	printf("lVx: %d, lVy: %d, lVz: %d\n", js.lVX, js.lVY, js.lVZ); 
    return S_OK;
}

int Joystick::HasChanged(DIJOYSTATE2& js)
{
	int movedFlag = 0;
/*	This should work, but doesn't. lVX is always 0 for the PS4 controller..
	if(js.lVX != 0) movedFlag |= X_AXIS;
	if(js.lVY != 0) movedFlag |= Y_AXIS;
	if(js.lVZ != 0) movedFlag |= Z_AXIS;
*/
	if(abs(js.lX - prevJoystate.lX) > 4) movedFlag |= X_AXIS;
	if(abs(js.lY - prevJoystate.lY) > 4) movedFlag |= Y_AXIS;
	if(abs(js.lZ - prevJoystate.lZ) > 4) movedFlag |= Z_AXIS;

	if(js.rgbButtons[QuadStart] != 0 && (prevJoystate.rgbButtons[QuadStart]	!= js.rgbButtons[QuadStart])) movedFlag |= START;
	if(js.rgbButtons[QuadSpeedUp] != 0 /*js.rgbButtons[QuadSpeedUp]*/) movedFlag |= SPEEDUP;
	if(js.rgbButtons[QuadSpeedDown] != 0 /*js.rgbButtons[QuadSpeedDown]*/) movedFlag |= SPEEDDOWN;

	prevJoystate.lX = js.lX;
	prevJoystate.lY = js.lY;
	prevJoystate.lZ = js.lZ;

	for (int i = 0 ; i < 128; i++)
	{
		prevJoystate.rgbButtons[i] = js.rgbButtons[i];
	}

	return movedFlag;
}