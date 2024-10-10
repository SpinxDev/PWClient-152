// DlgNewReverb.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgNewReverb.h"
#include "Engine.h"
#include "FReverb.h"
#include "FReverbDataManager.h"
#include "Global.h"

using AudioEngine::ReverbDataManager;

// CDlgNewReverb dialog

IMPLEMENT_DYNAMIC(CDlgNewReverb, CBCGPDialog)

CDlgNewReverb::CDlgNewReverb(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgNewReverb::IDD, pParent)
	, m_strName(_T(""))
	, m_pSelectReverb(0)
{

}

CDlgNewReverb::~CDlgNewReverb()
{
}

void CDlgNewReverb::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_REVERBS, m_wndReverbCombo);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CDlgNewReverb, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgNewReverb::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNewReverb message handlers

BOOL CDlgNewReverb::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	ReverbDataManager* pReverbDataManager = Engine::GetInstance().GetEventSystem()->GetReverbDataManager();
	if(!pReverbDataManager)
		return FALSE;
	int iReverbNum = pReverbDataManager->GetReverbNum();
	for (int i=0; i<iReverbNum; ++i)
	{
		REVERB* pReverb = pReverbDataManager->GetReverbByIndex(i);
		m_wndReverbCombo.InsertString(i, pReverb->strName.c_str());
		m_wndReverbCombo.SetItemData(i, (DWORD_PTR)pReverb);
	}
	m_wndReverbCombo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNewReverb::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(!GF_CheckName(m_strName, GetSafeHwnd()))
		return;
	if(isExist(m_strName))
	{
		MessageBox("存在同名Reverb", "警告", MB_ICONWARNING);
		return;
	}
	int iCurSel = m_wndReverbCombo.GetCurSel();
	if(iCurSel == -1)
		return;
	m_pSelectReverb = (REVERB*)m_wndReverbCombo.GetItemData(iCurSel);
	if(!m_pSelectReverb)
		return;
	CBCGPDialog::OnOK();
}

REVERB* CDlgNewReverb::GetSelectReverb() const
{
	return m_pSelectReverb;
}

const char* CDlgNewReverb::GetName() const
{
	return m_strName;
}

bool CDlgNewReverb::isExist(const char* szName)
{
	ReverbDataManager* pReverbDataManager = Engine::GetInstance().GetEventSystem()->GetReverbDataManager();
	int iReverbNum = pReverbDataManager->GetReverbNum();
	for (int i=0; i<iReverbNum; ++i)
	{
		REVERB* pReverb = pReverbDataManager->GetReverbByIndex(i);
		if(GF_CompareNoCase(pReverb->strName.c_str(), strlen(pReverb->strName.c_str()), szName, strlen(szName)))
		{
			return true;
		}
	}
	return false;
}