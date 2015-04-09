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
#include "MatrixOps.h"
#include "AccelSampler.h"

extern ExceptionMgr cExceptionMgr;
extern AccelSampler cAccelSampler;
extern volatile bool MPUInterrupt = false; // indicates whether MPU interrupt pin has gone high

void DMPDataReady()
{
	MPUInterrupt = true;
}

IMU::IMU()
{  
	MPUInterrupt = false;
	accelgyro = new MPU6050();
	RADIANS_TO_DEGREES = 180.0/M_PI;
	DEGREES_TO_RADIANS = 1.0/RADIANS_TO_DEGREES;
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
	// longs are 32bit integers on Arduino mega, so good for storing sums
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
	// If gyroSamplingRate = 1, that means we are sampling the sensors every 500hz, which is 2ms
	// between every two samples. So to be safe, we sample every 3 ms to collect new samples every time
	// lets discard the first 200 samples
	int timeInterval = 1+gyroSamplingRate+1; //second plus one is provide some margin
	int numSamples = 0;
	do
	{
		accelgyro->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		delay(timeInterval);
		numSamples++;
	}
	while(numSamples < 200); // ignore 200 frames of data

	before = millis();
	// Now collect data for offset calculation
	numSamples = 0;
	accelgyro->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	int yawBefore = gz;
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
		numSamples++;
		delay(timeInterval);
	}
	while(numSamples < 1200); // collect average over 400 frames of data
	int yawNow = gz;
	now = millis();
	SERIAL.print("yawBefore = ");
	SERIAL.println(yawBefore);
	SERIAL.print("yawNow = ");
	SERIAL.println(yawNow);
	gXOffset = (int)base_x_gyro/numSamples;
	gYOffset = (int)base_y_gyro/numSamples;
	gZOffset = (int)base_z_gyro/numSamples;
	aXOffset = (int)base_x_accel/numSamples;
	aYOffset = (int)base_y_accel/numSamples;
	aZOffset = (int)base_z_accel/numSamples;

	// Calculate yaw drift estimate
	YawDrift = (int)(yawNow - yawBefore)*1000/(now - before);
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
	accelgyro->setFullScaleGyroRange(MPU6050_GYRO_FS_1000); // Units: degrees/second
	delay(1);
	accelgyro->setFullScaleAccelRange(MPU6050_ACCEL_FS_2); //+-2g
	delay(1);
	accelgyro->setDLPFMode(4);  //Set Low Pass filter
	delay(1);
	accelgyro->setRate(1); // Corresponds to a sampling rate of 500 hz
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
	GYRO_FACTOR = 131.0 / (READ_FS_SEL + 1);

	// get default full scale value of accelerometer - may not be default value.
	// Accelerometer scale factor doesn't really matter as it divides out
	uint8_t READ_AFS_SEL = accelgyro->getFullScaleAccelRange();
	ACCEL_FACTOR = 16384.0/(READ_AFS_SEL + 1);
	SERIAL.print("GYRO_FACTOR = ");
	SERIAL.println(GYRO_FACTOR);
	SERIAL.print("ACCEL_FACTOR = ");
	SERIAL.println(ACCEL_FACTOR);
	SERIAL.print("YawDrift = ");
	SERIAL.println(YawDrift);
	// Calculating the offsets is very important. Without doing so, gyro/accel measurements will
	// experience significant drift.
	CalculateOffsets(gyroSampleRate, gXOffset, gYOffset, gZOffset, aXOffset, aYOffset, aZOffset);

	SERIAL.println("gXOffset, gYOffset, gZOffset, aXOffset, aYOffset, aZOffset");
	SERIAL.print(gXOffset), SERIAL.print(" ");
	SERIAL.print(gYOffset), SERIAL.print(" ");
	SERIAL.print(gZOffset), SERIAL.print(" ");
	SERIAL.print(aXOffset), SERIAL.print(" ");
	SERIAL.print(aYOffset), SERIAL.print(" ");
	SERIAL.print(aZOffset), SERIAL.print(" ");

	Reset();
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
static unsigned long tmp = 0;
bool IMU::IntegrateGyro(float& yaw, float& pitch, float& roll, float& yaw_omega, float& pitch_omega, float& roll_omega,
		float& yaw_accel, float& pitch_accel, float& roll_accel, float& yaw2, float& pitch2, float& roll2)
{
	unsigned long now = millis();
	float dt = (now - Before) / 1000.0;
	accelgyro->getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ); //Set Starting angles
	// Remove offsets and scale gyro data
	fgyroX = (gyroX - gXOffset) / GYRO_FACTOR;
	fgyroY = (gyroY - gYOffset) / GYRO_FACTOR;
	fgyroZ = (gyroZ - gZOffset) / GYRO_FACTOR;

	cAccelSampler.GetAccelAvg(faccX, faccY, faccZ);
	// Remove offsets and scale accelerometer data
	//faccX = (accX /*- aXOffset*/) / ACCEL_FACTOR;
	//faccY = (accY /*- aYOffset)*/) / ACCEL_FACTOR;
	//faccZ = (accZ) / ACCEL_FACTOR;

	// Note that we must use scaled angular velocities and accelerations for PID loop
	// After the scale adjustment is applied, the resulting values are in physical units (degrees/sec etc)
	// and independent of gyro settings such as gyro/accel range etc. If unscaled values are used,
	// the magnitude of the unscaled values will be different and PID coefficients set for a certain
	// scale will likely not work.
	pitch_omega 	= fgyroY; // Means angle of rotation about Y axis
	roll_omega 		= fgyroX;
	yaw_omega		= fgyroZ;
	pitch_accel		= faccX; // Means acceleration along x axis
	roll_accel		= faccY;
	yaw_accel		= faccZ;

	// Angle of rotation about Y axis computed from the acceleration vector
//	float accelAngleY = atan(-faccX / sqrt(faccZ*faccZ + faccY*faccY)) * RADIANS_TO_DEGREES;
	// Angle of rotation about X Axis computer from the acceleration vector
//	float accelAngleX = atan(faccY / faccZ) * RADIANS_TO_DEGREES;
	// Acceleration doesn't give info about rotation around Z
//	float accelAngleZ = 0;

//	float gyroAngleX = fgyroX * dt + fLastGyroAngleX;
//	float gyroAngleY = fgyroY * dt + fLastGyroAngleY;
//	float gyroAngleZ = fgyroZ * dt + fLastGyroAngleZ;
	UpdateMatrix(fgyroX, fgyroY, fgyroZ, faccX, faccY, faccZ, dt);
	Rot.ToEuler(yaw, pitch, roll);
	// Convert to degree
	yaw = yaw*RADIANS_TO_DEGREES;
	pitch = pitch*RADIANS_TO_DEGREES;
	roll = roll*RADIANS_TO_DEGREES;
	// Apply the complementary filter to figure out the change in angle - choice of alpha is
	// estimated now.  Alpha depends on the sampling rate...
	const float alpha = 0.95;
//	float angleX = alpha * gyroAngleX + (1.0 - alpha) * accelAngleX;
//	float angleY = alpha * gyroAngleY + (1.0 - alpha) * accelAngleY;
//	float angleZ = gyroAngleZ;  //Accelerometer doesn't give z-angle

//	fLastGyroAngleX = angleX;
//	fLastGyroAngleY = angleY;
//	fLastGyroAngleZ = angleZ;
//	pitch2 			= fLastGyroAngleY;
//	roll2 			= fLastGyroAngleX;
//	yaw2 			= fLastGyroAngleZ;
	Before = now;
	return true;
}

void IMU::UpdateMatrix(float fgyroX, float fgyroY, float fgyroZ, float faccX,
		float faccY, float faccZ, float dt)
{
	// Apply Roll Pitch Correction using Accelerometer data
	float Kp = -QuadState.Alpha;
	float Ki = 0;
	VectorF _r3 = Rot.GetRow(3);
	VectorF Acc(faccX, faccY, faccZ);
	VectorF RPCorrection = _r3.Cross(Acc);
	VectorF RPCorrection_K = RPCorrection.Dot(Kp);
//	RPCorrection_I = RPCorrection_I + RPCorrection.Dot(dt*Ki);
	VectorF gyroCorrected = VectorF(fgyroX*DEGREES_TO_RADIANS, fgyroY*DEGREES_TO_RADIANS, fgyroZ*DEGREES_TO_RADIANS)
			+ RPCorrection_K /*+ RPCorrection_I*/;
	// Omega Correction
	// Gyro reports results in degrees per second.
	VectorF r1(1, -gyroCorrected.z*dt, gyroCorrected.y*dt);
	VectorF r2(gyroCorrected.z*dt, 1, -gyroCorrected.x*dt);
	VectorF r3(-gyroCorrected.y*dt, gyroCorrected.x*dt, 1);
	Matrix3D omega_c(r1, r2, r3);

	Rot = Rot.Multiply(omega_c);
	// Renormalize Rot
	Rot.Renormalize();

}

void IMU::Reset()
{
	// X/Y angles should be reset to the orientation obtained from accel?
	fLastGyroAngleZ = 0;
	fLastGyroAngleX = 0;
	fLastGyroAngleY = 0;
	RPCorrection_I.SetZero();
	// Reset Rot
	Rot.Reset();
	Before = millis();
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

bool IMU::GetAccel(float& faccX, float& faccY, float& faccZ)
{
	int16_t accX, accY, accZ;
	accelgyro->getAcceleration(&accX, &accY, &accZ);
	faccX = accX/ACCEL_FACTOR;
	faccY = accY/ACCEL_FACTOR;
	faccZ = accZ/ACCEL_FACTOR;
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

