/**************************************************************************

Filename    :   ErrorsDef.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#include "ErrorsDef.h"

ExceptionMgr::ExceptionMgr()
{
	ExceptionFlag = 0;
}

void ExceptionMgr::SetException(Exception e)
{
	ExceptionFlag |= e;
}

void ExceptionMgr::ClearExceptionFlag()
{
	ExceptionFlag = 0;
}

long ExceptionMgr::GetException()
{
	return ExceptionFlag;
}

void ExceptionMgr::ClearFlag(Exception e)
{
	ExceptionFlag = ExceptionFlag & (~e);
}

bool ExceptionMgr::IsException()
{
	return ExceptionFlag != 0;
}

bool ExceptionMgr::IsCriticalException()
{
	return (ExceptionFlag&EXCESSIVE_PID_OUTPUT) || (ExceptionFlag&NO_BEACON_SIGNAL) || (ExceptionFlag&BAD_MPU_DATA);
}
