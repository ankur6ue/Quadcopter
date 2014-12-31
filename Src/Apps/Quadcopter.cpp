

#include "IMU.h"
#include "PIDControl.h"
#include "BLMotorControl.h"
#include "Quadcopter.h"
#include "Pinger.h"
#include "Callbacks.h"
#include "CommandCtrl.h"
#include "ReadAnalogPin.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here

IMU Imu;

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

    // initialize SERIAL communication
    Serial.begin(115200);
    SSerial.begin(115200); // need this baudrate otherwise FIFO overflow will occur. WE are not sending data fast enough
    SERIAL.print("In Setup");
    ////////////////// MPU Initialization ////////////////
    Imu.Init();
    Imu.DMPInit();
    Motors.Init();
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

MyLog cLog;
Pinger cPinger(100, SendLog, &cLog);


void loop()
{
	float yaw, pitch, roll;
	char currentChar = 0;

//	Imu.GetMotion6(angles);
//	PrintData(angles, 6);
	Command cmd;
	if (CommandReceived(cmd))
	{
		ProcessCommands(cmd);
	}

	int sensorPin0 	= A0;
	int sensorPin1 	= A1;
	int sensorValue0 = ReadPinA0(); // Factors debounce
	int sensorValue1 = analogRead(sensorPin1);

	if (Imu.GetYPR(yaw, pitch, roll)) // Interrupts arrive in roughly 10000 microsec ~ 100HZ.
	{
		now = millis();
	//	SERIAL.print("\nNew data arrived in "); SERIAL.print(now - before);
		before = millis();
		float angles[3] = {yaw, pitch, roll};
	//	cPinger.Execute(angles);
		double ki = 0.1; double kd = 0; double kp = 1;
		if (now - StartupTime > 2000 && !IsPIDSetup)
		{
			RollCntrl.SetSetPoint(0);
			RollCntrl.SetTunings(kp, ki, kd);
			PitchCntrl.SetSetPoint(0);
			PitchCntrl.SetTunings(kp, ki, kd);
			YawCntrl.SetSetPoint(yaw);
			YawCntrl.SetTunings(kp, ki, kd);

			IsPIDSetup = true;
		}
		if (IsPIDSetup)
		{
			kp = map_i(sensorValue1, 0, 512, 0, 100);
			RollCntrl.SetTunings(kp, 0, 0);
			PitchCntrl.SetTunings(kp, 0, 0);
			RollPIDOutput = RollCntrl.Compute(roll);
			PitchPIDOutput = PitchCntrl.Compute(pitch);
			YawPIDOutput = YawCntrl.Compute(yaw);

			if (bMotorToggle && ((now - ESCPoweredTime) > 5000)) // Good to wait for a few seconds after the ESCs have been powered on
			{
				Motors.Run(FL, Speed - RollPIDOutput  /*+ YawPIDOutput*/);
				Motors.Run(BL, Speed + PitchPIDOutput /*+ YawPIDOutput*/);
				Motors.Run(FR, Speed - PitchPIDOutput /*- YawPIDOutput*/);
				Motors.Run(BR, Speed + RollPIDOutput  /*- YawPIDOutput*/);
			}
//			SERIAL.print("\n Kp = ");
//			SERIAL.print(kp);
//			SERIAL.print("PID\t");
//			SERIAL.print(YawPIDOutput);
//			SERIAL.print("\t");
//			SERIAL.print(PitchPIDOutput);
//			SERIAL.print("\t");
//			SERIAL.println(RollPIDOutput);
//			SERIAL.print("Speed\t");
//			SERIAL.print(Speed);
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
