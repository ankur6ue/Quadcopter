

#include "IMU.h"
#include "PIDControl.h"
#include "BLMotorControl.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here

IMU Imu;

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

PIDController 	PitchCntrl;
PIDController	YawCntrl;
PIDController	RollCntrl;
BLMotors		Motors;
int 			StartupTime;
bool			IsPIDSetup = false;
bool			ESCPowered = false;
int				ESCPoweredTime = 0;
int				RollPIDOutput = 0;
int				PitchPIDOutput = 0;
int				YawPIDOutput = 0;
int				Speed = 40;
unsigned long	now;
unsigned long	before;
void setup()
{
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    Serial.begin(115200); // need this baudrate otherwise FIFO overflow will occur. WE are not sending data fast enough

    ////////////////// MPU Initialization ////////////////
    Imu.Init();
  //  Imu.DMPInit();
    Motors.Init();
    StartupTime = millis();

}

void PrintData(float data[], int numElem)
{
	for (int i = 0; i < numElem; i++)
	{
		Serial.print(data[i]); Serial.print(" ");
	}
	Serial.print("\n");
}

void loop()
{
	float angles[6];
	float yaw, pitch, roll;
	char currentChar = 0;

	Imu.GetMotion6(angles);
	PrintData(angles, 6);
	if(Serial.available())
	{
		char currentChar = Serial.read();
		if (currentChar == 'u')
		{
			Speed = Speed + 2;
		}
		if (currentChar == 'd')
		{
			Speed = Speed - 2;
		}
		if (currentChar == 'e')
		{
			Speed = 30;
		}
		if (currentChar == 's')
		{
			Speed = 55;
		}
	}

	int sensorPin0 	= A0;
	int sensorPin1 	= A1;
	int sensorValue0 = analogRead(sensorPin0);
	int sensorValue1 = analogRead(sensorPin1);

//	Serial.print(sensorValue0); Serial.print("\n");
	if (sensorValue0 > 900 && !ESCPowered)
	{
		ESCPoweredTime 	= millis();
		ESCPowered 		= true;
		// Turn on motor after a suitable delay
	}


	if (Imu.GetYPR(yaw, pitch, roll)) // Interrupts arrive in roughly 3920 microsec ~ 255HZ.
	{
		now = millis();
	//	Serial.print("\nNew data arrived in "); Serial.print(now - before);
		before = millis();
		float angles[6];
	//	Imu.GetMotion6(angles);
	//	PrintData(angles, 6);
//		Serial.print("\nypr\t");
//		Serial.print(yaw * 180/M_PI);
//		Serial.print(" ");
//		Serial.print(pitch * 180/M_PI);
//		Serial.print(" ");
//		Serial.println(roll * 180/M_PI);
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

			if (ESCPowered && (now - ESCPoweredTime) > 5000)
			{
				Motors.Run(FR, Speed + RollPIDOutput - PitchPIDOutput /*+ YawPIDOutput*/);
				Motors.Run(BL, Speed - RollPIDOutput + PitchPIDOutput /*+ YawPIDOutput*/);
				Motors.Run(BR, Speed + RollPIDOutput + PitchPIDOutput /*- YawPIDOutput*/);
				Motors.Run(FL, Speed - RollPIDOutput - PitchPIDOutput /*- YawPIDOutput*/);
			}
//			Serial.print("\n Kp = ");
//			Serial.print(kp);
//			Serial.print("\n Roll Cntrl Output = ");
//			Serial.print(RollPIDOutput);
//			Serial.print("\n sensorValue Output = ");
//			Serial.print(sensorValue1);

		}
	}

//	PrintData(angles, 6);

}
