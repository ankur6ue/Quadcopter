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
#include <Arduino.h>
#include <Servo.h>

#define NUMMOTORS 4
typedef struct MotorDef
{
    Servo   Motor; 
    int     Pin;   // Indicates the Pin this motor is connected to
};

MotorDef Motors[NUMMOTORS];

// Stores the settings for all ESC. This can be made specific to each ESC, but that's not needed
// for a quadcopter project
typedef struct ESCSettingsDef
{
  int Low;
  int High;
};

ESCSettingsDef ESCSettings; 

//#define PROGRAM_MODE
//#define THROTTLE_MODE
#define NORMAL_MODE

int CurrentSpeed;
int Step = 6;


#define ESC_HIGH_DEFAULT 2000 // Note that the servo lib maxes out at 200, so any value higher is capped at 180
#define ESC_LOW_DEFAULT 700

void setup()
{
	// Required for I/O from Serial monitor
	Serial.begin(115200);
	Serial.println("Setup: Serial port communication at 115200bps");

	// Attach motors to pins

    Motors[0].Pin =  11;
	Motors[1].Pin =  10;
	Motors[2].Pin =  5;
	Motors[3].Pin =  6;

	for(int i = 0; i < NUMMOTORS; i++)
	{
	  int pin = Motors[i].Pin;
	  Motors[i].Motor.attach(pin);
	}

	// Set the ESC settings to the defaults
	ESCSettings.Low   = ESC_LOW_DEFAULT;
	ESCSettings.High  = ESC_HIGH_DEFAULT;
}

// Read low/high speed for the ESC
void ReadLHSpeed()
{
	Serial.println("Enter Low Speed");
	delay(10);
	while(!Serial.available()){}
	ESCSettings.Low = Serial.parseInt();
	Serial.println("Low Speed is");
	Serial.print(ESCSettings.Low);
	Serial.println("\nEnter High Speed\n");
	delay(10);
	while(!Serial.available()){}
	ESCSettings.High = Serial.parseInt();
	Serial.println("High Speed is");
	Serial.print(ESCSettings.High);
	Serial.println("\n");
 
}

void SetThrottleRange()
{
	Serial.println("In Set Throttle Range mode");

	for (int i = 0; i < NUMMOTORS; i++)
	{
	  Motors[i].Motor.writeMicroseconds(ESCSettings.High);
	}

	Serial.println("Connect the ESC now. After connecting the ESC, you should hear \
			the ESC startup tones. Shortly afterwards, you should hear two beeps \
			indicating that the ESC has registered the high throttle value. Immediately \
			after hearing the two beeps, push any key. If you don't do so in 5 sec, the ESC will \
			go into program mode");

	// Wait for user input
	while (!Serial.available())
	{
	}
	Serial.read();

	Serial.println("\nSetting the low throttle setting. If this happens successfully, you should hear \
			several beeps indicating the input voltage supplied to the ESC followed by a long beep \
			indicating that the low throttle has been set. After this point, push any key to proceed,\
		    your ESC is ready to be used");

	for (int i = 0; i < NUMMOTORS; i++)
	{
	  Motors[i].Motor.writeMicroseconds(ESCSettings.Low);
	}

	// Wait for user input
	while (!Serial.available())
	{
	}
	Serial.read();
}

void ProgramESC()
{
	Serial.println("In program mode");

	for (int i = 0; i < NUMMOTORS; i++)
	{
	  Motors[i].Motor.writeMicroseconds(ESCSettings.High);
	}

//	Serial.println("Connect the ESC now. After connecting the ESC, you should hear \
//			the ESC startup tones. Shortly afterwards, you should hear two beeps \
//			indicating that the ESC has registered the high throttle value. Now wait \
//			for about 5 seconds and the ESC will enter program mode. Once the ESC is in \
//			program mode, it will play a special tone. AFter this, it will emit a series of \
//			beeps indicating different settings. When you hear the beep corresponding to the \
//			setting you want to change, push a key. This will set the throttle to low and the \
//			ESC will play beeps corresponding to the various values of that setting. As soon as \
//			you hear the right beep, push another key and the throttle will be raised to high \
//			and the value for that setting will be set. The ESC will acknowledge by playing a \
//			special tone");

	// After the user inputs a key stroke, set the throttle to low
	while (!Serial.available())
	{
	}
	Serial.read();

	for (int i = 0; i < NUMMOTORS; i++)
	{
	  Motors[i].Motor.writeMicroseconds(ESCSettings.Low);
	}

	// After the user inputs a key stroke, set the throttle to high. This will set the value of the setting.
	while (!Serial.available())
	{
	}
	Serial.read();

	for (int i = 0; i < NUMMOTORS; i++)
	{
	  Motors[i].Motor.writeMicroseconds(ESCSettings.High);
	}

	delay(2000);

	Serial.println("Programming the ESC finished, you can power off the ESC now");

}

// Increase the speed of the motor from low to high as set by the user
void Run()
{
	for (int i = 0; i < NUMMOTORS; i++)
	{
	  Motors[i].Motor.writeMicroseconds(ESCSettings.Low);
	}
	Serial.println("Running ESC");
	Serial.println("Step = ");
	Serial.print(Step);
	Serial.println("\nPress 'u' to increase speed, 'd' to reduce speed");

	CurrentSpeed = ESCSettings.Low;
	while (1) {
		while (!Serial.available())
		{
		}
		char currentChar = Serial.read();
		if (currentChar == 'u')
		{
			Serial.println("\nIncreasing motor speed by step");
			if (CurrentSpeed + Step < ESCSettings.High) {
				CurrentSpeed = CurrentSpeed + Step;
				Serial.println("New speed = ");
				Serial.print(CurrentSpeed);
			}

			else
			{
				Serial.println("\nMax speed reached\n");
			}
		}
		if (currentChar == 'd')
		{
			Serial.println("\nDecreasing motor speed by step\n");
			if (CurrentSpeed - Step >= ESCSettings.Low)
			{
				CurrentSpeed = CurrentSpeed - Step;
				Serial.println("New speed = ");
				Serial.print(CurrentSpeed);
			}

			else
			{
				Serial.println("\nMin speed reached\n");
			}
		}
		if (currentChar == 'e')
		{
			Serial.println("\nStopping Motors\n");
			CurrentSpeed = ESCSettings.Low;
		}
		for (int i = 0; i < NUMMOTORS; i++)
		{
		  Motors[i].Motor.writeMicroseconds(CurrentSpeed);
		}
	}
}

void loop()
{
#ifdef NORMAL_MODE
	Run();
#elif defined THROTTLE_MODE
	SetThrottleRange();
	Run();
#elif defined PROGRAM_MODE
	ProgramESC();
#endif

}

