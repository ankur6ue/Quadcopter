/**************************************************************************

Filename    :   RatePIDCtrl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#ifndef __RATEPIDCONTROL__
#define __RATEPIDCONTROL__
#include "PIDControl.h"

class RatePIDCtrl: public PIDController
{
public:
	RatePIDCtrl(){};
	double Compute(double input);
};

#endif
