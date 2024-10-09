// DelObjectDlg.cpp : implementation file
//

#include "Global.h"
#include "elementeditor.h"
#include "DelObjectDlg.h"
#include "ElementMap.h"
#include "sceneObjectManager.h"
#include "SceneObject.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDelObjectDlg dialog


CDelObjectDlg::CDelObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelObjectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelObjectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDelObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelObjectDlg)
	DDX_Control(pDX, IDC_LIST_OBJECT_LIST, m_listObj);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CDelObjectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelObjectDlg message handlers

BOOL CDelObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return true;
	
	if(pMap->GetSceneObjectMan()->m_listSelectedObject.GetCount()>0)
	{
		ALISTPOSITION pos =  pMap->GetSceneObjectMan()->m_listSelectedObject.GetTailPosition();
		while(pos)
		{
			CSceneObject *pObj = pMap->GetSceneObjectMan()->m_listSelectedObject.GetPrev(pos);
			m_listObj.AddString(pObj->GetObjectName());
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
