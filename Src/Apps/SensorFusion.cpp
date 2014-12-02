#include "Arduino.h"
#include "MPU6050.h"

int gXOffset = 0, gYOffset = 0, gZOffset = 0, aXOffset = 0, aYOffset = 0, aZOffset = 0;
float fLastGyroAngleX, fLastGyroAngleY, fLastGyroAngleZ;
float fAngleX, fAngleY, fAngleZ;
float GYRO_FACTOR;
float RADIANS_TO_DEGREES = 180/PI;


unsigned long Before = 0;
float AccelAngleY, AccelAngleX, AccelAngleZ;
extern MPU6050 mpu;

//#define UNFILTERED
#define FILTERED

void PrintMotion6Data(int ax, int ay, int az, int gx, int gy, int gz)
{
	Serial.print(ax); Serial.print(" "); Serial.print(ay); Serial.print(" "); Serial.print(az);
	Serial.print(gx); Serial.print(" "); Serial.print(gy); Serial.print(" "); Serial.print(gz);
	Serial.print("\n");
}

void CalculateOffsets(uint8_t gyroSamplingRate)
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
		mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		now = millis();
	}
	while(now - before < 500/(gyroSamplingRate+1)); // ignore 500 frames of data

	before = millis();

	do
	{
		mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
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

bool IntegrateGyro()
{
	// Set the full scale range of the gyro
	uint8_t FS_SEL = 0;
	int16_t 	accX, accY, accZ;
	int16_t		gyroX, gyroY, gyroZ;
	mpu.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);  //Set Starting angles
	unsigned long now = millis();
	float dt =(now - Before)/1000.0;
	Before = now;
	//mpu.setFullScaleGyroRange(FS_SEL);

	// get default full scale value of gyro - may have changed from default
	// function call returns values between 0 and 3
	uint8_t READ_FS_SEL = mpu.getFullScaleGyroRange();
//	Serial.print("FS_SEL = ");
//	Serial.println(READ_FS_SEL);
	GYRO_FACTOR = 131.0/(READ_FS_SEL + 1);


	// get default full scale value of accelerometer - may not be default value.
	// Accelerometer scale factor doesn't reall matter as it divides out
	uint8_t READ_AFS_SEL = mpu.getFullScaleAccelRange();
//	Serial.print("AFS_SEL = ");
//	Serial.println(READ_AFS_SEL);
	//ACCEL_FACTOR = 16384.0/(AFS_SEL + 1);


	// Remove offsets and scale gyro data
	float fgyroX, fgyroY, fgyroZ;

	fgyroX = (gyroX - gXOffset)/GYRO_FACTOR;
	fgyroY = -(gyroY - gYOffset)/GYRO_FACTOR;
	fgyroZ = -(gyroZ - gZOffset)/GYRO_FACTOR;
	accX = accX; // - base_x_accel;
	accY = accY; // - base_y_accel;
	accZ = accZ; // - base_z_accel;

	const double Q_angle = 0.001;

	const double Q_gyroBias = 0.003;

	const double R_angle = 0.03;

	AccelAngleY = atan2(accX, sqrt(pow(accY,2) + pow(accZ,2)))*RADIANS_TO_DEGREES;
	AccelAngleX = atan2(accY, sqrt(pow(accX,2) + pow(accZ,2)))*RADIANS_TO_DEGREES;
	AccelAngleZ = accZ;


#ifdef UNFILTERED
	// Compute the (filtered) gyro angles
	fAngleX = fgyroX*dt + fLastGyroAngleX;
	fAngleY = fgyroY*dt + fLastGyroAngleY;
	fAngleZ = fgyroZ*dt + fLastGyroAngleZ;
#endif

#ifdef FILTERED
	// Apply the complementary filter to figure out the change in angle - choice of alpha is
	// estimated now.  Alpha depends on the sampling rate...
	const float alpha = 0.9;
	float gyroAngleX = fgyroX*dt + fLastGyroAngleX;
	float gyroAngleY = fgyroY*dt + fLastGyroAngleY;
	float gyroAngleZ = fgyroZ*dt + fLastGyroAngleZ;
	fAngleX = alpha*gyroAngleX + (1.0 - alpha)*AccelAngleX;
	fAngleY = alpha*gyroAngleY + (1.0 - alpha)*AccelAngleY;
	fAngleZ = gyroAngleZ;  //Accelerometer doesn't give z-angle
#endif
	fLastGyroAngleX = fAngleX; fLastGyroAngleY = fAngleY; fLastGyroAngleZ = fAngleZ;
	return true;
}

bool GetMotion6(float angles[])
{
	int16_t 	accX, accY, accZ;
	int16_t		gyroX, gyroY, gyroZ;
	mpu.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);  //400µs
	angles[0] = accX; angles[1] = accY; angles[2] = accZ; angles[3] = gyroX; angles[4] = gyroY; angles[5] = gyroZ;
	return true;
}

bool GetIntegratedMotion6(float angles[])
{
	angles[0] = AccelAngleX; angles[1] = AccelAngleY; angles[2] = AccelAngleZ; angles[3] = fAngleX; angles[4] = fAngleY; angles[5] = fAngleZ;
	return true;
}
