/*
  IMU.cpp - Library to use the IMU 
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
#include "IMU.h"
#include "MPU6050_6Axis_MotionApps20.h"

extern volatile bool MPUInterrupt = false; // indicates whether MPU interrupt pin has gone high

void DMPDataReady()
{
	MPUInterrupt = true;
}

IMU::IMU()
{  
	MPUInterrupt = false;
	accelgyro = new MPU6050();
}

void IMU::Init()
{
	// Initialize device
	Serial.println("Initializing I2C devices...");
	accelgyro->initialize();

	//-885	-2964	1254	-4	-45	-34

	accelgyro->setXAccelOffset(-885);
	accelgyro->setYAccelOffset(-2964);
	accelgyro->setZAccelOffset(1254);
	accelgyro->setXGyroOffset(-4);
	accelgyro->setYGyroOffset(-45);
	accelgyro->setZGyroOffset(-34);

	// Check connection
	Serial.println("Testing device connections...");
	Serial.println(accelgyro->testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

	delay(100); // Wait for sensor to stabilize

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
}

bool IMU::DMPInit()
{

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    bool devStatus = accelgyro->dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
 //   mpu.setXGyroOffset(220);
 //   mpu.setYGyroOffset(76);
 //   mpu.setZGyroOffset(-85);
 //   mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        accelgyro->setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, DMPDataReady, RISING);
        // get expected DMP packet size for later comparison
        PacketSize = accelgyro->dmpGetFIFOPacketSize();
        Serial.println("Packet Size = "); Serial.print(PacketSize);
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("\nDMP ready! Waiting for first interrupt..."));
        DMPReady = true;
    }
}

bool IMU::GetMotion6(float angles[])
{
	accelgyro->getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);  //400µs
	angles[0] = accX; angles[1] = accY; angles[2] = accZ; angles[3] = gyroX; angles[4] = gyroY; angles[5] = gyroZ;
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
			Serial.println(F("FIFO overflow!"));

			// otherwise, check for DMP data ready interrupt (this should happen freQuently)
		}
		else if (mpuIntStatus & 0x02)
		{
			// wait for correct available data length, should be a VERY short wait
			while (fifoCount < PacketSize)
				fifoCount = accelgyro->getFIFOCount();

			// read a packet from FIFO
			accelgyro->getFIFOBytes(FIFOBuffer, PacketSize);
			// track FIFO count here in case there is > 1 packet available
			// (this lets us immediately read more without waiting for an interrupt)
			fifoCount -= PacketSize;
			accelgyro->dmpGetQuaternion(&Q, FIFOBuffer);
			accelgyro->dmpGetGravity(&Gravity, &Q);
			accelgyro->dmpGetYawPitchRoll(YPR, &Q, &Gravity);
			yaw = YPR[0];
			pitch = YPR[1];
			roll = YPR[2];
			return true;
		}
	}
	return false;
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
		    //Serial.print(StrAnglesvib); 
		   //break;
	  //case 2:
		   //Serial.print("  ");
		   //break;
	  //case 3:
	  		//dtostrf(gyroXangle -180,6,2,StrAnglesvib);	
			//Serial.println(StrAnglesvib);
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
