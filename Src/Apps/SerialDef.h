/**************************************************************************

Filename    :   SerialDef.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef _SERIAL_DEF_
#define _SERIAL_DEF_

extern SoftwareSerial SSerial;
#define USE_HARDWARE_SERIAL

#ifdef USE_HARDWARE_SERIAL
#define SERIAL Serial1
#else
#define SERIAL SSerial
#endif

#endif
