// Filename	: DlgPetEvolutionAnim.cpp
// Creator	: Wang Yongdong
// Date		: 2012/12/04

#include "DlgPetEvolutionAnim.h"

#include "AFI.h"
#include "EC_GameUIMan.h"
#include "EC_PetCorral.h"
#include "EC_Game.h"
#include "elementdataman.h"
#include "EC_Global.h"
#include "EC_Viewport.h"
#include <A3DSkin.h>
#include "EC_Model.h"
#include "EC_NPC.h"
#include "EC_HostPlayer.h"
#include "A3DCombinedAction.h"
#include "EC_PortraitRender.h"
#include "A3DCamera.h"
#include "A3DSkinRender.h"
#include <A3DSkinMan.h>
#include <A3DLight.h>

#define new A_DEBUG_NEW

#define INTERAL_LONG	618
#define INTERAL_SHORT	100

#define MAP_DEFAULT_WIDTH 1024
#define MAP_DEFAULT_HEIGHT 768

const char* PET_EVO_START = "宠物进化_开始";
const char* PET_EVO_END = "宠物进化_结束";


AUI_BEGIN_COMMAND_MAP(CDlgPetEvoAnim, CDlgBase)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetEvoAnim, CDlgBase)
AUI_END_EVENT_MAP()

CDlgPetEvoAnim::CDlgPetEvoAnim()
: m_pModel(NULL)
, m_idPet(0)
, m_idEvo(0)
, m_aabb(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0))
,m_pCurModel(NULL)
,m_pNewModel(NULL)
,m_bLastState(false)
{
}

CDlgPetEvoAnim::~CDlgPetEvoAnim()
{
	A3DRELEASE(m_pModel);
	A3DRELEASE(m_pNewModel);
}

void CDlgPetEvoAnim::Reset()
{
	A3DRELEASE(m_pModel);
	A3DRELEASE(m_pNewModel);
	m_idPet = 0;
	m_idEvo = 0;
	m_aabb = A3DAABB(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0));
	m_newaabb = A3DAABB(A3DVECTOR3(0, 0, 0), A3DVECTOR3(0, 0, 0));
	m_counter.Reset();
	m_lagCounter.Reset();
	m_lastCounter.Reset();
	m_incubatorCounter.Reset();
	m_bStandFlag = false;
	m_bLastState = false;
}

void CDlgPetEvoAnim::UpdateRenderCallback()
{	
	if (m_pImg_Pet && m_pCurModel)
	{
		m_pImg_Pet->SetRenderCallback(NPCRenderDemonstration, (DWORD)m_pCurModel, 0, (DWORD)&m_aabb);
	}
}

bool CDlgPetEvoAnim::OnInitDialog()
{
	m_pImg_Pet = (PAUIIMAGEPICTURE)GetDlgItem("Img_Char");

	m_counter.SetPeriod(20000);
	m_lagCounter.SetPeriod(2000);
	m_lastCounter.SetPeriod(3000);
	m_incubatorCounter.SetPeriod(3000);

	m_fScale = 1.0f;

	m_bStandFlag = false;
	m_bLastState = false;

	return true;
}
void CDlgPetEvoAnim::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
}

CECModel * CDlgPetEvoAnim::LoadPetModel(int idPet)
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
				pModel->SetPos(A3DVECTOR3(0));
				pModel->SetDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
				pModel->SetAutoUpdateFlag(false);
			}
		}
	}

	return pModel;
}

void CDlgPetEvoAnim::OnShowDialog()
{
	GetDlgItem("Img_Bg")->SetAlpha(255);
	GetDlgItem("Img_Char")->SetAlpha(255);
}
void CDlgPetEvoAnim::OnHideDialog()
{
	m_idEvo = 0; 
	m_idPet = 0;

	if (m_pModel)
		m_pModel->StopAllActions(true);
	if(m_pNewModel)
		m_pNewModel->StopAllActions(true);
}

void CDlgPetEvoAnim::OnTick()
{
	// 新模型播进化结束动作后，再播站立
	if (m_pNewModel && (m_pNewModel == m_pCurModel) && m_pNewModel->IsActionStopped(0))
	{
		const char *szName = CECNPC::GetBaseActionName(CECNPC::ACT_STAND);
		m_pNewModel->PlayActionByName(szName, 1.0f, false);
		m_bLastState = true;
	}

	// 新模型播放完2秒后关闭界面
	if (m_bLastState)
	{
		int alpha = 255 * m_lastCounter.GetCounter() / m_lastCounter.GetPeriod();
		alpha = 255 - alpha;
		a_Clamp(alpha,0,255);
		GetDlgItem("Img_Bg")->SetAlpha(alpha);
		GetDlgItem("Img_Char")->SetAlpha(0);
	
		if (m_lastCounter.IncCounter(GetGame()->GetRealTickTime()))
		{
			Show(false);		
			m_pCurModel = NULL;
			m_lastCounter.Reset();
		}		
	}

	// 切换模型
	bool bSwitchModel = false;
	if (m_pCurModel && m_pCurModel->IsActionStopped(0) && (m_pCurModel != m_pNewModel) && !m_bStandFlag && m_incubatorCounter.IncCounter(GetGame()->GetRealTickTime()))
	{
		m_pCurModel = m_pNewModel;			
		
		// 播放孵化后的动作， 该动作不能是循环动作
		if(m_pNewModel)
			m_pNewModel->PlayActionByName(PET_EVO_END,1.0f,false);
		
		m_incubatorCounter.Reset();
		bSwitchModel = true;
	}
		
	// 确保界面关闭。
	if (m_counter.IncCounter(GetGame()->GetRealTickTime()))
	{
		Show(false);
	}
	
	if (m_bStandFlag && m_lagCounter.IncCounter(GetGame()->GetRealTickTime()))
	{
		// 	// 播放孵化前的动作， 该动作不能为循环动作
		if(m_pCurModel)
			m_pCurModel->PlayActionByName(PET_EVO_START,1.0f,false);

		m_bStandFlag = false;
		m_lagCounter.Reset();
		m_incubatorCounter.Reset();
	}

	if (m_pCurModel)
	{
		m_pCurModel->Tick(GetGame()->GetTickTime());
	
// 		if(m_pCurModel!= m_pModel || m_idPet != 33908)
// 			ComputeAABB(m_pCurModel,m_aabb);
		if((m_pCurModel == m_pModel && m_idPet != 33908) || bSwitchModel)
			ComputeAABB(m_pCurModel,m_aabb);

		UpdateRenderCallback();
	}
}

void CDlgPetEvoAnim::OnCommand_CANCEL(const char * szCommand)
{
	SetPet(0,0);
	Show(false);
}
void CDlgPetEvoAnim::ComputeAABB(CECModel* pModel, A3DAABB& aabb)
{
	if(!pModel) return;

	aabb = pModel->GetModelAABB(); // 可能是bone的aabb，所以有时候aabb会小于mesh的aabb，导致处于视锥外面

	// mesh的aabb
	for (int i=0;i<pModel->GetSkinNum();i++)
	{
		A3DAABB a = pModel->GetA3DSkin(i)->GetInitMeshAABB();
		aabb.Merge(a);
	}

	if (pModel == m_pNewModel && m_idEvo == 37900) //皇骸
		aabb.Extents *= 1.1f;
	else if(pModel == m_pModel && m_idPet == 33908) //帝骨
		aabb.Extents *= 1.8f;
	else if(pModel == m_pModel && m_idPet == 11681) //小青蛙
		aabb.Extents *= 1.1f;
	else if(pModel == m_pNewModel && m_idPet == 38608) // 月宫蟾
		aabb.Extents *= 1.8f;
	else if(m_idPet != 18156) //炽天凤凰
		aabb.Extents *= 1.8f;

	
	aabb.CompleteMinsMaxs();
}
void CDlgPetEvoAnim::SetPet(int tid,int evo_id)
{	
	//	释放现有模型
	Reset();

	m_idPet = tid;	
	m_idEvo = evo_id;
	
	CECModel *pModel = LoadPetModel(tid);
	if (pModel)	
	{
		m_pModel = pModel;	
	}	
	pModel = LoadPetModel(evo_id);
	if (pModel)
	{
		m_pNewModel = pModel;	
	}

	m_pCurModel = m_pModel;

	const char *szName = CECNPC::GetBaseActionName(CECNPC::ACT_STAND);
	if(m_pCurModel)
	{
		m_pCurModel->PlayActionByName(szName, 1.0f, false);		

		ComputeAABB(m_pCurModel,m_aabb);
		m_bStandFlag = true;
	}

	ResizeWindow(NULL);
}

void CDlgPetEvoAnim::ResizeWindow(A3DRECT *prc)
{
	RECT rc;
	if( prc )
	{
		rc.left = prc->left;
		rc.top = prc->top;
		rc.right = prc->right;
		rc.bottom = prc->bottom;
	}
	else
	{
		A3DVIEWPORTPARAM param = *g_pGame->GetViewport()->GetA3DViewport()->GetParam();
		rc.left = param.X;
		rc.top = param.Y;
		rc.right = param.X + param.Width;
		rc.bottom = param.Y + param.Height;
	}


	const int dw = MAP_DEFAULT_WIDTH; // default width
	const int dh = MAP_DEFAULT_HEIGHT; // default height
	const float dr = (float)dw / dh; // default ratio
	
	const int w = rc.right - rc.left;
	const int h = rc.bottom - rc.top;
	const float r = (float)w / h; // current ratio

	PAUIOBJECT pObj = NULL;

	// resize the dlg size	
	SetSize(w, h);
	SetPosEx(0, 0, AUIDialog::alignCenter, AUIDialog::alignCenter);

	PAUIOBJECT pBG = GetDlgItem("Img_Bg");
	pBG->SetSize( w, h );
	pBG->SetPos( 0, 0 ); 

	pObj = GetDlgItem("Img_Char");
	pObj->SetSize(w,h);
	pObj->SetPos( 0, 0 ); 
}

