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

extern PIDController 	PIDCntrl;
extern MotorCtrl		cMotorCtrl;
extern ExceptionMgr		cExceptionMgr;
extern BeaconListener	cBeaconListener;

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

	void SendAck()
	{
		char tmp1[50];
		char tmp2[10];
		dtostrf(Param, 3, 3, tmp2);
		sprintf(tmp1, "%s %s %s", "Ack", Name, tmp2);
		SERIAL.print(tmp1);
	}
};

unsigned long CommandCtrl::Run()
{
	Command cmd;
	unsigned long before = micros();
	if (CommandReceived(&cmd))
	{
		ProcessCommands(&cmd);
	}
	unsigned long now = micros();
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
		}
	}

	if (!strcmp(cmd->Name, "MotorToggle"))
	{
		QuadState.bMotorToggle = (bool) cmd->Param;

		if (QuadState.bMotorToggle)
		{
			ESCPoweredTime = millis();
			QuadState.Speed = 0;

			// cLog.EchoCommand(cLog.CreateCommand("ESC", 1));
			// Turn on motor after a suitable delay
		} else
		{
			cMotorCtrl.ResetMotors();
			cExceptionMgr.ClearExceptionFlag();
			bIsPIDSetup 	= false;
			bIsKpSet 		= false;
			bIsKiSet 		= false;
			bIsKdSet 		= false;
			bIsYawKpSet 	= false;
			bIsYawKiSet 	= false;
			bIsYawKdSet 	= false;
			bIsPIDTypeSet 	= false;
			PIDCntrl.Reset();
			cBeaconListener.Reset();
			// Stop the exception manager when motors are off
			cBeaconListener.Stop();
			//	cLog.EchoCommand(cLog.CreateCommand("ESC", 0));
		}
	}

	if (!strcmp(cmd->Name, "PIDType"))
	{
		QuadState.ePIDType = (PIDType)(int)cmd->Param;
		PIDCntrl.Reset();
		bIsPIDTypeSet = true;
	}

	if (!strcmp(cmd->Name, "Kp"))
	{
		QuadState.Kp = cmd->Param;
		bIsKpSet = true;
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Pitch);
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Roll);
	}

	if (!strcmp(cmd->Name, "Ki"))
	{
		QuadState.Ki = cmd->Param;
		bIsKiSet = true;
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Pitch);
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Roll);
		PIDCntrl.SetErrSum(0.0, Axis_Pitch);
		PIDCntrl.SetErrSum(0.0, Axis_Roll);
	}

	if (!strcmp(cmd->Name, "Kd"))
	{
		QuadState.Kd = cmd->Param;
		bIsKdSet = true;
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Pitch);
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Roll);

	}

	if (!strcmp(cmd->Name, "Yaw_Kp"))
	{
		QuadState.Yaw_Kp = cmd->Param;
		bIsYawKpSet = true;
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "Yaw_Ki"))
	{
		QuadState.Yaw_Ki = cmd->Param;
		bIsYawKiSet = true;
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Yaw);
		PIDCntrl.SetErrSum(0.0, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "Yaw_Kd"))
	{
		QuadState.Yaw_Kd = cmd->Param;
		bIsYawKdSet = true;
		PIDCntrl.SetTunings(QuadState.Kp, QuadState.Ki, QuadState.Kd, Axis_Yaw);
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
		PIDCntrl.SetHoverAttitude(attitude, Axis_Pitch);
	}

	if (!strcmp(cmd->Name, "PitchDisp"))
	{
		double disp = cmd->Param;
		PIDCntrl.OnControlInput(disp, Axis_Pitch);
	}

	if (!strcmp(cmd->Name, "RollHoverAtt"))
	{
		double attitude = cmd->Param;
		PIDCntrl.SetHoverAttitude(attitude, Axis_Roll);
		double yawAttitude = QuadState.Yaw; // Set yaw to wherever the quad is oriented at.
		PIDCntrl.SetHoverAttitude(yawAttitude, Axis_Yaw);
	}

	if (!strcmp(cmd->Name, "RollDisp"))
	{
		double disp = cmd->Param;
		PIDCntrl.OnControlInput(disp, Axis_Roll);

	}

	if (!strcmp(cmd->Name, "YawDisp"))
	{
		double disp = cmd->Param;
		PIDCntrl.OnControlInput(disp, Axis_Yaw);
	}
};
