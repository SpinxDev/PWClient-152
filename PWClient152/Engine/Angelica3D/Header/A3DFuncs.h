/*
 * FILE: A3DFuncs.h
 *
 * DESCRIPTION: some data operation routines for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DFUNCS_H_
#define _A3DFUNCS_H_

#include <math.h>
#include "A3DTypes.h"
#include "A3DMacros.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"

//	Fast inverse square root calculation
FLOAT a3d_InvSqrt(FLOAT v);
//	Length-related functions
inline FLOAT a3d_SquareMagnitude(const A3DVECTOR3& v) {return v.x * v.x + v.y * v.y + v.z * v.z;}
inline FLOAT a3d_Magnitude(const A3DVECTOR3& v) {return (FLOAT)(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));}
inline FLOAT a3d_MagnitudeH(const A3DVECTOR3& v) {return (FLOAT)(sqrt(v.x * v.x + v.z * v.z));}

inline A3DVECTOR3 a3d_Vector423(const A3DVECTOR4& v) {return A3DVECTOR3(v.x / v.w, v.y / v.w, v.z / v.w);}

//	Returns vector with same direction and unit length
inline A3DVECTOR3 a3d_Normalize(const A3DVECTOR3& v)
{
	FLOAT mag = v.Magnitude();
	if (mag < 1e-12	&& mag > -1e-12)
		return A3DVECTOR3(0.0f);
	else
		return v / mag;
}

//	Compress horizontal direction to a byte
BYTE a3d_CompressDirH(float x, float z);
//	Decompress horizontal direction
A3DVECTOR3 a3d_DecompressDirH(BYTE byDir);
//	Compress direction to two bytes
void a3d_CompressDir(const A3DVECTOR3& v, BYTE& b1, BYTE& b2);
//	Decompress direction from two bytes
A3DVECTOR3 a3d_DecompressDir(BYTE b1, BYTE b2);

//	Return min/max vector composed with min/max component of the input 2 vector
A3DVECTOR3 a3d_VecMin(const A3DVECTOR3& v1, const A3DVECTOR3& v2); 
A3DVECTOR3 a3d_VecMax(const A3DVECTOR3& v1, const A3DVECTOR3& v2); 

//	Convert a vector from view coordinates to world coordinates
A3DVECTOR3 a3d_ViewToWorld(const A3DVECTOR3& vIn, A3DMATRIX4& matView);

//	Matrix initialize;
A3DMATRIX3 a3d_IdentityMatrix3();
A3DMATRIX4 a3d_IdentityMatrix();
A3DMATRIX4 a3d_ZeroMatrix();
A3DMATRIX4 a3d_ViewMatrix(const A3DVECTOR3& from, const A3DVECTOR3& dir, const A3DVECTOR3& vecUp, FLOAT roll);
A3DMATRIX4 a3d_LookAtMatrix(const A3DVECTOR3& from, const A3DVECTOR3& to, const A3DVECTOR3& vecUp, FLOAT roll);
A3DMATRIX4 a3d_TransformMatrix(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, const A3DVECTOR3& vecPos);

//	Matrix manipulation;
A3DMATRIX4 a3d_RotateX(FLOAT vRad);
A3DMATRIX4 a3d_RotateX(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 a3d_RotateY(FLOAT vRad);
A3DMATRIX4 a3d_RotateY(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 a3d_RotateZ(FLOAT vRad);
A3DMATRIX4 a3d_RotateZ(const A3DMATRIX4& mat, FLOAT vRad);
A3DMATRIX4 a3d_RotateAxis(const A3DVECTOR3& vecAxis, FLOAT vRad);
A3DMATRIX4 a3d_RotateAxis(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecAxis, FLOAT vRad);

//	Position and vector rotating routines
A3DVECTOR3 a3d_RotatePosAroundX(const A3DVECTOR3& vPos, float fRad);
A3DVECTOR3 a3d_RotatePosAroundY(const A3DVECTOR3& vPos, float fRad);
A3DVECTOR3 a3d_RotatePosAroundZ(const A3DVECTOR3& vPos, float fRad);
A3DVECTOR3 a3d_RotatePosAroundAxis(const A3DVECTOR3& vPos, const A3DVECTOR3& vAxis, float fRad);
A3DVECTOR3 a3d_RotatePosAroundLine(const A3DVECTOR3& vPos, const A3DVECTOR3& vOrigin, const A3DVECTOR3& vDir, float fRad);
A3DVECTOR3 a3d_RotateVecAroundLine(const A3DVECTOR3& v, const A3DVECTOR3& vOrigin, const A3DVECTOR3& vDir, float fRad);

A3DMATRIX4 a3d_Scaling(FLOAT sx, FLOAT sy, FLOAT sz);
A3DMATRIX4 a3d_Scaling(const A3DMATRIX4& mat, FLOAT sx, FLOAT sy, FLOAT sz);
A3DMATRIX4 a3d_ScalingRelative(const A3DMATRIX4& matRoot, FLOAT sx, FLOAT sy, FLOAT sz);
A3DMATRIX4 a3d_Translate(FLOAT x, FLOAT y, FLOAT z);
A3DMATRIX4 a3d_ScaleAlongAxis(const A3DVECTOR3& vAxis, float fScale);

A3DMATRIX4 a3d_Transpose(const A3DMATRIX4& mat);
void a3d_Transpose(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut);

void a3d_DecomposeMatrix(const A3DMATRIX4& mat, A3DVECTOR3& scale, A3DQUATERNION& orient, A3DVECTOR3& pos);

// Get the dir and up of a view within the cube map
// 0 ---- right
// 1 ---- left
// 2 ---- top
// 3 ---- bottom
// 4 ---- front
// 5 ---- back
bool a3d_GetCubeMapDirAndUp(int nFaceIndex, A3DVECTOR3 * pDir, A3DVECTOR3 * pUp);

//Color Value and Color RGB Convertion;
A3DCOLOR a3d_ColorValueToColorRGBA(A3DCOLORVALUE colorValue);
A3DCOLORVALUE a3d_ColorRGBAToColorValue(A3DCOLOR colorRGBA);

A3DMATRIX4 a3d_MirrorMatrix(const A3DVECTOR3 &p, const A3DVECTOR3 &n);

//	Note: a3d_InverseTM can only be used on transform matrix. A transform matrix is a
//	translation, rotation or scale matrix, it also can be a matrix combined by these
//	three type matrices. For calcuating an arbitrary matrix's inverse, use a3d_InverseMatrix
A3DMATRIX4 a3d_InverseTM(const A3DMATRIX4& mat);
void a3d_InverseTM(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut);
A3DMATRIX4 a3d_InverseMatrix(const A3DMATRIX4& mat);
void a3d_InverseMatrix(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut);



//	AABB operations
void a3d_ClearAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs);
void a3d_AddVertexToAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint);
void a3d_ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DOBB& obb);
void a3d_ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DAABB& subAABB);

inline void a3d_BuildAABB(A3DAABB* pAABB, const A3DOBB& obb) { pAABB->Build(obb); }
inline void a3d_MergeAABB(A3DAABB* pAABB, const A3DAABB& subAABB) { pAABB->Merge(subAABB); }
inline void a3d_CompleteAABB(A3DAABB* pAABB) { pAABB->CompleteCenterExts(); }

//	OBB operations;
inline void a3d_CompleteOBB(A3DOBB* pOBB) { pOBB->CompleteExtAxis(); }

A3DVECTOR3 a3d_RandDirH();

//	Convert a color from HSV to RGB
//	h, s, v is in [0.0f, 1.0f]
void a3d_HSVToRGB(A3DHSVCOLORVALUE& hsvColor, A3DCOLOR * pRgbColor);
//	Convert a color from RGB to HSV
//	h, s, v is in [0.0f, 1.0f]
void a3d_RGBToHSV(A3DCOLOR rgbColor, A3DHSVCOLORVALUE * pHsvColor);
void a3d_RGBToHSL(float r, float g, float b, float&  h, float& s, float& l);
//	Convert a color from RGB to seperate r, g, b value;
void a3d_DecompressColor(A3DCOLOR rgbColor, BYTE * pR, BYTE * pG, BYTE * pB, BYTE *pA=NULL);

A3DMATRIX3 a3d_Matrix3Rotate(float fRad);
A3DMATRIX3 a3d_Matrix3Rotate(const A3DMATRIX3& mat, float fRad);
A3DMATRIX3 a3d_Matrix3Translate(float x, float y);
A3DMATRIX3 a3d_Matrix3Translate(const A3DMATRIX3& mat, float x, float y);

D3DVERTEXELEMENT9 a3d_SetVSDecl(WORD Stream, WORD Offset, BYTE Type, BYTE Method,BYTE Usage, BYTE UsageIndex);
//	Build standard vertex shader declarator from vertex type
void a3d_MakeVSDeclarator(A3DVERTEXTYPE VertexType, int iStreamIdx, D3DVERTEXELEMENT9* pDecl);
//	Build vertex shader declarator from FVF
void a3d_MakeVSDeclarator(DWORD dwFVF, D3DVERTEXELEMENT9* pDecl);

//	Multiply a vertex but don't consider translation factors (the fourth row of matrix),
//	this function can be used to transform a normal. 
A3DVECTOR3 a3d_VectorMatrix3x3(const A3DVECTOR3& v, const A3DMATRIX4& mat);

// squared minimum distance from point to aabb 
float a3d_DistSquareToAABB(const A3DVECTOR3& vPos, const A3DAABB& aabb);

////////////////////////////////////////////////////////////////////////////
//
//	Below functions are only remained for the compatibility with old 
//	Angelica applications and games, so don't use them again when developing
//	new applications and games.
//
////////////////////////////////////////////////////////////////////////////

inline FLOAT SquareMagnitude(const A3DVECTOR3& v) { return a3d_SquareMagnitude(v); }
inline FLOAT Magnitude(const A3DVECTOR3& v) { return a3d_Magnitude(v); }
inline FLOAT MagnitudeH(const A3DVECTOR3& v) { return a3d_MagnitudeH(v); }
inline A3DVECTOR3 Vector423(const A3DVECTOR4& v) { return a3d_Vector423(v); }
inline A3DVECTOR3 Normalize(const A3DVECTOR3& v) { return a3d_Normalize(v); }
inline A3DVECTOR3 VecMin(const A3DVECTOR3& v1, const A3DVECTOR3& v2) { return a3d_VecMin(v1, v2); }
inline A3DVECTOR3 VecMax(const A3DVECTOR3& v1, const A3DVECTOR3& v2) { return a3d_VecMax(v1, v2); }
inline A3DVECTOR3 ViewToWorld(A3DVECTOR3& vIn, A3DMATRIX4& matView) { return a3d_ViewToWorld(vIn, matView); }
inline A3DMATRIX3 IdentityMatrix3() { return a3d_IdentityMatrix3(); }
inline A3DMATRIX4 IdentityMatrix() { return a3d_IdentityMatrix(); }
inline A3DMATRIX4 ZeroMatrix() { return a3d_ZeroMatrix(); }
inline A3DMATRIX4 ViewMatrix(const A3DVECTOR3& from, const A3DVECTOR3& dir, const A3DVECTOR3& vecUp, FLOAT roll) { return a3d_ViewMatrix(from, dir, vecUp, roll); }
inline A3DMATRIX4 LookAtMatrix(const A3DVECTOR3& from, const A3DVECTOR3& to, const A3DVECTOR3& vecUp, FLOAT roll) { return a3d_LookAtMatrix(from, to, vecUp, roll); }
inline A3DMATRIX4 TransformMatrix(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, const A3DVECTOR3& vecPos) { return a3d_TransformMatrix(vecDir, vecUp, vecPos); }
inline A3DMATRIX4 RotateX(FLOAT vRad) { return a3d_RotateX(vRad); }
inline A3DMATRIX4 RotateX(const A3DMATRIX4& mat, FLOAT vRad) { return a3d_RotateX(mat, vRad); }
inline A3DMATRIX4 RotateY(FLOAT vRad) { return a3d_RotateY(vRad); }
inline A3DMATRIX4 RotateY(const A3DMATRIX4& mat, FLOAT vRad) { return a3d_RotateY(mat, vRad); }
inline A3DMATRIX4 RotateZ(FLOAT vRad) { return a3d_RotateZ(vRad); }
inline A3DMATRIX4 RotateZ(const A3DMATRIX4& mat, FLOAT vRad) { return a3d_RotateZ(mat, vRad); }
inline A3DMATRIX4 RotateAxis(const A3DVECTOR3& vecAxis, FLOAT vRad) { return a3d_RotateAxis(vecAxis, vRad); }
inline A3DMATRIX4 RotateAxis(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecAxis, FLOAT vRad) { return a3d_RotateAxis(vecPos, vecAxis, vRad); }
inline A3DMATRIX4 Scaling(FLOAT sx, FLOAT sy, FLOAT sz) { return a3d_Scaling(sx, sy, sz); }
inline A3DMATRIX4 Scaling(const A3DMATRIX4& mat, FLOAT sx, FLOAT sy, FLOAT sz) { return a3d_Scaling(mat, sx, sy, sz); }
inline A3DMATRIX4 Translate(FLOAT x, FLOAT y, FLOAT z) { return a3d_Translate(x, y, z); }
inline A3DMATRIX4 Transpose(const A3DMATRIX4& mat) { return a3d_Transpose(mat); }
inline void Transpose(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut) { a3d_Transpose(matIn, pmatOut); }
inline bool GetCubeMapDirAndUp(int nFaceIndex, A3DVECTOR3 * pDir, A3DVECTOR3 * pUp) { return a3d_GetCubeMapDirAndUp(nFaceIndex, pDir, pUp); }
inline A3DCOLOR ColorValueToColorRGBA(A3DCOLORVALUE colorValue) { return a3d_ColorValueToColorRGBA(colorValue); }
inline A3DCOLORVALUE ColorRGBAToColorValue(A3DCOLOR colorRGBA) { return a3d_ColorRGBAToColorValue(colorRGBA); }
inline A3DMATRIX4 MirrorMatrix(const A3DVECTOR3 &p, const A3DVECTOR3 &n) { return a3d_MirrorMatrix(p, n); }
inline A3DMATRIX4 InverseTM(const A3DMATRIX4& mat) { return a3d_InverseTM(mat); }
inline void InverseTM(const A3DMATRIX4& matIn, A3DMATRIX4* pmatOut) { a3d_InverseTM(matIn, pmatOut); }
inline void ClearAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs) { a3d_ClearAABB(vMins, vMaxs); }
inline void AddVertexToAABB(A3DVECTOR3& vMins, A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint) { a3d_AddVertexToAABB(vMins, vMaxs, vPoint); }
inline void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DOBB& obb) { a3d_ExpandAABB(vecMins, vecMaxs, obb); }
inline void ExpandAABB(A3DVECTOR3& vecMins, A3DVECTOR3& vecMaxs, const A3DAABB& subAABB) { a3d_ExpandAABB(vecMins, vecMaxs, subAABB); }
inline void ExpandAABB(A3DAABB* pAABB, const A3DOBB& obb) { a3d_BuildAABB(pAABB, obb); }
inline void ExpandAABB(A3DAABB* pAABB, const A3DAABB& subAABB) { a3d_MergeAABB(pAABB, subAABB); }
inline void CompleteAABB(A3DAABB* pAABB) { a3d_CompleteAABB(pAABB); }
inline void CompleteOBB(A3DOBB* pOBB) { a3d_CompleteOBB(pOBB); }
inline A3DVECTOR3 RandDirH() { return a3d_RandDirH(); }
inline void HSVToRGB(A3DHSVCOLORVALUE& hsvColor, A3DCOLOR * pRgbColor) { a3d_HSVToRGB(hsvColor, pRgbColor); }
inline void RGBToHSV(A3DCOLOR rgbColor, A3DHSVCOLORVALUE * pHsvColor) { a3d_RGBToHSV(rgbColor, pHsvColor); }
inline void DecompressColor(A3DCOLOR rgbColor, BYTE * pR, BYTE * pG, BYTE * pB, BYTE *pA=NULL) { a3d_DecompressColor(rgbColor, pR, pG, pB, pA); }

#endif	//	_A3DFUNCS_H_
