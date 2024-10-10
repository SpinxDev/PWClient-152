/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   18:15
	file name:	DlgReportToGM.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "DlgReportToGM.h"
#include "EC_GameUIMan.h"
#include "AUILabel.h"
#include "EC_GameSession.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgReportToGM, CDlgBase)

AUI_ON_COMMAND("confirm", OnCommandConfirm)

AUI_END_COMMAND_MAP()

CDlgReportToGM::CDlgReportToGM() :
	m_pLblName(NULL),
	m_pEbxContent(NULL)
{

}

CDlgReportToGM::~CDlgReportToGM()
{

}

void CDlgReportToGM::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Txt_Name", m_pLblName);
	DDX_Control("DEFAULT_Txt_Input", m_pEbxContent);
	DDX_EditBox(bSave, "DEFAULT_Txt_Input", m_strContent);

}

bool CDlgReportToGM::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	m_pEbxContent->SetIsAutoReturn(true);
	return true;
}

void CDlgReportToGM::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_strContent.Empty();
	UpdateData(false);
}

void CDlgReportToGM::OnCommandConfirm(const char *szCommand)
{
	UpdateData(true);
	m_strContent.TrimLeft(); m_strContent.TrimRight();
	if (m_strContent.IsEmpty()) return;

	ACString strName = GetDlgItem("Txt_Name")->GetText();
	GetGameSession()->ImpeachPlayerToGM(strName, m_strContent);
	SetData(GetTickCount());
	GetGameUIMan()->AddChatMessage(
		GetGameUIMan()->GetStringFromTable(569), GP_CHAT_MISC);
	
	Show(false);
}

void CDlgReportToGM::SetNameLabel(const ACHAR *szName)
{
	m_pLblName->SetText(szName);
}
