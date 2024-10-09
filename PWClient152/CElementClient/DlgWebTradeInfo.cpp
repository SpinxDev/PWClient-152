// File		: DlgWebTradeInfo.cpp
// Creator	: Feng Ning
// Date		: 2011/01/07

#include "DlgWebTradeInfo.h"
#include "EC_LoginUIMan.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "webtradegetdetail_re.hpp"

#include "DlgWebTradeBase.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWebTradeInfo, CDlgTheme)
AUI_END_COMMAND_MAP()

int CDlgWebTradeInfo::s_RoleCount = 0;

CDlgWebTradeInfo::CDlgWebTradeInfo()
:m_pTradeItem(NULL)
,m_pItemInfo(NULL)
,m_Waiting(false)
,m_RoleID(0)
{
}

CDlgWebTradeInfo::~CDlgWebTradeInfo()
{
}

bool CDlgWebTradeInfo::OnInitDialog()
{
	if(!CDlgTheme::OnInitDialog())
	{
		return false;
	}

	m_Waiting = false;
	m_RoleID = 0;

	//
	return true;
}

void CDlgWebTradeInfo::OnShowDialog()
{
	//
	CDlgTheme::OnShowDialog();
}

bool CDlgWebTradeInfo::Release()
{
	delete m_pItemInfo;
	m_pItemInfo = NULL;

	delete m_pTradeItem;
	m_pTradeItem = NULL;

	return CDlgTheme::Release();
}

void CDlgWebTradeInfo::OnTick()
{
	if(m_pTradeItem && m_pItemInfo)
	{
		ACString szText;
		szText.Format( GetStringFromTable(5560),
			//info.GetSellerName(),
			m_pItemInfo->GetTime(),
			m_pItemInfo->GetFormatedPrice(),
			m_pItemInfo->GetBuyerName() );
		
		PAUILABEL pDesc = (PAUILABEL)GetDlgItem("Txt_Desc");
		pDesc->SetText(szText);

		RefreshPosition(m_pTradeItem->seller_roleid);
	}

	if(!m_Waiting && m_RoleID != 0)
	{
		ForceFetchRoleTradeInfo();
		m_Waiting = true;
	}

	CDlgTheme::OnTick();
}

void CDlgWebTradeInfo::RefreshPosition(int roleid)
{
	CECLoginUIMan* pUIMan = (CECLoginUIMan*)m_pAUIManager;

	const abase::vector<GNET::RoleInfo>& infos = pUIMan->GetVecRoleInfo();
	for(size_t i=0;i<infos.size();i++)
	{
		if(roleid == infos[i].roleid)
		{
			// currently we align the dialog to the RadioButton on Win_Select
			PAUIDIALOG pDlg = pUIMan->GetDialog("Win_Select");
			if(pDlg)
			{
				POINT targetPos = GetPos();

				PAUISCROLL pScroll = (PAUISCROLL)pDlg->GetDlgItem("Scroll_01");
				int objecIndex = i + 1 - (pScroll ? pScroll->GetBarLevel() : 0);

				// set the y position
				AString alignObjName;
				PAUIOBJECT pAlignObj = pDlg->GetDlgItem(alignObjName.Format("Rdo_Char%d", objecIndex));
				if(pAlignObj)
				{
					targetPos.y = pAlignObj->GetPos().y; // + pAlignObj->GetSize().cy / 2 - GetSize().cy / 2;
				}

				// set the x position
				if( pDlg->GetPos().x > GetSize().cx )
				{
					targetPos.x = pDlg->GetPos().x - GetSize().cx;
				}
				else
				{
					targetPos.x = pDlg->GetPos().x + pDlg->GetSize().cx;
				}

				SetPosEx(targetPos.x, targetPos.y);
			}

			break;
		}
	}
}

void CDlgWebTradeInfo::ForceFetchRoleTradeInfo()
{
	PAUILABEL pDesc = (PAUILABEL)GetDlgItem("Txt_Desc");
	pDesc->SetText(GetStringFromTable(5559));
	g_pGame->GetGameSession()->webTrade_RoleGetDetail(m_RoleID);
}

void CDlgWebTradeInfo::FetchRoleTradeInfo(int roleid)
{
	m_RoleID = roleid;
	if(!m_Waiting)
	{
		ForceFetchRoleTradeInfo();
		m_Waiting = true;
	}
}

void CDlgWebTradeInfo::SetTradeDetail(const GNET::WebTradeGetDetail_Re& data)
{	
	if(data.retcode != ERROR_SUCCESS)
	{
		return;
	}

	const GWebTradeItem& tradeItem = data.detail.info;
	if(tradeItem.posttype != CDlgWebTradeBase::ItemInfo::POSTTYPE_ROLE )
	{
		return;
	}

	// should in waitting status
	ASSERT(m_Waiting);
	// drop the old one
	if(m_RoleID && m_RoleID != tradeItem.seller_roleid)
	{
		m_Waiting = false;
		return;
	}

	if(m_pTradeItem) delete m_pTradeItem;
	m_pTradeItem = (GWebTradeItem*)tradeItem.Clone();

	// check the unknown buyer name
	if (m_pItemInfo) delete m_pItemInfo;
	m_pItemInfo = new CDlgWebTradeBase::ItemInfo(*m_pTradeItem);
	AArray<int, int> aUnknowNames;
	m_pItemInfo->CheckNameKnown(&aUnknowNames);
	if(aUnknowNames.GetSize() > 0)
	{
		if (g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME)
		{
			//	此协议只在ingame状态下调用
			g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(aUnknowNames.GetSize(), aUnknowNames.GetData(), 2);
		}
		else
		{
			ASSERT(false);
			a_LogOutput(1, "CDlgWebTradeInfo::SetTradeDetail, cannot get player name in login state");
		}
	}

	m_RoleID = 0;
	m_Waiting = false;
}