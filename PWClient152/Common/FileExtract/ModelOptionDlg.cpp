// ModelOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileExtract.h"
#include "ModelOptionDlg.h"
#include "FileAnalyse.h"

#include <AFI.h>
#include <EC_Model.h>
#include <A3DCombinedAction.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelOptionDlg dialog


CModelOptionDlg::CModelOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelOptionDlg)
	m_szModelName = _T("");
	m_bEquipGfx = FALSE;
	//}}AFX_DATA_INIT
}


void CModelOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelOptionDlg)
	DDX_Control(pDX, IDC_ACTION_LIST, m_Actions);
	DDX_Text(pDX, IDC_MODEL_NAME, m_szModelName);
	DDX_Check(pDX, IDC_CHK_FASHIONGFX, m_bEquipGfx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CModelOptionDlg)
	ON_BN_CLICKED(IDC_BTN_SELECTALL, OnBtnSelectall)
	ON_BN_CLICKED(IDC_BTN_NOSELECT, OnBtnNoselect)
	ON_BN_CLICKED(IDC_BTN_INVERTSEL, OnBtnInvertsel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelOptionDlg message handlers

void CModelOptionDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	FileAnalyse::MODEL_OPTION& opt = FileAnalyse::GetInstance().GetModelOption();
	opt.bEquipGfx = (m_bEquipGfx == TRUE);
	opt.actionExport.clear();

	for( int i=0;i<m_Actions.GetCount();i++ )
	{
		opt.actionExport.push_back(m_Actions.GetSel(i) ? true : false);
	}
	
	CDialog::OnOK();
}

void CModelOptionDlg::OnCancel()
{
	FileAnalyse::MODEL_OPTION& opt = FileAnalyse::GetInstance().GetModelOption();
	opt.bEquipGfx = (m_bEquipGfx == TRUE);
	opt.actionExport.clear();

	CDialog::OnCancel();
}

void CModelOptionDlg::OnBtnSelectall() 
{
	// TODO: Add your control notification handler code here
	m_Actions.SelItemRange(TRUE, 0, m_Actions.GetCount());
}

void CModelOptionDlg::OnBtnNoselect() 
{
	// TODO: Add your control notification handler code here
	m_Actions.SelItemRange(FALSE, 0, m_Actions.GetCount());
}

void CModelOptionDlg::OnBtnInvertsel() 
{
	// TODO: Add your control notification handler code here
	for( int i=0;i<m_Actions.GetCount();i++ )
	{
		m_Actions.SetSel(i, (m_Actions.GetSel(i) == 0));
	}
}

BOOL CModelOptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CECModel* pModel = new CECModel();
	if( !pModel ) return false;
	if( !pModel->Load(m_szModelPath) )
	{
		A3DRELEASE(pModel);
		AfxMessageBox(_T("无效的模型路径！"));
		return FALSE;
	}

	for( int i=0;i<pModel->GetComActCount();i++ )
	{
		m_Actions.AddString(AS2AC(pModel->GetComActByIndex(i)->GetName()));
	}

	m_Actions.SelItemRange(TRUE, 0, m_Actions.GetCount());

	AString strName;
	af_GetFileTitle(m_szModelPath, strName);
	m_szModelName = AS2AC(strName);
	UpdateData(FALSE);

	A3DRELEASE(pModel);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
