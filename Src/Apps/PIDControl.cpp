
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
