// Filename	: DlgCountryReward.cpp
// Creator	: Han Guanghui
// Date		: 2012/8/10

#include "DlgCountryReward.h"
#include "EC_HostPlayer.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_DomainCountry.h"
#include "EC_CountryConfig.h"
#include "Network\\ids.hxx"
#include "countrybattleresult.hpp"

AUI_BEGIN_COMMAND_MAP(CDlgCountryReward, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgCountryReward::CDlgCountryReward()
{
}

CDlgCountryReward::~CDlgCountryReward()
{
}

void CDlgCountryReward::OnCommandCancel(const char * szCommand)
{
	Show(false);
}

bool CDlgCountryReward::OnInitDialog()
{
	return true;
}

void CDlgCountryReward::OnPrtcCountryResult(GNET::Protocol* pProtocol)
{
	CountryBattleResult* p = dynamic_cast<CountryBattleResult*>(pProtocol);
	if (p)
	{
		if (p->country_domains_cnt.size() != DOMAIN2_INFO::COUNTRY_COUNT) ASSERT(false);
		AString strTerritoryPrefix = "Lbl_Territory";
		AString strScorePrefix = "Lbl_Reward";
		AString strTerritory;
		AString strScore;
		ACString strNumber;
		PAUIOBJECT pLabel = NULL;
		int i;
		for (i = 0; i < DOMAIN2_INFO::COUNTRY_COUNT; ++i)
		{
			strTerritory.Format("%s%d", strTerritoryPrefix, i + 1);
			strScore.Format("%s%d", strScorePrefix, i + 1);
			pLabel = GetDlgItem(strTerritory);
			if (pLabel)
			{
				strNumber.Format(GetGameUIMan()->GetStringFromTable(9925), p->country_domains_cnt[i]);
				pLabel->SetText(strNumber);
			}
			pLabel = GetDlgItem(strScore);
			if (pLabel)
			{
				strNumber.Format(_AL("%d"), p->country_bonus[i]);
				pLabel->SetText(strNumber);
			}
		}
		pLabel = GetDlgItem(strScorePrefix);
		if (pLabel)
		{
			strNumber.Format(_AL("%d"), p->player_bonus);
			pLabel->SetText(strNumber);
		}
		Show(true);
	}
}


void CDlgCountryReward::OnShowDialog()
{

}
