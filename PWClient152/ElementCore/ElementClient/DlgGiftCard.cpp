// Filename	: DlgGiftCard.cpp
// Creator	: Han Guanghui
// Date		: 2013/07/30

#include "DlgGiftCard.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGiftCard, CDlgBase)
AUI_ON_COMMAND("Btn_Confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgGiftCard::CDlgGiftCard()
{
	m_pTxt_Input = NULL;
}

CDlgGiftCard::~CDlgGiftCard()
{
}

bool CDlgGiftCard::OnInitDialog()
{
	DDX_Control("txt_cardnumber", m_pTxt_Input);

	return true;
}

void CDlgGiftCard::OnCommandConfirm(const char* szCommand)
{
	ACString strCode = m_pTxt_Input->GetText();
	if (strCode.GetLength() != 20) {
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10819), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	GetGameSession()->c2s_CmdNPCSevGiftCard(AC2AS(strCode));
	GetGameUIMan()->EndNPCService();
	Show(false);
}

void CDlgGiftCard::OnShowDialog()
{
	m_pTxt_Input->SetText(_AL(""));
	ChangeFocus(m_pTxt_Input);
}
void CDlgGiftCard::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
}
