/**************************************************************************

Filename    :   BeaconListener.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "AltitudeCtrl.h"
#include "Quadcopter.h"
#include "IMU.h"
#include "Utils.h"

extern IMU Imu;

// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
// Also, Multiplication are a lot more efficient on the Arduino than divisions
// https://learn.sparkfun.com/tutorials/data-types-in-arduino
const float 	MicroSecToCm = 1.0/(2*29.0);
// Declared global as these variables are used in a global ISR
// Use Interrupt 0 which is Pin 2 on Arduino Mega
const int 		EchoPin = 2;
// Variables that could be modified in an interrupt service routine
// must be declared volatile otherwise the compiler might replace them
// with constants.
volatile long 	EchoStart 	= 0; // Records start of echo pulse
volatile long 	EchoEnd		= 0;   // Records end of echo pulse
volatile long 	EchoDuration= 0; // Duration - difference between end and start
// Used to tell the ISR that the sonar ping pulse is being sent, so don't look
// for level change on the Echopin
volatile bool 	bSonarPulse	= false;
extern int		AltWindUp;

void EchoInterruptISR()
{
  if(bSonarPulse)
  {
    switch (digitalRead(EchoPin))                     // Test to see if the signal is high or low
    {
      case HIGH:                                      // High so must be the start of the echo pulse
        EchoEnd = 0;                                 // Clear the end time
        EchoStart = micros();                        // Save the start time
        break;

      case LOW:                                       // Low so must be the end of hte echo pulse
        EchoEnd 		= micros();                          // Save the end time
        EchoDuration 	= EchoEnd - EchoStart;        // Calculate the pulse duration
        break;
    }
  }
}


AltitudeCtrl::AltitudeCtrl(int _frequency, const char* _name): Task(_frequency, _name)
{
	attachInterrupt(0, EchoInterruptISR, CHANGE);
	int maxClimbRate 		= 3000; //1/10 of mm/sec, so 2000 would correspond to 20 cm/sec
	MaxAltDiff 				= maxClimbRate/_frequency;
	ClimbRate				= 1000/_frequency;
	RestGroundSonarDistance = 300;
	PrevAltitude = 0;
}

long AltitudeCtrl::Filter(long newVal, long oldVal, long maxDiff)
{
	long diff;
	long result;

	if (oldVal == 0)     					// Filter is not initialized (no old value)
		return(newVal);

	diff = newVal - oldVal;      			// Difference with old reading
	if (diff > maxDiff)
	    result = oldVal + maxDiff;    		// We limit the max difference between readings
	else
	{
	    if (diff < -maxDiff)
	    	result = oldVal - maxDiff;        	// We limit the max difference between readings
	    else  								// We are within Range
	    	result = (newVal + oldVal) >> 1;  // Small filtering (average filter)
	}
	return(result);
}

void AltitudeCtrl::SetTunings(PIDParams& params)
{
	mPIDParams = params;
	mPIDParams.Kp = mPIDParams.Kp/50; // Apply scaling to bring Kp in the right range
	mPIDParams.Kd = mPIDParams.Kd*10;
	mPIDParams.Ki = mPIDParams.Ki/25;

}

void AltitudeCtrl::SetHoverAltitude(float hoverAlt)
{
	TargetAltitude = max(RestGroundSonarDistance, hoverAlt*100); // convert from cm to 1/10 of a mm
}

unsigned long AltitudeCtrl::Run()
{
	unsigned long before = millis();
	TimeElapsed = before - LastRun;
	LastRun = before;
	SendPing();
	// Apply filtering to new altitude measurement so it's not too far from previous measurement
	CurrentAltitude = Filter(GetCurrentAltitude(), PrevAltitude, MaxAltDiff);
	//CurrentAltitude = GetCurrentAltitude();
	if (CurrentAltTarget - TargetAltitude > ALT_DEAD_ZONE)
	{
		CurrentAltTarget = TargetAltitude - ClimbRate;
	}
	if (CurrentAltTarget - TargetAltitude < ALT_DEAD_ZONE)
	{
		CurrentAltTarget = TargetAltitude + ClimbRate;
	}

	ThrottleAdjustment = 0;
	bool bIsPIDSetup = QuadState.IsPIDControlReady();
	if (bIsPIDSetup)
	{
		// If the user is setting speed directly, don't run altitude PID
		if (QuadState.bThrottleOverride == false)
		{
			float V_Delta = Imu.GetVelocityDelta();
			ThrottleAdjustment = UpdatePID(CurrentAltTarget, CurrentAltitude) - mPIDParams.Kd*V_Delta;
			QuadState.Vz = V_Delta;
		}
	}
//	Vz_Sonar = (float)((CurrentAltitude - PrevAltitude))*1000/timeElapsed;
	PrevAltitude	= CurrentAltitude;

	QuadState.CurrentAltitude = CurrentAltitude;
	QuadState.PID_Alt	= ThrottleAdjustment;
	QuadState.Vz_Sonar = Vz_Sonar;

	unsigned long now = millis();
	return now - before;
}

void AltitudeCtrl::Reset()
{
	TargetAltitude = CurrentAltitude;
	ErrorSum = 0;
}

int AltitudeCtrl::UpdatePID(long currentAltTarget, long currentAltitude)
{
	long altErr = currentAltTarget - currentAltitude;
	ErrorSum += altErr*mPIDParams.Ki*TimeElapsed/1000;
	ErrorSum = constrain_f(ErrorSum, 0, AltWindUp);
	return altErr*mPIDParams.Kp + ErrorSum;
}

void AltitudeCtrl::SendPing()
{
	 // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
	  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	  bSonarPulse = false;
	  pinMode(EchoPin, OUTPUT);
	  digitalWrite(EchoPin, LOW);
	  delayMicroseconds(2);
	  digitalWrite(EchoPin, HIGH);
	  // Ok to wait 5 microseconds as that corresponds to 200KHz which is way faster
	  // than any control loops we are running
	  delayMicroseconds(5);
	  digitalWrite(EchoPin, LOW);
	  pinMode(EchoPin, INPUT);
	  bSonarPulse = true;
	// The same pin is used to read the signal from the PING))): a HIGH
	  // pulse whose duration is the time (in microseconds) from the sending
	  // of the ping to the reception of its echo off of an object.

}

long AltitudeCtrl::GetCurrentAltitude()
{
	// convert the time into a distance
	// inches = microsecondsToInches(echo_duration);
	return 100*MicrosecondsToCentimeters(EchoDuration);
}

float AltitudeCtrl::MicrosecondsToCentimeters(unsigned long echoDuration_muS)
{
	  return (float)(echoDuration_muS)*MicroSecToCm ;
}
