/*
Copyright (c) <2014> <Ankur Mohan>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Note: Select the mode (program mode/throttle setting mode/normal mode), run this code, 
then power on the ESC. Don't power on the ESC before this code is executing, otherwise
the ESC will see random values on the PWM pin.
*/

// Need the Servo library
#include "Utils.h"
#include "BLMotorControl.h"

BLMotors::BLMotors()
{
	// Required for I/O from Serial monitor
	// Attach motors to pins

    Motors[0].Pin =  11;
	Motors[1].Pin =  10;
	Motors[2].Pin =  5;
	Motors[3].Pin =  6;

	ESCSettings.High = ESC_HIGH_DEFAULT;
	ESCSettings.Low = ESC_LOW_DEFAULT;
}

void BLMotors::Init()
{
	for(int i = 0; i < NUMMOTORS; i++)
		{
		  int pin = Motors[i].Pin;
		  Motors[i].Motor.attach(pin);
		  Motors[i].Speed 	= ESCSettings.Low;
		  Motors[i].Step	= 1;
		}

		Serial.println("Setting the motors in normal mode");

		for (int i = 0; i < NUMMOTORS; i++)
		{
		  Motors[i].Motor.write(ESCSettings.Low);
		}
		delay(500);
}

void BLMotors::Run(MotorId motorId, int _speed)
{
	if (motorId < 0 || motorId > 3) return;

	int speed = constrain_i(_speed, ESCSettings.Low, ESCSettings.High);
	Motors[motorId].Speed = speed;
	Motors[motorId].Motor.write(speed);
}



