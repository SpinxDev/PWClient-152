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

#include <A3DFuncs.h>
#include <AFI.h>
#include "Expression.h"
#include "FaceBoneController.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExpression::CExpression()
{
}

CExpression::~CExpression()
{
	Release();
}

// Load eFace file
bool CExpression::LoadExpression(const char* szFile)
{
	AString strFile = af_GetBaseDir();
	strFile += "\\";
	strFile += szFile;

	return LoadExpressionFullPath(strFile);
}

// Save eFace file
bool CExpression::SaveExpression(const char* szFile)
{
	AString strFile = af_GetBaseDir();
	strFile += "\\";
	strFile += szFile;

	return SaveExpressionFullPath(strFile);
}

// Save bone to file
bool CExpression::SaveBone(AFile* f, int nIndex)
{
	AString strLine, strTemp;
	strLine.Format("%d ", m_Index[nIndex]);
	f->WriteLine(strLine);

	int i,j;
	for (i=0; i<4; i++)
	{
		strLine = "";
		for (j=0; j<4; j++)
		{
			strTemp.Format(" %f", m_TM[nIndex].m[i][j]);
			strLine += strTemp;
		}
		f->WriteLine(strLine);
	}

	return true;
}

// Clear expression
void CExpression::Clear()
{
	m_Index.RemoveAll();
	m_TM.RemoveAll();
}

// Add bone
void CExpression::AddBone(int nIndex, A3DMATRIX4& mat)
{
	m_Index.Add(nIndex);
	m_TM.Add(mat);
}

// Get Transform matrix of a bone
bool CExpression::GetTM(int nIndex, A3DMATRIX4& mat)
{
	int i;
	for (i=0; i<m_Index.GetSize(); i++)
	{
		if (m_Index[i] == nIndex)
		{
			mat = m_TM[i];
			return true;
		}
	}

	mat.Identity();
	return false;
}

// Get affected bone count
int CExpression::GetAffectedBoneCount()
{
	return m_Index.GetSize();
}

// Get bone index by array index
int CExpression::GetBoneIndex(int nIndex)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < m_Index.GetSize());

	return m_Index[nIndex];
}
/*
// Combine expression
void CombineExpression(int nCount, CExpression* pExpIn, float* pRatio, CExpression* pExpOut)
{
	pExpOut->Clear();

	int i, j;

	A3DMATRIX4* matCombine = new A3DMATRIX4[nCount];

	bool bNeedCombine;

	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		bNeedCombine = false;
		for (j=0; j<nCount; j++)
		{
			if (pExpIn[j].GetTM(i, matCombine[j]))
				bNeedCombine = true;
		}
		if (bNeedCombine)
		{
			A3DMATRIX4 matBone = CombineMatrix(nCount, matCombine, pRatio);
			pExpOut->AddBone(i, matBone);
		}
	}

	delete[] matCombine;
}

// Apply expression to model
void ApplyExpression(CFaceBoneController* pFaceBoneCtrler, CExpression* pExpression)
{
	int i;
	int nIndex;
	A3DMATRIX4 mat;
	int n = pExpression->GetAffectedBoneCount();
	CFaceBone* pFaceBone = pFaceBoneCtrler->GetFaceBone();
	for (i=0; i<pExpression->GetAffectedBoneCount(); i++)
	{
		nIndex = pExpression->GetBoneIndex(i);
		pExpression->GetTM(i, mat);
		pFaceBone[nIndex].SetTransformMatrix(mat);
		pFaceBoneCtrler->UpdateBone(nIndex);
	}
}
*/

/*
// Combine and directly apply expression on bones
void CombineAndApplyShape(CFaceBoneController* pFaceBoneCtrler, int nCount, CExpression* pExpIn, float* pRatio)
{
	int i, j;

	A3DMATRIX4* matCombine = new A3DMATRIX4[nCount];

	bool bNeedCombine;

	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		bNeedCombine = false;
		for (j=0; j<nCount; j++)
		{
			if (pExpIn[j].GetTM(i, matCombine[j]))
				bNeedCombine = true;
		}
		if (bNeedCombine)
		{
			pFaceBoneCtrler->AddExpr(i, matCombine, pRatio, nCount);
		}
	}

	delete[] matCombine;
}
*/
/*
// Combine matrix
A3DMATRIX4 CombineMatrix(int nCount, A3DMATRIX4* pMat, float* pRatio)
{
	A3DMATRIX4 matOut;
	matOut.Identity();

	if (nCount <= 0)
		return matOut;

	A3DVECTOR3 vPos, vScale;
	A3DQUATERNION qOrient;
	A3DVECTOR3 vTotalPos(0, 0, 0);
	A3DVECTOR3 vTotalScale(0, 0, 0);
	A3DQUATERNION qTotalOrient(0, 0, 0, 0);
	float fSum = 0;

	int i;
	for (i=0; i<nCount; i++)
	{
		a3d_DecomposeMatrix(pMat[i], vScale, qOrient, vPos);
		vTotalScale += (vScale - 1) * pRatio[i];
		vTotalPos += vPos * pRatio[i];
		qTotalOrient = qTotalOrient + qOrient * pRatio[i];
		fSum += pRatio[i];
//		if (fSum > 0.0001)
//			qTotalOrient = SLERPQuad(qTotalOrient, qOrient, pRatio[i] / fSum);
	}
	if (fSum < 0.0001)
		return matOut;

	vTotalScale /= fSum;
	vTotalPos /= fSum;
	vTotalScale += 1;
	qTotalOrient.Normalize();

	A3DMATRIX4 matTemp;
	matTemp.Scale(vTotalScale.x, vTotalScale.y, vTotalScale.z);
	matOut *= matTemp;
	qTotalOrient.ConvertToMatrix(matTemp);
	matOut *= matTemp;
	matTemp.Translate(vTotalPos.x, vTotalPos.y, vTotalPos.z);
	matOut *= matTemp;

	return matOut;
}
*/

// Copy expression
void CExpression::CopyExpression(const CExpression* pExpression)
{
	m_Index.RemoveAll();
	m_Index.Append(pExpression->m_Index);
	m_TM.RemoveAll();
	m_TM.Append(pExpression->m_TM);
}

// Combine and directly apply expression on bones
void AddExpression(CFaceBoneController* pFaceBoneCtrler, int nCount, CExpression* pExpIn, float* pRatio, int nDuration)
{
	if (!pFaceBoneCtrler)
		return;

	int i, j;
	A3DMATRIX4* matCombine = new A3DMATRIX4[nCount];
	bool bNeedCombine;

	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		bNeedCombine = false;
		for (j=0; j<nCount; j++)
		{
			if (pExpIn[j].GetTM(i, matCombine[j]))
				bNeedCombine = true;
		}
		if (bNeedCombine)
		{
			pFaceBoneCtrler->AddExpr(i, matCombine, pRatio, nCount, nDuration);
		}
	}

	delete[] matCombine;
}

// Combine and directly apply shape on bones
void AddShape(CFaceBoneController* pFaceBoneCtrler, int nCount, CExpression* pExpIn, float* pRatio, int nDuration)
{
	if (!pFaceBoneCtrler)
		return;

	int i, j;
	A3DMATRIX4* matCombine = new A3DMATRIX4[nCount];
	bool bNeedCombine;

	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		bNeedCombine = false;
		for (j=0; j<nCount; j++)
		{
			if (pExpIn[j].GetTM(i, matCombine[j]))
				bNeedCombine = true;
		}
		if (bNeedCombine)
		{
			pFaceBoneCtrler->AddShape(i, matCombine, pRatio, nCount, nDuration);
		}
	}

	delete[] matCombine;
}

/*
// Play expression
void CExpression::Play(CFaceBoneController* pFaceBoneCtrler, float fWeight, int nDuration)
{
	if (!pFaceBoneCtrler)
		return;

	A3DMATRIX4 matCombine;
	bool bNeedCombine;

	for (int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		bNeedCombine = false;
		if (GetTM(i, matCombine))
			bNeedCombine = true;
		if (bNeedCombine)
		{
			pFaceBoneCtrler->AddExpr(i, &matCombine, &fWeight, 1, nDuration);
		}
	}
}
*/
// Load eFace file by a full path file name
bool CExpression::LoadExpressionFullPath(const char* szFile)
{
	AFileImage f;
	if (!f.Open(szFile, AFILE_TEXT | AFILE_OPENEXIST))
		return false;

	AString strLine;
	DWORD dwReadLen;
	char szLine[256];
	int nCount;
//	f.ReadString(strLine);
//	sscanf(strLine, "%d", &nCount);
	// Read blank line
	f.ReadLine(szLine, 256, &dwReadLen);

	// Read bone count
	f.ReadLine(szLine, 256, &dwReadLen);
	sscanf(szLine, "%d", &nCount);
	ASSERT(nCount > 0 && nCount <= TOTAL_BONE_COUNT); 

	if ((nCount <= 0) || (nCount >TOTAL_BONE_COUNT))
	{
		f.Close();
		return false;
	}

	// Read blank line
	f.ReadLine(szLine, 256, &dwReadLen);

	m_Index.RemoveAll();
	m_TM.RemoveAll();

	int i,j,k;
	int nIndex, nPos;
	A3DMATRIX4 mTemp;
	for (i=0; i<nCount; i++)
	{
//		f.ReadString(strLine);
		f.ReadLine(szLine, 256, &dwReadLen);
		sscanf(szLine, "%d", &nIndex);
		for (j=0; j<4; j++)
		{
//			f.ReadString(strLine);
			f.ReadLine(szLine, 256, &dwReadLen);
			strLine = szLine;
			for (k=0; k<4; k++)
			{
				nPos = strLine.Find(' ');
				strLine = strLine.Right(strLine.GetLength() - nPos - 1);
				sscanf(strLine, "%f", &mTemp.m[j][k]);
			}
		}
		m_Index.Add(nIndex);
		m_TM.Add(mTemp);
	}

	f.Close();

	return true;
}
// Save eFace file by a full path file name
bool CExpression::SaveExpressionFullPath(const char* szFile)
{
	AFile f;
	if (!f.Open(szFile, AFILE_TEXT | AFILE_CREATENEW))
		return false;

	int i;

	int nCount = m_Index.GetSize();

	AString strLine;
	f.WriteLine("");
	strLine.Format("%d", nCount);
	f.WriteLine(strLine);
	f.WriteLine("");

	for (i=0; i<nCount; i++)
	{
		SaveBone(&f, i);
	}

	f.Close();
	
	return true;
}

// Release
void CExpression::Release()
{
	Clear();
}

// Load eFace file 
bool CExpression::LoadExpression(AFileImage* pFile)
{
	AString strLine;
	int nCount;
	DWORD dwReadLen;
	char szLine[256];

	pFile->ReadLine(szLine, 256, &dwReadLen);
	sscanf(szLine, "%d", &nCount);

	if ((nCount <= 0) || (nCount >TOTAL_BONE_COUNT))
	{
		return false;
	}

	m_Index.RemoveAll();
	m_TM.RemoveAll();

	int i,j,k;
	int nIndex, nPos;
	A3DMATRIX4 mTemp;
	for (i=0; i<nCount; i++)
	{
		pFile->ReadLine(szLine, 256, &dwReadLen);
		sscanf(szLine, "%d", &nIndex);
		for (j=0; j<4; j++)
		{
			pFile->ReadLine(szLine, 256, &dwReadLen);
			strLine = szLine;
			for (k=0; k<4; k++)
			{
				nPos = strLine.Find(' ');
				strLine = strLine.Right(strLine.GetLength() - nPos - 1);
				sscanf(strLine, "%f", &mTemp.m[j][k]);
			}
		}
		m_Index.Add(nIndex);
		m_TM.Add(mTemp);
	}

	return true;
}

// Save eFace file to AFile
bool CExpression::SaveExpression(AFile* pFile)
{
	int nCount = m_Index.GetSize();
	if ((nCount <= 0) || (nCount >TOTAL_BONE_COUNT))
	{
		return false;
	}

	AString strLine;
	strLine.Format("%d", nCount);
	pFile->WriteLine(strLine);

	for (int i=0; i<nCount; i++)
	{
		SaveBone(pFile, i);
	}

	return true;
}
