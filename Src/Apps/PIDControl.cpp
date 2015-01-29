
/*
  PWMMotorControl
 */

// the setup routine runs once when you press reset:
#include "arduino.h"
#include "PIDControl.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"

PIDController::PIDController()
{
	Errsum 			= 0;
	LastErr 		= 0;
	Setpoint		= 0;
	TargetSetpoint 	= 0;
}

void PIDController::SetSpeed(int speed)
{
	QuadSpeed = speed;
}

double PIDController::Compute(double input)
{
	// Did Setpoint change?
	if (fabs (TargetSetpoint - Setpoint) > 2*fabs(StepSize))
	{
		Setpoint += StepSize;
	}
   /*How long since we last calculated*/
   unsigned long now = millis();
   double timeChange = (double)(now - lastTime);
  
   /*Compute all the working error variables*/
   double error = (Setpoint - input);
   // Start accumulating I error only when the Quad is approach lift off speed. Otherwise there could be a large accumulated
   // I error before motors start leading to erratic (and potentially dangerous behavior)
   if (QuadSpeed > 50)
   {
	   Errsum += (error)*timeChange;
   }

   double dErr = (error - LastErr) / timeChange;
   /*Compute PID Output*/
   double currentKp = max(0, Kp*(1));
   Output = currentKp * error + Ki * Errsum + Kd * dErr;
  
   /*Remember some variables for next time*/
   LastErr = error;
   lastTime = now;
   // Subject Output to some sanity tests before returning
   if (Output > 5)
	{
		SERIAL.println("PID output exceeded safety bound");
	}
   Output = min(5, Output);
   return Output;
}
  
void PIDController::SetTunings(double kp, double ki, double kd)
{
   Kp = kp;
   Ki = ki/1000;
   Kd = kd*1000;
}

void PIDController::SetErrorSum(double val)
{
	Errsum = val;
}

void PIDController::SetLastError(double val)
{
	LastErr = val;
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

void PIDController::SetNewSetpoint(double _setPoint)
{
	TargetSetpoint = _setPoint;
	StepSize = (TargetSetpoint - Setpoint)/100;
}
