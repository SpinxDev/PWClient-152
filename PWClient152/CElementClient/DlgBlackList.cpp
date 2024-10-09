// Filename	: DlgBlackList.cpp
// Creator	: zhangyitian
// Date		: 2014/07/21
// 将原先的CDlgSettingBlackList略作修改，删除了一些内容

#include "DlgBlackList.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_ManPlayer.h"
#include "EC_CrossServer.h"
#include "EC_TimeSafeChecker.h"

#include "AUIStillImageButton.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"
#include "AUICheckBox.h"


#define new A_DEBUG_NEW

#define MAX_PLAYER_LEVEL 105
#define MAX_BLOCKED_PLAYER 200

AUI_BEGIN_COMMAND_MAP(CDlgBlackList, CDlgBase)

AUI_ON_COMMAND("add", OnCommandAdd)
AUI_ON_COMMAND("remove", OnCommandRemove)
AUI_ON_COMMAND("choosename", OnCommandChooseName)
AUI_ON_COMMAND("Btn_Apply", OnCommandApply)

AUI_END_COMMAND_MAP()

CDlgBlackList::CDlgBlackList()
{
	m_pEbxName = NULL;
	m_pLbxBlackList = NULL;
	m_pLevelBlock = NULL;
	m_pBtnApply = NULL;
}

CDlgBlackList::~CDlgBlackList()
{

}


bool CDlgBlackList::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	DisableForCrossServer("Btn_Add");
	DisableForCrossServer("Btn_Remove");

	UpdateView();
	
	return true;
}

void CDlgBlackList::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	PAUIOBJECT pBtnAdd = GetDlgItem("Btn_Add");
	if(pBtnAdd)
	{
		ACString strHint;
		strHint.Format(GetStringFromTable(309), MAX_BLOCKED_PLAYER, EC_BLACKLIST_LEN);
		pBtnAdd->SetHint(strHint);
	}
	// init diaplay
	UpdateView();
}

void CDlgBlackList::OnHideDialog() {
	SaveLevelBlock();
}

void CDlgBlackList::DoDataExchange(bool bSave)
{	
	CDlgBase::DoDataExchange(bSave);
	DDX_Control("Txt_Name", m_pEbxName);
	DDX_Control("Lst_Blacklist", m_pLbxBlackList);
	DDX_EditBox(bSave, "Txt_Name", m_strName);
	DDX_Control("Txt_Level", m_pLevelBlock);
	DDX_Control("Btn_Apply", m_pBtnApply);
}

void CDlgBlackList::OnCommandAdd(const char *szCommand)
{
	if (CECCrossServer::Instance().IsOnSpecialServer())
		return;

	UpdateData(true);
	if( m_strName == _AL("") )
		return;

	int nCount = m_pLbxBlackList->GetCount();
	// blacklist length limit
	if( nCount + m_vecBlackCache.size() >= MAX_BLOCKED_PLAYER )
	{
		AUIDialog *pMsgBox = NULL;
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(262), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
		return;
	}

	// avoid duplication 
	bool bDuplicate = false;
	for (int i = 0; i < nCount; i++ )
	{
		AUIDialog *pMsgBox = NULL;
		ACString strItem = m_pLbxBlackList->GetText(i);
		if( 0 == m_strName.CompareNoCase(strItem) )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(260), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
	}

	// get player id if he is online and valid
	int idRole = GetGameRun()->GetPlayerID(m_strName);
	if( idRole > 0 )	// online and valid
	{
		int col = m_pLbxBlackList->GetCount();
		m_pLbxBlackList->AddString(m_strName);
		m_pLbxBlackList->SetItemData(col, idRole);
		m_pLbxBlackList->SetItemTextColor(col, col < EC_BLACKLIST_LEN ? A3DCOLORRGB(255,255,255) : A3DCOLORRGB(192,192,192));
		g_pGame->GetConfigs()->SetBlockedName(idRole, &m_strName);
	}
	else // offline or invalid
	{
		GetGameSession()->GetPlayerIDByName(m_strName, 0);
		BLACK_CACHE bc;
		bc.dwTime = GetTickCount();
		bc.strName = m_strName;
		m_vecBlackCache.push_back(bc);
	}
	
	// set control state
	m_strName.Empty();

	m_pEbxName->SetFocus(true);
}

void CDlgBlackList::OnCommandRemove(const char *szCommand)
{
	if (CECCrossServer::Instance().IsOnSpecialServer())
		return;

	int nCount = m_pLbxBlackList->GetCount();
	int nCurSel = m_pLbxBlackList->GetCurSel();
	
	if( nCount > 0 && nCurSel >= 0 && nCurSel < nCount )
	{
		int idRole = m_pLbxBlackList->GetItemData(nCurSel);
		g_pGame->GetConfigs()->SetBlockedName(idRole, NULL);
		m_pLbxBlackList->DeleteString(nCurSel);
		for(int col=nCurSel;col<min(m_pLbxBlackList->GetCount(), EC_BLACKLIST_LEN); col++)
		{
			m_pLbxBlackList->SetItemTextColor(col, A3DCOLORRGB(255,255,255));
		}
	}
}

void CDlgBlackList::OnCommandChooseName(const char *szCommand)
{
	int nCurSel = m_pLbxBlackList->GetCurSel();
	m_strName = m_pLbxBlackList->GetText(nCurSel);
	UpdateData(false);
}

void CDlgBlackList::OnTick(void)
{
	CDlgBase::OnTick();

	if( m_vecBlackCache.size() == 0 || !IsShow())
		return;
	
	CECGameRun *pGameRun = g_pGame->GetGameRun();
	DWORD dwTick = GetTickCount();
	
	for(int i = 0; i < (int)m_vecBlackCache.size(); i++ )
	{
		int idRole = pGameRun->GetPlayerID(m_vecBlackCache[i].strName);
		if( idRole > 0 )
		{
			int col = m_pLbxBlackList->GetCount();
			m_pLbxBlackList->AddString(m_vecBlackCache[i].strName);
			m_pLbxBlackList->SetItemData(col, idRole);
			m_pLbxBlackList->SetItemTextColor(col, col < EC_BLACKLIST_LEN ? A3DCOLORRGB(255,255,255) : A3DCOLORRGB(192,192,192));
			
			g_pGame->GetConfigs()->SetBlockedName(idRole, &m_vecBlackCache[i].strName);
			m_vecBlackCache.erase(m_vecBlackCache.begin() + i);
			i--;
		}
		else if( CECTimeSafeChecker::ElapsedTime(dwTick, m_vecBlackCache[i].dwTime) >= 10 * 1000 )
		{
			ACString strMsg;
			strMsg.Format(GetGameUIMan()->GetStringFromTable(261), m_vecBlackCache[i].strName);
			GetGameUIMan()->AddChatMessage(strMsg, GP_CHAT_MISC);
			m_vecBlackCache.erase(m_vecBlackCache.begin() + i);
			i--;
		}
	}
}

void CDlgBlackList::OnCommandApply(const char * szCommand)
{
	SaveLevelBlock();
}

void CDlgBlackList::UpdateView()
{
	m_pLbxBlackList->ResetContent();
	m_vecBlackCache.clear();
	CECGameRun *pGameRun = GetGame()->GetGameRun();

	const abase::vector<int>& blocked = GetGame()->GetConfigs()->GetBlockedList();
	for (unsigned int i = 0; i < blocked.size(); i++)
	{
		int idRole = blocked[i];
		int col = m_pLbxBlackList->GetCount();
		m_pLbxBlackList->AddString(pGameRun->GetPlayerName(idRole, true));
		m_pLbxBlackList->SetItemData(col, idRole);
		m_pLbxBlackList->SetItemTextColor(col, col < EC_BLACKLIST_LEN ? A3DCOLORRGB(255,255,255) : A3DCOLORRGB(192,192,192));
	}
	
	const EC_BLACKLIST_SETTING& bs = GetGame()->GetConfigs()->GetBlackListSettings();
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), bs.levelBlock);
	m_pLevelBlock->SetText(szText);
	
	m_strName.Empty();
}

void CDlgBlackList::SaveLevelBlock() {
	EC_BLACKLIST_SETTING bs = GetGame()->GetConfigs()->GetBlackListSettings();
	
	int levelBlock = a_atoi(m_pLevelBlock->GetText());
	// check the input parameters
	if(levelBlock > MAX_PLAYER_LEVEL) levelBlock = MAX_PLAYER_LEVEL;
	else if(levelBlock < 0) levelBlock = 0;
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), levelBlock);
	m_pLevelBlock->SetText(szText);
	bs.levelBlock = levelBlock;

	GetGame()->GetConfigs()->SetBlackListSettings(bs);	
}