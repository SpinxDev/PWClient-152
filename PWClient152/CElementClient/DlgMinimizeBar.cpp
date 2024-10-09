// Filename	: DlgMinimizeBar.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/14

#include "DlgMinimizeBar.h"
#include "EC_GameUIMan.h"
#include "AUICommon.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgMinimizeBar, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMinimizeBar, CDlgBase)

AUI_ON_EVENT("*",	WM_LBUTTONDBLCLK,	OnEventLButtonDBCLK)
AUI_ON_EVENT(NULL,	WM_LBUTTONDBLCLK,	OnEventLButtonDBCLK)

AUI_END_EVENT_MAP()

CDlgMinimizeBar::CDlgMinimizeBar()
{
	m_pTxt_Subject = NULL;
}

CDlgMinimizeBar::~CDlgMinimizeBar()
{
}

void CDlgMinimizeBar::OnCommandCancel(const char * szCommand)
{
	GetGameUIMan()->RespawnMessage();
}

bool CDlgMinimizeBar::OnInitDialog()
{
	DDX_Control("Txt_Subject", m_pTxt_Subject);
	SetCanMove(false);

	return true;
}

void CDlgMinimizeBar::OnTick()
{
	if( m_bFlash && (GetTickCount() / 600) % 2 )
		m_pTxt_Subject->SetColor(A3DCOLORRGB(128, 128, 128));
	else
		m_pTxt_Subject->SetColor(m_Info.color);
}

void CDlgMinimizeBar::OnEventLButtonDBCLK(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUIDIALOG pDlg = (PAUIDIALOG)GetDataPtr();
	if( pDlg )
	{
		GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(pDlg);
		pDlg->Show(true);
	}
}

void CDlgMinimizeBar::CopyData(CDlgMinimizeBar* pOther)
{
	SetDataPtr(pOther->GetDataPtr());
	m_pTxt_Subject->SetText(pOther->m_pTxt_Subject->GetText());
	m_bFlash = pOther->m_bFlash;
	m_Info = pOther->m_Info;
}

CDlgMinimizeBar* CECMiniBarMgr::FindBar(PAUIDIALOG pDlg)
{
	for(int i = 0; i < m_nTotalBars; i++ )
	{
		CDlgMinimizeBar* pThis = m_Bars[i];
		if( pThis->GetDataPtr() == pDlg )
			return pThis;
	}
	return NULL;
}

bool CECMiniBarMgr::RestoreDialog(PAUIDIALOG pDlg)
{
	for(int i = 0; i < m_nTotalBars; i++ )
	{
		CDlgMinimizeBar* pThis = m_Bars[i];
		if( pThis->GetDataPtr() == pDlg )
		{
			for(int j = i; j < m_nTotalBars - 1; j++ )
			{
				m_Bars[j]->CopyData(m_Bars[j + 1]);
			}
			m_nTotalBars--;
			m_Bars[m_nTotalBars]->Show(false);
			return true;
		}
	}

	return false;
}

void CECMiniBarMgr::FlashDialog(PAUIDIALOG pDlg)
{
	CDlgMinimizeBar* pThis = FindBar(pDlg);
	if( pThis) pThis->m_bFlash = true;
}

void CECMiniBarMgr::UpdateDialog(PAUIDIALOG pDlg, const CECMiniBarMgr::BarInfo& info)
{
	CDlgMinimizeBar* pThis = FindBar(pDlg);
	if (pThis) UpdateDialogImpl(pThis, info);
}

void CECMiniBarMgr::UpdateDialogImpl(CDlgMinimizeBar *pThis, const CECMiniBarMgr::BarInfo& info)
{
	if (!pThis) return;
	
	// set flash color
	pThis->m_pTxt_Subject->SetColor(info.color);
	pThis->m_pTxt_Subject->SetHint(info.subject);
	
	// set subject
	ACString strSubject;
	int nWidth, nHeight, nLines;
	AUI_GetTextRect(m_pUIMan->GetDefaultFont(), info.subject, nWidth, nHeight, nLines);
	if( nWidth <= pThis->m_pTxt_Subject->GetSize().cx )
	{
		strSubject = info.subject;
	}
	else
	{
		const ACString& strText = info.subject;
		int i;
		for( i = strText.GetLength() - 1; i > 0; i-- )
		{
			AUI_GetTextRect(m_pUIMan->GetDefaultFont(), strText.Left(i), 
				nWidth, nHeight, nLines);
			if( nWidth + 15 <= pThis->m_pTxt_Subject->GetSize().cx )
			{
				strSubject = strText.Left(i) + _AL("...");
				break;
			}
		}
	}
	pThis->m_pTxt_Subject->SetText(strSubject);
	
	// bind other data
	pThis->m_Info = info;
	
	ArrangeMinimizeBar(true);
}

bool CECMiniBarMgr::MinimizeDialog(PAUIDIALOG pDlg, const CECMiniBarMgr::BarInfo& info)
{
	CDlgMinimizeBar* pNew = GetAvailableBar();
	if(!pNew) 
		return false; // no available bar
	m_nTotalBars++;
	
	pNew->SetDataPtr(pDlg);
	pNew->m_bFlash = false;
	pNew->Show(true);
	pDlg->Show(false);

	UpdateDialogImpl(pNew, info);
	return true;
}

void CECMiniBarMgr::ArrangeMinimizeBar(bool bForceRearrange /* = false */)
{
	int i;

	PAUIDIALOG pDlgSystem = m_pUIMan->GetDialog("Win_Main");
	PAUIDIALOG pDlgSystem2 = m_pUIMan->GetDialog("Win_Main2");
	PAUIDIALOG pDlgSystem3 = m_pUIMan->GetDialog("Win_Main3");
	PAUIDIALOG pDlgSystem4 = m_pUIMan->GetDialog("Win_Main4");
	PAUIDIALOG pDlgSystem5 = m_pUIMan->GetDialog("Win_Main5");

	bool b = pDlgSystem2->IsShow() || pDlgSystem3->IsShow() || (pDlgSystem4 && pDlgSystem4->IsShow()) || (pDlgSystem5 && pDlgSystem5->IsShow());

	if( bForceRearrange || b != m_bSystemMenuExpand )
	{
		m_bSystemMenuExpand = b;
		CDlgMinimizeBar *pLast = *m_Bars.begin();
		if( m_bSystemMenuExpand )
			pLast->AlignTo(pDlgSystem,
				AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
				AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP,
				0 , -42 );
		else
			pLast->AlignTo(pDlgSystem,
				AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
				AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);

		CDlgMinimizeBar *pThis;
		for(i = 1; i < (int)m_Bars.size() ; i++ )
		{
			pThis = m_Bars[i];
			pThis->AlignTo(pLast,
				AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
				AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
			pLast = pThis;
		}
	}
}

CDlgMinimizeBar* CECMiniBarMgr::GetAvailableBar()
{
	// find a mini bar pointer that still available
	return m_nTotalBars < (int)m_Bars.size() ? m_Bars[m_nTotalBars] : NULL;
}

CECMiniBarMgr::CECMiniBarMgr()
:m_pUIMan(NULL)
,m_bSystemMenuExpand(false)
,m_nTotalBars(0)
{

}

bool CECMiniBarMgr::Init(AUIManager* pUIMan)
{
	ASSERT(pUIMan);
	if(!pUIMan) return false;

	m_pUIMan = pUIMan;
	m_Bars.clear();
	m_nTotalBars = 0;

	for (int i = 0; ; i++)
	{
		AString strDlgName;
		strDlgName.Format("Win_MinimizeBar%d", i);
		CDlgMinimizeBar* pDlg = dynamic_cast<CDlgMinimizeBar*>(m_pUIMan->GetDialog(strDlgName));
		if(!pDlg) break;

		m_Bars.push_back(pDlg);
	}
	
	PAUIDIALOG pDlgSystem = m_pUIMan->GetDialog("Win_Main");
	if(m_Bars.size() > 0)
	{
		(*m_Bars.begin())->AlignTo(pDlgSystem,
								  AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
								  AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
	}

	// whether we got any mini bar
	return m_Bars.size() > 0;
}