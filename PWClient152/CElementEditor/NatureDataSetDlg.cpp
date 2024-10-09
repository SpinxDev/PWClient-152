// NatureDataSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "NatureDataSetDlg.h"

#include "A3D.h"

//#define new A_DEBUG_NEW
/////////////////////////////////////////////////////////////////////////////
// CNatureDataSetDlg dialog


CNatureDataSetDlg::CNatureDataSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNatureDataSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNatureDataSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bRain = true;
	
}


void CNatureDataSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNatureDataSetDlg)
	DDX_Control(pDX, IDC_SELCOLOR, m_SelColor);
	//}}AFX_DATA_MAP
}

void CNatureDataSetDlg::Update(bool bUpdate)
{
	CWnd *pwnd = NULL;
	CString temp;
	int speed;
	float size;
	if(bUpdate)
	{//
		pwnd = GetDlgItem(IDC_EDIT_SPEED);
		pwnd->GetWindowText(temp);
		speed = (int)atof(temp);
		pwnd = GetDlgItem(IDC_EDIT_SIZE);
		pwnd->GetWindowText(temp);
		size = (float)atof(temp);
		
		if(m_bRain)
		{
			m_NatureData[TYPE_RAIN].nSpeed = speed;
			m_NatureData[TYPE_RAIN].fSize  = size;
			DWORD clr = m_SelColor.GetColor();
			int r = GetRValue(clr);
			int g = GetGValue(clr);
			int b = GetBValue(clr);
			m_NatureData[TYPE_RAIN].dwColor = A3DCOLORRGBA(r,g,b,150);
		}else
		{
			m_NatureData[TYPE_SNOW].nSpeed = speed;
			m_NatureData[TYPE_SNOW].fSize  = size;
			DWORD clr = m_SelColor.GetColor();
			int r = GetRValue(clr);
			int g = GetGValue(clr);
			int b = GetBValue(clr);
			m_NatureData[TYPE_SNOW].dwColor = A3DCOLORRGBA(r,g,b,255);
		}
	}else
	{
		if(m_bRain)
		{
			speed = m_NatureData[TYPE_RAIN].nSpeed;
			size = m_NatureData[TYPE_RAIN].fSize;
			
			int r = A3DCOLOR_GETRED(m_NatureData[TYPE_RAIN].dwColor);
			int g = A3DCOLOR_GETGREEN(m_NatureData[TYPE_RAIN].dwColor);
			int b = A3DCOLOR_GETBLUE(m_NatureData[TYPE_RAIN].dwColor);
			m_SelColor.SetColor(RGB(r,g,b));
			m_SelColor.Redraw();
		}else
		{
			speed = m_NatureData[TYPE_SNOW].nSpeed;
			size = m_NatureData[TYPE_SNOW].fSize;
			int r = A3DCOLOR_GETRED(m_NatureData[TYPE_SNOW].dwColor);
			int g = A3DCOLOR_GETGREEN(m_NatureData[TYPE_SNOW].dwColor);
			int b = A3DCOLOR_GETBLUE(m_NatureData[TYPE_SNOW].dwColor);
			m_SelColor.SetColor(RGB(r,g,b));
			m_SelColor.Redraw();
		}
		
		pwnd = GetDlgItem(IDC_EDIT_SPEED);
		temp.Format("%d",speed);
		pwnd->SetWindowText(temp);
		
		pwnd = GetDlgItem(IDC_EDIT_SIZE);
		temp.Format("%4.2f",size);
		pwnd->SetWindowText(temp);
		
	}
}


BEGIN_MESSAGE_MAP(CNatureDataSetDlg, CDialog)
	//{{AFX_MSG_MAP(CNatureDataSetDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_EN_CHANGE(IDC_EDIT_SPEED, OnChangeEditSpeed)
	ON_EN_CHANGE(IDC_EDIT_SIZE, OnChangeEditSize)
	ON_EN_KILLFOCUS(IDC_EDIT_SIZE, OnKillfocusEditSize)
	ON_EN_KILLFOCUS(IDC_EDIT_SPEED, OnKillfocusEditSpeed)
	ON_BN_CLICKED(ID_UPDATEDATA, OnUpdatedata)

	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELCOLCHANGED, OnColorChange)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNatureDataSetDlg message handlers

BOOL CNatureDataSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CButton *pbutton = (CButton *)GetDlgItem(IDC_RADIO1);
	if(pbutton) pbutton->SetCheck(true);
	pbutton = (CButton *)GetDlgItem(IDC_RADIO2);
	if(pbutton) pbutton->SetCheck(false);
	m_SelColor.SetNotifyWindow(this);
	m_SelColor.Redraw();
	m_SelColor.EnableWindow(TRUE);
	// TODO: Add extra initialization here
	Update(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNatureDataSetDlg::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	Update(true);
	m_bRain = true;
	Update(false);
	CButton *pbutton = (CButton *)GetDlgItem(IDC_RADIO2);
	if(pbutton) pbutton->SetCheck(false);
}

void CNatureDataSetDlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	Update(true);
	m_bRain = false;
	Update(false);
	CButton *pbutton = (CButton *)GetDlgItem(IDC_RADIO1);
	if(pbutton) pbutton->SetCheck(false);
}

void CNatureDataSetDlg::OnChangeEditSpeed() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
}

void CNatureDataSetDlg::OnChangeEditSize() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CNatureDataSetDlg::OnKillfocusEditSize() 
{
	// TODO: Add your control notification handler code here
	Update(true);
}

void CNatureDataSetDlg::OnKillfocusEditSpeed() 
{
	// TODO: Add your control notification handler code here
	Update(true);
}

void CNatureDataSetDlg::OnUpdatedata() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	EndDialog(IDOK);
}

void CNatureDataSetDlg::OnOK()
{
	Update(true);
}

HRESULT CNatureDataSetDlg::OnColorChange(WPARAM wParam, LPARAM lParam)
{
	Update(true);

	return S_OK;
}


