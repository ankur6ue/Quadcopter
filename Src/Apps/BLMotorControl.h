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

#ifndef BLMOTORCONTROL
#define BLMOTORCONTROL

// Need the Servo library
#include <Arduino.h>
#include <Servo.h>

#define NUMMOTORS 4

typedef struct MotorDef
{
    Servo   Motor;
    int     Pin;   // Indicates the Pin this motor is connected to
    int 	Speed;
    int 	Step;
};

enum MotorId
{
	FR = 0,
	BL,
	BR,
	FL
};

#define ESC_HIGH_DEFAULT 180 // Note that the servo lib maxes out at 180, so any value higher is capped at 180
#define ESC_LOW_DEFAULT 20
//#define ESC_HIGH_DEFAULT MAX_PULSE_WIDTH // Note that the servo lib maxes out at 200, so any value higher is capped at 180
//#define ESC_LOW_DEFAULT MIN_PULSE_WIDTH
// Stores the settings for all ESC. This can be made specific to each ESC, but that's not needed
// for a quadcopter project
typedef struct ESCSettingsDef
{
  int Low;
  int High;
};

class BLMotors
{
public:
	BLMotors();
	void Init();
	void Reset();
	void Test(int speed);
	void Run(MotorId id, int speed);

	MotorDef Motors[NUMMOTORS];
	ESCSettingsDef ESCSettings;

};
#endif




