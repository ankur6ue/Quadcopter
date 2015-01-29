
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
	double 	Input, Output, Setpoint;
	double 	Errsum, LastErr;
	double 	Kp, Ki, Kd;
	double 	TargetSetpoint;
	double 	StepSize;
	int 	QuadSpeed;

	double Compute(double input);

	void SetTunings(double Kp, double Ki, double Kd);

	void SetSetPoint(double _setPoint);

	void SetNewSetpoint(double _setPoint);

	void SetSpeed(int);

	double GetSetPoint();

	void SetErrorSum(double val);

	void SetLastError(double val);

	double GetErrorSum();
};

#endif
