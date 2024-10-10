// DlgEventPropSet.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgEventPropSet.h"

#include "PropListCtrl/BCGPPropertyListWrapper.h"
#include "PropListCtrl/BCGPPropertyObject.h"

using AudioEngine::MODE_2D;
using AudioEngine::ROLLOFF_INVERSE;
using AudioEngine::MPB_STEAL_OLDEST;


// CDlgEventPropSet dialog

IMPLEMENT_DYNAMIC(CDlgEventPropSet, CBCGPDialog)

CDlgEventPropSet::CDlgEventPropSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgEventPropSet::IDD, pParent)
{
	m_pBCGPPropListWrapper = new CBCGPPropertyListWrapper;
	m_pPropertyListener = new CPropertyUpdateListener(this);
	m_pProperties = new CPropertyArray;
	m_pProperties->AddListener(m_pPropertyListener);
}

CDlgEventPropSet::~CDlgEventPropSet()
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

void CDlgEventPropSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEventPropSet, CBCGPDialog)
END_MESSAGE_MAP()


// CDlgEventPropSet message handlers

BOOL CDlgEventPropSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();

	CRect rc;
	GetDlgItem(IDC_EVENT_PROP_FRM)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(1,1,1,1);

	if (!m_pBCGPPropListWrapper->OnCreate(rc, this))
		return FALSE;	

	m_pProperties->Clear();

	memset(&m_EventProperty, 0, sizeof(m_EventProperty));
	m_EventProperty.bOneShot = true;
	m_EventProperty.fMinDistance = 1.0f;
	m_EventProperty.fMaxDistance = 100.0f;
	m_EventProperty.mode = MODE_2D;
	m_EventProperty.rolloff = ROLLOFF_INVERSE;
	m_EventProperty.maxPlaybacksBehavior = MPB_STEAL_OLDEST;
	m_EventProperty.uiMaxPlaybacks = 10;
	m_EventProperty.iPriority = 10000;
	m_EventProperty.f3DConeInsideAngle = 360.0f;
	m_EventProperty.f3DConeOutsideAngle = 360.0f;
	m_EventProperty.f3DConeOutsideVolume = 1.0f;
 	if(!m_EventPropList.Init(m_pProperties, m_pBCGPPropListWrapper))
		return FALSE;
	if(!m_EventPropList.BuildProperty(m_EventProperty))
		return FALSE;

	m_pBCGPPropListWrapper->AttachTo(m_pProperties);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEventPropSet::UpdateProperty(bool bSave, bool bIsCommit /*= false*/)
{
	m_EventPropList.PropChanged(m_EventProperty, bSave, bIsCommit, 0);
}