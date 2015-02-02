#include "AttitudePIDCtrl.h"
#include "arduino.h"

double AttitudePIDCtrl::Compute(double input)
{
	// Did Setpoint change?
	if (fabs(TargetSetpoint - Setpoint) > 2 * fabs(StepSize))
	{
		Setpoint += StepSize;
	}
	/*How long since we last calculated*/
	unsigned long now = millis();
	double timeChange = (double) (now - lastTime);

	/*Compute all the working error variables*/
	double error = (Setpoint - input);
	// Start accumulating I error only when the Quad is approach lift off speed. Otherwise there could be a large accumulated
	// I error before motors start leading to erratic (and potentially dangerous behavior)
	if (QuadSpeed > 500)
	{
		Errsum += (error) * timeChange;
		// Cap Errsum
		Errsum = Errsum > 0? min(20000, Errsum):max(-20000, Errsum);
	}

	double dErr = (error - LastErr) / timeChange;
	/*Compute PID Output*/
	double currentKp = max(0, Kp * (1));
	Output = currentKp * error + Ki * Errsum + Kd * dErr;

	/*Remember some variables for next time*/
	LastErr = error;
	lastTime = now;

	return ApplySafeCheck(Output);
}

double AttitudePIDCtrl::ApplySafeCheck(double Output)
{
	if (fabs(LastOutput) > 150 && fabs(Output) > 150)
	{
		return 0;
	}
	LastOutput = Output;
	return Output;
}
