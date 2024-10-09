/*
 * FILE: FaceBone.cpp
 *
 * DESCRIPTION: Class for one face bone of skin face model
 *
 * CREATED BY: Jiangdalong, 2004/12/09
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#include "FaceBone.h"
#include <A3DFuncs.h>
#include <AString.h>
#include <A3DBone.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaceBone::CFaceBone()
{
	m_TM.Identity();
	m_OldTM.Identity();
	m_ShapeTM.Identity();
	m_fScale = 1.0f;
	m_fOldScale = 1.0f;
//	m_fShapeScale = 1.0f;
	m_pBone = NULL;
	m_pController = NULL;
	for (int i=0; i<MAX_BACKUP_NUMBER; i++)
	{
		m_BakTM[i].Identity();
	}
}

CFaceBone::~CFaceBone()
{
	ReleaseController();
}

// Translate bone
void CFaceBone::Translate(A3DVECTOR3& v)
{
	A3DMATRIX4 m;
	m.Translate(v.x, v.y, v.z);
	m_TM *= m;
}

// Rotate bone
void CFaceBone::Rotate(A3DVECTOR3& vPos, A3DVECTOR3& vRad)
{
	A3DMATRIX4 m;

	m = a3d_RotateAxis(vPos, A3DVECTOR3(1.0, 0, 0), vRad.x);
	m_TM *= m;
	m = a3d_RotateAxis(vPos, A3DVECTOR3(0, 1.0, 0), vRad.y);
	m_TM *= m;
	m = a3d_RotateAxis(vPos, A3DVECTOR3(0, 0, 1.0), vRad.z);
	m_TM *= m;  
}

// Scale bone
void CFaceBone::Scale(A3DVECTOR3& vPos, A3DVECTOR3& v)
{
	A3DMATRIX4 m;
	m.Translate(vPos.x, vPos.y, vPos.z);
	m = a3d_ScalingRelative(m, v.x, v.y, v.z);
	m_TM *= m;
	m_fScale *= v.x;
}

// Save bone to file
bool CFaceBone::SaveBone(AFile* f, int nIndex)
{
	AString strLine, strTemp;
	strLine.Format("%d ", nIndex);
	strLine += m_strName;
	f->WriteLine(strLine);

	int i,j;
	for (i=0; i<4; i++)
	{
		strLine = "";
		for (j=0; j<4; j++)
		{
			strTemp.Format(" %f", m_TM.m[i][j]);
			strLine += strTemp;
		}
		f->WriteLine(strLine);
	}

	return true;
}

// Get transform matrix from stored TM to current TM
A3DMATRIX4 CFaceBone::GetTMFromOldToNow()
{
	A3DMATRIX4 mat;
	mat = m_OldTM.GetInverseTM() * m_TM;

	return mat;
}

// Update bone
void CFaceBone::Update(bool bSaveShape, int nTransTime)
{
	if (NULL == m_pBone)
		return;

	A3DVECTOR3 vPos, vScale;
	A3DQUATERNION qOrient;
	A3DMATRIX4 mt = GetTransformMatrix();
	a3d_DecomposeMatrix(mt, vScale, qOrient, vPos);

	if (NULL == m_pController)
	{
		m_pController = new A3DBoneMatrixController(m_pBone);
		m_pBone->SetFirstController(m_pController);
	}

	m_pController->CombineMatrix(mt, nTransTime);

	if( nTransTime == 0 )
		m_pBone->Update(0);

	// For face editor, the correct position is as following.
//	SetPos(m_pBone->GetAbsoluteTM().GetRow(3));//GetPos());
	// For element(game), the following is correct.
	SetPos(m_pBone->GetRelativeTM().GetRow(3));

	SetScale(vScale.x);

	// Save shape TM
	if (bSaveShape)
	{
		SetShapeTM(mt);
	}
}

// Release Controllers of a bone
void CFaceBone::ReleaseController()
{
	if (NULL != m_pController)
	{
		delete m_pController;
		m_pController = NULL;
	}
}
/*
// Blend bone
void CFaceBone::Blend(const A3DMATRIX4* mats, const float* weights, int nNumMat)
{
	if (NULL == m_pBone)
		return;

	if (NULL == m_pController)
	{
		m_pController = new A3DBoneMatrixController(m_pBone);
		m_pBone->SetFirstController(m_pController);
	}

	m_pController->CombineMatrixList(mats, weights, nNumMat, 0);
	m_pBone->Update(0);

	A3DVECTOR3 vPos, vScale;
	A3DQUATERNION qOrient;
	A3DMATRIX4 mt = m_pController->GetControllerMatrix();
	a3d_DecomposeMatrix(mt, vScale, qOrient, vPos);

	SetTransformMatrix(mt);

//	SetPos(m_pBone->GetPos());
	SetPos(m_pBone->GetRelativeTM().GetRow(3));
	SetScale(vScale.x);
}
*/
// Restore old transform matrix
void CFaceBone::RestoreOldTM()
{
	m_TM = m_OldTM;
	m_fScale = m_fOldScale;
	m_vPosition = m_vOldPos;
}

// Store transform matrix to old 
void CFaceBone::StoreTM()
{
	m_OldTM = m_TM;
	m_fOldScale = m_fScale;
	m_vOldPos = m_vPosition;
}

// Initialize controller
void CFaceBone::InitController()
{
	if (NULL != m_pController)
	{
		delete m_pController;
	}
	m_pController = new A3DBoneMatrixController(m_pBone);
	m_pBone->SetFirstController(m_pController);
}
/*
// Store transform matrix to shape
void CFaceBone::StoreShapeTM()
{
	m_ShapeTM *= m_TM;
//	m_fShapeScale = m_fScale;
//	m_vShapePos = m_vPosition;
}
*/

// Restore shape transform matrix to current matrix
void CFaceBone::RestoreShapeTM()
{
	m_TM = m_ShapeTM;
//	m_fScale = m_fShapeScale;
//	m_vPosition = m_vShapePos;
}

/*
// Blend bone for expression
void CFaceBone::BlendExp(const A3DMATRIX4* mats, const float* weights, int nNumMat)
{
	if (NULL == m_pBone)
		return;

	if (NULL == m_pController)
	{
		m_pController = new A3DBoneMatrixController(m_pBone);
		m_pBone->SetFirstController(m_pController);
	}

	// Add current shape as a expression, and it's weight is 1
	A3DMATRIX4* mtExp = new A3DMATRIX4[nNumMat+1];
	float* fWeights = new float[nNumMat+1];

	memcpy(mtExp, mats, sizeof(A3DMATRIX4)*nNumMat);
	memcpy(fWeights, weights, sizeof(float)*nNumMat);
	mtExp[nNumMat] = m_ShapeTM;
	fWeights[nNumMat] = 1.0f;

	m_pController->CombineMatrixList(mtExp, fWeights, nNumMat+1, 0);
	m_pBone->Update(0);

	delete[] mtExp;
	delete[] fWeights;
}
*/

// Add some bones' changes on backuped face bone
void CFaceBone::ApplyBoneOnBakFace(int nIndex)
{
	m_TM = m_BakTM[nIndex] * m_TM;
}

// Reset transform matrix
void CFaceBone::ResetTM()
{
	m_TM.Identity();
}

// Add expression to current face
void CFaceBone::Add(const A3DMATRIX4* mats, const float* weights, int nNumMat, int nDuration, bool bShape)
{
	if (NULL == m_pBone)
		return;

	if (NULL == m_pController)
	{
		m_pController = new A3DBoneMatrixController(m_pBone);
		m_pBone->SetFirstController(m_pController);
	}

	// Add current shape as a expression, and it's weight is 1
	A3DMATRIX4* mtExp = new A3DMATRIX4[nNumMat+1];
	float* fWeights = new float[nNumMat+1];

	memcpy(mtExp, mats, sizeof(A3DMATRIX4)*nNumMat);
	memcpy(fWeights, weights, sizeof(float)*nNumMat);

	if (bShape)
		mtExp[nNumMat] = m_TM;
	else
		mtExp[nNumMat] = m_ShapeTM;

	fWeights[nNumMat] = 1.0f;

	m_pController->CombineMatrixList(mtExp, fWeights, nNumMat+1, nDuration);
	m_pBone->Update(0);

	delete[] mtExp;
	delete[] fWeights;

	// Get the modified parameters
	A3DVECTOR3 vPos, vScale;
	A3DQUATERNION qOrient;
	A3DMATRIX4 mt = m_pController->GetControllerMatrix();
	a3d_DecomposeMatrix(mt, vScale, qOrient, vPos);

	// Store the new parameters
	SetTransformMatrix(mt);
	SetPos(m_pBone->GetRelativeTM().GetRow(3));
	SetScale(vScale.x);

}