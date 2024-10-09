// DlgLighting.cpp : implementation file
//

#include "Global.h"
#include "DlgLighting.h"
#include "ElementMap.h"
#include "A3DMacros.h"
#include "AAssist.h"
#include "AMemory.h"
#include "MainFrm.h"
#include "SceneBoxArea.h"
#include "SceneObjectManager.h"

//#define new A_DEBUG_NEW

#define MAX_SUNDIR		3600
#define MAX_SUNHEIGHT	900

static const int s_LightMapSize = 64;

//	Convert sun dir and height to a vector
static A3DVECTOR3 _MakeSunDirction(int iSunDir, int iHeight)
{
	A3DVECTOR3 vDir;

	//	Cacluate sun direction
	float fRad = DEG2RAD(iSunDir / 10.0f);
	float fPitch = DEG2RAD(iHeight / 10.0f);
	vDir.y = -(float)sin(fPitch);

	float fProj = (float)cos(fPitch);
	vDir.x = fProj * (float)cos(fRad);
	vDir.z = fProj * (float)sin(fRad);
	vDir.Normalize();

	return vDir;
}

//	Convert a vector to sun dir and heihgt
static void _ParseSunDirection(const A3DVECTOR3& vDir, int* piSunDir, int* piSunHei)
{
	//	Sun direction slider
	A3DVECTOR3 vProj(vDir.x, 0.0f, vDir.z);
	vProj.Normalize();
	float fTemp = (float)RAD2DEG(acos(vProj.x));
	if (vProj.z < 0.0f)
		fTemp = 360.0f - fTemp;

	*piSunDir = (int)(fTemp * 10.0f + 0.5f);

	//	Sun height
	fTemp = (float)RAD2DEG(asin(-vDir.y));
	*piSunHei = (int)(fTemp * 10.0f + 0.5f);
}

/////////////////////////////////////////////////////////////////////////////
// CLightingView window

CLightingView::CLightingView()
{
	m_iSunDir		= 0;
	m_iSunHei		= 50;
	m_dwSunColor	= 0;
	m_dwAmbient		= 0;
	m_bLeftPress	= false;
}

CLightingView::~CLightingView()
{
}

BEGIN_MESSAGE_MAP(CLightingView, CBackBufWnd)
	//{{AFX_MSG_MAP(CLightingView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create window
bool CLightingView::Create(const RECT& rc, CWnd* pParent)
{
	//	Register class name
	CString strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
							LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)),
							NULL);//(HBRUSH)GetStockObject(WHITE_BRUSH));
	
	if (!CBackBufWnd::Create(strClassName, "LightingView", 
						WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						rc, pParent, 0))
		return false;

	return true;
}

//	Set sun direction
void CLightingView::SetSunDirAndHeight(int iSunDir, int iHeight)
{
	m_iSunDir	= iSunDir;
	m_iSunHei	= iHeight;
}

//	Draw back buffer
bool CLightingView::DrawBackBuffer(RECT* lprc)
{
	A3DVECTOR3 vDir = _MakeSunDirction(m_iSunDir, m_iSunHei);

	//	Clear background with sun color and ambient
	A3DCOLORVALUE col = m_dwSunColor;
	col *= -vDir.y;
	col += m_dwAmbient;
	col.Clamp();

	a_Swap(col.r, col.b);
	DWORD dwCol = col.ToRGBAColor();
	ClearBackBuffer(dwCol);

	//	Draw sun line
	RECT rcClient;
	GetClientRect(&rcClient);

	//	Inverse the color so that we can always see it clearly.
	//	The color used as COLORREF should have 0 alpha channel
	dwCol  = ~dwCol;
	dwCol &= 0x00ffffff;

	HPEN hOldPen, hPen;
	hPen = ::CreatePen(PS_SOLID, 1, dwCol);
	hOldPen = (HPEN)::SelectObject(m_hMemDC, hPen);

	int cx = rcClient.right / 2;
	int cy = rcClient.bottom / 2;
	int iLen = rcClient.right / 2 - 5;
	int dx = cx - (int)(iLen * vDir.x);
	int dy = cy + (int)(iLen * vDir.z);

	MoveToEx(m_hMemDC, cx, cy, NULL);
	LineTo(m_hMemDC, dx, dy);

	Ellipse(m_hMemDC, dx-4, dy-4, dx+5, dy+5);

	::SelectObject(m_hMemDC, hOldPen);
	::DeleteObject(hPen);

	return true;
}

void CLightingView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawBackBuffer(NULL);
	Flip(dc.m_hDC, NULL);
	
	// Do not call CBackBufWnd::OnPaint() for painting messages
}

void CLightingView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLeftPress = true;
	SetCapture();
	DragSunDir(point.x, point.y);

	CBackBufWnd::OnLButtonDown(nFlags, point);
}

void CLightingView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bLeftPress)
	{
		ReleaseCapture();
		m_bLeftPress = false;
	}

	CBackBufWnd::OnLButtonUp(nFlags, point);
}

void CLightingView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bLeftPress)
		DragSunDir(point.x, point.y);

	CBackBufWnd::OnMouseMove(nFlags, point);
}

//	Drag sun direction to specified position
void CLightingView::DragSunDir(int x, int y)
{
	RECT rcClient;
	GetClientRect(&rcClient);

	A3DVECTOR3 vNewDir;
	vNewDir.x = rcClient.right * 0.5f - x;
	vNewDir.z = y - rcClient.bottom * 0.5f;
	vNewDir.y = 0.0f;

	int iSunHei;
	_ParseSunDirection(vNewDir, &m_iSunDir, &iSunHei);

	InvalidateRect(NULL);

	//	Notify parent dialog
	CDlgLighting* pParent = (CDlgLighting*)GetParent();
	pParent->SetSunDir(m_iSunDir);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgLighting dialog

CDlgLighting::CDlgLighting(const INITDATA& InitData, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLighting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLighting)
	m_fShadowFaction = 1.0f;
	//}}AFX_DATA_INIT

	m_Params.dwAmbient	= InitData.dwAmbient;
	m_Params.dwSunCol	= InitData.dwSunCol;
	m_Params.iMapSize	= InitData.iMapSize;
	m_Params.fSunPower =  InitData.fSunPower;

	m_NightParams.dwAmbient	= InitData.dwNightAmbient;
	m_NightParams.dwSunCol	= InitData.dwNightSunCol;
	m_NightParams.iMapSize	= InitData.iNightMapSize;
	m_NightParams.fSunPower = InitData.fNightSunPower;

	if (InitData.bVecDir)
	{
		m_Params.vSunDir = InitData.vSunDir;
		m_NightParams.vSunDir = InitData.vNightSunDir;
	}
	else
	{
		m_Params.vSunDir = _MakeSunDirction(InitData.iSunDir, InitData.iSunHei);
		m_NightParams.vSunDir = _MakeSunDirction(InitData.iSunDir, InitData.iSunHei);
	}

	m_SunColWnd.SetNotifyWindow(this);
	m_AmbientWnd.SetNotifyWindow(this);

	m_bInited = false;
	m_bOnlyDir = false;
}

void CDlgLighting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLighting)
	DDX_Control(pDX, IDC_EDIT_SUNPOWER, m_editSunPower);
	DDX_Control(pDX, IDC_EDIT_SUNHEIPOS, m_editSunHeight);
	DDX_Control(pDX, IDC_EDIT_SUNDIRPOS, m_editSunDir);
	DDX_Control(pDX, IDC_EDIT_SUNCOLOR, m_editSunColor);
	DDX_Control(pDX, IDC_EDIT_AMBIENT, m_editAmbient);
	DDX_Control(pDX, IDC_EDIT_SUNDIRZ, m_editSunDirZ);
	DDX_Control(pDX, IDC_EDIT_SUNDIRY, m_editSunDirY);
	DDX_Control(pDX, IDC_EDIT_SUNDIRX, m_editSunDirX);
	DDX_Control(pDX, IDC_STATIC_SUNCOLOR, m_SunColWnd);
	DDX_Control(pDX, IDC_STATIC_AMBIENT, m_AmbientWnd);
	DDX_Control(pDX, IDC_COMBO_MAPSIZE, m_MapSizeCombo);
	DDX_Control(pDX, IDC_SLIDER_SUNHEIGHT, m_SunHeiSlider);
	DDX_Control(pDX, IDC_SLIDER_SUNDIR, m_SunDirSlider);
	DDX_Text(pDX, IDC_EDIT_SUNPOWER2, m_fShadowFaction);
	DDV_MinMaxFloat(pDX, m_fShadowFaction, 1.f, 2.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLighting, CDialog)
	//{{AFX_MSG_MAP(CDlgLighting)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SUNDIR, OnReleasedcaptureSliderSundir)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SUNHEIGHT, OnReleasedcaptureSliderSunheight)
	ON_EN_CHANGE(IDC_EDIT_SUNDIRPOS, OnChangeEditSundirpos)
	ON_EN_CHANGE(IDC_EDIT_SUNHEIPOS, OnChangeEditSunheipos)
	ON_EN_CHANGE(IDC_EDIT_SUNPOWER, OnChangeEditSunpower)
	ON_BN_CLICKED(IDC_RADIO_DAY, OnRadioDay)
	ON_BN_CLICKED(IDC_RADIO_NIGHT, OnRadioNight)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_SELCOLCHANGED, OnColorChange)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLighting message handlers

BOOL CDlgLighting::OnInitDialog() 
{
	CDialog::OnInitDialog();

	AUX_FlatEditBox(&m_editSunDir);
	AUX_FlatEditBox(&m_editSunHeight);
	AUX_FlatEditBox(&m_editSunPower);
	//	Create lighting view window
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_LIGHTINGVIEW)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	rcWnd.right		= rcWnd.left + 239;
	rcWnd.bottom	= rcWnd.top + 239;
	if (!m_LightingView.Create(rcWnd, this))
	{
		g_Log.Log("CDlgLighting::OnInitDialog, Failed to create lighting view window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	m_bIsDay = true;
	CButton *pButton = (CButton*) GetDlgItem(IDC_RADIO_DAY);
	if(pButton) pButton->SetCheck(m_bIsDay);
	pButton = (CButton*) GetDlgItem(IDC_RADIO_NIGHT);
	if(pButton) pButton->SetCheck(!m_bIsDay);
	
	//	Initalize map size combo
	m_MapSizeCombo.InsertString(-1, "64 x 64");
	m_MapSizeCombo.InsertString(-1, "128 x 128");
	m_MapSizeCombo.InsertString(-1, "256 x 256");
	m_MapSizeCombo.InsertString(-1, "512 x 512");
	m_MapSizeCombo.InsertString(-1, "1024 x 1024");
	m_MapSizeCombo.InsertString(-1, "2048 x 2048");
	m_MapSizeCombo.InsertString(-1, "4096 x 4096");
	m_MapSizeCombo.SetCurSel(SIZE_1024);

	int iMapSize = s_LightMapSize;//256;
	for (int i=0; i < NUM_SIZE; i++)
	{
		if (iMapSize == m_Params.iMapSize)
		{
			m_MapSizeCombo.SetCurSel(i);
			break;
		}

		iMapSize <<= 1;
	}

	//	Sun direction slider
	int iSunDir, iHeight;
	_ParseSunDirection(m_Params.vSunDir, &iSunDir, &iHeight);

	CString str;
	str.Format("%f",m_Params.vSunDir.x);
	m_editSunDirX.SetWindowText(str);
	str.Format("%f",m_Params.vSunDir.y);
	m_editSunDirY.SetWindowText(str);
	str.Format("%f",m_Params.vSunDir.z);
	m_editSunDirZ.SetWindowText(str);
	
	m_SunDirSlider.SetRange(0, MAX_SUNDIR);
	m_SunDirSlider.SetPos(iSunDir);
	m_SunHeiSlider.SetRange(0, MAX_SUNHEIGHT);
	m_SunHeiSlider.SetPos(iHeight);
	UpdateSunDir(iSunDir);
	UpdateSunHei(iHeight);

	m_LightingView.SetSunDirAndHeight(iSunDir, iHeight);
	m_LightingView.SetSunColor(m_Params.dwSunCol);
	m_LightingView.SetAmbient(m_Params.dwAmbient);

	//	Initialize ambient color window
	BYTE r, g, b;
	r = A3DCOLOR_GETRED(m_Params.dwAmbient);
	g = A3DCOLOR_GETGREEN(m_Params.dwAmbient);
	b = A3DCOLOR_GETBLUE(m_Params.dwAmbient);
	m_AmbientWnd.SetColor(RGB(r, g, b));
	m_AmbientWnd.Redraw();
	m_AmbientWnd.EnableWindow(TRUE);

	//	Initialize sun color window
	r = A3DCOLOR_GETRED(m_Params.dwSunCol);
	g = A3DCOLOR_GETGREEN(m_Params.dwSunCol);
	b = A3DCOLOR_GETBLUE(m_Params.dwSunCol);

	str.Format("%x",m_Params.dwAmbient);
	m_editAmbient.SetWindowText(str);
	str.Format("%x",m_Params.dwSunCol);
	m_editSunColor.SetWindowText(str);
	str.Format("%4.2f",m_Params.fSunPower);
	m_editSunPower.SetWindowText(str);

	m_SunColWnd.SetColor(RGB(r, g, b));
	m_SunColWnd.Redraw();
	m_SunColWnd.EnableWindow(TRUE);

	m_fShadowFaction = g_Configs.fSunFaction;
	UpdateData(FALSE);
	m_bInited = true;

	DWORD item[9] = { IDC_RADIO_DAY, IDC_RADIO_NIGHT,IDC_COMBO_MAPSIZE,IDC_STATIC_AMBIENT,IDC_EDIT_AMBIENT,IDC_STATIC_SUNCOLOR,IDC_EDIT_SUNCOLOR,IDC_EDIT_SUNPOWER,IDC_EDIT_SUNPOWER2};
	if(m_bOnlyDir)
	{
		CWnd *pWnd = NULL;
		for( int i = 0; i < 9; i++)
		{
			pWnd = GetDlgItem(item[i]);
			pWnd->EnableWindow(false);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLighting::UpdateControl()
{
	int iMapSize = s_LightMapSize;//256;
	for (int i=0; i < NUM_SIZE; i++)
	{
		int curMapSize;
		if(m_bIsDay) curMapSize = m_Params.iMapSize;
		else curMapSize = m_NightParams.iMapSize;
		if (iMapSize == curMapSize)
		{
			m_MapSizeCombo.SetCurSel(i);
			break;
		}

		iMapSize <<= 1;
	}

	//	Sun direction slider
	int iSunDir, iHeight;
	if(m_bIsDay) _ParseSunDirection(m_Params.vSunDir, &iSunDir, &iHeight);
	else _ParseSunDirection(m_NightParams.vSunDir, &iSunDir, &iHeight);
	
	CString str;
	if(m_bIsDay)
	{
		str.Format("%f",m_Params.vSunDir.x);
		m_editSunDirX.SetWindowText(str);
		str.Format("%f",m_Params.vSunDir.y);
		m_editSunDirY.SetWindowText(str);
		str.Format("%f",m_Params.vSunDir.z);
		m_editSunDirZ.SetWindowText(str);
	}else
	{
		str.Format("%f",m_NightParams.vSunDir.x);
		m_editSunDirX.SetWindowText(str);
		str.Format("%f",m_NightParams.vSunDir.y);
		m_editSunDirY.SetWindowText(str);
		str.Format("%f",m_NightParams.vSunDir.z);
		m_editSunDirZ.SetWindowText(str);
	}

	
	m_SunDirSlider.SetRange(0, MAX_SUNDIR);
	m_SunDirSlider.SetPos(iSunDir);
	m_SunHeiSlider.SetRange(0, MAX_SUNHEIGHT);
	m_SunHeiSlider.SetPos(iHeight);
	UpdateSunDir(iSunDir);
	UpdateSunHei(iHeight);

	m_LightingView.SetSunDirAndHeight(iSunDir, iHeight);
	if(m_bIsDay)
	{
		m_LightingView.SetSunColor(m_Params.dwSunCol);
		m_LightingView.SetAmbient(m_Params.dwAmbient);
	}else
	{
		m_LightingView.SetSunColor(m_NightParams.dwSunCol);
		m_LightingView.SetAmbient(m_NightParams.dwAmbient);
	}

	//	Initialize ambient color window
	BYTE r, g, b;
	if(m_bIsDay)
	{
		r = A3DCOLOR_GETRED(m_Params.dwAmbient);
		g = A3DCOLOR_GETGREEN(m_Params.dwAmbient);
		b = A3DCOLOR_GETBLUE(m_Params.dwAmbient);
	}else
	{
		r = A3DCOLOR_GETRED(m_NightParams.dwAmbient);
		g = A3DCOLOR_GETGREEN(m_NightParams.dwAmbient);
		b = A3DCOLOR_GETBLUE(m_NightParams.dwAmbient);
	}
	m_AmbientWnd.SetColor(RGB(r, g, b));
	m_AmbientWnd.Redraw();
	m_AmbientWnd.EnableWindow(TRUE);

	//	Initialize sun color window
	if(m_bIsDay)
	{
		
		str.Format("%x",m_Params.dwAmbient);
		m_editAmbient.SetWindowText(str);
		str.Format("%x",m_Params.dwSunCol);
		m_editSunColor.SetWindowText(str);
		str.Format("%4.2f",m_Params.fSunPower);
		m_editSunPower.SetWindowText(str);
		
		r = A3DCOLOR_GETRED(m_Params.dwSunCol);
		g = A3DCOLOR_GETGREEN(m_Params.dwSunCol);
		b = A3DCOLOR_GETBLUE(m_Params.dwSunCol);
	}else
	{
		str.Format("%x",m_NightParams.dwAmbient);
		m_editAmbient.SetWindowText(str);
		str.Format("%x",m_NightParams.dwSunCol);
		m_editSunColor.SetWindowText(str);
		str.Format("%4.2f",m_NightParams.fSunPower);
		m_editSunPower.SetWindowText(str);

		r = A3DCOLOR_GETRED(m_NightParams.dwSunCol);
		g = A3DCOLOR_GETGREEN(m_NightParams.dwSunCol);
		b = A3DCOLOR_GETBLUE(m_NightParams.dwSunCol);
	}

	

	m_SunColWnd.SetColor(RGB(r, g, b));
	m_SunColWnd.Redraw();
	m_SunColWnd.EnableWindow(TRUE);

	UpdateData(FALSE);
}

void CDlgLighting::SaveItemData()
{
	if (!UpdateData(TRUE))
		return;

	if(m_bIsDay)
	{
		iSunDir		= m_SunDirSlider.GetPos();
		iSunHeight	= m_SunHeiSlider.GetPos();
		iNightSunDir = m_SunDirSlider.GetPos();
		iNightSunHeight	= m_SunHeiSlider.GetPos();
		//	Cacluate sun direction
		m_NightParams.vSunDir = _MakeSunDirction(iNightSunDir, iNightSunHeight);
		//	Cacluate sun direction
		m_Params.vSunDir = _MakeSunDirction(iSunDir, iSunHeight);
		
		int iSize = m_MapSizeCombo.GetCurSel();
		m_Params.iMapSize = s_LightMapSize << iSize;
		
		iLightMapSize	= m_Params.iMapSize;
		dwSunCol		= m_Params.dwSunCol;
		dwAmbient		= m_Params.dwAmbient;
	}else
	{
		iSunDir		= m_SunDirSlider.GetPos();
		iSunHeight	= m_SunHeiSlider.GetPos();
		iNightSunDir = m_SunDirSlider.GetPos();
		iNightSunHeight	= m_SunHeiSlider.GetPos();
		//	Cacluate sun direction
		m_NightParams.vSunDir = _MakeSunDirction(iNightSunDir, iNightSunHeight);
		//	Cacluate sun direction
		m_Params.vSunDir = _MakeSunDirction(iSunDir, iSunHeight);
		
		int iSize = m_MapSizeCombo.GetCurSel();
		m_NightParams.iMapSize = s_LightMapSize << iSize;
		
		iNightLightMapSize	= m_NightParams.iMapSize;
		dwNightSunCol		= m_NightParams.dwSunCol;
		dwNightAmbient		= m_NightParams.dwAmbient;		
	}
}

void CDlgLighting::OnOK() 
{
	SaveItemData();
	UpdateData(true);
	g_Configs.iSunDir		= iSunDir;
	g_Configs.iSunHeight	= iSunHeight;
	//	Cacluate sun direction
	m_Params.vSunDir = _MakeSunDirction(g_Configs.iSunDir, g_Configs.iSunHeight);
	if(m_bOnlyDir)
	{
		CDialog::OnOK();
		return;
	}
	g_Configs.iLightMapSize	= m_Params.iMapSize;
	g_Configs.dwSunCol		= m_Params.dwSunCol;
	g_Configs.dwAmbient		= m_Params.dwAmbient;
	
	g_Configs.iNightSunDir		= iNightSunDir;
	g_Configs.iNightSunHeight	= iNightSunHeight;
	//	Cacluate sun direction
	m_NightParams.vSunDir = _MakeSunDirction(g_Configs.iNightSunDir, g_Configs.iNightSunHeight);
	
	g_Configs.iNightLightMapSize	= m_NightParams.iMapSize;
	g_Configs.dwNightSunCol		    = m_NightParams.dwSunCol;
	g_Configs.dwNightAmbient		= m_NightParams.dwAmbient;	
	g_Configs.fSunFaction           = m_fShadowFaction;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) 
	{
		CDialog::OnOK();
		return;
	}
	CSceneObjectManager *pSceneObjectMan = pMap->GetSceneObjectMan();
	if(IDYES==AfxMessageBox("你想把本地图中所有的区域的灯光都设置成这样吗？",MB_YESNO))
	{
		ALISTPOSITION pos = pSceneObjectMan->m_listSceneObject.GetHeadPosition();
		while( pos )
		{
			CSceneObject *pObj = pSceneObjectMan->m_listSceneObject.GetNext(pos);
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_AREA)
			{
				CSceneBoxArea *pArea = (CSceneBoxArea *)pObj;
				BOX_AREA_PROPERTY data;
				pArea->GetProperty(data);
				data.m_dwAmbient		= m_Params.dwAmbient;
				data.m_dwDirLightClr	= m_Params.dwSunCol; 
				data.m_fSunPower		= m_Params.fSunPower;
				data.m_nMapSize			= m_Params.iMapSize;
				data.m_vLightDir		= m_Params.vSunDir; 
				
				data.m_dwAmbientNight		= m_NightParams.dwAmbient;
				data.m_dwDirLightClrNight	= m_NightParams.dwSunCol; 
				data.m_fSunPowerNight		= m_NightParams.fSunPower;
				data.m_nMapSizeNight		= m_NightParams.iMapSize;
				data.m_vLightDirNight		= m_NightParams.vSunDir; 
				pArea->SetProperty(data);
				pArea->UpdateProperty(false);
			}
		}
	}else
	{
		CSceneBoxArea *pArea = pSceneObjectMan->GetDefaultBoxArea();
		if(pArea)
		{
			BOX_AREA_PROPERTY data;
			pArea->GetProperty(data);
			data.m_dwAmbient		= m_Params.dwAmbient;
			data.m_dwDirLightClr	= m_Params.dwSunCol; 
			data.m_fSunPower		= m_Params.fSunPower;
			data.m_nMapSize			= m_Params.iMapSize;
			data.m_vLightDir		= m_Params.vSunDir; 

			data.m_dwAmbientNight		= m_NightParams.dwAmbient;
			data.m_dwDirLightClrNight	= m_NightParams.dwSunCol; 
			data.m_fSunPowerNight		= m_NightParams.fSunPower;
			data.m_nMapSizeNight		= m_NightParams.iMapSize;
			data.m_vLightDirNight		= m_NightParams.vSunDir; 
			pArea->SetProperty(data);
			pArea->UpdateProperty(false);
		}
	}
	AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(true);
	CDialog::OnOK();
}

void CDlgLighting::OnReleasedcaptureSliderSundir(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	
	int iSunDir = m_SunDirSlider.GetPos();
	int iSunHei = m_SunHeiSlider.GetPos();
	m_LightingView.SetSunDirAndHeight(iSunDir, iSunHei);
	m_LightingView.InvalidateRect(NULL);
	UpdateSunDir(iSunDir);
	A3DVECTOR3 vSunDir = _MakeSunDirction(iSunDir, iSunHei);
	CString str;
	str.Format("%f",vSunDir.x);
	m_editSunDirX.SetWindowText(str);
	str.Format("%f",vSunDir.y);
	m_editSunDirY.SetWindowText(str);
	str.Format("%f",vSunDir.z);
	m_editSunDirZ.SetWindowText(str);
	UpdateData(false);
}

void CDlgLighting::OnReleasedcaptureSliderSunheight(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	int iSunDir = m_SunDirSlider.GetPos();
	int iSunHei = m_SunHeiSlider.GetPos();
	m_LightingView.SetSunDirAndHeight(iSunDir, iSunHei);
	m_LightingView.InvalidateRect(NULL);

	UpdateSunHei(iSunHei);
	A3DVECTOR3 vSunDir = _MakeSunDirction(iSunDir, iSunHei);
	CString str;
	str.Format("%f",vSunDir.x);
	m_editSunDirX.SetWindowText(str);
	str.Format("%f",vSunDir.y);
	m_editSunDirY.SetWindowText(str);
	str.Format("%f",vSunDir.z);
	m_editSunDirZ.SetWindowText(str);
	UpdateData(false);
}

//	Set sun direction
void CDlgLighting::SetSunDir(int iSunDir)
{
	a_Clamp(iSunDir, 0, MAX_SUNDIR);
	m_SunDirSlider.SetPos(iSunDir);
	UpdateSunDir(iSunDir);
}

//	Update sun direction position
void CDlgLighting::UpdateSunDir(int iSunDir)
{
	char szText[10];
	itoa(iSunDir, szText, 10);
	m_editSunDir.SetWindowText(szText);

	int iSunHei = m_SunHeiSlider.GetPos();
	A3DVECTOR3 vSunDir = _MakeSunDirction(iSunDir, iSunHei);
	CString str;
	str.Format("%f",vSunDir.x);
	m_editSunDirX.SetWindowText(str);
	str.Format("%f",vSunDir.y);
	m_editSunDirY.SetWindowText(str);
	str.Format("%f",vSunDir.z);
	m_editSunDirZ.SetWindowText(str);
	UpdateData(false);
}

//	Update sun height position
void CDlgLighting::UpdateSunHei(int iSunHei)
{
	char szText[10];
	itoa(iSunHei, szText, 10);
	m_editSunHeight.SetWindowText(szText);

	int iSunDir = m_SunDirSlider.GetPos();
	A3DVECTOR3 vSunDir = _MakeSunDirction(iSunDir, iSunHei);
	CString str;
	str.Format("%f",vSunDir.x);
	m_editSunDirX.SetWindowText(str);
	str.Format("%f",vSunDir.y);
	m_editSunDirY.SetWindowText(str);
	str.Format("%f",vSunDir.z);
	m_editSunDirZ.SetWindowText(str);
	UpdateData(false);
}

HRESULT CDlgLighting::OnColorChange(WPARAM wParam, LPARAM lParam)
{
	BYTE r, g, b;

	if (wParam == (WPARAM)m_SunColWnd.m_hWnd)
	{
		DWORD dwCol = m_SunColWnd.GetColor();
		r = GetRValue(dwCol);
		g = GetGValue(dwCol);
		b = GetBValue(dwCol);

		if(m_bIsDay)
		{
			m_Params.dwSunCol = A3DCOLORRGBA(r, g, b, 255);
			//	Update lighting view
			m_LightingView.SetSunColor(m_Params.dwSunCol);
			m_LightingView.InvalidateRect(NULL); 
			CString str;
			str.Format("%x",m_Params.dwSunCol);
			m_editSunColor.SetWindowText(str);
		}else
		{
			m_NightParams.dwSunCol = A3DCOLORRGBA(r, g, b, 255);
			//	Update lighting view
			m_LightingView.SetSunColor(m_NightParams.dwSunCol);
			m_LightingView.InvalidateRect(NULL); 
			CString str;
			str.Format("%x",m_NightParams.dwSunCol);
			m_editSunColor.SetWindowText(str);
		}
	}
	else	//	Ambient color
	{
		DWORD dwCol = m_AmbientWnd.GetColor();
		r = GetRValue(dwCol);
		g = GetGValue(dwCol);
		b = GetBValue(dwCol);

		if(m_bIsDay)
		{
			m_Params.dwAmbient = A3DCOLORRGBA(r, g, b, 255);
			//	Update lighting view
			m_LightingView.SetAmbient(m_Params.dwAmbient);
			m_LightingView.InvalidateRect(NULL);
			CString str;
			str.Format("%x",m_Params.dwAmbient);
			m_editAmbient.SetWindowText(str);
		}else
		{
			m_NightParams.dwAmbient = A3DCOLORRGBA(r, g, b, 255);
			//	Update lighting view
			m_LightingView.SetAmbient(m_NightParams.dwAmbient);
			m_LightingView.InvalidateRect(NULL);
			CString str;
			str.Format("%x",m_NightParams.dwAmbient);
			m_editAmbient.SetWindowText(str);
		}
	}

	return TRUE;
}


void CDlgLighting::OnChangeEditSundirpos() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	if(!m_bInited) return;
	
	CString str;
	UpdateData(true);
	int dir; 
	m_editSunDir.GetWindowText(str);
	dir = (int)atof(str);

	if(dir>3600)
	{
		dir = 3600;
		str.Format("%d",dir);
		m_editSunDir.SetWindowText(str);
	}else
	if(dir<0)
	{
		dir = 0;
		str.Format("%d",dir);
		m_editSunDir.SetWindowText(str);
	}
	m_SunDirSlider.SetPos(dir);
	int iSunHei = m_SunHeiSlider.GetPos();
	A3DVECTOR3 vSunDir = _MakeSunDirction(dir, iSunHei);
	m_LightingView.SetSunDirAndHeight(dir, iSunHei);
	m_LightingView.InvalidateRect(NULL);
	str.Format("%f",vSunDir.x);
	m_editSunDirX.SetWindowText(str);
	str.Format("%f",vSunDir.y);
	m_editSunDirY.SetWindowText(str);
	str.Format("%f",vSunDir.z);
	m_editSunDirZ.SetWindowText(str);
	UpdateData(false);
	// TODO: Add your control notification handler code here
	
}

void CDlgLighting::OnChangeEditSunheipos() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if(!m_bInited) return;
	
	CString str;
	UpdateData(true);
	int iHeight; 
	m_editSunHeight.GetWindowText(str);
	iHeight = (int)atof(str);
	if(iHeight>900)
	{
		iHeight = 900;
		str.Format("%d",iHeight);
		m_editSunHeight.SetWindowText(str);
	}else
	if(iHeight<0)
	{
		iHeight = 0;
		str.Format("%d",iHeight);
		m_editSunHeight.SetWindowText(str);
	}
	m_SunHeiSlider.SetPos(iHeight);
	int iSunDir = m_SunDirSlider.GetPos();
	A3DVECTOR3 vSunDir = _MakeSunDirction(iSunDir, iHeight);
	m_LightingView.SetSunDirAndHeight(iSunDir, iHeight);
	m_LightingView.InvalidateRect(NULL);
	str.Format("%f",vSunDir.x);
	m_editSunDirX.SetWindowText(str);
	str.Format("%f",vSunDir.y);
	m_editSunDirY.SetWindowText(str);
	str.Format("%f",vSunDir.z);
	m_editSunDirZ.SetWindowText(str);
	UpdateData(false);
}

void CDlgLighting::OnChangeEditSunpower() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	UpdateData(true);
	float power; 
	m_editSunPower.GetWindowText(str);
	power = (float)atof(str);
	if(power>100.0f)
	{
		power = 100;
		str.Format("%4.2f",power);
		m_editSunPower.SetWindowText(str);
	}else
	{
		if(power<0.01f)
		{
			power = 0.01f;
			str.Format("%4.2f",power);
			m_editSunPower.SetWindowText(str);
		}
	}
	if(m_bIsDay)
	{
		m_Params.fSunPower = power;
		g_Configs.fSunPower = power; 
	}else
	{
		m_NightParams.fSunPower = power;
		g_Configs.fNightSunPower = power; 
	}
}

void CDlgLighting::OnRadioDay() 
{
	// TODO: Add your control notification handler code here
	SaveItemData();
	m_bIsDay = true;
	UpdateControl();
}

void CDlgLighting::OnRadioNight() 
{
	// TODO: Add your control notification handler code here
	SaveItemData();
	m_bIsDay = false;
	UpdateControl();
}
