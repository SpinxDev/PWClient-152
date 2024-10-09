#include "DlgFortressContrib.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressContrib, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Contrib", OnCommand_Contrib)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressContrib, CDlgBase)
AUI_ON_EVENT("Edit_01",	WM_KEYDOWN,	OnEventKeyDown_Edit_01)
AUI_END_EVENT_MAP()

CDlgFortressContrib::CDlgFortressContrib()
{
}

bool CDlgFortressContrib::OnInitDialog()
{
	m_pLab_Contrib= GetDlgItem("Lab_Contrib");
	m_pEdit_01= GetDlgItem("Edit_01");
	m_pBtn_Contrib= GetDlgItem("Btn_Contrib");
	return true;
}

void CDlgFortressContrib::OnShowDialog()
{
	UpdateInfo();
}

void CDlgFortressContrib::OnTick()
{
	m_pBtn_Contrib->Enable(GetContribToSend() > 0);
}

void CDlgFortressContrib::UpdateInfo()
{
	m_pLab_Contrib->SetText(_AL(""));
	m_pEdit_01->SetText(_AL(""));
	m_pBtn_Contrib->Enable(false);

	const CECHostPlayer::CONTRIB_INFO *pContrib = GetHostPlayer()->GetContribInfo();
	if (pContrib)
	{
		ACString strText;

		strText.Format(_AL("%d"), pContrib->exp_contrib);
		m_pLab_Contrib->SetText(strText);
	}

	//	设置输入焦点
	ChangeFocus(m_pEdit_01);
}

void CDlgFortressContrib::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
	
	//	关闭相关对话框
	PAUIDIALOG pDlgInfo = GetGameUIMan()->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->OnCommand("IDCANCEL");
	
	//	关闭 NPC 服务
	if (GetGameUIMan()->m_pCurNPCEssence != NULL)
		GetGameUIMan()->EndNPCService();
}

void CDlgFortressContrib::OnCommand_Contrib(const char *szCommand)
{
	int nContrib = GetContribToSend();
	if (nContrib <= 0)
	{
		ChangeFocus(m_pEdit_01);
		return;
	}
	GetGameSession()->c2s_CmdNPCSevFactionFortressHandInContrib(nContrib);
}

int CDlgFortressContrib::GetContribToSend()
{
	int nContrib = 0;
	const CECHostPlayer::CONTRIB_INFO *pContrib = GetHostPlayer()->GetContribInfo();
	if (pContrib)
	{
		ACString strNumber = m_pEdit_01->GetText();
		nContrib = strNumber.ToInt();
		if (nContrib <= 0 || nContrib > pContrib->exp_contrib)
		{
			nContrib = 0;
		}
	}
	return nContrib;
}

void CDlgFortressContrib::OnEventKeyDown_Edit_01(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_ESCAPE)
	{
		ChangeFocus(NULL);
		return;
	}
}