/*
 * FILE: EC_Sprite.cpp
 *
 * DESCRIPTION: Sprite follow a player
 *
 * CREATED BY: Hedi, 2005/12/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include <A3DTypes.h>
#include <A3DBone.h>
#include <A3DSkinModel.h>
#include <A3DSkinMan.h>
#include <A3DMacros.h>
#include <A3DEngine.h>
#include <A3DSkeleton.h>
#include <A3DFuncs.h>

#include "EC_Sprite.h"
#include "EC_Model.h"
#include "EC_Player.h"
#include "EC_Viewport.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_Utility.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECFace
//	
///////////////////////////////////////////////////////////////////////////

CECSprite::CECSprite()
{
	m_idSprite		= SPRITE_NULL;
	m_pModelSprite	= NULL;
	m_color			= 0;

	m_curState		= SPRITE_STATE_NULL;
	m_dwStateTicks	= 0;
	m_dwIdleAllowed	= 0;
}

CECSprite::~CECSprite()
{
	Release();
}

//	Initialize object
bool CECSprite::Init(SPRITE_ID idSprite, CECPlayer * pPlayer)
{
	const char * szModel = "";

	switch(idSprite)
	{
	case SPRITE_GOD1:
		szModel = "models\\npcs\\宠物\\观赏宠\\仙光球\\仙光球.ecm";
		m_color = A3DCOLORRGB(0, 200, 255);
		break;
	case SPRITE_GOD2:
		szModel = "models\\npcs\\宠物\\观赏宠\\小精灵\\小精灵.ecm";
		m_color = A3DCOLORRGB(0, 200, 255);
		break;
	case SPRITE_GOD3:
		szModel = "models\\npcs\\宠物\\观赏宠\\小天使\\小天使.ecm";
		m_color = A3DCOLORRGB(0, 200, 255);
		break;
	case SPRITE_EVIL1:
		szModel = "models\\npcs\\宠物\\观赏宠\\魔光球\\魔光球.ecm";
		m_color = A3DCOLORRGB(255, 0, 0);
		break;
	case SPRITE_EVIL2:
		szModel = "models\\npcs\\宠物\\观赏宠\\小恶魔2\\小恶魔2.ecm";
		m_color = A3DCOLORRGB(255, 0, 0);
		break;
	case SPRITE_EVIL3:
		szModel = "models\\npcs\\宠物\\观赏宠\\小恶魔3\\小恶魔3.ecm";
		m_color = A3DCOLORRGB(255, 0, 0);
		break;
	default:
		return false;
	}

	m_idSprite = idSprite;

//	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("armor.sdr", A3DSkinMan::SHADERREPLACE_REFLECTPREFIX);
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile((const char*)glb_ArmorReplaceShader_ReflectPrefix, A3DSkinMan::SHADERREPLACE_USERDEFINE);
	m_pModelSprite = new CECModel();
	if( !m_pModelSprite->Load(szModel) )
	{
		delete m_pModelSprite;
		m_pModelSprite = NULL;
		g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
		a_LogOutput(1, "CECSprite::Init(), failed to load ecmodel!");
		return false;
	}
	char szSkin[MAX_PATH];
	strncpy(szSkin, szModel, MAX_PATH);
	glb_ChangeExtension(szSkin, "ski");
	A3DSkin * pSkin = g_pGame->LoadA3DSkin(szSkin, false);
	if( pSkin )
	{
		m_pModelSprite->GetA3DSkinModel()->AddSkin(pSkin, true);
	}
	
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
	m_pModelSprite->PlayActionByName("休闲", 1.0f);
	m_pModelSprite->SetTransparent(1.0f);

	m_pPlayer		= pPlayer;
	// initialize some parameters.
	m_vSpeed		= 0.0f;
	m_vSpeedMax		= 20.0f;
	m_vAcceleration = 15.0f;
	m_vDeceleration = 15.0f;

	m_vecDir		= A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_curState		= SPRITE_STATE_IDLE;
	m_dwStateTicks	= 0;
	m_dwIdleAllowed = 2000;
	return true;
}

//	Release object
void CECSprite::Release()
{
	if( m_pModelSprite )
	{
		m_pModelSprite->Release();
		delete m_pModelSprite;
		m_pModelSprite = NULL;
	}
}

//	Tick routinue
bool CECSprite::Tick(DWORD dwDeltaTime)
{
	A3DVECTOR3 vecTargetPos;

	if( !m_pPlayer || !m_pPlayer->GetPlayerModel() || !m_pModelSprite )
		return true;

	CECModel * pPlayerModel = m_pPlayer->GetPlayerModel();
	int index;
	A3DBone* pHeadBone = pPlayerModel->GetA3DSkinModel()->GetSkeleton()->GetBone("Bip01 Head", &index);
	if( pHeadBone )
	{
		A3DVECTOR3 vecOffset = A3DVECTOR3(-0.4f, 0.4f, 0.4f);
		vecOffset = a3d_VectorMatrix3x3(vecOffset, m_pPlayer->GetAbsoluteTM());
		A3DMATRIX4 matHead = pHeadBone->GetAbsoluteTM();
		vecTargetPos = matHead.GetRow(3) + vecOffset;
	}
	else
	{
		A3DVECTOR3 vecOffset = A3DVECTOR3(-0.4f, 2.4f, 0.4f);
		vecOffset = a3d_VectorMatrix3x3(vecOffset, m_pPlayer->GetAbsoluteTM());
		vecTargetPos = m_pPlayer->GetPos() + vecOffset;
	}

	float		f = dwDeltaTime * 0.001f;
	A3DVECTOR3	vecDestDir;

	// change states now if needed.
	m_dwStateTicks += dwDeltaTime;
	if( m_pPlayer->IsPlayerMoving() )
	{
		if( m_curState != SPRITE_STATE_CHASE )
		{
			m_curState = SPRITE_STATE_CHASE;
			m_dwStateTicks = 0;
		}
		vecTargetPos = vecTargetPos + m_pPlayer->GetDir() * 3.5f;
	}
	else if( m_curState == SPRITE_STATE_IDLE )
	{
		if( m_dwStateTicks > m_dwIdleAllowed )
		{
			m_curState = (SPRITE_STATE) (SPRITE_STATE_TRICK1 + (rand() % 6));
			m_dwStateTicks = 0;
		}
	}
	else if( m_curState >= SPRITE_STATE_TRICK1 && m_curState <= SPRITE_STATE_TRICK6 )
	{
		if( m_dwStateTicks > 5000 )
		{
			m_curState = SPRITE_STATE_CHASE;
			m_dwStateTicks = 0;
		}
		else
		{
			float r		= 0.5f + m_dwStateTicks * 0.0002f;
			float rad	= m_dwStateTicks * 0.002f;
			float h		= m_dwStateTicks * 0.0005f;
			if( m_curState == SPRITE_STATE_TRICK1 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(rad);
				vecTargetPos.y = vecTargetPos.y - 2.6f + h;
			}
			else if( m_curState == SPRITE_STATE_TRICK2 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(rad);
				vecTargetPos.y = vecTargetPos.y - h;
			}
			else if( m_curState == SPRITE_STATE_TRICK3 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(rad);
				vecTargetPos.y = vecTargetPos.y - 1.5f;
			}
			if( m_curState == SPRITE_STATE_TRICK4 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(-rad);
				vecTargetPos.y = vecTargetPos.y - 2.6f + h;
			}
			else if( m_curState == SPRITE_STATE_TRICK5 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(-rad);
				vecTargetPos.y = vecTargetPos.y - h;
			}
			else if( m_curState == SPRITE_STATE_TRICK6 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(-rad);
				vecTargetPos.y = vecTargetPos.y - 1.5f;
			}
		}
	}

	// move according to current states.
	vecDestDir = vecTargetPos - m_vecPos;
	float vDis = vecDestDir.Normalize();

	if( m_curState == SPRITE_STATE_CHASE || m_curState == SPRITE_STATE_IDLE )
	{
		if( vDis > 20.0f )
		{
			// maybe transported
			m_vecPos = vecTargetPos - vecDestDir * 10.0f;
			m_vSpeed = 0.0f;
		}
		else if( vDis > 5.0f )
		{
			// accelerate
			if( m_vSpeed < m_vSpeedMax )
				m_vSpeed += f * m_vAcceleration;
		}
		else if( vDis > 0.3f || m_vSpeed > 0.5f )
		{
			// decelerate
			if( m_vSpeed > 0.0f )
				m_vSpeed -= f * m_vDeceleration;
			if( m_vSpeed < vDis )
				m_vSpeed = vDis;
		}
		else if( vDis > 0.05f )
		{
			// just begin stop here and turn direction to the same as player
			m_vSpeed = 0.0f;
			m_vecPos = m_vecPos + vecDestDir * 0.3f * f;
			vecDestDir = m_pPlayer->GetDir();
			if( m_curState != SPRITE_STATE_IDLE )
			{
				m_curState = SPRITE_STATE_IDLE;
				m_dwStateTicks = 0;
				m_dwIdleAllowed = 1000 + (rand() % 10000);
			}
		}
	}
	else if( m_curState >= SPRITE_STATE_TRICK1 && m_curState <= SPRITE_STATE_TRICK6 )
	{
		if( vDis > 20.0f )
		{
			m_curState = SPRITE_STATE_CHASE;
			m_dwStateTicks = 0;
		}
		else if( vDis > 0.5f )
		{
			// accelerate
			if( m_vSpeed < vDis )
				m_vSpeed = vDis;
		}
		else if( vDis > 0.3f )
		{
			// decelerate
			if( m_vSpeed > 0.0f )
				m_vSpeed -= f * m_vDeceleration;
			if( m_vSpeed < vDis )
				m_vSpeed = vDis;
		}
		else if( vDis > 0.05f )
		{
			// just begin stop here and turn direction to the same as player
			m_vSpeed = 0.0f;
			m_vecPos = m_vecPos + vecDestDir * 0.3f * f;
			vecDestDir = m_pPlayer->GetDir();
			m_curState = SPRITE_STATE_IDLE;
			m_dwStateTicks = 0;
			m_dwIdleAllowed = 1000 + (rand() % 10000);
		}
	}

	m_vecDir = Normalize(m_vecDir + vecDestDir * (vDis + 1.0f) * 0.1f);
	m_vecPos = m_vecPos + m_vSpeed * f * m_vecDir;
	
	A3DMATRIX4 matOrient;
	m_pModelSprite->SetPos(m_vecPos);
	A3DVECTOR3 vecDirH = m_vecDir;
	vecDirH.y = 0.0f;
	vecDirH.Normalize();
	m_pModelSprite->SetDirAndUp(vecDirH, A3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pModelSprite->Tick(dwDeltaTime);

	return true;
}

//	Render routine
bool CECSprite::Render(CECViewport* pViewport)
{
	if( m_pModelSprite )
		m_pModelSprite->Render(pViewport->GetA3DViewport());

	return true;
}

bool CECSprite::SetPos(const A3DVECTOR3& vecPos)
{
	m_vecPos = vecPos;
	return true;
}

void CECSprite::SetTransparent(float fTransparent)
{
	if( m_pModelSprite )
		m_pModelSprite->SetTransparent(fTransparent);
}
