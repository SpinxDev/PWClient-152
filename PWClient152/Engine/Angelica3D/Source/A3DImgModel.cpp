/*
 * FILE: A3DImgModel.cpp
 *
 * DESCRIPTION: Class for Image model object
 *
 * CREATED BY: duyuxin, 2002/11/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DImgModel.h"
#include "A3DImgModelMan.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DCollision.h"
#include "A3DDevice.h"
#include "A3DCameraBase.h"
#include "AFileImage.h"
#include "AScriptFile.h"

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
//	Implement of A3DImgModel
//
///////////////////////////////////////////////////////////////////////////

A3DImgModel::A3DImgModel()
{
	m_pManager		= NULL;
	m_aGroups		= NULL;
	m_iNumGroup		= 0;
	m_dwID			= 0;
	m_bPause		= false;
	m_bStop			= true;
	m_iCurAction	= 0;
	m_iFrameCnt		= 0;
	m_iCurCell		= 0;
	m_iActIndex		= -1;
	m_iMGIndex		= -1;
	m_bVisible		= true;
	m_bNoAction		= true;
	m_vPos			= A3DVECTOR3(0.0f);
	m_vCenter		= A3DVECTOR3(0.0f);
	m_vRight		= A3DVECTOR3(-1.0f, 0.0f, 0.0f);
	m_vRotateAxis	= A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_szName[0]		= '\0';
	m_pCurGroup		= NULL;
	m_bViewCull		= true;
	m_RayInfo.iType	= RAYTRACE_NONE;	//	Don't do ray trace

	memset(m_aVerts, 0, sizeof (m_aVerts));

	//	Default sole group data
	memset(&m_SoleGroup, 0, sizeof (m_SoleGroup));

	m_SoleGroup.aIndices[0] = -1;
	m_SoleGroup.bLoop		= false;
	m_SoleGroup.iNumAction	= 1;

	//	Default color of diffuse and specular
	m_aVerts[0].diffuse	= 0xffffffff;
	m_aVerts[1].diffuse = 0xffffffff;
	m_aVerts[2].diffuse = 0xffffffff;
	m_aVerts[3].diffuse = 0xffffffff;

	m_aVerts[0].specular = 0xff000000;
	m_aVerts[1].specular = 0xff000000;
	m_aVerts[2].specular = 0xff000000;
	m_aVerts[3].specular = 0xff000000;
}

A3DImgModel::~A3DImgModel()
{
}

/*	Copy image model. Note, this function don't copy image model's action state.

	Return true for success, otherwise return false.

	pSrc: source image model
*/
bool A3DImgModel::Copy(A3DImgModel* pSrc)
{
	//	Release old data
	Release();

	//	Copy action groups
	m_iNumGroup = pSrc->m_iNumGroup;

	if (!(m_aGroups = (ACTGROUP*)malloc(sizeof (ACTGROUP) * m_iNumGroup)))
	{
		g_A3DErrLog.Log("A3DImgModel::Copy, Not enough memory !");
		return false;
	}

	memcpy(m_aGroups, pSrc->m_aGroups, sizeof (ACTGROUP) * m_iNumGroup);

	//	Other data
	m_pManager		= pSrc->m_pManager;
	m_dwID			= pSrc->m_dwID;
	m_iMGIndex		= pSrc->m_iMGIndex;
	m_vPos			= pSrc->m_vPos;
	m_vRight		= pSrc->m_vRight;
	m_vRotateAxis	= pSrc->m_vRotateAxis;
	m_RayInfo.iType	= pSrc->m_RayInfo.iType;

	strcpy_s(m_szName, pSrc->m_szName);

	m_bPause		= false;
	m_bStop			= true;
	m_iCurAction	= 0;
	m_iFrameCnt		= 0;
	m_iCurCell		= 0;
	m_iActIndex		= -1;
	m_bVisible		= true;
	m_bNoAction		= true;
	m_pCurGroup		= NULL;

	return true;
}

/*	Bit-Copy image model. This function directly copy srouce image model's data,
	including dynamic allocated memory data. Use this function very carefully !!!
	Be sure this image model is exactly same as source one and all dynamic
	buffers must have been prepared. This function don't image model's action state.

	Return true for success, otherwise return false.

	pSrc: source image model
*/
bool A3DImgModel::BitCopy(A3DImgModel* pSrc)
{
	if (!m_aGroups || m_iNumGroup != pSrc->m_iNumGroup)
	{
		g_A3DErrLog.Log("A3DImgModel::BitCopy, Dismatch objects!");
		return false;
	}

	memcpy(m_aGroups, pSrc->m_aGroups, sizeof (ACTGROUP) * m_iNumGroup);

	//	Other data
	m_pManager		= pSrc->m_pManager;
	m_dwID			= pSrc->m_dwID;
	m_iMGIndex		= pSrc->m_iMGIndex;
	m_vPos			= pSrc->m_vPos;
	m_vRight		= pSrc->m_vRight;
	m_vRotateAxis	= pSrc->m_vRotateAxis;
	m_RayInfo.iType	= pSrc->m_RayInfo.iType;

	strcpy_s(m_szName, pSrc->m_szName);

	m_bPause		= false;
	m_bStop			= true;
	m_iCurAction	= 0;
	m_iFrameCnt		= 0;
	m_iCurCell		= 0;
	m_iActIndex		= -1;
	m_bVisible		= true;
	m_bNoAction		= true;
	m_pCurGroup		= NULL;

	return true;
}

/*	Initialize image model object.

	Return true for success, otherwise return false.

	pMan: image model manager object's address
	szFile: image model file which describe the object.
	iMGIndex: Index of model group this model belongs to
*/
bool A3DImgModel::Load(A3DImageModelMan *pMan, char* szFile, int iMGIndex)
{
	assert(pMan);

	m_pManager = pMan;
	m_iMGIndex = iMGIndex;

	AFileImage File;
	AScriptFile Script;

	if (!File.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DImgModel::Load, Failed to open file %s", szFile);
		return false;
	}

	if (!Script.Open(&File))
	{
		g_A3DErrLog.Log("A3DImgModel::Load, Failed to open script file.");
		File.Close();
		return false;
	}

	File.Close();

	int iCount = 0;

	//	Read model name
	if (!Script.GetNextToken(true))
	{
		g_A3DErrLog.Log("A3DImgModel::Load, Failed to open script file.");
		goto Error;
	}

	strcpy_s(m_szName, Script.m_szToken);
	m_dwID = a_MakeIDFromString(m_szName);

	if (!Script.MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImgModel::Load, Failed to match '{'");
		goto Error;
	}

	while (Script.PeekNextToken(true))
	{
		if (!_stricmp(Script.m_szToken, "}"))
		{
			Script.GetNextToken(true);	//	Skip '{'
			break;
		}

		if (!_stricmp(Script.m_szToken, "actgroupnumber"))
		{
			Script.GetNextToken(true);	//	Skip 'actgroupnumber'

			Script.GetNextToken(true);
			m_iNumGroup = atoi(Script.m_szToken);
			Script.SkipLine();

			if (m_iNumGroup)
			{
				if (!(m_aGroups = (ACTGROUP*)malloc(sizeof (ACTGROUP) * m_iNumGroup)))
				{
					g_A3DErrLog.Log("A3DImgModel::Load, Not enough memory !");
					goto Error;
				}
				
				memset(m_aGroups, 0, sizeof (ACTGROUP) * m_iNumGroup);
			}
		}
		else	//	Is action group
		{
			if (!ParseActionGroup(&Script, &m_aGroups[iCount++]))
			{
				free(m_aGroups);
				g_A3DErrLog.Log("A3DImgModel::Load, Failed to parse action group");
				goto Error;
			}
		}
	}

	assert(iCount == m_iNumGroup);

	m_bNoAction = true;

	Script.Close();
	return true;

Error:

	Script.Close();
	return false;
}

//	Release
void A3DImgModel::Release()
{
	//	Release all action groups
	if (m_aGroups)
	{
		free(m_aGroups);
		m_aGroups = NULL;
	}

	m_iNumGroup	= 0;
	m_pManager	= NULL;
}

/*	Parse action group in image model file

	Return true for success, otherwise return false.

	pFile: image model file's pointer.
	pGroup: used to receive group information
*/
bool A3DImgModel::ParseActionGroup(AScriptFile* pFile, ACTGROUP* pGroup)
{
	//	Action group name
	pFile->GetNextToken(true);
	strcpy_s(pGroup->szName, pFile->m_szToken);

	//	Make ID
	pGroup->dwID = a_MakeIDFromString(pGroup->szName);

	//	Number of action in this group
	pFile->GetNextToken(false);
	pGroup->iNumAction = atoi(pFile->m_szToken);

	//	Loop flag
	pFile->GetNextToken(false);
	pGroup->bLoop = atoi(pFile->m_szToken) ? true : false;

	if (!pFile->MatchToken("{", false))
	{
		g_A3DErrLog.Log("A3DImgModel::ParseActionGroup, Failed to match {");
		return false;
	}

	int iCount = 0;

	while (pFile->PeekNextToken(true))
	{
		if (!_stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '{'
			break;
		}

		//	ID of ation
		pFile->GetNextToken(true);
		pGroup->aActions[iCount] = a_MakeIDFromString(pFile->m_szToken);
		pGroup->aIndices[iCount] = -1;
		iCount++;

		pFile->SkipLine();
	}

	assert(iCount == pGroup->iNumAction);

	return true;
}

/*	Set current action in current group.

	iAction: action's index
	iStart: start frame of play. -1 means to use the default start frame
			of action, which is	IMGMODELACTION.iStart;
*/
void A3DImgModel::SetCurrentAction(int iAction, int iStart)
{
	if (iAction < 0 || iAction >= m_pCurGroup->iNumAction)
		return;

	m_iCurAction = iAction;

	if (m_pCurGroup->aIndices[iAction] < 0)
	{
		m_iActIndex = m_pManager->GetAction(m_iMGIndex, m_pCurGroup->aActions[iAction], &m_CurAct);
		m_pCurGroup->aIndices[iAction] = m_iActIndex;
	}
	else
	{
		//	GetActionByIndex() is faster than GetAction()
		m_iActIndex = m_pManager->GetActionByIndex(m_iMGIndex, m_pCurGroup->aIndices[iAction], &m_CurAct);
	}

	//	Calculate the start frame
	if (iStart < 0)
		m_iFrameCnt	= 0;
	else
	{
		if (iStart >= m_CurAct.iNumFrame)
			iStart = m_CurAct.iNumFrame - 1;

		m_iFrameCnt = m_CurAct.iFirstCell + iStart - m_CurAct.iStart;
		if (m_iFrameCnt < 0)
			m_iFrameCnt += m_CurAct.iNumFrame;
	}

	SetCurrentFrame();
}

//	Set current frame's index (not frame count)
void A3DImgModel::SetCurrentFrame()
{
	if (m_CurAct.bForward)
	{
		m_iCurCell = m_CurAct.iStart + m_iFrameCnt;
		if (m_iCurCell > m_CurAct.iLastCell)
			m_iCurCell -= m_CurAct.iNumFrame;
	}
	else
	{
		m_iCurCell = m_CurAct.iStart - m_iFrameCnt;
		if (m_iCurCell < m_CurAct.iLastCell)
			m_iCurCell += m_CurAct.iNumFrame;
	}
	
	return;
}

/*	Play specified action group

	Return true for success, otherwise return false.

	szName: specified action group's name
	iStart: start frame of play. -1 means to use the default start frame
			of action, which is	IMGMODELACTION.iStart;
*/
bool A3DImgModel::PlayActionByName(char* szName, int iStart/* -1 */)
{
	assert(szName);

	bool bGroup = szName[0] == '[' ? true : false;

	if (!m_aGroups && bGroup)
	{
		m_bNoAction = true;
		return false;
	}

	DWORD dwID = a_MakeIDFromString(szName);

	if (bGroup)
	{
		int i;
		for (i=0; i < m_iNumGroup; i++)
		{
			if (m_aGroups[i].dwID == dwID)
				break;
		}
		
		if (i >= m_iNumGroup || !m_aGroups[i].iNumAction)
		{
			m_bNoAction = true;
			return false;
		}
		
		m_pCurGroup	= &m_aGroups[i];
	}
	else
	{
		if (m_SoleGroup.aActions[0] == dwID && m_SoleGroup.aIndices[0] >= 0)
			m_pManager->GetActionByIndex(m_iMGIndex, m_SoleGroup.aIndices[0], &m_CurAct);
		else
		{
			m_SoleGroup.aIndices[0] = m_pManager->GetAction(m_iMGIndex, dwID, &m_CurAct);
			m_SoleGroup.aActions[0] = dwID;

			if (m_SoleGroup.aIndices[0] < 0)	//	Cannot find action
			{
				m_bNoAction = true;
				return false;
			}
		}

		m_pCurGroup = &m_SoleGroup;
	}

	m_bPause	= false;
	m_bStop		= false;
	m_bNoAction	= false;

	SetCurrentAction(0, iStart);

	return true;
}

/*	This function doesn't render model really, just add it to render queue, 

	Return true for success, otherwise return false.

	pViewport: current viewport used to render image model
	bUpdateShape: true, do update shape operation. If UpdateShape() has been
				  called separately, this function can be set to false
*/
bool A3DImgModel::FakeRender(A3DViewport* pViewport, bool bUpdateShape)
{
	if (!m_bVisible || m_bNoAction)
		return true;

	assert(m_pManager);

	if (bUpdateShape)
		UpdateShape(pViewport->GetCamera());

	if (m_bViewCull)
	{
		//	Cull model with it's bounding sphere
		if (!pViewport->GetCamera()->SphereInViewFrustum(m_vCenter, m_CellInfo.fWidth + m_CellInfo.fHeight))
			return true;
	}

	return m_pManager->AddRenderModel(this);
}

//	Tick animation
bool A3DImgModel::TickAnimation()
{
	if (m_bPause || m_bStop || m_bNoAction)
		return true;

	if (!m_pCurGroup || !m_pCurGroup->iNumAction)
	{
		assert(0);
		return false;
	}

	int iCurAction = m_iCurAction;

	m_iFrameCnt++;
	if (m_iFrameCnt >= m_CurAct.iNumFrame)
	{
		iCurAction++;
		m_iFrameCnt = 0;
			
		if (iCurAction >= m_pCurGroup->iNumAction)
		{
			//	If the whole action group is loop
			if (m_pCurGroup->bLoop)
				iCurAction = 0;
			else if (m_CurAct.bLoop)	//	If the last action is loop, play it looply
				iCurAction--;
			else
			{
				m_bStop = true;
				return true;
			}
		}
	}

	if (iCurAction != m_iCurAction)
		SetCurrentAction(iCurAction, -1);
	else
		SetCurrentFrame();

	return true;
}

/*	Set vertex color

	v: index of vertex which color will be set. -1 means set all vertices' color
	dwCol: vertex's new color
*/
void A3DImgModel::SetVertexCol(int v, DWORD dwCol)
{
	if (v >= 0)
		m_aVerts[v].diffuse = dwCol;
	else
	{
		m_aVerts[0].diffuse = dwCol;
		m_aVerts[1].diffuse = dwCol;
		m_aVerts[2].diffuse = dwCol;
		m_aVerts[3].diffuse = dwCol;
	}
}

//	Pause animation
void A3DImgModel::PauseAniamtion(bool bPause)
{
	m_bPause = bPause;
}

/*	Get mark point position in world space

	Return mark point's position in world space.

	szName: mark point's position.
*/
A3DVECTOR3 A3DImgModel::GetMarkPointPos(char* szName)
{
	A3DVECTOR3 vPos(0.0f);

	if (m_bNoAction)
		return vPos;

	float fOffx, fOffy;
	if (!m_pManager->GetMarkPoint(m_iMGIndex, szName, &fOffx, &fOffy))
		return vPos;

	vPos = m_vPos + m_vRight * fOffx + m_vRotateAxis * fOffy;

	return vPos;
}

//	Get current frame. Return -1 for failure
int	A3DImgModel::GetCurFrame()
{
	if (m_bNoAction)
		return -1;

	return m_iCurCell - m_CurAct.iFirstCell;
}

/*	Update shape.

	pCamera: camera to which the image model will face
*/
void A3DImgModel::UpdateShape(A3DCameraBase* pCamera)
{
	if (m_bNoAction)
		return;

	//	Get frame cell's information
	m_pManager->GetFrameCell(m_iMGIndex, m_iCurCell, &m_CellInfo);

	A3DVECTOR3 vPos0, vPos, vView = pCamera->GetDir();
	
	float fLen = Normalize(CrossProduct(vView, m_vRotateAxis), m_vRight);
	if (fLen < 0.5f)
		m_vRight = A3DVECTOR3(-1.0f, 0.0f, 0.0f);

	vPos0 = m_vPos + m_vRight * m_CellInfo.fOffx + m_vRotateAxis * m_CellInfo.fOffy;
	m_aVerts[0].x	= vPos0.x;
	m_aVerts[0].y	= vPos0.y;
	m_aVerts[0].z	= vPos0.z;
	m_aVerts[0].tu	= m_CellInfo.u1;
	m_aVerts[0].tv	= m_CellInfo.v1;
	m_vCenter = vPos0;

	vPos = vPos0 - m_vRight * m_CellInfo.fWidth;
	m_aVerts[1].x	= vPos.x;
	m_aVerts[1].y	= vPos.y;
	m_aVerts[1].z	= vPos.z;
	m_aVerts[1].tu	= m_CellInfo.u2;
	m_aVerts[1].tv	= m_CellInfo.v1;
	m_vCenter = m_vCenter + vPos;

	vPos = vPos - m_vRotateAxis * m_CellInfo.fHeight;
	m_aVerts[2].x	= vPos.x;
	m_aVerts[2].y	= vPos.y;
	m_aVerts[2].z	= vPos.z;
	m_aVerts[2].tu	= m_CellInfo.u2;
	m_aVerts[2].tv	= m_CellInfo.v2;
	m_vCenter = m_vCenter + vPos;

	vPos = vPos0 - m_vRotateAxis * m_CellInfo.fHeight;
	m_aVerts[3].x	= vPos.x;
	m_aVerts[3].y	= vPos.y;
	m_aVerts[3].z	= vPos.z;
	m_aVerts[3].tu	= m_CellInfo.u1;
	m_aVerts[3].tv	= m_CellInfo.v2;
	m_vCenter = (m_vCenter + vPos) * 0.25f;
}

/*	Do ray trace.

	Return true if specified ray collide this image model, otherwise return false.

	vStart: ray's start position
	vVel: ray's velocity
	fTime: ray's moving time
	pvHitPos (out): used to receive hit position if true is returned, can be NULL
	pfFrac (out): used to receive hit fraction if true is returned, can be NULL
*/
bool A3DImgModel::RayTrace(A3DVECTOR3& vStart, A3DVECTOR3& vVel, float fTime,
						   A3DVECTOR3* pvHitPos, float* pfFrac)
{
	if (m_RayInfo.iType	== RAYTRACE_NONE)
		return false;

	m_RayInfo.vStart	= vStart;
	m_RayInfo.vDir		= Normalize(vVel);
	m_RayInfo.vDelta	= vVel * fTime;

	bool bRet = false;

	switch (m_RayInfo.iType)
	{
	case RAYTRACE_SHEET:	bRet = RayTrace_Sheet();	break;
	case RAYTRACE_FIXAABB:
	case RAYTRACE_AABB:		bRet = RayTrace_AABB();		break;
	case RAYTRACE_FIXOBB:
	case RAYTRACE_OBB:		bRet = RayTrace_OBB();		break;
	}

	if (bRet)
	{
		if (pvHitPos)
			*pvHitPos = m_RayInfo.vHitPos;

		if (pfFrac)
			*pfFrac = m_RayInfo.fFrac;
	}

	return bRet;
}

//	Sheet ray trace
bool A3DImgModel::RayTrace_Sheet()
{
	float d, d1, d2, fDist;
	A3DVECTOR3 vNormal, aVerts[4];
	int i;
	
	for (i=0; i < 4; i++)
	{
		aVerts[i].x = m_aVerts[i].x;
		aVerts[i].y = m_aVerts[i].y;
		aVerts[i].z = m_aVerts[i].z;
	}

	vNormal = CrossProduct(m_vRight, m_vRotateAxis);
	fDist = DotProduct(aVerts[0], vNormal);

	d1 = DotProduct(m_RayInfo.vStart, vNormal) - fDist;
	d2 = DotProduct(m_RayInfo.vStart + m_RayInfo.vDelta, vNormal) - fDist;

	if (m_RayInfo.bTwoSide)
	{
		if (d1 < 0.0f)
			vNormal = -vNormal;
	}
	else if (d1 < 0.0f)		//	Start point is behind side
		return false;

	if (DotProduct(m_RayInfo.vDir, vNormal) >= 0.0f)
		return false;

	if (d1 < 0.0f)
		d = d1 / (d1 - d2);
	else
		d = d1 / (d1 - d2);

	if (d < 0.0f)
		d = 0.0f;

	//	Get normal's major axis
	int iMajor = 0;
	float fMax = (float)fabs(vNormal.x);
	
	fDist = (float)fabs(vNormal.y);
	if (fMax < fDist)
	{
		fMax = fDist;
		iMajor = 1;
	}
	
	if (fMax < (float)fabs(vNormal.z))
		iMajor = 2;

	A3DVECTOR3 vInter;
	bool bInter=false, bFlag1, bFlag2;
	int i0, i1;
	float *vert1, *vert2, *vHit;

	//	Calculate intersection point of line and plane
	vInter	= m_RayInfo.vDelta * d + m_RayInfo.vStart;
	vHit	= vInter.m;

	switch (iMajor)
	{
	case 0:	i0 = 1;	i1 = 2;	break;
	case 1:	i0 = 0; i1 = 2; break;
	case 2:	i0 = 0; i1 = 1; break;
	}

	vert1	= aVerts[3].m;
	bFlag1	= (vHit[i1] >= vert1[i1]);

	for (i=0; i < 4; i++)
	{
		vert2	= aVerts[i].m;
		bFlag2	= (vHit[i1] >= vert2[i1]);

		if (bFlag1 != bFlag2)
		{
			if (((vert2[i1] - vHit[i1]) * (vert1[i0] - vert2[i0]) >=
				(vert2[i0] - vHit[i0]) * (vert1[i1] - vert2[i1])) == bFlag2)
				bInter = !bInter;
		}

		vert1  = vert2;
		bFlag1 = bFlag2;
	}

	//	Calculate intersection point's position
	if (bInter)
	{
		m_RayInfo.fFrac		= d;
		m_RayInfo.vHitPos	= vInter;
		return true;
	}

	return false;
}

//	AABB ray trace
bool A3DImgModel::RayTrace_AABB()
{
	A3DVECTOR3 vMins, vMaxs;

	if (m_RayInfo.iType == RAYTRACE_FIXAABB)
	{
		vMins = m_vCenter - m_RayInfo.aabb.Extents;
		vMaxs = m_vCenter + m_RayInfo.aabb.Extents;
	}
	else
	{
		a3d_ClearAABB(vMins, vMaxs);
		for (int i=0; i < 4; i++)
		{
			A3DVECTOR3 v(m_aVerts[i].x, m_aVerts[i].y, m_aVerts[i].z);
			a3d_AddVertexToAABB(vMins, vMaxs, v);
		}
	}

	A3DVECTOR3 vNormal;
	if (CLS_RayToAABB3(m_RayInfo.vStart, m_RayInfo.vDelta, vMins, vMaxs,
					   m_RayInfo.vHitPos, &m_RayInfo.fFrac, vNormal))
		return true;

	return false;
}

//	OBB ray trace
bool A3DImgModel::RayTrace_OBB()
{
	A3DOBB obb;

	obb.Center	= m_vCenter;
	obb.XAxis	= m_vRight;
	obb.YAxis	= m_vRotateAxis;
	obb.ZAxis	= CrossProduct(m_vRight, m_vRotateAxis);

	obb.Extents.x = m_CellInfo.fWidth * 0.5f;
	obb.Extents.y = m_CellInfo.fHeight * 0.5f;
	
	if (m_RayInfo.iType == RAYTRACE_FIXOBB)
		obb.Extents.z = m_RayInfo.fThick * 0.5f;
	else
		obb.Extents.z = obb.Extents.x;

	obb.ExtX	= obb.XAxis * obb.Extents.x;
	obb.ExtY	= obb.YAxis * obb.Extents.y;
	obb.ExtZ	= obb.ZAxis * obb.Extents.z;

	A3DVECTOR3 vNormal;
	if (CLS_RayToOBB3(m_RayInfo.vStart, m_RayInfo.vDelta, obb, m_RayInfo.vHitPos,
					  &m_RayInfo.fFrac, vNormal))
		return true;

	return false;
}


