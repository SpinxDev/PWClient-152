// DlgSetValue.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSetValue.h"


// CDlgSetValue dialog

IMPLEMENT_DYNAMIC(CDlgSetValue, CBCGPDialog)

CDlgSetValue::CDlgSetValue(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSetValue::IDD, pParent)
	, m_fMin(0)
	, m_fMax(0)
	, m_fVal(0)
	, m_fParam(0)
{

}

CDlgSetValue::~CDlgSetValue()
{
}

void CDlgSetValue::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MIN_VALUE, m_fMin);
	DDX_Text(pDX, IDC_MAX_VALUE, m_fMax);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_fVal);
	DDX_Text(pDX, IDC_EDIT_PARAM, m_fParam);
}


BEGIN_MESSAGE_MAP(CDlgSetValue, CBCGPDialog)
//	ON_EN_CHANGE(IDC_EDIT_VALUE, &CDlgSetValue::OnEnChangeEditValue)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE, &CDlgSetValue::OnEnKillfocusEditValue)
END_MESSAGE_MAP()


// CDlgSetValue message handlers

BOOL CDlgSetValue::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetValue::OnEnKillfocusEditValue()
{
	UpdateData();
	if(m_fVal < m_fMin)
		m_fVal = m_fMin;
	if(m_fVal > m_fMax)
		m_fVal = m_fMax;
	UpdateData(FALSE);
}
