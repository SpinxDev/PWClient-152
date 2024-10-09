// Filename	: DlgKingCommand.cpp
// Creator	: Shi && Wang
// Date		: 2013/1/29

#include "DlgKingCommand.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "AUIManager.h"
#include "DlgDomainCondition.h"
#include "DlgCountryMap.h"
#include "EC_DomainCountry.h"
#include "EC_UIConfigs.h"
#include "globaldataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgKingCommand, CDlgBase)

AUI_ON_COMMAND("Btn_Order*", OnCommand_Order)

AUI_END_COMMAND_MAP()

CDlgKingCommand::CDlgKingCommand()
{
	m_pLblScore = NULL;
	m_pBtnUse1 = NULL;
	m_pBtnUse2 = NULL;
	m_pBtnCancel = NULL;
}

CDlgKingCommand::~CDlgKingCommand()
{
}

bool CDlgKingCommand::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	DDX_Control("Lbl_Point", m_pLblScore);
	DDX_Control("Btn_Order1", m_pBtnUse1);
	DDX_Control("Btn_Order2", m_pBtnUse2);
	DDX_Control("Btn_Order4", m_pBtnCancel);

	return true;
}

void CDlgKingCommand::OnShowDialog()
{
	const DOMAIN2_INFO* info = CECDomainCountryInfo::GetInstance()->Find(m_iDomainID);
	if( info && (info->byStateMask & DOMAIN2_INFO::STATE_RESTRICT) )
		m_pBtnCancel->Enable(true);
	else
		m_pBtnCancel->Enable(false);
}

void CDlgKingCommand::OnTick()
{
	ACString str;
	int iScore = GetHostPlayer()->GetKingPoint();
	str.Format(_AL("%d"), iScore);
	m_pLblScore->SetText(str);

	m_pBtnUse1->Enable(iScore >= 50);
	m_pBtnUse2->Enable(iScore >= 100);
}

void CDlgKingCommand::OnCommand_Order(const char * szCommand)
{
	int iOrder = atoi(szCommand + strlen("Btn_Order"));
	if( iOrder == 1 || iOrder == 2 )
	{
		bool bUseBig = (iOrder == 2);

		if( (bUseBig && GetHostPlayer()->GetKingPoint() < 100) ||
			(!bUseBig && GetHostPlayer()->GetKingPoint() < 50) )
		{
			m_pAUIManager->MessageBox("", GetStringFromTable(10309), MB_OK, A3DCOLORRGB(255, 255, 255));
			return;
		}

		PAUIDIALOG pDlg;
		GetGameUIMan()->MessageBox("Game_AssignAssault", GetStringFromTable(10321), MB_OKCANCEL, A3DCOLORRGB(255, 255, 255), &pDlg);
		pDlg->SetData(bUseBig ? 1 : 0);
	}
	else if( iOrder == 3 )
	{
		const DOMAIN2_INFO* info = CECDomainCountryInfo::GetInstance()->Find(m_iDomainID);

		int nMaxLimit = CECUIConfig::Instance().GetGameUI().nCountryWarKingMaxDomainLimit;
		if( GetGameUIMan()->m_pDlgCountryMap->GetLimitDomainCount() >= nMaxLimit &&
			!(info->byStateMask & DOMAIN2_INFO::STATE_RESTRICT) )
		{
			ACString strMsg;
			strMsg.Format(GetStringFromTable(10319), nMaxLimit);
			GetGameUIMan()->MessageBox("", strMsg, MB_OK, A3DCOLORRGB(255, 255, 255));
			return;
		}

		if( info->byStateMask & DOMAIN2_INFO::STATE_RESTRICT )
		{
			ASSERT(info->iPlayer.size() >= NUM_PROFESSION);
			ASSERT(info->iGhost.size() >= NUM_PROFESSION);
			GetGameUIMan()->m_pDlgDomainCondition->SetLimitData(info->iPlayer, info->iGhost);
		}
		else
		{
			abase::vector<int> empty_vec;
			GetGameUIMan()->m_pDlgDomainCondition->SetLimitData(empty_vec, empty_vec);
		}

		GetGameUIMan()->m_pDlgDomainCondition->Show(true);
		Show(false);
	}
	else if( iOrder == 4 )
	{
		// ·¢Ð­Òé
		GetGameUIMan()->MessageBox("Game_CancelDomainLimit", GetStringFromTable(10318), MB_OKCANCEL, A3DCOLORRGB(255, 255, 255));
	}
}

void CDlgKingCommand::DoAssignAssault( bool bBig )
{
	GetGameSession()->king_AssignAssault(m_iDomainID, bBig);
	Show(false);
}

void CDlgKingCommand::DoCancelDomainLimit()
{
	GetGameSession()->king_ClearBattleLimit(m_iDomainID);
	Show(false);
}