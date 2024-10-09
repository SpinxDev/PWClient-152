// DlgRandomMapInfoSetting.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "DlgRandomMapInfoSetting.h"
#include "Global.h"
#include "VssOperation.h"
#include "WorkSpaceSetDlg.h"
#include "AScriptFile.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CSingleMapGridSelectPolicy
//	
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

bool CSingleMapGridSelectPolicy::IsSelected(int iTileIndex)
{
	return IsValid(iTileIndex) && (m_iSelectedIndex == iTileIndex);
}

void CSingleMapGridSelectPolicy::ClearSelect()
{	
	m_iSelectedIndex = -1;
}

bool CSingleMapGridSelectPolicy::OnDataChanged(int iOld,int iNew)
{
	CString str;
	str.Format("%s\\randommap.dat",g_szEditMapDir);

	if(m_randmapProp==NULL || m_pDlgSetting == NULL) return false;
	
	m_pDlgSetting->UpdateData(TRUE);
	CRandMapProp::MAP_INFO info;
	info.type = m_pDlgSetting->m_iMapType;
	info.connection = m_pDlgSetting->GetMask();
	info.name = m_pDlgSetting->m_strMapName;
	
	if (iOld != -1)
	{
		CRandMapProp::MAP_INFO oldInfo;
		
		bool bHas = m_randmapProp->GetGirdProp(iOld,oldInfo);
		
		if (bHas && !(info == oldInfo))
		{
			if (info.name.GetLength()<1)
			{
				AfxMessageBox("名字不能为空");
				return false;
			}
			if(AUX_FileIsReadOnly(str)) 
			{
				AfxMessageBox("修改数据请先签出");
				return false;
			}
			int ret = AfxMessageBox("确定要保存修改吗?",MB_OKCANCEL);
			if (ret == MB_OKCANCEL)		
			{
				info.index = oldInfo.index;
				m_randmapProp->SetGridProp(iOld,info);				
			}
		}
	}
	
	bool bHas = m_randmapProp->GetGirdProp(iNew,info);
	if(!bHas)
	{
		info.connection = CRandMapProp::GRID_CONNECT_NONE;
		info.type = CRandMapProp::GRID_TYPE_NOUSE; // no use
		info.name = "maze";
	}
	
	m_pDlgSetting->m_iMapType = info.type;
	m_pDlgSetting->SetMask(info.connection);
	m_pDlgSetting->m_strMapName = info.name;
	
	m_pDlgSetting->UpdateData(FALSE);	

	return true;
}
bool CSingleMapGridSelectPolicy::OnRButtonDown(int iTileMouseOn, UINT nFlags)
{
	OnDataChanged(m_iSelectedIndex);

	ClearSelect();

	return true;
}
bool CSingleMapGridSelectPolicy::OnLButtonDown(int iTileMouseOn, UINT nFlags)
{
	bool bRet(false);

	if (CanSelect(iTileMouseOn) && m_iSelectedIndex != iTileMouseOn)
	{
		if(OnDataChanged(m_iSelectedIndex,iTileMouseOn))
		{
			m_iSelectedIndex = iTileMouseOn;
			bRet = true;								
		}
	}
	return bRet;
}

bool CSingleMapGridSelectPolicy::OnMouseMove(int iTileMouseOn, UINT nFlags)
{
	return false;
}

void CSingleMapGridSelectPolicy::ResetMore()
{
	ClearSelect();
}

void CSingleMapGridSelectPolicy::ClearSelectEnableMore(bool bEnable)
{
	ClearSelect();
}


BEGIN_MESSAGE_MAP(CRandomMapGird, CMapGrid)
//{{AFX_MSG_MAP(CMapGrid)
ON_WM_RBUTTONDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRandomMapGird::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pSelectPolicy)
	{
		m_pSelectPolicy->OnRButtonDown(-1,nFlags);
		Invalidate();
	}
}

int CRandomMapGird::GetSelectIndex()
{
	if (m_pSelectPolicy)
	{
		CSingleMapGridSelectPolicy* p = (CSingleMapGridSelectPolicy*)m_pSelectPolicy;
		if(p)
			return p->GetSelectIndex();
	}
	return -1;
}
//	Draw back buffer
bool CRandomMapGird::DrawBackBuffer(RECT* lprc)
{
	//	Clear background
	ClearBackBuffer(0xffffffff);

	if (!m_iNumTile)
		return true;

	RECT rcClient;
	GetClientRect(&rcClient);

	int iMaxRow = m_iOffsetY + (rcClient.bottom - SIZE_RULERTHICK + m_iTileSize - 1) / m_iTileSize;
	int iMaxCol = m_iOffsetX + (rcClient.right - SIZE_RULERTHICK + m_iTileSize - 1) / m_iTileSize;
	a_ClampRoof(iMaxRow, m_iNumRow);
	a_ClampRoof(iMaxCol, m_iNumCol);

	int i, x, y;

	HPEN hOldPen, hPen;
	hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hOldPen = (HPEN)::SelectObject(m_hMemDC, hPen);

	//	Draw horizonal table line
	x = SIZE_RULERTHICK + (m_iNumCol - m_iOffsetX) * m_iTileSize + 1;
	x = a_Min(x, (int)rcClient.right);
	y = SIZE_RULERTHICK;
	for (i=m_iOffsetY; i <= iMaxRow; i++, y+=m_iTileSize)
	{
		MoveToEx(m_hMemDC, 0, y, NULL);
		LineTo(m_hMemDC, x, y);
	}

	//	Draw vertical table line
	x = SIZE_RULERTHICK;
	y = SIZE_RULERTHICK + (m_iNumRow - m_iOffsetY) * m_iTileSize + 1;
	y = a_Min(y, (int)rcClient.bottom);
	for (i=m_iOffsetX; i <= iMaxCol; i++, x+=m_iTileSize)
	{
		MoveToEx(m_hMemDC, x, 0, NULL);
		LineTo(m_hMemDC, x, y);
	}

	//	Draw horizonal ruler scale
	char szText[100];
	RECT rc = {SIZE_RULERTHICK, 0, 0, SIZE_RULERTHICK};
	for (i=m_iOffsetX; i < iMaxCol; i++)
	{
		itoa(i, szText, 10);
		rc.right = rc.left + m_iTileSize;
		::DrawText(m_hMemDC, szText, strlen(szText), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		rc.left += m_iTileSize;
	}

	//	Draw vertical ruler scale
	::SetRect(&rc, 0, SIZE_RULERTHICK, SIZE_RULERTHICK, 0);
	for (i=m_iOffsetY; i < iMaxRow; i++)
	{
		itoa(i, szText, 10);
		rc.bottom = rc.top + m_iTileSize;
		::DrawText(m_hMemDC, szText, strlen(szText), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		rc.top += m_iTileSize;
	}

	::SelectObject(m_hMemDC, hOldPen);
	::DeleteObject(hPen);

	//	Fill map tiles

	for (y=m_iOffsetY; y < iMaxRow; y++)
	{
		int iCount = y * m_iNumCol + m_iOffsetX;

		rc.left		= SIZE_RULERTHICK + 1;
		rc.top		= SIZE_RULERTHICK + (y - m_iOffsetY) * m_iTileSize + 1,
		rc.bottom	= rc.top + m_iTileSize - 1;

		for (x=m_iOffsetX; x < iMaxCol; x++, iCount++)
		{
			rc.right = rc.left + m_iTileSize - 1;

			CString projName;
			GetProjName(iCount,projName);
			if(m_pGridDrawPolicy)
				m_pGridDrawPolicy->DrawGird(projName,iCount,IsSelected(iCount), m_hMemDC,rc);

			rc.left += m_iTileSize;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

bool CRandomMapGridDrawPolicy::GetConnectMark(int grid_con,int c,RECT& rc)
{
	if ((c & grid_con)==c)
	{
		switch (c)
		{
		case CRandMapProp::GRID_CONNECT_TOP:
			rc.top += 2;
			rc.bottom = rc.top + 6;
			rc.left = (rc.left + rc.right) / 2 - 3;
			rc.right = rc.left + 6;
			break;
		case CRandMapProp::GRID_CONNECT_BOTTOM:
			rc.bottom -= 2;
			rc.top = rc.bottom - 6;
			rc.left = (rc.left + rc.right) / 2 - 3;
			rc.right = rc.left + 6;
			break;
		case CRandMapProp::GRID_CONNECT_LEFT:
			rc.left += 2;
			rc.right = rc.left + 6;
			rc.top = (rc.top + rc.bottom) / 2 - 3;
			rc.bottom = rc.top + 6;
			break;
		case CRandMapProp::GRID_CONNECT_RIGHT:
			rc.right -= 2;
			rc.left = rc.right - 6;
			rc.top = (rc.top + rc.bottom) / 2 - 3;
			rc.bottom = rc.top + 6;
			break;
		}
		return true;
	}
	return false;
}

static COLORREF brushColor[CRandMapProp::GRID_TYPE_NUM+1] = {
	RGB(255, 255, 255), // normal
	RGB(255, 255, 0), //start
	RGB(0,255,255), // end
	RGB(64, 64, 64), // no use
	RGB(128,128,255),
	RGB(128,255,128),
	RGB(0,128,255),
	RGB(255, 0, 0), // sel
}; 
static COLORREF penColor = RGB(0,0,0);

CRandomMapGridDrawPolicy::CRandomMapGridDrawPolicy(CRandMapProp* prop):m_randMapProp(prop)
{
	for (int i=0;i<=CRandMapProp::GRID_TYPE_NUM;i++)
	{
		hBrush[i] = AUX_CreateBrush(BS_SOLID,brushColor[i],0);
	}
	
	hPen = ::CreatePen(PS_SOLID, 1, penColor);
}
CRandomMapGridDrawPolicy::~CRandomMapGridDrawPolicy()
{
	for(int i=0;i<=CRandMapProp::GRID_TYPE_NUM;i++)
		::DeleteObject(hBrush[i]);
	
	::DeleteObject(hPen);
}
void CRandomMapGridDrawPolicy::DrawGird(const CString& name,int iGrid,bool bSel,HDC hDC,RECT& rc)
{
	COLORREF col = ::GetBkColor(hDC);
	
	if (m_randMapProp)
	{
		CRandMapProp::MAP_INFO info;
		m_randMapProp->GetGirdProp(iGrid,info);
		::SetBkColor(hDC, brushColor[info.type]);
		::FillRect(hDC, &rc, hBrush[info.type]);
	}
	
	if (bSel)
	{
		::SetBkColor(hDC, brushColor[CRandMapProp::GRID_TYPE_NUM]);
		::FillRect(hDC, &rc, hBrush[CRandMapProp::GRID_TYPE_NUM]);
	}
	
	if (m_randMapProp)
	{
		CRandMapProp::MAP_INFO info;
		m_randMapProp->GetGirdProp(iGrid,info);
		HPEN oldPen = (HPEN)::SelectObject(hDC,hPen);	
		
		int con = CRandMapProp::GRID_CONNECT_LEFT;
		
		while(con<=CRandMapProp::GRID_CONNECT_BOTTOM)
		{
			RECT markRC = rc;
			if(GetConnectMark(info.connection,con,markRC))
				::Ellipse(hDC,markRC.left,markRC.top,markRC.right,markRC.bottom);

			con<<=1;			
		}
		::SelectObject(hDC,oldPen);
	}
	::DrawText(hDC, name, name.GetLength(), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	
	::SetBkColor(hDC, col);
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRandomMapInfoSetting dialog
CDlgRandomMapInfoSetting::CDlgRandomMapInfoSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRandomMapInfoSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRandomMapInfoSetting)
	m_iMapType = 3;
	for (int i=0;i<CRandMapProp::GRID_CON_NUM;i++)
	{
		m_mkCon[i] = FALSE;
	}
	m_iMainLineMinLen = 0;
	m_iMainLineMaxLen = 0;
	m_iBranchMinCount = 0;
	m_iBranchMaxCount = 0;
	m_iBranchMinLen = 0;
	m_iBranchMaxLen = 0;

	m_fPosX = 0.0f;
	m_fPosY = 0.0f;
	m_fPosZ = 0.0f;

	//}}AFX_DATA_INIT
}


void CDlgRandomMapInfoSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRandomMapInfoSetting)
	DDX_Radio(pDX, IDC_RAD_TYPE, m_iMapType);
	DDX_Check(pDX, IDC_CHK_CONNECT1, m_mkCon[0]);
	DDX_Check(pDX, IDC_CHK_CONNECT2, m_mkCon[1]);
	DDX_Check(pDX, IDC_CHK_CONNECT3, m_mkCon[2]);
	DDX_Check(pDX, IDC_CHK_CONNECT4, m_mkCon[3]);
	DDX_Control(pDX, IDC_RAD_TYPE, m_crtlMapType[0]);
	DDX_Control(pDX, IDC_RAD_TYPE2, m_crtlMapType[1]);
	DDX_Control(pDX, IDC_RAD_TYPE3, m_crtlMapType[2]);
	DDX_Control(pDX, IDC_RAD_TYPE4, m_crtlMapType[3]);
	DDX_Control(pDX, IDC_RAD_TYPE5, m_crtlMapType[4]);
	DDX_Control(pDX, IDC_RAD_TYPE6, m_crtlMapType[5]);
	DDX_Control(pDX, IDC_RAD_TYPE7, m_crtlMapType[6]);
	DDX_Text(pDX,IDC_EDIT_NAME,m_strMapName);
	DDX_Text(pDX,IDC_MAINLINE_MIN_LEN,m_iMainLineMinLen);
	DDX_Text(pDX,IDC_MAINLINE_MAX_LEN,m_iMainLineMaxLen);
	DDX_Text(pDX,IDC_BRANCH_MIN_COUNT,m_iBranchMinCount);
	DDX_Text(pDX,IDC_BRANCH_MAX_COUNT,m_iBranchMaxCount);
	DDX_Text(pDX,IDC_BRANCH_MIN_LEN,m_iBranchMinLen);
	DDX_Text(pDX,IDC_BRANCH_MAX_LEN,m_iBranchMaxLen);
	DDX_Text(pDX,IDC_START_POSX,m_fPosX);
	DDX_Text(pDX,IDC_START_POSY,m_fPosY);
	DDX_Text(pDX,IDC_START_POSZ,m_fPosZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRandomMapInfoSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgRandomMapInfoSetting)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_BTN_CHECKOUT, OnBtnCheckout)
	ON_BN_CLICKED(IDC_BTN_CHECKIN, OnBtnCheckin)
	ON_EN_CHANGE(IDC_MAINLINE_MIN_LEN, OnChangeEdit)
	ON_EN_CHANGE(IDC_MAINLINE_MAX_LEN, OnChangeEdit)
	ON_EN_CHANGE(IDC_BRANCH_MIN_COUNT, OnChangeEdit)
	ON_EN_CHANGE(IDC_BRANCH_MAX_COUNT, OnChangeEdit)
	ON_EN_CHANGE(IDC_BRANCH_MIN_LEN, OnChangeEdit)
	ON_EN_CHANGE(IDC_BRANCH_MAX_LEN, OnChangeEdit)
	ON_EN_CHANGE(IDC_START_POSX,OnChangeEdit)
	ON_EN_CHANGE(IDC_START_POSY,OnChangeEdit)
	ON_EN_CHANGE(IDC_START_POSZ,OnChangeEdit)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRandomMapInfoSetting message handlers
void CDlgRandomMapInfoSetting::OnBtnCheckout()
{
	bool ret = AUX_CheckOutRandomMapInfoFile();

	AfxMessageBox(ret ? "签出成功":"签出失败");
}
void CDlgRandomMapInfoSetting::OnBtnCheckin()
{
	if (m_pMapGridSelectPolicy)
	{
		m_pMapGridSelectPolicy->OnRButtonDown(-1,0);
	}

	bool ret = !m_randMapGridProp.Save()|| !AUX_CheckInRandomMapInfoFile();
	
	AfxMessageBox(ret ? "签入失败":"签入成功");

	m_mapGrid.Invalidate(FALSE);
}
void CDlgRandomMapInfoSetting::OnCancel()
{
	OnBtnOk();
}
void CDlgRandomMapInfoSetting::OnBtnOk() 
{
	UpdateData(TRUE);

	if (m_pMapGridSelectPolicy)
	{
		m_pMapGridSelectPolicy->OnRButtonDown(-1,0);
	}
	
	if (m_randMapGridProp.Save())
	{
		AUX_CheckInRandomMapInfoFile();
	}

	CDialog::OnOK();
}

BOOL CDlgRandomMapInfoSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//

	m_pMapGridSelectPolicy = NULL;
	m_pMapGridDrawProlicy = NULL;

	if (!m_projList.Init()){
		g_Log.Log("CDlgRandomMapInfoSetting::OnInitDialog, Failed to init ProjList.dat.");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	m_randMapGridProp.InitGird(m_projList.GetListCount());

	//
	
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_MAP)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	
	m_pMapGridSelectPolicy = new CSingleMapGridSelectPolicy(this,&m_randMapGridProp);	

	m_mapGrid.SetSelectPolicy(m_pMapGridSelectPolicy);

	if (!m_mapGrid.Create(this, rcWnd, 40))
	{
		g_Log.Log("CDlgRandomMapInfoSetting::OnInitDialog, Failed to create map grid window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	m_mapGrid.SetTileAndRow(m_projList.GetList(), m_projList.GetNumCol(), true);
	
	m_pMapGridDrawProlicy = new CRandomMapGridDrawPolicy(&m_randMapGridProp);

	
	m_mapGrid.SetDrawPolicy(m_pMapGridDrawProlicy);

	m_mapGrid.InvalidateRect(NULL);

	CRandMapProp::MAP_INFO info;
	
	if(m_randMapGridProp.GetGirdProp(m_mapGrid.GetSelectIndex(),info))
	{
		m_iMapType = info.type;
		SetMask(info.connection);		
	}

	SetHeaderInfo(m_randMapGridProp.GetHeader());

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRandomMapInfoSetting::OnDestroy() 
{
	CDialog::OnDestroy();
	
	delete m_pMapGridSelectPolicy;
	delete m_pMapGridDrawProlicy;	
}
void CDlgRandomMapInfoSetting::SetMask(int m)
{
	for (int i=0;i<CRandMapProp::GRID_CON_NUM;i++)
	{
		int flag = 1<<i;
		m_mkCon[i] = (m & flag) == flag;
	}
}
int CDlgRandomMapInfoSetting::GetMask()
{
	int ret = 0;
	for (int i=0;i<CRandMapProp::GRID_CON_NUM;i++)
	{
		if (m_mkCon[i])
		{
			ret |= 1<<i;
		}
	}
	return ret;
}
void CDlgRandomMapInfoSetting::SetHeaderInfo(const CRandMapProp::FILEHEADER& header)
{
	m_iMainLineMinLen = header.mainLineMinLen;
	m_iMainLineMaxLen = header.mainLineMaxLen;
	m_iBranchMinCount = header.branchLineNumMin;
	m_iBranchMaxCount = header.branchLineNumMax;
	m_iBranchMinLen = header.branchLineMinLen;
	m_iBranchMaxLen = header.branchLineMaxLen;
	m_fPosX = header.fPosX;
	m_fPosY = header.fPosY;
	m_fPosZ = header.fPosZ;

	UpdateData(FALSE);
}
void CDlgRandomMapInfoSetting::GetHeaderInfo(CRandMapProp::FILEHEADER& header)
{
	UpdateData(TRUE);
	header.mainLineMinLen = m_iMainLineMinLen;
	header.mainLineMaxLen = m_iMainLineMaxLen;
	header.branchLineNumMin = m_iBranchMinCount;
	header.branchLineNumMax = m_iBranchMaxCount;
	header.branchLineMinLen = m_iBranchMinLen;
	header.branchLineMaxLen = m_iBranchMaxLen;
	header.fPosX = m_fPosX;
	header.fPosY = m_fPosY;
	header.fPosZ = m_fPosZ;
}
static int pic_id[] = {
	IDC_COL_1,
	IDC_COL_2,
	IDC_COL_3,
	IDC_COL_4,
	IDC_COL_5,
	IDC_COL_6,
	IDC_COL_7
};
void CDlgRandomMapInfoSetting::OnPaint() 
{
	CDialog::OnPaint();

	for (int i=0;i<CRandMapProp::GRID_TYPE_NUM;i++)
	{
		RECT rc;
		GetDlgItem(pic_id[i])->GetClientRect(&rc);
		CDC* pDC = GetDlgItem(pic_id[i])->GetDC();
		pDC->FillSolidRect(rc.left,rc.top,rc.right,rc.bottom,brushColor[i]);
	}	
}
void CDlgRandomMapInfoSetting::OnChangeEdit() 
{
	CString str;
	str.Format("%s\\randommap.dat",g_szEditMapDir);

	if(AUX_FileIsReadOnly(str))
	{
		MessageBox("请先签出");
		return;
	}

	CRandMapProp::FILEHEADER header = m_randMapGridProp.GetHeader();
	GetHeaderInfo(header);
	m_randMapGridProp.SetHeader(header);
}