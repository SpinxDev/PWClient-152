/*
 * FILE: A3DMatrix.cpp
 *
 * DESCRIPTION: Matrix routines
 *
 * CREATED BY: duyuxin, 2003/6/5
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "A3DMatrix.h"
#include "A3DPlatform.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DMATRIX3
//
///////////////////////////////////////////////////////////////////////////

//	Clear all elements to zero
void A3DMATRIX3::Clear()
{
	_11 = _12 = _13 = 0.0f;
	_21 = _22 = _23 = 0.0f;
	_31 = _32 = _33 = 0.0f;
}

//	Set matrix to identity matrix
void A3DMATRIX3::Identity()
{
	_12 = _13 = 0.0f;
	_21 = _23 = 0.0f;
	_31 = _32 = 0.0f;
	_11 = _22 = _33 = 1.0f;
}

//	* operator
A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX3& mat)
{
	return A3DVECTOR3(v.x * mat._11 + v.y * mat._21 + v.z * mat._31,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33);
}

A3DVECTOR3 operator * (const A3DMATRIX3& mat, const A3DVECTOR3& v)
{
	return A3DVECTOR3(v.x * mat._11 + v.y * mat._21 + v.z * mat._31,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33);
}

A3DMATRIX3 operator * (const A3DMATRIX3& mat1, const A3DMATRIX3& mat2)
{
	A3DMATRIX3 matRet(A3DMATRIX3::CLEARED);

	for (int i=0; i < 3; i++)
	{
		for (int j=0; j < 3; j++)
		{
			for (int k=0; k < 3; k++)
				matRet.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
		}
	}

	return matRet;
}


void A3DMATRIX3::Translate(float x, float y)
{
	Identity();
	_31 = x; 
	_32 = y;
}

void A3DMATRIX3::Rotate(float fRad)
{
	Identity();
	m[1][1] = m[0][0] = (float)cos(fRad);
	m[0][1] = (float)sin(fRad);
	m[1][0] = -m[0][1];
}

void A3DMATRIX3::InverseTM()
{
    float matInverse[3][3];

    matInverse[0][0] = m[1][1]*m[2][2] -
        m[1][2]*m[2][1];
    matInverse[0][1] = m[0][2]*m[2][1] -
        m[0][1]*m[2][2];
    matInverse[0][2] = m[0][1]*m[1][2] -
        m[0][2]*m[1][1];
    matInverse[1][0] = m[1][2]*m[2][0] -
        m[1][0]*m[2][2];
    matInverse[1][1] = m[0][0]*m[2][2] -
        m[0][2]*m[2][0];
    matInverse[1][2] = m[0][2]*m[1][0] -
        m[0][0]*m[1][2];
    matInverse[2][0] = m[1][0]*m[2][1] -
        m[1][1]*m[2][0];
    matInverse[2][1] = m[0][1]*m[2][0] -
        m[0][0]*m[2][1];
    matInverse[2][2] = m[0][0]*m[1][1] -
        m[0][1]*m[1][0];

    float fDet =
        m[0][0] * matInverse[0][0] +
        m[0][1] * matInverse[1][0] +
        m[0][2] * matInverse[2][0];

    float fInvDet = 1.0f / fDet;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            m[iRow][iCol] = matInverse[iRow][iCol] * fInvDet;
    }
}
///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DMATRIX4
//
///////////////////////////////////////////////////////////////////////////

//	Matrix4 * vector3
A3DVECTOR3 operator * (const A3DVECTOR3& v, const A3DMATRIX4& mat)
{
/*	A3DVECTOR3 vRet(v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41,
					v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42,
					v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43);
	float w = 1.0f / (v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44);
	return vRet *= w;
*/
	A3DVECTOR3 vRet;
	D3DXVec3TransformCoord((D3DXVECTOR3*)&vRet, (const D3DXVECTOR3*)&v, (const D3DXMATRIX*)&mat);
	return vRet;
}

//	Vector3 * matrix4
A3DVECTOR3 operator * (const A3DMATRIX4& mat, const A3DVECTOR3& v)
{
/*	A3DVECTOR3 vRet(v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41,
					v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42,
					v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43);
	float w = 1.0f / (v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44);
	return vRet *= w;
*/
	A3DVECTOR3 vRet;
	D3DXVec3TransformCoord((D3DXVECTOR3*)&vRet, (const D3DXVECTOR3*)&v, (const D3DXMATRIX*)&mat);
	return vRet;
}

//	Matrix4 * vector4
A3DVECTOR4 operator * (const A3DVECTOR4& v, const A3DMATRIX4& mat)
{
	return A3DVECTOR4(v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + v.w * mat._41,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + v.w * mat._42,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + v.w * mat._43,
					  v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + v.w * mat._44);
}

A3DVECTOR4 operator * (const A3DMATRIX4& mat, const A3DVECTOR4& v)
{
	return A3DVECTOR4(v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + v.w * mat._41,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + v.w * mat._42,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + v.w * mat._43,
					  v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + v.w * mat._44);
}

A3DMATRIX4 operator * (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4 matRet;
/*	matRet.Clear();

	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
		{
			for (int k=0; k < 4; k++)
				matRet.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
		}
	}
*/
	D3DXMatrixMultiply((D3DXMATRIX*)&matRet, (const D3DXMATRIX*)&mat1, (const D3DXMATRIX*)&mat2);

	return matRet;
}

//	Scale matrix4
A3DMATRIX4 operator * (const A3DMATRIX4& mat, float s)
{
	A3DMATRIX4 matRet;
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = mat.m[i][j] * s;
	}

	return matRet;
}

A3DMATRIX4& A3DMATRIX4::operator *= (float s)
{
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			m[i][j] *= s;
	}

	return *this;
}

A3DMATRIX4 operator + (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4 matRet;
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = mat1.m[i][j] + mat2.m[i][j];
	}
	
	return matRet;
}

//	- operator
A3DMATRIX4 operator - (const A3DMATRIX4& mat1, const A3DMATRIX4& mat2)
{
	A3DMATRIX4 matRet;
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = mat1.m[i][j] - mat2.m[i][j];
	}
	
	return matRet;
}

//	+= operator
A3DMATRIX4& A3DMATRIX4::operator += (const A3DMATRIX4& mat)
{
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			m[i][j] += mat.m[i][j];
	}

	return *this;
}

//	-= operator
A3DMATRIX4& A3DMATRIX4::operator -= (const A3DMATRIX4& mat)
{
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			m[i][j] -= mat.m[i][j];
	}

	return *this;
}

//	Clear all elements to zero
void A3DMATRIX4::Clear()
{
	_11 = _12 = _13 = _14 = 0.0f;
	_21 = _22 = _23 = _24 = 0.0f;
	_31 = _32 = _33 = _34 = 0.0f;
	_41 = _42 = _43 = _44 = 0.0f;
}

//	Set matrix to identity matrix
void A3DMATRIX4::Identity()
{
	_12 = _13 = _14 = 0.0f;
	_21 = _23 = _24 = 0.0f;
	_31 = _32 = _34 = 0.0f;
	_41 = _42 = _43 = 0.0f;
	_11 = _22 = _33 = _44 = 1.0f;
}

//	Transpose matrix
void A3DMATRIX4::Transpose()
{
	float t;
	t = _12; _12 = _21; _21 = t;
	t = _13; _13 = _31; _31 = t;
	t = _14; _14 = _41; _41 = t;
	t = _23; _23 = _32; _32 = t;
	t = _24; _24 = _42; _42 = t;
	t = _34; _34 = _43; _43 = t;
}

//	Get transpose matrix of this matrix
A3DMATRIX4 A3DMATRIX4::GetTranspose() const
{
	A3DMATRIX4 matRet;
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = m[j][i];
	}

	return matRet;
}

//	Set translation matrix
void A3DMATRIX4::Translate(float x, float y, float z)
{
	Identity();
	_41 = x; 
	_42 = y;
	_43 = z;
}

//	fRad rotate radian
void A3DMATRIX4::RotateX(float fRad)
{
	Identity();
	m[2][2] = m[1][1] = (float)cos(fRad);
	m[1][2] = (float)sin(fRad);
	m[2][1] = -m[1][2];
}

//	fRad rotate radian
void A3DMATRIX4::RotateY(float fRad)
{
	Identity();
	m[2][2] = m[0][0] = (float)cos(fRad);
	m[2][0] = (float)sin(fRad);
	m[0][2] = -m[2][0];
}

//	fRad rotate radian
void A3DMATRIX4::RotateZ(float fRad)
{
	Identity();
	m[1][1] = m[0][0] = (float)cos(fRad);
	m[0][1] = (float)sin(fRad);
	m[1][0] = -m[0][1];
}

/*	fRad rotate radian

	v: normalized axis
	fRad: rotation radian
*/
void A3DMATRIX4::RotateAxis(const A3DVECTOR3& v, float fRad)
{
/*	float fCos = (float)cos(fRad);
	float fSin = (float)sin(fRad);

	_11 = (v.m[0] * v.m[0]) * (1.0f - fCos) + fCos;
	_21 = (v.m[0] * v.m[1]) * (1.0f - fCos) - (v.m[2] * fSin);
	_31 = (v.m[0] * v.m[2]) * (1.0f - fCos) + (v.m[1] * fSin);

	_12 = (v.m[1] * v.m[0]) * (1.0f - fCos) + (v.m[2] * fSin);
	_22 = (v.m[1] * v.m[1]) * (1.0f - fCos) + fCos;
	_32	= (v.m[1] * v.m[2]) * (1.0f - fCos) - (v.m[0] * fSin);

	_13	= (v.m[2] * v.m[0]) * (1.0f - fCos) - (v.m[1] * fSin);
	_23	= (v.m[2] * v.m[1]) * (1.0f - fCos) + (v.m[0] * fSin);
	_33 = (v.m[2] * v.m[2]) * (1.0f - fCos) + fCos;

	_14 = _24 = _34 = 0.0f;
	_41 = _42 = _43 = 0.0f;
	_44 = 1.0f;
*/
	D3DXMatrixRotationAxis((D3DXMATRIX*)this, (D3DXVECTOR3*)&v, fRad);
}

/*	Rotate camera round an axis

	vPos: start position of rotate axis
	vAxis: rotate axis
	fRad: rotate radian
*/
void A3DMATRIX4::RotateAxis(const A3DVECTOR3& vPos, const A3DVECTOR3& vAxis, float fRad)
{
	Translate(-vPos.x, -vPos.y, -vPos.z);

	A3DMATRIX4 mat;
	mat.RotateAxis(vAxis, fRad);
	*this *= mat;
	mat.Translate(vPos.x, vPos.y, vPos.z);
	*this *= mat;
}

//	Get determinant of this matrix
float A3DMATRIX4::Determinant() const
{
	float fDet;
	fDet  = _11 * _22 * _33 * _44;
	fDet += _21 * _32 * _43 * _14;
	fDet += _31 * _42 * _13 * _24;
	fDet += _41 * _12 * _23 * _34;
	fDet -= _41 * _32 * _23 * _14;
	fDet -= _42 * _33 * _24 * _11;
	fDet -= _43 * _34 * _21 * _12;
	fDet -= _44 * _31 * _22 * _13;
	return fDet;
}
/*
//	Get inverse matrix of this matrix
A3DMATRIX4 A3DMATRIX4::GetInverse() const
{
	float fDet = 1.0f / Determinant();

	A3DMATRIX4 mat;
	
    mat._11 = fDet * Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
    mat._12 = fDet * Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44);
    mat._13 = fDet * Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44);
    mat._14 = fDet * Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34);

    mat._21 = fDet * Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
    mat._22 = fDet * Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44);
    mat._23 = fDet * Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44);
    mat._24 = fDet * Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34);

    mat._31 = fDet * Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
    mat._32 = fDet * Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44);
    mat._33 = fDet * Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44);
    mat._34 = fDet * Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34);

	mat._41 = fDet * Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
    mat._42 = fDet * Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43);
    mat._43 = fDet * Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43);
    mat._44 = fDet * Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);

	return mat;
}
*/
//	Get inverse matrix (used only by transform matrix)
A3DMATRIX4 A3DMATRIX4::GetInverseTM() const
{
	float fDet = 1.0f / Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);

	A3DMATRIX4 mat;
	
    mat._11 = fDet * Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
    mat._12 = -fDet * Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44);
    mat._13 = fDet * Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44);
    mat._14 = -fDet * Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34);

    mat._21 = -fDet * Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
    mat._22 = fDet * Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44);
    mat._23 = -fDet * Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44);
    mat._24 = fDet * Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34);

    mat._31 = fDet * Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
    mat._32 = -fDet * Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44);
    mat._33 = fDet * Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44);
    mat._34 = -fDet * Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34);

	mat._41 = -fDet * Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
    mat._42 = fDet * Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43);
    mat._43 = -fDet * Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43);
    mat._44 = fDet * Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);

	return mat;
}

//	Build scale matrix
void A3DMATRIX4::Scale(float sx, float sy, float sz)
{
	Clear();
    _11 = sx;
    _22 = sy;
    _33 = sz;
    _44 = 1.0f;
}

//	Set rotation part of transform matrix
void A3DMATRIX4::SetRotatePart(const A3DMATRIX3& mat)
{
    for (int i=0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
            m[i][j] = mat.m[i][j];
    }
}

//	Get rotation part of transform matrix
A3DMATRIX3 A3DMATRIX4::GetRotatePart() const
{
    A3DMATRIX3 ret;

    for (int i=0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
            ret.m[i][j] = m[i][j];
    }

    return ret;
}