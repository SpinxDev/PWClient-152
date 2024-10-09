// BezierSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "BezierSelDlg.h"
#include "MainFrm.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "EditerBezier.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CBezierSelDlg dialog


CBezierSelDlg::CBezierSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBezierSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBezierSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nBezierID = -1;
	m_nObjectType = CSceneObject::SO_TYPE_BEZIER;
}


void CBezierSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBezierSelDlg)
	DDX_Control(pDX, IDC_LIST_BEZIER, m_listBezier);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBezierSelDlg, CDialog)
	//{{AFX_MSG_MAP(CBezierSelDlg)
	ON_LBN_SELCHANGE(IDC_LIST_BEZIER, OnSelchangeListBezier)
	ON_BN_CLICKED(IDC_BUTTON_NULL, OnButtonNull)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBezierSelDlg message handlers

void CBezierSelDlg::OnSelchangeListBezier() 
{
	// TODO: Add your control notification handler code here
}

BOOL CBezierSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FreshList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBezierSelDlg::FreshList()
{
	int nCur = -1;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		ALISTPOSITION pos = pSManager->m_listSceneObject.GetTailPosition();
		while(pos)
		{
			PSCENEOBJECT ptemp = pSManager->m_listSceneObject.GetPrev(pos);
			if(ptemp->GetObjectType()==m_nObjectType)
			{
				int index = m_listBezier.AddString(ptemp->GetObjectName());
				if(ptemp->GetObjectID() == m_nBezierID)
					nCur = index;
			}
		}
	}
	
	if(nCur!=-1) m_listBezier.SetCurSel(nCur);
}

void CBezierSelDlg::OnButtonNull() 
{
	// TODO: Add your control notification handler code here
	m_listBezier.SetCurSel(-1);
	m_strBezierName = "";
	m_nBezierID = -1;

}

void CBezierSelDlg::OnOK()
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		int cur = m_listBezier.GetCurSel();
		if(cur!=-1)
		{
			char txt[128];
			m_listBezier.GetText(cur,txt);
			AString strBezierName(txt); 
			CSceneObject *pObj = pSManager->FindSceneObject(strBezierName);
			if(pObj) 
			{
				m_strBezierName = strBezierName;
				m_nBezierID = pObj->GetObjectID();
			}
		}
	}

	CDialog::OnOK();
}
