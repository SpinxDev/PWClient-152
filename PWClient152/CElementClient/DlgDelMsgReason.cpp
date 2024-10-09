#include <AUIComboBox.h>
#include "DlgGMDelMsgReason.h"
#include "DlgGMConsole.h"
#include "EC_GameUIMan.h"


const int CDlgGMDelMsgReason::predefinedReason[] =
{
	6001, //_AL("自定义"),
	6002, //_AL("过期，无法处理"),
	6003, //_AL("无理要求，不予处理"),
	0
};

AUI_BEGIN_COMMAND_MAP(CDlgGMDelMsgReason, CDlgBase)

AUI_ON_COMMAND("ok", OnCommandOK)
AUI_ON_COMMAND("cancel", OnCommandCancel)
AUI_ON_COMMAND("select_reason", OnCommandSelectReason)

AUI_END_COMMAND_MAP()

CDlgGMDelMsgReason::CDlgGMDelMsgReason() :
	m_pCbxReason(NULL)
{

}


bool CDlgGMDelMsgReason::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	const int * pReason = predefinedReason;
	while (*pReason != 0)
	{
		m_pCbxReason->AddString(GetStringFromTable(*(pReason++)));
	}
	return true;
}

void CDlgGMDelMsgReason::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("cbxReason", m_pCbxReason);

	DDX_ComboBox(bSave, "cbxReason", m_nSelReasonIndex);
	DDX_EditBox(bSave, "ebxReason", m_strCustomReason);
}

void CDlgGMDelMsgReason::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_nSelReasonIndex = 0;
	m_strCustomReason.Empty();
	UpdateData(false);
}

void CDlgGMDelMsgReason::OnCommandOK(const char * szCommand)
{
	UpdateData(true);
	if (0 == m_nSelReasonIndex)
	{
		if (m_strCustomReason.IsEmpty())
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(6005), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
	}

	CDlgGMConsole *pConsoleDlg = GetGameUIMan()->m_pDlgGMConsole;
	pConsoleDlg->ConfiremCommandIgnore(m_nSelReasonIndex, m_strCustomReason);
	Show(false);
}

void CDlgGMDelMsgReason::OnCommandCancel(const char * szCommand)
{
	Show(false);
}


void CDlgGMDelMsgReason::OnCommandSelectReason(const char * szCommand)
{
	UpdateData(true);
	if (0 != m_nSelReasonIndex)
	{
		m_strCustomReason.Empty();
		UpdateData(false);
	}
}
