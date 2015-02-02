#ifndef _SERIAL_DEF_
#define _SERIAL_DEF_

extern SoftwareSerial SSerial;
#define USE_HARDWARE_SERIAL

#ifdef USE_HARDWARE_SERIAL
#define SERIAL Serial1
#else
#define SERIAL SSerial
#endif

#endif
