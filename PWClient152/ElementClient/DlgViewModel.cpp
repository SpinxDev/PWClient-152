// Filename	: DlgViewModel.cpp
// Creator	: Xu Wenbin
// Date		: 2013/2/21

#include "AFI.h"
#include "DlgViewModel.h"
#include "EC_Game.h"
#include "EC_Model.h"
#include "EC_NPC.h"
#include "EC_Player.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrPetItem.h"
#include "EC_IvtrFlySword.h"
#include "ExpTypes.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include <A3DGFXEx.h>
#include <A3DGFXElement.h>
#include <A3DCombinedAction.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgViewModel, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

CDlgViewModel::CDlgViewModel()
: m_pModel(NULL)
, m_nAngle(0)
, m_nMove(0)
, m_aabb(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0))
, m_bAABBDirty(false)
, m_pImg_Model(NULL)
, m_pBtn_LeftTurn(NULL)
, m_pBtn_RightTurn(NULL)
, m_pBtn_MoveNear(NULL)
, m_pBtn_MoveFar(NULL)
{
}

CDlgViewModel::~CDlgViewModel()
{
	A3DRELEASE(m_pModel);
}

void CDlgViewModel::Reset()
{
	A3DRELEASE(m_pModel);
	m_strAct.Empty();
	m_nAngle = 0;
	m_nMove = 0;
	m_bAABBDirty = true;
	UpdateAABB();
	UpdateRenderCallback();
}

void CDlgViewModel::UpdateRenderCallback()
{	
	if (m_pImg_Model){
		DWORD dwCameraParam = MakeNPCRenderCameraParam(m_nAngle, m_nMove);
		m_pImg_Model->SetRenderCallback(NPCRenderDemonstration, (DWORD)m_pModel, dwCameraParam, (DWORD)&m_aabb);
	}
}

bool CDlgViewModel::OnInitDialog()
{
	DDX_Control("Img_Model", m_pImg_Model);
	DDX_Control("Btn_LeftTurn", m_pBtn_LeftTurn);
	DDX_Control("Btn_RightTurn", m_pBtn_RightTurn);
	DDX_Control("Btn_MoveNear",	m_pBtn_MoveNear);
	DDX_Control("Btn_MoveFar", m_pBtn_MoveFar);
	return true;
}

void CDlgViewModel::OnShowDialog()
{
	m_nAngle = 0;
	m_nMove = 0;
}

void CDlgViewModel::OnTick()
{
	if (m_pBtn_LeftTurn->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK){
		m_nAngle = (m_nAngle + 360 - GetGame()->GetTickTime() / 6 ) % 360;
	}
	else if (m_pBtn_RightTurn->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK){
		m_nAngle = (m_nAngle + GetGame()->GetTickTime() / 6) % 360;
	}	
	if (m_pBtn_MoveNear->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK){
		m_nMove -= 5;
		a_ClampFloor(m_nMove, -100);
	}
	else if (m_pBtn_MoveFar->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK){
		m_nMove += 5;
		a_ClampRoof(m_nMove, 100);
	}
	if (m_pModel){
		if (!m_strAct.IsEmpty()){
			m_pModel->PlayActionByName(m_strAct, 1.0f, false);
		}
		m_pModel->Tick(GetGame()->GetTickTime());
		UpdateAABB();
	}
	UpdateRenderCallback();
}

void CDlgViewModel::OnCommand_CANCEL(const char * szCommand)
{
	Reset();
	Show(false);
}

void CDlgViewModel::SetModel(CECModel *pModel, const char *szActToPlay/* =NULL */)
{
	if (pModel == m_pModel){
		m_strAct = szActToPlay;
		m_bAABBDirty = true;
		return;
	}
	Reset();
	if (!pModel){
		return;
	}

	m_pModel = pModel;
	m_strAct = szActToPlay;
	m_nAngle = 0;
	m_nMove = 0;
	m_bAABBDirty = true;
}

void CDlgViewModel::UpdateAABB()
{
	if (!m_bAABBDirty){
		return;
	}
	m_bAABBDirty = false;

	if (!m_pModel){
		m_aabb = A3DAABB(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0));
		return;
	}

	//	获取模型包围盒
	m_aabb = m_pModel->GetModelAABB();
	
 	float fModelExtend = m_aabb.Extents.Magnitude();
	if (fModelExtend < 0.2f){
		//	获取特效包围盒
		//	汐族飞行器等用主要用特效实现，无法从 pModel 直接获取有效 AABB
		//	直接 Merge pGfx->GetAABB() 有问题，改为从 GFX 大小来扩展模型AABB
		float fGFXExtent = 0.0f;
		for (CoGfxMap::iterator it = m_pModel->GetCoGfxMap().begin(); it != m_pModel->GetCoGfxMap().end(); ++ it)
		{
			GFX_INFO* pInfo = it->second;
			A3DGFXEx* pGfx = pInfo->GetGfx();
			if (!pGfx || pGfx->GetState() == ST_STOP || !pGfx->IsVisible())
				continue;
			fGFXExtent = a_Max(fGFXExtent, pGfx->GetExtent());
		}
		if (fModelExtend > 0.001f && fGFXExtent > fModelExtend){
			m_aabb.Extents *= fGFXExtent/fModelExtend;

			//	扩充后如果有长度仍旧很小，则使用最大值填充
			float fMax = a_Max(m_aabb.Extents.x, m_aabb.Extents.y, m_aabb.Extents.z);
			if (m_aabb.Extents.x < 0.2f) m_aabb.Extents.x = fMax;
			if (m_aabb.Extents.y < 0.2f) m_aabb.Extents.y = fMax;
			if (m_aabb.Extents.z < 0.2f) m_aabb.Extents.z = fMax;

			m_aabb.CompleteMinsMaxs();
		}
	}
}

//	class CECViewModelExtractor
bool CECViewModelExtractor::CanExtract(CECIvtrItem *pItem)
{
	bool bCan(false);
	if (pItem != NULL){
		switch (pItem->GetClassID())
		{
		case CECIvtrItem::ICID_PETEGG:
			{
				CECIvtrPetEgg *pEgg = dynamic_cast<CECIvtrPetEgg *>(pItem);
				if (pEgg->GetPetEssence()!=NULL &&
					pEgg->GetEssence().pet_class == GP_PET_CLASS_MOUNT && 
					af_IsFileExist(pEgg->GetPetEssence()->file_model)){
					bCan = true;
				}
			}
			break;			
		case CECIvtrItem::ICID_FLYSWORD:
			{
				CECIvtrFlySword *pFlySword = dynamic_cast<CECIvtrFlySword *>(pItem);
				if (pFlySword->GetDBEssence() != NULL &&
					af_IsFileExist(pFlySword->GetDBEssence()->file_model)){
					bCan = true;
				}
			}
			break;
		}
	}
	return bCan;
}

CECModel * CECViewModelExtractor::Extract(CECIvtrItem *pItem)
{
	CECModel * pModel = NULL;
	if (CanExtract(pItem)){
		switch (pItem->GetClassID())
		{
		case CECIvtrItem::ICID_PETEGG:
			{
				CECIvtrPetEgg *pEgg = dynamic_cast<CECIvtrPetEgg *>(pItem);
				const char *szModel = pEgg->GetPetEssence()->file_model;
				if (CECPlayer::LoadPetModel(szModel, &pModel)){
					A3DCOLOR clr = CECPlayer::RIDINGPET::GetDefaultColor();
					CECPlayer::SetRidingPetColor(pModel, clr);
					pModel->SetPos(A3DVECTOR3(0));
					//	骑宠面朝左，可视空间大
					pModel->SetDirAndUp(A3DVECTOR3(-1.0f, 0, 0), A3DVECTOR3(0, 1.0f, 0));
					pModel->SetAutoUpdateFlag(false);
				}
			}
			break;
		case CECIvtrItem::ICID_FLYSWORD:
			{
				CECIvtrFlySword *pFlySword = dynamic_cast<CECIvtrFlySword *>(pItem);
				const char *szModel = pFlySword->GetDBEssence()->file_model;
				CECPlayer::EquipsLoadResult result;
				memset(&result, 0, sizeof(result));
				result.wingType = CECPlayer::FlyMode2WingType(pFlySword->GetDBEssence()->fly_mode);
				if (CECPlayer::ChangeWing(&result, szModel)){
					pModel = result.pWing;
					if (pModel){
						//	飞剑和翅膀骨骼较小，使用 MESH 进行计算
						pModel->SetAABBType(CECModel::AABB_INITMESH);
						pModel->SetPos(A3DVECTOR3(0));
						//if (result.bUsingWing){
						if (result.wingType == WINGTYPE_WING) {
							//	从后面看翅膀，可视空间更大
							pModel->SetDirAndUp(A3DVECTOR3(0, 0, 1.0f), A3DVECTOR3(0, 1.0f, 0));
						} else{
							//	飞剑面朝左，可视空间更大
							pModel->SetDirAndUp(A3DVECTOR3(-1.0f, 0, 0), A3DVECTOR3(0, 1.0f, 0));
						}
						pModel->SetAutoUpdateFlag(false);
						pModel->Show(true);
					}
				}
			}
			break;
		}
	}
	return pModel;
}

//	class CECViewModelCondition
bool CECViewModelCondition::Meet(CECIvtrItem *pItem)
{
	bool bCan(false);
	if (pItem != NULL){
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (pHost){
			switch (pItem->GetClassID())
			{
			case CECIvtrItem::ICID_PETEGG:
				{
					CECIvtrPetEgg *pEgg = dynamic_cast<CECIvtrPetEgg *>(pItem);
					if (pEgg->GetPetEssence()!=NULL &&
						pEgg->GetEssence().pet_class == GP_PET_CLASS_MOUNT){
						const PET_ESSENCE *pEssence = pEgg->GetPetEssence();
						if (!(pEssence->character_combo_id & (1 << pHost->GetProfession())))
							break;
						if (pEssence->level_require > pHost->GetBasicProps().iLevel)
							break;
						bCan = true;
					}
				}
				break;			
			case CECIvtrItem::ICID_FLYSWORD:
				{
					CECIvtrFlySword *pFlySword = dynamic_cast<CECIvtrFlySword *>(pItem);
					if (pFlySword->GetDBEssence() != NULL){
						const FLYSWORD_ESSENCE *pEssence = pFlySword->GetDBEssence();
						if (pEssence->require_player_level_min > pHost->GetBasicProps().iLevel)
							break;
						bCan = true;
					}
				}
				break;
			}//	switch
		}
	}
	return bCan;
}