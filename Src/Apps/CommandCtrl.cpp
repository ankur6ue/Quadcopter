/**************************************************************************

Filename    :   CommandCtrl.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "arduino.h"
#include "CommandCtrl.h"
#include "Quadcopter.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"
#include "AttitudePIDCtrl.h"
#include "BLMotorControl.h"
#include "BeaconListener.h"
#include "ErrorsDef.h"
#include "AltitudeCtrl.h"

extern PIDController 	cPIDCntrl;
extern MotorCtrl		cMotorCtrl;
extern ExceptionMgr		cExceptionMgr;
extern BeaconListener	cBeaconListener;
extern AltitudeCtrl		cAltitudeCtrl;

struct Command
{
	Command()
	{
		Param = 0.0;
	}
	char Name[16];
	double Param;
	char CommandBuffer[32];

	char* Serialize(char* buffer)
	{
		char tmp[10];
		dtostrf(Param, 3, 3, tmp);
		sprintf(buffer, "%s %s %s", "AT", Name, tmp);
		return buffer;
	}

	void AttachSentinal()
	{
		Serial.print("z");
	}
	void SendAck()
	{
		char tmp1[50];
		char tmp2[10];
		dtostrf(Param, 3, 3, tmp2);
		sprintf(tmp1, "%s %s %s", "Ack", Name, tmp2);
		SERIAL.print(tmp1);
		AttachSentinal();
	}
};

unsigned long CommandCtrl::Run()
{
	Command cmd;
	unsigned long before = millis();
	if (CommandReceived(&cmd))
	{
		ProcessCommands(&cmd);
	}
	unsigned long now = millis();
	return now - before;

}

bool CommandCtrl::CommandReceived(Command* cmd)
{
	int numBytes = SERIAL.available();
	char currentChar;
	// If there was some data, but we couldn't read the entire command, wait for a little bit for the data transfer to
	// finish.
	if (numBytes != 32 && numBytes > 0)
	{
		delay(5);
	}
	// check again
	numBytes = SERIAL.available();

	if (numBytes == 32)
	{
		int index = 0;
		// only read the first 32 characters.
		while (index < 32)
		{
			currentChar = SERIAL.read();
			cmd->CommandBuffer[index++] = currentChar;
		}

		// Check for sentinals. This helps to ensure we are not reading a malformed packet
		if (cmd->CommandBuffer[0] != 'A' || cmd->CommandBuffer[30] != 'Z')
		{
			return false;
		}

		sprintf(cmd->Name, "%s", cmd->CommandBuffer + 2);
		cmd->Param = strtod(cmd->CommandBuffer + 18, NULL);
		// This doesn't work on Arduino. sscanf can't parse floats.
		// sscanf(cmd->CommandBuffer+16, "%f", &(cmd->Param));
		return true;
	}
	else // empty out the serial buffer
	{
		for (int i = 0; i < numBytes; i++)
			char c = SERIAL.read();
	}
	return false;
}

void CommandCtrl::ProcessCommands(Command* cmd)
{
	cmd->SendAck();

	if (!strcmp(cmd->Name, "Speed"))
	{
		if (cmd->Param < 0 || cmd->Param > 1500)
		{
			QuadState.Speed = 100; // if crazy data, then set Speed to some safe value
		} else
		{
			QuadState.Speed = cmd->Param;
			QuadState.bThrottleOverride = true;
		}
	}

	if (!strcmp(cmd->Name, "MotorToggle"))
	{
		QuadState.bMotorToggle = (bool) cmd->Param;

		if (QuadState.bMotorToggle)
		{
			ESCPoweredTime = millis();
			QuadState.Speed = BaseSpeed;
			QuadState.bThrottleOverride = false;
			// Set yaw to wherever the quad is pointed at
			double yawAttitude = QuadState.Yaw;
			cPIDCntrl.SetHoverAttitude(yawAttitude, Axis_Yaw);

			// cLog.EchoCommand(cLog.CreateCommand("ESC", 1));
			// Turn on motor after a suitable delay
		}
		else
		{
			cMotorCtrl.ResetMotors();
			cExceptionMgr.ClearExceptionFlag();
			QuadState.Reset();
			cPIDCntrl.Reset();
			cAltitudeCtrl.Reset();
			cBeaconListener.Reset();
			// Stop the exception manager when motors are off
			cBeaconListener.Stop();
			//	cLog.EchoCommand(cLog.CreateCommand("ESC", 0));
		}
	}

	if (!strcmp(cmd->Name, "PIDType"))
	{
		QuadState.ePIDType = (PIDType)(int)cmd->Param;
		cPIDCntrl.Reset();
		QuadState.bIsPIDTypeSet = true;
	}

	if (!strcmp(cmd->Name, "Kp"))
	{
		QuadState.PitchParams.Kp = cmd->Param;
		QuadState.bIsKpSet = true;
		cPIDCntrl.SetTunings(QuadState.PitchParams, Axis_Pitch);
		cPIDCntrl.SetTunings(QuadState.PitchParams, Axis_Roll);
	}

	if (!strcmp(cmd->Name, "Ki"))
	{
		QuadState.PitchParams.Ki = cmd->Param;
		QuadState.bIsKiSet = true;
		cPIDCntrl.SetTunings(QuadState.PitchParams, Axis_Pitch);
		cPIDCntrl.SetTunings(QuadState.PitchParams, Axis_Roll);
	//	cPIDCntrl.SetErrSum(0.0, Axis_Pitch);
	//	cPIDCntrl.SetErrSum(0.0, Axis_Roll);
	}

	if (!strcmp(cmd->Name, "Kd"))
	{
		QuadState.PitchParams.Kd = cmd->Param;
		QuadState.bIsKdSet = true;
		cPIDCntrl.SetTunings(QuadState.PitchParams, Axis_Pitch);
		cPIDCntrl.SetTunings(QuadState.PitchParams, Axis_Roll);

	}

	if (!strcmp(cmd->Name, "Yaw_Kp"))
	{
		QuadState.YawParams.Kp = cmd->Param;
		QuadState.bIsYawKpSet = true;
		cPIDCntrl.SetTunings(QuadState.YawParams, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "Yaw_Ki"))
	{
		QuadState.YawParams.Ki = cmd->Param;
		QuadState.bIsYawKiSet = true;
		cPIDCntrl.SetTunings(QuadState.YawParams, Axis_Yaw);
		cPIDCntrl.SetErrSum(0.0, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "Yaw_Kd"))
	{
		QuadState.YawParams.Kd = cmd->Param;
		QuadState.bIsYawKdSet = true;
		cPIDCntrl.SetTunings(QuadState.YawParams, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "Altitude_Kp"))
	{
		QuadState.AltParams.Kp = cmd->Param;
	//	QuadState.bIsAltitudeKpSet = true;
		cAltitudeCtrl.SetTunings(QuadState.AltParams);
	}

	if (!strcmp(cmd->Name, "Altitude_Ki"))
	{
		QuadState.AltParams.Ki = cmd->Param;
	//	QuadState.bIsAltitudeKiSet = true;
		cAltitudeCtrl.SetTunings(QuadState.AltParams);
	}

	if (!strcmp(cmd->Name, "HoverAlt"))
	{
		QuadState.HoverAltitude = cmd->Param;
	//	QuadState.bIsAltitudeKiSet = true;
		cAltitudeCtrl.SetHoverAltitude(QuadState.HoverAltitude);
	}

	if (!strcmp(cmd->Name, "Altitude_Kd"))
	{
		QuadState.AltParams.Kd = cmd->Param;
	//	QuadState.bIsAltitudeKdSet = true;
		cAltitudeCtrl.SetTunings(QuadState.AltParams);
	}

	if (!strcmp(cmd->Name, "A2R_PKp"))
	{
		QuadState.A2R_PKp = cmd->Param;
		QuadState.bIsA2R_PKpSet = true;
		cPIDCntrl.SetA2RTunings(QuadState.A2R_PKp, Axis_Pitch);
	}

	if (!strcmp(cmd->Name, "A2R_RKp"))
	{
		QuadState.A2R_RKp = cmd->Param;
		QuadState.bIsA2R_RKpSet = true;
		cPIDCntrl.SetA2RTunings(QuadState.A2R_RKp, Axis_Roll);
	}

	if (!strcmp(cmd->Name, "A2R_YKp"))
	{
		QuadState.A2R_YKp = cmd->Param;
		QuadState.bIsA2R_YKpSet = true;
		cPIDCntrl.SetA2RTunings(QuadState.A2R_YKp, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "MotorState"))
	{
		int motorState = cmd->Param;
		MotorFL.bIsRunning = motorState & FL;
		MotorBL.bIsRunning = motorState & BL;
		MotorBR.bIsRunning = motorState & BR;
		MotorFR.bIsRunning = motorState & FR;
	}

	// Handle Beacon signal .
	if (!strcmp(cmd->Name, "Beacon"))
	{
		cBeaconListener.SetBeaconReceived();
	}
	// Sent when the motors are reset. Both the setpoint and target setpoint are set to the default value.
	if (!strcmp(cmd->Name, "PitchHoverAtt"))
	{
		double attitude = cmd->Param;
		cPIDCntrl.SetHoverAttitude(attitude, Axis_Pitch);
	}

	if (!strcmp(cmd->Name, "PitchDisp"))
	{
		double disp = cmd->Param;
		cPIDCntrl.OnControlInput(disp, Axis_Pitch);
	}

	if (!strcmp(cmd->Name, "RollHoverAtt"))
	{
		double attitude = cmd->Param;
		cPIDCntrl.SetHoverAttitude(attitude, Axis_Roll);
	}

	if (!strcmp(cmd->Name, "RollDisp"))
	{
		double disp = cmd->Param;
		cPIDCntrl.OnControlInput(disp, Axis_Roll);

	}

	if (!strcmp(cmd->Name, "YawDisp"))
	{
		double disp = cmd->Param;
		cPIDCntrl.OnControlInput(disp, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "DCMAlpha"))
	{
		double alpha = cmd->Param;
		QuadState.Alpha = alpha;
	}
};

