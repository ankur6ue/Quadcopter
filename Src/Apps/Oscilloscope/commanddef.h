/**************************************************************************

Filename    :   CommandDef.h
Content     :   
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef _COMMANpDEF_H_
#define _COMMANpDEF_H_ 

#include <qrect.h>
#include "plotiddef.h"
#include <qreadwritelock.h>
#include "PIDTypeMenu.h"

struct EchoCommand;

class CommandDef
{
public:
	CommandDef(char* name, float param)
	{
		strcpy(CommandName, name);
		// A command is 32 bytes in size. On the arduino side, only a packet of size 32 is considered to be a command
		memset(CommandBuffer, 0, 32);
		// strcpy(buffer, "Ankur   Hello");
		// Add sentinals in the beginning and end to identify malformed packets
		
		strcpy_s(CommandBuffer, 2, "A");
		if(CommandBuffer[0] == 'A')
		{
			int k = 0;
		}
		strcpy_s(CommandBuffer + 2, 14, name);
		sprintf_s(CommandBuffer + 18, 10, "%3.3f", param) ;
		strcpy_s(CommandBuffer+30, 2, "Z");
		numChar = 32;
	}
public:
	char	CommandName[20];
	char	CommandBuffer[32];
	int		numChar;
};

enum DirtyFlags: int
{
	SPEED				= 1,
	YAW					= 2,
	PITCH				= 4,
	ROLL				= 8,
	MOTORTOGGLE			= 16,
	PIDPARAMS			= 32,
	MOTORSTATE			= 64,
	PITCHSETPOINT		= 128,
	ROLLSETPOINT		= 256,
	YAWSETPOINT			= 512,
	DEF_PITCHSETPOINT	= 1024,
	DEF_ROLLSETPOINT	= 2048,
	SEND_BEACON			= 4096,
	PIDTYPE				= 8192
};

enum PIDFlags: int
{
	Pitch_Ki	= 1,
	Pitch_Kp	= 2,
	Pitch_Kd	= 4,

	Roll_Ki		= 8,
	Roll_Kp		= 16,
	Roll_Kd		= 32,

	Yaw_Ki		= 64,
	Yaw_Kp		= 128,
	Yaw_Kd		= 256,
};

enum MotorId: int
{
	FR = 1,
	BL = 2,
	BR = 4,
	FL = 8
};

// Stores user commands such as speed, yaw, pitch
class UserCommands
{
public:

	void SetFlag(DirtyFlags flag)
	{
		DirtyFlag |= flag;
	}
	void ClearFlag(DirtyFlags flag)
	{
		DirtyFlag = DirtyFlag & (~flag);
	}

	void SetPIDParamsFlag(PIDFlags flag)
	{
		PIDParamsFlag |= flag;
	}
	void ClearPIDParamsFlag(PIDFlags flag)
	{
		PIDParamsFlag = PIDParamsFlag & (~flag);
	}

	bool IsDirty()
	{
		return DirtyFlag != 0;
	}

	bool IsDirty(DirtyFlags flag)
	{
		return DirtyFlag & flag;
	}

	DirtyFlags GetDirtyFlag()
	{
		return (DirtyFlags)DirtyFlag;
	}

	bool IsPIDFlagDirty()
	{
		return PIDParamsFlag != 0;
	}

	bool IsPIDFlagDirty(PIDFlags flag)
	{
		return PIDParamsFlag & flag;
	}

	PIDFlags GetPIDParamsFlag()
	{
		return (PIDFlags)PIDParamsFlag;
	}

	void SetSpeed(int _speed)
	{
		lock.lockForWrite();
		Speed = _speed;
		SetFlag(SPEED);
		doUnlock();
	}

	void SetPIDType(PIDType pidType)
	{
		lock.lockForWrite();
		ePIDType = pidType;
		SetFlag(PIDTYPE);
		doUnlock();
	}

	double GetPIDType()
	{
		ClearFlag(PIDTYPE);
		return ePIDType;
	}
	void SetDefaultPitchSetpoint(double setPoint)
	{
		lock.lockForWrite();
		DefaultPitchSetPoint = setPoint;
		SetFlag(DEF_PITCHSETPOINT);
		doUnlock();
	}

	void SetDefaultRollSetpoint(double setPoint)
	{
		lock.lockForWrite();
		DefaultRollSetPoint = setPoint;
		SetFlag(DEF_ROLLSETPOINT);
		doUnlock();
	}

	void SetPitchSetpoint(double setPoint)
	{
		lock.lockForWrite();
		PitchSetPoint = setPoint;
		SetFlag(PITCHSETPOINT);
		doUnlock();
	}

	void SetRollSetpoint(double setPoint)
	{
		lock.lockForWrite();
		RollSetPoint = setPoint;
		SetFlag(ROLLSETPOINT);
		doUnlock();
	}

	void SetYawSetpoint(double setPoint)
	{
		lock.lockForWrite();
		YawSetPoint = setPoint;
		SetFlag(YAWSETPOINT);
		doUnlock();
	}

	double GetDefaultPitchSetpoint()
	{
		ClearFlag(DEF_PITCHSETPOINT);
		return PitchSetPoint;
	}

	double GetDefaultRollSetpoint()
	{
		ClearFlag(DEF_ROLLSETPOINT);
		return RollSetPoint;
	}

	double GetPitchSetpoint()
	{
		ClearFlag(PITCHSETPOINT);
		return PitchSetPoint;
	}

	double GetRollSetpoint()
	{
		ClearFlag(ROLLSETPOINT);
		return RollSetPoint;
	}

	double GetYawSetpoint()
	{
		ClearFlag(YAWSETPOINT);
		return YawSetPoint;
	}

	void SetPitchKi(double _ki)
	{
		lock.lockForWrite();
		PitchPIDParams.fKi = _ki;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Pitch_Ki);
		doUnlock();
	}

	void SetPitchKp(double _kp)
	{
		lock.lockForWrite();
		PitchPIDParams.fKp = _kp;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Pitch_Kp);
		doUnlock();
	}

	void SetPitchKd(double _kd)
	{
		lock.lockForWrite();
		PitchPIDParams.fKd = _kd;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Pitch_Kd);
		doUnlock();
	}

	void SetRollKi(double _ki)
	{
		lock.lockForWrite();
		RollPIDParams.fKi = _ki;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Roll_Ki);
		doUnlock();
	}

	void SetRollKp(double _kp)
	{
		lock.lockForWrite();
		RollPIDParams.fKp = _kp;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Roll_Kp);
		doUnlock();
	}

	void SetRollKd(double _kd)
	{
		lock.lockForWrite();
		RollPIDParams.fKd = _kd;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Roll_Kd);
		doUnlock();
	}

	void SetYawKi(double _ki)
	{
		lock.lockForWrite();
		YawPIDParams.fKi = _ki;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Yaw_Ki);
		doUnlock();
	}

	void SetYawKp(double _kp)
	{
		lock.lockForWrite();
		YawPIDParams.fKp = _kp;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Yaw_Kp);
		doUnlock();
	}

	void SetYawKd(double _kd)
	{
		lock.lockForWrite();
		YawPIDParams.fKd = _kd;
		SetFlag(PIDPARAMS);
		SetPIDParamsFlag(Yaw_Kd);
		doUnlock();
	}

	float GetPitchKp()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Pitch_Kp);
		return PitchPIDParams.fKp;
	}

	float GetPitchKi()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Pitch_Ki);
		return PitchPIDParams.fKi;
	}

	float GetPitchKd()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Pitch_Kd);
		return PitchPIDParams.fKd;
	}

	float GetRollKp()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Roll_Kp);
		return RollPIDParams.fKp;
	}

	float GetRollKi()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Roll_Ki);
		return RollPIDParams.fKi;
	}

	float GetRollKd()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Roll_Kd);
		return RollPIDParams.fKd;
	}

	float GetYawKp()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Yaw_Kp);
		return YawPIDParams.fKp;
	}

	float GetYawKi()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Yaw_Ki);
		return YawPIDParams.fKi;
	}

	float GetYawKd()
	{
		ClearFlag(PIDPARAMS);
		ClearPIDParamsFlag(Yaw_Kd);
		return YawPIDParams.fKd;
	}

	bool IsSendBeacon()
	{
		return DirtyFlag&SEND_BEACON;
	}

	void SetSendBeaconFlag()
	{
		SetFlag(SEND_BEACON);
	}

	void ClearSendBeaconFlag()
	{
		ClearFlag(SEND_BEACON);
	}

	void SetMotorsState(MotorId mId, int bState)
	{
		lock.lockForWrite();
		SetFlag(MOTORSTATE);
		if (bState == Qt::Checked)
		{
			MotorState = MotorState | mId;
		}
		else
		{
			MotorState = MotorState&(~mId);
		}
		doUnlock();
	}

	int GetMotorState() 
	{ 
		ClearFlag(MOTORSTATE);
		return MotorState; };

	void ToggleMotors(bool bmotorToggle)
	{
		lock.lockForWrite();
		bMotorToggle = bmotorToggle;
		SetFlag(MOTORTOGGLE);
		doUnlock();
	}

	void doLock()
	{
		lock.lockForRead();
	}

	void doUnlock()
	{
		lock.unlock();
	}

	int GetSpeed()
	{
		ClearFlag(SPEED);
		return Speed;
	}

	bool GetMotorToggle()
	{
		ClearFlag(MOTORTOGGLE);
		return bMotorToggle;
	}

	static UserCommands &Instance()
	{
		static UserCommands command;
		return command;
	}

private:
	// Can't create a usercommand object without calling instance
    UserCommands()
	{
		DirtyFlag = 0;
	};
    UserCommands( const UserCommands & );
    UserCommands &operator=( const UserCommands & );

	int		DirtyFlag;
	int		PIDParamsFlag;
	int		MotorState;
	int		Speed;
	int		PitchSetPoint;
	int		DefaultPitchSetPoint;
	int		RollSetPoint;
	int		DefaultRollSetPoint;
	int		YawSetPoint;
	PIDType	ePIDType;

	struct PitchPIDParamsDef
	{
	float	fKp;
	float	fKi;
	float	fKd;
	} PitchPIDParams;

	struct YawPIDParamsDef
	{
	float	fKp;
	float	fKi;
	float	fKd;
	} YawPIDParams;

	struct RollPIDParamsDef
	{
	float	fKp;
	float	fKi;
	float	fKd;
	} RollPIDParams;

	bool	bMotorToggle;
	QReadWriteLock lock;	
};


#endif
