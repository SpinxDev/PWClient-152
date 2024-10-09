#include "DlgGMFinishMsg.h"
#include "AUIListBox.h"
#include "AUIEditBox.h"
#include "AUILabel.h"
#include "CodeTemplate.h"
#include "DlgGMConsole.h"
#include "GMCommandListBox.h"
#include "GMCDatabase.h"
#include "EC_GameUIMan.h"


AUI_BEGIN_COMMAND_MAP(CDlgGMFinishMsg, CDlgBase)

AUI_ON_COMMAND("ok", OnCommandOk)
AUI_ON_COMMAND("cancel", OnCommandCancel)
AUI_ON_COMMAND("select_command", OnCommandDblClickCommand)
AUI_ON_COMMAND("IDCANCEL", OnCommandIDCancel)

AUI_END_COMMAND_MAP()

CDlgGMFinishMsg::CDlgGMFinishMsg() :
	m_pLbxCommand(NULL),
	m_pLblMsg(NULL),
	m_pGMCommandListBox(NULL),
	m_dwItemIDInMsgListBox(-1),
	m_bDataReady(false)
{

}

CDlgGMFinishMsg::~CDlgGMFinishMsg()
{
	
}


void CDlgGMFinishMsg::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("lbxCommand", m_pLbxCommand);
	DDX_Control("lblMsg", m_pLblMsg);
}

void CDlgGMFinishMsg::SetAssocData(GMCDatabase *pDatabase, ACString strMsg, int nPlayerID, SimpleDB::DateTime * pdtReceivedMsg, DWORD dwItemIDInMsgListBox)
{
	ASSERT(m_bDataReady == false);

	if (!m_pGMCommandListBox)
	{
		m_pGMCommandListBox = new CGMCommandListBox;
		m_pGMCommandListBox->Init(m_pLbxCommand, this);
	}
	m_pGMCommandListBox->SetAssocData(pDatabase, nPlayerID, pdtReceivedMsg);

	m_pLblMsg->SetText(strMsg);
	m_dwItemIDInMsgListBox = dwItemIDInMsgListBox;

	m_bDataReady = true;
}

bool CDlgGMFinishMsg::Release()
{
//	SAFE_RELEASE(m_pGMCommandListBox);
	if (m_pGMCommandListBox)
	{
		m_pGMCommandListBox->Release();
		delete m_pGMCommandListBox;
		m_pGMCommandListBox = NULL;
	}
	m_dwItemIDInMsgListBox = -1;
	m_pLblMsg->SetText(_AL(""));
	m_bDataReady = false;

	return CDlgBase::Release();
}

void CDlgGMFinishMsg::OnCommandOk(const char * szCommand)
{
	CDlgGMConsole *pDlgGMConsole = GetGameUIMan()->m_pDlgGMConsole;
	pDlgGMConsole->ConfirmCommandFinish(m_dwItemIDInMsgListBox, true);
	
	Show(false);
	m_bDataReady = false;
}

void CDlgGMFinishMsg::OnCommandCancel(const char * szCommand)
{
	Show(false);
	m_bDataReady = false;
	GetGameUIMan()->m_pDlgGMConsole->ConfirmCommandFinish(m_dwItemIDInMsgListBox, false);
}

void CDlgGMFinishMsg::OnCommandDblClickCommand(const char * szCommand)
{
	m_pGMCommandListBox->OnDblClick();	
}

void CDlgGMFinishMsg::OnCommandIDCancel(const char * szCommand)
{
	OnCommandCancel(NULL);
}
