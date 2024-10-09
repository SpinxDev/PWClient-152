// Filename	: DlgPetFittingRoom.cpp
// Creator	: Xu Wenbin
// Date		: 2010/3/9

#include "AFI.h"
#include "DlgPetFittingRoom.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_Model.h"
#include "EC_Player.h"
#include "EC_ManPlayer.h"
#include "EC_NPC.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetFittingRoom, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetFittingRoom, CDlgBase)
AUI_END_EVENT_MAP()

CDlgPetFittingRoom::CDlgPetFittingRoom()
: m_pModel(NULL)
, m_nAngle(0)
, m_idPet(0)
, m_color(0)
, m_aabb(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0))
{
}

CDlgPetFittingRoom::~CDlgPetFittingRoom()
{
	A3DRELEASE(m_pModel);
}

void CDlgPetFittingRoom::Reset()
{
	A3DRELEASE(m_pModel);
	m_nAngle = 0;
	m_idPet = 0;
	m_color = 0;
	m_aabb = A3DAABB(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0));

	UpdateRenderCallback();
}

void CDlgPetFittingRoom::UpdateRenderCallback()
{	
	if (m_pImg_Pet)
		m_pImg_Pet->SetRenderCallback(NPCRenderDemonstration, (DWORD)m_pModel, m_nAngle, (DWORD)&m_aabb);
}

bool CDlgPetFittingRoom::OnInitDialog()
{
	m_pImg_Pet = (PAUIIMAGEPICTURE)GetDlgItem("Img_Pet");
	m_pBtn_LeftTurn = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_LeftTurn");
	m_pBtn_RightTurn = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_RightTurn");
	return true;
}

CECModel * CDlgPetFittingRoom::LoadPetModel(int idPet, A3DCOLOR clr)
{
	CECModel *pModel = NULL;
	if (idPet > 0)
	{
		DATA_TYPE DataType;
		const PET_ESSENCE* pEssence = (const PET_ESSENCE*)GetGame()->GetElementDataMan()->get_data_ptr(idPet, ID_SPACE_ESSENCE, DataType);
		if (DataType == DT_PET_ESSENCE && pEssence != NULL && pEssence->file_model[0])
		{
			if (CECPlayer::LoadPetModel(pEssence->file_model, &pModel))
			{
				CECPlayer::SetRidingPetColor(pModel, clr);
				
				pModel->SetPos(A3DVECTOR3(0));
				pModel->SetDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
				pModel->SetAutoUpdateFlag(false);
			}
		}
	}

	return pModel;
}

void CDlgPetFittingRoom::OnShowDialog()
{
	m_nAngle = 0;
}

void CDlgPetFittingRoom::OnTick()
{
	if( m_pBtn_LeftTurn->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK )
		m_nAngle = (m_nAngle + 360 - GetGame()->GetTickTime() / 6 ) % 360;
	else if( m_pBtn_RightTurn->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK )
		m_nAngle = (m_nAngle + GetGame()->GetTickTime() / 6) % 360;

	if (m_pModel)
	{
		const char *szName = CECNPC::GetBaseActionName(CECNPC::ACT_STAND);
		m_pModel->PlayActionByName(szName, 1.0f, false);
		m_pModel->Tick(GetGame()->GetTickTime());
		UpdateRenderCallback();
	}
}

void CDlgPetFittingRoom::OnCommand_CANCEL(const char * szCommand)
{
	SetPet(0);
	Show(false);
}

void CDlgPetFittingRoom::SetPet(int tid, A3DCOLOR clr)
{
	if (tid == m_idPet)
	{
		//	宠物 id 相同，直接设置颜色即可
		SetPetColor(clr);
		return;
	}
	
	//	释放现有模型
	Reset();

	CECModel *pModel = LoadPetModel(tid, clr);
	if (!pModel)	return;

	m_pModel = pModel;
	m_idPet = tid;
	m_color = clr;
	m_nAngle = 0;
	
	pModel->Tick(0);
	m_aabb = pModel->GetModelAABB();

	UpdateRenderCallback();
}

void CDlgPetFittingRoom::SetPetColor(A3DCOLOR clr)
{
	if (m_idPet)
	{
		m_color = clr;
		if (m_pModel)
			CECPlayer::SetRidingPetColor(m_pModel, clr);
	}
}
