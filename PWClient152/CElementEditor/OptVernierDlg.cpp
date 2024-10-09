// OptVernierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "OptVernierDlg.h"
#include "SceneVernier.h"
#include "ElementMap.h"
#include "SceneObjectManager.h"
#include "OperationManager.h"
#include "MainFrm.h"


//#define new A_DEBUG_NEW


/////////////////////////////////////////////////////////////////////////////
// COptVernierDlg dialog


COptVernierDlg::COptVernierDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptVernierDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptVernierDlg)
	m_editPosX = 0.0f;
	m_editPosY = 0.0f;
	m_editPosZ = 0.0f;
	m_editVernierWidth = 0.0f;
	//}}AFX_DATA_INIT
}


void COptVernierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptVernierDlg)
	DDX_Text(pDX, IDC_EDIT_POSX, m_editPosX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_editPosY);
	DDX_Text(pDX, IDC_EDIT_POSZ, m_editPosZ);
	DDX_Text(pDX, IDC_EDIT_VERNIER_WIDTH, m_editVernierWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptVernierDlg, CDialog)
	//{{AFX_MSG_MAP(COptVernierDlg)
	ON_EN_CHANGE(IDC_EDIT_POSX, OnChangeEditPosx)
	ON_EN_CHANGE(IDC_EDIT_POSY, OnChangeEditPosy)
	ON_EN_CHANGE(IDC_EDIT_POSZ, OnChangeEditPosz)
	ON_EN_CHANGE(IDC_EDIT_VERNIER_WIDTH, OnChangeEditVernierWidth)
	ON_BN_CLICKED(IDC_RADIO_XZ, OnRadioXz)
	ON_BN_CLICKED(IDC_RADIO_XY, OnRadioXy)
	ON_BN_CLICKED(IDC_RADIO_ZY, OnRadioZy)
	ON_BN_CLICKED(IDC_CHECK_GETPOS, OnCheckGetpos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptVernierDlg message handlers

void COptVernierDlg::OnChangeEditPosx() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		A3DVECTOR3 v = pVernier->GetPos();
		UpdateData(true);
		v.x = m_editPosX;
		pVernier->SetPos(v);
	}
}

void COptVernierDlg::OnChangeEditPosy() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		A3DVECTOR3 v = pVernier->GetPos();
		UpdateData(true);
		v.y = m_editPosY;
		pVernier->SetPos(v);
	}
}

void COptVernierDlg::OnChangeEditPosz() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		A3DVECTOR3 v = pVernier->GetPos();
		UpdateData(true);
		v.z= m_editPosZ;
		pVernier->SetPos(v);
	}
}

void COptVernierDlg::OnChangeEditVernierWidth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		UpdateData(true);
		pVernier->m_fAreaSize = m_editVernierWidth;
	}
}

BOOL COptVernierDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptVernierDlg::ShowData()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		if(pVernier)
		{
		
			A3DVECTOR3 v = pVernier->GetPos();
			m_editPosX = v.x;
			m_editPosY = v.y;
			m_editPosZ = v.z;
			m_editVernierWidth = pVernier->m_fAreaSize;
			UpdateData(false);

		
			CButton *pButton = NULL;
			pButton = (CButton *)GetDlgItem(IDC_RADIO_XZ);
			pButton->SetCheck(0);
			pButton = (CButton *)GetDlgItem(IDC_RADIO_XY);
			pButton->SetCheck(0);
			pButton = (CButton *)GetDlgItem(IDC_RADIO_ZY);
			pButton->SetCheck(0);
			
			switch(pVernier->m_nShowPlane)
			{
			case 0: pButton = (CButton *)GetDlgItem(IDC_RADIO_XZ);
				pButton->SetCheck(1); break;
			case 1: (CButton *)GetDlgItem(IDC_RADIO_XY); 
				pButton->SetCheck(1); break;
			case 2: (CButton *)GetDlgItem(IDC_RADIO_ZY);
				pButton->SetCheck(1); break;
			}
		}
	}
}

void COptVernierDlg::OnRadioXz() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		pVernier->m_nShowPlane = 0;
	}
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_XY);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_ZY);
	pButton->SetCheck(0);
}

void COptVernierDlg::OnRadioXy() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		pVernier->m_nShowPlane = 1;
	}
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_XZ);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_ZY);
	pButton->SetCheck(0);
}

void COptVernierDlg::OnRadioZy() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		CSceneVernier *pVernier = pSManager->GetVernier();
		pVernier->m_nShowPlane = 2;
	}
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_XZ);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_XY);
	pButton->SetCheck(0);
}

void COptVernierDlg::OnCheckGetpos() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame)
	{
		CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_GETPOS);
		if(!pButton->GetCheck())
		{
			CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
			if(pViewFrame) pViewFrame->SetCurrentOperation(NULL);
		}else
		{
			pMainFrame->SetOtherOperation(true);
			COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_SET_VERNIER);
			CViewFrame *pViewFrame = pMainFrame->GetViewFrame();
			if(pViewFrame) pViewFrame->SetCurrentOperation(pOperation);
		}
	}
}

void COptVernierDlg::RaiseButton()
{
	
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame)
	{
		CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_GETPOS);
		pButton->SetCheck(0);
	}
	
}
