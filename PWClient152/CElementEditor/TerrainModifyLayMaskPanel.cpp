// CTerrainModifyLayMaskPanel.cpp : implementation file
//

#include "Global.h"
#include "MainFrm.h"
#include "elementeditor.h"
#include "Operation.h"
#include "OperationManager.h"
#include "TerrainModifyLayMaskPanel.h"
#include "TerrainStretchBrush.h"
#include "TerrainMaskPaintOperation.h"
#include "TerrainMaskSmoothOperation.h"
#include "TerrainLayer.h"
#include "Render.h"
#include "CommonFileDlg.h"
#include "TerrainBrushSelDlg.h"

//#define new A_DEBUG_NEW



/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyLayMaskPanel dialog


CTerrainModifyLayMaskPanel::CTerrainModifyLayMaskPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainModifyLayMaskPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainModifyLayMaskPanel)
	m_strUserBrushName = _T("默认刷子");
	//}}AFX_DATA_INIT
	m_pData = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bUserBrush = false;
}


void CTerrainModifyLayMaskPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainModifyLayMaskPanel)
	DDX_Control(pDX, IDC_TERRAIN_MODIFY_LAYLIST, m_listLayMask);
	DDX_Control(pDX, IDC_SPIN_BRUSHLAYHARD, m_spinBrushHard);
	DDX_Control(pDX, IDC_SPIN_BRUSHHGRAY, m_spinBrushGray);
	DDX_Control(pDX, IDC_SLIDER_BRUSHLAYHARD, m_sliderBrushHard);
	DDX_Control(pDX, IDC_SLIDER_BRUSHHGRAY, m_sliderBrushGray);
	DDX_Control(pDX, IDC_EDIT_BRUSHLAYHARD, m_editBrushHard);
	DDX_Control(pDX, IDC_EDIT_BRUSHHGRAY, m_editBrushGray);
	DDX_Text(pDX, IDC_EDIT_USER_BRUSH_NAME, m_strUserBrushName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainModifyLayMaskPanel, CDialog)
	//{{AFX_MSG_MAP(CTerrainModifyLayMaskPanel)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRUSHHGRAY, OnReleasedcaptureSliderBrushhgray)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRUSHLAYHARD, OnReleasedcaptureSliderBrushlayhard)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRUSHHGRAY, OnDeltaposSpinBrushhgray)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRUSHLAYHARD, OnDeltaposSpinBrushlayhard)
	ON_EN_CHANGE(IDC_EDIT_BRUSHHGRAY, OnChangeEditBrushhgray)
	ON_EN_CHANGE(IDC_EDIT_BRUSHLAYHARD, OnChangeEditBrushlayhard)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_TERRAIN_MODIFY_LAYLIST, OnSelchangeTerrainModifyLaylist)
	ON_BN_CLICKED(IDC_FRESH_LAYER_LIST, OnFreshLayerList)
	ON_BN_CLICKED(IDC_RADIO_MASKPAINT_CLRSUB, OnRadioMaskpaintClrsub)
	ON_BN_CLICKED(IDC_RADIO_MASKPAINT_CLRADD, OnRadioMaskpaintClradd)
	ON_BN_CLICKED(IDC_CHECK_USER_BRUSH, OnCheckUserBrush)
	ON_BN_CLICKED(IDC_LOAD_BRUSH_RAW, OnLoadBrushRaw)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainModifyLayMaskPanel message handlers

BOOL CTerrainModifyLayMaskPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_sliderBrushGray.SetRange(0,255);
	m_sliderBrushHard.SetRange(0,100);

	AUX_FlatEditBox(&m_editBrushHard);
	AUX_FlatEditBox(&m_editBrushGray);

	
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_STRETCH);
	int hard,gray;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainMaskPaintOperation*)pOperation)->GetBrush();
	hard = pBrush->GetBrushHard();
	gray = pBrush->GetBrushGray();
	CString str;
	str.Format("%d", hard);
	m_editBrushHard.SetWindowText(str);
	str.Format("%d", gray);
	m_editBrushGray.SetWindowText(str);


	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_MASKPAINT_CLRADD);
	if(pButton) pButton->SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainModifyLayMaskPanel::OnReleasedcaptureSliderBrushhgray(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	int gray = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainMaskPaintOperation*)pOperation)->GetBrush();
	gray = pBrush->GetBrushGray();
	
	gray = m_sliderBrushGray.GetPos();
	CString str;
	str.Format("%d", gray);
	m_editBrushGray.SetWindowText(str);
	pBrush->SetBrushGray(gray);
	*pResult = 0;
}

void CTerrainModifyLayMaskPanel::OnReleasedcaptureSliderBrushlayhard(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	int hard = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainMaskPaintOperation*)pOperation)->GetBrush();
	hard = pBrush->GetBrushHard();
	
	hard = m_sliderBrushHard.GetPos();
	CString str;
	str.Format("%d", hard);
	m_editBrushHard.SetWindowText(str);
	pBrush->SetBrushHard(hard);
	*pResult = 0;
}

void CTerrainModifyLayMaskPanel::OnDeltaposSpinBrushhgray(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	int gray = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainMaskPaintOperation*)pOperation)->GetBrush();
	gray = pBrush->GetBrushGray();
	
	gray += -pNMUpDown->iDelta*1;
	if(gray>255)
		gray = 255;
	else if(gray<0)
		gray = 0;
		
	
	CString str;
	str.Format("%d", gray);
	m_editBrushGray.SetWindowText(str);

	m_sliderBrushGray.SetPos(gray);
	pBrush->SetBrushGray(gray);
	*pResult = 0;
}

void CTerrainModifyLayMaskPanel::OnDeltaposSpinBrushlayhard(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	int hard = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainMaskPaintOperation*)pOperation)->GetBrush();
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

void CTerrainModifyLayMaskPanel::OnChangeEditBrushhgray() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	int gray = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainMaskPaintOperation*)pOperation)->GetBrush();
	gray = pBrush->GetBrushGray();

	CString str;
	m_editBrushGray.GetWindowText(str);
	gray = (int)atof(str);
	if(gray>255) 
	{
		gray = 255;
		str.Format("%d", gray);
		m_editBrushGray.SetWindowText(str);
	}else if(gray<0)
	{
		gray = 0;
		str.Format("%d", gray);
		m_editBrushGray.SetWindowText(str);
	}

	m_sliderBrushGray.SetPos(gray);
	pBrush->SetBrushGray(gray);
}

void CTerrainModifyLayMaskPanel::OnChangeEditBrushlayhard() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	COperation *pOperation = g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	int hard = 0;
	CTerrainStretchBrush *pBrush = 0;
	if(pOperation)  pBrush = ((CTerrainMaskPaintOperation*)pOperation)->GetBrush();
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

void CTerrainModifyLayMaskPanel::UpdateLayList()
{
	//	Intialzie layer list
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain == NULL) return;
	
	int cur = m_listLayMask.GetCurSel();
	if(cur>=pTerrain->GetLayerNum() || cur<0)
		cur = -1;
	
	m_listLayMask.ResetContent();
	
	int i, iNumLayer = pTerrain->GetLayerNum();
	for (i=0; i < iNumLayer; i++)
	{
		CTerrainLayer* pLayer = pTerrain->GetLayer(i);
		int iItem = m_listLayMask.InsertString(-1, pLayer->GetName());
		m_listLayMask.SetItemData(iItem, (DWORD)pLayer);
	}

	//	Select current layer and update operation data 
	if(cur==-1)
	{
		m_listLayMask.SetCurSel(iNumLayer - 1);
		CTerrainMaskPaintOperation *pOperation = (CTerrainMaskPaintOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
		if(pOperation) pOperation->SetCurrentLayer(iNumLayer - 1);
	}else
	{
		m_listLayMask.SetCurSel(cur);
		CTerrainMaskPaintOperation *pOperation = (CTerrainMaskPaintOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
		if(pOperation) pOperation->SetCurrentLayer(cur);
	}
}

void CTerrainModifyLayMaskPanel::SetMaskLay(char* szLayName)
{
	int n = m_listLayMask.GetCount();	
	char txt[128];
	for(int i = 0 ; i < n; i++)
	{
		m_listLayMask.GetText(i,txt);
		if(strcmp(txt,szLayName)==0)
		{
			m_listLayMask.SetCurSel(i);
			CTerrainMaskPaintOperation *pOperation = (CTerrainMaskPaintOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
			if(pOperation) pOperation->SetCurrentLayer(i);
			break;
		}
	}
	
}

void CTerrainModifyLayMaskPanel::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
	UpdateLayList();
	
}

void CTerrainModifyLayMaskPanel::OnSelchangeTerrainModifyLaylist() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain == NULL) return;
	
	char txt[128];
	int i = m_listLayMask.GetCurSel();
	if(i>pTerrain->GetLayerNum() || i<0)
	{
		UpdateLayList();
		return;
	}
	m_listLayMask.GetText(i,txt);
	
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
	
	CTerrainMaskPaintOperation *pOperation = (CTerrainMaskPaintOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	if(pOperation) pOperation->SetCurrentLayer(i);
}

void CTerrainModifyLayMaskPanel::OnFreshLayerList() 
{
	// TODO: Add your control notification handler code here
	UpdateLayList();
}

void CTerrainModifyLayMaskPanel::OnRadioMaskpaintClrsub() 
{
	// TODO: Add your control notification handler code here
	CTerrainMaskPaintOperation *pOperation = (CTerrainMaskPaintOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	if(pOperation)
	{
		pOperation->SetPaintColorAdd(false);
		//pOperation->RecreateTerrainRender();
	}

	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_MASKPAINT_CLRADD);
	if(pButton) pButton->SetCheck(0);

}

void CTerrainModifyLayMaskPanel::OnRadioMaskpaintClradd() 
{
	// TODO: Add your control notification handler code here
	CTerrainMaskPaintOperation *pOperation = (CTerrainMaskPaintOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
	if(pOperation)
	{
		pOperation->SetPaintColorAdd(true);
		//pOperation->RecreateTerrainRender();
	}

	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_MASKPAINT_CLRSUB);
	if(pButton) pButton->SetCheck(0);
}

void CTerrainModifyLayMaskPanel::OnCheckUserBrush() 
{
	// TODO: Add your control notification handler code here
}

void CTerrainModifyLayMaskPanel::OnLoadBrushRaw() 
{
	// TODO: Add your control notification handler code here
	CTerrainBrushSelDlg dlg;
	CString temp = m_strUserBrushName;
	int pos = m_strUserBrushName.ReverseFind('\\');
	if(pos!=-1)
	{
		temp = temp.Right(temp.GetLength() - pos - 1);
		dlg.m_strBrushPath = temp;
	}else dlg.m_strBrushPath = m_strUserBrushName; 
	if(IDOK==dlg.DoModal())
	{
		CTerrainMaskPaintOperation *pOperation = (CTerrainMaskPaintOperation*)g_OperationContainer.GetOperation(OPERATION_TERRAIN_PAINT);
		if(dlg.m_strBrushPath == "默认刷子") 
		{
			pOperation->EnableUserBrush(m_bUserBrush);
			m_strUserBrushName = dlg.m_strBrushPath;
			UpdateData(false);
			return;
		}
		
		temp.Format("%sBrush\\%s",g_szWorkDir,dlg.m_strBrushPath);
		if(temp == m_strUserBrushName) return;
		m_strUserBrushName = temp;
		if(!m_strUserBrushName.IsEmpty())
		{
			HBITMAP hbmp = (HBITMAP)LoadImage(NULL,m_strUserBrushName,IMAGE_BITMAP,LR_DEFAULTSIZE ,LR_DEFAULTSIZE,LR_LOADFROMFILE);
			
			if(NULL==hbmp)
			{
				CString msg("加载刷子位图失败--");
				msg = msg + m_strUserBrushName;
				pOperation->EnableUserBrush(false);
				m_strUserBrushName = "默认刷子"; 
				UpdateData(false);
				MessageBox(msg);
				return;
			}
			BITMAP bmp;
			GetObject(hbmp,sizeof(BITMAP),&bmp);
			m_nWidth = bmp.bmWidth;
			m_nHeight = bmp.bmHeight; 
			int bit = bmp.bmBitsPixel;
			if(bit<8) 
			{
				DeleteObject(hbmp);
				pOperation->EnableUserBrush(false);
				m_strUserBrushName = "默认刷子"; 
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
			for(int i=0; i<m_nHeight; i++)
			{
				for(int j=0; j<m_nWidth; j++)
				{
					COLORREF color = pData[i*m_nWidth + j];
					int ir = _getRed(color);
					int ig = _getGreen(color);
					int ib = _getBlue(color);
					m_pData[i*m_nWidth + j] = (int)(0.33f*ir + 0.53f*ig + 0.13f*ib);
				}
			}
			delete []pData;
			DeleteObject(hbmp);
			UpdateData(false);
			pOperation->SetUserData(m_pData,m_nWidth,m_nHeight);
			pOperation->EnableUserBrush(true);
		}
	}
}

BOOL CTerrainModifyLayMaskPanel::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	 
	return CDialog::DestroyWindow();
}

void CTerrainModifyLayMaskPanel::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

void CTerrainModifyLayMaskPanel::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pData) delete m_pData;
}
