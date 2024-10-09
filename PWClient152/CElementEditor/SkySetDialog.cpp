// SkySetDialog.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "SkySetDialog.h"
#include "MainFrm.h"
#include "OperationManager.h"
#include "SceneObjectManager.h"
#include "CommonFileDlg.h"
#include "Render.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CSkySetDialog dialog


CSkySetDialog::CSkySetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSkySetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkySetDialog)
	m_strTex1 = _T("textures\\sky\\roof.bmp");
	m_strTex2 = _T("textures\\sky\\01.bmp");
	m_strTex3 = _T("textures\\sky\\02.bmp");
	m_strTex4 = _T("");
	m_strTex5 = _T("");
	m_strTex6 = _T("");
	//}}AFX_DATA_INIT
	m_bSphereSky = true;
}


void CSkySetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkySetDialog)
	DDX_Text(pDX, IDC_EDIT_SKY_TEX1, m_strTex1);
	DDX_Text(pDX, IDC_EDIT_SKY_TEX2, m_strTex2);
	DDX_Text(pDX, IDC_EDIT_SKY_TEX3, m_strTex3);
	DDX_Text(pDX, IDC_EDIT_SKY_TEX4, m_strTex4);
	DDX_Text(pDX, IDC_EDIT_SKY_TEX5, m_strTex5);
	DDX_Text(pDX, IDC_EDIT_SKY_TEX6, m_strTex6);
	//}}AFX_DATA_MAP
}

void CSkySetDialog::GetSkyTexture(AString &t1,AString &t2,AString &t3,AString &t4,AString &t5,AString &t6)
{
	UpdateData();
	t1 = AString(m_strTex1);
	t2 = AString(m_strTex2);
	t3 = AString(m_strTex3);

	t4 = AString(m_strTex4);
	t5 = AString(m_strTex5);
	t6 = AString(m_strTex6);
}

BEGIN_MESSAGE_MAP(CSkySetDialog, CDialog)
	//{{AFX_MSG_MAP(CSkySetDialog)
	ON_BN_CLICKED(IDC_BUTTON_SKY1, OnButtonSky1)
	ON_BN_CLICKED(IDC_BUTTON_SKY2, OnButtonSky2)
	ON_BN_CLICKED(IDC_BUTTON_SKY3, OnButtonSky3)
	ON_BN_CLICKED(IDC_CREATE_SKYBOX, OnCreateSkybox)
	ON_BN_CLICKED(IDC_RADIO_SKY_SPHERE, OnRadioSkySphere)
	ON_BN_CLICKED(IDC_RADIO_SKY_BOX, OnRadioSkyBox)
	ON_BN_CLICKED(IDC_BUTTON_SKY4, OnButtonSky4)
	ON_BN_CLICKED(IDC_BUTTON_SKY5, OnButtonSky5)
	ON_BN_CLICKED(IDC_BUTTON_SKY6, OnButtonSky6)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkySetDialog message handlers

void CSkySetDialog::OnButtonSky1() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "textures\\sky");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strTex1 = FileDlg.GetFullFileName();
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SKY_TEX1, m_strTex1);
}

void CSkySetDialog::OnButtonSky2() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "textures\\sky");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strTex2 = FileDlg.GetFullFileName();
	
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SKY_TEX2, m_strTex2);
}

void CSkySetDialog::OnButtonSky3() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "textures\\sky");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strTex3 = FileDlg.GetFullFileName();
	
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SKY_TEX3, m_strTex3);
}

void CSkySetDialog::SetInitTexture(AString &t1,AString &t2,AString &t3)
{
	m_strTex1 = t1;
	m_strTex2 = t2;
	m_strTex3 = t3;
}

void CSkySetDialog::OnCreateSkybox() 
{
}

void CSkySetDialog::OnRadioSkySphere() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_SKY_BOX);
	pButton->SetCheck(false);
	EnableBox(false);
	m_bSphereSky = true;
}

void CSkySetDialog::OnRadioSkyBox() 
{
	// TODO: Add your control notification handler code here
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_SKY_SPHERE);
	pButton->SetCheck(false);
	EnableBox(true);
	m_bSphereSky = false;
}

void CSkySetDialog::OnButtonSky4() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "textures\\sky");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strTex4 = FileDlg.GetFullFileName();
	
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SKY_TEX4, m_strTex4);
}

void CSkySetDialog::OnButtonSky5() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "textures\\sky");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strTex5 = FileDlg.GetFullFileName();
	
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SKY_TEX5, m_strTex5);
}

void CSkySetDialog::OnButtonSky6() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "textures\\sky");
	if (FileDlg.DoModal() != IDOK)
		return;

	m_strTex6 = FileDlg.GetFullFileName();
	
	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_SKY_TEX6, m_strTex6);
}

void CSkySetDialog::EnableBox(bool b)
{
	CWnd *pWnd = NULL;
	pWnd = GetDlgItem(IDC_STATIC_SKY_TEX4);
	pWnd->EnableWindow(b);
	pWnd = GetDlgItem(IDC_STATIC_SKY_TEX5);
	pWnd->EnableWindow(b);
	pWnd = GetDlgItem(IDC_STATIC_SKY_TEX6);
	pWnd->EnableWindow(b);

	pWnd = GetDlgItem(IDC_EDIT_SKY_TEX4);
	pWnd->EnableWindow(b);
	pWnd = GetDlgItem(IDC_EDIT_SKY_TEX5);
	pWnd->EnableWindow(b);
	pWnd = GetDlgItem(IDC_EDIT_SKY_TEX6);
	pWnd->EnableWindow(b);
	
	pWnd = GetDlgItem(IDC_BUTTON_SKY4);
	pWnd->EnableWindow(b);
	pWnd = GetDlgItem(IDC_BUTTON_SKY5);
	pWnd->EnableWindow(b);
	pWnd = GetDlgItem(IDC_BUTTON_SKY6);
	pWnd->EnableWindow(b);
}

BOOL CSkySetDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	EnableBox(false);
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_SKY_SPHERE);
	pButton->SetCheck(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSkySetDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	Update();
}

void CSkySetDialog::Update()
{

}
