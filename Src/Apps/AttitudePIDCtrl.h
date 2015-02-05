/**************************************************************************

Filename    :   AttitudePIDCtrl.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

/*
  PIDControl
 */
#ifndef __ATTITUDEPIDCONTROL__
#define __ATTITUDEPIDCONTROL__
#include "PIDControl.h"

class AttitudePIDCtrl: public PIDController
{
public:
	AttitudePIDCtrl(){};
	double Compute(double input);
	double ApplySafeCheck(double Output);
	double LastOutput;
};

#endif
