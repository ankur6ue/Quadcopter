/**************************************************************************

Filename    :   IMU.cpp
Content     :
Created     :   Feb 2015
Authors     :   Ankur Mohan

Copyright   :   Copyright 2015 Ankur Mohan, All Rights reserved.

Use of this software is subject to the terms of the license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "MatrixOps.h"
#include "Utils.h"

void VectorF::Normalize()
{
	float m = GetMagnitude();
	x /= m;
	y /= m;
	z /= m;
}

VectorF VectorF::GetNormalized()
{
	VectorF r(x, y, z);
	r.Normalize();
	return r;
}

float VectorF::GetMagnitude()
{
	return sqrt(x*x + y*y + z*z);
}

void VectorF::Add(VectorF& v)
{
	x += v.x; y += v.y; z += v.z;
}

void Matrix3D::Rotate(VectorF& v)
{
	Matrix3D m;
	m.rxx = rxy * v.z - rxz * v.y;
	m.rxy = rxz * v.x - rxx * v.z;
	m.rxz = rxx * v.y - rxy * v.x;
	m.ryx = ryy * v.z - ryz * v.y;
	m.ryy = ryz * v.x - ryx * v.z;
	m.ryz = ryx * v.y - ryy * v.x;
	m.rzx = rzy * v.z - rzz * v.y;
	m.rzy = rzz * v.x - rzx * v.z;
	m.rzz = rzx * v.y - rzy * v.x;

	(*this) = (*this) + m;
}

Matrix3D Matrix3D::Multiply(Matrix3D& m)
{
	VectorF r1(rxx * m.rxx + rxy * m.ryx + rxz * m.rzx,
				rxx * m.rxy + rxy * m.ryy + rxz * m.rzy,
				rxx * m.rxz + rxy * m.ryz + rxz * m.rzz);
	VectorF r2(ryx * m.rxx + ryy * m.ryx + ryz * m.rzx,
				ryx * m.rxy + ryy * m.ryy + ryz * m.rzy,
				ryx * m.rxz + ryy * m.ryz + ryz * m.rzz);
	VectorF r3(rzx * m.rxx + rzy * m.ryx + rzz * m.rzx,
				rzx * m.rxy + rzy * m.ryy + rzz * m.rzy,
				rzx * m.rxz + rzy * m.ryz + rzz * m.rzz);
	 Matrix3D temp (r1, r2, r3);
	 return temp;
}

void Matrix3D::FromEuler(float yaw, float pitch, float roll)
{
	float cp = cos(pitch);
	float sp = sin(pitch);
	float sr = sin(roll);
	float cr = cos(roll);
	float sy = sin(yaw);
	float cy = cos(yaw);

	rxx = cp * cy;
	rxy = (sr * sp * cy) - (cr * sy);
	rxz = (cr * sp * cy) + (sr * sy);
	ryx = cp * sy;
	ryy = (sr * sp * sy) + (cr * cy);
	ryz = (cr * sp * sy) - (sr * cy);
	rzx = -sp;
	rzy = sr * cp;
	rzz = cr * cp;
}

void Matrix3D::Renormalize()
{
	// Normalize each row
	VectorF r1 = GetRow(1); 
	VectorF r2 = GetRow(2); 
	VectorF r3 = GetRow(3);
	float error = r1.Dot(r2);
	VectorF r1_orth = r1 - r2.Dot(error/2);
	VectorF r2_orth = r2 - r1.Dot(error/2);
	VectorF r3_orth = r1_orth.Cross(r2_orth);
	r1_orth.Normalize();
	r2_orth.Normalize();
	r3_orth.Normalize();
	*this = Matrix3D(r1_orth, r2_orth, r3_orth);
}
VectorF Matrix3D::GetRow(int rowNum)
{
	VectorF v;
	if (rowNum == 1)
	{
		VectorF r1(rxx, rxy, rxz);
		v.Add(r1);
	}
	if (rowNum == 2)
	{
		VectorF r2(ryx, ryy, ryz);
		v.Add(r2);
	}
	if (rowNum == 3)
	{
		VectorF r3(rzx, rzy, rzz);
		v.Add(r3);
	}
	return v;
}

void Matrix3D::ToEuler(float& yaw, float& pitch, float& roll)
{
	 pitch = -safe_asin(rzx);
	 roll = atan2(rzy, rzz);
	 yaw = atan2(ryx, rxx);
}
