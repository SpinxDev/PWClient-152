// Filename	: DlgForceActivityIcon.cpp
// Creator	: Feng Ning
// Date		: 2011/10/25

#include "DlgForceActivityIcon.h"
#include "AUIManager.h"
#include "EC_GameSession.h"
#include "EC_ForceMgr.h"
#include "EC_HostPlayer.h"

#define DIAGRAM_TIMEOUT 600
#define DIAGRAM_CLICK_TIMEOUT 0
#define DIAGRAM_NOTREADY_TIMEOUT 10

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgForceActivityIcon, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgForceActivityIcon, CDlgBase)
AUI_ON_EVENT("Img_FortressStatus", WM_RBUTTONUP,		OnEvent_Icon)
AUI_END_EVENT_MAP()

CDlgForceActivityIcon::CDlgForceActivityIcon()
:m_LastTimeStamp(0)
,m_pIcon(NULL)
{
}

CDlgForceActivityIcon::~CDlgForceActivityIcon()
{
}

void CDlgForceActivityIcon::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

bool CDlgForceActivityIcon::OnInitDialog()
{
	if( !CDlgBase::OnInitDialog() )
		return false;

	m_LastTimeStamp = 0;
	m_pIcon = GetDlgItem("Img_FortressStatus");

	return true;
}

void CDlgForceActivityIcon::RefreshForceGlobalData(bool dataReady)
{
	int timeout = // if no global data available then update every 180 seconds
		dataReady ? DIAGRAM_TIMEOUT : DIAGRAM_NOTREADY_TIMEOUT;

	// check the timeout
	int cur = GetGame()->GetServerAbsTime();
	if( (cur - m_LastTimeStamp) >= timeout )
	{
		GetGameSession()->c2s_CmdGetForceGlobalData();
		m_LastTimeStamp = cur;
	}
}

void CDlgForceActivityIcon::OnTick()
{
	CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
	bool dataReady = (pMgr && !pMgr->IsForceGlobalDataEmpty());

	RefreshForceGlobalData(dataReady);

	// show proper hint string
	if(m_pIcon)
	{
		m_pIcon->SetHint(GetStringFromTable(dataReady ? 9423 : 9422));
	}
}

void CDlgForceActivityIcon::OnEvent_Icon(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
	bool dataReady = (pMgr && !pMgr->IsForceGlobalDataEmpty());

	if(dataReady)
	{
		// show the diagram
		PAUIDIALOG pDlgStatus = m_pAUIManager->GetDialog("Win_ForceActivity");
		if(pDlgStatus)
		{
			pDlgStatus->Show(!pDlgStatus->IsShow());
			pDlgStatus->AlignTo(this,
				AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT,
				AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM, 0, 0);
		}
	}

	int cur = GetGame()->GetServerAbsTime();
	if(cur - m_LastTimeStamp >= DIAGRAM_CLICK_TIMEOUT)
	{
		GetGameSession()->c2s_CmdGetForceGlobalData();
		m_LastTimeStamp = cur;
	}
}