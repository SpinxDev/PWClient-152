// File		: DlgSystem.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/15

#include "DlgSystem.h"
#include "DlgSystem2.h"
#include "DlgSystem3.h"
#include "DlgCharacter.h"
#include "DlgTask.h"
#include "DlgTrade.h"
#include "DlgShop.h"
#include "DlgInventory.h"
#include "DlgSkillSubList.h"
#include "DlgSkillAction.h"

#include "DlgQShop.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "globaldataman.h"
#include "EC_HostPlayer.h"
#include "EC_FixedMsg.h"
#include "EC_Configs.h"
#include "EC_Utility.h"
#include "EC_Shop.h"
#include "EC_HostSkillModel.h"
#include "EC_PlayerLevelRankRealmChangeCheck.h"
#include "EC_TaoistRank.h"
#include "EC_UIHelper.h"
#include "EC_GameUIMan.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSystem, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCAL)
AUI_ON_COMMAND("winventory",	OnCommandInventory)
AUI_ON_COMMAND("wcharacter",	OnCommandCharacter)
//AUI_ON_COMMAND("wskill",		OnCommandSkill)
AUI_ON_COMMAND("wquest",		OnCommandQuest)
AUI_ON_COMMAND("wmain2",		OnCommandMain2)
AUI_ON_COMMAND("wmain3",		OnCommandMain3)
AUI_ON_COMMAND("wshop",			OnCommandShop)
AUI_ON_COMMAND("mini",			OnCommandMini)
AUI_ON_COMMAND("Btn_Function",	OnCommandMain4)
AUI_ON_COMMAND("Btn_Practice",	OnCommandMain5)
AUI_ON_COMMAND("Btn_ModeChange",OnCommandModeChange)
AUI_END_COMMAND_MAP()

CDlgSystem::CDlgSystem()
{
	m_pChktemp = NULL;
	m_pBtnInventory = NULL;
	m_pBtnCharacter = NULL;
	m_pBtnSkill = NULL;
	m_pBtnQuest = NULL;
	m_pBtnConnection = NULL;
	m_pBtnSystem = NULL;
	m_pFrameSystem2 = NULL;
	m_pBtnFunction = NULL;
	m_pBtnModeChange = NULL;
}

CDlgSystem::~CDlgSystem()
{
}

void CDlgSystem::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Chk_temp", m_pChktemp);
	DDX_Control("Btn_Inventory", m_pBtnInventory);
	DDX_Control("Btn_Character", m_pBtnCharacter);
	DDX_Control("Btn_Practice", m_pBtnSkill);
	DDX_Control("Btn_Quest", m_pBtnQuest);
	DDX_Control("Btn_Connection", m_pBtnConnection);
	DDX_Control("Btn_System", m_pBtnSystem);
	DDX_Control("Frame_System2", m_pFrameSystem2);
	DDX_Control("Btn_Function", m_pBtnFunction);
	DDX_Control("Btn_ModeChange", m_pBtnModeChange);

	ShowNewImg(false);
}

void CDlgSystem::OnTick()
{
	if (m_pBtnSystem->IsShow())
	{
		
		ACString strHint;
		ACString strTemp;

		//	服务器时间
		strTemp.Format(GetStringFromTable(8780), GetGameRun()->GetServerName(), GetGameRun()->GetServerGroup());
		strHint += strTemp;

		//	网络状况
		int delay = GetGameRun()->GetInGameNetworkDelay();
		int delayClass = (delay <= 300 ? 0 : delay <= 600 ? 1 : 2);
		strTemp.Format(GetStringFromTable(8781),
			GetStringFromTable(8783 + delayClass),
			delay,
			GetStringFromTable(8786 + delayClass));
		strHint += strTemp;

		//	更新速度
		int frameRate = GetFPS();
		strTemp.Format(GetStringFromTable(8782), frameRate);
		strHint += strTemp;

		m_pBtnSystem->SetHint(strHint);
	}
}

int CDlgSystem::GetFPS(void)
{
	static bool	s_bFirst = true;	//	第一次调用此函数
	static int s_lastFPS = 0;		//	上次更新后保存的FPS
	static DWORD s_lastUpdate = timeGetTime();	//	上次更新时戳
	const DWORD GET_FPS_INTERFAL = 15000;		//	FPS 更新周期

	bool bUpdate = false;
	DWORD dwCurrent = timeGetTime();
	if (s_bFirst)
	{
		//	首次调用此函数，需初始化
		bUpdate = true;
		s_bFirst = false;
	}
	else if (dwCurrent >= s_lastUpdate + GET_FPS_INTERFAL)
	{
		//	更新周期已到，需要更新
		s_lastUpdate = dwCurrent;
		bUpdate = true;
	}

	if (bUpdate)
	{
		//	更新当前的 FPS
		s_lastFPS = (int)(GetGame()->GetAverageFrameRate()+0.5f);
	}

	return s_lastFPS;
}

void CDlgSystem::OnCommandCANCAL(const char *szCommand)
{
}

void CDlgSystem::OnCommandInventory(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;
	CDlgInventory* pDlg = GetGameUIMan()->m_pDlgInventory;
	if(pDlg)
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem::OnCommandCharacter(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;
	CDlgCharacter* pDlg = GetGameUIMan()->m_pDlgCharacter;
	if( pDlg != NULL && !pDlg->IsShow() )
	{
		GetGameSession()->c2s_CmdGetExtProps();
		pDlg->ResetPoints();
	}
	if(pDlg)
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem::OnCommandSkill(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;
	GetGameUIMan()->m_pDlgSkillAction->TryOpenDialog(false);
}

void CDlgSystem::OnCommandQuest(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;

	GetGameUIMan()->m_pDlgTask->Show(!GetGameUIMan()->m_pDlgTask->IsShow());
	OnGfxButtonClicked(szCommand);
}

void CDlgSystem::OnCommandMain2(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;

	CDlgSystem2* pDlg = IsHorizontal()? GetGameUIMan()->m_pDlgSystem2 : GetGameUIMan()->m_pDlgSystem2b;
	if(pDlg)
	{
		OpenOrCloseSubMenuDialog(pDlg);
	}
}

void CDlgSystem::OnCommandMain3(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;
	
	CDlgSystem3* pDlg = IsHorizontal()? GetGameUIMan()->m_pDlgSystem3 : GetGameUIMan()->m_pDlgSystem3b;
	if(pDlg)
	{
		OpenOrCloseSubMenuDialog(pDlg);
	}
}

void CDlgSystem::OnCommandMini(const char *szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;

	m_pBtnInventory->Show(!m_pChktemp->IsChecked());
	m_pBtnCharacter->Show(!m_pChktemp->IsChecked());
	m_pBtnSkill->Show(!m_pChktemp->IsChecked());
	m_pBtnQuest->Show(!m_pChktemp->IsChecked());
	m_pBtnConnection->Show(!m_pChktemp->IsChecked());
	m_pBtnSystem->Show(!m_pChktemp->IsChecked());
	m_pFrameSystem2->Show(!m_pChktemp->IsChecked());
	m_pBtnFunction->Show(!m_pChktemp->IsChecked());
	m_pBtnModeChange->Show(!m_pChktemp->IsChecked());
}

void CDlgSystem::OnCommandShop(const char *szCommand)
{
	CDlgQShop *pDlgQShop = CECUIHelper::GetShopDialog(true);
	if (pDlgQShop->IsShow()){
		pDlgQShop->OnCommand_CANCEL("");
		return;
	}
	CDlgQShop *pDlgBackShop = CECUIHelper::GetShopDialog(false);
	if (pDlgBackShop->IsShow()){
		pDlgBackShop->OnCommand_CANCEL("");
		return;
	}
	CECUIHelper::ShowQShop(0, true);
}

void CDlgSystem::OnCommandMain4(const char* szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;

	CDlgSystem4* pDlg = IsHorizontal() ? GetGameUIMan()->m_pDlgSystem4 : GetGameUIMan()->m_pDlgSystem4b;
	if(pDlg)
	{
		OpenOrCloseSubMenuDialog(pDlg);
	}
}


void CDlgSystem::OnCommandMain5(const char* szCommand)
{
	if( GetGameUIMan()->m_pDlgTrade->IsShow() ||
		GetGameUIMan()->m_pDlgShop->IsShow() )
		return;
	
	CDlgSystem5* pDlg = IsHorizontal() ? GetGameUIMan()->m_pDlgSystem5 : GetGameUIMan()->m_pDlgSystem5b;
	if(pDlg)
	{
		OpenOrCloseSubMenuDialog(pDlg);
	}
}

void CDlgSystem::OnCommandModeChange(const char* szCommand) {
	Show(false);
	GetTwinDialog()->Show(true);

	if (GetGameUIMan()->m_pShowOld) {
		GetGameUIMan()->m_pShowOld->Show(false);
		GetGameUIMan()->m_pShowOld = dynamic_cast<CDlgHorizontalVertical*>(GetGameUIMan()->m_pShowOld)->GetTwinDialog();
		GetGameUIMan()->m_pShowOld->Show(true);
	}
}

void CDlgSystem::OpenOrCloseSubMenuDialog(AUIDialog* pDlg) {
	PAUIDIALOG pShowOld = GetGameUIMan()->m_pShowOld;
	if( pDlg->IsShow() )
	{
		pDlg->Show(false);
		GetGameUIMan()->m_pShowOld = NULL;
	}
	else
	{
		if( pShowOld ) pShowOld->Show(false);
		
		SIZE s = pDlg->GetSize();
		A3DRECT rc = GetRect();
		
		if (IsHorizontal()) {
			pDlg->SetPosEx(rc.right - s.cx, rc.top - s.cy + 1);
		} else {
			pDlg->SetPosEx(rc.left - s.cx, rc.bottom - s.cy + 1);
		}
		GetGameUIMan()->m_pShowOld = pDlg;
		pDlg->Show(true);
	}
}

void CDlgSystem::ShowNewImg(bool bShow) {
	AUIObject* pObj = GetDlgItem("Img_New");
	if (pObj) {
		pObj->Show(bShow);
	}
}

////////////////////////////////////////////////////////////////////

CDlgHorizontalVertical* CDlgHorizontalVertical::GetTwinDialog() {
	if (m_pTwinDlg == NULL) {
		int len = strlen(GetName());
		char* strDlgName = new char[len + 2];
		strcpy(strDlgName, GetName());
		if (strDlgName[len - 1] == 'b') {
			m_bHorizontal = false;
			strDlgName[len - 1] = '\0';
			m_pTwinDlg = dynamic_cast<CDlgHorizontalVertical*>(GetGameUIMan()->GetDialog(strDlgName));
		} else {
			m_bHorizontal = true;
			strDlgName[len] = 'b';
			strDlgName[len + 1] = '\0';
			m_pTwinDlg = dynamic_cast<CDlgHorizontalVertical*>(GetGameUIMan()->GetDialog(strDlgName));
		}
		delete [] strDlgName;
	}
	return m_pTwinDlg;
}

bool CDlgHorizontalVertical::IsHorizontal() {
	int len = strlen(GetName());
	return GetName()[len - 1] != 'b';
}

//////////////////////////////////////////////////////////////////////

// 观察玩家的等级、修真、境界变化，提示有新技能可学
class CECShowNewSkillHelper : public CECObserver<CECPlayerLevelRankRealmChangeCheck> {
public:
	CECShowNewSkillHelper() {
		CECPlayerLevelRankRealmChangeCheck::Instance().RegisterObserver(this);
	}
	~CECShowNewSkillHelper() {
		CECPlayerLevelRankRealmChangeCheck::Instance().UnregisterObserver(this);
	}
	void OnModelChange(const CECPlayerLevelRankRealmChangeCheck *p, const CECObservableChange *q) {
		const CECPlayerLevelRankRealmChange* change = dynamic_cast<const CECPlayerLevelRankRealmChange*>(q);
		if (change) {
			CECHostPlayer* pHost = CECUIHelper::GetHostPlayer();
			unsigned int oldRank = pHost->GetBasicProps().iLevel2;
			unsigned int oldMaxLevel = pHost->GetMaxLevelSofar();
			unsigned int oldRealmLevel = pHost->GetRealmLevel();

			bool bNeedUpdateSkill = false;

			switch (change->m_changeMask) {
			case CECPlayerLevelRankRealmChange::MAX_LEVEL_CHANGE:
				{
					bNeedUpdateSkill = true;
					oldMaxLevel = change->m_oldValue;
					break;
				}
			case CECPlayerLevelRankRealmChange::RANK_CHANGE:
				{
					// 如果是仙魔转换，则不提示新技能
					const CECTaoistRank* oldTaoistRank = CECTaoistRank::GetTaoistRank(change->m_oldValue);
					const CECTaoistRank* newTaoistRank = CECTaoistRank::GetTaoistRank(change->m_newValue);
					if (oldTaoistRank && newTaoistRank) {
						if (oldTaoistRank->IsEvilRank() && newTaoistRank->IsGodRank()) {
							break;
						}
						if (oldTaoistRank->IsGodRank() && newTaoistRank->IsEvilRank()) {
							break;
						}
					}
					bNeedUpdateSkill = true;
					oldRank = change->m_oldValue;
					break;
				}
			case CECPlayerLevelRankRealmChange::REALM_LEVEL_CHANGE:
				{
					bNeedUpdateSkill = true;
					oldRealmLevel = change->m_oldValue;
					break;
				}
			default:
				break;
			}

			if (bNeedUpdateSkill) {
				if (CECHostSkillModel::Instance().CheckHasNewSkillCanLearn(oldMaxLevel, oldRank, oldRealmLevel)) {
					CECGameUIMan* pGameUIMan = CECUIHelper::GetGameUIMan();
					CDlgSystem* pHDlgSystem = dynamic_cast<CDlgSystem*>(pGameUIMan->GetDialog("Win_Main"));
					CDlgSystem* pVDlgSystem = dynamic_cast<CDlgSystem*>(pGameUIMan->GetDialog("Win_Mainb"));
					CDlgSystem5* pHDlgSystem5 = dynamic_cast<CDlgSystem5*>(pGameUIMan->GetDialog("Win_Main5"));
					CDlgSystem5* pVDlgSystem5 = dynamic_cast<CDlgSystem5*>(pGameUIMan->GetDialog("Win_Main5b"));
					pHDlgSystem->ShowNewImg(true);
					pVDlgSystem->ShowNewImg(true);
					pHDlgSystem5->ShowNewImg(true);
					pVDlgSystem5->ShowNewImg(true);
				}
			}
		}
	}

	static CECShowNewSkillHelper s_showNewSkillHelper;
};

CECShowNewSkillHelper CECShowNewSkillHelper::s_showNewSkillHelper;