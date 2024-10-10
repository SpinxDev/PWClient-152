/*
 * FILE: A3DQuaternion.cpp
 *
 * DESCRIPTION: Class standing for Quaternions
 *
 * CREATED BY: Hedi, 2003/7/11
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Epie Entertainment, All Rights Reserved.
 */

#include <math.h>
#include "AAssist.h"
#include "A3DTypes.h"
#include "A3DQuaternion.h"
#include "A3DPlatform.h"
///////////////////////////////////////////////////////////////////////////////////////
//
// QuadToMatrix()
//
//	construct a rotation matrix with a quaternion
///////////////////////////////////////////////////////////////////////////////////////
void QuadToMatrix(const A3DQUATERNION& q, A3DMATRIX4& mat)
{
/*	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2; 
	// calculate coefficients
	x2 = q.x + q.x; 
	y2 = q.y + q.y; 
	z2 = q.z + q.z;
	xx = q.x * x2; 
	xy = q.x * y2; 
	xz = q.x * z2;
	yy = q.y * y2; 
	yz = q.y * z2; 
	zz = q.z * z2;
	wx = q.w * x2; 
	wy = q.w * y2; 
	wz = q.w * z2;
	mat._11 = 1.0f - (yy + zz); 
	mat._21 = xy - wz;
	mat._31 = xz + wy; 
	mat._41 = 0.0f;
	mat._12 = xy + wz; 
	mat._22 = 1.0f - (xx + zz);
	mat._32 = yz - wx; 
	mat._42 = 0.0f;
	mat._13 = xz - wy; 
	mat._23 = yz + wx;
	mat._33 = 1.0f - (xx + yy); 
	mat._43 = 0.0f;
	mat._14 = 0.0f; 
	mat._24 = 0.0f;
	mat._34 = 0.0f; 
	mat._44 = 1.0f;
*/
	D3DXMatrixRotationQuaternion((D3DXMATRIX*)&mat, (const D3DXQUATERNION*)&q);
}

///////////////////////////////////////////////////////////////////////////////////////
//
// MatrixToQuad()
//
//	construct a quaternion with a rotating matrix. The matrix is not required to be a 
// pure rotation matrix, it can contains translation and scale factor, but the output
// quaternion will not be unit quaternion if it contains scale factor.
//
///////////////////////////////////////////////////////////////////////////////////////
void MatrixToQuad(const A3DMATRIX4& mat, A3DQUATERNION& q)
{
/*	float	tr, s;
	
	// calculating the trace of the matrix, it is equal to 4(1 - x*x - y*y - z*z)=4w*w if it is a unit quaternion
	tr = mat._11 + mat._22 + mat._33 + 1.0f;
	// check the diagonal
	if (tr > 0.36f) // we can calculate out w directly
	{
		s = (float)sqrt(tr); // s is 2w
		q.w = s * 0.5f;    
		s = 0.5f / s;	// now s is 1/4w
		q.x = (mat._23 - mat._32) * s;
		q.y = (mat._31 - mat._13) * s;
		q.z = (mat._12 - mat._21) * s;
	} 
	else
	{			 
		// we have to calculate x, y or z first
		if( mat._11 >= mat._22 && mat._11 >= mat._33 )
		{
			s = (float)sqrt(1.0f + mat._11 - mat._22 - mat._33); // s is 2x
			q.x = s *0.5f;
			s = 0.5f / s;
			q.y = (mat._12 + mat._21) * s;
			q.z = (mat._13 + mat._31) * s;
			q.w = (mat._23 - mat._32) * s;
		}
		else if( mat._22 >= mat._11 && mat._22 >= mat._33 )
		{
			s = (float)sqrt(1.0f + mat._22 - mat._11 - mat._33); // s is 2y
			q.y = s *0.5f;
			s = 0.5f / s;
			q.x = (mat._12 + mat._21) * s;
			q.z = (mat._23 + mat._32) * s;
			q.w = (mat._31 - mat._13) * s;
		}
		else // mat._33 is maximum
		{
			s = (float)sqrt(1.0f + mat._33 - mat._11 - mat._22); // s is 2z
			q.z = s *0.5f;
			s = 0.5f / s;
			q.x = (mat._13 + mat._31) * s;
			q.y = (mat._23 + mat._32) * s;
			q.w = (mat._12 - mat._21) * s;
		}
	}	
*/
	D3DXQuaternionRotationMatrix((D3DXQUATERNION*)&q, (const D3DXMATRIX*)&mat);
}

void QuadToEulerAngle(const A3DQUATERNION& q, FLOAT& vXAngle, FLOAT& vYAngle, FLOAT& vZAngle)
{
	// Not implemented yet!	
}

void EulerAngleToQuad(FLOAT vXAngle, FLOAT vYAngle, FLOAT vZAngle, A3DQUATERNION& q)
{
	// We can convert each euler angle to a quaternion, than we multiply them together.
	vXAngle *= 0.5f;
	vYAngle *= 0.5f;
	vZAngle *= 0.5f;

	// calculate trig identities
	float cr, cp, cy, sr, sp, sy, cpcy, spsy, cpsy, spcy;
	cr = (float)cos(vXAngle);	
	cp = (float)cos(vYAngle);	
	cy = (float)cos(vZAngle);	
	sr = (float)sin(vXAngle);	
	sp = (float)sin(vYAngle);	
	sy = (float)sin(vZAngle);		
	cpcy = cp * cy;	
	spsy = sp * sy;	
	cpsy = cp * sy;
	spcy = sp * cy;
	q.w = cr * cpcy + sr * spsy;	
	q.x = sr * cpcy - cr * spsy;	
	q.y = cr * spcy + sr * cpsy;	
	q.z = cr * cpsy - sr * spcy;
}

void QuadToAxisAngle(const A3DQUATERNION& q, A3DVECTOR3& vecAxis, FLOAT& vAngle)
{
	float cosine, sine;

	cosine	= q.w;
    vAngle	= (float)(acos(cosine)) * 2.0f;
    sine	= (float)sqrt(1.0f - cosine * cosine);

    if( sine < 0.0005f )
      sine = 1.0f;

    vecAxis.x = q.x / sine;
    vecAxis.y = q.y / sine;
    vecAxis.z = q.z / sine;
}

void AxisAngleToQuad(const A3DVECTOR3& vecAxis, FLOAT vAngle, A3DQUATERNION& q)
{
	// q = (cos(theta/2), x*sin(theta/2), y*sin(theta/2), z*sin(theta/2));
	float sine, cosine;

	vAngle *= 0.5f;
	sine = (float)sin(vAngle);
	cosine = (float)cos(vAngle);

	q.w = cosine;
	q.x = vecAxis.x * sine;
	q.y = vecAxis.y * sine;
	q.z = vecAxis.z * sine;
}

A3DQUATERNION SLERPQuad(const A3DQUATERNION& q1, const A3DQUATERNION& q2, float fraction)
{
	FLOAT cosine, sign;
	FLOAT f1, f2;
	cosine = DotProduct(q1, q2);
	if( cosine < 0.0f )
	{
		cosine = -cosine;
		sign = -1.0f;
	}
	else
		sign = 1.0f;

	if( cosine > 1.0f - SLERP_EPSILON )
	{
		// the from and to value are very close, so use LERP will be ok
		f1 = 1.0f - fraction;
		f2 = fraction * sign;
	}
	else
	{
		FLOAT theta;
		theta = (FLOAT)acos(cosine);
		FLOAT sine;
		sine = (FLOAT)sin(theta);
		f1 = (FLOAT)(sin((1.0f - fraction) * theta) / sine);
		f2 = (FLOAT)(sin(fraction * theta) / sine) * sign;
	}

	return q1 * f1 + q2 * f2;
}

#define MATH_PI (3.141592653589793f)
float QuatDiffAngle(const A3DQUATERNION& q1, const A3DQUATERNION& q2)
{
	// we assume q can rotate q1 to q2, so we get
	// q1 * q = q2
	// q =  q1.Inverse() * q2
	// q.w = q1.w * q2.w - (-q1.x) * q2.x - (-q1.y) * q2.y - (-q1.z) * q2.z

	float fW = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

	if(fW > 1.0f || fW < -1.0f) return 0.0f;

	float fDiffAngle = ::acos(fW) * 2.0f;
	if(fDiffAngle > MATH_PI)
		return 360 - fDiffAngle * 180 / MATH_PI;
	else
		return fDiffAngle * 180 / MATH_PI;


}