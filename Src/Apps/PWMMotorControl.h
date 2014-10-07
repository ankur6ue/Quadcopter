
/*
  PWMMotorControl  
 */

// Error Codes
enum PWMMotorErrorCodes
{
	INVALID_MOTOR_ID	= 1,
	MOTOR_ID_OK			= 0
};

#ifdef USE_NAIVE
#define E1 10  // Enable Pin for motor 1
#else
#define E1 3 // Enable Pin for motor 1
#define E2 11 // Enable Pin for motor 2

#define I1 8  // Control pin 1 for motor 1
#define I2 9  // Control pin 2 for motor 1
#define I3 6  // Control pin 1 for motor 2
#define I4 7  // Control pin 2 for motor 2
#endif
//#define USE_NAIVE

void PWMMotorCtrlInit();
int SetMotorSpeed(int _motorId, int _speed);
int SetMotorDirection(int _motorId, int clckWise);
int StopMotor(int _motorId);

typedef struct MotorCntrl
{
	int MotorId;
	int Speed;
};

