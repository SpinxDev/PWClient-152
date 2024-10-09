// TerrainModifyNoisePanel.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "TerrainModifyNoisePanel.h"
#include "TerrainStretchBrush.h"
#include "TerrainNoiseOperation.h"
#include "OperationManager.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyNoisePanel dialog


CTerrainModifyNoisePanel::CTerrainModifyNoisePanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainModifyNoisePanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainModifyNoisePanel)
	//}}AFX_DATA_INIT
}


void CTerrainModifyNoisePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainModifyNoisePanel)
	DDX_Control(pDX, IDC_CHECK_LOCK_MAP_EDGE, m_checkLockEdge);
	DDX_Control(pDX, IDC_EDIT_NOISEDELTA, m_editDelta);
	DDX_Control(pDX, IDC_SPIN_NOISEDELTA, m_spinDelta);
	DDX_Control(pDX, IDC_SPIN_NOISEPERSISTENCE, m_spinPersistence);
	DDX_Control(pDX, IDC_EDIT_NOISEPERSISTENCE, m_editPersistence);
	DDX_Control(pDX, IDC_SPIN_NOISERADIUS, m_spinNoiseRadius);
	DDX_Control(pDX, IDC_EDIT_NOISERADIUS, m_editNoiseRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainModifyNoisePanel, CDialog)
	//{{AFX_MSG_MAP(CTerrainModifyNoisePanel)
	ON_EN_CHANGE(IDC_EDIT_NOISEPERSISTENCE, OnChangeEditNoisepersistence)
	ON_EN_CHANGE(IDC_EDIT_NOISERADIUS, OnChangeEditNoiseradius)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NOISEPERSISTENCE, OnDeltaposSpinNoisepersistence)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NOISERADIUS, OnDeltaposSpinNoiseradius)
	ON_CBN_SELCHANGE(IDC_COMBO_NOISE_TYPE, OnSelchangeComboNoiseType)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NOISEDELTA, OnDeltaposSpinNoisedelta)
	ON_EN_CHANGE(IDC_EDIT_NOISEDELTA, OnChangeEditNoisedelta)
	ON_BN_CLICKED(IDC_CHECK_LOCK_MAP_EDGE, OnCheckLockMapEdge)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyNoisePanel message handlers

BOOL CTerrainModifyNoisePanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//AUX_FlatEditBox(&m_editNoiseWaveLen);
	AUX_FlatEditBox(&m_editDelta);
	AUX_FlatEditBox(&m_editNoiseRadius);
	AUX_FlatEditBox(&m_editPersistence);
	

	
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	
	m_checkLockEdge.SetCheck(((CTerrainNoiseOperation*)pOperation)->GetLockEdgeState());
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainNoiseOperation*)pOperation)->GetBrush();
	
	int wave = pBrush->GetNoiseWaveLenght();
	int radius = pBrush->GetNoiseAmplitude();
	int oct = ((CTerrainNoiseOperation*)pOperation)->GetDelta();
	int persistence = pBrush->GetNoisePersistence();
	CString str;

	str.Format("%d", radius);
	m_editNoiseRadius.SetWindowText(str);
	str.Format("%d", persistence);
	m_editPersistence.SetWindowText(str);
	str.Format("%d",oct);
	m_editDelta.SetWindowText(str);

	//m_comboNoiseType.InsertString(NOISE_TYPE_ADD,"上方向迭加");
	//m_comboNoiseType.InsertString(NOISE_TYPE_SUB,"下方向迭加");
	//m_comboNoiseType.InsertString(NOISE_TYPE_ORIGIN,"原地起伏");
	//m_comboNoiseType.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainModifyNoisePanel::OnChangeEditNoisepersistence() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	int num = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainNoiseOperation*)pOperation)->GetBrush();
	num = pBrush->GetNoisePersistence();

	CString str;
	m_editPersistence.GetWindowText(str);
	num = (int)atof(str);
	if(num>100) 
	{
		num = 100;
		str.Format("%d", num);
		m_editPersistence.SetWindowText(str);
	}else if(num<1)
	{
		num = 1;
		str.Format("%d", num);
		m_editPersistence.SetWindowText(str);
	}
	pBrush->SetNoisePersistence(num);

	((CTerrainNoiseOperation*)pOperation)->UpdateNoise();
}

void CTerrainModifyNoisePanel::OnChangeEditNoiseradius() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	int num = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainNoiseOperation*)pOperation)->GetBrush();
	num = pBrush->GetNoiseAmplitude();

	CString str;
	m_editNoiseRadius.GetWindowText(str);
	num = (int)atof(str);
	if(num>100) 
	{
		num = 100;
		str.Format("%d", num);
		m_editNoiseRadius.SetWindowText(str);
	}else if(num<1)
	{
		num = 1;
		str.Format("%d", num);
		m_editNoiseRadius.SetWindowText(str);
	}
	pBrush->SetNoiseAmplitude(num);

	((CTerrainNoiseOperation*)pOperation)->UpdateNoise();
}
void CTerrainModifyNoisePanel::OnDeltaposSpinNoisepersistence(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	int num = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainNoiseOperation*)pOperation)->GetBrush();
	num = pBrush->GetNoisePersistence();
	
	num += -pNMUpDown->iDelta*1;
	if(num>100)
		num = 100;
	else if(num<1)
		num = 1;
		
	
	CString str;
	str.Format("%d", num);
	m_editPersistence.SetWindowText(str);

	pBrush->SetNoisePersistence(num);
	
	((CTerrainNoiseOperation*)pOperation)->UpdateNoise();
	*pResult = 0;
}

void CTerrainModifyNoisePanel::OnDeltaposSpinNoiseradius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	int num = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainNoiseOperation*)pOperation)->GetBrush();
	num = pBrush->GetNoiseAmplitude();
	
	num += -pNMUpDown->iDelta*1;
	if(num>100)
		num = 100;
	else if(num<1)
		num = 1;
		
	
	CString str;
	str.Format("%d", num);
	m_editNoiseRadius.SetWindowText(str);

	pBrush->SetNoiseAmplitude(num);

	((CTerrainNoiseOperation*)pOperation)->UpdateNoise();
	*pResult = 0;
}


void CTerrainModifyNoisePanel::OnSelchangeComboNoiseType() 
{
	// TODO: Add your control notification handler code here
	/*
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	if(pOperation)
	{
		int cpos = m_comboNoiseType.GetCurSel();
		if(cpos<NOISE_TYPE_NUM)
			((CTerrainNoiseOperation*)pOperation)->SetCurrentNoiseType((NOISE_TYPE)cpos);
	}*/
}
/*
void CTerrainModifyNoisePanel::OnChangeEditNoisedelta() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	CString str;
	int num = 0;
	m_editDelta.GetWindowText(str);
	num = (int)atof(str);
	if(num>100)
		num = 100;
	else if(num<1)	
		num = 1;
		
	str.Format("%d", num);
	m_editDelta.SetWindowText(str);

	((CTerrainNoiseOperation*)pOperation)->SetDelta(num);
	// TODO: Add your control notification handler code here
	
}
*/

void CTerrainModifyNoisePanel::OnDeltaposSpinNoisedelta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	int num = ((CTerrainNoiseOperation*)pOperation)->GetDelta();

	num += -pNMUpDown->iDelta*1;
	if(num>100)
		num = 100;
	else if(num<1)
		num = 1;
		
	
	CString str;
	str.Format("%d", num);
	m_editDelta.SetWindowText(str);

	((CTerrainNoiseOperation*)pOperation)->SetDelta(num);
	*pResult = 0;
}

void CTerrainModifyNoisePanel::OnChangeEditNoisedelta() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_NOISE);
	CString str;
	int num = 0;
	m_editDelta.GetWindowText(str);
	num = (int)atof(str);
	if(num>100)
	{
		num = 100;
		str.Format("%d", num);
		m_editDelta.SetWindowText(str);
	}else if(num<1)	
	{
		num = 1;
		str.Format("%d", num);
		m_editDelta.SetWindowText(str);
	}
	((CTerrainNoiseOperation*)pOperation)->SetDelta(num);
}

void CTerrainModifyNoisePanel::OnCheckLockMapEdge() 
{
	// TODO: Add your control notification handler code here
	CTerrainNoiseOperation *pOperation = (CTerrainNoiseOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	if(pOperation)
	{
		CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_LOCK_MAP_EDGE);
		if(pOperation->GetLockEdgeState())
			pOperation->LockEdge(false);
		else pOperation->LockEdge(true);
	}
}
