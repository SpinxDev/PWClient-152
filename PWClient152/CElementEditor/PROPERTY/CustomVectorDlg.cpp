// CustomVectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CustomVectorDlg.h"
#include <A3DVector.h>

/////////////////////////////////////////////////////////////////////////////
// CCustomVectorDlg dialog

CCustomVectorDlg::CCustomVectorDlg(const A3DVECTOR3 & vec,CWnd* pParent /*=NULL*/)
	: CDialog(CCustomVectorDlg::IDD, pParent)
{
	m_x = vec.x;
	m_y = vec.y;
	m_z = vec.z;

	//{{AFX_DATA_INIT(CCustomVectorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCustomVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomVectorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT_VECOTR_X, m_x);
	DDX_Text(pDX, IDC_EDIT_VECOTR_Y, m_y);
	DDX_Text(pDX, IDC_EDIT_VECOTR_Z, m_z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomVectorDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomVectorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomVectorDlg message handlers
