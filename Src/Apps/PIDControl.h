
/*
  PIDControl
 */
#ifndef PIDCONTROL_h
#define PIDCONTROL_h

class PIDController
{
public:
	PIDController();

	unsigned long lastTime;
	double Input, Output, Setpoint;
	double Errsum, LastErr;
	double Kp, Ki, Kd;

	double Compute(double input);

	void SetTunings(double Kp, double Ki, double Kd);

	void SetSetPoint(double _setPoint);

	double GetSetPoint();

	double GetErrorSum();
};

#endif
