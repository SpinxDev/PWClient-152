// DlgSlider.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSlider.h"
#include "..\AudioEditor.h"


// CDlgSlider dialog

IMPLEMENT_DYNAMIC(CDlgSlider, CBCGPDialog)

CDlgSlider::CDlgSlider(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSlider::IDD, pParent)
	, m_iMin(0)
	, m_iMax(100)
	, m_iCur(0)
	, m_TipFunc(0)
{
}

CDlgSlider::~CDlgSlider()
{
}

void CDlgSlider::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_CTRL, m_Slider);
	DDX_Control(pDX, IDC_STATIC_SLIDER_CTRL_MIN, m_StaticMin);
	DDX_Control(pDX, IDC_STATIC_SLIDER_CTRL_MAX, m_StaticMax);
	DDX_Control(pDX, IDC_STATIC_SLIDER_CTRL_CUR, m_StaticPos);
	DDX_Control(pDX, IDC_STATIC_SLIDER_CTRL_STEP, m_wndStep);
	DDX_Control(pDX, IDC_STATIC_SLIDER_CTRL_TIP, m_wndTip);
}


BEGIN_MESSAGE_MAP(CDlgSlider, CBCGPDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDOK, &CDlgSlider::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgSlider::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgSlider message handlers

BOOL CDlgSlider::SetRange(int iMin, int iMax, int iCur, double step)
{
	m_iMin = iMin;
	m_iMax = iMax; 
	m_iCur = iCur;
	m_step = step;
	if (m_iMin <= m_iCur
		&& m_iCur <= m_iMax)
		return TRUE;
	return FALSE;
}

void CDlgSlider::OnBnClickedOk()
{
	DestroyWindow();
}

void CDlgSlider::OnBnClickedCancel()
{
	DestroyWindow();
}

BOOL CDlgSlider::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();

	SetWindowText(m_strTitle);
	CString strStep;
	strStep.Format("²½³¤:%.3f %s", (float)m_step, m_strUnit);
	m_wndStep.SetWindowText(strStep);

	m_Slider.SetRange(m_iMin, m_iMax);
	m_Slider.SetPos(m_iCur);
	updateText();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSlider::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos = m_Slider.GetPos();
	m_iCur = pos;

	switch(nSBCode)
	{
	case TB_LINEUP:
	case TB_LINEDOWN:
	case TB_THUMBPOSITION:
	case TB_PAGEUP:
	case TB_PAGEDOWN:
		if (m_pListener)
			m_pListener->OnCommitChange(m_iMin, m_iMax, m_iCur);
		break;
	case TB_ENDTRACK:
	case TB_THUMBTRACK:
		if (m_pListener)
			m_pListener->OnSliderPosChanged(m_iMin, m_iMax, m_iCur);
		break;
	}
	updateText();
}

void CDlgSlider::updateText()
{
	CString strMin, strMax, strCur;
	if (m_pListener) {
		BOOL bIsInteger;
		double fMin, fMax, fCur;
		m_pListener->OnGetValue(bIsInteger, fMin, fMax, fCur);
		if (bIsInteger) {
			strMin.Format("%d %s", (int)fMin, m_strUnit);
			strMax.Format("%d %s", (int)fMax, m_strUnit);
			strCur.Format("%d %s", (int)fCur, m_strUnit);
			if(m_TipFunc)
				m_TipFunc((int)fCur, m_strTip);
		}
		else {
			strMin.Format("%.2f %s", fMin, m_strUnit);
			strMax.Format("%.2f %s", fMax, m_strUnit);
			strCur.Format("%.2f %s", fCur, m_strUnit);
			if(m_TipFunc)
				m_TipFunc(fCur, m_strTip);
		}
	}
	else {
		strMin.Format("%d %s", m_iMin, m_strUnit);
		strMax.Format("%d %s", m_iMax, m_strUnit);
		strCur.Format("%d %s", m_iCur, m_strUnit);
		if(m_TipFunc)
			m_TipFunc(m_iCur, m_strTip);
	}
	m_StaticMin.SetWindowText(strMin);
	m_StaticMax.SetWindowText(strMax);
	m_StaticPos.SetWindowText(strCur);	
	m_wndTip.SetWindowText(m_strTip);
}