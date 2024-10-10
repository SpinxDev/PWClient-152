#include <A3DTypes.h>
#include <A3DBone.h>
#include <A3DSkinModel.h>
#include <A3DSkinMan.h>
#include <A3dEngine.h>
#include <a3dskeleton.h>
#include <A3DFuncs.h>

#include "EC_Goblin.h"
#include "EC_Model.h"
#include "EC_Player.h"
#include "EC_Viewport.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_Utility.h"
#include "EC_IvtrGoblin.h"

#include "EC_Inventory.h"
#include "EC_Skill.h"
#include "elementdataman.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_NPC.h"
#include "EC_Resource.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGoblin
//	
///////////////////////////////////////////////////////////////////////////
CECGoblin::CECGoblin()
{
	m_tid			= 0;
	m_pPlayer		= NULL;
	m_pCurSkill		= NULL;
	m_pModel		= NULL;

	m_iRefineLvl	= 0;
	m_bRefineActive	= true;
	
	m_idModel		= GOBLIN_MODEL1;
	m_curState		= GOBLIN_STATE_NULL;

	m_dwStateTicks	= 0;
	m_dwIdleAllowed	= 0;

	m_pDBEssence	= NULL;
}

CECGoblin::~CECGoblin()
{
	Release();
}
//	Initialize object
bool CECGoblin::Init(int tid, int idModel, int iRefineLvl, CECPlayer* pPlayer)
{
	m_tid			= tid;
	m_iRefineLvl	= iRefineLvl;
	m_pPlayer		= pPlayer;

	if( pPlayer->IsGoblinRefineActive())
		m_bRefineActive = true;
	else
		m_bRefineActive = false;

	const char* szModelFile = NULL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	const void* pDBData = pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	if (!pDBData)
	{
		ASSERT(pDBData);
		return false;
	}

	GOBLIN_MODEL_ID oldModel = m_idModel;
	m_pDBEssence = (GOBLIN_ESSENCE*)pDBData;
	
	if(idModel == 1)
	{
		m_idModel = CECGoblin::GOBLIN_MODEL1;		// 显示1档模型
		szModelFile = m_pDBEssence->file_model1;
	}
	else if(idModel == 2)
	{
		m_idModel = CECGoblin::GOBLIN_MODEL2;		// 显示2档模型
		szModelFile = m_pDBEssence->file_model2;
	}	
	else if(idModel == 3)
	{
		m_idModel = CECGoblin::GOBLIN_MODEL3;		// 显示3档模型
		szModelFile = m_pDBEssence->file_model3;
	}	
	else if(idModel == 4)
	{
		m_idModel = CECGoblin::GOBLIN_MODEL4;		// 显示4档模型
		szModelFile = m_pDBEssence->file_model4;
	}
	else 
	{
		a_LogOutput(1, "CECGoblin::Init(), total status point range error!(idModel=%d)", idModel);
		return false;
	}

	// create goblin model
//	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("armor.sdr", A3DSkinMan::SHADERREPLACE_REFLECTPREFIX);
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile((const char*)glb_ArmorReplaceShader_ReflectPrefix, A3DSkinMan::SHADERREPLACE_USERDEFINE);

	// if the model already exists
	if( m_pModel)
	{
		if(oldModel != m_idModel)
		{
			m_pModel->Release();
			delete m_pModel;
			m_pModel = NULL;

			m_pModel = new CECModel();

			if( !m_pModel->Load(szModelFile) )
			{
				delete m_pModel;
				m_pModel = NULL;
				g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
				a_LogOutput(1, "CECGoblin::Init()1, failed to load ecmodel!");
				return false;
			}

			// initialize some parameters.
			m_vSpeed		= 0.0f;
			m_vSpeedMax		= 20.0f;
			m_vAcceleration = 15.0f;
			m_vDeceleration = 15.0f;

			m_vecDir		= A3DVECTOR3(0.0f, 0.0f, 1.0f);
			m_curState		= GOBLIN_STATE_IDLE;
			m_dwStateTicks	= 0;
			m_dwIdleAllowed = 2000;
			m_pModel->PlayActionByName("行走", 1.0f);
			m_pModel->SetTransparent(1.0f);
		}
	}
	else
	{
		m_pModel = new CECModel();
		if( !m_pModel->Load(szModelFile) )
		{
			delete m_pModel;
			m_pModel = NULL;
			g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);
			a_LogOutput(1, "CECGoblin::Init()2, failed to load ecmodel!");
			return false;
		}
		
		// initialize some parameters.
		m_vSpeed		= 0.0f;
		m_vSpeedMax		= 20.0f;
		m_vAcceleration = 15.0f;
		m_vDeceleration = 15.0f;

		m_vecDir		= A3DVECTOR3(0.0f, 0.0f, 1.0f);
		m_curState		= GOBLIN_STATE_IDLE;
		m_dwStateTicks	= 0;
		m_dwIdleAllowed = 2000;
		m_pModel->PlayActionByName("行走", 1.0f);
		m_pModel->SetTransparent(0.1f);
	}

	char szSkin[MAX_PATH];
	strncpy(szSkin, szModelFile, MAX_PATH);
	glb_ChangeExtension(szSkin, "ski");
	A3DSkin * pSkin = g_pGame->LoadA3DSkin(szSkin, false);
	if( pSkin )
	{
		m_pModel->GetA3DSkinModel()->AddSkin(pSkin, true);
	}
	
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetReplaceShaderFile("", 0);

	m_pPlayer		= pPlayer;

	if(m_bRefineActive)
		PlayRefineActiveEffect();			

	return true;
}

bool CECGoblin::Init(int tid, CECIvtrGoblin* pIvtrGoblin, CECPlayer* pPlayer)
{
	if(!pIvtrGoblin)
		return false;

	// 确定使用哪一档模型
	int iModelLvl = pIvtrGoblin->GetModelLevel();

	return Init(tid, iModelLvl, pIvtrGoblin->GetEssence().data.refine_level, pPlayer);
}
//  小精灵的名字
const wchar_t* CECGoblin::GetName()
{ 
	return m_pDBEssence->name; 
}

//	Release object
void CECGoblin::Release()
{
	if( m_pModel )
	{
		m_pModel->Release();
		delete m_pModel;
		m_pModel = NULL;
	}
	
	//	Release current skill if it exists
	if( m_pCurSkill )
	{
		delete m_pCurSkill;
		m_pCurSkill = NULL;
	}
}

//	Tick routine
bool CECGoblin::Tick(DWORD dwDeltaTime)
{
	A3DVECTOR3 vecTargetPos;

	if( !m_pPlayer || !m_pPlayer->GetPlayerModel() || !m_pModel )
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
		if( m_curState != GOBLIN_STATE_CHASE )
		{
			m_curState = GOBLIN_STATE_CHASE;
			m_dwStateTicks = 0;
		}
	}
	else if( m_pPlayer->IsFighting())
	{
		if( m_curState != GOBLIN_STATE_IDLE && m_curState != GOBLIN_STATE_ATTACK)
		{
			m_curState = GOBLIN_STATE_IDLE;
			m_dwStateTicks = 0;
		}
//		vecTargetPos = vecTargetPos + m_pPlayer->GetDir() * 3.5f;
	}
	else if( m_curState == GOBLIN_STATE_IDLE )
	{
		if( m_dwStateTicks > m_dwIdleAllowed )
		{
			m_curState = (GOBLIN_STATE) (GOBLIN_STATE_TRICK1 + (rand() % 6));
			m_dwStateTicks = 0;
		}
	}
// 	else if( m_curState == GOBLIN_STATE_ATTACK )
// 	{
// 		if(m_pCurSkill != NULL)
// 		{
// 			m_pModel->PlayActionByName("攻击",1.0f);
// 
// 			delete m_pCurSkill;
// 			m_pCurSkill = NULL;
// 		}
// 		if(m_SkillCnt.IncCounter(dwDeltaTime))
// 		{
// 			m_SkillCnt.Reset(true);
// 			m_curState = GOBLIN_STATE_IDLE;
// 			m_pModel->PlayActionByName("行走", 1.0f);
// 		}
// 
//	}
	else if( m_curState >= GOBLIN_STATE_TRICK1 && m_curState <= GOBLIN_STATE_TRICK6 )
	{
		if( m_dwStateTicks > 5000 )
		{
			m_curState = GOBLIN_STATE_CHASE;
			m_dwStateTicks = 0;
		}
		else
		{
			float r		= 0.5f + m_dwStateTicks * 0.0002f;
			float rad	= m_dwStateTicks * 0.002f;
			float h		= m_dwStateTicks * 0.0005f;
			if( m_curState == GOBLIN_STATE_TRICK1 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(rad);
				vecTargetPos.y = vecTargetPos.y - 2.6f + h;
			}
			else if( m_curState == GOBLIN_STATE_TRICK2 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(rad);
				vecTargetPos.y = vecTargetPos.y - h;
			}
			else if( m_curState == GOBLIN_STATE_TRICK3 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(rad);
				vecTargetPos.y = vecTargetPos.y - 1.5f;
			}
			if( m_curState == GOBLIN_STATE_TRICK4 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(-rad);
				vecTargetPos.y = vecTargetPos.y - 2.6f + h;
			}
			else if( m_curState == GOBLIN_STATE_TRICK5 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(-rad);
				vecTargetPos.y = vecTargetPos.y - h;
			}
			else if( m_curState == GOBLIN_STATE_TRICK6 )
			{
				vecTargetPos = m_pPlayer->GetPos() + A3DVECTOR3(0.0f, 2.4f, 0.0f) + A3DVECTOR3(-1.5f, 0.0f, 0.0f) * RotateY(-rad);
				vecTargetPos.y = vecTargetPos.y - 1.5f;
			}
		}
	}

	// move according to current states.
	vecDestDir = vecTargetPos - m_vecPos;
	float vDis = vecDestDir.Normalize();

	if( m_curState == GOBLIN_STATE_CHASE || m_curState == GOBLIN_STATE_IDLE )
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
			if( m_curState != GOBLIN_STATE_IDLE )
			{
				m_curState = GOBLIN_STATE_IDLE;
				m_dwStateTicks = 0;
				m_dwIdleAllowed = 1000 + (rand() % 10000);
			}
		}
	}
	else if( m_curState >= GOBLIN_STATE_TRICK1 && m_curState <= GOBLIN_STATE_TRICK6 )
	{
		if( vDis > 20.0f )
		{
			m_curState = GOBLIN_STATE_CHASE;
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
			m_curState = GOBLIN_STATE_IDLE;
			m_dwStateTicks = 0;
			m_dwIdleAllowed = 1000 + (rand() % 10000);
		}
	}

	m_vecDir = Normalize(m_vecDir + vecDestDir * (vDis + 1.0f) * 0.1f);
	m_vecPos = m_vecPos + m_vSpeed * f * m_vecDir;
	
	A3DMATRIX4 matOrient;
	m_pModel->SetPos(m_vecPos);
	A3DVECTOR3 vecDirH = m_vecDir;
	vecDirH.y = 0.0f;
	vecDirH.Normalize();
	m_pModel->SetDirAndUp(vecDirH, A3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pModel->Tick(dwDeltaTime);
	
	return true;
}
//	Render routine
bool CECGoblin::Render(CECViewport* pViewport)
{
	if( m_pModel )
	{	
		m_pModel->Render(pViewport->GetA3DViewport());
	}

	return true;
}
//	Process message
bool CECGoblin::ProcessMessage(const ECMSG& Msg)
{
	using namespace S2C;
	
	switch (Msg.dwParam2)
	{
	case ELF_REFINE_ACTIVATE:	OnMsgGoblinRefineActive(Msg);	break;
	case CAST_ELF_SKILL:		OnMsgGoblinCastSkill(Msg);		break;
	default:
		return false;
 	}

	return true;
}

bool CECGoblin::SetPos(const A3DVECTOR3& vecPos)
{
	m_vecPos = vecPos;
	return true;
}

void CECGoblin::SetTransparent(float fTransparent)
{
	if( m_pModel )
		m_pModel->SetTransparent(fTransparent);
}

void CECGoblin::OnMsgGoblinRefineActive(const ECMSG &Msg)
{
	using namespace S2C;

	cmd_elf_refine_activate* pCmd = (cmd_elf_refine_activate*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->pid == m_pPlayer->GetCharacterID());
	
	m_bRefineActive = (pCmd->status==1) ? true : false;

	PlayRefineActiveEffect();
}
// Play refine active effect
void CECGoblin::PlayRefineActiveEffect()
{
	// for test
	if(m_bRefineActive)
	{
		if(m_iRefineLvl >= 6)
			m_pModel->PlayGfx("程序联入\\魔光球2.gfx", NULL, 1.0f);
		else
			m_pModel->PlayGfx("程序联入\\仙光球.gfx", NULL, 1.0f);
	}
	else
	{
		if(m_iRefineLvl >= 6)
			m_pModel->RemoveGfx("程序联入\\魔光球2.gfx", NULL);
		else
			m_pModel->RemoveGfx("程序联入\\仙光球.gfx", NULL);
	}
}
void CECGoblin::OnMsgGoblinCastSkill(const ECMSG &Msg)
{
	using namespace S2C;
	cmd_cast_elf_skill* pCmd = (cmd_cast_elf_skill*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->pid == m_pPlayer->GetCharacterID());
	
	if( !m_pModel )
		return;
	
	m_SkillCnt.SetPeriod(500);
	m_SkillCnt.SetCounter(0);
	
	//	Face to target
	A3DVECTOR3 vTarget;
	CECObject* pObject = g_pGame->GetGameRun()->GetWorld()->GetObject(pCmd->target, 0);
	if (!pObject)
		return;

	if (ISNPCID(pCmd->target))
	{
		CECNPC* pNPC = (CECNPC*)pObject;
		vTarget = pNPC->GetPos();
	}
	else if (ISPLAYERID(pCmd->target))
	{
		CECPlayer* pPlayer = (CECPlayer*)pObject;
		vTarget = pPlayer->GetPos();
	}
	else
	{
		return;
	}

	A3DVECTOR3 vDir = vTarget - m_pPlayer->GetPos();
	vDir.y = 0.0f;
	vDir.Normalize();

	m_pModel->SetDirAndUp(vDir, A3DVECTOR3(0.0f,1.0f,0.0f));

	m_pModel->PlayActionByName("攻击",1.0f);
	m_pModel->QueueAction("行走",300);
}

void CECGoblin::LevelUp()
{
	if (m_pModel)
		m_pModel->PlayGfx(res_GFXFile(RES_GFX_GOBLIN_LEVELUP), NULL);
}