/*
 * FILE: EC_Object.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Object.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_GPDataType.h"
#include "EC_NPC.h"
#include "EC_Player.h"
#include "EC_ManMatter.h"
#include "EC_Matter.h"
#include "A3DFuncs.h"

#define new A_DEBUG_NEW

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
//	Implement CECObject
//	
///////////////////////////////////////////////////////////////////////////

CECObject::CECObject()
{
	m_iCID			= OCID_OBJECT;
	m_bAdjustOrient = false;
	m_bVisible		= true;
	m_bBornInSight	= false;
	m_dwBornStamp	= 0;

	m_bUseGroundNormal = false;
	m_vecGroundNormal = g_vAxisY;
	m_vecGroundNormalSet = g_vAxisY;

	m_bSelectable = true;
}

CECObject::~CECObject()
{
}

//	Tick routine
bool CECObject::Tick(DWORD dwDeltaTime)
{
	if (m_bAdjustOrient)
		AdjustOrientation(dwDeltaTime);

	m_vecGroundNormal = Normalize(m_vecGroundNormal * 0.85f + m_vecGroundNormalSet * 0.15f); 
	return true;
}

//	Set absolute forward and up direction
void CECObject::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if( m_bUseGroundNormal )
	{
		A3DVECTOR3 vecRight = Normalize(CrossProduct(g_vAxisY, vDir));
		A3DVECTOR3 vecNormal = m_vecGroundNormal - DotProduct(m_vecGroundNormal, vecRight) * vecRight;
		A3DVECTOR3 vecNewDir = Normalize(CrossProduct(vecRight, vecNormal));
		A3DCoordinate::SetDirAndUp(vecNewDir, vecNormal);	
	}
	else
		A3DCoordinate::SetDirAndUp(vDir, vUp);

	//	Stop orientation adjusting
	m_bAdjustOrient = false;
}

//	Set destination orientation of model
void CECObject::SetDestDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, DWORD dwTime)
{
	m_bAdjustOrient		= true;
	m_dwOrientTime		= dwTime;
	m_dwOrientTimeCnt	= 0;

	//	Calculate start orientation
	A3DMATRIX4 mat = a3d_TransformMatrix(GetDir(), GetUp(), g_vOrigin);
	m_quOrientStart.ConvertFromMatrix(mat);

	//	Calculate end orientation
	if( m_bUseGroundNormal )
	{
		A3DVECTOR3 vecRight = Normalize(CrossProduct(g_vAxisY, vDir));
		A3DVECTOR3 vecNormal = m_vecGroundNormal - DotProduct(m_vecGroundNormal, vecRight) * vecRight;
		A3DVECTOR3 vecNewDir = Normalize(CrossProduct(vecRight, vecNormal));
		mat = a3d_TransformMatrix(vecNewDir, vecNormal, g_vOrigin);
	}
	else
	{
		mat = a3d_TransformMatrix(vDir, vUp, g_vOrigin);
	}

	m_quOrientEnd.ConvertFromMatrix(mat);
}

//	Adjust orientation
void CECObject::AdjustOrientation(DWORD dwDeltaTime)
{
	m_dwOrientTimeCnt += dwDeltaTime;

	A3DMATRIX4 mat;

	//	Adjust model's orientation
	if (m_dwOrientTimeCnt >= m_dwOrientTime)
	{
		m_quOrientEnd.ConvertToMatrix(mat);
		SetDirAndUp(mat.GetRow(2), mat.GetRow(1));
		m_bAdjustOrient = false;
	}
	else
	{
		float f = (float)m_dwOrientTimeCnt / m_dwOrientTime;
		A3DQUATERNION qu = SLERPQuad(m_quOrientStart, m_quOrientEnd, f);
		qu.Normalize();
		qu.ConvertToMatrix(mat);
		SetDirAndUp(mat.GetRow(2), mat.GetRow(1));
		//	Note: SetDirAndUp will change m_bAdjustOrient flag
		m_bAdjustOrient = true;
	}
}

//	Turn around object and face to specified object
void CECObject::TurnFaceTo(int idTarget, DWORD dwTime)
{
	//	Face to target
	A3DVECTOR3 vTarget;

	CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(idTarget, 0);
	if (!pObject)
		return;

	if (ISNPCID(idTarget))
	{
		CECNPC* pNPC = (CECNPC*)pObject;
		vTarget = pNPC->GetPos();
	}
	else if (ISPLAYERID(idTarget))
	{
		CECPlayer* pPlayer = (CECPlayer*)pObject;
		vTarget = pPlayer->GetPos();
	}
	else
	{
		CECMatter* pMatter = g_pGame->GetGameRun()->GetWorld()->GetMatterMan()->GetMatter(idTarget);
		if (pMatter && pMatter->IsMonsterSpiritMine()) vTarget = pMatter->GetPos(); 
		else return;
	}

	//	Every some time we check whether player still face to
	//	it's attack target.
	A3DVECTOR3 vDir = vTarget - GetPos();
	vDir.y = 0.0f;
	if (!vDir.Normalize())
		vDir = GetDir();

	SetDestDirAndUp(vDir, g_vAxisY, dwTime);
}

void CECObject::SetUseGroundNormal(bool bFlag)
{
	if( m_bUseGroundNormal == bFlag )
		return;

	m_bUseGroundNormal = bFlag;
	if( m_bUseGroundNormal )
	{
		// now reset dir and up to make the object show correct pose
		SetDirAndUp(GetDir(), GetUp());
	}
	else
	{
		// now reset dir and up to make the player show correct pose
		A3DVECTOR3 vLeft = CrossProduct(GetDir(), g_vAxisY);
		A3DVECTOR3 vDir = Normalize(CrossProduct(g_vAxisY, vLeft));
		SetDirAndUp(vDir, g_vAxisY);
	}
}

void CECObject::SetGroundNormal(const A3DVECTOR3& vecNormal)
{
	m_vecGroundNormalSet = vecNormal;
}

//	Get object id
int CECObject::GetObjectID(CECObject* pObject)
{
	if (!pObject)
		return 0;

	if (pObject->IsPlayer())
		return ((CECPlayer*)pObject)->GetCharacterID();
	else if (pObject->IsNPC())
		return ((CECNPC*)pObject)->GetNPCID();
	else if (pObject->IsMatter())
		return ((CECMatter*)pObject)->GetMatterID();
	else
		return 0;
}
