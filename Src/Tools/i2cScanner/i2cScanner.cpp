//#line 1 "AnalogReadSerial.ino"
/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
 This example code is in the public domain.
 */

// the setup routine runs once when you press reset:
#include <Arduino.h>
#include <Wire.h>

void setup();
void loop();
#line 10
extern HardwareSerial Serial;
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial)
  {
  }

  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
	  Wire.beginTransmission (i);
	  if (Wire.endTransmission () == 0)
	  {
		  Serial.print ("Found address: ");
		  Serial.print (i, DEC);
		  Serial.print (" (0x");
		  Serial.print (i, HEX);
		  Serial.println (")");
		  count++;
		  delay (1);  // maybe unneeded?
	  } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
 // end of setup
}

// the loop routine runs over and over again forever:
void loop() {
 
}


int main(void)
{
	init();

	#if defined(USBCON)
	USBDevice.attach();
	#endif
	
	setup();
	
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
	
	return 0;
}
