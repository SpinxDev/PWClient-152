// Filename	: DlgGMConsoleForbid.cpp
// Creator	: Wang Yongdong
// Date		: 2012/04/26

#include "DlgGMConsoleForbid.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_ManPlayer.h"
#include "EC_World.h"
#include "EC_ElsePlayer.h"
#include "gmgetplayerconsumeinfo_re.hpp"
#include "GMCommandInGame.h"
#include "DlgGMParam.h"
#include "EC_GameUIMan.h"
#include "GMCDBMan.h"
#include "EC_HostPlayer.h"
#include "DlgGMConsole.h"
#include "EC_Configs.h"


static void ConvertIntToIPAddr(int ip,ACString& ipStr)
{
	unsigned char* pIPAddr = reinterpret_cast<unsigned char*>(&ip);
	ipStr.Format(_AL("%d.%d.%d.%d"),pIPAddr[0],pIPAddr[1],pIPAddr[2],pIPAddr[3]);
}

AUI_BEGIN_COMMAND_MAP(CDlgGMConsoleForbid, CDlgBase)
AUI_ON_COMMAND("Btn_Name",OnCommand_Name)
AUI_ON_COMMAND("Btn_Job",OnCommand_Job)
AUI_ON_COMMAND("Btn_Level",OnCommand_Level) 
AUI_ON_COMMAND("Btn_Ip",OnCommand_IPAddr)
AUI_ON_COMMAND("Btn_Pay",OnCommand_Pay)
AUI_ON_COMMAND("Btn_Contribution",OnCommand_Contri)
AUI_ON_COMMAND("Btn_PTime",OnCommand_PTime)
AUI_ON_COMMAND("Slider_Range",OnCommand_ChangeRange)
AUI_ON_COMMAND("Btn_Lock",OnCommand_ForbidUser)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgGMConsoleForbid, CDlgBase)
AUI_END_EVENT_MAP()

CDlgGMConsoleForbid::CDlgGMConsoleForbid()
{
	m_pList_Info = NULL;
	m_iDist = 214;
	m_bTickFlag = true;
	m_duration = 0;
	m_pTimeEdit = NULL;
	m_pReasonEdit = NULL;
	m_pLabelMax = NULL;
	m_pSliderRange = NULL;
	m_pLabelMin = NULL;
}

CDlgGMConsoleForbid::~CDlgGMConsoleForbid()
{
	ReleasePlayerInfoMap();
}

void CDlgGMConsoleForbid::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Slider(bSave, "Slider_Range", m_iDist);
	DDX_EditBox(bSave,"Edt_Time",m_duration);
	DDX_EditBox(bSave,"Edt_Reason",m_reason);

	DDX_Control("lbxPlayerMsg", m_pList_Info);
	DDX_Control("Edt_Time", m_pTimeEdit);
	DDX_Control("Edt_Reason", m_pReasonEdit);
	DDX_Control("Lbl_Max",m_pLabelMax);
	DDX_Control("Slider_Range",m_pSliderRange);
	DDX_Control("Lbl_Min",m_pLabelMin);
}
void CDlgGMConsoleForbid::OnCommand_Name(const char *szCommand)
{
	m_pList_Info->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_UNISTRING, 0);
}

void CDlgGMConsoleForbid::OnCommand_Job(const char *szCommand)
{
	m_pList_Info->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_UNISTRING, 1);
}
void CDlgGMConsoleForbid::OnCommand_Level(const char* szCommand)
{
	m_pList_Info->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 2);
}
void CDlgGMConsoleForbid::OnCommand_IPAddr(const char* szCommand)
{
	m_pList_Info->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_STRING, 3);
}
void CDlgGMConsoleForbid::OnCommand_Pay(const char* szCommand)
{
	m_pList_Info->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 4);
}
void CDlgGMConsoleForbid::OnCommand_Contri(const char* szCommand)
{
	m_pList_Info->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 5);
}
void CDlgGMConsoleForbid::OnCommand_PTime(const char* szCommand)
{
	m_pList_Info->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 6);
}
void CDlgGMConsoleForbid::OnCommand_ChangeRange(const char *szCommand)
{
	UpdateData(true);

	FillPlayerList((float)m_iDist);

	ACString str;
	str.Format(_AL("%d"),m_iDist);
	m_pSliderRange->SetHint(str);
}
void CDlgGMConsoleForbid::OnCommand_ForbidUser(const char* szCommand)
{
	UpdateData(true);

	if (m_duration<=0)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(9623), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (m_reason.IsEmpty())
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(9624), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}


	if(m_pList_Info->IsMultipleSelection())
	{
		int i(0);
		i = m_pList_Info->GetSelCount();
		if(i<1) // 没有选中玩家
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(9625), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		else if (i==1)//选中一个玩家
		{
			int sel = m_pList_Info->GetCurSel();
			if (sel>=0 && sel<m_pList_Info->GetCount())
			{				
				int id = m_pList_Info->GetItemData(sel,LISTINDEX_ID);
				
				if (m_playerInfoMap.find(id)!=m_playerInfoMap.end())
				{
					player_info* pInfo = m_playerInfoMap[id];
					
					ACString strInfo;
					strInfo.Format(GetStringFromTable(9622),pInfo->name,m_duration);
					
					PAUIDIALOG pMsgDlg;
					
					GetGameUIMan()->MessageBox("GMForbidPlayerRoleByConsume", strInfo, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160),&pMsgDlg);
					pMsgDlg->SetData(id);			
				}	
			}
		}
		else //选中多个玩家
		{
			GetGameUIMan()->MessageBox("GMForbidMultiPlayerRoleByConsume", GetStringFromTable(9626), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
	}
}
void CDlgGMConsoleForbid::ForbidMultiRole()
{
	int i,count = m_pList_Info->GetSelCount();
	if(count<1) return;

	for(i=0;i<m_pList_Info->GetCount();i++)
	{
		if (m_pList_Info->GetSel(i))
		{
			int id = m_pList_Info->GetItemData(i,LISTINDEX_ID);
			ForbidRole(id);
		}
	}

	m_pTimeEdit->SetText(_AL("0"));
	m_pReasonEdit->SetText(_AL(""));
}
void CDlgGMConsoleForbid::ForbidRole(int id)
{
	if(m_playerInfoMap.find(id)==m_playerInfoMap.end()) return;
	
	player_info* pInfo = m_playerInfoMap[id];

	CGMCommandKickOutRole_Consume gmKickOutRole(pInfo,m_duration,m_reason);
	
	ACString strErr, strDetail;
	bool bResult = gmKickOutRole.Execute(strErr, strDetail);
	if (bResult)
	{
		ASSERT(g_pGame->GetGameRun()->GetHostPlayer()->IsGM());
		int nGMID = g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID();
		
		// GM log
		GMCDBMan* pDBMan = GetGameUIMan()->m_pDlgGMConsole->GetGMCDBMan();
		if(pDBMan)
			pDBMan->GetDatabase().LogExecutedCommand(gmKickOutRole.GetDisplayName(),strDetail,nGMID);			

		int i(0);
		for (;i<m_pList_Info->GetCount();i++)
		{
			int playerID = m_pList_Info->GetItemData(i,LISTINDEX_ID);
			
			if (playerID == id)			
				m_pList_Info->SetItemTextColor(i,0xFFFF0000);			
		}
	}
	else
	{
		GetGameUIMan()->MessageBox("", strErr, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}

	m_pTimeEdit->SetText(_AL("0"));
	m_pReasonEdit->SetText(_AL(""));
}
void CDlgGMConsoleForbid::GetLineString(player_info* pInfo,ACString& str)
{
	if(pInfo)
	{
		const player_info& info = *pInfo;

		ACString name = info.name;
		if(name.GetLength()<1)
			name = _AL("-");
		
		ACString prof = GetGameRun()->GetProfName(info.prof);

		ACString ipStr;
		ConvertIntToIPAddr(info.ip,ipStr);

		if(info.bLoadFlag)
		{
			ACString avgtime = GetGameUIMan()->GetFormatTime(info.avg_time);
			str.Format(_AL("%s\t%s\t%d\t%s\t%.2f\t%.2f\t%s"),name,prof,info.level,ipStr,info.fCash_add,info.fContrib,avgtime);
		}
		else
			str.Format(_AL("%s\t%s\t%d\t-\t-\t-\t-"),name,prof,info.level);
	}
}
void CDlgGMConsoleForbid::FillPlayerList(float fDist)
{
	m_pList_Info->ResetContent();

	int i(0);
	PlayerInfoMap::iterator itr = m_playerInfoMap.begin();
	for (;itr!=m_playerInfoMap.end();++itr)
	{
		player_info* pInfo = itr->second;
		if (pInfo && pInfo->dist<=fDist)
		{
			ACString str;
			
			GetLineString(pInfo,str);
		
			m_pList_Info->AddString(str);
			m_pList_Info->SetItemData(i,pInfo->id,LISTINDEX_ID);
			m_pList_Info->SetItemData(i,CONSUMEINFO_EMPTY,LISTINDEX_DONE);
			i++;
		}
	}
}
void CDlgGMConsoleForbid::OnHideDialog()
{
	ReleasePlayerInfoMap();

	// 保存Log
	GetGameUIMan()->m_pDlgGMConsole->SaveGMCDBMan(false);
}
void CDlgGMConsoleForbid::OnShowDialog()
{
	m_duration = 0;
	m_reason.Empty();

	CECConfigs* pConfigs = g_pGame->GetConfigs();
	int iMaxDist = (int)pConfigs->GetSevActiveRadius();


	ACString str;
	str.Format(_AL("%d"),iMaxDist);
	m_pLabelMax->SetText(str);
	str.Format(_AL("%d"),0);
	m_pLabelMin->SetText(str);

	m_pSliderRange->SetTotal(iMaxDist);

	// 周围玩家部分信息
	GetPlayerInfoMap();

	m_bTickFlag = true;

	FillPlayerList((float)m_iDist);

	abase::vector<int> playeridvec;
	GetPlayerIDVecByDist((float)iMaxDist,playeridvec);
	//请求玩家消费信息
	GetGameSession()->GetPlayerConsumeInfo(playeridvec.size(),playeridvec.begin());
}
void CDlgGMConsoleForbid::OnTick()
{
	if(!m_bTickFlag) return;

	int i(0);
	int count(0);
	for (;i<m_pList_Info->GetCount();i++)
	{
		DWORD flag = m_pList_Info->GetItemData(i,LISTINDEX_DONE);
		if (flag==CONSUMEINFO_EMPTY)
		{
			int id = m_pList_Info->GetItemData(i,LISTINDEX_ID);
			
			if (m_playerInfoMap.find(id)!=m_playerInfoMap.end())
			{
				player_info* p = m_playerInfoMap[id];
				if (p->bLoadFlag)
				{
					ACString str;
					
					GetLineString(p,str);
					m_pList_Info->SetText(i,str);
					m_pList_Info->SetItemData(i,CONSUMEINFO_DONE,LISTINDEX_DONE);
				}				
			}

			count++;
		}
	}

	m_bTickFlag = (count>0);
}
void CDlgGMConsoleForbid::ReleasePlayerInfoMap()
{
	PlayerInfoMap::iterator itr = m_playerInfoMap.begin();
	for (;itr!=m_playerInfoMap.end();++itr)
	{
		player_info* p = itr->second;
		delete p;
	}

	m_playerInfoMap.clear();
}

void CDlgGMConsoleForbid::GetPlayerInfoMap()
{
	ReleasePlayerInfoMap();

	CECPlayerMan* pMan = GetGameRun()->GetWorld()->GetPlayerMan();
	if (pMan)
	{
		const CECPlayerMan::PlayerTable& pt = pMan->GetPlayerTable();

		CECPlayerMan::PlayerTable::const_iterator it = pt.begin();
		for (; it != pt.end(); ++it)
		{
			CECElsePlayer* pPlayer = *it.value();
			if (pPlayer)
			{
				float d = pPlayer->GetDistToHostH();
				player_info* info = new player_info;
				info->id = pPlayer->GetCharacterID();
				info->level = pPlayer->GetBasicProps().iLevel;
				info->prof = pPlayer->GetProfession();
				info->dist = d;
				info->name = pPlayer->GetName();

				m_playerInfoMap[info->id] = info;
			}
		}
	}
}


void CDlgGMConsoleForbid::GetPlayerIDVecByDist(float dist, abase::vector<int>& playervec)
{
	playervec.clear();
	
	CECPlayerMan* pMan = GetGameRun()->GetWorld()->GetPlayerMan();
	if (pMan)
	{
		const CECPlayerMan::PlayerTable& pt = pMan->GetPlayerTable();
		
		CECPlayerMan::PlayerTable::const_iterator it = pt.begin();
		for (; it != pt.end(); ++it)
		{
			CECElsePlayer* pPlayer = *it.value();
			if (pPlayer)
			{
				float d = pPlayer->GetDistToHostH();
				if (d<=dist)
				{					
					playervec.push_back(pPlayer->GetCharacterID());
				}
			}
		}
	}
}

void CDlgGMConsoleForbid::OnPrtcGMGetConsumeInfo_Re(GNET::GMGetPlayerConsumeInfo_Re *pProtocol)
{
	if (!pProtocol)
		return;
	
	if(pProtocol->gmroleid != GetGameSession()->GetCharacterID())
	{
		ASSERT(FALSE);
		return;
	}

	if (pProtocol->retcode == ERR_SUCCESS)
	{
		unsigned int i(0);
		for (;i<pProtocol->playerlist.size();i++)
		{
			const GNET::PlayerConsumeInfo& info = pProtocol->playerlist[i];
			
			if (m_playerInfoMap.find(info.roleid)== m_playerInfoMap.end())
			{
				a_LogOutput(1,"CDlgGMConsoleForbid::OnPrtcGMGetConsumeInfo_Re, error roleid: %d",info.roleid);
			}
			else
			{
				player_info* p = m_playerInfoMap[info.roleid];
			//	ASSERT(p->level == info.level);
				p->level = info.level;
				p->ip = info.login_ip;
				p->fContrib = info.mall_consumption * 0.01f;
				p->fCash_add = info.cash_add * 0.01f;
				p->avg_time = info.avg_online_time;
				
				p->bLoadFlag = true;
			}
		}
	}
	else
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9621), GP_CHAT_MISC);	
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
CGMCommandKickOutRole_Consume::CGMCommandKickOutRole_Consume(const CDlgGMConsoleForbid::player_info* pInfo,int duration,ACString reason)
{
	m_reason = reason;
	m_duration = duration;
	m_pInfo = pInfo;
};
ACString CGMCommandKickOutRole_Consume::GetDisplayName()
{
	return GETSTRINGFROMTABLE(9620);
}
bool CGMCommandKickOutRole_Consume::Execute(ACString & strErr, ACString &strDetail)
{
	if(!m_pInfo) return false;

	if (m_pInfo->id <= 0)
	{
		strErr = GETSTRINGFROMTABLE(6304);
		return false;
	}

	if (m_duration < 0)
	{
		strErr = GETSTRINGFROMTABLE(6305);
		return false;
	}

	if (m_reason.IsEmpty())
	{
		strErr = GETSTRINGFROMTABLE(6306);
		return false;
	}
	
	ACString ipStr;
	ConvertIntToIPAddr(m_pInfo->ip,ipStr);

//	ACString strProf = g_pGame->GetGameRun()->GetProfName(m_pInfo->prof);
	strDetail.Format(
		_AL("playerID = [%d], duration = [%d], reason = [%s], name = [%s], prof = [%d], level = [%d], ip = [%s], cash_add = [%.2f], contrib = [%.2f], agvtime = [%d]"),
		m_pInfo->id,
		m_duration, 
		m_reason,
		m_pInfo->name,m_pInfo->prof,m_pInfo->level,ipStr,m_pInfo->fCash_add,m_pInfo->fContrib,m_pInfo->avg_time);
	
	g_pGame->GetGameSession()->gm_KickOutRole(m_pInfo->id, m_duration, m_reason);

	return true;
}
