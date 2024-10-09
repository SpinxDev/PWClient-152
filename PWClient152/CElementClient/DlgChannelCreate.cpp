// Filename	: DlgChannalCreate.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#include "DlgChannelCreate.h"
#include "DlgChannelChat.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Utility.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgChannelCreate, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("enablepw",		OnCommandEnablePW)

AUI_END_COMMAND_MAP()

CDlgChannelCreate::CDlgChannelCreate()
{
	m_pChk_EnablePw = NULL;
	m_pTxt_ChName = NULL;
	m_pTxt_ChPw = NULL;
	m_pBtn_Create = NULL;
}

CDlgChannelCreate::~CDlgChannelCreate()
{
}

bool CDlgChannelCreate::OnInitDialog()
{
	DDX_Control("Chk_EnablePw", m_pChk_EnablePw);
	DDX_Control("DEFAULT_Txt_ChName", m_pTxt_ChName);
	DDX_Control("Txt_ChPw", m_pTxt_ChPw);
	DDX_Control("Btn_Create", m_pBtn_Create);

	return true;
}

void CDlgChannelCreate::OnCommandConfirm(const char* szCommand)
{
	if( glb_IsTextNotEmpty(m_pTxt_ChName))
	{
		if( GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0 &&
			GetGameUIMan()->m_pDlgChannelChat->IsCreator() )
		{
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("Game_ChannelCreate", GetStringFromTable(654), MB_YESNO,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			ACString strText;
			strText.Format(_AL("%s\r%s"), m_pTxt_ChName->GetText(), m_pTxt_ChPw->GetText());
			pMsgBox->SetText(strText);
		}
		else
			GetGameSession()->chatroom_Create(m_pTxt_ChName->GetText(), m_pTxt_ChPw->GetText(), 30);
		Show(false);
	}
}

void CDlgChannelCreate::OnCommandEnablePW(const char* szCommand)
{
	m_pTxt_ChPw->Enable(m_pChk_EnablePw->IsChecked());
	m_pTxt_ChPw->SetText(_AL(""));
}

void CDlgChannelCreate::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_pTxt_ChName->SetText(_AL(""));
	m_pChk_EnablePw->Check(false);
	m_pTxt_ChPw->Enable(false);
	m_pTxt_ChPw->SetText(_AL(""));
	m_pBtn_Create->Enable(false);
}

void CDlgChannelCreate::OnTick()
{
	m_pBtn_Create->Enable(glb_IsTextNotEmpty(m_pTxt_ChName));
}
