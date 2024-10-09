// TerrainModifyLayMaskSmoothPanel.cpp : implementation file
//
#include "Global.h"
#include "MainFrm.h"
#include "elementeditor.h"
#include "Operation.h"
#include "OperationManager.h"
#include "TerrainModifyLayMaskSmoothPanel.h"
#include "TerrainStretchBrush.h"
#include "TerrainMaskPaintOperation.h"
#include "TerrainMaskSmoothOperation.h"
#include "TerrainLayer.h"

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyLayMaskSmoothPanel dialog

//#define new A_DEBUG_NEW

CTerrainModifyLayMaskSmoothPanel::CTerrainModifyLayMaskSmoothPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainModifyLayMaskSmoothPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainModifyLayMaskSmoothPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTerrainModifyLayMaskSmoothPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainModifyLayMaskSmoothPanel)
	DDX_Control(pDX, IDC_SPIN_LAYMASKSMOOTH, m_spinLayMaskSmooth);
	DDX_Control(pDX, IDC_SLIDER_LAYMASKSMOOTH, m_sliderLayMaskSmooth);
	DDX_Control(pDX, IDC_EDIT_LAYMASKSMOOTH, m_editLayMaskSmooth);
	DDX_Control(pDX, IDC_TERRAIN_MODIFY_LAYLISTSMOOTH, m_listLayMaskSmooth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainModifyLayMaskSmoothPanel, CDialog)
	//{{AFX_MSG_MAP(CTerrainModifyLayMaskSmoothPanel)
	ON_LBN_SELCHANGE(IDC_TERRAIN_MODIFY_LAYLISTSMOOTH, OnSelchangeTerrainModifyLaylistsmooth)
	ON_EN_CHANGE(IDC_EDIT_LAYMASKSMOOTH, OnChangeEditLaymasksmooth)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_LAYMASKSMOOTH, OnReleasedcaptureSliderLaymasksmooth)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LAYMASKSMOOTH, OnDeltaposSpinLaymasksmooth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyLayMaskSmoothPanel message handlers

void CTerrainModifyLayMaskSmoothPanel::UpdateLayList()
{
	//	Intialzie layer list
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain == NULL) return;
	
	int cur = m_listLayMaskSmooth.GetCurSel();
	if(cur>=pTerrain->GetLayerNum() || cur<0)
		cur = -1;
	
	m_listLayMaskSmooth.ResetContent();
	
	int i, iNumLayer = pTerrain->GetLayerNum();
	for (i=0; i < iNumLayer; i++)
	{
		CTerrainLayer* pLayer = pTerrain->GetLayer(i);
		int iItem = m_listLayMaskSmooth.InsertString(-1, pLayer->GetName());
		m_listLayMaskSmooth.SetItemData(iItem, (DWORD)pLayer);
	}

	
	//	Select current layer and update operation data 
	if(cur==-1)
	{
		m_listLayMaskSmooth.SetCurSel(iNumLayer - 1);
		CTerrainMaskSmoothOperation *pOperationExt = (CTerrainMaskSmoothOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
		if(pOperationExt) pOperationExt->SetCurrentLayer(iNumLayer - 1);
	}else
	{
		m_listLayMaskSmooth.SetCurSel(cur);
		CTerrainMaskSmoothOperation *pOperationExt = (CTerrainMaskSmoothOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
		if(pOperationExt) pOperationExt->SetCurrentLayer(cur);
	}
	
}

void CTerrainModifyLayMaskSmoothPanel::SetMaskLay(char* szLayName)
{
	int n = m_listLayMaskSmooth.GetCount();	
	char txt[128];
	for(int i = 0 ; i < n; i++)
	{
		m_listLayMaskSmooth.GetText(i,txt);
		if(strcmp(txt,szLayName)==0)
		{
			m_listLayMaskSmooth.SetCurSel(i);
			CTerrainMaskSmoothOperation *pOperationExt = (CTerrainMaskSmoothOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
			if(pOperationExt) pOperationExt->SetCurrentLayer(i);
			break;
		}
	}
	
}



BOOL CTerrainModifyLayMaskSmoothPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateLayList();

	AUX_FlatEditBox(&m_editLayMaskSmooth);
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
	int factor = ((CTerrainMaskSmoothOperation*)pOperation)->GetSmoothFactory();
	CString str;
	str.Format("%d", factor);
	m_editLayMaskSmooth.SetWindowText(str);

	m_sliderLayMaskSmooth.SetRange(0,100);
	m_sliderLayMaskSmooth.SetPos(factor);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainModifyLayMaskSmoothPanel::OnSelchangeTerrainModifyLaylistsmooth() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain == NULL) return;
	
	char txt[128];
	int i = m_listLayMaskSmooth.GetCurSel();
	if(i>=pTerrain->GetLayerNum() || i<0)
	{
		UpdateLayList();
		return;
	}
	m_listLayMaskSmooth.GetText(i,txt);
	
	int iNumLayer = pTerrain->GetLayerNum();
	
	bool bFind = false;
	for (i=0; i < iNumLayer; i++)
	{
		CTerrainLayer* pLayer = pTerrain->GetLayer(i);
		if(strcmp(pLayer->GetName(),txt)==0) 
		{
			bFind = true;
			break;
		}
	
	}

	if(!bFind) i = -1; 
	CTerrainMaskSmoothOperation *pOperation = (CTerrainMaskSmoothOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
	if(pOperation) pOperation->SetCurrentLayer(i);
}

void CTerrainModifyLayMaskSmoothPanel::OnChangeEditLaymasksmooth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	CTerrainMaskSmoothOperation *pOperation = (CTerrainMaskSmoothOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
	int factory = 0;
	CString str;
	m_editLayMaskSmooth.GetWindowText(str);
	factory = (int)atof(str);
	if(factory>100) 
	{
		factory = 100;
		str.Format("%d", factory);
		m_editLayMaskSmooth.SetWindowText(str);
	}else if(factory<0)
	{
		factory = 0;
		str.Format("%d", factory);
		m_editLayMaskSmooth.SetWindowText(str);
	}

	m_sliderLayMaskSmooth.SetPos(factory);
	pOperation->SetSmoothFactory(factory);
	
}

void CTerrainModifyLayMaskSmoothPanel::OnReleasedcaptureSliderLaymasksmooth(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTerrainMaskSmoothOperation *pOperation = (CTerrainMaskSmoothOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
	int factory = 0;
	factory = m_sliderLayMaskSmooth.GetPos();
	CString str;
	str.Format("%d",factory);
	m_editLayMaskSmooth.SetWindowText(str);
	pOperation->SetSmoothFactory(factory);
	*pResult = 0;
}

void CTerrainModifyLayMaskSmoothPanel::OnDeltaposSpinLaymasksmooth(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	CTerrainMaskSmoothOperation *pOperation = (CTerrainMaskSmoothOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_MASK_SMOOTH);
	int factory = 0;
	factory = pOperation->GetSmoothFactory();
	
	factory += -pNMUpDown->iDelta*1;
	if(factory>100)
		factory = 100;
	else if(factory<0)
		factory = 0;
		
	
	CString str;
	str.Format("%d", factory);
	m_editLayMaskSmooth.SetWindowText(str);

	m_sliderLayMaskSmooth.SetPos(factory);
	pOperation->SetSmoothFactory(factory);

	*pResult = 0;
}
