/**************************************************************************

Filename    :   MatrixOps.h
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/
#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H

#ifndef WIN32
#include <Arduino.h>
#else
#include "qmath.h"
#endif

class VectorF {

public:
	float x;
	float y;
	float z;

	VectorF() {
		x = 0;
		y = 0;
		z = 0;
	}

	VectorF(float nx, float ny, float nz) {
		x = nx;
		y = ny;
		z = nz;
	}

	void Add(VectorF& v);

	VectorF operator +(const VectorF& v)
	{
		VectorF v2(x + v.x, y + v.y, z + v.z);
		return v2;
	}

	// Note: GCC doesn't accept operator overloading declaration without const. VectorF operator -(VectorF& v)
	VectorF operator -(const VectorF& v)
	{
		VectorF v2(x - v.x, y - v.y, z - v.z);
		return v2;
	}
	
	// Dot product
	float	Dot(VectorF& v)
	{
		return x*v.x + y*v.y + z*v.z;
	}

	// Multiplication by scalar
	VectorF	Dot(float v)
	{
		return VectorF(x*v, y*v, z*v);
	}

	VectorF Cross(VectorF& v)
	{
		VectorF temp(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
		return temp;
	}

	void FastNormalize()
	{
		Scale(0.5*(3 - this->Dot(*this)));
	}

	VectorF	SetZero() { x = 0; y = 0; z = 0; }

	VectorF	Scale(float scaleFactor)
	{
		x = x*scaleFactor; y = y*scaleFactor; z = z*scaleFactor;
		return *this;
	}
	void 	Normalize();
	VectorF GetNormalized();
	float 	GetMagnitude();
};

class Matrix3D
{
public:
	Matrix3D()
	{
		rxx = 1; rxy = rxz = 0;
		ryy = 1; ryx = ryz = 0;
		rzz = 1; rzx = rzy = 0;
	}
	
	Matrix3D(Matrix3D& m)
	{
		rxx = m.rxx; rxy = m.rxy; rxz = m.rxz;
		ryx = m.ryx; ryy = m.ryy; ryz = m.ryz;
		rzx = m.rzx; rzy = m.rzy; rzz = m.rzz;
	}

	Matrix3D(VectorF& r1, VectorF& r2, VectorF& r3)
	{
		rxx = r1.x; rxy = r1.y; rxz = r1.z;
		ryx = r2.x; ryy = r2.y; ryz = r2.z;
		rzx = r3.x; rzy = r3.y; rzz = r3.z;
	}

	// Rotate matrix by vector v
	void Rotate(VectorF& v);

	Matrix3D operator +(const Matrix3D& m)
	{
		VectorF r1(rxx + m.rxx, rxy + m.rxy, rxz + m.rxz);
		VectorF r2(ryx + m.ryx, ryy + m.ryy, ryz + m.ryz);
		VectorF r3(rzx + m.rzx, rzy + m.rzy, rzz + m.rzz);
		Matrix3D m2(r1, r2, r3);
		return m2;
	}

	Matrix3D operator -(const Matrix3D& m)
	{
		VectorF r1(rxx - m.rxx, rxy - m.rxy, rxz - m.rxz);
		VectorF r2(ryx - m.ryx, ryy - m.ryy, ryz - m.ryz);
		VectorF r3(rzx - m.rzx, rzy - m.rzy, rzz - m.rzz);
		Matrix3D m2(r1, r2, r3);
		return m2;
	}

	Matrix3D Multiply(Matrix3D& m);

	void 	FromEuler(float yaw, float pitch, float roll);

	void 	ToEuler(float& yaw, float& pitch, float& roll);

	void	Scale(float scaleFactor)
	{
		rxx = rxx*scaleFactor; rxy = rxy*scaleFactor; rxz = rxz*scaleFactor;
		ryx = ryx*scaleFactor; ryy = ryy*scaleFactor; ryz = ryz*scaleFactor;
		rzx = rzx*scaleFactor; rzy = rzy*scaleFactor; rzz = rzz*scaleFactor;
	}

	void Renormalize();

	VectorF	GetRow(int rowNum);

	void 	Reset()
	{
		rxx = ryy = rzz = 1;
		rxy = rxz = ryx = ryz = rzx = rzy = 0;
	}

	float 	rxx, rxy, rxz;
	float 	ryx, ryy, ryz;
	float	rzx, rzy, rzz;
};
#endif
