
struct QuadStateDef
{
	float 	Yaw;
	float 	Pitch;
	float 	Roll;
	float 	Kp;
	float 	Ki;
	float 	Kd;
	float 	Yaw_Kp;
	float 	Yaw_Ki;
	float 	Yaw_Kd;
	float 	PID_Yaw; // Denotes output of the PID Controller
	float 	PID_Roll;
	float 	PID_Pitch;
	bool  	bMotorToggle;
	int		Speed;
	int		QuadStateFlag;
};

enum
{
	Kp_Set 		= 1,
	Kd_Set 		= 2,
	Ki_Set 		= 4,
	YawKp_Set 	= 8,
	YawKi_Set	= 16,
	YawKd_Set	= 32,
	Def_PitchSet= 64,
	Def_RollSet = 128
};

extern QuadStateDef QuadState;

// Global Variables

extern int 				StartupTime;
extern bool				bIsPIDSetup;
extern bool				bIsKpSet;
extern bool				bIsKiSet;
extern bool				bIsKdSet;
extern bool				bIsYawKpSet;
extern bool				bIsYawKiSet;
extern bool				bIsYawKdSet;

extern int				ESCPoweredTime;
extern unsigned long	Now;
extern unsigned long	Before;
