// File		: EC_CrossServer.cpp
// Creator	: Xu Wenbin
// Date		: 2012/12/6

#include "EC_CrossServer.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_BaseUIMan.h"
#include "EC_GameSession.h"
#include "EC_CrossServerList.h"
#include "EC_ServerList.h"
#include "EC_Reconnect.h"

#include "EC_LoginUIMan.h"

#include "changeds_re.hpp"
#include "playerchangeds_re.hpp"
#include "roleinfo"
#include "keyreestablish.hpp"
#include "gameclient.h"

//#define CROSS_SERVER_LOG_DEBUG_INFO		//	��ӡ������Ϣ����־������ʱ�ɹر�

CECCrossServer::CECCrossServer()
: m_pPlayerChangeDS_Re(NULL)
, m_bWaitLogin(false)
, m_bOnSpecialServer(false)
, m_iOrgServerIndex(-1)
, m_srcZoneID(-1)
, m_iSaveBackImage(0)
, m_iSaveTipIndex(-1)
, m_fSaveLoadPos(0.0f)
{
}

CECCrossServer::~CECCrossServer()
{
	delete m_pPlayerChangeDS_Re;
	m_pPlayerChangeDS_Re = NULL;
}

CECCrossServer & CECCrossServer::Instance()
{
	static CECCrossServer s_dummy;
	return s_dummy;
}

void CECCrossServer::OnPrtcChangeDS_Re(GNET::Protocol *pProtocol)
{
	using namespace GNET;
	ChangeDS_Re *p = (ChangeDS_Re *)pProtocol;
	g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
	a_LogOutput(1, "ChangeDS_Re: retcode=%d", p->retcode);
}

#ifdef CROSS_SERVER_LOG_DEBUG_INFO
template <typename T> void PrintDiff(T& n, T& o, const char *szPrefix, AString &strDiff)
{
	if (n != o)
		strDiff += AString().Format("%s:%d(new)!=%d, ", szPrefix, n, o);
}
template <> void PrintDiff<ACString>(ACString & n, ACString & o, const char *szPrefix, AString &strDiff)
{
	if (n != o)
		strDiff += AString().Format("%s:%s(new)!=%s, ", szPrefix, AC2AS(n), AC2AS(o));
}
template <> void PrintDiff<float>(float& n, float& o, const char *szPrefix, AString &strDiff)
{
	if (n != o)
		strDiff += AString().Format("%s:%f(new)!=%f, ", szPrefix, n, o);
}
template <> void PrintDiff<GNET::Octets>(GNET::Octets &n, GNET::Octets &o, const char *szPrefix, AString &strDiff)
{
	if (n != o)
		strDiff += AString().Format("%s:%s(new)!=%s, ", szPrefix, glb_FormatOctets(n), glb_FormatOctets(o));
}
template <> void PrintDiff<GNET::GRoleInventory>(GNET::GRoleInventory & n, GNET::GRoleInventory & o, const char *szPrefix, AString &strDiff)
{
	PrintDiff(n.id, o.id, AString().Format("%s.id", szPrefix), strDiff);
	PrintDiff(n.pos, o.pos, AString().Format("%s.pos", szPrefix), strDiff);
	PrintDiff(n.count, o.count, AString().Format("%s.count", szPrefix), strDiff);
	PrintDiff(n.max_count, o.max_count, AString().Format("%s.max_count", szPrefix), strDiff);
	PrintDiff(n.data, o.data, AString().Format("%s.data", szPrefix), strDiff);
	PrintDiff(n.proctype, o.proctype, AString().Format("%s.proctype", szPrefix), strDiff);
	PrintDiff(n.expire_date, o.expire_date, AString().Format("%s.expire_date", szPrefix), strDiff);
	PrintDiff(n.guid1, o.guid1, AString().Format("%s.guid1", szPrefix), strDiff);
	PrintDiff(n.guid2, o.guid2, AString().Format("%s.guid2", szPrefix), strDiff);
	PrintDiff(n.mask, o.mask, AString().Format("%s.mask", szPrefix), strDiff);
}

ACString Octets2Name(const GNET::Octets &o)
{
	return ACString((const ACHAR*)o.begin(), o.size() / sizeof (ACHAR));
}
void PrintRoleDiff(GNET::RoleInfo &new_info, GNET::RoleInfo &old_info)
{
	//	��ӡ�½�ɫ��Ϣ����������ɫ��Ϣ�в�ͬ����
	a_LogOutput(1, "PlayerChangeDS_Re:roleid=%d, gender=%d, race=%d, occupation=%d, level=%d, level2=%d, name=%s, custom_data=%s, equipment.size()=%d, status=%d, delete_time=%d, create_time=%d, lastlogin_time=%d, posx=%f, posy=%f, posz=%f, worldtag=%d, custom_status=%s, charactermode=%s, referrer_role=%d, cash_add=%d",
		new_info.roleid, new_info.gender, new_info.race, new_info.occupation, new_info.level, new_info.level2,
		AC2AS(Octets2Name(new_info.name)), glb_FormatOctets(new_info.custom_data), new_info.equipment.size(),
		new_info.status, new_info.delete_time, new_info.create_time, new_info.lastlogin_time,
		new_info.posx, new_info.posy, new_info.posz, new_info.worldtag,
		glb_FormatOctets(new_info.custom_status), glb_FormatOctets(new_info.charactermode),
		new_info.referrer_role, new_info.cash_add);

	AString strDiff;

#define PRINT_DIFF(m)	PrintDiff(new_info.m, old_info.m, #m, strDiff)

	PRINT_DIFF(roleid);
	PRINT_DIFF(gender);
	PRINT_DIFF(race);
	PRINT_DIFF(occupation);
	PRINT_DIFF(level);
	PRINT_DIFF(level2);

	ACString strNewName = Octets2Name(new_info.name);
	ACString strOldName = Octets2Name(old_info.name);
	PrintDiff(strNewName, strOldName, "name", strDiff);

	PRINT_DIFF(custom_data);
	
	PRINT_DIFF(status);
	PRINT_DIFF(delete_time);
	PRINT_DIFF(create_time);
	PRINT_DIFF(lastlogin_time);
	PRINT_DIFF(posx);
	PRINT_DIFF(posy);
	PRINT_DIFF(posz);
	PRINT_DIFF(worldtag);
	PRINT_DIFF(custom_status);
	PRINT_DIFF(charactermode);
	PRINT_DIFF(referrer_role);
	PRINT_DIFF(cash_add);
	
	if (new_info.equipment.size() != old_info.equipment.size())
	{
		size_t sn = new_info.equipment.size();
		size_t so = old_info.equipment.size();
		PrintDiff(sn, so, "equipment.size()", strDiff);
	}
	else for (size_t u(0); u < new_info.equipment.size(); ++ u)
	{
		GRoleInventory &n = new_info.equipment[u];
		GRoleInventory &o = old_info.equipment[u];
		PrintDiff(n, o, AString().Format("equipment[%d]", u), strDiff);
	}

#undef PRINT_DIFF
	
	if (!strDiff.IsEmpty())
		a_LogOutput(1, "PlayerChangeDS_Re:roleinfo different as %s", strDiff);
}
#endif

void CECCrossServer::OnPrtcPlayerChangeDS_Re(GNET::Protocol *pProtocol)
{
	using namespace GNET;
	PlayerChangeDS_Re *p = (PlayerChangeDS_Re *)pProtocol;
	
	//	��ӡЭ�����
	a_LogOutput(1, "PlayerChangeDS_Re:retcode=%d,roleid=%d,remote_roleid=%d,userid=%d,flag=%d,dst_zoneid=%d,localsid=%u,random=%s",
		p->retcode, p->roleid, p->remote_roleid, p->userid,
		p->flag, p->dst_zoneid, p->localsid,
		glb_FormatOctets(p->random));

	RoleInfo new_info;
	try
	{
		Marshal::OctetsStream(p->roleinfo_pack) >> new_info;
	}
	catch (Marshal::Exception &)
	{
		ASSERT(false);
		ShowError(ACString().Format(
			_AL("PlayerChangeDS_Re(error):unmarshal roleinfo exception(roleinfo_pack.size())=%u"),
			p->roleinfo_pack.size()));
		return;
	}

#ifdef CROSS_SERVER_LOG_DEBUG_INFO
	//	��֤����ӡ��ɫ��Ϣ�в�ͬ����
	{
		RoleInfo last_info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
		PrintRoleDiff(new_info, last_info);
	}
#endif

	if (IsWaitLogin())
	{
		//	����ת���У���Ӧ���յ���Э��
		ASSERT(false);
		ShowError(_AL("PlayerChangeDS_Re:already in state"));
		return;
	}

	if (p->retcode != 0)
	{
		//	ʧ��ʱ��������ԭ��
		g_pGame->GetGameSession()->OutputLinkSevError(p->retcode);
		a_LogOutput(1, "PlayerChangeDS_Re: retcode=%d", p->retcode);
		return;
	}

	//	flag ��֤����Լ�����ͻ��˲����յ��ı�־��
	switch (p->flag)
	{
	case 0:
		ASSERT(false);
		ShowError(_AL("PlayerChangeDS_Re:Invalid flag=0"));
		return;

	case 1:	//	ԭ���������
	case 2:	//	�������ԭ��
		if (g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_GAME)
		{
			ASSERT(false);
			ShowError(_AL("PlayerChangeDS_Re:not in game"));
			return;
		}
		break;

	case 3:	//	δ������Ϸ��EnterWorld��ֱ�ӵ�¼�����
		if (g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_LOGIN)
		{
			ASSERT(false);
			ShowError(_AL("PlayerChangeDS_Re:not at login"));
			return;
		}
		break;

	default:
		ASSERT(false);
		ShowError(_AL("PlayerChangeDS_Re:Invalid flag"));
		return;
	}

	//	��֤�ͻ��˵�¼״̬
	if (!g_pGame->GetGameSession()->IsConnected())
	{
		ShowError(_AL("PlayerChangeDS_Re:link already broken"));
		return;
	}

	//	��¼����
	if (m_pPlayerChangeDS_Re) delete m_pPlayerChangeDS_Re;
	m_pPlayerChangeDS_Re = (PlayerChangeDS_Re *)p->Clone();
	SetSrcZoneID(g_pGame->GetGameSession()->GetZoneID());

	//	����ת��״̬
	m_bWaitLogin = true;

	//	��¼ԭ����ɫ ID ��������
	if (IsLoginToSpecial()){
		if (!CECReconnect::Instance().IsReconnecting()){
			CECReconnect::Instance().SetRoleID(g_pGame->GetGameRun()->GetSelectedRoleInfo().roleid);
		}
	}

	int gState = g_pGame->GetGameRun()->GetGameState();
	if (gState == CECGameRun::GS_GAME)
	{
		//	���öϿ����ӱ�־���ص���¼����ȴ���һ������
		g_pGame->GetGameRun()->SetLogoutFlag(2);
		
		//	����м��۽�ɫ������Է�Ӱ��ת������
		if (g_pGame->GetGameRun()->GetSellingRoleID() > 0)
		{
			g_pGame->GetGameRun()->SetSellingRoleID(0);
			a_LogOutput(1, "CECGameRun::m_SellingRoleID cleared.");
		}
	}
	else if (gState == CECGameRun::GS_LOGIN)
	{
		//	��ѡ�˽��桢�Ͽ�ԭ���Ӷ���������������ٴε�¼		
		CECLoginUIMan *pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
		pLoginUIMan->ReclickLoginButton();
	}
	else
	{
		//	��Ӧ�÷��������
		ASSERT(false);
		OnLoginFail();
	}
}

void CECCrossServer::ShowError(const ACString &strMsg)
{
	if (!strMsg.IsEmpty())
		a_LogOutput(1, AC2AS(strMsg));
}

CECCrossServer::CrossType CECCrossServer::GetType()const
{
	CrossType ret = CT_NONE;
	if (IsWaitLogin())
		ret = static_cast<CrossType>(m_pPlayerChangeDS_Re->flag);
	return ret;
}

bool CECCrossServer::ApplyServerSetting()
{
	bool bRet(false);

	while (true)
	{
		if (!IsWaitLogin())
		{
			ASSERT(false);
			break;
		}

		char flag = m_pPlayerChangeDS_Re->flag;
		int zoneid = m_pPlayerChangeDS_Re->dst_zoneid;
		if (flag == 1 || flag == 3)
		{
			//	ԭ���������
			CECCrossServerList::ServerInfo * pInfo = 
				CECCrossServerList::GetSingleton().Find(zoneid, GetOrgServerLine());
			if (!pInfo)
			{
				ShowError(ACString().Format(_AL("CECCrossServer::ApplyServerSetting,Unknown zoneid=%d"), zoneid));
				break;
			}

			//	ÿ�ε�¼����˿�
			pInfo->RandPort();
			
			CECGame::GAMEINIT &gi = g_pGame->GetGameInit();
			gi.iPort = pInfo->port;
			strcpy(gi.szIP, pInfo->address);
			strcpy(gi.szServerName, AC2AS(pInfo->server_name));
			gi.iServerID = pInfo->zoneid;
			gi.iLine = pInfo->line;
			
			g_pGame->GetGameRun()->SetServerName(_AL(""), pInfo->server_name);
		}
		else if (flag == 2)
		{
			//	�������ԭ��
			CECServerList::Instance().SelectServer(GetOrgServerIndex());
			if (!CECServerList::Instance().ApplyServerSetting())
			{
				ShowError(ACString().Format(_AL("CECCrossServer::ApplyServerSetting, Invalid server index:%d"), CECServerList::Instance().GetSelected()));
				break;
			}
		}
		else
		{
			ASSERT(false);
			ShowError(ACString().Format(_AL("CECCrossServer::ApplyServerSetting,Invalid flag=%d"), flag));
			break;
		}

		bRet = true;
		break;
	}
	return bRet;
}

void CECCrossServer::SelectRole()
{
	if (!IsWaitLogin())
	{
		ASSERT(false);
		return;
	}
	
	using namespace GNET;
	RoleInfo info;
	Marshal::OctetsStream(m_pPlayerChangeDS_Re->roleinfo_pack) >> info;
	g_pGame->GetGameRun()->SetSelectedRoleInfo(info);
	g_pGame->GetGameRun()->SetRedirectLoginPosRoleInfo(RoleInfo());
	g_pGame->GetGameSession()->SelectRole(info.roleid, m_pPlayerChangeDS_Re->flag);
}

void CECCrossServer::OnLogout()
{
	m_bOnSpecialServer = false;
}

void CECCrossServer::OnPrtcChallenge()
{
	if (!IsWaitLogin())
	{
		ASSERT(false);
		return;
	}
	
	using namespace GNET;

	//	�ظ� Challenge
	KeyReestablish kr;
	kr.roleid = m_pPlayerChangeDS_Re->remote_roleid;
	kr.userid = m_pPlayerChangeDS_Re->userid;
	kr.flag = m_pPlayerChangeDS_Re->flag;
	kr.src_zoneid = GetSrcZoneID();
	kr.random = m_pPlayerChangeDS_Re->random;
	g_pGame->GetGameSession()->SendNetData(kr);

	//	KeyReestablish Э�鷢�Ͳ����ܣ�SendNetData ��ӵ���������ֻ��δ���ܵ�
	//	��ˣ����治���������� OSecurity�����򽫵��� KeyReestablish �Լ��ܷ���

	//	���ܽ�������
	GameClient *pNetManager = g_pGame->GetGameSession()->GetNetManager();	
	DWORD sid = g_pGame->GetGameSession()->GetLinkID();
	pNetManager->SetISecurity(sid, DECOMPRESSARCFOURSECURITY, GetISecurity());

	//	���ܷ������ӵȽ��յ��¸�Э��ʱ���У��� OnlineAnnounce ��
}

void CECCrossServer::OnPrtcOnlineAnnounce()
{
	if (!IsWaitLogin())
	{
		ASSERT(false);
		return;
	}
	
	using namespace GNET;
	
	//	���ܷ�������
	GameClient *pNetManager = g_pGame->GetGameSession()->GetNetManager();	
	DWORD sid = g_pGame->GetGameSession()->GetLinkID();
	pNetManager->SetOSecurity(sid, ARCFOURSECURITY, GetOSecurity());

	//	��ʱ���޸�״̬Ϊ STATE_KEEPING����ֹ IO �̷߳���δ���ܵ� KeepAlive Э��
	pNetManager->ChangeState(sid, &state_GSelectRoleClient);
	
	CECLoginUIMan *pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
	if (!IsLoginToSpecial())
	{		
		//	������ƺ�ID��Ӧ��
		g_pGame->GetGameRun()->ClearNameIDPairs();
		
		//	ģ�� RoleList �� RoleListRe Э�飬���Ωһ��ɫ
		RoleInfo info;
		Marshal::OctetsStream(m_pPlayerChangeDS_Re->roleinfo_pack) >> info;
		
		pLoginUIMan->AddCharacter(&info);
		
		//	ѡ�д˽�ɫ
		pLoginUIMan->SelectLatestCharacter();
	}
	//	else ֱ�ӵ�¼�����������ѡ�˽������ݲ��䣬һ�а��в���

	//	ѡ�н�ɫ��������Ϸ����
	pLoginUIMan->LoginSelectedChar();
}

void CECCrossServer::OnLoginFail()
{
	if (!IsWaitLogin())
	{
		ASSERT(false);
		return;
	}

	//	ת��������ʧ��
	m_bWaitLogin = false;
	
	delete m_pPlayerChangeDS_Re;
	m_pPlayerChangeDS_Re = NULL;

	a_LogOutput(1, "CECCrossServer::OnLoginFail");
}

void CECCrossServer::OnLoginSuccess()
{
	if (!IsWaitLogin())
	{
		ASSERT(false);
		return;
	}

	//	ת���ɹ�
	m_bWaitLogin = false;

	m_bOnSpecialServer = (m_pPlayerChangeDS_Re->flag == 1 || m_pPlayerChangeDS_Re->flag == 3);
	delete m_pPlayerChangeDS_Re;
	m_pPlayerChangeDS_Re = NULL;

	a_LogOutput(1, "CECCrossServer::OnLoginSuccess");
}

bool CECCrossServer::IsOnSpecialServer()const
{
	//	�Ƿ���������ϣ�������Ϸ���жϣ�
	return g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME
		&& m_bOnSpecialServer;
}

int CECCrossServer::GetOrgServerLine()const
{
	int iLine(-1);
	if (m_iOrgServerIndex >= 0 && m_iOrgServerIndex < CECServerList::Instance().GetServerCount()){
		iLine = CECServerList::Instance().GetServer(m_iOrgServerIndex).line;
	}
	return iLine;
}