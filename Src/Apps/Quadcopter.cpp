/**************************************************************************

Filename    :   Quadcopter.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "Quadcopter.h"
#include "Logger.h"
#include "CommandCtrl.h"
#include "Scheduler.h"
#include "AttitudePIDCtrl.h"
#include "BLMotorControl.h"
#include "IMU.h"
#include "ErrorsDef.h"
#include "BeaconListener.h"
#include "SoftwareSerial.h"
#include "Orientation.h"
#include "SerialDef.h"
#include "AccelSampler.h"
#include "AltitudeCtrl.h"
/* The Logger class sends the chracters accumulated in the Log to the serial port every 100 (configurable) ms.
 * This appears to be safe as the packetization timeout (RO) of the Xbee radio that I'm using for radio communication
 * is defined as 3*character time. At a baudrate of 115200, with 10 bits (including start/stop bit) needed for transfering
 * one bit, the timeout period = 3*10/115200*1000ms = 0.26ms. The time taken to tranfer one command packet (70 bytes) =
 * 70*10/115200 = 6ms. Also, please note that the size of a packet according to the Xbee docs is 72 bytes, so it's good to
 * keep the command buffer < 72 bytes, so all of it can be transmitted in one go. See the XBee manual on Sparkfun's website
 * for more detail.
 */
/*
 * cQuadStateLogger sends Quadstate every 1000ms
 */

Scheduler 			cScheduler;
QuadStateLogger 	cQuadStateLogger(1, "QuadStateLogger");
PIDStateLogger		cPIDStateLogger(15, "PIDStateLogger");
AltitudeCtrl		cAltitudeCtrl(50, "AltitudeControl");
ExceptionLogger		cExceptionLogger(1, "ExceptionLogger");
CommandCtrl			cCommandCtrl(100, "CommandCtrl");
MotorCtrl			cMotorCtrl(100, "MotorCtrl");
BeaconListener		cBeaconListener(1, "BeaconListener");
CalcOrientation		cCalcOrientation(200, "Calculate Orientation");
PIDController 		cPIDCntrl(100, "PIDController");
AccelSampler 		cAccelSampler(500, "AccelSampler");
ExceptionMgr		cExceptionMgr;

int 				StartupTime;
int					ESCPoweredTime = 0;
unsigned long		Now;
unsigned long		Before;
SoftwareSerial 		SSerial(3,4);
QuadStateDef 		QuadState;
int 				MPUInterruptCounter = 0;

// Thresholds
// Thresholds
// Controls how far the I term for the orientation rate controller is allowed to go
int					RateWindUp = 150;

// Controls how far the I term for the altitude PId controller is allowed to go
int					AltWindUp = 500;

// The PID output is not allowed to exceed this threshold
int					MaxPIDOutput = 450;
// To prevent damage to the motor, the motor input is capped
int					MaxMotorInput = 1500;
// Speed at which life off occurs. Depends on the weight of the quad, type of motors etc.
int					LiftOffSpeed = 1100;

int					BaseSpeed = 800;
IMU Imu;

void InitQuadState()
{
	QuadState.Speed 		= 030;
	QuadState.bMotorToggle 	= false;
	QuadState.Alpha 		= 0.7;
}
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================


void setup()
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    // Setup Initial Quadstate
    InitQuadState();
    // initialize SERIAL communication
    SERIAL.begin(115200);

    SERIAL.print("In Setup");
    cPIDCntrl.CreateControllers();
    cScheduler.SetPerfReportFrequency(10000); // Report performance every 10 seconds
    cScheduler.RegisterTask(&cQuadStateLogger);
    cScheduler.RegisterTask(&cPIDStateLogger);
    cScheduler.RegisterTask(&cCommandCtrl);
    cScheduler.RegisterTask(&cBeaconListener);
    cScheduler.RegisterTask(&cExceptionLogger);
    cScheduler.RegisterTask(&cCalcOrientation);
    cScheduler.RegisterTask(&cPIDCntrl);
    cScheduler.RegisterTask(&cAccelSampler);
    cScheduler.RegisterTask(&cAltitudeCtrl);

    /// Motors must be initialized first, otherwise the ESC will see inconsistent voltage on the PWM pin. They should
    /// see the ESCLow setting set during ESC calibration.
    cMotorCtrl.InitMotors();
    cBeaconListener.Stop(); // Only listen for beacon when the motors are on

    // Clear all exceptions

    cExceptionMgr.ClearExceptionFlag();
    ////////////////// MPU Initialization ////////////////
    Imu.Init();
 //   Imu.DMPInit();

    // Set default PIDController to be the Rate controller
    QuadState.ePIDType = RatePIDControl;
    StartupTime = millis();

}

void loop()
{
	cScheduler.Tick();
}

int main(void)
{
	init();

	#if defined(USBCON)
	USBDevice.attach();
	#endif

	setup();

	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}

	return 0;
}
