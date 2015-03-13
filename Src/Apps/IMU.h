/**************************************************************************

Filename    :   IMU.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#ifndef IMU_h
#define IMU_h

#include <Arduino.h>
//#include "MPU6050.h"
#include "Utils.h"
#include "Kalman.h"
#include "Filter.h"
#include "helper_3dmath.h"

class MPU6050;

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#define  ROLL_MAX_IMU  30
#define  PITCH_MAX_IMU 30

#define  ROLL_OFFSET -0.16
#define  PITCH_OFFSET 2.10
#define  YAW_OFFSET 0

extern volatile bool MPUInterrupt; // indicates whether MPU interrupt pin has gone high

void DMPDataReady();

struct IMUInitParams
{
	IMUInitParams(uint8_t gyroRange, uint8_t accelRange, uint8_t dlpfMode, uint8_t sampleRate)
	{
		GyroRange 		= gyroRange;
		AccelRange 		= accelRange;
		DLPFMode		= dlpfMode;
		GyroSampleRate 	= sampleRate;
	}

	uint8_t GyroRange;
	uint8_t AccelRange;
	uint8_t	DLPFMode;
	uint8_t	GyroSampleRate;
};

class IMU
{

  public:

  IMU();
  void Init();
  bool DMPInit();
  void CalculateOffsets(uint8_t gyroSamplingRate, int& gXOffset,
  		int& gyOffset, int& gzOffset, int& aXOffset, int& aYOffset,
  		int& aZOffset);
  bool processAngles(float angles[],float rates[] );
  bool GetMotion6(float angles[]);
  bool IntegrateGyro(float& yaw, float& pitch, float& roll, float& yaw_omega, float& pitch_omega, float& roll_omega);
  bool GetYPR(float& yaw, float& pitch, float& roll);
  bool DoSanityCheck(float& yaw, float& pitch, float& roll);

  int 	gXOffset, gYOffset, gZOffset, aXOffset, aYOffset, aZOffset;
  float fLastGyroAngleX, fLastGyroAngleY, fLastGyroAngleZ;
  float GYRO_FACTOR;
  float RADIANS_TO_DEGREES;

  private:

	//Kalman kalmanX; // Create the Kalman instances
	//Kalman kalmanY;
	//Kalman kalmanZ;
	/* IMU Data */
	int16_t 	accX, accY, accZ;
	int16_t		gyroX, gyroY, gyroZ;
	uint8_t 	FIFOBuffer[64];
	float		YPR[3];
	VectorFloat	Gravity;
	Quaternion 	Q;
	unsigned long Before;

	int		PacketSize;
	bool 	DMPReady;
	float 	accXangle, accYangle, accZangle; // Angle calculate using the accelerometer
	float 	gyroXangle, gyroYangle, gyroZangle; // Angle calculate using the gyro
	float 	kalAngleX, kalAngleY, kalAngleZ; // Calculate the angle using a Kalman filter
	float 	compAngleX, compAngleY, compAngleX0;
	float 	LastYaw;
	float 	LastPitch;
	float	LastRoll;
	// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
	// AD0 high = 0x69
	MPU6050* accelgyro; //MPU6050 accelgyro(0x69); // <-- use for AD0 high
	uint32_t timer;
	float gyroXoffset, gyroYoffset, gyroZoffset;
	
	float gyroXrate ;
	float gyroYrate ;
	float gyroZrate;
	
    //float xv[NZEROS+1], yv[NPOLES+1];
     //float xv1[NZEROS+1], yv1[NPOLES+1];  
	float accXf;
	float accYf;
	float accZf;
	
	Filter filterX;
	Filter filterY;	
	Filter filterZ;

	
	
	float alpha_gyro;
    float c;
    float dt;
	char StrAnglesvib[7];
	int j;
};

#endif
