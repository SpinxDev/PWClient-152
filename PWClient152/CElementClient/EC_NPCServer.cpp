/*
 * FILE: EC_NPCServer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_NPCServer.h"
#include "EC_Game.h"
#include "EC_Model.h"
#include "EC_Utility.h"
#include "EC_Skill.h"
#include "EC_PateText.h"
#include "EC_ImageRes.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_TaskInterface.h"
#include "EC_HostPlayer.h"
#include "EC_Viewport.h"
#include "EC_SceneLoader.h"
#include "EC_Inventory.h"
#include "EC_NPCModel.h"

#include "TaskTempl.h"
#include "TaskTemplMan.h"
#include "elementdataman.h"

#include "A3DViewport.h"
#include <A3DMacros.h>

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
//	Implement CECNPCServer
//	
///////////////////////////////////////////////////////////////////////////

CECNPCServer::CECNPCServer(CECNPCMan* pNPCMan) : CECNPC(pNPCMan)
{
	m_iCID			= OCID_SERVER;
	m_pDBEssence	= NULL;
	m_pMonEssence	= NULL;
	m_fTaxRate		= 0.05f;
	m_bQuestIcon	= false;
	m_iQuestIcon	= QI_NONE;
	m_TaskCounter.SetPeriod(1000);
}

CECNPCServer::~CECNPCServer()
{
}

//	Initlaize object
bool CECNPCServer::Init(int tid, const S2C::info_npc& Info)
{
	if (!CECNPC::Init(tid, Info))
		return false;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (NPC_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	//	Some server NPC data stored in another place, get it.
	m_pMonEssence = (MONSTER_ESSENCE*)pDB->get_data_ptr(m_pDBEssence->id_src_monster, ID_SPACE_ESSENCE, DataType);
	if (!m_pMonEssence)
	{
		if (!(m_pMonEssence = (MONSTER_ESSENCE*)pDB->get_data_ptr(4249, ID_SPACE_ESSENCE, DataType)))
		{
			a_LogOutput(1, "CECNPCServer::Init, server NPC reference to null monster data");
			return false;
		}
	}
	
	SetUseGroundNormal(m_pMonEssence->stand_mode == 0 ? true : false);
	m_fTouchRad	= m_pMonEssence->size;
	m_BasicProps.iLevel = m_pMonEssence->level;

	//	Submit EC model loading request to loading thread
	QueueLoadNPCModel();

	m_cdr.vExts = m_pNPCModelPolicy->GetDefaultPickAABB().Extents;

	//	If NPC doesn't have specific name, use the name in database
	if (!(Info.state & GP_STATE_NPC_NAME))
	{
		m_strName = m_pDBEssence->name;
		if (m_pPateName){
			m_pPateName->SetText(m_strName, false);
			m_pPateName->SetBarImage("图标\\npc悬浮框.tga");
		}
	}

	A3DVECTOR3 vPos = Info.pos;
	SetPos(vPos);
	SetDirAndUp(glb_DecompressDirH(Info.dir), g_vAxisY);

	StartWork(WT_NOTHING, WORK_STAND);

	return true;
}

//	Release object
void CECNPCServer::Release()
{
	//	Release skill list
	ReleaseAllSkills();

	//	Release pet skill list
	ReleaseAllPetSkills();

	CECNPC::Release();
}

//	Release all skills
void CECNPCServer::ReleaseAllSkills()
{
	//	Release skill list
	for (int i=0; i < m_aSkills.GetSize(); i++)
		delete m_aSkills[i];
	
	m_aSkills.RemoveAll();
}

//	Release all pet skills
void CECNPCServer::ReleaseAllPetSkills()
{
	//	Release pet skill list
	for (int i=0; i < m_aPetSkills.GetSize(); i++)
		delete m_aPetSkills[i];

	m_aPetSkills.RemoveAll();
}

//	Build skill list
bool CECNPCServer::BuildSkillList(int idSkillSevice)
{
	ASSERT(m_pDBEssence);

	if (!idSkillSevice)
		return true;

	ASSERT(m_pDBEssence->id_skill_service == (DWORD)idSkillSevice || 
		m_pDBEssence->id_goblin_skill_service == (DWORD)idSkillSevice);

	ReleaseAllSkills();

	DATA_TYPE DataType;
	NPC_SKILL_SERVICE* pSevData = (NPC_SKILL_SERVICE*)g_pGame->GetElementDataMan()->get_data_ptr(idSkillSevice, ID_SPACE_ESSENCE, DataType);
	if (!pSevData)
		return true;

	for (int i=0; i < 128; i++)
	{
		if (!pSevData->id_skills[i])
			break;

		CECSkill* pSkill = new CECSkill(pSevData->id_skills[i], 1);
		m_aSkills.Add(pSkill);
	}

	return true;
}

//	Build pet skill list
bool CECNPCServer::BuildPetSkillList()
{
	ASSERT(m_pDBEssence);

	if (!m_pDBEssence->id_petlearnskill_service || m_aPetSkills.GetSize())
		return true;

	DATA_TYPE DataType;
	NPC_PETLEARNSKILL_SERVICE* pSevData = (NPC_PETLEARNSKILL_SERVICE*)g_pGame->GetElementDataMan()->get_data_ptr(m_pDBEssence->id_petlearnskill_service, ID_SPACE_ESSENCE, DataType);
	if (!pSevData)
		return true;

	for (int i=0; i < 128; i++)
	{
		if (!pSevData->id_skills[i])
			break;

		CECSkill* pSkill = new CECSkill(pSevData->id_skills[i], 1);
		m_aPetSkills.Add(pSkill);
	}

	return true;
}

//	Tick routine
bool CECNPCServer::Tick(DWORD dwDeltaTime)
{
	CECNPC::Tick(dwDeltaTime);

	if (m_TaskCounter.IncCounter(dwDeltaTime))
	{
		m_TaskCounter.Reset();

		//	Update quest icon
		UpdateCurQuestIcon();
	}

	return true;
}

//	Render routine
bool CECNPCServer::Render(CECViewport* pViewport, int iRenderFlag/* 0 */)
{
	CECNPC::Render(pViewport, iRenderFlag);

	if (iRenderFlag == RD_NORMAL)
	{
		//	Render quest icon on head
		if (m_bQuestIcon && HasModel())
			RenderQuestIcon(pViewport);
	}

	return true;
}

bool CECNPCServer::RenderHighLight(CECViewport* pViewport)
{
	CECNPC::RenderHighLight(pViewport);

	//	Render quest icon on head
	if (m_bQuestIcon && HasModel())
		RenderQuestIcon(pViewport);

	return true;
}

//	Render quest icon on header
bool CECNPCServer::RenderQuestIcon(CECViewport* pViewport)
{
	if (m_iQuestIcon < 0)
		return true;

	if (!FillPateContent(pViewport))
		return true;

	int iIcon = m_iQuestIcon;
	DWORD dwCol = A3DCOLORRGB(255, 255, 255);	//	Force to use image color

	CECImageRes* pImageRes = g_pGame->GetImageRes();
	int x, y=m_PateContent.iCurY, cx, cy;

	pImageRes->GetImageItemSize(CECImageRes::IMG_PATEQUEST, iIcon, &cx, &cy);
	x = m_PateContent.iBaseX - (cx >> 1);
	y -= cy + 2;
	pImageRes->DrawImage(CECImageRes::IMG_PATEQUEST, x, y, dwCol, iIcon, m_PateContent.z);

	m_PateContent.iCurY = y;

	return true;
}

// Update current qeust icon index
void CECNPCServer::UpdateCurQuestIcon()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost || !pHost->GetTaskInterface())
		return;

	ATaskTemplMan* pTempMan = g_pGame->GetTaskTemplateMan();
	elementdataman* pDataMan = g_pGame->GetElementDataMan();
	CECTaskInterface* pTask = pHost->GetTaskInterface();

	enum
	{
		TASK_HAVE_COMPLETE = 0x10000000,

		TASK_COMPLETE	= 0x01,		// Has task which is completed
		TASK_INCOMPLETE	= 0x02,		// Has task which isn't completed
		TASK_CANGET		= 0x04,		// Has task can be checked out
		TASK_CANNOTGET	= 0x08,		// Has task can't be checked out

		TASK_COMPLETE_TYPE1	= 0x10,
		TASK_CANGET_TYPE1	= 0x20,
		TASK_COMPLETE_TYPE2	= 0x40,
		TASK_CANGET_TYPE2	= 0x80,
		TASK_COMPLETE_TYPE3	= 0x100,
		TASK_CANGET_TYPE3	= 0x200,
		TASK_COMPLETE_TYPE4	= 0x400,
		TASK_CANGET_TYPE4	= 0x800,
	};

	m_iQuestIcon = QI_NONE;

	DATA_TYPE DataType;
	int i, iFlags = 0;

	//	Check in tasks at first
	if (m_pDBEssence->id_task_in_service)
	{
		const void* pData = pDataMan->get_data_ptr(m_pDBEssence->id_task_in_service, ID_SPACE_ESSENCE, DataType);
		if (!pData || DataType != DT_NPC_TASK_IN_SERVICE)
		{
			ASSERT(pData);
			return;
		}

		NPC_TASK_IN_SERVICE* aInServices = (NPC_TASK_IN_SERVICE*)pData;
		for (i=0; i < SIZE_OF_ARRAY(aInServices->id_tasks); i++)
		{
			int idTask = aInServices->id_tasks[i];
			if (idTask <= 0 || !pTask->HasTask(idTask))
				continue;

			ATaskTempl* pTaskTemp = pTempMan->GetTaskTemplByID(idTask);

			if (pTask->CanFinishTask(idTask))
			{
				iFlags |= TASK_HAVE_COMPLETE;

				if (pTaskTemp->IsKeyTask())
				{
					m_iQuestIcon = QI_IN_K;
					return;
				}
				else if(pTaskTemp->GetType() == enumTTEvent || 
					pTaskTemp->GetType() == enumTTDaily) 
					iFlags |= TASK_COMPLETE_TYPE3;
				else if(pTaskTemp->GetType() ==  enumTTQiShaList)
					iFlags |= TASK_COMPLETE_TYPE2;
				else if (pTaskTemp->GetType() == enumTTFaction ||
					pTaskTemp->GetType() == enumTTFunction)
					iFlags |= TASK_COMPLETE_TYPE1;
				else if (pTaskTemp->GetType() == enumTTMajor)
					iFlags |= TASK_COMPLETE_TYPE4;
				else
					iFlags |= TASK_COMPLETE;
			}
			else
			{
				iFlags |= TASK_INCOMPLETE;
			}
		}
	}
	
	// set icon by flag and specific priority
	if (iFlags & TASK_HAVE_COMPLETE)
	{
		if (iFlags & TASK_COMPLETE_TYPE4)
			m_iQuestIcon = QI_IN_TYPE4;
		else if (iFlags & TASK_COMPLETE_TYPE3)
			m_iQuestIcon = QI_IN_TYPE3;
		else if (iFlags & TASK_COMPLETE_TYPE2)
			m_iQuestIcon = QI_IN_TYPE2;
		else if (iFlags & TASK_COMPLETE)
			m_iQuestIcon = QI_IN;
		else if (iFlags & TASK_COMPLETE_TYPE1)
			m_iQuestIcon = QI_IN_TYPE1;

		return;
	}

	//	Then Check out tasks
	if (m_pDBEssence->id_task_out_service)
	{
		const void* pData = pDataMan->get_data_ptr(m_pDBEssence->id_task_out_service, ID_SPACE_ESSENCE, DataType);
		if (!pData || DataType != DT_NPC_TASK_OUT_SERVICE)
		{
			ASSERT(pData);
			return;
		}

		NPC_TASK_OUT_SERVICE* aOutServices = (NPC_TASK_OUT_SERVICE*)pData;
		
		//	酒馆任务对图标的修改
		//	是否持有开启物品	是否显示有可接任务	是否还有刷新次数	图标状态
		//	无					-					-					无
		//	有					有					-					紫色令牌
		//	有					无					有					银色令牌
		//	有					无					无					无
		if( aOutServices->storage_id != 0 && aOutServices->storage_open_item != 0 )
		{
			if(pHost->GetPack()->GetItemTotalNum(aOutServices->storage_open_item) > 0 )
			{
				const unsigned short* pTasks = pTask->GetStorageTasks(aOutServices->storage_id);
				for(int i=0;i<TASK_STORAGE_LEN;i++)
				{
					const unsigned long idTask = *(pTasks++);
					if(idTask > 0)
					{
						iFlags |= TASK_CANGET_TYPE2;
					}
				}
				
				StorageTaskList* pStorage = static_cast<StorageTaskList*>(pTask->GetStorageTaskList());
				int refreshCount = pStorage->m_StoragesRefreshCount[aOutServices->storage_id - 1];
				int refreshPerDay = GetTaskTemplMan()->GetStorageRefreshPerDay(aOutServices->storage_id);
				if(refreshPerDay > refreshCount )
				{
					iFlags |= TASK_CANNOTGET;
				}
			}
		}
		else
		{
			//	普通任务发放图标
			for (i=0; i < SIZE_OF_ARRAY(aOutServices->id_tasks); i++)
			{
				int idTask = aOutServices->id_tasks[i];
				if (idTask <= 0 || !pTask->CanShowTask(idTask))
					continue;
				
				ATaskTempl* pTaskTemp = pTempMan->GetTaskTemplByID(idTask);
				
				if (!pTask->CanDeliverTask(idTask))
				{
					if (pTaskTemp->IsKeyTask())
					{
						m_iQuestIcon = QI_OUT_K;
						return;
					}
					else if(pTaskTemp->GetType() == enumTTEvent || 
						pTaskTemp->GetType() == enumTTDaily) 
						iFlags |= TASK_CANGET_TYPE3;
					else if(pTaskTemp->GetType() ==  enumTTQiShaList)
						iFlags |= TASK_CANGET_TYPE2;
					else if (pTaskTemp->GetType() == enumTTFaction ||
						pTaskTemp->GetType() == enumTTFunction)
						iFlags |= TASK_CANGET_TYPE1;
					else if (pTaskTemp->GetType() == enumTTMajor)
						iFlags |= TASK_CANGET_TYPE4;
					else
						iFlags |= TASK_CANGET;
				}
				else
					iFlags |= TASK_CANNOTGET;
			}
		}
	}

	// set icon by flag and specific priority
	if (iFlags & TASK_CANGET_TYPE4)
		m_iQuestIcon = QI_OUT_TYPE4;
	else if (iFlags & TASK_CANGET_TYPE3)
		m_iQuestIcon = QI_OUT_TYPE3;
	else if (iFlags & TASK_CANGET_TYPE2)
		m_iQuestIcon = QI_OUT_TYPE2;
	else if (iFlags & TASK_CANGET)
		m_iQuestIcon = QI_OUT;
	else if (iFlags & TASK_CANGET_TYPE1)
		m_iQuestIcon = QI_OUT_TYPE1;

	else if (iFlags & TASK_INCOMPLETE)
		m_iQuestIcon = QI_IN_N;
	else if (iFlags & TASK_CANNOTGET)
		m_iQuestIcon = QI_OUT_N;
}

//	Get way point ID bound with this NPC
DWORD CECNPCServer::GetWayPointID()
{
	DWORD dwID = 0;
	if (m_pDBEssence->combined_services & 0x08)
		dwID = m_pDBEssence->id_to_discover;

	return dwID;
}

//	Get item price scale factor
float CECNPCServer::GetPriceScale()
{
	return 1.0f + m_pDBEssence->tax_rate;
}

//	Get NPC name color
DWORD CECNPCServer::GetNameColor()
{
	DWORD dwNameCol = A3DCOLORRGB(255, 255, 0);
	if (IsInBattleInvaderCamp())
	{
		//	In invader camp in battle
		dwNameCol = NAMECOL_BC_RED;
	}
	else if (IsInBattleDefenderCamp())
	{
		//	In defender camp in battle
		dwNameCol = NAMECOL_BC_BLUE;
	}

	return dwNameCol;
}

//	Is monster in invader camp in battle ?
bool CECNPCServer::IsInBattleInvaderCamp()
{ 
	return (m_pMonEssence && (m_pMonEssence->faction & (1 << 11))) ? true : false; 
}

//	Is monster in defender camp in battle ?
bool CECNPCServer::IsInBattleDefenderCamp()
{
	return (m_pMonEssence && (m_pMonEssence->faction & (1 << 12))) ? true : false; 
}

//	Get role in battle
int CECNPCServer::GetRoleInBattle()
{
	return m_pMonEssence ? m_pMonEssence->role_in_war : 0;
}

bool CECNPCServer::IsImmuneDisable()
{
	if( m_pMonEssence && (m_pMonEssence->combined_switch & MCS_HIDE_IMMUNE) )
		return true;
	
	return false;
}