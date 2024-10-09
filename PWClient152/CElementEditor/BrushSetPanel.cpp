// BrushSetPanel.cpp : implementation file
//
#include "Global.h"
#include "stdafx.h"
#include "MainFrm.h"
#include "elementeditor.h"
#include "BrushSetPanel.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CBrushSetPanel dialog


CBrushSetPanel::CBrushSetPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CBrushSetPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBrushSetPanel)
	//}}AFX_DATA_INIT
}


void CBrushSetPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrushSetPanel)
	DDX_Control(pDX, IDC_SPIN_BRUSHRADIUS, m_spinBrushRadius);
	DDX_Control(pDX, IDC_EDIT_BRUSHRADIUS, m_editBrushRadius);
	DDX_Control(pDX, IDC_SLIDER_BRUSHRADIUS, m_sliderBrushRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrushSetPanel, CDialog)
	//{{AFX_MSG_MAP(CBrushSetPanel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRUSHRADIUS, OnDeltaposSpinBrushradius)
	ON_EN_CHANGE(IDC_EDIT_BRUSHRADIUS, OnChangeEditBrushradius)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRUSHRADIUS, OnReleasedcaptureSliderBrushradius)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SLIDER_BRUSHRADIUS, OnOutofmemorySliderBrushradius)
	ON_BN_CLICKED(IDC_RADIO_BRUSHCIRCLE, OnRadioBrushcircle)
	ON_BN_CLICKED(IDC_RADIO_BRUSHRECT, OnRadioBrushrect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrushSetPanel message handlers

BOOL CBrushSetPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//	Change edit boxes to flat style
	AUX_FlatEditBox(&m_editBrushRadius);
	
	CString str;
	str.Format("%d", g_Configs.iBrushRadius);
	m_editBrushRadius.SetWindowText(str);

	m_sliderBrushRadius.SetRange(1,100);


	CButton * temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHCIRCLE);
	
	if(g_Configs.bCircleBrush)
	{
		if(temp)
			temp->SetCheck(true);
	}else
	{
		temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHRECT);
		if(temp)
			temp->SetCheck(true);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBrushSetPanel::OnDeltaposSpinBrushradius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	g_Configs.iBrushRadius += -pNMUpDown->iDelta*1;
	if(g_Configs.iBrushRadius>100)
		g_Configs.iBrushRadius = 100;
	else if(g_Configs.iBrushRadius<1)
		g_Configs.iBrushRadius = 1;
		
	
	CString str;
	str.Format("%d", g_Configs.iBrushRadius);
	m_editBrushRadius.SetWindowText(str);

	m_sliderBrushRadius.SetPos(g_Configs.iBrushRadius);

	CMainFrame* p = AUX_GetMainFrame();
	if(p) p->UpdateBrushWidth();
	*pResult = 0;
}

void CBrushSetPanel::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	CString str;
	m_editBrushRadius.GetWindowText(str);
	g_Configs.iBrushRadius = (int)atof(str);

	str.Format("%d", g_Configs.iBrushRadius);
	m_editBrushRadius.SetWindowText(str);
	m_sliderBrushRadius.SetPos(g_Configs.iBrushRadius);

	//	Activate main window
	AUX_GetMainFrame()->SetFocus();
}

void CBrushSetPanel::OnChangeEditBrushradius() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	m_editBrushRadius.GetWindowText(str);
	g_Configs.iBrushRadius = (int)atof(str);
	if(g_Configs.iBrushRadius>100) 
	{
		g_Configs.iBrushRadius = 100;
		str.Format("%d", g_Configs.iBrushRadius);
		m_editBrushRadius.SetWindowText(str);
	}else if(g_Configs.iBrushRadius<1)
	{
		g_Configs.iBrushRadius = 1;
		str.Format("%d", g_Configs.iBrushRadius);
		m_editBrushRadius.SetWindowText(str);
	}
	
	m_sliderBrushRadius.SetPos(g_Configs.iBrushRadius);
	CMainFrame* p = AUX_GetMainFrame();
	if(p) p->UpdateBrushWidth();
}

void CBrushSetPanel::OnReleasedcaptureSliderBrushradius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	g_Configs.iBrushRadius = m_sliderBrushRadius.GetPos();
	CString str;
	str.Format("%d", g_Configs.iBrushRadius);
	m_editBrushRadius.SetWindowText(str);
	
	CMainFrame* p = AUX_GetMainFrame();
	if(p) p->UpdateBrushWidth();
	*pResult = 0;
}

void CBrushSetPanel::OnOutofmemorySliderBrushradius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CBrushSetPanel::OnRadioBrushcircle() 
{
	// TODO: Add your control notification handler code here
	CButton * temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHCIRCLE);
	if(temp)
	{
		temp->SetCheck(true);
	}
	temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHRECT);
	if(temp)
		temp->SetCheck(false);

	g_Configs.bCircleBrush = true;
	
}

void CBrushSetPanel::OnRadioBrushrect() 
{
	// TODO: Add your control notification handler code here
	CButton * temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHCIRCLE);
	if(temp)
	{
		temp->SetCheck(false);
	}
	temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHRECT);
	if(temp)
		temp->SetCheck(true);

	g_Configs.bCircleBrush = false;
}

void CBrushSetPanel::UpdateUI()
{
	CString str;
	str.Format("%d", g_Configs.iBrushRadius);
	m_editBrushRadius.SetWindowText(str);

	CButton * temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHCIRCLE);
	
	if(g_Configs.bCircleBrush)
	{
		if(temp) temp->SetCheck(true);
		temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHRECT);
		if(temp) temp->SetCheck(false);
	}else
	{
		if(temp) temp->SetCheck(false);
		temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHRECT);
		if(temp)
			temp->SetCheck(true);
	}
}
