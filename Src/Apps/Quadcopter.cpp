
#include "Quadcopter.h"
#include "Pinger.h"
#include "CommandCtrl.h"
#include "Callbacks.h"
#include "ReadAnalogPin.h"

MyLog 			cLog;

/* The pinger class sends the chracters accumulated in the Log to the serial port every 100 (configurable) ms.
 * This appears to be safe as the packetization timeout (RO) of the Xbee radio that I'm using for radio communication
 * is defined as 3*character time. At a baudrate of 115200, with 10 bits (including start/stop bit) needed for transfering
 * one bit, the timeout period = 3*10/115200*1000ms = 0.26ms. The time taken to tranfer one command packet (70 bytes) =
 * 70*10/115200 = 6ms. Also, please note that the size of a packet according to the Xbee docs is 72 bytes, so it's good to
 * keep the command buffer < 72 bytes, so all of it can be transmitted in one go. See the XBee manual on Sparkfun's website
 * for more detail.
 */
Pinger 			cPinger(100, SendLog, &cLog);
PIDController 		PitchCntrl;
PIDController		YawCntrl;
PIDController		RollCntrl;

int 				StartupTime;
bool				bIsPIDSetup = false;
bool				bIsKpSet = false;
bool				bIsKiSet = false;
bool				bIsKdSet = false;
bool				bIsYawKpSet = false;
bool				bIsYawKiSet = false;
bool				bIsYawKdSet = false;
int					ESCPoweredTime = 0;
unsigned long		Now;
unsigned long		Before;
SoftwareSerial 		SSerial(3,4);
QuadStateDef 		QuadState;
Command 			cmd;
int 				MPUInterruptCounter = 0;
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here

IMU Imu;

void InitQuadState()
{
	QuadState.Speed = 030;
	QuadState.bMotorToggle = false;
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
//    SSerial.begin(115200); // need this baudrate otherwise FIFO overflow will occur. WE are not sending data fast enough
    SERIAL.print("In Setup");

    /// Motors must be initialized first, otherwise the ESC will see inconsistent voltage on the PWM pin. They should
    /// see the ESCLow setting set during ESC calibration.
    InitMotors();

    ////////////////// MPU Initialization ////////////////
    Imu.Init();
    Imu.DMPInit();

    StartupTime = millis();

}

void PrintData(float data[], int numElem)
{
	for (int i = 0; i < numElem; i++)
	{
		SERIAL.print(data[i]); SERIAL.print(" ");
	}
	SERIAL.print("\n");
}

void loop()
{
	float yaw, pitch, roll;

//	Imu.GetMotion6(angles);
//	PrintData(angles, 6);

	if (CommandReceived(cmd))
	{
		ProcessCommands(cmd);
	}

//	int a0 = ReadPinA0(); // Factors debounce
//	int a1 = analogRead(A1); // Used to set Kp

	if (Imu.GetYPR(yaw, pitch, roll)) // Interrupts arrive in roughly 10000 microsec ~ 100HZ.
	{
		Now = millis();
	//	SERIAL.print("\nNew data arrived in "); SERIAL.print(Now - Before);
		Before = millis();

		QuadState.Yaw = yaw; QuadState.Pitch = pitch; QuadState.Roll= roll;
	//	QuadState.Kp = a1;
		MPUInterruptCounter++;
		cPinger.Ping();
		bIsPIDSetup = bIsKpSet & bIsKiSet & bIsKdSet & bIsYawKpSet & bIsYawKiSet & bIsYawKdSet;
		if (bIsPIDSetup)
		{
			//kp = map_i(a1, 0, 512, 0, 100);
			RollCntrl.SetSpeed(QuadState.Speed);
			QuadState.PID_Roll 	= RollCntrl.Compute(roll);
			PitchCntrl.SetSpeed(QuadState.Speed);
			QuadState.PID_Pitch = PitchCntrl.Compute(pitch);
			YawCntrl.SetSpeed(QuadState.Speed);
			QuadState.PID_Yaw 	= YawCntrl.Compute(yaw);

			if (QuadState.bMotorToggle)
			{
				MotorFL.Run(QuadState.Speed - QuadState.PID_Roll  + QuadState.PID_Yaw);
				MotorBL.Run(QuadState.Speed + QuadState.PID_Pitch - QuadState.PID_Yaw);
				MotorFR.Run(QuadState.Speed - QuadState.PID_Pitch - QuadState.PID_Yaw);
				MotorBR.Run(QuadState.Speed + QuadState.PID_Roll  + QuadState.PID_Yaw);
			}
		}
	}

//	PrintData(angles, 6);

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
