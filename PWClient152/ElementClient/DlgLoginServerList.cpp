// Filename	: DlgLoginServerList.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/4

#include "DlgLoginServerList.h"
#include "DlgCurrentServer.h"
#include "CSplit.h"
#include "EC_LoginUIMan.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "gameclient.h"
#include "EC_LoginSwitch.h"
#include "EC_UIManager.h"
#include "EC_ServerList.h"
#include "EC_CrossServer.h"
#include "EC_Split.h"
#include "EC_UIAnimation.h"
#include "EC_CommandLine.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgLoginServerList, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("sortname",		OnCommandSortName)
AUI_ON_COMMAND("sortspeed",		OnCommandSortSpeed)
AUI_ON_COMMAND("sortstats",		OnCommandSortStats)
AUI_ON_COMMAND("sortother",		OnCommandSortOther)
AUI_ON_COMMAND("query",         OnCommandQuery)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgLoginServerList, CDlgBase)

AUI_ON_EVENT("Lst_Server",	WM_LBUTTONDOWN,	OnEventSelect_LstServer)
AUI_ON_EVENT("Lst_Server",	WM_LBUTTONUP,	OnEventSelect_LstServer)
AUI_ON_EVENT("Lst_ServerGroup", WM_LBUTTONDOWN, OnEventSelect_LstServerGroup)

AUI_END_EVENT_MAP()

CDlgLoginServerList::CDlgLoginServerList()
: m_pDlgCurrentServer(NULL)
{
}

bool CDlgLoginServerList::Release()
{
	if (m_pDlgCurrentServer){
		delete m_pDlgCurrentServer;
		m_pDlgCurrentServer = NULL;
	}
	return CDlgBase::Release();
}

bool CDlgLoginServerList::OnInitDialog()
{
	m_pLst_Server = (PAUILISTBOX)GetDlgItem("Lst_Server");
	m_pLst_Server->ResetContent();
	m_pLst_ServerGroup = (PAUILISTBOX)GetDlgItem("Lst_ServerGroup");
	m_pLst_ServerGroup->ResetContent();

	int i=0;
	for (i = 0; i < CECServerList::Instance().GetGroupCount(); ++ i)
	{
		const CECServerList::GroupInfo &g = CECServerList::Instance().GetGroup(i);
		m_pLst_ServerGroup->AddString(g.group_name);
	}
	
	m_pDlgCurrentServer = new CDlgCurrentServer(this);
	return true;
}

void CDlgLoginServerList::OnShowDialog()
{	
	CECLoginUIMan *pLoginUIMan = GetLoginUIMan();

	PAUIDIALOG pDlgLogin = pLoginUIMan->GetDialog("Win_Login");
	pDlgLogin->SetEnable(false);
	if (pDlgLogin->IsShow()){
		pDlgLogin->Show(false);
	}
	PAUIDIALOG pDlgLoginServerListButton = pLoginUIMan->GetDialog("Win_LoginServerListButton");
	if (!pDlgLoginServerListButton->IsShow()){
		pDlgLoginServerListButton->Show(true);
	}
	PAUIDIALOG pDlgLoginVersion = pLoginUIMan->GetDialog("Win_LoginVersion");
	if (!pDlgLoginVersion->IsShow()){
		pDlgLoginVersion->Show(true);
	}

	bool selectGroup(true);
	if (GetGameRun()->IsFirstShow()){
		ACString strArea = CECCommandLine::GetArea();
		if (!strArea.IsEmpty() && SetServerGroup(strArea)){
			selectGroup = false;
		}
	}
	if (selectGroup){
		int iServer = CECServerList::Instance().GetSelected();	
		int group = CECServerList::Instance().FindGroup(iServer, true);
		if (group < 0){
			group = 0;		//	无法找到保存的服务器时, 显示第一个位置
		}
		SelectGroup(group);
	}

	CDlgBase::OnShowDialog();
}

void CDlgLoginServerList::OnHideDialog()
{
	CECLoginUIMan *pLoginUIMan = GetLoginUIMan();

	PAUIDIALOG pDlgLogin = pLoginUIMan->GetDialog("Win_Login");
	pLoginUIMan->BringWindowToTop(pDlgLogin);
	pDlgLogin->SetEnable(true);
	if (!pDlgLogin->IsShow()){
		pDlgLogin->Show(true);
	}
	pDlgLogin->ChangeFocus(pDlgLogin->GetDlgItem("DEFAULT_Txt_Account"));
	
	PAUIDIALOG pDlgLoginServerListButton = pLoginUIMan->GetDialog("Win_LoginServerListButton");
	if (pDlgLoginServerListButton->IsShow()){
		pDlgLoginServerListButton->Show(false);
	}

	CDlgBase::OnHideDialog();
}

void CDlgLoginServerList::OnTick()
{
	CECServerList::Instance().OnTick();	
	m_pDlgCurrentServer->OnTick();

	CECGameSession *pSession = g_pGame->GetGameSession();
	bool bCanChoose = false;
	int nSel = m_pLst_Server->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Server->GetCount() )
		bCanChoose = true;
	for(int i = 0; i < m_pLst_Server->GetCount(); i++ )
	{
		if( m_pLst_Server->GetItemData(i, COL_PINGTIME) == 99999999 )
		{
			int iServer = m_pLst_Server->GetItemData(i, COL_SERVERINDEX);
			CSplit s(m_pLst_Server->GetText(i));
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			DWORD dwStatus, dwCreateTime;
			unsigned char cExpRate;
			DWORD dwTime = g_pGame->GetGameSession()->GetPingTime(iServer, dwStatus, dwCreateTime, cExpRate);
			if( dwTime != 99999999 )
			{
				CECServerList::Instance().SetNeedPing(iServer, false);
				if( dwTime < 300 )
					m_pLst_Server->SetItemTextColor(i, 0xFF00FF7F, 1);
				else if( dwTime < 1000 )
					m_pLst_Server->SetItemTextColor(i, 0xFFFFFF7F, 1);
				else
					m_pLst_Server->SetItemTextColor(i, 0xFFFF0000, 1);
				unsigned char load = ((GNET::Attr *)&dwStatus)->load;
				if( load < 100 )
				{
					vec[2] = GetStringFromTable(222);
					m_pLst_Server->SetItemTextColor(i, 0xFF00FF7F, 2);
				}
				else if( load < 180 )
				{
					vec[2] = GetStringFromTable(223);
					m_pLst_Server->SetItemTextColor(i, 0xFFFFFF7F, 2);
				}
				else
				{
					vec[2] = GetStringFromTable(224);
					m_pLst_Server->SetItemTextColor(i, 0xFFFF0000, 2);
				}

				ACString strText;
				strText.Format(_AL("%dms"), dwTime);
				vec[1] = strText;
				ACString strHint = _AL("");
				if( cExpRate == 20 &&
					((GNET::Attr *)&dwStatus)->doubleObject == 1 &&
					((GNET::Attr *)&dwStatus)->doubleMoney == 1 &&
					((GNET::Attr *)&dwStatus)->doubleSP == 1 )
				{
					strHint += GetStringFromTable(263);
					strHint += GetStringFromTable(264);
					strHint += GetStringFromTable(265);
					strHint += GetStringFromTable(266);
				}
				else
				{
					if (cExpRate == 20)
					{
						// 双倍经验
						strHint += GetStringFromTable(263);
					}
					else if (cExpRate > 10 && cExpRate <= 100)
					{
						// 其它倍数经验
						ACString strFormat;
						if (cExpRate % 10)
						{
							// 带小数倍数
							strFormat = _AL("%.1f");
							strFormat += GetStringFromTable(267);
							strText.Format(strFormat, cExpRate*0.1f);
						}
						else
						{
							// 整数倍数
							strFormat = _AL("%d");
							strFormat += GetStringFromTable(267);
							strText.Format(strFormat, cExpRate/10);
						}
						strHint += strText;
					}
					if( ((GNET::Attr *)&dwStatus)->doubleObject == 1 )
					{
						strHint += GetStringFromTable(264);
					}
					if( ((GNET::Attr *)&dwStatus)->doubleMoney == 1 )
					{
						strHint += GetStringFromTable(265);
					}
					if( ((GNET::Attr *)&dwStatus)->doubleSP == 1 )
					{
						strHint += GetStringFromTable(266);
					}
				}

				// show detailed string in list, append "..." if hint string is too long
				#define SERVER_HINT_MAX 16
				ACHAR szOther[SERVER_HINT_MAX + 1] = {0};
				int pos = 0;
				while(pos < SERVER_HINT_MAX && pos < strHint.GetLength())
				{
					// replace the '\r' to ','
					szOther[pos] = (strHint[pos] == _AL('\r') && pos != strHint.GetLength() - 1) ? 
						_AL(',') : strHint[pos];
					++pos;
				}
				if(strHint.GetLength() > SERVER_HINT_MAX)
				{
					szOther[SERVER_HINT_MAX - 3] = 0;
					vec[3].Format(_AL("%s..."), szOther);
				}
				else
				{
					vec[3] = szOther;
				}
				
				if( strHint.GetLength() > 0 )
					strHint.CutRight(1);
				strText.Format(_AL("%s\t%s\t%s\t%s"), vec[0], vec[1], vec[2], vec[3]);
				m_pLst_Server->SetText(i, strText);
				m_pLst_Server->SetItemData(i, dwTime, COL_PINGTIME);
				m_pLst_Server->SetItemHint(i, strHint);
			}
		}
	}
	PAUIOBJECT pObjChoose = m_pAUIManager->GetDialog("Win_LoginServerListButton")->GetDlgItem("Btn_Choose");
	pObjChoose->Enable(bCanChoose);
}

void CDlgLoginServerList::OnEventSelect_LstServerGroup(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	SelectGroup(m_pLst_ServerGroup->GetCurSel());
}

void CDlgLoginServerList::SelectGroup(int nGroup)
{
	if (nGroup < 0 || nGroup >= m_pLst_ServerGroup->GetCount()){
		return;
	}
	if (m_pLst_ServerGroup->GetCurSel() != nGroup){
		m_pLst_ServerGroup->SetCurSel(nGroup);
	}
	
	m_pLst_Server->ResetContent();
	
	int iSelected = CECServerList::Instance().GetSelected();
	int nSel = -1;
	const CECServerList::GroupInfo &group = CECServerList::Instance().GetGroup(nGroup);
	for (int i(0); i < (int)group.server_vec.size(); i++)
	{
		int iServer = group.server_vec[i];
		const CECServerList::ServerInfo& serverInfo = CECServerList::Instance().GetServer(iServer);
		ACString strText;		
		strText.Format(_AL("%s\t---\t---\t "), serverInfo.server_name);
		m_pLst_Server->AddString(strText);
		int itemIndex = m_pLst_Server->GetCount() - 1;
		m_pLst_Server->SetItemData(itemIndex, iServer,			COL_SERVERINDEX);
		m_pLst_Server->SetItemData(itemIndex, serverInfo.line,	COL_LINE);
		m_pLst_Server->SetItemData(itemIndex, 99999999,			COL_PINGTIME);
		m_pLst_Server->SetItemTextColor(itemIndex, 0xFFFFFFFF, 1);
		if (iServer == iSelected){
			nSel = i;
			m_pLst_Server->SetItemTextColor(itemIndex, 0xFFFFCB4A, 0);
		}else{
			m_pLst_Server->SetItemTextColor(itemIndex, 0xFFFFFFFF, 0);
		}		
		CECServerList::Instance().SetNeedPing(iServer, true);
	}
	if( nSel >= 0 ){
		m_pLst_Server->BringItemToTop(nSel);
		m_pLst_Server->SetCurSel(0);
	}
}

void CDlgLoginServerList::OnCommandCancel(const char* szCommand)
{
	Show(false);
	if (CECServerList::Instance().CanApply()){
		GetLoginUIMan()->DefaultLogin();
	}
}

void CDlgLoginServerList::OnCommandConfirm(const char* szCommand)
{
	int nSel = m_pLst_Server->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Server->GetCount() )
	{
		int iServer = m_pLst_Server->GetItemData(nSel, COL_SERVERINDEX);
		CECServerList::Instance().SelectServer(iServer);
		Show(false);
		GetLoginUIMan()->DefaultLogin();
	}
}
void CDlgLoginServerList::OnCommandSortName(const char* szCommand)
{
	m_bSortName = !m_bSortName;
	if( m_bSortName )
		m_pLst_Server->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_STRING, 0);
	else
		m_pLst_Server->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_STRING, 0);
}

void CDlgLoginServerList::OnCommandSortSpeed(const char* szCommand)
{
	m_bSortSpeed = !m_bSortSpeed;
	if( m_bSortSpeed )
		m_pLst_Server->SortItemsByData(AUILISTBOX_SORT_ASCENT, AUILISTBOX_DATASORTTYPE_DWORD, 0);
	else
		m_pLst_Server->SortItemsByData(AUILISTBOX_SORT_DESCENT, AUILISTBOX_DATASORTTYPE_DWORD, 0);
}

void CDlgLoginServerList::OnCommandSortStats(const char* szCommand)
{
	m_bSortStats = !m_bSortStats;
	if( m_bSortStats )
		m_pLst_Server->SortItemsByData(AUILISTBOX_SORT_ASCENT, AUILISTBOX_DATASORTTYPE_DWORD, 2);
	else
		m_pLst_Server->SortItemsByData(AUILISTBOX_SORT_DESCENT, AUILISTBOX_DATASORTTYPE_DWORD, 2);
}

void CDlgLoginServerList::OnCommandSortOther(const char* szCommand)
{
	m_bSortOther = !m_bSortOther;
	if( m_bSortOther )
		m_pLst_Server->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_STRING, 3);
	else
		m_pLst_Server->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_STRING, 3);
}

void CDlgLoginServerList::OnCommandQuery(const char *szCommand)
{
	PAUIDIALOG pDlgFindServer = m_pAUIManager->GetDialog("Win_FindServer");
	pDlgFindServer->Show(true, true);
}

void CDlgLoginServerList::OnEventSelect_LstServer(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
}

bool CDlgLoginServerList::SetServerGroup(const ACString &strGroup)
{
	bool bRet(false);

	if (!strGroup.IsEmpty())
	{
		int nGroup = -1;
		for (int i = 0; i < m_pLst_ServerGroup->GetCount(); ++ i)
		{
			if (strGroup == m_pLst_ServerGroup->GetText(i)){
				nGroup = i;
				break;
			}
		}
		if (nGroup >= 0){
			SelectGroup(nGroup);
			bRet = true;
		}
	}

	return bRet;
}

void CDlgLoginServerList::SelectServerByName(ACString strServer)
{
	// Switch to the right group with given server name
	//
	int iServer = CECServerList::Instance().FindServerByName(strServer, 0);
	if (iServer < 0){
		return;
	}
	int nGroup = CECServerList::Instance().FindGroup(iServer, true);
	SelectGroup(nGroup);

	// Select item with given server name
	CECGameSession *pSession = g_pGame->GetGameSession();
	int nSel = -1;
	for(int i = 0; i < m_pLst_Server->GetCount(); i++ )
	{
		CSplit s(m_pLst_Server->GetText(i));
		CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
		if( vec[0] == strServer )
		{
			m_pLst_Server->SetItemTextColor(i, 0xFFFFCB4A, 0);
			nSel = i;
		}
		else
			m_pLst_Server->SetItemTextColor(i, 0xFFFFFFFF, 0);
		m_pLst_Server->SetItemData(i, 99999999, COL_PINGTIME);
		ACString strText;
		strText.Format(_AL("%s\t---\t---\t "), vec[0]);
		m_pLst_Server->SetItemTextColor(i, 0xFFFFFFFF, 1);
		m_pLst_Server->SetText(i, strText);
	}
	m_pLst_Server->SetCurSel(nSel);
}

//	CDlgLoginServerListButton

AUI_BEGIN_COMMAND_MAP(CDlgLoginServerListButton, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("query",         OnCommandQuery)
AUI_END_COMMAND_MAP()

void CDlgLoginServerListButton::OnCommandCancel(const char* szCommand)
{
	PAUIDIALOG pDlgLoginServerList = m_pAUIManager->GetDialog("Win_LoginServerList");
	pDlgLoginServerList->OnCommand("IDCANCEL");
}

void CDlgLoginServerListButton::OnCommandConfirm(const char* szCommand)
{
	PAUIDIALOG pDlgLoginServerList = m_pAUIManager->GetDialog("Win_LoginServerList");
	pDlgLoginServerList->OnCommand("confirm");
}

void CDlgLoginServerListButton::OnCommandQuery(const char *szCommand)
{
	PAUIDIALOG pDlgLoginServerList = m_pAUIManager->GetDialog("Win_LoginServerList");
	pDlgLoginServerList->OnCommand("query");
}