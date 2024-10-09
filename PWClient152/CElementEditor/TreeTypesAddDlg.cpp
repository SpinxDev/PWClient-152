// TreeTypesAddDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "TreeTypesAddDlg.h"
#include "CommonFileDlg.h"
#include "OperationManager.h"
#include "EL_Forest.h"
#include "EL_Tree.h"
#include "SceneObjectManager.h"
#include "Render.h"
#include "TerrainPlants.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTreeTypesAddDlg dialog


CTreeTypesAddDlg::CTreeTypesAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeTypesAddDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeTypesAddDlg)
	m_strSpeedTreeName = _T("");
	m_strDDSName = _T("");
	m_strTreeName = _T("");
	//}}AFX_DATA_INIT
	m_bIsChange = false;
}


void CTreeTypesAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeTypesAddDlg)
	DDX_Text(pDX, IDC_EDIT_SPEED_TREE_NAME, m_strSpeedTreeName);
	DDX_Text(pDX, IDC_EDIT_DDS_NAME, m_strDDSName);
	DDX_Text(pDX, IDC_EDIT_TREE_NAME, m_strTreeName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreeTypesAddDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeTypesAddDlg)
	ON_BN_CLICKED(IDC_BUTTON_SPEEDTREE, OnButtonSpeedtree)
	ON_BN_CLICKED(IDC_BUTTON_DDS, OnButtonDds)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeTypesAddDlg message handlers

void CTreeTypesAddDlg::OnButtonSpeedtree() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "Trees","*.spt");
	if (FileDlg.DoModal() != IDOK)
		return; 
	AString strName = FileDlg.GetFullFileName();
	m_strSpeedTreeName = strName;
	m_strTreeName = strName;
	UpdateData(false);
}

void CTreeTypesAddDlg::OnButtonDds() 
{
	// TODO: Add your control notification handler code here
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "Trees","*.dds,*.bmp,*.jpg,*.png,*.tga");
	if (FileDlg.DoModal() != IDOK)
		return; 
	AString strName = FileDlg.GetFullFileName();
	m_strDDSName = strName;
	UpdateData(false);
}

void CTreeTypesAddDlg::OnOK()
{
	UpdateData(false);
	if(m_strSpeedTreeName.IsEmpty() || m_strDDSName.IsEmpty() || m_strTreeName.IsEmpty())
		return;
	if(m_bIsChange) 
	{
		CDialog::OnOK();
		return;
	}
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
 	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CTerrainPlants *pPlants  = pManager->GetPlants();
		CELForest *pForest = pManager->GetElForest();
		DWORD numTypes = pForest->GetNumTreeType();
		DWORD newID = 0;
		for(int i = 0; i<numTypes; i++)
		{
			CELTree *pTree = pForest->GetTreeType(i);
			DWORD id = pTree->GetTypeID();
			if(newID<id) newID = id;
		}
		if(pPlants->GetPlant(AString(m_strTreeName)))
		{
			MessageBox("加入类型失败，树的名字已经存在");
			return;
		}
		if(!pForest->AddTreeType(++newID,m_strSpeedTreeName,m_strDDSName))
		{
			MessageBox("加入类型失败，可能是不正确的文件类型或是文件不存在");
			return;
		}
		pManager->UpdateLighting(g_bIsNight);
		pPlants->AddPlant(AString(m_strTreeName),newID,PLANT_TREE);
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePlantsPanel();
	}
	CDialog::OnOK();
}


BOOL CTreeTypesAddDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWnd *pwnd = GetDlgItem(IDC_EDIT_TREE_NAME);
	if(pwnd && m_bIsChange)
		pwnd->EnableWindow(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
