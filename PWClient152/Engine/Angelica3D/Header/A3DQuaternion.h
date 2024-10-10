/*
 * FILE: A3DQuaternion.h
 *
 * DESCRIPTION: Class standing for Quaternions
 *
 * CREATED BY: Hedi, 2003/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Epie Entertainment, All Rights Reserved.
 */

#ifndef _A3DQUATERNION_H_
#define _A3DQUATERNION_H_

#include <math.h>
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//
// class A3DQUATERNION
//
///////////////////////////////////////////////////////////////////////////
class A3DQUATERNION
{
public:
	// Constructors
	A3DQUATERNION() {}
	A3DQUATERNION(const A3DMATRIX4& mat) { ConvertFromMatrix(mat); }
	A3DQUATERNION(const A3DVECTOR3& vecAxis, FLOAT vAngle) { ConvertFromAxisAngle(vecAxis, vAngle); }
	A3DQUATERNION(FLOAT vXAngle, FLOAT vYAngle, FLOAT vZAngle) { ConvertFromEulerAngle(vXAngle, vYAngle, vZAngle); }

	A3DQUATERNION(FLOAT _w, FLOAT _x, FLOAT _y, FLOAT _z) { w=_w; x=_x; y=_y; z=_z; }
	A3DQUATERNION(FLOAT _w, const A3DVECTOR3& _u) { w=_w; x=_u.x; y=_u.y; z=_u.z; }

public:

	union
	{
		struct
		{
			FLOAT x, y, z, w;
		};

		FLOAT m[4];
	};

public:
	// + operator
	inline friend A3DQUATERNION operator + (const A3DQUATERNION& q1, const A3DQUATERNION& q2) { return A3DQUATERNION(q1.w+q2.w, q1.x+q2.x, q1.y+q2.y, q1.z+q2.z); }
	
	// - operator
	inline friend A3DQUATERNION operator - (const A3DQUATERNION& q1, const A3DQUATERNION& q2) { return A3DQUATERNION(q1.w-q2.w, q1.x-q2.x, q1.y-q2.y, q1.z-q2.z); }
	
	// * operator
	inline friend A3DQUATERNION operator * (const A3DQUATERNION& q, float f) { return A3DQUATERNION(q.w*f, q.x*f, q.y*f, q.z*f); }
	inline friend A3DQUATERNION operator * (float f, const A3DQUATERNION& q) { return A3DQUATERNION(q.w*f, q.x*f, q.y*f, q.z*f); }
	inline friend A3DQUATERNION operator * (const A3DQUATERNION& q, int f)	{ return q * (float)f; }
	inline friend A3DQUATERNION operator * (int f, const A3DQUATERNION& q) { return q * (float)f; }
	
	//	== operator
	inline friend bool operator == (const A3DQUATERNION& q1, const A3DQUATERNION& q2) { return q1.x==q2.x && q1.y==q2.y && q1.z==q2.z && q1.w==q2.w; }
	//	!= operator
	inline friend bool operator != (const A3DQUATERNION& q1, const A3DQUATERNION& q2) { return q1.x!=q2.x || q1.y!=q2.y || q1.z!=q2.z || q1.w!=q2.w; }

	// / operator
	inline friend A3DQUATERNION operator / (const A3DQUATERNION& q, float f) { f = 1.0f / f; return A3DQUATERNION(q.w*f, q.x*f, q.y*f, q.z*f); }
	inline friend A3DQUATERNION operator / (const A3DQUATERNION& q, int f) { return q / (float)f; }
	inline friend A3DQUATERNION operator * (const A3DQUATERNION& q1, const A3DQUATERNION& q2)
	{
		/*
		// Below is a optimized code with only 8 multiplication. See in d:\ÎÄµµ\Gamasutra - Features - Rotating Objects Using Quaternions [07_03_98].doc
		float A, B, C, D, E, F, G, H;
		A = (q1.s + q1.x) * (q2.s + q2.x);
		B = (q1.z - q1.y) * (q2.y - q2.z);
		C = (q1.s - q1.x) * (q2.y + q2.z); 
		D = (q1.y + q1.z) * (q2.s - q2.x);
		E = (q1.x + q1.z) * (q2.x + q2.y);
		F = (q1.x - q1.z) * (q2.x - q2.y);
		G = (q1.s + q1.y) * (q2.s - q2.z);
		H = (q1.w - q1.y) * (q2.s + q2.z);
		return A3DQUATERNION(B + (-E - F + G + H) / 2, A - (E + F + G + H) / 2, C + (E - F + G - H) / 2, D + (E - F - G + H) / 2);
		*/
		//product = (w1 * w2 - v1.v2, s1v2 + s2v1 + v2 * v2);
		return A3DQUATERNION(q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
							 q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
							 q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
							 q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
	}

	inline friend A3DVECTOR3 operator * (const A3DQUATERNION& q, const A3DVECTOR3& v)
	{
		A3DQUATERNION	vq = A3DQUATERNION(1.0f, v);
		A3DQUATERNION	qi	= A3DQUATERNION(q.w, -q.x, -q.y, -q.z);
		A3DQUATERNION	qvq = q * vq * qi;

		return A3DVECTOR3(qvq.x, qvq.y, qvq.z);
	}

	inline friend A3DVECTOR3 operator ^ (const A3DQUATERNION& q, const A3DVECTOR3& v)
	{
		// nVidia SDK implementation
		A3DVECTOR3  uv, uuv; 
		A3DVECTOR3	qvec(q.x, q.y, q.z);

		uv = CrossProduct(qvec, v);
		uuv = CrossProduct(qvec, uv); 
		uv = (2.0f * q.w) * uv; 
		uuv = 2.0f * uuv; 
		
		return v + uv + uuv;
	}

public:

	inline void Clear() { x = y = z = w = 0.0f; }
	inline void Set(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w) { x=_x; y=_y; z=_z; w=_w; }

	/////////////////////////////////////////////////////////////////////////////////
	//
	// Normalize methods.
	// NOTE: Only unit quaternions can stand from rotations.
	//
	/////////////////////////////////////////////////////////////////////////////////
	inline FLOAT Norm() const { return (FLOAT)sqrt(w * w + x * x + y * y + z * z); }

	inline const A3DQUATERNION& Normalize() { FLOAT f = 1.0f / Norm(); w*=f; x*=f; y*=f; z*=f; return *this; }
	inline const A3DQUATERNION& Conjugate() { x = -x; y = -y; z = -z; return *this; }
	inline const A3DQUATERNION& Inverse() { FLOAT f = 1.0f / Norm(); w*=f; x*=-f; y*=-f; z*=-f; return *this; }

	inline friend A3DQUATERNION Normalize(const A3DQUATERNION& q) { return q / q.Norm(); }
	inline friend A3DQUATERNION Conjugate(const A3DQUATERNION& q) { return A3DQUATERNION(q.w, -q.x, -q.y, -q.z); }
	inline friend A3DQUATERNION Inverse(const A3DQUATERNION& q) { A3DQUATERNION q2(q.w, -q.x, -q.y, -q.z); return q2 / q2.Norm(); }
	
	inline friend FLOAT DotProduct(const A3DQUATERNION& q1, const A3DQUATERNION& q2) { return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z; }

	/////////////////////////////////////////////////////////////////////////////////
	//
	// Convert methods with other orientation types.
	//
	/////////////////////////////////////////////////////////////////////////////////
	inline void ConvertToMatrix(A3DMATRIX4& mat) const { QuadToMatrix(*this, mat); }
	inline void ConvertFromMatrix(const A3DMATRIX4& mat) { MatrixToQuad(mat, *this); }
	inline void ConvertToEulerAngle(FLOAT& vXAngle, FLOAT& vYAngle, FLOAT& vZAngle) const { QuadToEulerAngle(*this, vXAngle, vYAngle, vZAngle); }
	inline void ConvertFromEulerAngle(FLOAT vXAngle, FLOAT vYAngle, FLOAT vZAngle) { EulerAngleToQuad(vXAngle, vYAngle, vZAngle, *this); }
	inline void ConvertToAxisAngle(A3DVECTOR3& vecAxis, FLOAT& vAngle) const { QuadToAxisAngle(*this, vecAxis, vAngle); }
	inline void ConvertFromAxisAngle(const A3DVECTOR3& vecAxis, FLOAT vAngle) { AxisAngleToQuad(vecAxis, vAngle, *this); }

	friend void QuadToMatrix(const A3DQUATERNION& q, A3DMATRIX4& mat);
	friend void MatrixToQuad(const A3DMATRIX4& mat, A3DQUATERNION& q);
	friend void QuadToEulerAngle(const A3DQUATERNION& q, FLOAT& vXAngle, FLOAT& vYAngle, FLOAT& vZAngle);
	friend void EulerAngleToQuad(FLOAT vXAngle, FLOAT vYAngle, FLOAT vZAngle, A3DQUATERNION& q);
	friend void QuadToAxisAngle(const A3DQUATERNION& q, A3DVECTOR3& vecAxis, FLOAT& vAngle);
	friend void AxisAngleToQuad(const A3DVECTOR3& vecAxis, FLOAT vAngle, A3DQUATERNION& q);

	///////////////////////////////////////////////////////////////////////////////////
	// Interpolate method.
	// 
	// A3DQUATERNION SLERPQuad(const A3DQUATERNION& q1, const A3DQUATERNION&q2, float fraction)
	//	calculate the interpolation of two quaternion using Spherical Linear Interpolation
	// algorithm.
	//
	// q1				IN				the start quaternion
	// q2				IN				the end quaternion
	// fraction			IN				the fraction between q1 and q2, 0.0 means q1 and 1.0 means q2
	//
	///////////////////////////////////////////////////////////////////////////////////
#define SLERP_EPSILON	(0.25f)
	friend A3DQUATERNION SLERPQuad(const A3DQUATERNION& q1, const A3DQUATERNION& q2, float fraction);
	
	//--------------------------------------------------------------------------------
	// compute the hypersphere-angle between two quaternions, and we can use it as the 
	// difference of these two quaternions.
	//
	// q1,q2 should be normalized
	// 
	// return the angle in unit of degree
	//
	friend float QuatDiffAngle(const A3DQUATERNION& q1, const A3DQUATERNION& q2);
};

#endif//_A3DQUATERNION_H_
