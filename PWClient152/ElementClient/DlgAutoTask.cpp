// Filename	: DlgAutoTask.cpp
// Creator	: Feng Ning
// Date		: 2010/09/21

#include "DlgAutoTask.h"
#include "DlgTask.h"

#include "A2DSprite.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"

#include "elementdataman.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Utility.h"
#include "EC_GameUIMan.h"
#include "EC_TaskInterface.h"
#include "EC_Viewport.h"
#include "EC_HostPlayer.h"
#include "EC_Configs.h"
#include "TaskTemplMan.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "DlgExclusiveAward.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAutoTask, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoTask, CDlgBase)
AUI_ON_EVENT("Img_Reward",		WM_LBUTTONDOWN,		OnEventLButtonDown_Img_Reward)
AUI_ON_EVENT("Img_Reward",		WM_LBUTTONDBLCLK,	OnEventLButtonDown_Img_Reward)
AUI_ON_EVENT("Img_Reward",		WM_LBUTTONUP,		OnEventLButtonUp_Img_Reward)
AUI_END_EVENT_MAP()

static const char* DEFAULT_IMG = "version01\\新手宝箱\\宝箱.tga";
static const char* DEFAULT_GFX = "界面\\宝箱发光.gfx";
static const DWORD DEFAULT_COLOR = A3DCOLORRGB(255, 255, 255);

AString CDlgAutoTask::m_strIcon;
AString CDlgAutoTask::m_strGfx;

CDlgAutoTask::CDlgAutoTask()
:m_TaskID(0)
,m_RemainTime(0)
,m_LastTime(0)
,m_pImgAutoTask(NULL)
{
	ResetLButtonDownPosition();
}

CDlgAutoTask::~CDlgAutoTask()
{
}

bool CDlgAutoTask::OnInitDialog()
{
	//
	PAUIOBJECT pObj = GetDlgItem("Lab_Time");
	if(pObj) pObj->SetText(_AL(""));
	DDX_Control("Img_Reward", m_pImgAutoTask);


	return CDlgBase::OnInitDialog();
}

bool CDlgAutoTask::Release()
{
	//
	return CDlgBase::Release();
}

void CDlgAutoTask::OnShowDialog()
{
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	PAUIDIALOG pDlg = pGameUIMan->GetDialog("Win_Hpmpxp");
	POINT pos = pDlg->GetPos();
	SIZE size = pDlg->GetDefaultSize();
	SetPosEx(pos.x + size.cx, 0);
}

POINT CDlgAutoTask::GetMousePosition(LPARAM lParam)const{
	POINT result;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	result.x = GET_X_LPARAM(lParam) - p->X;
	result.y = GET_Y_LPARAM(lParam) - p->Y;
	return result;
}

void CDlgAutoTask::ResetLButtonDownPosition(){
	m_ptLButtonDown.x = m_ptLButtonDown.y = -1;
}

void CDlgAutoTask::OnEventLButtonDown_Img_Reward(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_ptLButtonDown = GetMousePosition(lParam);
}

void CDlgAutoTask::OnEventLButtonUp_Img_Reward(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	POINT ptLButtonDown = GetMousePosition(lParam);
	if (ptLButtonDown.x != m_ptLButtonDown.x ||
		ptLButtonDown.y != m_ptLButtonDown.y){
		ResetLButtonDownPosition();
		return;
	}
	ResetLButtonDownPosition();
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_ExclusiveAward");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}

void CDlgAutoTask::ConfirmAccept(unsigned long task_id)
{
	if(m_TaskID == task_id)	m_TaskID = 0;
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl* pTmpl = pMan->GetTaskTemplByID(task_id);
	if(pTmpl) pTmpl->ConfirmFinishAutoDelTask();
}

void CDlgAutoTask::UpdateAutoDelTask(unsigned long taskID, unsigned long remainTime)
{
	if( m_TaskID == taskID || remainTime <= m_RemainTime || m_TaskID == 0 )
	{
		// set the hint message for task
		if(taskID != m_TaskID && taskID > 0)
		{	
			ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
			ATaskTempl* pTmpl = pMan->GetTaskTemplByID(taskID);
			if(pTmpl)
			{
				AUTOTASK_DISPLAY_PARAM DisplayParam;
				DisplayParam.color = DEFAULT_COLOR;
				DisplayParam.file_gfx = DEFAULT_GFX;
				DisplayParam.file_icon = DEFAULT_IMG;				
				GetDisplayParam(taskID, DisplayParam);
				
				// 设置文字提示
				m_pImgAutoTask->SetHint(CDlgTask::FormatTaskText(pTmpl->GetDescription(), 
														DisplayParam.color));
				AUIOBJECT_SETPROPERTY prop;
				// 设置图标
				if (DisplayParam.file_icon[0]) strcpy(prop.fn, DisplayParam.file_icon);
				// 没填路径则使用默认图标
				else strcpy(prop.fn, DEFAULT_IMG);
				
				{
					ScopedAUIControlSpriteModify _dummy(m_pImgAutoTask);
					m_pImgAutoTask->SetProperty("Image File", &prop);
				}

				A2DSprite* pSprite = m_pImgAutoTask->GetImage();
				if (pSprite) pSprite->SetLinearFilter(true);
				m_strIcon = prop.fn;
				// 设置gfx
				if (DisplayParam.file_gfx[0]) strcpy(prop.fn, DisplayParam.file_gfx);
				// 每天路径则使用默认gfx
				else strcpy(prop.fn, DEFAULT_GFX);
				m_pImgAutoTask->SetProperty("Gfx File", &prop);
				m_strGfx = prop.fn;
				if (!m_pImgAutoTask->GetGfx()){
					a_LogOutput(1, "CDlgAutoTask::UpdateAutoDelTask, unable to load gfx %s", m_strGfx);
				}
			}
		}
		else if(taskID == 0)
		{
			m_pImgAutoTask->SetHint(_AL(""));
		}

		m_TaskID = taskID;
		m_RemainTime = remainTime;
		m_LastTime = GetGame()->GetServerAbsTime();

		if(g_pGame->GetConfigs()->IsMiniClient() || glb_IsInMapForMiniClient()) Show(false);
		else if(!IsShow()){
			if (GetGameUIMan()->IsCustomizeCharacter()){
				GetGameUIMan()->AddRestoreDlgToCC(this);
			}else{
				Show(true);
			}
		}
	}
}
void CDlgAutoTask::UpdateShow()
{
	CDlgExclusiveAward* pExclu = (CDlgExclusiveAward*)GetGameUIMan()->GetDialog("Win_ExclusiveAward");
	bool bCheck = pExclu->GetExclusiveAwardMode();
	bool bShow = IsShow();
	if (!bCheck && !bShow)
	{		
		if (!GetGameUIMan()->IsCustomizeCharacter() && pExclu->HasTaskToShow())
		{
			AUIOBJECT_SETPROPERTY prop;
			// 设置图标使用默认图标
			strcpy(prop.fn, DEFAULT_IMG);
			{
				ScopedAUIControlSpriteModify _dummy(m_pImgAutoTask);
				m_pImgAutoTask->SetProperty("Image File", &prop);
			}
			Show(true);
		}
	}
}
void CDlgAutoTask::OnTick()
{
	CDlgExclusiveAward* pExclu = (CDlgExclusiveAward*)GetGameUIMan()->GetDialog("Win_ExclusiveAward");
	bool bCheck = pExclu->GetExclusiveAwardMode(); // true: 原来的行为； false： 该界面一直显示；
	if(m_TaskID == 0 && bCheck)
	{
		Show(false);
	}
	
	CECTaskInterface* pTask = GetHostPlayer()->GetTaskInterface();
	if (!pTask->HasTask(m_TaskID) && bCheck) Show(false);

	// control remain time
	PAUIOBJECT pObj = GetDlgItem("Lab_Time");
	if(m_RemainTime > 0)
	{
		int curTime = GetGame()->GetServerAbsTime();
		if(m_LastTime > 0)
		{
			int eclipse = min(m_RemainTime, max(0, curTime - m_LastTime));
			m_RemainTime -= eclipse;
		}
		m_LastTime = curTime;

		pObj->Show(true);
		ACString szTxt;
		szTxt.Format(GetStringFromTable(7628), GetGameUIMan()->GetFormatTimeSimple(m_RemainTime));
		pObj->SetText(szTxt);
	}
	else
	{
		// hide the remain time
		pObj->Show(false);
	}

	// control gfx
	A3DGFXEx* pGfx = m_pImgAutoTask->GetGfx();
	if(pGfx) pGfx->SetVisible(m_TaskID > 0 && m_RemainTime == 0);
}

void CDlgAutoTask::OnCommand_CANCEL(const char * szCommand)
{
	// ignore
	return;
}

bool CDlgAutoTask::GetDisplayParam(int taskid, AUTOTASK_DISPLAY_PARAM& param)
{
	bool ret(false);
	static bool bConfigLoaded(false);
	typedef abase::vector<AUTOTASK_DISPLAY_CONFIG*> VecAutoTaskConfig;
	static VecAutoTaskConfig vecConfig;
	if (!bConfigLoaded)
	{
		bConfigLoaded = true;
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
		
		while(tid)
		{
			if(DataType == DT_AUTOTASK_DISPLAY_CONFIG)
			{
				AUTOTASK_DISPLAY_CONFIG* pAutoTaskConfig = (AUTOTASK_DISPLAY_CONFIG *)pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType);
				if (pAutoTaskConfig)
					vecConfig.push_back(pAutoTaskConfig);
			}
			tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
		}
	}

	for (size_t i = 0; i < vecConfig.size() && taskid && !ret; ++i)
	{
		AUTOTASK_DISPLAY_CONFIG* pAutoTaskConfig = vecConfig[i];
		unsigned int size = ARRAY_SIZE(pAutoTaskConfig->list);
		for (unsigned int j = 0; j < size; ++j)
		{
			if (taskid == pAutoTaskConfig->list[j].task_id)
			{
				param.color = pAutoTaskConfig->list[j].color;
				param.file_gfx = pAutoTaskConfig->list[j].file_gfx;
				param.file_icon = pAutoTaskConfig->list[j].file_icon;
				ret = true;
				break;
			}
		}
	}

	return ret;
}

void CDlgAutoTask::OnChangeLayoutEnd(bool bAllDone)
{
	AUIOBJECT_SETPROPERTY prop;
	strcpy(prop.fn, m_strIcon);
	{
		ScopedAUIControlSpriteModify _dummy(m_pImgAutoTask);
		m_pImgAutoTask->SetProperty("Image File", &prop);
	}
	strcpy(prop.fn, m_strGfx);
	m_pImgAutoTask->SetProperty("Gfx File", &prop);
}
