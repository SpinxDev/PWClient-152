// TerrainModifySmoothPanel.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "TerrainModifySmoothPanel.h"
#include "TerrainSmoothOperation.h"
#include "OperationManager.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifySmoothPanel dialog


CTerrainModifySmoothPanel::CTerrainModifySmoothPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainModifySmoothPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainModifySmoothPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTerrainModifySmoothPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainModifySmoothPanel)
	DDX_Control(pDX, IDC_CHECK_LOCK_MAP_EDGE, m_checkLockEdge);
	DDX_Control(pDX, IDC_SPIN_BRUSHSMOOTH, m_spinBrushSmooth);
	DDX_Control(pDX, IDC_SLIDER_BRUSHSMOOTH, m_sliderBrushSmooth);
	DDX_Control(pDX, IDC_EDIT_BRUSHSMOOTH, m_editBrushSmooth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainModifySmoothPanel, CDialog)
	//{{AFX_MSG_MAP(CTerrainModifySmoothPanel)
	ON_EN_CHANGE(IDC_EDIT_BRUSHSMOOTH, OnChangeEditBrushsmooth)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRUSHSMOOTH, OnReleasedcaptureSliderBrushsmooth)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRUSHSMOOTH, OnDeltaposSpinBrushsmooth)
	ON_BN_CLICKED(IDC_CHECK_LOCK_MAP_EDGE, OnCheckLockMapEdge)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifySmoothPanel message handlers

BOOL CTerrainModifySmoothPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AUX_FlatEditBox(&m_editBrushSmooth);
	m_sliderBrushSmooth.SetRange(1,100);

	CTerrainSmoothOperation *pOperation = (CTerrainSmoothOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_SMOOTH);
	if(pOperation)
	{
		int Num = pOperation->GetSmoothFactory();
		m_sliderBrushSmooth.SetPos(1);
		CString str;
		str.Format("%d",Num);
		m_editBrushSmooth.SetWindowText(str);

		m_checkLockEdge.SetCheck((pOperation)->GetLockEdgeState());
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainModifySmoothPanel::OnChangeEditBrushsmooth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CTerrainSmoothOperation *pOperation = (CTerrainSmoothOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_SMOOTH);
	if(pOperation==NULL) return;
	
	int Num;
	CString str;
	m_editBrushSmooth.GetWindowText(str);
	Num = (int)atof(str);
	if(Num>100) 
	{
		Num = 100;
		str.Format("%d", Num);
		m_editBrushSmooth.SetWindowText(str);
	}else if(Num<1)
	{
		Num = 1;
		str.Format("%d", Num);
		m_editBrushSmooth.SetWindowText(str);
	}

	m_sliderBrushSmooth.SetPos(Num);
	pOperation->SetSmoothFactory(Num);
	
}

void CTerrainModifySmoothPanel::OnReleasedcaptureSliderBrushsmooth(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	CTerrainSmoothOperation *pOperation = (CTerrainSmoothOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_SMOOTH);
	if(pOperation)
	{
		
		int Num;
		CString str;
		Num = m_sliderBrushSmooth.GetPos();
		str.Format("%d", Num);
		m_editBrushSmooth.SetWindowText(str);
		pOperation->SetSmoothFactory(Num);
	}
	*pResult = 0;
}

void CTerrainModifySmoothPanel::OnDeltaposSpinBrushsmooth(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	CTerrainSmoothOperation *pOperation = (CTerrainSmoothOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_SMOOTH);
	if(pOperation==NULL) return;
	
	int Num = pOperation->GetSmoothFactory();
	CString str;
	Num += -pNMUpDown->iDelta*1;
	if(Num>100) 
	{
		Num = 100;
	}else if(Num<1)
	{
		Num = 1;
	}
	str.Format("%d", Num);
	m_editBrushSmooth.SetWindowText(str);
	m_sliderBrushSmooth.SetPos(Num);
	pOperation->SetSmoothFactory(Num);
	*pResult = 0;
}

void CTerrainModifySmoothPanel::OnCheckLockMapEdge() 
{
	// TODO: Add your control notification handler code here
	CTerrainSmoothOperation *pOperation = (CTerrainSmoothOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	if(pOperation)
	{
		CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_LOCK_MAP_EDGE);
		if(pOperation->GetLockEdgeState())
			pOperation->LockEdge(false);
		else pOperation->LockEdge(true);
	}
}
