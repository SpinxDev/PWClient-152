/*
 * FILE: A3DTypes.cpp
 * 
 * DESCRIPTION: Implements the overloads of some structures
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include <math.h>
#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "AAssist.h"


static BOOL _KeepOrthogonal(A3DMATRIX4 mat)
{
	FLOAT vDot;
	FLOAT vNormal;
	FLOAT error = 1e-5f;

	A3DVECTOR3 x = A3DVECTOR3(mat._11, mat._12, mat._13);
	A3DVECTOR3 y = A3DVECTOR3(mat._21, mat._22, mat._23);
	A3DVECTOR3 z = A3DVECTOR3(mat._31, mat._32, mat._33);

	vNormal = x.Magnitude();
	if (fabs(fabs(vNormal) - 1.0f) > error)
		return FALSE;

	vNormal = y.Magnitude();
	if (fabs(fabs(vNormal) - 1.0f) > error)
		return FALSE;

	vNormal = y.Magnitude();
	if (fabs(fabs(vNormal) - 1.0f) > error)
		return FALSE;

	vDot = DotProduct(x, y);
	if (fabs(vDot) > error)
		return FALSE;

	vDot = DotProduct(y, z);
	if (fabs(vDot) > error)
		return FALSE;

	vDot = DotProduct(x, z);
	if (fabs(vDot) > error)
		return FALSE;
	
	vDot = DotProduct(CrossProduct(x, y), z);
	if (fabs(fabs(vDot) - 1.0f) > error)
		return FALSE;

	return TRUE;
}

//	Inverse sqrt using Newton approximation
FLOAT a3d_InvSqrt(FLOAT v)
{
	FLOAT vhalf = 0.5f * v;
	int i = *(int*)&v;
	i = 0x5f3759df - (i >> 1);
	v = *(float*)&i;
	v = v * (1.5f - vhalf * v * v);
	return v;
}

// Return min/max vector composed with min/max component of the input 2 vector
A3DVECTOR3 a3d_VecMin(const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	return A3DVECTOR3(min2(v1.x, v2.x), min2(v1.y, v2.y), min2(v1.z, v2.z));
}

A3DVECTOR3 a3d_VecMax(const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	return A3DVECTOR3(max2(v1.x, v2.x), max2(v1.y, v2.y), max2(v1.z, v2.z));
}

//	Convert a vector from view coordinates to world coordinates
A3DVECTOR3 a3d_ViewToWorld(const A3DVECTOR3& vIn, A3DMATRIX4& matView)
{
	A3DVECTOR3 vOut;
	vOut.x = vIn.x * matView._11 + vIn.y * matView._12 + vIn.z * matView._13;
	vOut.y = vIn.x * matView._21 + vIn.y * matView._22 + vIn.z * matView._23;
	vOut.z = vIn.x * matView._31 + vIn.y * matView._32 + vIn.z * matView._33;
	return vOut;
}

A3DMATRIX3 a3d_IdentityMatrix3()
{
	A3DMATRIX3 result;
    memset(&result, 0, sizeof(result));
    result._11 = result._22 = result._33 = 1.0f;
    return result;
}

A3DMATRIX4 a3d_IdentityMatrix()
{
    A3DMATRIX4 result;
    memset(&result, 0, sizeof(result));
    result._11 = result._22 = result._33 = result._44 = 1.0f;
    return result;
}

A3DMATRIX4 a3d_ZeroMatrix()
{
    A3DMATRIX4 result;
    memset(&result, 0, sizeof(result));
    return result;
}

A3DMATRIX4 a3d_ViewMatrix(const A3DVECTOR3& from, const A3DVECTOR3& dir, 
					  const A3DVECTOR3& vecUp, FLOAT roll)
{
    A3DMATRIX4 view = a3d_IdentityMatrix();
    A3DVECTOR3 up, right, view_dir;

    view_dir = Normalize(dir);
	right = CrossProduct(vecUp, dir);
    right = Normalize(right);
	up = Normalize(vecUp);
	
    view.m[0][0] = right.x;
    view.m[1][0] = right.y;
    view.m[2][0] = right.z;
    view.m[0][1] = up.x;
    view.m[1][1] = up.y;
    view.m[2][1] = up.z;
    view.m[0][2] = view_dir.x;
    view.m[1][2] = view_dir.y;
    view.m[2][2] = view_dir.z;
	
    view.m[3][0] = -DotProduct(right, from);
    view.m[3][1] = -DotProduct(up, from);
    view.m[3][2] = -DotProduct(view_dir, from);

	// Set roll
	if (roll != 0.0f)
		view = a3d_RotateZ(-roll) * view;
	
    return view;
} 

A3DMATRIX4 a3d_LookAtMatrix(const A3DVECTOR3& from, const A3DVECTOR3& to, const A3DVECTOR3& vecUp, FLOAT roll)
{
	A3DMATRIX4 mat;
	D3DXMatrixLookAtLH((D3DXMATRIX*)&mat, (D3DXVECTOR3*)&from, (D3DXVECTOR3*)&to, (D3DXVECTOR3*)&vecUp);

	//	Set roll
	if (roll != 0.0f)
		mat = a3d_RotateZ(-roll) * mat;

	return mat;
}

A3DMATRIX4 a3d_Transpose(const A3DMATRIX4& tm)
{
	A3DMATRIX4 matT;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
			matT.m[i][j] = tm.m[j][i];
	}

	return matT;
}

void a3d_Transpose(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut)
{
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			pmatOut->m[i][j] = matIn.m[j][i];
	}
}

void a3d_DecomposeMatrix(const A3DMATRIX4& mat, A3DVECTOR3& scale, A3DQUATERNION& orient, A3DVECTOR3& pos)
{
	A3DVECTOR3	vecX, vecY, vecZ;
	
	vecX = mat.GetRow(0);
	vecY = mat.GetRow(1);
	vecZ = mat.GetRow(2);

	scale.x = Magnitude(vecX);
	scale.y = Magnitude(vecY);
	scale.z = Magnitude(vecZ);

	pos  = mat.GetRow(3);

	A3DMATRIX4 matOrient = IdentityMatrix();
	matOrient.SetRow(0, vecX / scale.x);
	matOrient.SetRow(1, vecY / scale.y);
	matOrient.SetRow(2, vecZ / scale.z);

	orient.ConvertFromMatrix(matOrient);
}

/*
A3DMATRIX4 InverseTM(A3DMATRIX4 tm)
{
	//We should check if tm is a unified transform matrix;
	//that is to say: each row's magnitude must be 1.0f;
	A3DMATRIX4 matInv = a3d_IdentityMatrix();

	//X axis;
	matInv._11 = tm._11;
	matInv._21 = tm._12;
	matInv._31 = tm._13;

	//Y axis;
	matInv._12 = tm._21;
	matInv._22 = tm._22;
	matInv._32 = tm._23;

	//Z axis;
	matInv._13 = tm._31;
	matInv._23 = tm._32;
	matInv._33 = tm._33;

	//Offset;
	matInv._41 = -(tm._41 * tm._11 + tm._42 * tm._12 + tm._43 * tm._13);
	matInv._42 = -(tm._41 * tm._21 + tm._42 * tm._22 + tm._43 * tm._23);
	matInv._43 = -(tm._41 * tm._31 + tm._42 * tm._32 + tm._43 * tm._33);

	return matInv;
}*/

static FLOAT _Det(FLOAT a11, FLOAT a12, FLOAT a13, 
				FLOAT a21, FLOAT a22, FLOAT a23,
				FLOAT a31, FLOAT a32, FLOAT a33)
{
    return a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 -
        a13 * a22 * a31 - a23 * a32 * a11 - a33 * a12 * a21;
}

A3DMATRIX4 a3d_InverseTM(const A3DMATRIX4& mat)
{
	A3DMATRIX4 ret;
	a3d_InverseTM(mat, &ret);
    return ret;
}

void a3d_InverseTM(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut)
{
	FLOAT vDet;

	vDet = 1.0f / _Det(matIn._11, matIn._12, matIn._13,
					   matIn._21, matIn._22, matIn._23,
					   matIn._31, matIn._32, matIn._33);

    pmatOut->_11 = vDet * _Det(matIn._22, matIn._23, matIn._24,
		matIn._32, matIn._33, matIn._34,
		matIn._42, matIn._43, matIn._44);
    pmatOut->_12 = -vDet * _Det(matIn._12, matIn._13, matIn._14,
		matIn._32, matIn._33, matIn._34,
		matIn._42, matIn._43, matIn._44);
    pmatOut->_13 = vDet * _Det(matIn._12, matIn._13, matIn._14,
		matIn._22, matIn._23, matIn._24,
		matIn._42, matIn._43, matIn._44);
    pmatOut->_14 = -vDet * _Det(matIn._12, matIn._13, matIn._14,
        matIn._22, matIn._23, matIn._24,
        matIn._32, matIn._33, matIn._34);

    pmatOut->_21 = -vDet * _Det(matIn._21, matIn._23, matIn._24,
        matIn._31, matIn._33, matIn._34,
        matIn._41, matIn._43, matIn._44);
    pmatOut->_22 = vDet * _Det(matIn._11, matIn._13, matIn._14,
        matIn._31, matIn._33, matIn._34,
        matIn._41, matIn._43, matIn._44);
    pmatOut->_23 = -vDet * _Det(matIn._11, matIn._13, matIn._14,
        matIn._21, matIn._23, matIn._24,
        matIn._41, matIn._43, matIn._44);
    pmatOut->_24 = vDet * _Det(matIn._11, matIn._13, matIn._14,
        matIn._21, matIn._23, matIn._24,
        matIn._31, matIn._33, matIn._34);

    pmatOut->_31 = vDet * _Det(matIn._21, matIn._22, matIn._24,
        matIn._31, matIn._32, matIn._34,
        matIn._41, matIn._42, matIn._44);
    pmatOut->_32 = -vDet * _Det(matIn._11, matIn._12, matIn._14,
        matIn._31, matIn._32, matIn._34,
        matIn._41, matIn._42, matIn._44);
    pmatOut->_33 = vDet * _Det(matIn._11, matIn._12, matIn._14,
        matIn._21, matIn._22, matIn._24,
        matIn._41, matIn._42, matIn._44);
    pmatOut->_34 = -vDet * _Det(matIn._11, matIn._12, matIn._14,
        matIn._21, matIn._22, matIn._24,
        matIn._31, matIn._32, matIn._34);

	pmatOut->_41 = -vDet * _Det(matIn._21, matIn._22, matIn._23,
        matIn._31, matIn._32, matIn._33,
        matIn._41, matIn._42, matIn._43);
    pmatOut->_42 = vDet * _Det(matIn._11, matIn._12, matIn._13,
        matIn._31, matIn._32, matIn._33,
        matIn._41, matIn._42, matIn._43);
    pmatOut->_43 = -vDet * _Det(matIn._11, matIn._12, matIn._13,
        matIn._21, matIn._22, matIn._23,
        matIn._41, matIn._42, matIn._43);
    pmatOut->_44 = vDet * _Det(matIn._11, matIn._12, matIn._13,
        matIn._21, matIn._22, matIn._23,
        matIn._31, matIn._32, matIn._33);
}


A3DMATRIX4 a3d_InverseMatrix(const A3DMATRIX4& mat)
{
	A3DMATRIX4 matInv;
	a3d_InverseMatrix(mat, &matInv);
	return matInv;
}

void a3d_InverseMatrix(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut)
{
	D3DXMatrixInverse((D3DXMATRIX*) pmatOut, NULL, (D3DXMATRIX*) &matIn);
}

A3DMATRIX4 a3d_TransformMatrix(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, const A3DVECTOR3& vecPos)
{
    A3DMATRIX4   mat;
    A3DVECTOR3   vecXAxis, vecYAxis, vecZAxis;

    vecZAxis = Normalize(vecDir);
    vecYAxis = Normalize(vecUp);
    vecXAxis = Normalize(CrossProduct(vecYAxis, vecZAxis));

    memset(&mat, 0, sizeof(mat));
    mat.m[0][0] = vecXAxis.x;
    mat.m[0][1] = vecXAxis.y;
    mat.m[0][2] = vecXAxis.z;

    mat.m[1][0] = vecYAxis.x;
    mat.m[1][1] = vecYAxis.y;
    mat.m[1][2] = vecYAxis.z;

    mat.m[2][0] = vecZAxis.x;
    mat.m[2][1] = vecZAxis.y;
    mat.m[2][2] = vecZAxis.z;

    mat.m[3][0] = vecPos.x;
    mat.m[3][1] = vecPos.y;
    mat.m[3][2] = vecPos.z;
    mat.m[3][3] = 1.0f;

    return mat;
}

A3DMATRIX4 a3d_RotateX(FLOAT vRad)
{
    A3DMATRIX4 ret = a3d_IdentityMatrix();
    ret.m[2][2] = ret.m[1][1] = (FLOAT) cos(vRad);
    ret.m[1][2] = (FLOAT) sin(vRad);
    ret.m[2][1] = (FLOAT) -ret.m[1][2];

	//	_KeepOrthogonal(ret);
	return ret;
}

A3DMATRIX4 a3d_RotateX(const A3DMATRIX4& mat, FLOAT vRad)
{
    return mat * a3d_RotateX(vRad);
}

A3DMATRIX4 a3d_RotateY(FLOAT vRad)
{
    A3DMATRIX4 ret = a3d_IdentityMatrix();
    ret.m[2][2] = ret.m[0][0] = (FLOAT) cos(vRad);
    ret.m[2][0] = (FLOAT) sin(vRad);
    ret.m[0][2] = -ret.m[2][0];
	//	_KeepOrthogonal(ret);
    return ret;
}

A3DMATRIX4 a3d_RotateY(const A3DMATRIX4& mat, FLOAT vRad)
{
    return mat * a3d_RotateY(vRad);
}

A3DMATRIX4 a3d_RotateZ(FLOAT vRad)
{
    A3DMATRIX4 ret = a3d_IdentityMatrix();
    ret.m[1][1] = ret.m[0][0] = (FLOAT) cos(vRad);
    ret.m[0][1] = (FLOAT) sin(vRad);
    ret.m[1][0] = -ret.m[0][1];
	//	_KeepOrthogonal(ret);
    return ret;
}

A3DMATRIX4 a3d_RotateZ(const A3DMATRIX4& mat, FLOAT vRad)
{
    return mat * a3d_RotateZ(vRad);
}

A3DMATRIX4 a3d_RotateAxis(const A3DVECTOR3& vRotAxis, FLOAT vRad)
{
	/*
		The derivation of this algorithm can be seen in rotation.pdf in my ÎÄµµ directory.
		The basic idea is to divide the original vector to two part: paralell to axis and 
		perpendicular to axis, then only perpendicular part can be affected by this rotation.
		Now divide the rotation part onto the main axis on the rotation plane then it will
		be clear to see what composes the rotated vector. Finally we can get the transform
		matrix from a set of 3 equations.
	*/
	A3DVECTOR3 vecAxis = Normalize(vRotAxis);

	A3DMATRIX4 ret;
	FLOAT xx, xy, xz, yy, yz, zz, cosine, sine, one_cs, xsine, ysine, zsine;

	xx = vecAxis.x * vecAxis.x;
	xy = vecAxis.x * vecAxis.y;
	xz = vecAxis.x * vecAxis.z;
	yy = vecAxis.y * vecAxis.y;
	yz = vecAxis.y * vecAxis.z;
	zz = vecAxis.z * vecAxis.z;

	cosine = (FLOAT)cos(vRad);
	sine = (FLOAT)sin(vRad);
	one_cs = 1.0f - cosine;

	xsine = vecAxis.x * sine;
	ysine = vecAxis.y * sine;
	zsine = vecAxis.z * sine;
	
	ret._11 = xx + cosine * (1.0f - xx);
	ret._12 = xy * one_cs + zsine;
	ret._13 = xz * one_cs - ysine;
	ret._14 = 0.0f;

	ret._21 = xy * one_cs - zsine;
	ret._22 = yy + cosine * (1.0f - yy);
	ret._23 = yz * one_cs + xsine;
	ret._24 = 0.0f;

	ret._31 = xz * one_cs + ysine;
	ret._32 = yz * one_cs - xsine;
	ret._33 = zz + cosine * (1.0f - zz);
	ret._34 = 0.0f;

	ret._41 = 0.0f;
	ret._42 = 0.0f;
	ret._43 = 0.0f;
	ret._44 = 1.0f;
	
	//D3DXMatrixRotationAxis((D3DXMATRIX *)&ret, (D3DXVECTOR3*)&vecAxis, vRad);
	return ret;
}

A3DMATRIX4 a3d_RotateAxis(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecAxis, FLOAT vRad)
{
	A3DMATRIX4 ret = a3d_Translate(-vecPos.x, -vecPos.y, -vecPos.z);
	ret = ret * a3d_RotateAxis(vecAxis, vRad);
	ret = ret * a3d_Translate(vecPos.x, vecPos.y, vecPos.z);

	return ret;
}

//	Rotate a position around axis X
A3DVECTOR3 a3d_RotatePosAroundX(const A3DVECTOR3& vPos, float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateX(fRad);
	return vPos * mat;
}

//	Rotate a position around axis Y
A3DVECTOR3 a3d_RotatePosAroundY(const A3DVECTOR3& vPos, float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateY(fRad);
	return vPos * mat;
}

//	Rotate a position around axis Z
A3DVECTOR3 a3d_RotatePosAroundZ(const A3DVECTOR3& vPos, float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateZ(fRad);
	return vPos * mat;
}

/*	Rotate a position around arbitrary axis

	Return result position.

	vPos: position will do rotate
	vAxis: normalized axis
	fRad: rotation radian
*/
A3DVECTOR3 a3d_RotatePosAroundAxis(const A3DVECTOR3& vPos, const A3DVECTOR3& vAxis, float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateAxis(vAxis, fRad);
	return vPos * mat;
}

/*	Rotate a position around a line
	
	Return result position.

	vPos: position will do rotate
	vOrigin: point on line
	vDir: normalized line's direction
	fRad: rotation radian
*/
A3DVECTOR3 a3d_RotatePosAroundLine(const A3DVECTOR3& vPos, const A3DVECTOR3& vOrigin, const A3DVECTOR3& vDir, float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateAxis(vOrigin, vDir, fRad);
	return vPos * mat;
}

/*	Rotate a position around a line
	
	Return result vector.

	v: vector will do rotate
	vOrigin: point on line
	vDir: normalized line's direction
	fRad: rotation radian
*/
A3DVECTOR3 a3d_RotateVecAroundLine(const A3DVECTOR3& v, const A3DVECTOR3& vOrigin, const A3DVECTOR3& vDir, float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateAxis(vOrigin, vDir, fRad);
	return v * mat - mat.GetRow(3);
}

A3DMATRIX4 a3d_Scaling(FLOAT sx, FLOAT sy, FLOAT sz)
{
    A3DMATRIX4 ret = ZeroMatrix();
    ret.m[0][0] = sx;
    ret.m[1][1] = sy;
    ret.m[2][2] = sz;
    ret.m[3][3] = 1.0f;
    return ret;
}

A3DMATRIX4 a3d_Scaling(const A3DMATRIX4& mat, FLOAT sx, FLOAT sy, FLOAT sz)
{
    A3DMATRIX4 ret = mat;
    ret.m[0][0] *= sx;
    ret.m[0][1] *= sx;
    ret.m[0][2] *= sx;
    ret.m[0][3] *= sx;

    ret.m[1][0] *= sy;
    ret.m[1][1] *= sy;
    ret.m[1][2] *= sy;
    ret.m[1][3] *= sy;

    ret.m[2][0] *= sz;
    ret.m[2][1] *= sz;
    ret.m[2][2] *= sz;
    ret.m[2][3] *= sz;
    return ret;
}

A3DMATRIX4 a3d_ScalingRelative(const A3DMATRIX4& matRoot, FLOAT sx, FLOAT sy, FLOAT sz)
{
	A3DMATRIX4 matScale; 
	matScale = InverseTM(matRoot) * a3d_Scaling(sx, sy, sz) * matRoot;

	return matScale;
}

A3DMATRIX4 a3d_Translate(FLOAT x, FLOAT y, FLOAT z)
{
	A3DMATRIX4 ret = a3d_IdentityMatrix();

	ret._41 = x;
	ret._42 = y;
	ret._43 = z;

	return ret;
}

A3DMATRIX4 a3d_ScaleAlongAxis(const A3DVECTOR3& vAxis, float fScale)
{
/*	A3DMATRIX4 mat1, mat2;

	mat1.Clear();
	mat1._11 = mat1._22 = mat1._33 = 1.0f;
	mat1._14 = vAxis.x;
	mat1._24 = vAxis.y;
	mat1._34 = vAxis.z;

	mat2.Clear();
	mat2._11 = mat2._22 = mat2._33 = 1.0f;
	mat2._41 = (fScale - 1.0f) * vAxis.x;
	mat2._42 = (fScale - 1.0f) * vAxis.y;
	mat2._43 = (fScale - 1.0f) * vAxis.z;

	mat = mat1 * mat2;
	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
*/
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);

	float s = fScale - 1.0f;
	float f1 = s * vAxis.x * vAxis.y;
	float f2 = s * vAxis.y * vAxis.z;
	float f3 = s * vAxis.x * vAxis.z;

	mat._11 = 1 + s * vAxis.x * vAxis.x;
	mat._22 = 1 + s * vAxis.y * vAxis.y;
	mat._33 = 1 + s * vAxis.z * vAxis.z;
	mat._12 = mat._21 = f1;
	mat._13 = mat._31 = f3;
	mat._23 = mat._32 = f2;

	return mat;
}

//	Multiply a vertex but don't consider translation factors (the fourth row of matrix),
//	this function can be used to transform a normal. 
A3DVECTOR3 a3d_VectorMatrix3x3(const A3DVECTOR3& v, const A3DMATRIX4& mat)
{
	return A3DVECTOR3(v.x * mat._11 + v.y * mat._21 + v.z * mat._31,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33);
}

A3DCOLOR a3d_ColorValueToColorRGBA(A3DCOLORVALUE colorValue)
{
	int r, g, b, a;

	a = (int)(colorValue.a * 255);
	r = (int)(colorValue.r * 255);
	g = (int)(colorValue.g * 255);
	b = (int)(colorValue.b * 255);

	if( a > 255 ) a = 255;
	if( r > 255 ) r = 255;
	if( g > 255 ) g = 255;
	if( b > 255 ) b = 255;
	return A3DCOLORRGBA(r, g, b, a);
}

A3DCOLORVALUE a3d_ColorRGBAToColorValue(A3DCOLOR colorRGBA)
{
	BYTE r, g, b, a;
	static FLOAT fTemp = 1.0f / 255.0f;
						
	a = A3DCOLOR_GETALPHA(colorRGBA);
	r = A3DCOLOR_GETRED(colorRGBA);
	g = A3DCOLOR_GETGREEN(colorRGBA);
	b = A3DCOLOR_GETBLUE(colorRGBA);

	return A3DCOLORVALUE(r * fTemp, g * fTemp, b * fTemp, a * fTemp);
}

A3DVECTOR3 a3d_RandDirH()
{
	A3DVECTOR3 vecDirH;

	float vRad = (rand() % 10000) / 10000.0f * 2.0f * A3D_PI;
	
	vecDirH.x = (FLOAT) cos(vRad);
	vecDirH.y = 0.0f;
	vecDirH.z = (FLOAT) sin(vRad);

	return vecDirH;
}

A3DMATRIX4 a3d_MirrorMatrix(const A3DVECTOR3& p, // IN: point on the plane
							const A3DVECTOR3& n) // IN: plane perpendicular direction
{
	A3DMATRIX4 ret;

	//V' = V - 2((V - P)*N)N)
	FLOAT dot = p.x*n.x + p.y*n.y + p.z*n.z;
	
	ret.m[0][0] = 1 - 2*n.x*n.x;
	ret.m[1][0] = - 2*n.x*n.y;
	ret.m[2][0] = - 2*n.x*n.z;
	ret.m[3][0] = 2*dot*n.x;

	ret.m[0][1] = - 2*n.y*n.x;
	ret.m[1][1] = 1 - 2*n.y*n.y;
	ret.m[2][1] = - 2*n.y*n.z;
	ret.m[3][1] = 2*dot*n.y;

	ret.m[0][2] = - 2*n.z*n.x;
	ret.m[1][2] = - 2*n.z*n.y;
	ret.m[2][2] = 1 - 2*n.z*n.z;
	ret.m[3][2] = 2*dot*n.z;

	ret.m[0][3] = 0;
	ret.m[1][3] = 0;
	ret.m[2][3] = 0;
	ret.m[3][3] = 1;

	return ret;
}

//	Clear AABB
void a3d_ClearAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs)
{
	vMins.Set(999999.0f, 999999.0f, 999999.0f);
	vMaxs.Set(-999999.0f, -999999.0f, -999999.0f);
}

//Use a vertex point to expand an AABB data;
void a3d_AddVertexToAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint)
{
	for (int i=0; i < 3; i++)
	{
		if (vPoint.m[i] < vMins.m[i])
			vMins.m[i] = vPoint.m[i];
	
		if (vPoint.m[i] > vMaxs.m[i])
			vMaxs.m[i] = vPoint.m[i];
	}
}

//Use a OBB data to expand an AABB data;
void a3d_ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DOBB& obb)
{
	A3DVECTOR3 v[8];

	//Up 4 corner;
	v[0] = obb.Center + obb.ExtY - obb.ExtX + obb.ExtZ;
	v[1] = v[0] + obb.ExtX + obb.ExtX;	//	+ obb.ExtX * 2.0f;
	v[2] = v[1] - obb.ExtZ - obb.ExtZ;	//	+ obb.ExtZ * (-2.0f);
	v[3] = v[2] - obb.ExtX - obb.ExtX;	//	+ obb.ExtX * (-2.0f);

	//Down 4 corner;
	v[4] = obb.Center - obb.ExtY - obb.ExtX + obb.ExtZ;
	v[5] = v[4] + obb.ExtX + obb.ExtX;	//	+ obb.ExtX * 2.0f;
	v[6] = v[5] - obb.ExtZ - obb.ExtZ;	//	+ obb.ExtZ * (-2.0f);
	v[7] = v[6] - obb.ExtX - obb.ExtX;	//	+ obb.ExtX * (-2.0f);

	for(int i=0; i<3; i++)
	{
		for(int j=0; j<8; j++)
		{
			if( vecMins.m[i] > v[j].m[i] )
				vecMins.m[i] = v[j].m[i];

			if( vecMaxs.m[i] < v[j].m[i] )
				vecMaxs.m[i] = v[j].m[i];
		}
	}
}

//Use a sub AABB data to expand an AABB data;
void a3d_ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DAABB& subAABB)
{
	for (int i=0; i<3; i++)
	{
		if (vecMins.m[i] > subAABB.Mins.m[i])
			vecMins.m[i] = subAABB.Mins.m[i];
		if (vecMaxs.m[i] < subAABB.Maxs.m[i])
			vecMaxs.m[i] = subAABB.Maxs.m[i];
	}
}

void a3d_HSVToRGB(A3DHSVCOLORVALUE& hsvColor, A3DCOLOR * pRgbColor)
{
	FLOAT aa, bb, cc,f;
	int r, g, b, a;

	FLOAT h = hsvColor.h;
	FLOAT s = hsvColor.s;
	FLOAT v = hsvColor.v * 255.0f + 0.5f;

	a = (int)(hsvColor.a * 255.0f);

	if( s == 0 )
		r = g = b = (int)v;
	else
	{
		if( h == 1.0f )
			h = 0.0f;

		h *= 6.0f;
		int i = int(floor(h));
		f = h - i;
		aa = v * (1 - s);
		bb = v * (1 - s * f);
		cc = v * (1 - s * (1 - f));

		switch(i)
		{
		case 0: r = (int)v;	 g = (int)cc; b = (int)aa; break;
		case 1: r = (int)bb; g = (int)v;  b = (int)aa; break;
		case 2: r = (int)aa; g = (int)v;  b = (int)cc; break;
		case 3:	r = (int)aa; g = (int)bb; b = (int)v;  break;
		case 4: r = (int)cc; g = (int)aa; b = (int)v;  break;
		case 5: r = (int)v;  g = (int)aa; b = (int)bb; break;
		}
	}

	*pRgbColor = A3DCOLORRGBA(r, g, b, a);
}

void a3d_RGBToHSV(A3DCOLOR rgbColor, A3DHSVCOLORVALUE * pHsvColor)
{
	FLOAT h = 0.0f;

	FLOAT r = A3DCOLOR_GETRED  (rgbColor) / 255.0f;
	FLOAT g = A3DCOLOR_GETGREEN(rgbColor) / 255.0f;
	FLOAT b = A3DCOLOR_GETBLUE (rgbColor) / 255.0f;
	FLOAT a = A3DCOLOR_GETALPHA(rgbColor) / 255.0f;

	float vMax = max2(r, max2(g, b)), vMin = min2(r, min2(g, b));
	float vDelta = vMax - vMin;

	pHsvColor->v = vMax;
	pHsvColor->a = a;

	if( vMax != 0.0f )
		pHsvColor->s = vDelta / vMax;
	else
		pHsvColor->s = 0.0f;

	if( pHsvColor->s == 0.0f ) 
		h = 0.0f;
	else
	{
		if( r == vMax )
			h = (g - b) / vDelta;
		else if( g == vMax)
			h = 2.0f + (b - r) / vDelta;
		else if( b == vMax )
			h = 4.0f + (r - g) / vDelta;
		
		h *= 60.0f;
		if( h < 0.0f ) h += 360.0f;
		h /= 360.0f;
	}
	pHsvColor->h = h;
}

void a3d_RGBToHSL(float r, float g, float b, float&  h, float& s, float& l)
{
	h = 0.0f;

	float vMax = max(r, max(g, b)), vMin = min(r, min(g, b));
	float vDelta = vMax - vMin;

	l = (vMax + vMin) / 2.0f;

	if( l == 0.0f || vDelta == 0.0f )
		s = 0.0f;
	else if( l <= 0.5f )
		s = vDelta / (2.0f * l);
	else
		s = vDelta / (2.0f - 2.0f*l);


	if( s == 0.0f ) 
		h = 0.0f;
	else
	{
		if( r == vMax )
			h = (g - b) / vDelta;
		else if( g == vMax)
			h = 2.0f + (b - r) / vDelta;
		else if( b == vMax )
			h = 4.0f + (r - g) / vDelta;

		h *= 60.0f;
		if( h < 0.0f ) h += 360.0f;
		h /= 360.0f;
	}
}

void a3d_DecompressColor(A3DCOLOR rgbColor, BYTE * pR, BYTE * pG, BYTE * pB, BYTE * pA)
{
	if( pR )
		*pR = A3DCOLOR_GETRED(rgbColor);
	if( pG )
		*pG = A3DCOLOR_GETGREEN(rgbColor);
	if( pB )
		*pB = A3DCOLOR_GETBLUE(rgbColor);
	if( pA )
		*pA = A3DCOLOR_GETALPHA(rgbColor);
}

// Get the dir and up of a view within the cube map
// 0 ---- right
// 1 ---- left
// 2 ---- top
// 3 ---- bottom
// 4 ---- front
// 5 ---- back
bool a3d_GetCubeMapDirAndUp(int nFaceIndex, A3DVECTOR3 * pDir, A3DVECTOR3 * pUp)
{
	switch(nFaceIndex)
	{
	case 0: // Right
		*pDir	= A3DVECTOR3(1.0f, 0.0f, 0.0f);
		*pUp	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
		break;
	case 1: // Left
		*pDir	= A3DVECTOR3(-1.0f, 0.0f, 0.0f);
		*pUp	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
		break;
	case 2: // Top
		*pDir	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
		*pUp	= A3DVECTOR3(0.0f, 0.0f, -1.0f);
		break;
	case 3: // Bottom
		*pDir	= A3DVECTOR3(0.0f, -1.0f, 0.0f);
		*pUp	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
		break;
	case 4: // Front
		*pDir	= A3DVECTOR3(0.0f, 0.0f, 1.0f);
		*pUp	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
		break;
	case 5: // Back
		*pDir	= A3DVECTOR3(0.0f, 0.0f, -1.0f);
		*pUp	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
		break;
	default: // Error
		*pDir	= A3DVECTOR3(0.0f);
		*pUp	= A3DVECTOR3(0.0f);
		return false;
	}
	return true;
}

D3DVERTEXELEMENT9 a3d_SetVSDecl(WORD Stream, WORD Offset, BYTE Type, BYTE Method,BYTE Usage, BYTE UsageIndex)
{
	D3DVERTEXELEMENT9 Element;
	Element.Stream = Stream;
	Element.Offset = Offset;
	Element.Type = Type;
	Element.Method = Method;
	Element.Usage = Usage;
	Element.UsageIndex = UsageIndex;
	return Element;
}

//	Build standard vertex shader declarator from vertex type
void a3d_MakeVSDeclarator(A3DVERTEXTYPE VertexType, int iStreamIdx, D3DVERTEXELEMENT9* pDecl)
{
	int iCnt = 0;
	switch (VertexType)
	{
	case A3DVT_VERTEX:
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			break;
	case A3DVT_VERTEXTAN:
		{
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0);
		}
		break;
	case A3DVT_LVERTEX:	
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			break;
	case A3DVT_TLVERTEX:	
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			break;
	case A3DVT_IBLVERTEX:
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1);
			break;
	case A3DVT_BVERTEX1:
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			break;
	case A3DVT_BVERTEX2:
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			break;
	case A3DVT_BVERTEX3:
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			break;
	case A3DVT_BVERTEX3TAN:
		{
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0);
		}
		break;
	default:
		ASSERT(0);
	}

	pDecl[iCnt++] = a3d_SetVSDecl(0xFF,0,D3DDECLTYPE_UNUSED,0,0,0);//D3DDECL_END();
	return;
}



//	Build vertex shader declarator from FVF
void a3d_MakeVSDeclarator(DWORD dwFVF, D3DVERTEXELEMENT9* pDecl)
{
	D3DXDeclaratorFromFVF(dwFVF, pDecl);
}

A3DMATRIX3 a3d_Matrix3Rotate(float fRad)
{
	A3DMATRIX3 mat;
	mat.Rotate(fRad);
	return mat;
}

A3DMATRIX3 a3d_Matrix3Rotate(const A3DMATRIX3& mat, float fRad)
{
    return mat * a3d_Matrix3Rotate(fRad);
}

A3DMATRIX3 a3d_Matrix3Translate(float x, float y)
{
	A3DMATRIX3 mat;
	mat.Translate(x, y);
	return mat;
}

A3DMATRIX3 a3d_Matrix3Translate(const A3DMATRIX3& mat, float x, float y)
{
	return mat * a3d_Matrix3Translate(x, y);
}

float a3d_DistSquareToAABB(const A3DVECTOR3& vPos, const A3DAABB& aabb)
{
    float d = 0;
    for (int i = 0; i < 3; i++)
    {
        if (vPos.m[i] < aabb.Mins.m[i])
        {
            float s = vPos.m[i] - aabb.Mins.m[i];
            d += s * s;
        }
        else if (vPos.m[i] > aabb.Maxs.m[i])
        {
            float s = vPos.m[i] - aabb.Maxs.m[i];
            d += s * s;
        }
    }
    return d;
}