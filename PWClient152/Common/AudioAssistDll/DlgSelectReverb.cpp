// DlgSelectReverb.cpp : implementation file
//

#include "stdafx.h"
#include "AudioAssistDll.h"
#include "DlgSelectReverb.h"
#include "FReverbDataManager.h"
#include "FEventSystem.h"

// CDlgSelectReverb dialog

IMPLEMENT_DYNAMIC(CDlgSelectReverb, CDialog)

CDlgSelectReverb::CDlgSelectReverb(EventSystem* pEventSystem, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectReverb::IDD, pParent)
	, m_pSelectedReverb(0)
{
	m_pReverbDataManager = pEventSystem->GetReverbDataManager();
}

CDlgSelectReverb::~CDlgSelectReverb()
{
}

void CDlgSelectReverb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REVERB, m_wndReverbListBox);
}


BEGIN_MESSAGE_MAP(CDlgSelectReverb, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSelectReverb::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelectReverb message handlers

BOOL CDlgSelectReverb::OnInitDialog()
{
	CDialog::OnInitDialog();
	while(m_wndReverbListBox.GetCount())
	{
		m_wndReverbListBox.DeleteString(0);
	}
	int iReverbNum = m_pReverbDataManager->GetReverbNum();
	for (int i=0; i<iReverbNum; ++i)
	{
		REVERB* pReverb = m_pReverbDataManager->GetReverbByIndex(i);
		m_wndReverbListBox.InsertString(i, pReverb->strName.c_str());
		m_wndReverbListBox.SetItemData(i, (DWORD_PTR)pReverb);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgSelectReverb::Load(const char* szPath)
{
	if(!m_pReverbDataManager)
		return false;
	if(!m_pReverbDataManager->LoadXML(szPath))
		return false;
	return true;
}

void CDlgSelectReverb::OnBnClickedOk()
{
	int iCurSel = m_wndReverbListBox.GetCurSel();
	if(iCurSel == -1)
	{
		MessageBox("请选择一个Reverb", "警告", MB_ICONWARNING);
		return;
	}
	m_pSelectedReverb = (REVERB*)m_wndReverbListBox.GetItemData(iCurSel);
	OnOK();
}

REVERB* CDlgSelectReverb::GetSelectedReverb() const
{
	return m_pSelectedReverb;
}