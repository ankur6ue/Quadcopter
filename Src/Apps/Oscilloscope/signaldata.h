#ifndef _SIGNAL_DATA_H_
#define _SIGNAL_DATA_H_ 1

#include <qrect.h>
#include "plotiddef.h"
#include <qreadwritelock.h>

struct EchoCommand;

enum DirtyFlags
{
	SPEED	= 0x1,
	YAW		= 0x2,
	PITCH	= 0x4,
	ROLL	= 0x8,
	MOTORTOGGLE = 0x16
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

	bool IsDirty()
	{
		return DirtyFlag != 0;
	}

	bool IsDirty(DirtyFlags flag)
	{
		return DirtyFlag & flag;
	}

	void SetSpeed(int _speed)
	{
		lock.lockForWrite();
		Speed = _speed;
		SetFlag(SPEED);
		doUnlock();
	}

	void ToggleMotors(bool bmotorToggle)
	{
		lock.lockForWrite();
		bMotorToggle = bmotorToggle;
		SetFlag(MOTORTOGGLE);
		doUnlock();
	}

	// A command is 32 bytes in size. On the arduino side, only a packet of size 32 is considered to be a command
	int CreateCommand(char* buffer, char* commandName, float data)
	{
		memset(buffer, 0, 32);
		strcpy_s(buffer, 16, commandName);
		sprintf_s(buffer + 16, 16, "%3.3f", data) ;
		return 32;
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
	int		Speed;
	bool	bMotorToggle;
	QReadWriteLock lock;	
};

// Stores plot data
class SignalData
{
public:
    static SignalData &instance(PlotId pid, CurveId cid);
	
    void append( const QPointF &pos );
    void clearStaleValues( double min );

    int size() const;
    QPointF value( int index ) const;

    QRectF boundingRect() const;

    void lock();
    void unlock();

private:
    SignalData();
    SignalData( const SignalData & );
    SignalData &operator=( const SignalData & );
	
    virtual ~SignalData();
	
    class PrivateData;
    PrivateData *d_data;
};

#endif
