#include "DlgCustomizeBase.h"
#include "DlgCustomizeFace.h"
#include "EC_CustomizeMgr.h"
#include "EC_CustomizeBound.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"

#include <AUIManager.h>

AUI_BEGIN_COMMAND_MAP(CDlgCustomizeBase, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_Cancel)
AUI_ON_COMMAND("Btn_Close",		OnCommand_Cancel)
AUI_ON_COMMAND("ChoosePreCustomize", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseFace", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseFace2", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseEye", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseEye2", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseNoseMouth", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseNoseMouth2", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseHair", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseFacePainting", OnCommandSwitchDialog)
AUI_ON_COMMAND("ChooseStature", OnCommandSwitchDialog)

AUI_END_COMMAND_MAP()



AUI_BEGIN_EVENT_MAP(CDlgCustomizeBase, CDlgBase)

AUI_END_EVENT_MAP()


CDlgCustomizeBase::CDlgCustomizeBase()
{
	m_bContextReady = false;
	m_pCustomize = NULL;
}

CDlgCustomizeBase::~CDlgCustomizeBase()
{

}

void CDlgCustomizeBase::OnShowDialog()
{
	CDlgBase::OnShowDialog();

}

bool CDlgCustomizeBase::InitContext(CECCustomizeMgr *pCustomize)
{
	m_pCustomize = pCustomize;

	m_bContextReady = true;
	if (!OnInitContext())
		return false;

	//m_bContextReady = true;
	return true;
}

CECCustomizeMgr * CDlgCustomizeBase::GetCustomizeMan()
{
	return m_pCustomize;
}

CECFace * CDlgCustomizeBase::GetFace()
{
	return GetPlayer()->GetFaceModel();
}

CECFace::FACE_CUSTOMIZEDATA * CDlgCustomizeBase::GetFaceData()
{
	return GetFace()->GetFaceData();
}

CECPlayer * CDlgCustomizeBase::GetPlayer()
{
	return GetCustomizeMan()->m_pCurPlayer;
}

CECCustomizeBound * CDlgCustomizeBase::GetSliderBound()
{
	return GetCustomizeMan()->m_pSliderBound;
}


void CDlgCustomizeBase::OnCommandSwitchDialog(const char *szCommand)
{
	GetCustomizeMan()->SwitchDialog(szCommand);
}


CECPlayer::PLAYER_CUSTOMIZEDATA * CDlgCustomizeBase::GetCustomizeData()
{
	return &GetPlayer()->GetCustomizeData();
}

void CDlgCustomizeBase::OnCommand_Cancel(const char * szCommand)
{
	int nCurUIMan = GetGameRun()->GetUIManager()->GetCurrentUIMan();
	if (nCurUIMan == CECUIManager::UIMAN_LOGIN){
		PAUIDIALOG pDlgCreateGenderName = GetAUIManager()->GetDialog("Win_CreateGenderName");
		if (pDlgCreateGenderName->IsShow()){
			pDlgCreateGenderName->OnCommand("IDCANCEL");
			return;
		}
	}
	PAUIDIALOG pDlgCustomize = GetAUIManager()->GetDialog("Win_Customize");
	if (this != pDlgCustomize){
		pDlgCustomize->OnCommand("IDCANCEL");
	}
}