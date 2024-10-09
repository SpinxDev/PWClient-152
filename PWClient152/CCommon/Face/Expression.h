/*
 * FILE: Expression.h
 *
 * DESCRIPTION: Class for skin face expression
 *
 * CREATED BY: Jiangdalong, 2004/12/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

//#include "FaceBoneController.h"
#include <A3DMatrix.h>
#include <AArray.h>
#include <AFileImage.h>

class CFaceBoneController;

class CExpression  
{
public:
	CExpression();
	virtual ~CExpression();

// Operations
public:
	// Load eFace file based on base dir
	bool LoadExpression(const char* szFile);
	// Save eFace file based on base dir
	bool SaveExpression(const char* szFile);
	// Load eFace file by a full path file name
	bool LoadExpressionFullPath(const char* szFile);
	// Save eFace file by a full path file name
	bool SaveExpressionFullPath(const char* szFile);
	// Load eFace file from AFile
	bool LoadExpression(AFileImage* pFile);
	// Save eFace file to AFile
	bool SaveExpression(AFile* pFile);

	// Clear expression
	void Clear();
	// Release
	void Release();
	// Add bone
	void AddBone(int nIndex, A3DMATRIX4& mat);

	// Get Transform matrix of a bone
	bool GetTM(int nIndex, A3DMATRIX4& mat);
	// Get affected bone count
	int GetAffectedBoneCount();
	// Get bone index by array index
	int GetBoneIndex(int nIndex);

	// Copy expression
	void CopyExpression(const CExpression* pExpression);

	// Combine expression
//	friend void CombineExpression(int nCount, CExpression* pExpIn, float* pRatio, CExpression* pExpOut);
	// Apply expression to model
//	friend void ApplyExpression(CFaceBoneController* pFaceBoneCtrler, CExpression* pExpression);
	// Combine matrix
//	friend A3DMATRIX4 CombineMatrix(int nCount, A3DMATRIX4* pMat, float* pRatio);
	// Combine and directly apply face shape on bones
//	friend void CombineAndApplyShape(CFaceBoneController* pFaceBoneCtrler, int nCount, CExpression* pExpIn, float* pRatio);
	// Combine and directly apply shape on bones
	friend void AddShape(CFaceBoneController* pFaceBoneCtrler, int nCount, CExpression* pExpIn, float* pRatio, int nDuration);
	// Combine and directly apply expression on bones
	friend void AddExpression(CFaceBoneController* pFaceBoneCtrler, int nCount, CExpression* pExpIn, float* pRatio, int nDuration);
	// Play expression
//	void Play(CFaceBoneController* pFaceBoneCtrler, float fWeight, int nDuration);

	inline bool IsEmpty()		{ return m_Index.GetSize() ==0 && m_TM.GetSize() == 0; }

// Attributes
protected:
	AArray<int, int>				m_Index;		// Bone index
	AArray<A3DMATRIX4, A3DMATRIX4&>	m_TM;			// Bone transform matrix

// Operations
protected:
	// Save bone to file
	bool SaveBone(AFile* f, int nIndex);

};

#endif // #ifndef _EXPRESSION_H_
