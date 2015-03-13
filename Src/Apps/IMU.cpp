/**************************************************************************

Filename    :   IMU.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "IMU.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "quadcopter.h"
#include "SoftwareSerial.h"
#include "SerialDef.h"
#include "ErrorsDef.h"

extern ExceptionMgr cExceptionMgr;
extern volatile bool MPUInterrupt = false; // indicates whether MPU interrupt pin has gone high

void DMPDataReady()
{
	MPUInterrupt = true;
}

IMU::IMU()
{  
	MPUInterrupt = false;
	accelgyro = new MPU6050();
	RADIANS_TO_DEGREES = 180/M_PI;
}

void PrintMotion6Data(int ax, int ay, int az, int gx, int gy, int gz)
{
	SERIAL.print(ax); SERIAL.print(" "); SERIAL.print(ay); SERIAL.print(" "); SERIAL.print(az);
	SERIAL.print(gx); SERIAL.print(" "); SERIAL.print(gy); SERIAL.print(" "); SERIAL.print(gz);
	SERIAL.print("\n");
}

void IMU::CalculateOffsets(uint8_t gyroSamplingRate, int& gXOffset,
		int& gYOffset, int& gZOffset, int& aXOffset, int& aYOffset,
		int& aZOffset)
{
	int numReadings = 0;
	long base_x_gyro = 0;
	long base_y_gyro = 0;
	long base_z_gyro = 0;
	long base_x_accel = 0;
	long base_y_accel = 0;
	long base_z_accel = 0;

	int16_t ax, ay, az, gx, gy, gz;

	delay(100);
	// Discard the first few reading
	unsigned long before = millis();
	unsigned long now;
	do
	{
		accelgyro->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		now = millis();
	}
	while(now - before < 1000/(gyroSamplingRate+1)); // ignore 1000 frames of data

	before = millis();

	do
	{
		accelgyro->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	//	PrintMotion6Data(ax, ay, az, gx, gy, gz);
		base_x_gyro += gx;
		base_y_gyro += gy;
		base_z_gyro += gz;
		base_x_accel += ax;
		base_y_accel += ay;
		base_z_accel += az;
		numReadings++;
		now = millis();
	}
	while(now - before < 1000/(gyroSamplingRate+1)); // collect average over 1000 frames of data
	base_x_gyro /= numReadings; 	gXOffset = base_x_gyro;
	base_y_gyro /= numReadings;		gYOffset = base_y_gyro;
	base_z_gyro /= numReadings;		gZOffset = base_z_gyro;
	base_x_accel /= numReadings;	aXOffset = base_x_accel;
	base_y_accel /= numReadings;	aYOffset = base_y_accel;
	base_z_accel /= numReadings;	aZOffset = base_z_accel;

}

void IMU::Init()
{
	/*
	 * 1. GyroRange = 1000 degrees/seconds
	 * 2. AccelRange = +-2g
	 * 3. Digital Low Pass Filter Mode: 3
	 * 4. GyroRate: Corresponds to a sample rate of 500Hz.
	 */

	IMUInitParams IMUInitParams(MPU6050_GYRO_FS_1000, MPU6050_ACCEL_FS_2, 3, 1);
	// Must Reset the MPU in the beginning
	accelgyro->reset();
	// Give the reset some time to work out
	delay(50);
	// Sleep enabled must be set to false BEFORE configuration parameters are modified
	accelgyro->setSleepEnabled(false);
	delay(1);
	// Not fully sure what this does, but Jeff's code uses it and it does no harm
	accelgyro->setClockSource(MPU6050_CLOCK_PLL_XGYRO);
	delay(1);
	accelgyro->setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	delay(1);
	accelgyro->setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
	delay(1);
	accelgyro->setDLPFMode(3);  //Set Low Pass filter
	delay(1);
	accelgyro->setRate(1);
	uint8_t dlpfMode = accelgyro->getDLPFMode();
	SERIAL.println("DLPFMode = ");
	SERIAL.println(dlpfMode);

	uint8_t gyroSampleRate = accelgyro->getRate();
	SERIAL.println("gyro sample rate = "); SERIAL.print(gyroSampleRate); SERIAL.print("\n");

	// Check connection
	SERIAL.println("Testing device connections...");
	SERIAL.println(accelgyro->testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
	// attachInterrupt(0, DMPDataReady, RISING);
	// get default full scale value of gyro - may have changed from default
	// function call returns values between 0 and 3
	uint8_t READ_FS_SEL = accelgyro->getFullScaleGyroRange();
	SERIAL.print("FS_SEL = ");
	SERIAL.println(READ_FS_SEL);
	GYRO_FACTOR = 131.0 / (READ_FS_SEL + 1);
	gXOffset = 0;
	gYOffset = 0;
	gZOffset = 0;
	// get default full scale value of accelerometer - may not be default value.
	// Accelerometer scale factor doesn't reall matter as it divides out
	uint8_t READ_AFS_SEL = accelgyro->getFullScaleAccelRange();
	SERIAL.print("AFS_SEL = ");
	SERIAL.println(READ_AFS_SEL);
	delay(100); // Wait for sensor to stabilize
/*
	accelgyro->getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);  //Set Starting angles
	accelgyro->setDLPFMode(3);  //Set Low Pass filter

	accXangle = (atan2(accX,accZ)+PI)*RAD_TO_DEG;
	accYangle = (atan2(accY,accZ)+PI)*RAD_TO_DEG; //400
	float x = M_PI;
	//kalmanX.setAngle(accXangle); 
	//kalmanY.setAngle(accYangle);
	//kalmanZ.setAngle(accZangle);

	gyroXangle = accXangle;
	gyroYangle = accYangle;
	gyroZangle = 0;

	alpha_gyro = 0.995; 
	c = (1-alpha_gyro)*1;     
	compAngleX = accXangle;   
	compAngleY = accYangle;
	compAngleX0 = accXangle;   

	//Gyro Calibration: Rough guess of the gyro drift at startup
	float n = 200;

	float sX = 0.0;
	float sY = 0.0;
	float sZ = 0.0;	

	for (int i = 0; i < n; i++)
	{
		accelgyro->getRotation(&gyroX, &gyroY, &gyroZ);
		sX += accelgyro->getRotationX();
		sY += accelgyro->getRotationY();
		sZ += accelgyro->getRotationZ();
	}

	gyroXoffset = sX/n;
	gyroYoffset = sY/n;
	gyroZoffset = sZ/n;

	j=0;
	*/
}

bool IMU::IntegrateGyro(float& yaw, float& pitch, float& roll, float& yaw_omega, float& pitch_omega, float& roll_omega)
{
	// Set the full scale range of the gyro
	uint8_t FS_SEL = 0;
	accelgyro->getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ); //Set Starting angles
	pitch_omega 	= gyroX;
	roll_omega 		= gyroY;
	yaw_omega		= gyroZ;

	//ACCEL_FACTOR = 16384.0/(AFS_SEL + 1);

	// Remove offsets and scale gyro data
	gyroX = (gyroX - gXOffset) / GYRO_FACTOR;
	gyroY = (gyroY - gYOffset) / GYRO_FACTOR;
	gyroZ = (gyroZ - gZOffset) / GYRO_FACTOR;

	float accelAngleY = atan(-(float)accX / (float)accZ) * RADIANS_TO_DEGREES; //atan(-1*accX/sqrt(pow(accY,2) + pow(accZ,2)))*RADIANS_TO_DEGREES;
	float accelAngleX = atan((float)accY / (float)accZ) * RADIANS_TO_DEGREES; // atan(accY/sqrt(pow(accX,2) + pow(accZ,2)))*RADIANS_TO_DEGREES;
	float accelAngleZ = 0;
	unsigned long now = millis();
	// Compute the (filtered) gyro angles
	float dt = (now - Before) / 1000.0;
	float gyroAngleX = gyroX * dt + fLastGyroAngleX;
	float gyroAngleY = gyroY * dt + fLastGyroAngleY;
	float gyroAngleZ = gyroZ * dt + fLastGyroAngleZ;

	// Compute the drifting gyro angles
	float unfilteredGyroAngleX = gyroX * dt + fLastGyroAngleX;
	float unfilteredGyroAngleY = gyroY * dt + fLastGyroAngleY;
	float unfilteredGyroAngleZ = gyroZ * dt + fLastGyroAngleZ;

	// Apply the complementary filter to figure out the change in angle - choice of alpha is
	// estimated now.  Alpha depends on the sampling rate...
	const float alpha = 0.96;
	float angleX = alpha * gyroAngleX + (1.0 - alpha) * accelAngleX;
	float angleY = alpha * gyroAngleY + (1.0 - alpha) * accelAngleY;
	float angleZ = gyroAngleZ;  //Accelerometer doesn't give z-angle
	fLastGyroAngleX = angleX;
	fLastGyroAngleY = angleY;
	fLastGyroAngleZ = angleZ;
	pitch 			= fLastGyroAngleX;
	roll 			= fLastGyroAngleY;
	yaw 			= fLastGyroAngleZ;
	Before = now;
	return true;
}

bool IMU::DMPInit()
{

	DMPReady = false;
    // load and configure the DMP
    SERIAL.println(F("Initializing DMP..."));
    bool devStatus = accelgyro->dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
 //   mpu.setXGyroOffset(220);
 //   mpu.setYGyroOffset(76);
 //   mpu.setZGyroOffset(-85);
 //   mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        SERIAL.println(F("Enabling DMP..."));
        accelgyro->setDMPEnabled(true);
        // enable Arduino interrupt detection

        attachInterrupt(0, DMPDataReady, RISING);
        // get expected DMP packet size for later comparison
        PacketSize = accelgyro->dmpGetFIFOPacketSize();
        SERIAL.println("Packet Length = "); SERIAL.print(PacketSize); Serial.print("z");
        delay(50);
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        SERIAL.println(F("\nDMP ready! Waiting for first interrupt..."));
        DMPReady = true;
    }
    return DMPReady;
}

bool IMU::GetMotion6(float angles[])
{
	accelgyro->getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);  //400µs
	angles[0] = accX; angles[1] = accY; angles[2] = accZ; angles[3] = gyroZ; angles[4] = gyroX; angles[5] = gyroY;
	return true;
}

bool IMU::GetYPR(float& yaw, float& pitch, float& roll)
{

	int fifoCount;
	if (MPUInterrupt)
	{
		MPUInterrupt = false;
		uint8_t mpuIntStatus = accelgyro->getIntStatus();

		// get current FIFO count
		fifoCount = accelgyro->getFIFOCount();

		// check for overflow (this should never happen unless our code is too inefficient)
		if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
			// reset so we can continue cleanly
			accelgyro->resetFIFO();
			SERIAL.println(F("FIFO overflow!"));
			// In case of FIFO overflow, just send the previously recorded yaw/pitch/roll
			yaw = YPR[0];
			pitch = YPR[1];
			roll = YPR[2];
			// otherwise, check for DMP data ready interrupt (this should happen freQuently)
		}
		else if (mpuIntStatus & 0x02)
		{
			// wait for correct available data length, should be a VERY short wait
			while (fifoCount < PacketSize)
				fifoCount = accelgyro->getFIFOCount();

			accelgyro->getFIFOBytes(FIFOBuffer, PacketSize);
			// Read all packets in the FIFO buffer
			/*
		while (fifoCount >= PacketSize)
		{
			// read a packet from FIFO
			accelgyro->getFIFOBytes(FIFOBuffer, PacketSize);
			fifoCount = fifoCount - PacketSize;
		}*/
			accelgyro->dmpGetQuaternion(&Q, FIFOBuffer);
			accelgyro->dmpGetGravity(&Gravity, &Q);
			accelgyro->dmpGetYawPitchRoll(YPR, &Q, &Gravity);
			yaw = YPR[0];
			pitch = YPR[1];
			roll = YPR[2];
			// TODO: perhaps return false if DoSanityCheck fails
			//	DoSanityCheck(yaw, pitch, roll);
			return true;
		}
	}
	return false;
}

bool IMU::DoSanityCheck(float& yaw, float& pitch, float& roll)
{
	// The MPU6050 sometimes gives out a burst of bad values. Hopefully this check will detect if bad
	// values are received and prevent crazy orientation data to be sent to the attitude control algorithm
	// We check for bad values by asserting that the current measurements shouldn't differ from the previous
	// measurements by
	if (fabs(LastYaw - yaw) > 0.2 * fabs(LastYaw)
			|| fabs(LastPitch - pitch) > 0.2 * fabs(LastPitch)
			|| fabs(LastRoll - roll) > 0.2 * fabs(LastRoll))
	{
		cExceptionMgr.SetException(BAD_MPU_DATA);
		LastYaw = yaw; LastPitch = pitch; LastRoll = roll;
		return false;
	}
	LastYaw = yaw; LastPitch = pitch; LastRoll = roll;
	return true;
}

bool IMU::processAngles(float angles[],float rates[])
{			
	accelgyro->getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);  //400µs
		
	//Filter
	accXf = filterX.update(accX);
	accYf = filterY.update(accY);
	accZf = filterZ.update(accZ);
	
	// Angular rates provided by gyro
	gyroXrate = (float) (gyroX-gyroXoffset)/131.0; //140 µsec on Arduino MEGA
	gyroYrate = -((float) (gyroY-gyroYoffset)/131.0);
	gyroZrate = ((float) (gyroZ-gyroZoffset)/131.0);

	//Angle provided by accelerometer
	//Time taken: 400 µsec on Arduino MEGA
	accYangle = (atan2(accXf,accZf)+PI)*RAD_TO_DEG; //
	accXangle = (atan2(accYf,accZf)+PI)*RAD_TO_DEG;
	
	//Integration of gyro rates to get the angles
	gyroXangle += gyroXrate*(float)(micros()-timer)/1000000;
	gyroYangle += gyroYrate*(float)(micros()-timer)/1000000;
	gyroZangle += gyroZrate*(float)(micros()-timer)/1000000;
	
	//Angle calculation through Complementary filter
	//Time taken: 200 µsec on Arduino MEGA
	dt = (float)(micros()-timer)/1000000.0;
	compAngleX = alpha_gyro*(compAngleX+(gyroXrate*dt))   +   c*accXangle; 
	compAngleY = alpha_gyro*(compAngleY+(gyroYrate*dt))  +   c*accYangle;
	//compAngleX0 = 0.997*(compAngleX0+(gyroXrate*(float)(micros()-timer)/1000000))   +   (1-0.997)*accXangle; 

	timer = micros(); 
	
	//45 deg rotation for roll and pitch (depending how your IMU is fixed to your quad)
	angles[0]=  -rac22* compAngleX + rac22*compAngleY + ROLL_OFFSET;
	angles[1]=  -rac22* compAngleX - rac22*compAngleY +2*rac22*PI*RAD_TO_DEG + PITCH_OFFSET;
	angles[2]=  gyroZangle;

	rates[0]=   -  rac22* gyroXrate + rac22*gyroYrate;
	rates[1]= - rac22* gyroXrate - rac22*gyroYrate;
	rates[2]=  gyroZrate;
		
	//////* Print Data  for vibration measurements*/ //Todo: Extract function
	//switch (j)
	  //{

	////	Frequency print
	  //case 1: 
		   //dtostrf(compAngleX - 180  ,6,2,StrAnglesvib);
		    //SERIAL.print(StrAnglesvib);
		   //break;
	  //case 2:
		   //SERIAL.print("  ");
		   //break;
	  //case 3:
	  		//dtostrf(gyroXangle -180,6,2,StrAnglesvib);	
			//SERIAL.println(StrAnglesvib);
		   //j=0;
		   //break;
	  //}	   
	//j++;

	if ( abs(angles[0]) < ROLL_MAX_IMU && abs(angles[1]) < PITCH_MAX_IMU  )
	{
	return true;
	}
	else
	{
	return false;
	}
}
