// Filename	: DlgGuildMan.cpp
// Creator	: Tom Zhou
// Date		: October 25, 2005

#include "DlgGuildMan.h"
#include "DlgInputName.h"
#include "DlgProclaim.h"
#include "DlgGuildDiplomacyMan.h"
#include "DlgMailWrite.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_Faction.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "CSplit.h"
#include "AUIEditBox.h"
#include "Network\\ids.hxx"
#include "GT/gt_overlay.h"
#include "EC_Configs.h"
#include "EC_Utility.h"
#include "EC_TimeSafeChecker.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildMan, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("sortname",		OnCommand_sortname)
AUI_ON_COMMAND("sortjob",		OnCommand_sortjob)
AUI_ON_COMMAND("sorttitle",		OnCommand_sorttitle)
AUI_ON_COMMAND("sortlevel",		OnCommand_sortlevel)
AUI_ON_COMMAND("sortprof",		OnCommand_sortprof)
AUI_ON_COMMAND("sortcontrib",	OnCommand_sortcontrib)
AUI_ON_COMMAND("sortol",		OnCommand_sortol)
AUI_ON_COMMAND("Edit",			OnCommand_Edit)
AUI_ON_COMMAND("gquit",			OnCommand_gquit)
AUI_ON_COMMAND("gpass",			OnCommand_gpass)
AUI_ON_COMMAND("gdemiss",		OnCommand_gdemiss)
AUI_ON_COMMAND("gpromote",		OnCommand_gpromote)
AUI_ON_COMMAND("gdemote",		OnCommand_gdemote)
AUI_ON_COMMAND("gkick",			OnCommand_gkick)
AUI_ON_COMMAND("gtitle",		OnCommand_gtitle)
AUI_ON_COMMAND("ginvite",		OnCommand_ginvite)
AUI_ON_COMMAND("refresh",		OnCommand_refresh)
AUI_ON_COMMAND("find",          OnCommand_find)
AUI_ON_COMMAND("Btn_Diplomacy",	OnCommand_Diplomacy)
AUI_ON_COMMAND("Btn_GTalk",		OnCommandGTTalk)
AUI_ON_COMMAND("Btn_SendMassMail",OnCommand_SendMassMail)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGuildMan, CDlgBase)

AUI_ON_EVENT("List_Member",	WM_RBUTTONUP,	OnEventRButtonUp)

AUI_END_EVENT_MAP()

CDlgGuildMan::CDlgGuildMan()
{
	m_dwLastRefreshTime = 0;
	m_nSortMode = SORT_BY_ORDER;
	m_bGTWaiting = false;
}

CDlgGuildMan::~CDlgGuildMan()
{
}

void CDlgGuildMan::SetLastRefreshTime(DWORD dwTime)
{
	m_dwLastRefreshTime = dwTime;
}

void CDlgGuildMan::OnCommand_confirm(const char * szCommand)
{
}

void CDlgGuildMan::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	POINT ptPos = pObj->GetPos();
	int nCurSel = m_pList_Member->GetCurSel();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	if( nCurSel >= 0 && nCurSel < m_pList_Member->GetCount() &&
		m_pList_Member->GetHitArea(x, y) == AUILISTBOX_RECT_TEXT )
	{
		int idPlayer = m_pList_Member->GetItemData(nCurSel);

		x = GET_X_LPARAM(lParam) - p->X;
		y = GET_Y_LPARAM(lParam) - p->Y;
		GetGameUIMan()->PopupPlayerContextMenu(idPlayer, x, y);
	}
}

bool CDlgGuildMan::OnInitDialog()
{
	m_pList_Member = (PAUILISTBOX)GetDlgItem("List_Member");
	m_pTxt_GuildName = (PAUILABEL)GetDlgItem("Txt_GuildName");
	m_pTxt_GuildName2 = (PAUILABEL)GetDlgItem("Txt_GuildName2");
	m_pTxt_GuildLevel = (PAUILABEL)GetDlgItem("Txt_GuildLevel");
	m_pTxt_PplCap = (PAUILABEL)GetDlgItem("Txt_PplCap");
	m_pTxt_PplOnline = (PAUILABEL)GetDlgItem("Txt_PplOnline");
	m_pTxt_Message = GetDlgItem("Txt_Message");
	m_pBtn_GuildPass = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildPass");
	m_pBtn_GuildDemission = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildDemission");
	m_pBtn_GuildPromote = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildPromote");
	m_pBtn_GuildDemote = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildDemote");
	m_pBtn_GuildKick = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildKick");
	m_pBtn_GuildTitle = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildTitle");
	m_pBtn_GuildQuit = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildQuit");
	m_pBtn_GuildInvite = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildInvite");
	m_pBtn_Edit = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Edit");
	m_pBtn_Refresh = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Refresh");
	m_pBtn_Diplomacy = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Diplomacy");
	m_pImg_GuildIcon = (PAUIIMAGEPICTURE)GetDlgItem("Img_GuildIcon");
	m_pBtn_SendMassMail = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SendMassMail");
	
	if(m_pTxt_GuildName) m_pTxt_GuildName->SetText(_AL(""));
	if(m_pTxt_GuildName2) m_pTxt_GuildName2->SetText(_AL(""));

	return true;
}

void CDlgGuildMan::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgGuildMan::OnCommand_sortname(const char * szCommand)
{
	m_nSortMode = SORT_BY_NAME;
	BuildFactionList();
}

void CDlgGuildMan::OnCommand_sortjob(const char * szCommand)
{
	m_nSortMode = SORT_BY_JOB;
	BuildFactionList();
}

void CDlgGuildMan::OnCommand_sorttitle(const char * szCommand)
{
	m_nSortMode = SORT_BY_TITLE;
	BuildFactionList();
}

void CDlgGuildMan::OnCommand_sortlevel(const char * szCommand)
{
	m_nSortMode = SORT_BY_LEVEL;
	BuildFactionList();
}

void CDlgGuildMan::OnCommand_sortprof(const char * szCommand)
{
	m_nSortMode = SORT_BY_PROFESSION;
	BuildFactionList();
}

void CDlgGuildMan::OnCommand_sortol(const char * szCommand)
{
	m_nSortMode = SORT_BY_ONLINE;
	BuildFactionList();
}

void CDlgGuildMan::OnCommand_sortcontrib(const char * szCommand)
{
	m_nSortMode = SORT_BY_CONTRIB;
	BuildFactionList();
}

void CDlgGuildMan::OnCommand_Edit(const char * szCommand)
{
	ACString strText = m_pTxt_Message->GetText();
	GetGameUIMan()->m_pDlgProclaim->Show(true);
	GetGameUIMan()->m_pDlgProclaim->m_pDEFAULT_Txt_Input->SetText(strText);
}

void CDlgGuildMan::OnCommand_gquit(const char * szCommand)
{
	GetGameUIMan()->MessageBox("Faction_Quit", GetStringFromTable(590),
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgGuildMan::OnCommand_gpass(const char * szCommand)
{
	int nCurSel = m_pList_Member->GetCurSel();
	if( nCurSel < 0 || nCurSel >= m_pList_Member->GetCount() ) return;

	CSplit s(m_pList_Member->GetText(nCurSel));
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));

	ACString strMsg;
	PAUIDIALOG pMsgBox;
	strMsg.Format(GetStringFromTable(584), vec[0]);
	GetGameUIMan()->MessageBox("Faction_Pass", strMsg,
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(m_pList_Member->GetItemData(nCurSel));
}

void CDlgGuildMan::OnCommand_gdemiss(const char * szCommand)
{
	GetGameUIMan()->MessageBox("Faction_Demiss", GetStringFromTable(585),
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgGuildMan::OnCommand_gpromote(const char * szCommand)
{
	int nCurSel = m_pList_Member->GetCurSel();
	if( nCurSel < 0 || nCurSel >= m_pList_Member->GetCount() ) return;

	CSplit s(m_pList_Member->GetText(nCurSel));
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));

	ACString strMsg;
	PAUIDIALOG pMsgBox;
	strMsg.Format(GetStringFromTable(586), vec[0]);
	GetGameUIMan()->MessageBox("Faction_Promote", strMsg,
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(m_pList_Member->GetItemData(nCurSel));
}

void CDlgGuildMan::OnCommand_gdemote(const char * szCommand)
{
	int nCurSel = m_pList_Member->GetCurSel();
	if( nCurSel < 0 || nCurSel >= m_pList_Member->GetCount() ) return;

	CSplit s(m_pList_Member->GetText(nCurSel));
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));

	ACString strMsg;
	PAUIDIALOG pMsgBox;
	strMsg.Format(GetStringFromTable(587), vec[0]);
	GetGameUIMan()->MessageBox("Faction_Demote", strMsg,
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(m_pList_Member->GetItemData(nCurSel));
}

void CDlgGuildMan::OnCommand_gkick(const char * szCommand)
{
	int nCurSel = m_pList_Member->GetCurSel();
	if( nCurSel < 0 || nCurSel >= m_pList_Member->GetCount() ) return;

	CSplit s(m_pList_Member->GetText(nCurSel));
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	int idPlayer = m_pList_Member->GetItemData(nCurSel);
	Faction_Mem_Info *pMem = pFMan->GetMember(idPlayer);
	if (!pMem){
		ASSERT(false);
		return;
	}

	ACString strMsg;
	PAUIDIALOG pMsgBox;
	strMsg.Format(GetStringFromTable(pMem->IsDelayExpel() ? 10382 : 588), vec[0]);
	GetGameUIMan()->MessageBox(pMem->IsDelayExpel() ? "Faction_CancelKick" : "Faction_Kick", strMsg,
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(idPlayer);
}

void CDlgGuildMan::OnCommand_gtitle(const char * szCommand)
{
	int nCurSel = m_pList_Member->GetCurSel();
	if( nCurSel < 0 || nCurSel >= m_pList_Member->GetCount() ) return;

	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_FACTION_TITLE, NULL, 8, (void *)m_pList_Member->GetItemData(nCurSel),"int");
}

void CDlgGuildMan::OnCommand_ginvite(const char * szCommand)
{
	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_ADD_GUILD);
}

void CDlgGuildMan::OnCommand_refresh(const char * szCommand)
{
	int idFaction = GetHostPlayer()->GetFactionID();
	if (idFaction == 0){
		return;
	}
	m_dwLastRefreshTime = GetTickCount();
	CECFactionMan *pFactionMan = GetGame()->GetFactionMan();
	pFactionMan->RefreshMemList();
}

void CDlgGuildMan::OnTick()
{
	if( GetHostPlayer()->GetFactionID() <= 0 )
	{
		Show(false);
		return;
	}
	
	CECFactionMan *pFMan = GetGame()->GetFactionMan();

	int nCurSel = m_pList_Member->GetCurSel();
	int idFRole = GetHostPlayer()->GetFRoleID();
	int idPlayer = -1, idFRoleU = GNET::_R_MASTER;

	if( nCurSel >= 0 && nCurSel < m_pList_Member->GetCount() )
	{
		idPlayer = m_pList_Member->GetItemData(nCurSel);
		idFRoleU = m_pList_Member->GetItemData(nCurSel, 1);
	}
	
	bool bDelayExpel(false);
	if (idPlayer > 0){
		Faction_Mem_Info *pMem = NULL;
		pMem = pFMan->GetMember(idPlayer);
		bDelayExpel = pMem->IsDelayExpel();
	}

	if( idPlayer <= 0 || idPlayer == GetHostPlayer()->GetCharacterID() )
	{
		m_pBtn_GuildPass->Enable(false);
		m_pBtn_GuildPromote->Enable(false);
		m_pBtn_GuildDemote->Enable(false);
		m_pBtn_GuildKick->Enable(false);
	}
	else
	{
		if( idFRole == GNET::_R_MASTER && idFRoleU == GNET::_R_VICEMASTER && !bDelayExpel)
			m_pBtn_GuildPass->Enable(true);
		else
			m_pBtn_GuildPass->Enable(false);

		if( idFRoleU - idFRole >= 2 &&
			(idFRole == GNET::_R_MASTER ||
			idFRole == GNET::_R_VICEMASTER ||
			idFRole == GNET::_R_BODYGUARD) &&
			!bDelayExpel)
		{
			m_pBtn_GuildPromote->Enable(true);
		}
		else
		{
			m_pBtn_GuildPromote->Enable(false);
		}

		if( idFRoleU > idFRole &&
			idFRoleU != GNET::_R_MEMBER &&
			(idFRole == GNET::_R_MASTER ||
			idFRole == GNET::_R_VICEMASTER ||
			idFRole == GNET::_R_BODYGUARD) &&
			!bDelayExpel)
		{
			m_pBtn_GuildDemote->Enable(true);
		}
		else
			m_pBtn_GuildDemote->Enable(false);

		if( idFRoleU == GNET::_R_MEMBER && 
			( idFRole == GNET::_R_MASTER || idFRole == GNET::_R_VICEMASTER) )
		{
			m_pBtn_GuildKick->Enable(true);
			Faction_Mem_Info *pMem = pFMan->GetMember(idPlayer);
			int idString = pMem->IsDelayExpel() ? 10381 : 10380;
			m_pBtn_GuildKick->SetText(GetStringFromTable(idString));
		}
		else
			m_pBtn_GuildKick->Enable(false);

		if( (idFRole == GNET::_R_MASTER || idFRole == GNET::_R_VICEMASTER) && !bDelayExpel )
			m_pBtn_GuildTitle->Enable(true);
		else
			m_pBtn_GuildTitle->Enable(false);
	}

	if( idFRole == GNET::_R_MEMBER)
		m_pBtn_GuildInvite->Enable(false);
	else
		m_pBtn_GuildInvite->Enable(true);
	
	if( idFRole == GNET::_R_VICEMASTER ||
		idFRole == GNET::_R_BODYGUARD ||
		idFRole == GNET::_R_POINEER )
	{
		m_pBtn_GuildDemission->Enable(true);
	}
	else
		m_pBtn_GuildDemission->Enable(false);

	if( idFRole == GNET::_R_MEMBER )
		m_pBtn_GuildQuit->Enable(true);
	else
		m_pBtn_GuildQuit->Enable(false);

	if( idFRole == GNET::_R_MASTER || idFRole == GNET::_R_VICEMASTER )
		m_pBtn_Edit->Enable(true);
	else
		m_pBtn_Edit->Enable(false);

	ACString strText;
	int idFaction = GetHostPlayer()->GetFactionID();
	Faction_Info *pInfo = pFMan->GetFaction(idFaction);

	if( pInfo )
	{
		m_pTxt_GuildName->SetText(pInfo->GetName());
		m_pTxt_GuildName2->SetText(pInfo->GetName());

		strText.Format(_AL("%d"), pInfo->GetLevel() + 1);
		m_pTxt_GuildLevel->SetText(strText);

		m_pTxt_PplCap->SetText(GetStringFromTable(1320 + pInfo->GetLevel()));
	}
	if( 0 != a_strcmp(pFMan->GetProclaim(), m_pTxt_Message->GetText()) )
		m_pTxt_Message->SetText(pFMan->GetProclaim());

	if( CECTimeSafeChecker::ElapsedTimeFor(m_dwLastRefreshTime) >= 30 * 1000 )
		m_pBtn_Refresh->Show(true);
	else
		m_pBtn_Refresh->Show(false);

	m_pBtn_Diplomacy->Enable(true);

	if (m_pBtn_SendMassMail){
		m_pBtn_SendMassMail->Enable(!pFMan->GetMemList().empty() && (idFRole == GNET::_R_MASTER || idFRole == GNET::_R_VICEMASTER || idFRole == GNET::_R_BODYGUARD));
	}
}

void CDlgGuildMan::BuildFactionList()
{
	int idPlayer = 0;
	int nCurSel = m_pList_Member->GetCurSel();
	if( nCurSel >= 0 && nCurSel < m_pList_Member->GetCount() )
		idPlayer = m_pList_Member->GetItemData(nCurSel);

	ACString strText;
	int i, nNumOnlines = 0;
	FactionMemList &fml = GetGame()->GetFactionMan()->GetMemList();

	if( m_nSortMode != SORT_BY_ORDER )
	{
		bool bExchanged;
		Faction_Mem_Info *pTemp;
		
		static int s_profOrder[NUM_PROFESSION] = {0, 1, 7, 2, 3, 6, 4, 5, 8, 9, 10, 11};  // Map profession to sort order

		do
		{
			bExchanged = false;
			for( i = 0; i < (int)fml.size() - 1; i++ )
			{
				if( (m_nSortMode == SORT_BY_NAME &&
					a_stricmp(fml[i]->GetName(), fml[i + 1]->GetName()) > 0) ||
					(m_nSortMode == SORT_BY_JOB &&
					fml[i]->GetFRoleID() > fml[i + 1]->GetFRoleID()) ||
					(m_nSortMode == SORT_BY_TITLE &&
					a_stricmp(fml[i]->GetNickName(), fml[i + 1]->GetNickName()) > 0) ||
					(m_nSortMode == SORT_BY_LEVEL &&
					fml[i]->GetLev() < fml[i + 1]->GetLev()) ||
					(m_nSortMode == SORT_BY_PROFESSION &&
					s_profOrder[fml[i]->GetProf()] > s_profOrder[fml[i + 1]->GetProf()]) ||
					(m_nSortMode == SORT_BY_ONLINE &&
					fml[i]->IsOnline() < fml[i + 1]->IsOnline()) ||
					(m_nSortMode == SORT_BY_CONTRIB &&
					fml[i]->GetContrib() < fml[i + 1]->GetContrib()))
				{
					pTemp = fml[i];
					fml[i] = fml[i + 1];
					fml[i + 1] = pTemp;
					bExchanged = true;
				}
			}
		}
		while( bExchanged );
	}

	const ACHAR *szHintFormat = GetStringFromTable(11230);
	m_pList_Member->ResetContent();
	for( i = 0; i < (int)fml.size(); i++ )
	{
		const Faction_Mem_Info *pMem = fml[i];
		strText.Format(_AL("%s\t%s\t%s\t%d\t%s\t%s\t%d"),
			pMem->GetName(), GetStringFromTable(1300 + pMem->GetFRoleID()),
			a_strlen(pMem->GetNickName()) > 0 ? pMem->GetNickName() : _AL(" "),
			pMem->GetLev(), GetGameRun()->GetProfName(pMem->GetProf()),
			GetStringFromTable(1310 + pMem->IsOnline()),
			pMem->GetContrib());
		m_pList_Member->AddString(strText);
		m_pList_Member->SetItemData(i, pMem->GetID());
		m_pList_Member->SetItemData(i, pMem->GetFRoleID(), 1);
		if (pMem->IsDelayExpel())
			m_pList_Member->SetItemTextColor(i, A3DCOLORRGB(255, 0, 0));

		int reincarnation_times = pMem->GetReincarnationTimes();
		const ACHAR *szReincarnation = reincarnation_times > 0 ? GetStringFromTable(10800+reincarnation_times) : GetStringFromTable(10799);
		const ACHAR *szGender = GetStringFromTable(8650+pMem->GetGender());

		time_t loginTime = pMem->GetLoginTime();
		struct tm t = *gmtime(&loginTime);
		ACString strTime;
		strTime.Format(GetStringFromTable(10629), t.tm_year+1900, t.tm_mon+1, t.tm_mday);

		strText.Format(szHintFormat, szReincarnation, szGender, pMem->GetReputation(), strTime);
		m_pList_Member->SetItemColHint(i, 0, strText);

		if(idPlayer == pMem->GetID())
			m_pList_Member->SetCurSel(i);
		if( pMem->IsOnline() ) nNumOnlines++;
	}

	strText.Format(_AL("%d/%d"), nNumOnlines, fml.size());
	m_pTxt_PplOnline->SetText(strText);
	char szIcon[20];
	sprintf(szIcon, "%d_%d.dds", GetGame()->GetGameInit().iServerID, 
		GetHostPlayer()->GetFactionID());
	int nIndex = 0;
	A2DSprite *pGuildIconSprite = GetGameUIMan()->GetGuildIcon(szIcon, &nIndex);
	m_pImg_GuildIcon->SetCover(pGuildIconSprite,
		nIndex);
}

void CDlgGuildMan::OnCommand_find(const char *szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_FindPlayer");
	pDlg->Show(true, true);
}

bool CDlgGuildMan::FindAndSelect(const ACHAR *szPlayerName)
{
	if (szPlayerName)
	{
		int dstLength = a_strlen(szPlayerName);
		int itemCount(m_pList_Member->GetCount());
		for (int i = 0; i < itemCount; ++ i)
		{
			const ACHAR *itemText = m_pList_Member->GetText(i);
			int itemTextLength = a_strlen(itemText);
			if (itemTextLength < dstLength ||
				itemText[dstLength] != '\t')
				continue;
			
			ACString itemName(itemText, dstLength);
			if (itemName == szPlayerName)
			{
				m_pList_Member->SetCurSel(i);
				m_pList_Member->EnsureVisible(i);
				return true;
			}
		}
	}
	return false;
}

void CDlgGuildMan::OnCommand_Diplomacy(const char *szCommand)
{
	CDlgGuildDiplomacyMan *pDlg = GetGameUIMan()->m_pDlgGuildDiplomacyMan;
	if (!pDlg->IsShow())
		pDlg->Show(true);
	OnCommand_CANCEL(NULL);
}

void CDlgGuildMan::OnCommandGTTalk(const char *szCommand)
{
	if (g_pGame->GetConfigs()->IsMiniClient())
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	//	GT已登录时，切换进出 Faction 频道
	//	GT未登录时，单次触发 GTWaiting 状态
	if (m_bGTWaiting) return;

	int idFaction = GetHostPlayer()->GetFactionID();
	if (idFaction <= 0) return;
	
	using namespace overlay;
	if (!GTOverlay::Instance().IsLogin())
	{
		if (!GetGameUIMan()->SSOGetGTTicket())
			return;
		m_bGTWaiting = true;
		a_LogOutput(1, "CDlgGuildMan::OnCommandGTTalk, GTWaiting open");
		return;
	}
	
	if (!GTOverlay::Instance().IsInFaction())
		GTOverlay::Instance().EnterFaction();
	else
		GTOverlay::Instance().LeaveFaction();
}

void CDlgGuildMan::ProcessGTEnter()
{
	//	非 GTWaiting 时，直接忽略
	//	GTWaiting 时，清除 GTWating 状态并调用进入 Faction 频道
	if (!m_bGTWaiting)	return;

	m_bGTWaiting = false;
	a_LogOutput(1, "CDlgGuildMan::ProcessGTEnter, GTWaiting cleared");

	using namespace overlay;
	if (!GTOverlay::Instance().IsInFaction())
		GTOverlay::Instance().EnterFaction();
}

void CDlgGuildMan::OnCommand_SendMassMail(const char *szCommand)
{	
	FactionMemList &fml = GetGame()->GetFactionMan()->GetMemList();
	if (fml.size() <= 1){
		return;
	}
	int idHost = GetHostPlayer()->GetCharacterID();
	CDlgMailWrite::MassReceivers receivers;
	receivers.reserve(fml.size()-1);
	for (int i(0); i < fml.size(); ++ i)
	{
		const Faction_Mem_Info *pMem = fml[i];
		if (pMem->GetID() != idHost){
			receivers.push_back(pMem->GetID());
		}
	}
	int nCost = ((int)fml.size()-1) * 500;
	GetGameUIMan()->m_pDlgMailWrite->CreateNewMail(GetStringFromTable(11231), _AL(""), nCost, receivers);
}