// DlgSoundDefPropSet.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSoundDefPropSet.h"

#include "PropListCtrl/BCGPPropertyListWrapper.h"
#include "PropListCtrl/BCGPPropertyObject.h"

// CDlgSoundDefPropSet dialog

IMPLEMENT_DYNAMIC(CDlgSoundDefPropSet, CBCGPDialog)

CDlgSoundDefPropSet::CDlgSoundDefPropSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSoundDefPropSet::IDD, pParent)
{
	m_pBCGPPropListWrapper = new CBCGPPropertyListWrapper;
	m_pPropertyListener = new CPropertyUpdateListener(this);
	m_pProperties = new CPropertyArray;
	m_pProperties->AddListener(m_pPropertyListener);
}

CDlgSoundDefPropSet::~CDlgSoundDefPropSet()
{
	delete m_pPropertyListener;
	m_pPropertyListener = 0;
	if (m_pProperties)
		m_pProperties->Clear();
	delete m_pProperties;
	m_pProperties = 0;
	delete m_pBCGPPropListWrapper;
	m_pBCGPPropListWrapper = 0;
}

void CDlgSoundDefPropSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgSoundDefPropSet, CBCGPDialog)
END_MESSAGE_MAP()


// CDlgSoundDefPropSet message handlers

BOOL CDlgSoundDefPropSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();

	CRect rc;
	GetDlgItem(IDC_SOUNDDEF_PROP_FRM)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(1,1,1,1);

	if (!m_pBCGPPropListWrapper->OnCreate(rc, this))
		return FALSE;	

	m_pProperties->Clear();

	memset(&m_SoundDefProperty, 0, sizeof(m_SoundDefProperty));

	if(!m_SoundDefPropList.Init(m_pProperties, m_pBCGPPropListWrapper))
		return FALSE;
	if(!m_SoundDefPropList.BuildProperty(m_SoundDefProperty))
		return FALSE;

	m_pBCGPPropListWrapper->AttachTo(m_pProperties);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSoundDefPropSet::UpdateProperty(bool bSave, bool bIsCommit /*= false*/)
{
	m_SoundDefPropList.PropChanged(m_SoundDefProperty, bSave, bIsCommit, 0);
}