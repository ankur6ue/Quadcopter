
/*
  PIDControl
 */
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
