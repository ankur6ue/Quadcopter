/*
  Utils.cpp - Utility functions for my quadcopter code


  Based on the code of Myles Grant <myles@mylesgrant.com>
  See also: https://github.com/grantmd/QuadCopter
  
  This program is free software: you can redistribute it and/or modify 
  it under the terms of the GNU General Public License as published by 
  the Free Software Foundation, either version 3 of the License, or 
  (at your option) any later version. 

  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
  GNU General Public License for more details. 

  You should have received a copy of the GNU General Public License 
  along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#include "utils.h"

// We want to share Utils and Matrix ops code between Arduino and C++ app. Some #ifdefs are needed
// to make sure code compiles
#ifndef WIN32
#include <Arduino.h>
#else
#include "qmath.h"
#define PI M_PI
#define isnan _isnan // defined in float.h
#endif

float map_f(float  x, float  in_min,float   in_max, float  out_min, float  out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float map_i(int  x, int  in_min, int   in_max, int  out_min, float  out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//new version of map_f which is a bit faster for the RADIO data than map_f
float map_f_s(int  x, int  in_min, int out_min, int ratio)
{
	return (x - in_min) / ratio + out_min;
}

float mean(int m, float a[]) {
    int sum=0, i;
    for(i=0; i<m; i++)
        sum+=a[i];
    return((float)sum/m);
}

// Insert sort. From "whistler" - http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1283456170/0
void isort(int *a, int n){
	for (int i = 1; i < n; ++i){
		int j = a[i];
		int k;
		for (k = i - 1; (k >= 0) && (j < a[k]); k--){
			a[k + 1] = a[k];
		}
    a[k + 1] = j;
	}
}

int findMedian(int *data, int arraySize){
	isort(data, arraySize);
	return data[arraySize/2];
}

// Low pass filter, kept as regular C function for speed
float filterSmooth(float currentData, float previousData, float smoothFactor){
	if (smoothFactor != 1.0) //only apply time compensated filter if smoothFactor is applied
		return (previousData * (1.0 - smoothFactor) + (currentData * smoothFactor)); 
	else
		return currentData; //if smoothFactor == 1.0, do not calculate, just bypass!
}

unsigned char sgn(int val) {
	if (val < 0) return -1;
	if (val==0) return 0;
	return 1;
}

float constrain_f(float x, float min, float max)
{
	if (x>max)
	{
		return max;
	}
	if (x<min)
	{
		return min;
	}
	else
	{
		return x;
	}
}

int constrain_i(int x, int min, int max)
{
	if (x>max)
	{
		return max;
	}
	if (x<min)
	{
		return min;
	}
	else
	{
		return x;
	}
}

// a varient of asin() that checks the input ranges and ensures a
// valid angle as output. If nan is given as input then zero is
// returned.
float safe_asin(float v)
{
    if (isnan(v)) {
        return 0.0;
    }
    if (v >= 1.0f) {
        return PI/2;
    }
    if (v <= -1.0f) {
        return -PI/2;
    }
    // The math.h that comes with Arduino libs doesn't define asinf. The one with WinAVR does.
    return asin(v);
}

// a varient of sqrt() that checks the input ranges and ensures a
// valid value as output. If a negative number is given then 0 is
// returned. The reasoning is that a negative number for sqrt() in our
// code is usually caused by small numerical rounding errors, so the
// real input should have been zero

// The math.h that comes with Arduino libs doesn't define sqrtf. The one with WinAVR does.
float safe_sqrt(float v)
{
    float ret = sqrt(v);
    if (isnan(ret)) {
        return 0;
    }
    return ret;
}

// a faster varient of atan.  accurate to 6 decimal places for values between -1 ~ 1 but then diverges quickly
float fast_atan(float v)
{
    float v2 = v*v;
    return (v*(1.6867629106f + v2*0.4378497304f)/(1.6867633134f + v2));
}

#define FAST_ATAN2_PIBY2_FLOAT  1.5707963f
// fast_atan2 - faster version of atan2
//      126 us on AVR cpu vs 199 for regular atan2
//      absolute error is < 0.005 radians or 0.28 degrees
//      origin source: https://gist.github.com/volkansalma/2972237/raw/
float fast_atan2(float y, float x)
{
   if (x == 0.0f) {
       if (y > 0.0f) {
           return FAST_ATAN2_PIBY2_FLOAT;
       }
       if (y == 0.0f) {
           return 0.0f;
       }
       return -FAST_ATAN2_PIBY2_FLOAT;
   }
   float atan;
   float z = y/x;
   if (fabs( z ) < 1.0f) {
       atan = z / (1.0f + 0.28f * z * z);
       if (x < 0.0f) {
           if (y < 0.0f) {
               return atan - PI;
           }
           return atan + PI;
       }
   } else {
       atan = FAST_ATAN2_PIBY2_FLOAT - (z / (z * z + 0.28f));
       if (y < 0.0f) {
           return atan - PI;
       }
   }
   return atan;
}
