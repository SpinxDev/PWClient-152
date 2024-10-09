// File		: DlgWebRoleTrade.cpp
// Creator	: Feng Ning
// Date		: 2010/12/30

#include "DlgWebRoleTrade.h"
#include "EC_LoginUIMan.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "webtradeprepost_re.hpp"
#include "webtradeprecancelpost_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWebRoleTrade, CDlgWebMyShop)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgWebRoleTrade::CDlgWebRoleTrade()
{
}

CDlgWebRoleTrade::~CDlgWebRoleTrade()
{
}

bool CDlgWebRoleTrade::OnInitDialog()
{
	if(!CDlgWebMyShop::OnInitDialog())
	{
		return false;
	}

	m_Mode = MODE_ROLE;

	return true;
}

void CDlgWebRoleTrade::OnShowDialog()
{
	//
	CDlgBase::OnShowDialog();
}

CECLoginUIMan* CDlgWebRoleTrade::GetLoginUIMan()
{
	return (CECLoginUIMan*)(m_pAUIManager);;
}

void CDlgWebRoleTrade::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	if(!stricmp(pDlg->GetName(), "Game_WebTrade_RoleTradeCancel"))
	{
		if(iRetVal == IDYES)
		{
			// return to game
			SetEnable(false);

			// clear the selling id
			g_pGame->GetGameRun()->SetSellingRoleID(0);
			GetLoginUIMan()->LoginSelectedChar();
		}
	}
	else
	{
		CDlgWebMyShop::OnMessageBox(pDlg, iRetVal);
	}
}

void CDlgWebRoleTrade::OnCommandCancel(const char * szCommand)
{
	GetLoginUIMan()->MessageBox("Game_WebTrade_RoleTradeCancel", 
								GetStringFromTable(5551),
								MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgWebRoleTrade::WebTradeAction(int idAction, void *pData)
{
	if( idAction == PROTOCOL_WEBTRADEPREPOST_RE )
	{
		WebTradePrePost_Re *pInfo = (WebTradePrePost_Re*)pData;
		
		// prepost finished, hide this dialog
		if(!pInfo->retcode && GetLoginUIMan()->PrepostCharacter(pInfo->info.seller_roleid))
		{
			Show(false);
			
			// clear the selling id
			g_pGame->GetGameRun()->SetSellingRoleID(0);
			
			// restore the selection scene
			GetLoginUIMan()->SetIDCurRole(-1);
			GetLoginUIMan()->SelectLatestCharacter();
			GetLoginUIMan()->SwitchToSelectChar();
		}
		else
		{
			GetLoginUIMan()->MessageBox("", GetStringFromTable(5554), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}

	} else if( idAction == PROTOCOL_WEBTRADEPRECANCELPOST_RE) {
		
		WebTradePreCancelPost_Re* pInfo = (WebTradePreCancelPost_Re*)pData;
		if(!pInfo->retcode)
			GetLoginUIMan()->MessageBox("", GetLoginUIMan()->GetStringFromTable(241), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

	} else {
		CDlgWebMyShop::WebTradeAction(idAction, pData);
	}
}

void CDlgWebRoleTrade::SetRoleInfo(const GNET::RoleInfo& info)
{
	PAUILABEL pName = (PAUILABEL)GetDlgItem("Lab_Name");
	PAUILABEL pLevel = (PAUILABEL)GetDlgItem("Lab_Level");

	ACString strText((const ACHAR *)info.name.begin(), info.name.size() / sizeof(ACHAR));
	ACHAR szName[256];
	AUI_ConvertChatString(strText, szName);
	pName->SetText(szName);
	
	strText.Format(_AL("%s %s %d"),
		g_pGame->GetGameRun()->GetProfName(info.occupation),
		GetStringFromTable(323), info.level);
	pLevel->SetText(strText);
}
