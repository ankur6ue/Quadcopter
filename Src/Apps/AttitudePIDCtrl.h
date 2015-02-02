
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
