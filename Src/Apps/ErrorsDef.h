/**************************************************************************

Filename    :   ErrorsDef.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef __ERRORSDEF__
#define __ERRORSDEF__

enum Exception
{
	BUFFER_OVERFLOW 		= 1,
	EXCESSIVE_PID_OUTPUT 	= 2, 	// If the PID output exceeds a threshold, enter shutddown mode to prevent damage to ESC
	NO_BEACON_SIGNAL		= 4, 	// We send pings every 1 seconds. If pings are not received for 2 seconds, enter shutdown mode
	BAD_MPU_DATA			= 8
};

class ExceptionMgr
{
public:
	ExceptionMgr();
	void SetException(Exception e);
	long GetException();
	void ClearExceptionFlag();
	void ClearFlag(Exception e);
	bool IsException();
	bool IsCriticalException();
	long ExceptionFlag;
};

#endif
