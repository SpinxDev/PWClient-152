// DlgCopySel.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "DlgCopySel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const DWORD dwControlID[] = 
{
	IDC_CHECK_COPY0,
	IDC_CHECK_COPY1,
	IDC_CHECK_COPY2,
	IDC_CHECK_COPY3,
	IDC_CHECK_COPY4,
	IDC_CHECK_COPY5,
	IDC_CHECK_COPY6,
	IDC_CHECK_COPY7,
	IDC_CHECK_COPY8,
	IDC_CHECK_COPY9,
	IDC_CHECK_COPY10,
	IDC_CHECK_COPY11,
	IDC_CHECK_COPY12,
	IDC_CHECK_COPY13,
	IDC_CHECK_COPY14,
	IDC_CHECK_COPY15,
	IDC_CHECK_COPY16,
	IDC_CHECK_COPY17,
	IDC_CHECK_COPY18,
	IDC_CHECK_COPY19,
	IDC_CHECK_COPY20,
	IDC_CHECK_COPY21,
	IDC_CHECK_COPY22,
	IDC_CHECK_COPY23,
	IDC_CHECK_COPY24,
	IDC_CHECK_COPY25,
	IDC_CHECK_COPY26,
	IDC_CHECK_COPY27,
	IDC_CHECK_COPY28,
	IDC_CHECK_COPY29,
	IDC_CHECK_COPY30,
	IDC_CHECK_COPY31,
};

/////////////////////////////////////////////////////////////////////////////
// CDlgCopySel dialog


CDlgCopySel::CDlgCopySel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCopySel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCopySel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgCopySel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCopySel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCopySel, CDialog)
	//{{AFX_MSG_MAP(CDlgCopySel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCopySel message handlers



BOOL CDlgCopySel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	

	for( int i = 0; i < 32; ++i)
	{
		CButton *pButton = (CButton*)GetDlgItem(dwControlID[i]);
		if(g_dwExpCopy & (1<<i)) pButton->SetCheck(1);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCopySel::OnOK()
{
	g_dwExpCopy = 0;
	for( int i = 0; i < 32; ++i)
	{
		CButton *pButton = (CButton*)GetDlgItem(dwControlID[i]);
		if(pButton->GetCheck())
			g_dwExpCopy |= 1 << i;
	}
	
	CDialog::OnOK();
}


