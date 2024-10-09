// DlgSurfaces.cpp : implementation file
//

#include "Global.h"
#include "DlgSurfaces.h"
#include "DlgSelectFile.h"
#include "CommonFileDlg.h"
#include "TerrainLayer.h"
#include "Bitmap.h"
#include "AutoMask.h"
#include "Terrain.h"
#include "Render.h"

#include "AF.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

//	Layer file version
#define LAYERFILEVERSION		2

//	Flag used to input data
enum
{
	INPUT_NEWLAYER = 0,
	INPUT_EDITLAYER,
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSurfaces dialog

CDlgSurfaces::CDlgSurfaces(CTerrain* pTerrain, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSurfaces::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSurfaces)
	m_bEnable = FALSE;
	m_bAutoMask = FALSE;
	m_bSmooth = FALSE;
	m_bMaskAxisX = FALSE;
	m_bMaskAxisY = FALSE;
	m_bMaskAxisZ = FALSE;
	m_strTexturePath1 = _T("");
	m_strTexturePath2 = _T("");
	//}}AFX_DATA_INIT

	m_pTerrain		= pTerrain;
	m_bModified		= false;
	m_pTexture		= NULL;
	m_pSpecuMap		= NULL;
	m_bAddDelLayer	= false;
}


void CDlgSurfaces::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSurfaces)
	DDX_Control(pDX, IDC_COMBO_WEIGHT, m_WeightCombo);
	DDX_Control(pDX, IDC_BTN_DELSPECUMAP, m_DelSpecuMapBtn);
	DDX_Control(pDX, IDC_COMBO_AUTOMASKSIZE, m_MaskSizeCombo);
	DDX_Control(pDX, IDC_BTN_MOVEUPLAYER, m_MULayerBtn);
	DDX_Control(pDX, IDC_BTN_MOVEDNLAYER, m_MDLayerBtn);
	DDX_Control(pDX, IDC_SPIN_VSCALE, m_VScaleSpin);
	DDX_Control(pDX, IDC_SPIN_USCALE, m_UScaleSpin);
	DDX_Control(pDX, IDC_EDIT_VSCALE, m_VScaleEdit);
	DDX_Control(pDX, IDC_EDIT_USCALE, m_UScaleEdit);
	DDX_Control(pDX, IDC_EDIT_ALTISTART, m_AltiStartEdit);
	DDX_Control(pDX, IDC_EDIT_ALTIEND, m_AltiEndEdit);
	DDX_Control(pDX, IDC_LIST_LAYERS, m_LayerList);
	DDX_Control(pDX, IDC_COMBO_PROJAXIS, m_ProjAxisCombo);
	DDX_Control(pDX, IDC_EDIT_SLOPESTART, m_SlopeStartEdit);
	DDX_Control(pDX, IDC_EDIT_SLOPEEND, m_SlopeEndEdit);
	DDX_Control(pDX, IDC_BTN_ADDLAYER, m_AddLayerBtn);
	DDX_Control(pDX, IDC_BTN_DELLAYER, m_DelLayerBtn);
	DDX_Check(pDX, IDC_CHECK_ENABLE, m_bEnable);
	DDX_Check(pDX, IDC_CHECK_AUTOGENMASK, m_bAutoMask);
	DDX_Check(pDX, IDC_CHECK_SMOOTH, m_bSmooth);
	DDX_Check(pDX, IDC_CHECK_MASKAXIS_X, m_bMaskAxisX);
	DDX_Check(pDX, IDC_CHECK_MASKAXIS_Y, m_bMaskAxisY);
	DDX_Check(pDX, IDC_CHECK_MASKAXIS_Z, m_bMaskAxisZ);
	DDX_Text(pDX, IDC_EDIT_TEXTURE_PATH1, m_strTexturePath1);
	DDX_Text(pDX, IDC_EDIT_TEXTURE_PATH2, m_strTexturePath2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSurfaces, CDialog)
	//{{AFX_MSG_MAP(CDlgSurfaces)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_LOADTEXTURE, OnBtnLoadtexture)
	ON_BN_CLICKED(IDC_CHECK_AUTOGENMASK, OnCheckAutogenmask)
	ON_LBN_SELCHANGE(IDC_LIST_LAYERS, OnSelchangeListLayers)
	ON_BN_CLICKED(IDC_BTN_ADDLAYER, OnBtnAddlayer)
	ON_BN_CLICKED(IDC_BTN_DELLAYER, OnBtnDellayer)
	ON_BN_CLICKED(IDC_BTN_MOVEDNLAYER, OnBtnMovednlayer)
	ON_BN_CLICKED(IDC_BTN_MOVEUPLAYER, OnBtnMoveuplayer)
	ON_BN_CLICKED(IDC_BTN_IMPORTMASK, OnBtnImportmask)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_USCALE, OnDeltaposSpinUscale)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VSCALE, OnDeltaposSpinVscale)
	ON_CBN_SELCHANGE(IDC_COMBO_PROJAXIS, OnSelchangeComboProjaxis)
	ON_BN_DOUBLECLICKED(IDC_BTN_MOVEDNLAYER, OnDoubleclickedBtnMovednlayer)
	ON_BN_DOUBLECLICKED(IDC_BTN_MOVEUPLAYER, OnDoubleclickedBtnMoveuplayer)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_EXPORTMASK, OnBtnExportmask)
	ON_BN_CLICKED(IDC_CHECK_ENABLE, OnCheckEnable)
	ON_BN_CLICKED(IDC_CHECK_SMOOTH, OnCheckSmooth)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTOMASKSIZE, OnSelchangeComboAutomasksize)
	ON_BN_CLICKED(IDC_CHECK_MASKAXIS_X, OnCheckMaskaxisX)
	ON_BN_CLICKED(IDC_CHECK_MASKAXIS_Y, OnCheckMaskaxisY)
	ON_BN_CLICKED(IDC_CHECK_MASKAXIS_Z, OnCheckMaskaxisZ)
	ON_LBN_DBLCLK(IDC_LIST_LAYERS, OnDblclkListLayers)
	ON_BN_CLICKED(IDC_BTN_LOADSPECUMAP, OnBtnLoadspecumap)
	ON_BN_CLICKED(IDC_BTN_DELSPECUMAP, OnBtnDelspecumap)
	ON_COMMAND(ID_SUF_FILE_LOAD, OnSufFileLoad)
	ON_COMMAND(ID_SUF_FILE_SAVE, OnSufFileSave)
	ON_CBN_SELCHANGE(IDC_COMBO_WEIGHT, OnSelchangeComboWeight)
	ON_EN_KILLFOCUS(IDC_EDIT_ALTIEND, OnKillfocusEditAltiend)
	ON_EN_KILLFOCUS(IDC_EDIT_ALTISTART, OnKillfocusEditAltistart)
	ON_EN_KILLFOCUS(IDC_EDIT_SLOPEEND, OnKillfocusEditSlopeend)
	ON_EN_KILLFOCUS(IDC_EDIT_SLOPESTART, OnKillfocusEditSlopestart)
	ON_EN_KILLFOCUS(IDC_EDIT_USCALE, OnKillfocusEditUscale)
	ON_EN_KILLFOCUS(IDC_EDIT_VSCALE, OnKillfocusEditVscale)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_INPUTWNDEND, OnInputWndEnd)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSurfaces message handlers

int CDlgSurfaces::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(m_pTerrain);

	//	Create texture window
	RECT rcWnd = {0, 0, 100, 100};
	if (!m_TextureWnd.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, this, rcWnd, 0))
	{
		g_Log.Log("CDlgSurfaces::OnCreate, Failed to create texture window");
		return -1;
	}

	//	Create specular map window
	if (!m_SpecuMapWnd.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, this, rcWnd, 0))
	{
		g_Log.Log("CDlgSurfaces::OnCreate, Failed to create texture window");
		return -1;
	}

	//	Create mask preview window
	if (!m_MaskWnd.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, this, rcWnd, 0))
	{
		g_Log.Log("CDlgSurfaces::OnCreate, Failed to create auto mask window");
		return -1;
	}

	//	Create input window
	if (!m_InputWnd.Create(NULL, NULL, WS_POPUP, rcWnd, this, 0))
		return -1;

	return 0;
}

void CDlgSurfaces::OnDestroy() 
{
	CDialog::OnDestroy();

	ReleaseTexture();
}

BOOL CDlgSurfaces::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i;

	//	Change edit boxes to flat style
	AUX_FlatEditBox(&m_UScaleEdit);
	AUX_FlatEditBox(&m_VScaleEdit);
	AUX_FlatEditBox(&m_SlopeStartEdit);
	AUX_FlatEditBox(&m_SlopeEndEdit);
	AUX_FlatEditBox(&m_AltiStartEdit);
	AUX_FlatEditBox(&m_AltiEndEdit);

	//	Set icon for buttons
	m_AddLayerBtn.SetButtonIcon(16, IDI_NEWLAYER);
	m_DelLayerBtn.SetButtonIcon(16, IDI_DELETE);
	m_MDLayerBtn.SetButtonIcon(16, IDI_MOVEDOWN);
	m_MULayerBtn.SetButtonIcon(16, IDI_MOVEUP);
	m_DelSpecuMapBtn.SetButtonIcon(16, IDI_DELETE);

	//	Initialize projection axis list
	m_ProjAxisCombo.InsertString(-1, "X");
	m_ProjAxisCombo.InsertString(-1, "Y");
	m_ProjAxisCombo.InsertString(-1, "Z");

	//	Initialize layer weight list
	char szStr[10];
	for (i=0; i <= A3DTerrain2::MAX_LAYERWEIGHT; i++)
	{
		itoa(i, szStr, 10);
		m_WeightCombo.InsertString(-1, szStr);
	}

	//	Get texture window's position
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_TEXTURE)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	m_TextureWnd.MoveWindow(&rcWnd);

	//	Get specular map window's position
	GetDlgItem(IDC_STATIC_SPECUMAP)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	m_SpecuMapWnd.MoveWindow(&rcWnd);

	//	Adjust auto mask preview window's position
	GetDlgItem(IDC_STATIC_AUTOMASK)->GetWindowRect(&rcWnd);
	rcWnd.right	 = rcWnd.left + 250;
	rcWnd.bottom = rcWnd.top + 250;
	ScreenToClient(&rcWnd);
	m_MaskWnd.MoveWindow(&rcWnd);

	//	Initalize mask map size combo
	m_MaskSizeCombo.InsertString(-1, "512 x 512");
	m_MaskSizeCombo.InsertString(-1, "1024 x 1024");
	m_MaskSizeCombo.InsertString(-1, "2048 x 2048");
	m_MaskSizeCombo.InsertString(-1, "4096 x 4096");
	m_MaskSizeCombo.InsertString(-1, "8192 x 8192");

	//	Inherit parent's font
	m_InputWnd.InheritFont();

	InitLayers(0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Initialize layers
void CDlgSurfaces::InitLayers(int iCurLayer)
{
	//	Intialzie layer list
	m_LayerList.ResetContent();
	
	int i, iNumLayer = m_pTerrain->GetLayerNum();
	for (i=0; i < iNumLayer; i++)
	{
		CTerrainLayer* pLayer = m_pTerrain->GetLayer(i);
		int iItem = m_LayerList.InsertString(-1, pLayer->GetName());
		m_LayerList.SetItemData(iItem, (DWORD)pLayer);
	}

	//	Select current layer
	m_LayerList.SetCurSel(iCurLayer);

	ChangeLayer(iCurLayer);
}

//	Change current selected layer
void CDlgSurfaces::ChangeLayer(int iLayer)
{
	ASSERT(iLayer >= 0 && iLayer < m_LayerList.GetCount());
	CTerrainLayer* pLayer = (CTerrainLayer*)m_LayerList.GetItemData(iLayer);

	CString str;

	m_bEnable	= pLayer->IsEnable();
	m_bAutoMask	= pLayer->GetMaskType() == CTerrainLayer::MASK_AUTO ? TRUE : FALSE;
	m_bSmooth	= pLayer->GetAutoMaskSmoothFlag() ? TRUE : FALSE;

	//	Scales
	str.Format("%.3f", 1.0f / pLayer->GetScaleU());
	m_UScaleEdit.SetWindowText(str);
	str.Format("%.3f", 1.0f / pLayer->GetScaleV());
	m_VScaleEdit.SetWindowText(str);

	//	Auto mask parameters
	str.Format("%.3f", pLayer->GetAltiStart());
	m_AltiStartEdit.SetWindowText(str);
	str.Format("%.3f", pLayer->GetAltiEnd());
	m_AltiEndEdit.SetWindowText(str);
	str.Format("%.3f", pLayer->GetSlopeStart());
	m_SlopeStartEdit.SetWindowText(str);
	str.Format("%.3f", pLayer->GetSlopeEnd());
	m_SlopeEndEdit.SetWindowText(str);

	//	Projection axis
	m_ProjAxisCombo.SetCurSel(pLayer->GetProjAxis());

	//	Layer's weight
	m_WeightCombo.SetCurSel(pLayer->GetLayerWeight());

	//	Mask map size
	int iMapSize = 512;
	int i;
	for (i=0; i < NUM_SIZE; i++)
	{
		if (iMapSize == pLayer->GetAutoMaskMapSize())
		{
			m_MaskSizeCombo.SetCurSel(i);
			break;
		}

		iMapSize <<= 1;
	}

	if (i >= NUM_SIZE)
		m_MaskSizeCombo.SetCurSel(SIZE_1024);

	//	Auto mask axis
	DWORD dwMaskAxisFlag = pLayer->GetAutoMaskAxisFlag();
	m_bMaskAxisX = (dwMaskAxisFlag & AXIS_X) ? TRUE : FALSE;
	m_bMaskAxisY = (dwMaskAxisFlag & AXIS_Y) ? TRUE : FALSE;
	m_bMaskAxisZ = (dwMaskAxisFlag & AXIS_Z) ? TRUE : FALSE;

	//	Update texture window
	if (pLayer->HasTexture())
	{
		char szFullFile[MAX_PATH];
		af_GetFullPath(szFullFile, pLayer->GetTextureFile());
		ChangeTexture(szFullFile);
		m_strTexturePath1 = CString(szFullFile);
	}
	else
	{
		ChangeTexture(NULL);
		m_strTexturePath1 = "";
	}
		

	//	Update specular window
	if (pLayer->HasSpecularMap())
	{
		char szFullFile[MAX_PATH];
		af_GetFullPath(szFullFile, pLayer->GetSpecularMapFile());
		m_strTexturePath2 = CString(szFullFile);
		ChangeSpecularMap(szFullFile);
	}
	else
	{
		ChangeSpecularMap(NULL);
		m_strTexturePath2 = "";
	}
	//	Is default layer ?
	if (pLayer->IsDefault() || !pLayer->GetMaskBitmap())
		m_MaskWnd.SetBitmap(NULL, false);
	else
		m_MaskWnd.SetBitmap(pLayer->GetMaskBitmap()->GetBitmap(), false);

	SetMaskViewTitle();

	//	Enable / Disable controls
	EnableAutoMaskControls();

	UpdateData(FALSE);
}

//	Enable controls base on auto mask flag
void CDlgSurfaces::EnableAutoMaskControls()
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	if (!m_bAutoMask || pLayer->IsDefault())
	{
		//	Disable auto mask controls
		GetDlgItem(IDC_CHECK_SMOOTH)->EnableWindow(FALSE);

		GetDlgItem(IDC_EDIT_ALTISTART)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ALTIEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SLOPESTART)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SLOPEEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_MASKAXIS_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_MASKAXIS_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_MASKAXIS_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_AUTOMASKSIZE)->EnableWindow(FALSE);

		if (pLayer->IsDefault())
		{
			GetDlgItem(IDC_CHECK_ENABLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_AUTOGENMASK)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_IMPORTMASK)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_ENABLE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_AUTOGENMASK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_IMPORTMASK)->EnableWindow(TRUE);
		}
	}
	else
	{
		//	Enable auto mask controls
		GetDlgItem(IDC_CHECK_ENABLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_AUTOGENMASK)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SMOOTH)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_ALTISTART)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_ALTIEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SLOPESTART)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SLOPEEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_MASKAXIS_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_MASKAXIS_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_MASKAXIS_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_AUTOMASKSIZE)->EnableWindow(TRUE);

		GetDlgItem(IDC_BTN_IMPORTMASK)->EnableWindow(FALSE);
	}
}

//	Get selected layer
CTerrainLayer* CDlgSurfaces::GetSelLayer()
{
	int iCurSel = m_LayerList.GetCurSel();
	if (iCurSel == LB_ERR)
		return NULL;

	return (CTerrainLayer*)m_LayerList.GetItemData(iCurSel);
}

bool CDlgSurfaces::CheckLayerValid()
{
	int n  = m_LayerList.GetCount();
	for( int i = 0; i < n ; i++)
	{
		CTerrainLayer* pData = (CTerrainLayer*)m_LayerList.GetItemData(i);
		if(pData) 
		{
			CString tex(pData->GetTextureFile());
			if(tex.IsEmpty()) return false;
		}
	}
	return true;
}

//	Enable / Disable auto mask
void CDlgSurfaces::OnCheckAutogenmask() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer || pLayer->IsDefault())
		return;

	bool bAuto = !m_bAutoMask;
	if (bAuto && pLayer->GetMaskType() == CTerrainLayer::MASK_EDIT)
	{
		if (MessageBox("掩膜已经被直接修改过，如果自动生成掩膜，修改的信息将会丢失。是否继续？", "提示", MB_YESNO) == IDNO)
		{
			//	Remain check box's state
			CDataExchange dx(this, FALSE);
			DDX_Check(&dx, IDC_CHECK_AUTOGENMASK, m_bAutoMask);
			return;
		}
	}

	m_bAutoMask = bAuto;
	pLayer->ChangeMaskType(bAuto ? CTerrainLayer::MASK_AUTO : CTerrainLayer::MASK_NONE);

	if (pLayer->GetMaskBitmap())
		m_MaskWnd.SetBitmap(pLayer->GetMaskBitmap()->GetBitmap(), false);
	else
		m_MaskWnd.SetBitmap(NULL, false);

	//	Enable / Disable controls
	EnableAutoMaskControls();

	m_bModified = true;
}

void CDlgSurfaces::OnCheckSmooth() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer || pLayer->IsDefault())
		return;

	m_bSmooth = !m_bSmooth;
	pLayer->SetAutoMaskSmoothFlag(m_bSmooth ? true : false);
	UpdateAutoMaskMap(pLayer, 0);

	m_bModified = true;
}

//	Change mask map size
void CDlgSurfaces::OnSelchangeComboAutomasksize() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer || pLayer->IsDefault())
		return;

	int iCurSel = m_MaskSizeCombo.GetCurSel();
	if (iCurSel == CB_ERR)
		return;

	int iNewSize = 512 << iCurSel;
	UpdateAutoMaskMap(pLayer, iNewSize);

	SetMaskViewTitle();

	m_bModified = true;
}

//	Change current layer
void CDlgSurfaces::OnSelchangeListLayers() 
{
	int iSel = m_LayerList.GetCurSel();
	if (iSel == LB_ERR)
		return;

	ChangeLayer(iSel);
}

HRESULT CDlgSurfaces::OnInputWndEnd(WPARAM wParam, LPARAM lParam)
{
	bool bCancel = wParam ? true : false;

	if (lParam == INPUT_NEWLAYER)
	{
		int iCount = m_LayerList.GetCount();

		//	Add a new layer
		CString strResult = m_InputWnd.GetResultString();
		if (bCancel || !strResult.GetLength())
		{
			//	Cancel the operation
			m_LayerList.DeleteString(iCount-1);
			return S_OK;
		}

		//	Check name collision
		int iRet = m_pTerrain->SearchLayer(strResult);
		if (iRet >= 0 && iRet != iCount-1)
		{
			//	Cancel the operation
			m_LayerList.DeleteString(iCount-1);
			MessageBox("层命名冲突！");
			return S_OK;
		}

		//	Create a new layer
		CTerrainLayer* pLayer = m_pTerrain->CreateLayer(strResult, false);
		if (!pLayer)
		{
			//	Cancel the operation
			m_LayerList.DeleteString(iCount-1);
			MessageBox("创建层失败！");
			return S_OK;
		}

		//	In order to change item text, we have to delete it before and then
		//	add it again
		m_LayerList.DeleteString(iCount-1);
		int iItem = m_LayerList.InsertString(-1, strResult);
		m_LayerList.SetItemData(iItem, (DWORD)pLayer);

		m_bModified = true;
		m_bAddDelLayer = true;
	}
	else if (lParam == INPUT_EDITLAYER)
	{
		int iCurSel = m_LayerList.GetCurSel();

		//	Add a new layer
		CString strResult = m_InputWnd.GetResultString();
		if (bCancel || !strResult.GetLength() || !iCurSel)
			return S_OK;

		//	Check name collision
		int iRet = m_pTerrain->SearchLayer(strResult);
		if (iRet >= 0 && iRet != iCurSel)
		{
			//	Cancel the operation
			MessageBox("层命名冲突！");
			return S_OK;
		}

		//	Change layer name
		CTerrainLayer* pLayer = (CTerrainLayer*)m_LayerList.GetItemData(iCurSel);
		ASSERT(pLayer);
		pLayer->SetName(strResult);

		//	Delete this item and insert it again
		m_LayerList.DeleteString(iCurSel);
		m_LayerList.InsertString(iCurSel, strResult);
		m_LayerList.SetItemData(iCurSel, (DWORD)pLayer);

		m_bModified = true;
		m_bAddDelLayer = true;
	}

	return S_OK;
}

//	Add a layer
void CDlgSurfaces::OnBtnAddlayer() 
{
	//	Insert a new item to list
	int iItem = m_LayerList.InsertString(-1, "New");
	
	//	Ensure new item is visible
	RECT rcItem, rcList;
	m_LayerList.GetClientRect(&rcList);
	m_LayerList.GetItemRect(iItem, &rcItem);

	if (rcItem.top < 0)
	{
		m_LayerList.SetTopIndex(iItem);
	}
	else if (rcItem.bottom > rcList.bottom)
	{
		int iItemHei = rcItem.bottom - rcItem.top;
		int iTop = iItem - (rcList.bottom - rcList.top) / iItemHei + 1;
		if (iTop < 0)
			iTop = 0;

		m_LayerList.SetTopIndex(iTop);
	}

	//	Popup input window
	m_LayerList.GetItemRect(iItem, &rcItem);
	m_LayerList.ClientToScreen(&rcItem);
	rcItem.top		-= 1;
	rcItem.bottom	+= 1;
	rcItem.left		+= 1;
	rcItem.right	-= 1;

	CString str = "";
	m_InputWnd.Popup(this, INPUT_NEWLAYER, CWndInput::DATA_STRING, (void*)(LPCTSTR)str, rcItem);
}

//	Double click a layer item
void CDlgSurfaces::OnDblclkListLayers() 
{
	//	Insert a new item to list
	int iItem = m_LayerList.GetCurSel();
	if (iItem == LB_ERR || !iItem)
		return;
	
	//	Popup input window
	RECT rcItem;
	m_LayerList.GetItemRect(iItem, &rcItem);

	m_LayerList.ClientToScreen(&rcItem);
	rcItem.top		-= 1;
	rcItem.bottom	+= 1;
	rcItem.left		+= 1;
	rcItem.right	-= 1;

	CString str;
	m_LayerList.GetText(iItem, str);
	m_InputWnd.Popup(this, INPUT_EDITLAYER, CWndInput::DATA_STRING, (void*)(LPCTSTR)str, rcItem);
}

//	Delete a layer
void CDlgSurfaces::OnBtnDellayer() 
{
	int iSel = m_LayerList.GetCurSel();
	if (iSel == LB_ERR || !iSel)
		return;	//	Don't allow to delete default layer

	if (MessageBox("确定要删除选中的层吗？", "提示", MB_YESNO) == IDNO)
		return;

	m_pTerrain->ReleaseLayer(iSel);

	//	Update list
	m_LayerList.DeleteString(iSel);
	if (iSel >= m_LayerList.GetCount())
		iSel = m_LayerList.GetCount() - 1;

	m_LayerList.SetCurSel(iSel);
	ChangeLayer(iSel);

	m_bModified = true;
	m_bAddDelLayer = true;
}

//	Move a layer down
void CDlgSurfaces::OnBtnMovednlayer() 
{
	int iSel = m_LayerList.GetCurSel();
	if (iSel == LB_ERR || !iSel)
		return;	//	Could't move default layer

	if (iSel == m_LayerList.GetCount()-1)
		return;	//	Has been the last item

	CTerrainLayer* pLayer = m_pTerrain->GetLayer(iSel+1);
	m_pTerrain->SetLayer(iSel+1, m_pTerrain->GetLayer(iSel));
	m_pTerrain->SetLayer(iSel, pLayer);

	//	Delete this item first and insert it again
	pLayer = (CTerrainLayer*)m_LayerList.GetItemData(iSel);
	m_LayerList.DeleteString(iSel);
	iSel = m_LayerList.InsertString(iSel+1, pLayer->GetName());
	m_LayerList.SetItemData(iSel, (DWORD)pLayer);
	m_LayerList.SetCurSel(iSel);

	m_bModified = true;
	m_bAddDelLayer = true;
}

//	Move a layer up
void CDlgSurfaces::OnBtnMoveuplayer() 
{
	int iSel = m_LayerList.GetCurSel();
	if (iSel == LB_ERR || !iSel || iSel == 1)
		return;	//	Could't move default layer and the No.1 layer

	CTerrainLayer* pLayer = m_pTerrain->GetLayer(iSel-1);
	m_pTerrain->SetLayer(iSel-1, m_pTerrain->GetLayer(iSel));
	m_pTerrain->SetLayer(iSel, pLayer);

	//	Delete this item first and insert it again
	pLayer = (CTerrainLayer*)m_LayerList.GetItemData(iSel);
	m_LayerList.DeleteString(iSel);
	iSel = m_LayerList.InsertString(iSel-1, pLayer->GetName());
	m_LayerList.SetItemData(iSel, (DWORD)pLayer);
	m_LayerList.SetCurSel(iSel);

	m_bModified = true;
	m_bAddDelLayer = true;
}

void CDlgSurfaces::OnDoubleclickedBtnMovednlayer() 
{
	OnBtnMovednlayer();
}

void CDlgSurfaces::OnDoubleclickedBtnMoveuplayer() 
{
	OnBtnMoveuplayer();	
}

//	Load texture from file
void CDlgSurfaces::OnBtnLoadtexture() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

/*	CString strPath = g_szWorkDir;
	strPath += "Textures";

	CDlgSelectFile::PARAMS Params;
	Params.bMultiSelect	= false;
	Params.szFileFilter	= "*.*";
	Params.szPrompt		= "请选择一个贴图文件";
	Params.szRootDir	= strPath;

	CDlgSelectFile FileDlg(Params);
	if (FileDlg.DoModal() != IDOK)
		return;
*/
	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "");
	if (FileDlg.DoModal() != IDOK)
		return;

	if (!ChangeTexture(FileDlg.GetFullFileName()))
	{
		AUX_MessageBox(MB_OK, "加载贴图文件失败!");
		return;
	}
	m_bModified = true;
	m_strTexturePath1 = FileDlg.GetFullFileName();
	UpdateData(false);
}

void CDlgSurfaces::OnBtnLoadspecumap() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "");
	if (FileDlg.DoModal() != IDOK)
		return;

	if (!ChangeSpecularMap(FileDlg.GetFullFileName()))
	{
		AUX_MessageBox(MB_OK, "加载高光图文件失败!");
		return;
	}
	m_bModified = true;
	
	m_strTexturePath2 = FileDlg.GetFullFileName();
	UpdateData(false);
}

//	Remove specular map
void CDlgSurfaces::OnBtnDelspecumap() 
{
	ChangeSpecularMap("");
	m_bModified = true;
}

void CDlgSurfaces::OnBtnImportmask() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	CCommonFileDlg FileDlg(g_Render.GetA3DEngine(), "");
	if (FileDlg.DoModal() != IDOK)
		return;

	if (!ChangeMaskMap(FileDlg.GetFullFileName()))
	{
		AUX_MessageBox(MB_OK, "更改掩膜失败!");
		return;
	}

	m_bModified = true;
}

//	Change mask map
bool CDlgSurfaces::ChangeMaskMap(const char* szFile)
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return false;

	if (pLayer->GetMaskType() == CTerrainLayer::MASK_EDIT)
	{
		if (MessageBox("掩膜已经被直接修改过，如果加载掩膜，修改的信息将会丢失。是否继续？", "提示", MB_YESNO) == IDNO)
			return true;
	}

	pLayer->ChangeMaskType(CTerrainLayer::MASK_FROMFILE);

	if (szFile && szFile[0])
	{
		if (!pLayer->LoadMaskMap(szFile))
		{
			MessageBox("加载文件失败！");
			return false;
		}

		m_MaskWnd.SetBitmap(pLayer->GetMaskBitmap()->GetBitmap(), false);
	}
	else
		m_MaskWnd.SetBitmap(NULL, false);

	SetMaskViewTitle();

	return true;
}

//	Change texture
bool CDlgSurfaces::ChangeTexture(const char* szFile)
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return false;

	//	Release current texture
	ReleaseTexture();

	if (szFile && szFile[0])
	{
		if (!(m_pTexture = new CELBitmap))
			return false;

		CRect rcWnd;
		m_TextureWnd.GetClientRect(&rcWnd);

		if (!m_pTexture->LoadAs32BitBitmap(szFile, rcWnd.Width(), rcWnd.Height()))
		{
			g_Log.Log("CDlgSurfaces::ChangeMaskMap, failed to load texture");
			return false;
		}

		m_TextureWnd.SetBitmap(m_pTexture->GetBitmap(), false);
	}
	else
		m_TextureWnd.SetBitmap(NULL, false);

	//	Change layer's texture
	if (!pLayer->ChangeTexture(szFile))
		return false;

	return true;
}

//	Release texture
void CDlgSurfaces::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = NULL;
	}
}

//	Change specular map
bool CDlgSurfaces::ChangeSpecularMap(const char* szFile)
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return false;

	//	Release current specular map
	ReleaseSpecularMap();

	if (szFile && szFile[0])
	{
		if (!(m_pSpecuMap = new CELBitmap))
			return false;

		CRect rcWnd;
		m_SpecuMapWnd.GetClientRect(&rcWnd);

		if (!m_pSpecuMap->LoadAs32BitBitmap(szFile, rcWnd.Width(), rcWnd.Height()))
		{
			g_Log.Log("CDlgSurfaces::ChangeSpecularMap, failed to load specular map");
			return false;
		}

		m_SpecuMapWnd.SetBitmap(m_pSpecuMap->GetBitmap(), false);
	}
	else
		m_SpecuMapWnd.SetBitmap(NULL, false);

	//	Change layer's specular map
	if (!pLayer->ChangeSpecularMap(szFile))
		return false;

	return true;
}

//	Release specular map
void CDlgSurfaces::ReleaseSpecularMap()
{
	if (m_pSpecuMap)
	{
		m_pSpecuMap->Release();
		delete m_pSpecuMap;
		m_pSpecuMap = NULL;
	}
}

BOOL CDlgSurfaces::PreTranslateMessage(MSG* pMsg) 
{
	bool bBlock = false;
	bool bUpdateMask = false;
	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == m_UScaleEdit.GetSafeHwnd())
		{
			AcceptEditBoxValue(IDC_EDIT_USCALE);
			bBlock = true;
		}
		else if (pMsg->hwnd == m_VScaleEdit.GetSafeHwnd())
		{
			AcceptEditBoxValue(IDC_EDIT_VSCALE);
			bBlock = true;
		}
		else if (pMsg->hwnd == m_AltiStartEdit.GetSafeHwnd())
		{
			AcceptEditBoxValue(IDC_EDIT_ALTISTART);
			bUpdateMask = bBlock = true;
		}
		else if (pMsg->hwnd == m_AltiEndEdit.GetSafeHwnd())
		{
			AcceptEditBoxValue(IDC_EDIT_ALTIEND);
			bUpdateMask = bBlock = true;
		}
		else if (pMsg->hwnd == m_SlopeStartEdit.GetSafeHwnd())
		{
			AcceptEditBoxValue(IDC_EDIT_SLOPESTART);
			bUpdateMask = bBlock = true;
		}
		else if (pMsg->hwnd == m_SlopeEndEdit.GetSafeHwnd())
		{
			AcceptEditBoxValue(IDC_EDIT_SLOPEEND);
			bUpdateMask = bBlock = true;
		}
	}
	
	CTerrainLayer* pLayer = GetSelLayer();
	if (bUpdateMask && pLayer)
		UpdateAutoMaskMap(pLayer, 0);
	
	if (bBlock)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgSurfaces::OnOK() 
{
	UpdateData(TRUE);
	if(CheckLayerValid())
	{
		
		if (m_bModified)
			m_pTerrain->SetUpdateRenderFlag(true);
		
		CDialog::OnOK();
	}else MessageBox("层的纹理文件必须指定！请重新指定，或者删除纹理为空的层.");
}

void CDlgSurfaces::OnCancel()
{
	OnOK();
}

void CDlgSurfaces::OnDeltaposSpinUscale(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	float fScale = 1.0f / pLayer->GetScaleU() + (-pNMUpDown->iDelta) * 0.1f;
	if (fScale <= 0.0f)
		return;

	pLayer->SetScaleU(1.0f / fScale);

	CString str;
	str.Format("%.3f", fScale);
	m_UScaleEdit.SetWindowText(str);

	m_bModified = true;
}

void CDlgSurfaces::OnDeltaposSpinVscale(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	float fScale = 1.0f / pLayer->GetScaleV() + (-pNMUpDown->iDelta) * 0.1f;
	if (fScale <= 0.0f)
		return;

	pLayer->SetScaleV(1.0f / fScale);

	CString str;
	str.Format("%.3f", fScale);
	m_VScaleEdit.SetWindowText(str);

	m_bModified = true;
}

//	Accept edit box value
void CDlgSurfaces::AcceptEditBoxValue(int iEditID)
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	CString str;
	float fValue;

	switch (iEditID)
	{
	case IDC_EDIT_USCALE:

		m_UScaleEdit.GetWindowText(str);
		fValue = (float)atof(str);
		if (fValue <= 0.0f)
			fValue = 1.0f / pLayer->GetScaleU();
		else
			pLayer->SetScaleU(1.0f / fValue);

		str.Format("%.3f", fValue);
		m_UScaleEdit.SetWindowText(str);

		break;
		
	case IDC_EDIT_VSCALE:

		m_VScaleEdit.GetWindowText(str);
		fValue = (float)atof(str);
		if (fValue <= 0.0f)
			fValue = 1.0f / pLayer->GetScaleV();
		else
			pLayer->SetScaleV(1.0f / fValue);

		str.Format("%.3f", fValue);
		m_VScaleEdit.SetWindowText(str);

		break;

	case IDC_EDIT_ALTISTART:

		m_AltiStartEdit.GetWindowText(str);
		fValue = (float)atof(str);
		if (fValue < 0.0f || fValue > 1.0f)
			fValue = pLayer->GetAltiStart();
		else
			pLayer->SetAltiStart(fValue);

		str.Format("%.3f", fValue);
		m_AltiStartEdit.SetWindowText(str);

		break;

	case IDC_EDIT_ALTIEND:

		m_AltiEndEdit.GetWindowText(str);
		fValue = (float)atof(str);
		if (fValue < 0.0f || fValue > 1.0f)
			fValue = pLayer->GetAltiEnd();
		else
			pLayer->SetAltiEnd(fValue);

		str.Format("%.3f", fValue);
		m_AltiEndEdit.SetWindowText(str);

		break;

	case IDC_EDIT_SLOPESTART:
		
		m_SlopeStartEdit.GetWindowText(str);
		fValue = (float)atof(str);
		if (fValue < 0.0f || fValue > 1.0f)
			fValue = pLayer->GetSlopeStart();
		else
			pLayer->SetSlopeStart(fValue);

		str.Format("%.3f", fValue);
		m_SlopeStartEdit.SetWindowText(str);

		break;

	case IDC_EDIT_SLOPEEND:

		m_SlopeEndEdit.GetWindowText(str);
		fValue = (float)atof(str);
		if (fValue < 0.0f || fValue > 1.0f)
			fValue = pLayer->GetSlopeEnd();
		else
			pLayer->SetSlopeEnd(fValue);

		str.Format("%.3f", fValue);
		m_SlopeEndEdit.SetWindowText(str);

		break;
	}

	m_bModified = true;
}

void CDlgSurfaces::OnSelchangeComboProjaxis() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	pLayer->SetProjAxis(m_ProjAxisCombo.GetCurSel());

	m_bModified = true;
}

void CDlgSurfaces::OnSelchangeComboWeight() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer)
		return;

	pLayer->SetLayerWeight(m_WeightCombo.GetCurSel());

	m_bModified = true;
}

//	Set mask view title
void CDlgSurfaces::SetMaskViewTitle()
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer || !pLayer->GetMaskBitmap())
	{
		GetDlgItem(IDC_STATIC_MASKTITLE)->SetWindowText("掩膜");
		return;
	}

	int iSize = pLayer->GetMaskBitmap()->GetWidth();
	CString str;
	str.Format("掩膜（%d x %d）", iSize, iSize);
	GetDlgItem(IDC_STATIC_MASKTITLE)->SetWindowText(str);
}

//	Export mask to file
void CDlgSurfaces::OnBtnExportmask() 
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer || !pLayer->GetMaskBitmap())
		return;

	char szFilter[] = "Text Files (*.bmp)|*.bmp||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.bmp", NULL, dwFlags, szFilter, NULL);
		
	if (FileDia.DoModal() == IDCANCEL)
		return;

	if (!pLayer->GetMaskBitmap()->SaveToFile(FileDia.GetPathName()))
		MessageBox("导出失败！");
}

void CDlgSurfaces::OnCheckEnable() 
{
	m_bEnable = !m_bEnable;

	CTerrainLayer* pLayer = GetSelLayer();
	if (pLayer)
		pLayer->Enable(m_bEnable ? true : false);
}

//	Update auto mask map
void CDlgSurfaces::UpdateAutoMaskMap(CTerrainLayer* pLayer, int iNewSize)
{
	ASSERT(pLayer);
	if (!iNewSize)
		iNewSize = pLayer->GetAutoMaskMapSize();

	pLayer->UpdateAutoMaskMap(iNewSize);
	if (pLayer->GetMaskBitmap())
		m_MaskWnd.SetBitmap(pLayer->GetMaskBitmap()->GetBitmap(), false);
	else
		m_MaskWnd.SetBitmap(NULL, false);
}

void CDlgSurfaces::OnCheckMaskaxisX() 
{
	OnCheckAutoMaskAxis(0, m_bMaskAxisX);
	m_bModified = true;
}

void CDlgSurfaces::OnCheckMaskaxisY() 
{
	OnCheckAutoMaskAxis(1, m_bMaskAxisY);
	m_bModified = true;
}

void CDlgSurfaces::OnCheckMaskaxisZ() 
{
	OnCheckAutoMaskAxis(2, m_bMaskAxisZ);
	m_bModified = true;
}

//	On check auto mask axis
void CDlgSurfaces::OnCheckAutoMaskAxis(int iAxis, BOOL& bValue)
{
	CTerrainLayer* pLayer = GetSelLayer();
	if (!pLayer || !pLayer->GetMaskBitmap())
		return;

	bValue = !bValue;
	DWORD dwFlags = pLayer->GetAutoMaskAxisFlag();

	if (bValue)
	{
		switch (iAxis)
		{
		case 0:	dwFlags |= AXIS_X;	break;
		case 1: dwFlags |= AXIS_Y;	break;
		case 2: dwFlags |= AXIS_Z;	break;
		}
	}
	else
	{
		switch (iAxis)
		{
		case 0:	dwFlags &= ~AXIS_X;	break;
		case 1: dwFlags &= ~AXIS_Y;	break;
		case 2: dwFlags &= ~AXIS_Z;	break;
		}
	}

	pLayer->SetAutoMaskAxisFlag(dwFlags);
	UpdateAutoMaskMap(pLayer, 0);
}

//	Export layers ...
void CDlgSurfaces::OnSufFileSave() 
{
	char szFilter[] = "Element Editor Layers Files (*.lay)|*.lay||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.lay", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	FILE* fp = fopen(FileDia.GetPathName(), "wt");
	if (!fp)
	{
		MessageBox("创建文件失败!");
		return;
	}

	int i, iNumLayer = m_LayerList.GetCount();
	int iVersion = LAYERFILEVERSION;

	fprintf(fp, "// Element Editor Terrain Layers description file.\n\n");
	fprintf(fp, "version %d", iVersion);

	for (i=0; i < iNumLayer; i++)
	{
		CTerrainLayer* pLayer = (CTerrainLayer*)m_LayerList.GetItemData(i);

		//	Layer name
		fprintf(fp, "layer_%d %s\n{\n", i, pLayer->GetName());

		//	Layer parameters
		fprintf(fp, "\t%d %d %f %f %d\n", pLayer->IsEnable() ? 1 : 0, pLayer->GetLayerWeight(), 
				pLayer->GetScaleU(), pLayer->GetScaleV(), pLayer->GetProjAxis());

		//	Normal texture file name
		if (pLayer->HasTexture())
			fprintf(fp, "\t\"%s\"\n", pLayer->GetTextureFile());
		else
			fprintf(fp, "\tnull\n");

		//	Specular texture file name
		if (pLayer->HasSpecularMap())
			fprintf(fp, "\t\"%s\"\n", pLayer->GetSpecularMapFile());
		else
			fprintf(fp, "\tnull\n");

		//	Auto-mask parameters
		DWORD dwFlags = pLayer->GetAutoMaskAxisFlag();
		fprintf(fp, "\t%d %d %d %d %d\n", pLayer->GetAutoMaskSmoothFlag() ? 1 : 0, pLayer->GetAutoMaskMapSize(),
			(dwFlags & AXIS_X) ? 1 : 0, (dwFlags & AXIS_Y) ? 1 : 0, (dwFlags & AXIS_Z) ? 1 : 0);
		fprintf(fp, "\t%f %f %f %f\n", pLayer->GetAltiStart(), pLayer->GetAltiEnd(), pLayer->GetSlopeStart(), pLayer->GetSlopeEnd());

		//	}
		fprintf(fp, "}\n\n");
	}

	fclose(fp);
}

//	Import layers ...
void CDlgSurfaces::OnSufFileLoad()
{
	//	Let user select a text file
	char szFilter[] = "Element Editor Layers Files (*.lay)|*.lay||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.lay", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;

	AScriptFile File;
	if (!File.Open(FileDia.GetPathName()))
	{
		MessageBox("读取文件失败!");
		return;
	}

	int iVersion = 1;
	File.PeekNextToken(true);
	if (!stricmp(File.m_szToken, "version"))
	{
		File.GetNextToken(true);
		iVersion = File.GetNextTokenAsInt(false);
	}

	int iCount=0, iCurNumLayer=m_LayerList.GetCount();
	AString strName;

	while (File.GetNextToken(true))
	{
		//	Layer name
		File.GetNextToken(false);
		strName = File.m_szToken;

		if (!File.MatchToken("{", true))
			break;

		CTerrainLayer* pLayer;

		if (iCount < iCurNumLayer)
		{
			pLayer = (CTerrainLayer*)m_LayerList.GetItemData(iCount);
			pLayer->SetName(strName);
		}
		else
		{
			//	Create a new layer
			if (!(pLayer = m_pTerrain->CreateLayer(strName, false)))
			{
				g_Log.Log("CDlgSurfaces::OnSufFileLoad, Failed to create layer");
				break;
			}

			int n = m_LayerList.InsertString(-1, strName);
			m_LayerList.SetItemData(n, (DWORD)pLayer);
		}

		//	Layer parameters
		pLayer->Enable(File.GetNextTokenAsInt(true) ? true : false);

		if (iVersion >= 2)
			pLayer->SetLayerWeight(File.GetNextTokenAsInt(false));

		pLayer->SetScaleU(File.GetNextTokenAsFloat(false));
		pLayer->SetScaleV(File.GetNextTokenAsFloat(false));
		pLayer->SetProjAxis(File.GetNextTokenAsInt(false));

		//	Normal texture
		File.GetNextToken(true);
		if (stricmp(File.m_szToken, "null"))
			pLayer->ChangeTexture(File.m_szToken);
		else
			pLayer->ChangeTexture(NULL);

		//	Specular map
		File.GetNextToken(true);
		if (stricmp(File.m_szToken, "null"))
			pLayer->ChangeSpecularMap(File.m_szToken);
		else
			pLayer->ChangeSpecularMap(NULL);

		//	Auto-mask parameters
		pLayer->SetAutoMaskSmoothFlag(File.GetNextTokenAsInt(true) ? true : false);
		pLayer->SetAutoMaskMapSize(File.GetNextTokenAsInt(false));
		
		DWORD dwFlags = 0;
		dwFlags |= File.GetNextTokenAsInt(false) ? AXIS_X : 0;
		dwFlags |= File.GetNextTokenAsInt(false) ? AXIS_Y : 0;
		dwFlags |= File.GetNextTokenAsInt(false) ? AXIS_Z : 0;
		pLayer->SetAutoMaskAxisFlag(dwFlags);

		pLayer->SetAltiStart(File.GetNextTokenAsFloat(true));
		pLayer->SetAltiEnd(File.GetNextTokenAsFloat(false));
		pLayer->SetSlopeStart(File.GetNextTokenAsFloat(false));
		pLayer->SetSlopeEnd(File.GetNextTokenAsFloat(false));

		iCount++;

		if (!File.MatchToken("}", true))
			break;
	}

	File.Close();

	if (iCount < iCurNumLayer)
	{
		//	Remove rest layers
		for (int i=iCount; i < iCurNumLayer; i++)
		{
			m_pTerrain->ReleaseLayer(i);
			m_LayerList.DeleteString(i);
		}
	}

	//	Select layer 0
	m_LayerList.SetCurSel(0);
	ChangeLayer(0);

	m_bModified = true;
}



void CDlgSurfaces::OnKillfocusEditAltiend() 
{
	// TODO: Add your control notification handler code here
	AcceptEditBoxValue(IDC_EDIT_ALTIEND);
}

void CDlgSurfaces::OnKillfocusEditAltistart() 
{
	// TODO: Add your control notification handler code here
	AcceptEditBoxValue(IDC_EDIT_ALTISTART);
}

void CDlgSurfaces::OnKillfocusEditSlopeend() 
{
	// TODO: Add your control notification handler code here
	AcceptEditBoxValue(IDC_EDIT_SLOPEEND);
}

void CDlgSurfaces::OnKillfocusEditSlopestart() 
{
	// TODO: Add your control notification handler code here
	AcceptEditBoxValue(IDC_EDIT_SLOPESTART);
}

void CDlgSurfaces::OnKillfocusEditUscale() 
{
	// TODO: Add your control notification handler code here
	AcceptEditBoxValue(IDC_EDIT_USCALE);
}

void CDlgSurfaces::OnKillfocusEditVscale() 
{
	// TODO: Add your control notification handler code here
	AcceptEditBoxValue(IDC_EDIT_VSCALE);
}
