// Filename	: DlgDomainCondition.cpp
// Creator	: Shi && Wang
// Date		: 2013/1/29

#include "DlgDomainCondition.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "AUIManager.h"
#include "AIniFile.h"
#include "DlgKingCommand.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgDomainCondition, CDlgBase)

AUI_ON_COMMAND("Btn_Confirm", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_Cancel", OnCommand_CANCEL)

AUI_END_COMMAND_MAP()


CDlgDomainCondition::CDlgDomainCondition()
{
	m_iPlayerLimit.insert(m_iPlayerLimit.begin(), NUM_PROFESSION, 20);
	m_iGhostLimit.insert(m_iGhostLimit.begin(), NUM_PROFESSION, 0);
}

CDlgDomainCondition::~CDlgDomainCondition()
{
}

bool CDlgDomainCondition::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	return true;
}

void CDlgDomainCondition::DoDataExchange(bool bSave)
{
	char szName[40];
	for(int i=0; i<NUM_PROFESSION; i++)
	{
		_snprintf(szName, 40, "Edt_Number%d", i+1);
		DDX_EditBox(bSave, szName, m_iPlayerLimit[i]);
		_snprintf(szName, 40, "Edt_Soul%d", i+1);
		DDX_EditBox(bSave, szName, m_iGhostLimit[i]);
	}
}

void CDlgDomainCondition::OnShowDialog()
{
	UpdateData(false);
}

void CDlgDomainCondition::OnCommand_Confirm(const char * szCommand)
{
	UpdateData();
	for( int i=0;i<NUM_PROFESSION;i++ )
	{
		a_Clamp(m_iPlayerLimit[i], 0, (int)MAX_PLAYER_COUNT);
		a_ClampFloor(m_iGhostLimit[i], 0);
	}

	int domain_id = GetGameUIMan()->m_pDlgKingCommand->GetDomainID();
	GetGameSession()->king_SetBattleLimit(domain_id, m_iPlayerLimit, m_iGhostLimit);
	Show(false);
}

void CDlgDomainCondition::SetLimitData(const abase::vector<int>& player_cnt, const abase::vector<int>& ghost)
{
	if( !player_cnt.size() && !ghost.size() )
	{
		for( int i=0;i<NUM_PROFESSION;i++ )
		{
			m_iGhostLimit[i] = 0;
			m_iPlayerLimit[i] = MAX_PLAYER_COUNT;
		}
	}
	else
	{
		for( size_t i=0;i<NUM_PROFESSION;i++ )
		{
			if( i < player_cnt.size() )
				m_iPlayerLimit[i] = player_cnt[i];
			if( i < ghost.size() )
				m_iGhostLimit[i] = ghost[i];
		}
	}
}
