
/*
  PWMRead
  This code will read the PWM signal on an Analog Input pin and provide methods to get the PWM output
 */
#ifdef _PWMREAD_

#include "arduino.h"

class PWMRead()
{
public:
	PWMRead(int inputPin): InputPin(inputPin){}

	int InputPin;
	int InputVal;

	void Read()
	{
		if (InputPin >= A0 && InputPin <= A5)
		InputVal = AnalogRead(InputPin);
	}
	int GetValue()
	{
		return InputVal;
	}
}

#endif
