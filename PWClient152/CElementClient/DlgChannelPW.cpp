// Filename	: DlgChannalCreate.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#include "DlgChannelPW.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgChannelPW, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)

AUI_END_COMMAND_MAP()

CDlgChannelPW::CDlgChannelPW()
{
	m_pTxt_Input = NULL;
}

CDlgChannelPW::~CDlgChannelPW()
{
}

bool CDlgChannelPW::OnInitDialog()
{
	DDX_Control("DEFAULT_Txt_Input", m_pTxt_Input);

	return true;
}

void CDlgChannelPW::OnCommandConfirm(const char* szCommand)
{
	GetGameSession()->chatroom_Join(GetData("int"), m_pTxt_Input->GetText());
	Show(false);
}

void CDlgChannelPW::OnShowDialog()
{
	if( IsShow() )
	{
		m_pTxt_Input->SetText(_AL(""));
	}
}
