
int ReadPinA0()
{
	static long lastDebounceTime = millis();
	int currentButtonReading = analogRead(A0); // returns numbers from 0 to 1023
	// convert to digital
	int currButtonReadingDigital 	= currentButtonReading > 1000? 1: 0;
	static int lastButtonState 		= currButtonReadingDigital;
	static int ButtonState 			= currButtonReadingDigital;

	if (currButtonReadingDigital != lastButtonState)
	{
		lastDebounceTime = millis();
	}
	if (millis() - lastDebounceTime > 100)
	{
		if (lastButtonState != ButtonState)
		{
			ButtonState = lastButtonState;
		}
	}
	lastButtonState = currButtonReadingDigital;

	return ButtonState;
};
