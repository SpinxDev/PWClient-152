/*
 * FILE: FaceBoneController.cpp
 *
 * DESCRIPTION: Class for controlling face bones
 *
 * CREATED BY: Jiangdalong, 2004/12/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.	
 */

#include "FaceBoneController.h"
#include <A3DBoneController.h>
#include <AFI.h>
#include <A3DFuncs.h>
#include <A3DBone.h>
#include "FaceAnimation.h"
#include <AScriptFile.h>
#include <AFileImage.h>

bool				CFaceBoneController::m_bStaticDataInited = false;	// flag indicate static data has been initialized
AString				CFaceBoneController::m_BoneName[TOTAL_BONE_COUNT];	// Bone names
AArray<int, int>	CFaceBoneController::m_BoneGroup[BONE_GROUP_COUNT];	// Bone groups
AString				CFaceBoneController::m_BoneGroupName[BONE_GROUP_COUNT];// Group names
AArray<int, int>	CFaceBoneController::m_BonePart[BONE_PART_COUNT];	// Bone parts


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaceBoneController::CFaceBoneController(A3DSkeleton* pSkeleton)
{
	if( !m_bStaticDataInited )
	{
		// Load bone initial parameters
		const char* szRoot = af_GetBaseDir();
		char szFile[256];
		strcpy(szFile, szRoot);
		
		strcat(szFile, "\\Configs\\BoneInit.cfg");
		LoadBoneInitParam(szFile);
		strcpy(szFile, szRoot);
		strcat(szFile, "\\Configs\\BoneGroup.cfg");
		LoadBoneGroupInfo(szFile);
		strcpy(szFile, szRoot);
		strcat(szFile, "\\Configs\\BonePart.cfg");
		LoadBonePartInfo(szFile);

		m_bStaticDataInited = true;
	}

	m_bReady = false;
	m_nSelectedCount = 0;
	m_nSpecialCenter = 0;
	m_BonePartScale[0] = 1.0f;
	m_BonePartScale[1] = 1.0f;
	m_BonePartScale[2] = 1.0f;

	// Import bone infos from skeleton
	m_pSkeleton = pSkeleton;
	if (NULL != m_pSkeleton)
		ImportBoneFromSkeleton();

}

CFaceBoneController::~CFaceBoneController()
{
	Release();
}

// Set skeleton
void CFaceBoneController::SetSkeleton(A3DSkeleton* pSkeleton)
{
	m_pSkeleton = pSkeleton;

	if (NULL != m_pSkeleton)
		ImportBoneFromSkeleton();
//	InitBone();
}

// Load bone initial parameters
bool CFaceBoneController::LoadBoneInitParam(const char* szFile)
{
#ifdef _FACE_EDITOR_
	float fSize[3];
	fSize[0] = 0.002f;
	fSize[1] = 0.006f;
	fSize[2] = 0.001f;
#endif

	AScriptFile ScriptFile;
	if (!ScriptFile.Open(szFile))
		return false;

	int i;

	int nSize;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		// Read bone name
		ScriptFile.GetNextToken(true);
		m_BoneName[i] = ScriptFile.m_szToken;
		//m_FaceBone[i].SetName(ScriptFile.m_szToken);
		
		// Read bone size, for editor
		nSize = ScriptFile.GetNextTokenAsInt(true);
		ASSERT(nSize >= 0 && nSize <= 2);
		//m_FaceBone[i].SetSize(fSize[nSize]);
	}

	ScriptFile.Close();

	return true;
}

// Load bone group infomation
bool CFaceBoneController::LoadBoneGroupInfo(const char* szFile)
{
	AScriptFile ScriptFile;
	if (!ScriptFile.Open(szFile))
		return false;

	int nCount, nIndex;
	int i, j;

	for (i=0; i<BONE_GROUP_COUNT; i++)
	{
		// Read group name
		ScriptFile.GetNextToken(true);
		m_BoneGroupName[i] = ScriptFile.m_szToken;

		// Read bone count in this group
		nCount = ScriptFile.GetNextTokenAsInt(true);
		ASSERT(nCount >= 0);

		// Read bones in this group
		for (j=0; j<nCount; j++)
		{
			ScriptFile.GetNextToken(true);
			nIndex = GetBoneIndexByName(ScriptFile.m_szToken);
			ASSERT(nIndex >=0 );
			m_BoneGroup[i].Add(nIndex);
		}
	}

	ScriptFile.Close();

	return true;
}

// Load bone part infomation
bool CFaceBoneController::LoadBonePartInfo(const char* szFile)
{
	AScriptFile ScriptFile;
	if (!ScriptFile.Open(szFile))
		return false;

	AString strLine;
	int nCount, nIndex;
	int i, j;
	for (i=0; i<BONE_PART_COUNT; i++)
	{
		// Read bone count in this part
		nCount = ScriptFile.GetNextTokenAsInt(true);
		ASSERT(nCount >= 0);
		// Read part name
		ScriptFile.GetNextToken(true);

		// Read bones in this part
		for (j=0; j<nCount; j++)
		{
			ScriptFile.GetNextToken(true);
			nIndex = GetBoneIndexByName(ScriptFile.m_szToken);
			ASSERT(nIndex >= 0);
			m_BonePart[i].Add(nIndex);
		}
	}
	ScriptFile.Close();

	return true;
}

// Import bone info from a skeleton
bool CFaceBoneController::ImportBoneFromSkeleton()
{
	if (NULL == m_pSkeleton)
		return false;

	int i;
	AString strName;
	A3DBone* pBone;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		strName = m_BoneName[i];
		pBone = m_pSkeleton->GetBone(strName, NULL);

		// For face editor, the correct position is as following.
//		m_FaceBone[i].SetPos(pBone->GetAbsoluteTM().GetRow(3));
		// For element(game), the following is correct.
		m_FaceBone[i].SetPos(pBone->GetRelativeTM().GetRow(3));

		m_FaceBone[i].SetBone(pBone);
		m_FaceBone[i].InitController();
	}
	return true;
}

// Init bone
void CFaceBoneController::InitBone()
{
	A3DMATRIX4 mat;
	mat.Identity();
	int i;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].SetScale(1.0f);
		m_FaceBone[i].SetTransformMatrix(mat);
		m_FaceBone[i].SetOldTM(mat);
	}

//	ImportBoneFromSkeleton();

	m_bReady = true;
}

// Get bone index by name
int CFaceBoneController::GetBoneIndexByName(const char* szName)
{
	int i;

	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		if (m_BoneName[i].Compare(szName) == 0)
			return i;
	}

	return -1;
}

// Clear selection
void CFaceBoneController::ClearSelection()
{
	m_nSelectedCount = 0;
}

// Add selection
void CFaceBoneController::AddSelection(int nIndex)
{
//	if (PosInSelection(nIndex))
//		return;

	m_SelectedBone[m_nSelectedCount] = nIndex;
	m_nSelectedCount++;

}

// Is in selection?
int CFaceBoneController::PosInSelection(int nIndex)
{
	int i;
	for (i=0; i<m_nSelectedCount; i++)
	{
		if (m_SelectedBone[i] == nIndex)
		{
			return i;
		}
	}

	return -1;
}

// Remove selection
void CFaceBoneController::RemoveSelection(int nSelIndex)
{
	ASSERT(nSelIndex>=0);
	ASSERT(nSelIndex<m_nSelectedCount);

	int i;
	for (i=nSelIndex; i<m_nSelectedCount - 1; i++)
	{
		m_SelectedBone[i] = m_SelectedBone[i+1];
	}

	m_nSelectedCount--;
}

// Get center of selected bones
A3DVECTOR3 CFaceBoneController::GetSelectedCenter()
{
	A3DVECTOR3 vSum(0, 0, 0);
	if (m_nSelectedCount <= 0)
		return vSum;

	for (int i=0; i<m_nSelectedCount; i++)
	{
		vSum += m_FaceBone[m_SelectedBone[i]].GetPos();
	}
	vSum /= (float)m_nSelectedCount;

	return vSum;
}

// Set special center
void CFaceBoneController::SetSpecialCenter(int nIndex)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < TOTAL_BONE_COUNT);

	m_nSpecialCenter = nIndex;
}

// Select bone group
void CFaceBoneController::SelectBoneGroup(int nIndex)
{

	int i;
	for (i=0; i<m_BoneGroup[nIndex].GetSize(); i++)
	{
		if (PosInSelection(m_BoneGroup[nIndex][i]) == -1)
			AddSelection(m_BoneGroup[nIndex][i]);
	}
}

// Translate bone
bool CFaceBoneController::TranslateBone(A3DVECTOR3& vector, bool bUpdate, bool bHalfFace)
{
	int i, nIndex;
	bool bEdit = false;
	for (i=0; i<m_nSelectedCount; i++)
	{
		nIndex = m_SelectedBone[i];
		if (nIndex == NECK_INDEX)
			continue;
		m_FaceBone[nIndex].Translate(vector);
		if (bUpdate)
			UpdateBone(nIndex, true);

		if (bHalfFace)
		{
			// Translate corresponding bone
			if (nIndex < RIGHT_BONE_COUNT)
			{
				nIndex += ACTIVE_BONE_COUNT;
				A3DVECTOR3 v = vector;
				v.x = -vector.x;
				m_FaceBone[nIndex].Translate(v);
				if (bUpdate)
					UpdateBone(nIndex, true);
			}
		}
		bEdit = true;
	}

	return bEdit;
}

// Rotate Bone
bool CFaceBoneController::RotateBone(A3DVECTOR3& v, CENTERTYPE type, bool bUpdate, bool bHalfFace)
{
	int i, nIndex;
	A3DVECTOR3 vCenter;
	bool bEdit = false;
	switch (type)
	{
	case CT_SELF:
		break;
	case CT_ALL:
		vCenter = GetSelectedCenter();
		break;
	case CT_SPECIAL:
		vCenter = m_FaceBone[m_nSpecialCenter].GetPos();
		break;
	}

	A3DVECTOR3 vCor;
	vCor.x = v.x;
	vCor.y = -v.y;
	vCor.z = -v.z;
	for (i=0; i<m_nSelectedCount; i++)
	{
		nIndex = m_SelectedBone[i];

		if (nIndex == NECK_INDEX)
			continue;

		if (type == CT_SELF)
		{
			vCenter = m_FaceBone[nIndex].GetPos();
		}
		m_FaceBone[nIndex].Rotate(vCenter, v);
		if (bUpdate)
			UpdateBone(nIndex, true);

		if (bHalfFace)
		{
			// Rotate corresponding bone
			if (nIndex < RIGHT_BONE_COUNT)
			{
				A3DVECTOR3 vCenterCor = vCenter;
				vCenterCor.x = -vCenter.x;
				nIndex += ACTIVE_BONE_COUNT;
				m_FaceBone[nIndex].Rotate(vCenterCor, vCor);
				if (bUpdate)
					UpdateBone(nIndex, true);
			}
		}
		bEdit = true;
	}

	return bEdit;
}

// Scale bone
bool CFaceBoneController::ScaleBone(A3DVECTOR3& v, CENTERTYPE type, bool bUpdate, bool bHalfFace)
{
	int i, nIndex;
	A3DVECTOR3 vCenter;
	bool bEdit = false;
	switch (type)
	{
	case CT_SELF:
		for (i=0; i<m_nSelectedCount; i++)
		{
			nIndex = m_SelectedBone[i];
			if (nIndex == NECK_INDEX)
				continue;

			vCenter = m_FaceBone[nIndex].GetPos();
			m_FaceBone[nIndex].Scale(vCenter, v);
			if (bUpdate)
				UpdateBone(nIndex, true);

			if (bHalfFace)
			{
				// Corresponding bone
				if (nIndex < RIGHT_BONE_COUNT)
				{
					nIndex += ACTIVE_BONE_COUNT;
					vCenter = m_FaceBone[nIndex].GetPos();
					m_FaceBone[nIndex].Scale(vCenter, v);
					if (bUpdate)
						UpdateBone(nIndex, true);
				}
			}
			bEdit = true;
		}
		break;
	case CT_ALL:
		{
			vCenter = GetSelectedCenter();
			A3DMATRIX4 m, mCor, mi;
			A3DVECTOR3 vPosOld, vPosNew;
			m.Translate(vCenter.x, vCenter.y, vCenter.z);
			mCor.Translate(-vCenter.x, vCenter.y, vCenter.z);
			for (i=0; i<m_nSelectedCount; i++)
			{
				nIndex = m_SelectedBone[i];
				if (nIndex == NECK_INDEX)
					continue;
				mi = a3d_ScalingRelative(m, v.x, v.y, v.z);
				vPosOld = m_FaceBone[nIndex].GetPos();
				vPosNew = vPosOld * mi;
				m_FaceBone[nIndex].Translate(vPosNew - vPosOld);
				if (bUpdate)
					UpdateBone(nIndex, true);
				if (bHalfFace)
				{
					// Corresponding bone
					if (nIndex < RIGHT_BONE_COUNT)
					{
						nIndex += ACTIVE_BONE_COUNT;
						mi = a3d_ScalingRelative(mCor, v.x, v.y, v.z);
						vPosOld = m_FaceBone[nIndex].GetPos();
						vPosNew = vPosOld * mi;
						m_FaceBone[nIndex].Translate(vPosNew - vPosOld);
						if (bUpdate)
							UpdateBone(nIndex, true);
					}
				}
				bEdit = true;
			}
		}
		break;
	case CT_SPECIAL:
		{
			vCenter = m_FaceBone[m_nSpecialCenter].GetPos();
			A3DMATRIX4 m, mCor, mi;
			A3DVECTOR3 vPosOld, vPosNew;
			m.Translate(vCenter.x, vCenter.y, vCenter.z);
			mCor.Translate(-vCenter.x, vCenter.y, vCenter.z);
			for (i=0; i<m_nSelectedCount; i++)
			{
				nIndex = m_SelectedBone[i];
				if (nIndex == NECK_INDEX)
					continue;
				mi = a3d_ScalingRelative(m, v.x, v.y, v.z);
				vPosOld = m_FaceBone[nIndex].GetPos();
				vPosNew = vPosOld * mi;
				m_FaceBone[nIndex].Translate(vPosNew - vPosOld);
				if (bUpdate)
					UpdateBone(nIndex, true);
				if (bHalfFace)
				{
					// Corresponding bone
					if (nIndex < RIGHT_BONE_COUNT)
					{
						nIndex += ACTIVE_BONE_COUNT;
						mi = a3d_ScalingRelative(mCor, v.x, v.y, v.z);
						vPosOld = m_FaceBone[nIndex].GetPos();
						vPosNew = vPosOld * mi;
						m_FaceBone[nIndex].Translate(vPosNew - vPosOld);
						if (bUpdate)
							UpdateBone(nIndex, true);
					}
				}
				bEdit = true;
			}
		}
		break;
	}

	return bEdit;
}
/*
// Load bone from file
bool CFaceBoneController::LoadBoneFromFile(const char* szFile)
{
	AFile f;
	if (!f.Open(szFile, AFILE_TEXT | AFILE_OPENEXIST))
		return false;

	AString strLine;
	int nCount;
	f.ReadString(strLine);
	sscanf(strLine, "%d", &nCount);

	if ((nCount <= 0) || (nCount >TOTAL_BONE_COUNT))
	{
		f.Close();
		return false;
	}

	int i,j,k;
	int nIndex, nPos;
	A3DMATRIX4 mTemp;
	for (i=0; i<nCount; i++)
	{
		f.ReadString(strLine);
		sscanf(strLine, "%d", &nIndex);
		for (j=0; j<4; j++)
		{
			f.ReadString(strLine);
			for (k=0; k<4; k++)
			{
				nPos = strLine.Find(' ');
				strLine = strLine.Right(strLine.GetLength() - nPos - 1);
				sscanf(strLine, "%f", &mTemp.m[j][k]);
			}
		}
		m_FaceBone[nIndex].SetTransformMatrix(mTemp);
		UpdateBone(nIndex);
	}

	f.Close();

	return true;
}
*/
// Load bone from file
bool CFaceBoneController::LoadBoneFromFile(const char* szFile)
{
	AFileImage f;
	if (!f.Open(szFile, AFILE_TEXT | AFILE_OPENEXIST))
		return false;

	AString strLine;
	DWORD dwReadLen;
	char szLine[256];
	int nCount;

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

	int i,j,k;
	int nIndex, nPos;
	A3DMATRIX4 mTemp;
	for (i=0; i<nCount; i++)
	{
		// Read bone index
		f.ReadLine(szLine, 256, &dwReadLen);
		sscanf(szLine, "%d", &nIndex);
		// Read bone matrix
		for (j=0; j<4; j++)
		{
			f.ReadLine(szLine, 256, &dwReadLen);
			strLine = szLine;
			for (k=0; k<4; k++)
			{
				nPos = strLine.Find(' ');
				strLine = strLine.Right(strLine.GetLength() - nPos - 1);
				sscanf(strLine, "%f", &mTemp.m[j][k]);
			}
		}
		m_FaceBone[nIndex].SetTransformMatrix(mTemp);
		UpdateBone(nIndex, true);
	}

	f.Close();

	return true;
}

// Save all bone
bool CFaceBoneController::SaveAllBoneToFile(const char* szFile)
{
	AFile f;
	if (!f.Open(szFile, AFILE_TEXT | AFILE_CREATENEW))
		return false;

	AString strLine;
	strLine.Format("%d", TOTAL_BONE_COUNT);
	f.WriteLine("");
	f.WriteLine(strLine);
	f.WriteLine("");

	int i;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		if (!m_FaceBone[i].SaveBone(&f, i))
		{
			f.Close();
			return false;
		}
	}

	f.Close();
	return true;
}

// Save selected bones
bool CFaceBoneController::SaveSelBoneToFile(const char* szFile, bool bHalfFace)
{
	AFile f;
	if (!f.Open(szFile, AFILE_TEXT | AFILE_CREATENEW))
		return false;

	int i;
	int nSelectIndex;
	int nTotalCount = m_nSelectedCount;
	for (i=0; i<m_nSelectedCount; i++)
	{
		nSelectIndex = m_SelectedBone[i];
		if (bHalfFace)
		{
			if (nSelectIndex < RIGHT_BONE_COUNT)
				nTotalCount++;
		}
	}

	AString strLine;
	strLine.Format("%d", nTotalCount);
	f.WriteLine("");
	f.WriteLine(strLine);
	f.WriteLine("");

	for (i=0; i<m_nSelectedCount; i++)
	{
		nSelectIndex = m_SelectedBone[i];
		if (!m_FaceBone[nSelectIndex].SaveBone(&f, nSelectIndex))
		{
			f.Close();
			return false;
		}
		if (bHalfFace)
		{
			// Save mirror bone
			if (nSelectIndex < RIGHT_BONE_COUNT)
			{
				nSelectIndex += ACTIVE_BONE_COUNT;
				if (!m_FaceBone[nSelectIndex].SaveBone(&f, nSelectIndex))
				{
					f.Close();
					return false;
				}
			}
		}
	}

	f.Close();
	return true;
}

// Store current transform matrix for bones
void CFaceBoneController::StoreBoneTM()
{
	if (!m_bReady)
		return;

	int i;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].StoreTM();
	}
}

// Get bones tranform matrix change from old to now
void CFaceBoneController::GetBoneTMChange(int* pIndex, A3DMATRIX4* pMatrix)
{
	int i;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		pIndex[i] = i;
		pMatrix[i] = m_FaceBone[i].GetTMFromOldToNow();
	}
}

// Update Bone
void CFaceBoneController::UpdateBone(int nIndex, bool bSaveShape)
{
	if (NULL == m_pSkeleton)
		return;

	m_FaceBone[nIndex].Update(bSaveShape, 0);

}

// Update all bones
void CFaceBoneController::UpdateAllBone(bool bSaveShape)
{
	if (NULL == m_pSkeleton)
		return;

	for (int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].Update(bSaveShape, 0);
	}
}

// Release
void CFaceBoneController::Release()
{

}
/*
// Blend bone
void CFaceBoneController::BlendBone(int nIndex, const A3DMATRIX4* mats, float* weights, int nNumMat)
{
	if (NULL == m_pSkeleton)
		return;

	m_FaceBone[nIndex].Add(mats, weights, nNumMat);
}
*/
// Scale part down
void CFaceBoneController::ScalePartDown(float fScale, bool bUpdate)
{
	ASSERT(fScale > 0);

	m_BonePartScale[2] = fScale;

	char szCenter[20];
	strcpy(szCenter, "鼻型060");
	int nIndex = GetBoneIndexByName(szCenter);
	SetSpecialCenter(nIndex);

	ClearSelection();
	SelectBonePart(2);

	ScaleBone(A3DVECTOR3(1.0, fScale, 1.0), CT_SPECIAL, bUpdate, true);

}

// Select a part
void CFaceBoneController::SelectBonePart(int nPart)
{
	ASSERT(nPart >= 0 );
	ASSERT(nPart < BONE_PART_COUNT);

	for (int i=0; i<m_BonePart[nPart].GetSize(); i++)
	{
		AddSelection(m_BonePart[nPart][i]);
	}
}

// Scale part up
void CFaceBoneController::ScalePartUp(float fScale, bool bUpdate)
{
	ASSERT(fScale > 0);

	m_BonePartScale[0] = fScale;

	char szCenter[20];
	strcpy(szCenter, "鼻型010");
	int nIndex = GetBoneIndexByName(szCenter);
	SetSpecialCenter(nIndex);

	ClearSelection();
	SelectBonePart(0);

	ScaleBone(A3DVECTOR3(1.0, fScale, 1.0), CT_SPECIAL, bUpdate, true);

}

// Scale part middle
void CFaceBoneController::ScalePartMiddle(float fScale, bool bUpdate)
{
	ASSERT(fScale > 0);

	m_BonePartScale[1] = fScale;

	char szCenter[20];
	strcpy(szCenter, "鼻型060");
	int nIndex = GetBoneIndexByName(szCenter);
	SetSpecialCenter(nIndex);

	ClearSelection();
	SelectBonePart(1);

	ScaleBone(A3DVECTOR3(1.0, fScale, 1.0), CT_SPECIAL, bUpdate, true);

	
	strcpy(szCenter, "鼻型010");
	nIndex = GetBoneIndexByName(szCenter);
	SetSpecialCenter(nIndex);

	ClearSelection();
	SelectBonePart(0);

	A3DMATRIX4 mat = m_FaceBone[nIndex].GetTransformMatrix();
	A3DVECTOR3 vPosOld, vPos;
	vPosOld = m_FaceBone[nIndex].GetOldPos();
	vPos = vPosOld * mat;
	vPos = vPos - vPosOld;
	vPos.x = 0.0;
	vPos.z = 0.0;
	TranslateBone(vPos, bUpdate, true);

}

// Scale 3 parts
void CFaceBoneController::ScaleParts(float fUp, float fMid, float fDown)
{
	ScalePartDown(fDown, false);
	ScalePartMiddle(fMid, false);
	ScalePartUp(fUp, false);
	UpdateAllBone(true);
}

// Restore all old transform matrix
void CFaceBoneController::RestoreBoneTM()
{
	if (!m_bReady)
		return;

	for (int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].RestoreOldTM();
	}
}
/*
// Store current transform matrix as face shape
void CFaceBoneController::StoreShapeTM()
{
	if (!m_bReady)
		return;

	for (int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].StoreShapeTM();
	}
}

*/ 
// Restore all shape transform matrix
void CFaceBoneController::RestoreShapeTM()
{
	if (!m_bReady)
		return;

	for (int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].RestoreShapeTM();
	}
}

// Move cheek
bool CFaceBoneController::MoveCheek(float fDis, bool bUpdate, bool bHalfFace)
{
	// Calculate cheek's moving direction
	A3DVECTOR3 vCheek;
	vCheek = m_FaceBone[CHEEK_BONE_INDEX1].GetPos() - m_FaceBone[CHEEK_BONE_INDEX2].GetPos();
	vCheek.Normalize();
	vCheek = CrossProduct(A3DVECTOR3(0, 0, 1), vCheek);

	// Calculate cheek's moving distance
	A3DVECTOR3 vDis;
	vDis = vCheek * fDis;

	// Select cheek bones
	ClearSelection();
	for (int i=CHEEK_BONE_INDEX1+1; i<CHEEK_BONE_INDEX2; i++)
	{
		AddSelection(i);
	}

	// Translate cheek bones
	TranslateBone(vDis, bUpdate, bHalfFace);

	return true;
}

// Move nose bridge
bool CFaceBoneController::MoveNoseBridge(float fDis, bool bUpdate, bool bHalfFace)
{
	A3DVECTOR3 vDis;

	ClearSelection();
	AddSelection(51);
	vDis = A3DVECTOR3(0, 0, fDis);
	TranslateBone(vDis, bUpdate, bHalfFace);

	ClearSelection();
	AddSelection(50);
	vDis = A3DVECTOR3(0, 0, fDis*0.5f);
	TranslateBone(vDis, bUpdate, bHalfFace);

	ClearSelection();
	AddSelection(52);
	vDis = A3DVECTOR3(0, 0, fDis*0.8f);
	TranslateBone(vDis, bUpdate, bHalfFace);

	AddSelection(50);
	AddSelection(51);

	return true;
}

// Scale eyeball
bool CFaceBoneController::ScaleEyeball(A3DVECTOR3& vScale, bool bUpdate, EDITSCOPE scope)
{
	A3DVECTOR3 vCenter;

	if (scope == ESP_RIGHT || scope == ESP_BOTH)
	{
		// Select eye ball
		ClearSelection();
		AddSelection(RIGHT_EYEBALL_INDEX);

		// Get the scale center of the right ball
		// The z coordinate is the middle of the two eye corners
		vCenter.x = m_FaceBone[RIGHT_EYEBALL_INDEX].GetPos().x;
		vCenter.y = m_FaceBone[RIGHT_EYEBALL_INDEX].GetPos().y;
		vCenter.z = (m_FaceBone[14].GetPos().z + m_FaceBone[18].GetPos().z) / 2;

		// Scale right eye ball
		m_FaceBone[RIGHT_EYEBALL_INDEX].Scale(vCenter, vScale);

		// Update
		if (bUpdate)
		{
			UpdateBone(RIGHT_EYEBALL_INDEX, true);
		}
	}

	if (scope == ESP_LEFT || scope == ESP_BOTH)
	{
		// Get the scale center of the left ball
		// The z coordinate is the middle of the two eye corners
		vCenter.x = m_FaceBone[LEFT_EYEBALL_INDEX].GetPos().x;
		vCenter.y = m_FaceBone[LEFT_EYEBALL_INDEX].GetPos().y;
		vCenter.z = (m_FaceBone[79].GetPos().z + m_FaceBone[83].GetPos().z) / 2;

		// Scale left eye ball
		m_FaceBone[LEFT_EYEBALL_INDEX].Scale(vCenter, vScale);

		// Update
		if (bUpdate)
		{
			UpdateBone(LEFT_EYEBALL_INDEX, true);
		}
	}

	return true;
}

// Updown move mouth corner
bool CFaceBoneController::MoveMouthCorner(float fDis, bool bUpdate, EDITSCOPE scope)
{
	A3DVECTOR3 vDis;

	bool bHalfFace;	// Only right face is edited, but whole face moves.
	if (scope == ESP_BOTH)
		bHalfFace = true;
	else
		bHalfFace = false;

	if (scope == ESP_BOTH || scope == ESP_RIGHT)
	{
		// Select most mouth corner
		ClearSelection();
		AddSelection(RIGHT_MOUTH_CORNER);
		// Calculate displacement
		vDis = A3DVECTOR3(0, fDis, 0);
		// Move most corner
		TranslateBone(vDis, bUpdate, bHalfFace);

		// Select middle mouth corner
		ClearSelection();
		AddSelection(31);
		AddSelection(33);
		AddSelection(37);
		AddSelection(40);
		// Calculate displacement
		vDis = A3DVECTOR3(0, fDis * 0.5f, 0);
		// Move middle corner
		TranslateBone(vDis, bUpdate, bHalfFace);

		// Select inner mouth corner
		ClearSelection();
		AddSelection(30);
		AddSelection(34);
		AddSelection(38);
		AddSelection(41);
		// Calculate displacement
		vDis = A3DVECTOR3(0, fDis * 0.25f, 0);
		// Move inner corner
		TranslateBone(vDis, bUpdate, bHalfFace);

		// Select other corner bones for showing
		AddSelection(31);
		AddSelection(33);
		AddSelection(37);
		AddSelection(40);
		AddSelection(RIGHT_MOUTH_CORNER);
	}
	else if (scope == ESP_LEFT)
	{
		// Select most mouth corner
		ClearSelection();
		AddSelection(RIGHT_MOUTH_CORNER + ACTIVE_BONE_COUNT);
		// Calculate displacement
		vDis = A3DVECTOR3(0, fDis, 0);
		// Move most corner
		TranslateBone(vDis, bUpdate, bHalfFace);

		// Select middle mouth corner
		ClearSelection();
		AddSelection(31 + ACTIVE_BONE_COUNT);
		AddSelection(33 + ACTIVE_BONE_COUNT);
		AddSelection(37 + ACTIVE_BONE_COUNT);
		AddSelection(40 + ACTIVE_BONE_COUNT);
		// Calculate displacement
		vDis = A3DVECTOR3(0, fDis * 0.5f, 0);
		// Move middle corner
		TranslateBone(vDis, bUpdate, bHalfFace);

		// Select inner mouth corner
		ClearSelection();
		AddSelection(30 + ACTIVE_BONE_COUNT);
		AddSelection(34 + ACTIVE_BONE_COUNT);
		AddSelection(38 + ACTIVE_BONE_COUNT);
		AddSelection(41 + ACTIVE_BONE_COUNT);
		// Calculate displacement
		vDis = A3DVECTOR3(0, fDis * 0.25f, 0);
		// Move inner corner
		TranslateBone(vDis, bUpdate, bHalfFace);
	}

	return true;
}

/*
// Blend bone for expression
void CFaceBoneController::BlendBoneExp(int nIndex, const A3DMATRIX4* mats, float* weights, int nNumMat)
{
	if (NULL == m_pSkeleton)
		return;

	m_FaceBone[nIndex].Add(mats, weights, nNumMat);
}
*/
// Save current transform matrix to backup face.
void CFaceBoneController::BackupBoneTM()
{
	if (!m_bReady)
		return;

	int i;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].BackupTM(0);
	}
}

// Apply other organs to backup face
void CFaceBoneController::ApplyBonesOnBakFace(bool bUpdate)
{
	if (!m_bReady)
		return;

	int i;
	for (i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].ApplyBoneOnBakFace(0);
		if (bUpdate)
			m_FaceBone[i].Update(true, 0);
	}
}

// Reset selected bones
void CFaceBoneController::ResetSelectedBones(bool bUpdate, bool bHalfFace)
{
	if (!m_bReady)
		return;

	int nIndex;
	for (int i=0; i<m_nSelectedCount; i++)
	{
		nIndex = m_SelectedBone[i];
		m_FaceBone[nIndex].ResetTM();
		if (bUpdate)
			m_FaceBone[nIndex].Update(bUpdate, 0);
		if (bHalfFace)
		{
			if (nIndex < RIGHT_BONE_COUNT)
			{
				nIndex += ACTIVE_BONE_COUNT;
				m_FaceBone[nIndex].ResetTM();
				if (bUpdate)
					m_FaceBone[nIndex].Update(bUpdate, 0);
			}
		}
	}
}

// Add shape to face
void CFaceBoneController::AddShape(int nIndex, const A3DMATRIX4* mats, const float* weights, int nNumMat, int nDuration)
{
	if (NULL == m_pSkeleton)
		return;

	m_FaceBone[nIndex].Add(mats, weights, nNumMat, nDuration, true);
}

// Add expression to face
void CFaceBoneController::AddExpr(int nIndex, const A3DMATRIX4* mats, const float* weights, int nNumMat, int nDuration)
{
	if (NULL == m_pSkeleton)
		return;

	m_FaceBone[nIndex].Add(mats, weights, nNumMat, nDuration, false);
}

// Remove left bones from selection
void CFaceBoneController::RemoveLeftBones()
{
	int nIndex = 0;
	while (nIndex<m_nSelectedCount)
	{
		if (m_SelectedBone[nIndex]>=ACTIVE_BONE_COUNT)
		{
			RemoveSelection(nIndex);
			m_nSelectedCount--;
		}
		else
			nIndex++;
	}
}

// Reset all bones
void CFaceBoneController::ResetAllBones(bool bUpdate)
{
	A3DMATRIX4 m;
	m.Identity();
	for (int i=0; i<TOTAL_BONE_COUNT; i++)
	{
		m_FaceBone[i].SetShapeTM(m);
	}
	if (bUpdate)
		UpdateAllBone(true);
}

// Mirror select from right to left
void CFaceBoneController::MirrorSelectFromRightToLeft()
{
	for (int i=0; i<m_nSelectedCount; i++)
	{
		if (m_SelectedBone[i] < RIGHT_BONE_COUNT)
		{
			m_SelectedBone[i] += ACTIVE_BONE_COUNT;
		}
	}
}
