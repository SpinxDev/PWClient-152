// File		: DlgDoubleExpS.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/15

#include "DlgDoubleExpS.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_UIConfigs.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgDoubleExpS, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgDoubleExpS::CDlgDoubleExpS()
{
	m_pImg_Icon = NULL;
	m_pImg_Captivation = NULL;
	m_pTxt_RestTime = NULL;
	m_nMode = 0;
}

CDlgDoubleExpS::~CDlgDoubleExpS()
{
}

void CDlgDoubleExpS::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->RespawnMessage();
}

bool CDlgDoubleExpS::OnInitDialog()
{
	DDX_Control("Img_Icon", m_pImg_Icon);
	DDX_Control("Img_Captivation", m_pImg_Captivation);
	DDX_Control("Txt_RestTime", m_pTxt_RestTime);

	return true;
}

void CDlgDoubleExpS::OnShowDialog()
{
	m_pImg_Icon->Show(false);
	m_pImg_Captivation->Show(false);
	m_pTxt_RestTime->Show(false);
}

void CDlgDoubleExpS::OnTick()
{
	int nRestTime = GetGameRun()->GetRemainDblExpTime();
	if( GetGameRun()->GetDoubleExpMode() != CECGameRun::DBEXP_NONE && nRestTime > 0 )
	{
		m_pImg_Icon->Show(true);
		m_pTxt_RestTime->Show(true);
	}
	else
	{
		m_pImg_Icon->Show(false);
		m_pTxt_RestTime->Show(false);
	}

	if( GetGameRun()->GetWallowInfo().anti_wallow_active )
	{
		m_pImg_Captivation->Show(CECUIConfig::Instance().GetGameUI().nWallowHintType != CECUIConfig::GameUI::WHT_KOREA);
	}
	else
		m_pImg_Captivation->Show(false);
		
	switch( GetGameRun()->GetDoubleExpMode() )
	{
	case CECGameRun::DBEXP_OWN:
		m_pImg_Icon->FixFrame(1);
		m_pImg_Icon->SetHint(GetStringFromTable(787));
		break;
	case CECGameRun::DBEXP_GLOBAL:
		m_pImg_Icon->FixFrame(0);
		m_pImg_Icon->SetHint(GetStringFromTable(788));
		break;
	}
	int nHour = nRestTime / 3600;
	int nMin = (nRestTime % 3600) / 60;
	int nSec = nRestTime % 60;
	ACString strText;
	if( nRestTime < 60 )
		strText.Format(GetStringFromTable(1601), nRestTime);
	else if( nHour == 0 )
		strText.Format(GetStringFromTable(1602), nMin);
	else
		strText.Format(GetStringFromTable(1611), nHour, nMin);
	m_pTxt_RestTime->SetText(strText);
}
