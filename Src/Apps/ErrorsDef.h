#ifndef __ERRORSDEF__
#define __ERRORSDEF__

enum Exception
{
	BUFFER_OVERFLOW 		= 1,
	EXCESSIVE_PID_OUTPUT 	= 2, 	// If the PID output exceeds a threshold, enter shutddown mode to prevent damage to ESC
	NO_BEACON_SIGNAL		= 4, 	// We send pings every 1 seconds. If pings are not received for 2 seconds, enter shutdown mode
	BAD_MPU_DATA			= 8
};

class ExceptionMgr
{
public:
	ExceptionMgr();
	void SetException(Exception e);
	long GetException();
	void ClearExceptionFlag();
	void ClearFlag(Exception e);
	bool IsException();
	bool IsCriticalException();
	long ExceptionFlag;
};

#endif
