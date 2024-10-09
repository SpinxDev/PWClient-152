// TerrainModifyStretchPanel.cpp : implementation file
//

#include "stdafx.h"
#include "Global.h"
#include "elementeditor.h"
#include "TerrainModifyStretchPanel.h"
#include "OperationManager.h"
#include "MainFrm.h"
#include "TerrainBrushSelDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyStretchPanel dialog


CTerrainModifyStretchPanel::CTerrainModifyStretchPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainModifyStretchPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainModifyStretchPanel)
	m_strBrushName = _T("默认刷子");
	//}}AFX_DATA_INIT
	m_pData = NULL;
	m_nBrushWidth = 0;
	m_nBrushHeight = 0;
}


void CTerrainModifyStretchPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainModifyStretchPanel)
	DDX_Control(pDX, IDC_CHECK_LOCK_MAP_EDGE, m_checkLockEdge);
	DDX_Control(pDX, IDC_EDIT_STRETCH_NOISE_RADIUS, m_editNoiseRadius);
	DDX_Control(pDX, IDC_EDIT_STRETCH_NOISE_PERSISTENCE, m_editNoisePersistence);
	DDX_Control(pDX, IDC_SPIN_STRETCH_NOISE_RADIUS, m_spinNoiseRadius);
	DDX_Control(pDX, IDC_SPIN_STRETCH_NOISE_PERSISTENCE, m_spinNosizePersistence);
	DDX_Control(pDX, IDC_COMBO_STRETCH_TYPE, m_comboStrecthType);
	DDX_Control(pDX, IDC_EDIT_BRUSHHEIGHT, m_editBrushHeight);
	DDX_Control(pDX, IDC_EDIT_BRUSHHARD, m_editBrushHard);
	DDX_Control(pDX, IDC_SPIN_BRUSHHEIGHT, m_spinBrushHeight);
	DDX_Control(pDX, IDC_SPIN_BRUSHHARD, m_spinBrushHard);
	DDX_Control(pDX, IDC_SLIDER_BRUSHHEIGHT, m_sliderBrushHeight);
	DDX_Control(pDX, IDC_SLIDER_BRUSHHARD, m_sliderBrushHard);
	DDX_Text(pDX, IDC_EDIT_BURSH_NAME, m_strBrushName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainModifyStretchPanel, CDialog)
	//{{AFX_MSG_MAP(CTerrainModifyStretchPanel)
	ON_EN_CHANGE(IDC_EDIT_BRUSHHARD, OnChangeEditBrushhard)
	ON_EN_CHANGE(IDC_EDIT_BRUSHHEIGHT, OnChangeEditBrushheight)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRUSHHARD, OnReleasedcaptureSliderBrushhard)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRUSHHEIGHT, OnReleasedcaptureSliderBrushheight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRUSHHARD, OnDeltaposSpinBrushhard)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRUSHHEIGHT, OnDeltaposSpinBrushheight)
	ON_BN_CLICKED(IDC_RADIO_BRUSHFLAT, OnRadioBrushflat)
	ON_BN_CLICKED(IDC_RADIO_BRUSHSMOOTH, OnRadioBrushsmooth)
	ON_CBN_SELCHANGE(IDC_COMBO_STRETCH_TYPE, OnSelchangeComboStretchType)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STRETCH_NOISE_PERSISTENCE, OnDeltaposSpinStretchNoisePersistence)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STRETCH_NOISE_RADIUS, OnDeltaposSpinStretchNoiseRadius)
	ON_EN_CHANGE(IDC_EDIT_STRETCH_NOISE_PERSISTENCE, OnChangeEditStretchNoisePersistence)
	ON_EN_CHANGE(IDC_EDIT_STRETCH_NOISE_RADIUS, OnChangeEditStretchNoiseRadius)
	ON_BN_CLICKED(IDC_RADIO_HEIGHTSUB, OnRadioHeightsub)
	ON_BN_CLICKED(IDC_RADIO_HEIGHTADD, OnRadioHeightadd)
	ON_BN_CLICKED(IDC_CHECK_ENABLENOISE, OnCheckEnablenoise)
	ON_BN_CLICKED(IDC_CHECK_LOCK_MAP_EDGE, OnCheckLockMapEdge)
	ON_BN_CLICKED(IDC_BUTTON_SEL_BRUSH, OnButtonSelBrush)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyStretchPanel message handlers

BOOL CTerrainModifyStretchPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_nHeight = 255;
	// TODO: Add extra initialization here
	RECT rcWnd;
	GetDlgItem(IDC_TERRAIN_FILTER_DLG)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	if (!g_CurveFilterView.Create(this, rcWnd))
	{
		g_Log.Log("CTerrainModifyStretchPanel::OnInitDialog() , Failed to create curve filter view window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	g_CurveFilterView.InvalidateRect(NULL);
	m_sliderBrushHard.SetRange(0,100);

	AUX_FlatEditBox(&m_editBrushHard);
	AUX_FlatEditBox(&m_editBrushHeight);
	AUX_FlatEditBox(&m_editNoiseRadius);
	AUX_FlatEditBox(&m_editNoisePersistence);

	
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int hard,height;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	hard = pBrush->GetBrushHard();
	height = pBrush->GetBrushHeight();
	CString str;
	str.Format("%d", hard);
	m_editBrushHard.SetWindowText(str);
	str.Format("%4.2f", (float)height);
	m_editBrushHeight.SetWindowText(str);

	m_checkLockEdge.SetCheck(((CTerrainStretchOperation*)pOperation)->GetLockEdgeState());

	hard = pBrush->GetNoisePersistence();
	height = ((CTerrainStretchOperation*)pOperation)->GetFrequency();
	str.Format("%d", hard);
	m_editNoisePersistence.SetWindowText(str);
	str.Format("%d", height);
	m_editNoiseRadius.SetWindowText(str);

	//ShowSmoothCtrl(pBrush->GetBrushSmoothSort());
	CButton * temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHFLAT);
	if(temp)
	{
		temp->SetCheck(!pBrush->GetBrushSmoothSort());
	}
	temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHSMOOTH);
	if(temp)
	{
		temp->SetCheck(pBrush->GetBrushSmoothSort());
	}

	m_comboStrecthType.InsertString(-1,"鼠标控制高度");
	m_comboStrecthType.InsertString(-1,"时间控制高度");
	m_comboStrecthType.InsertString(-1,"固定高度");
	m_comboStrecthType.SetCurSel(0);
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTADD);
	if(pButton){ pButton->SetCheck(1); pButton->EnableWindow(false);}
	pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTSUB);
	if(pButton){ pButton->SetCheck(0); pButton->EnableWindow(false);}
	
	ShowHeightCtrl(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainModifyStretchPanel::OnChangeEditBrushhard() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int hard = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	hard = pBrush->GetBrushHard();

	CString str;
	m_editBrushHard.GetWindowText(str);
	hard = (int)atof(str);
	if(hard>100) 
	{
		hard = 100;
		str.Format("%d", hard);
		m_editBrushHard.SetWindowText(str);
	}else if(hard<0)
	{
		hard = 0;
		str.Format("%d", hard);
		m_editBrushHard.SetWindowText(str);
	}

	m_sliderBrushHard.SetPos(hard);
	pBrush->SetBrushHard(hard);
}

void CTerrainModifyStretchPanel::OnChangeEditBrushheight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	float height = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	height = pBrush->GetBrushHeight();

	CString str;
	m_editBrushHeight.GetWindowText(str);
	height = (float)atof(str);
	if(height>m_nHeight) 
	{
		height = m_nHeight;
		str.Format("%4.2d", height);
		m_editBrushHeight.SetWindowText(str);
	}else if(height<0)
	{
		height = 0;
		str.Format("%4.2f", height);
		m_editBrushHeight.SetWindowText(str);
	}

	m_sliderBrushHeight.SetPos((int)height);
	pBrush->SetBrushHeight(height);
}

void CTerrainModifyStretchPanel::OnReleasedcaptureSliderBrushhard(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int hard = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	hard = pBrush->GetBrushHard();
	
	hard = m_sliderBrushHard.GetPos();
	CString str;
	str.Format("%d", hard);
	m_editBrushHard.SetWindowText(str);
	pBrush->SetBrushHard(hard);
	*pResult = 0;
}

void CTerrainModifyStretchPanel::OnReleasedcaptureSliderBrushheight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	float height = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	height = pBrush->GetBrushHeight();

	height = m_sliderBrushHeight.GetPos();
	CString str;
	str.Format("%4.2f", height);
	m_editBrushHeight.SetWindowText(str);
	pBrush->SetBrushHeight(height);
	*pResult = 0;
}

void CTerrainModifyStretchPanel::OnDeltaposSpinBrushhard(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int hard = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	hard = pBrush->GetBrushHard();
	
	hard += -pNMUpDown->iDelta*1;
	if(hard>100)
		hard = 100;
	else if(hard<0)
		hard = 0;
		
	
	CString str;
	str.Format("%d", hard);
	m_editBrushHard.SetWindowText(str);

	m_sliderBrushHard.SetPos(hard);
	pBrush->SetBrushHard(hard);

	*pResult = 0;
}

void CTerrainModifyStretchPanel::OnDeltaposSpinBrushheight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	float height = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	height = pBrush->GetBrushHeight();

	height += -pNMUpDown->iDelta*1;
	if(height>m_nHeight)
		height = m_nHeight;
	else if(height<0)
		height = 0;
		
	
	CString str;
	str.Format("%f", height);
	m_editBrushHeight.SetWindowText(str);

	m_sliderBrushHeight.SetPos(height);
	pBrush->SetBrushHeight(height);
	
	*pResult = 0;
}

void CTerrainModifyStretchPanel::OnRadioBrushflat() 
{
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();

	
	CButton * temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHFLAT);
	if(temp)
	{
		temp->SetCheck(true);
	}
	temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHSMOOTH);
	if(temp)
		temp->SetCheck(false);

	pBrush->SetBrushSmoothSort(false);

	ShowSmoothCtrl(false);
}

void CTerrainModifyStretchPanel::OnRadioBrushsmooth() 
{
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();

	
	CButton * temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHFLAT);
	if(temp)
	{
		temp->SetCheck(false);
	}
	temp = (CButton *)GetDlgItem(IDC_RADIO_BRUSHSMOOTH);
	if(temp)
		temp->SetCheck(true);

	pBrush->SetBrushSmoothSort(true);
	ShowSmoothCtrl(true);

}

void CTerrainModifyStretchPanel::ShowSmoothCtrl(bool bSmooth)
{
	CWnd *pWnd = GetDlgItem(IDC_SLIDER_BRUSHHARD);
	/*
	if(pWnd) pWnd->EnableWindow(bSmooth);
	pWnd = GetDlgItem(IDC_STATIC_HARD);
	if(pWnd) pWnd->EnableWindow(bSmooth);
	pWnd = GetDlgItem(IDC_EDIT_BRUSHHARD);
	if(pWnd) pWnd->EnableWindow(bSmooth);
	pWnd = GetDlgItem(IDC_SPIN_BRUSHHARD);
	if(pWnd) pWnd->EnableWindow(bSmooth);
	pWnd = GetDlgItem(IDC_STATIC_SOFTBRUSH);
	if(pWnd) pWnd->EnableWindow(bSmooth);
	*/
	pWnd = GetDlgItem(IDC_STATIC_RADIUS);
	if(pWnd) pWnd->EnableWindow(!bSmooth);
	pWnd = GetDlgItem(IDC_STATIC_PERSISTENCE);
	if(pWnd) pWnd->EnableWindow(!bSmooth);
	pWnd = GetDlgItem(IDC_EDIT_STRETCH_NOISE_RADIUS);
	if(pWnd) pWnd->EnableWindow(!bSmooth);
	pWnd = GetDlgItem(IDC_EDIT_STRETCH_NOISE_PERSISTENCE);
	if(pWnd) pWnd->EnableWindow(!bSmooth);
	pWnd = GetDlgItem(IDC_SPIN_STRETCH_NOISE_RADIUS);
	if(pWnd) pWnd->EnableWindow(!bSmooth);
	pWnd = GetDlgItem(IDC_SPIN_STRETCH_NOISE_PERSISTENCE);
	if(pWnd) pWnd->EnableWindow(!bSmooth);
	
	g_CurveFilterView.Enable(bSmooth);
}

void CTerrainModifyStretchPanel::ShowHeightCtrl(bool bShow)
{
	CWnd *pWnd = GetDlgItem(IDC_SLIDER_BRUSHHEIGHT);
	if(pWnd) pWnd->EnableWindow(bShow);
	pWnd = GetDlgItem(IDC_STATIC_HEIGHT);
	if(pWnd) pWnd->EnableWindow(bShow);
	pWnd = GetDlgItem(IDC_EDIT_BRUSHHEIGHT);
	if(pWnd) pWnd->EnableWindow(bShow);
	pWnd = GetDlgItem(IDC_SPIN_BRUSHHEIGHT);
	if(pWnd) pWnd->EnableWindow(bShow);
	pWnd = GetDlgItem(IDC_STATIC_RADIUS);
	if(pWnd) pWnd->EnableWindow(bShow);
	pWnd = GetDlgItem(IDC_EDIT_STRETCH_NOISE_RADIUS);
	if(pWnd) pWnd->EnableWindow(bShow);
	pWnd = GetDlgItem(IDC_SPIN_STRETCH_NOISE_RADIUS);
	if(pWnd) pWnd->EnableWindow(bShow);
	
	pWnd = GetDlgItem(IDC_STATIC_HARD);
	if(pWnd) pWnd->EnableWindow(!bShow);
	pWnd = GetDlgItem(IDC_EDIT_BRUSHHARD);
	if(pWnd) pWnd->EnableWindow(!bShow);
	pWnd = GetDlgItem(IDC_SPIN_BRUSHHARD);
	if(pWnd) pWnd->EnableWindow(!bShow);
	pWnd = GetDlgItem(IDC_SLIDER_BRUSHHARD);
	if(pWnd) pWnd->EnableWindow(!bShow);
	
	
}

void CTerrainModifyStretchPanel::OnSelchangeComboStretchType() 
{
	// TODO: Add your control notification handler code here
	int sel = m_comboStrecthType.GetCurSel();
	CTerrainStretchOperation *pOperation = (CTerrainStretchOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	if(sel==0)
	{
		ShowHeightCtrl(false);
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTADD);
		if(pButton){pButton->EnableWindow(false);}
		pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTSUB);
		if(pButton){pButton->EnableWindow(false);}
		pOperation->SetStretchType(0);
	}else if(sel==1)
	{
		ShowHeightCtrl(false);
		pOperation->SetStretchType(1);
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTADD);
		if(pButton){pButton->EnableWindow(true);}
		pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTSUB);
		if(pButton){pButton->EnableWindow(true);}
	}else
	{
		ShowHeightCtrl(true);
		pOperation->SetStretchType(2);
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTADD);
		if(pButton){pButton->EnableWindow(false);}
		pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTSUB);
		if(pButton){pButton->EnableWindow(false);}
	}
}

void CTerrainModifyStretchPanel::OnDeltaposSpinStretchNoisePersistence(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int num = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	num = pBrush->GetNoisePersistence();

	num += -pNMUpDown->iDelta*1;
	if(num>100)
		num = 100;
	else if(num<1)
		num = 1;
		
	
	CString str;
	str.Format("%d", num);
	m_editNoisePersistence.SetWindowText(str);
	pBrush->SetNoisePersistence(num);

	((CTerrainStretchOperation*)pOperation)->UpdateNoise();
	*pResult = 0;
}

void CTerrainModifyStretchPanel::OnDeltaposSpinStretchNoiseRadius(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: //这儿现在已经改成固定高度拉伸的时间频率
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int num = 0;
	CTerrainStretchBrush *pBrush = 0;
	
	num = ((CTerrainStretchOperation*)pOperation)->GetFrequency();

	num += -pNMUpDown->iDelta*1;
	if(num>1000)
		num = 1000;
	else if(num<1)
		num = 1;
		
	
	CString str;
	str.Format("%d", num);
	m_editNoiseRadius.SetWindowText(str);
	((CTerrainStretchOperation*)pOperation)->SetFrequency(num);
	*pResult = 0;
}

void CTerrainModifyStretchPanel::OnChangeEditStretchNoisePersistence() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int num = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainStretchOperation*)pOperation)->GetBrush();
	
	CString str;
	m_editNoisePersistence.GetWindowText(str);
	num = (int)atof(str);
	if(num>100) 
	{
		num = 100;
		str.Format("%d", num);
		m_editNoisePersistence.SetWindowText(str);
	}else if(num<1)
	{
		num = 1;
		str.Format("%d", num);
		m_editNoisePersistence.SetWindowText(str);
	}

	pBrush->SetNoiseAmplitude(num);
	((CTerrainStretchOperation*)pOperation)->UpdateNoise();
}

void CTerrainModifyStretchPanel::OnChangeEditStretchNoiseRadius() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: //这儿现在已经改成固定高度拉伸的时间频率
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int num = 0;
	
	CString str;
	m_editNoiseRadius.GetWindowText(str);
	num = (int)atof(str);
	if(num>1000) 
	{
		num = 1000;
		str.Format("%d", num);
		m_editNoiseRadius.SetWindowText(str);
	}else if(num<1)
	{
		num = 1;
		str.Format("%d", num);
		m_editNoiseRadius.SetWindowText(str);
	}
	((CTerrainStretchOperation*)pOperation)->SetFrequency(num);
}

void CTerrainModifyStretchPanel::OnRadioHeightsub() 
{
	// TODO: Add your control notification handler code here
	CTerrainStretchOperation *pOperation = (CTerrainStretchOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	
	if(pOperation)
	{
		pOperation->SetHeightAdd(false);
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTADD);
		if(pButton) pButton->SetCheck(0);
	}
}

void CTerrainModifyStretchPanel::OnRadioHeightadd() 
{
	// TODO: Add your control notification handler code here
	CTerrainStretchOperation *pOperation = (CTerrainStretchOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	if(pOperation)
	{
		pOperation->SetHeightAdd(true);
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_HEIGHTSUB);
		if(pButton) pButton->SetCheck(0);
	}
}

void CTerrainModifyStretchPanel::UpdateCurrentBrushHeight(float fHeight)
{
	CStatic*p = (CStatic*)GetDlgItem(IDC_STATIC_BRUSHHEIGHT);
	float max = 0;
	CString str;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain *pTerrain = pMap->GetTerrain();
		if(pTerrain)
			max = pTerrain->GetMaxHeight();
		if(fHeight>max ) fHeight = max;
		else if(fHeight<0) fHeight = 0;
		str.Format("%4.2fm",fHeight);
		if(p) p->SetWindowText(str);
	}
	
	
}

void CTerrainModifyStretchPanel::OnCheckEnablenoise() 
{
	// TODO: Add your control notification handler code here
	CTerrainStretchOperation *pOperation = (CTerrainStretchOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	if(pOperation)
	{
		CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_ENABLENOISE);
		if(pButton->GetCheck()<=0)
			pOperation->EnableNoise(false);
		else pOperation->EnableNoise(true);
	}
}

void CTerrainModifyStretchPanel::UpdateBrushHeightRect()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain *pTerrain = pMap->GetTerrain();
		if(pTerrain)
			m_nHeight = pTerrain->GetMaxHeight();
	}
	m_sliderBrushHeight.SetRange(0,m_nHeight);
}

void CTerrainModifyStretchPanel::OnCheckLockMapEdge() 
{
	// TODO: Add your control notification handler code here
	CTerrainStretchOperation *pOperation = (CTerrainStretchOperation *)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	if(pOperation)
	{
		CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_LOCK_MAP_EDGE);
		if(((CTerrainStretchOperation*)pOperation)->GetLockEdgeState())
			pOperation->LockEdge(false);
		else pOperation->LockEdge(true);
	}
}

void CTerrainModifyStretchPanel::OnButtonSelBrush() 
{
	// TODO: Add your control notification handler code here
	CTerrainBrushSelDlg dlg;
	CString temp = m_strBrushName;
	int pos = m_strBrushName.ReverseFind('\\');
	if(pos!=-1)
	{
		temp = temp.Right(temp.GetLength() - pos - 1);
		dlg.m_strBrushPath = temp;
	}else dlg.m_strBrushPath = m_strBrushName; 
	if(IDOK==dlg.DoModal())
	{
		CTerrainStretchOperation *pOperation = (CTerrainStretchOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
		if(dlg.m_strBrushPath == "默认刷子") 
		{
			pOperation->EnableUserBrush(false);
			m_strBrushName = dlg.m_strBrushPath;
			UpdateData(false);
			return;
		}
		
		temp.Format("%sBrush\\%s",g_szWorkDir,dlg.m_strBrushPath);
		if(temp == m_strBrushName) return;
		m_strBrushName = temp;
		if(!m_strBrushName.IsEmpty())
		{
			HBITMAP hbmp = (HBITMAP)LoadImage(NULL,m_strBrushName,IMAGE_BITMAP,LR_DEFAULTSIZE ,LR_DEFAULTSIZE,LR_LOADFROMFILE);
			
			if(NULL==hbmp)
			{
				CString msg("加载刷子位图失败--");
				msg = msg + m_strBrushName;
				pOperation->EnableUserBrush(false);
				m_strBrushName = "默认刷子"; 
				UpdateData(false);
				MessageBox(msg);
				return;
			}
			BITMAP bmp;
			GetObject(hbmp,sizeof(BITMAP),&bmp);
			m_nBrushWidth = bmp.bmWidth;
			m_nBrushHeight = bmp.bmHeight; 
			int bit = bmp.bmBitsPixel;
			if(bit<8) 
			{
				DeleteObject(hbmp);
				pOperation->EnableUserBrush(false);
				m_strBrushName = "默认刷子"; 
				UpdateData(false);
				MessageBox("Only surport more than 8 bit bitmap");
				return;
			}
			if(m_pData) 
			{
				delete []m_pData;
				m_pData = NULL;
			}
			
			COLORREF *pData = new COLORREF[bmp.bmWidth*bmp.bmHeight*(bit/8)];
			ASSERT(pData);
			CBitmap bt;
			bt.Attach(hbmp);
			bt.GetBitmapBits(bmp.bmWidth*bmp.bmHeight*(bit/8),pData);  
			m_pData = new BYTE[bmp.bmWidth*bmp.bmHeight*(bit/8)];
			for(int i=0; i<m_nBrushHeight; i++)
			{
				for(int j=0; j<m_nBrushWidth; j++)
				{
					COLORREF color = pData[i*m_nBrushWidth + j];
					int ir = _getRed(color);
					int ig = _getGreen(color);
					int ib = _getBlue(color);
					m_pData[i*m_nBrushWidth + j] = (int)(0.33f*ir + 0.53f*ig + 0.13f*ib);
				}
			}
			delete []pData;
			DeleteObject(hbmp);
			UpdateData(false);
			pOperation->SetUserData(m_pData,m_nBrushWidth,m_nBrushHeight);
			pOperation->EnableUserBrush(true);
		}
	}
}

void CTerrainModifyStretchPanel::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pData) delete m_pData;
}
