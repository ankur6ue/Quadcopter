
/*
  PWMMotorControl
 */

// the setup routine runs once when you press reset:
#include "arduino.h"
#include "PIDControl.h"

PIDController::PIDController()
{
	Errsum 		= 0;
	LastErr 	= 0;
}
double PIDController::Compute(double input)
{
   /*How long since we last calculated*/
   unsigned long now = millis();
   double timeChange = (double)(now - lastTime);
  
   /*Compute all the working error variables*/
   double error = Setpoint - input;
   Errsum += (error * timeChange);
   double dErr = (error - LastErr) / timeChange;
  
   /*Compute PID Output*/
   Output = Kp * error + Ki * Errsum + Kd * dErr;
  
   /*Remember some variables for next time*/
   LastErr = error;
   lastTime = now;
   return Output;
}
  
void PIDController::SetTunings(double kp, double ki, double kd)
{
   Kp = kp;
   Ki = ki;
   Kd = kd;
}

double PIDController::GetErrorSum()
{
	return Errsum;
}

double PIDController::GetSetPoint()
{
	return Setpoint;
}
void PIDController::SetSetPoint(double _setPoint)
{
	Setpoint = _setPoint;
}
