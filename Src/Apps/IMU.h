/*
  IMU.h - Library to use the IMU 
  Created by Romain Goussault <romain.goussault@gmail.com>
  
  This program is free software: you can redistribute it and/or modify 
  it under the terms of the GNU General Public License as published by 
  the Free Software Foundation, either version 3 of the License, or 
  (at your option) any later version. 

  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
  GNU General Public License for more details. 

  You should have received a copy of the GNU General Public License 
  afloat with this program. If not, see <http://www.gnu.org/licenses/>. 
*/
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

class IMU
{

  public:

  IMU();
  void Init();
  bool DMPInit();
  bool processAngles(float angles[],float rates[] );
  bool GetMotion6(float angles[]);
  bool GetYPR(float& yaw, float& pitch, float& roll);

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

	int		PacketSize;
	bool 	DMPReady;
	float 	accXangle, accYangle, accZangle; // Angle calculate using the accelerometer
	float 	gyroXangle, gyroYangle, gyroZangle; // Angle calculate using the gyro
	float 	kalAngleX, kalAngleY, kalAngleZ; // Calculate the angle using a Kalman filter
	float 	compAngleX, compAngleY, compAngleX0;

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
