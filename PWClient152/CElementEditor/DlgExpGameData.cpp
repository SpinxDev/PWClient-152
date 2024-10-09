// DlgExpGameData.cpp : implementation file
//

#include "Global.h"
#include "Bitmap.h"
#include "TerrainExportor.h"
#include "SceneExportor.h"
#include "DlgExpGameData.h"
#include "DlgLighting.h"
#include "LightMapMerger.h"
#include "TerrainConnectDlg.h"
#include "VssOperation.h"

#include "AF.h"
#include "AC.h"

//#define new A_DEBUG_NEW



///////////////////////////////////////////////////////////////////////////
//	
//	Implement CMapPieceView
//	
///////////////////////////////////////////////////////////////////////////

CMapPieceView::CMapPieceView()
{
	m_pExpDlg		= NULL;
	m_iNumRow		= 0;
	m_iNumCol		= 0;
	m_iNumTile		= 0;
	m_iTileSize		= 64;
	m_iOffsetX		= 0;
	m_iOffsetY		= 0;
	m_iSelTile		= -1;
	m_iNumVisRow	= 0;
	m_iNumVisCol	= 0;
}

CMapPieceView::~CMapPieceView()
{
}

BEGIN_MESSAGE_MAP(CMapPieceView, CBackBufWnd)
	//{{AFX_MSG_MAP(CMapPieceView)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create window
bool CMapPieceView::Create(CDlgExpGameData* pParent, const RECT& rc, int iTileSize)
{
	//	Register class name
	CString strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
							LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)),
							NULL);//(HBRUSH)GetStockObject(WHITE_BRUSH));
	
	if (!CBackBufWnd::Create(strClassName, "MapPieceView", 
						WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_HSCROLL | WS_VSCROLL,
						rc, pParent, 0))
		return false;

	RECT rcClient;
	GetClientRect(&rcClient);

	m_pExpDlg		= pParent;
	m_iTileSize		= iTileSize;
	m_iNumVisRow	= (rcClient.bottom - SIZE_RULERTHICK) / iTileSize;
	m_iNumVisCol	= (rcClient.right - SIZE_RULERTHICK) / iTileSize;

	ShowScrollBar(SB_BOTH, TRUE);

	//	Set initial state
	SetTileAndRow(0, 0);

	return true;
}

//	Set tile number and column number
bool CMapPieceView::SetTileAndRow(int iNumTile, int iNumCol)
{
	if (!iNumTile || !iNumCol)
	{
		m_iNumCol	= 0;
		m_iNumRow	= 0;
		m_iNumTile	= 0;
	}
	else
	{
		m_iNumRow	= (iNumTile + iNumCol - 1) / iNumCol;
		m_iNumCol	= iNumCol;
		m_iNumTile	= iNumTile;
	}

	InvalidateRect(NULL);

	//	Reset scroll bar
	ResetScrollBar();

	return true;
}

//	Change current selected tile
void CMapPieceView::ChangeSelect(int iTile)
{
	m_iSelTile = iTile;
	InvalidateRect(NULL);
}

//	Reset scroll bar
void CMapPieceView::ResetScrollBar()
{
	if (!m_iNumTile)
	{
		EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
		return;
	}

	if (m_iNumCol <= m_iNumVisCol)
		EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
	else
	{
		EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);

		SCROLLINFO Info;
		Info.cbSize		= sizeof (Info);
		Info.fMask		= SIF_ALL;
		Info.nMin		= 0;
		Info.nMax		= m_iNumCol - 1;
		Info.nPos		= 0;
		Info.nPage		= m_iNumVisCol;
		Info.nTrackPos	= 0;

		SetScrollInfo(SB_HORZ, &Info);
	}

	if (m_iNumRow <= m_iNumVisRow)
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	else
	{
		EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);

		SCROLLINFO Info;
		Info.cbSize		= sizeof (Info);
		Info.fMask		= SIF_ALL;
		Info.nMin		= 0;
		Info.nMax		= m_iNumRow - 1;
		Info.nPos		= 0;
		Info.nPage		= m_iNumVisRow;
		Info.nTrackPos	= 0;

		SetScrollInfo(SB_VERT, &Info);
	}

	m_iOffsetX	= 0;
	m_iOffsetY	= 0;
}

void CMapPieceView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawBackBuffer(NULL);
	Flip(dc.m_hDC, NULL);
	
	// Do not call CBackBufWnd::OnPaint() for painting messages
}

//	Draw back buffer
bool CMapPieceView::DrawBackBuffer(RECT* lprc)
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
	HBRUSH hBrush, hSelBrush, hEmptyBrush;
	hBrush = AUX_CreateBrush(BS_SOLID, RGB(255, 255, 0), 0);
	hSelBrush = AUX_CreateBrush(BS_SOLID, RGB(255, 0, 0), 0);
	hEmptyBrush = AUX_CreateBrush(BS_SOLID, RGB(220, 220, 220), 0);

	for (y=m_iOffsetY; y < iMaxRow; y++)
	{
		int iCount = y * m_iNumCol + m_iOffsetX;

		rc.left		= SIZE_RULERTHICK + 1;
		rc.top		= SIZE_RULERTHICK + (y - m_iOffsetY) * m_iTileSize + 1,
		rc.bottom	= rc.top + m_iTileSize - 1;

		for (x=m_iOffsetX; x < iMaxCol; x++, iCount++)
		{
			rc.right = rc.left + m_iTileSize - 1;

			COLORREF col;
			if (iCount == m_iSelTile)
			{
				col = ::SetBkColor(m_hMemDC, RGB(255, 0, 0));
				::FillRect(m_hMemDC, &rc, hSelBrush);
			}
			else if (iCount < m_iNumTile)
			{
				col = ::SetBkColor(m_hMemDC, RGB(255, 255, 0));
				::FillRect(m_hMemDC, &rc, hBrush);
			}
			else
			{
				col = ::SetBkColor(m_hMemDC, RGB(220, 220, 220));
				::FillRect(m_hMemDC, &rc, hEmptyBrush);
			}

			itoa(iCount, szText, 10);
			::DrawText(m_hMemDC, szText, strlen(szText), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			::SetBkColor(m_hMemDC, col);

			rc.left += m_iTileSize;
		}
	}

	::DeleteObject(hBrush);
	::DeleteObject(hSelBrush);

	return true;
}

void CMapPieceView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode)
	{
	case SB_BOTTOM:		m_iOffsetY = m_iNumRow - m_iNumVisRow;	break;
	case SB_TOP:		m_iOffsetY = 0;	break;
	case SB_LINEDOWN:	m_iOffsetY++;	break;
	case SB_LINEUP:		m_iOffsetY--;	break;
	case SB_PAGEDOWN:	m_iOffsetY += m_iNumVisRow;	break;
	case SB_PAGEUP:		m_iOffsetY -= m_iNumVisRow;	break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:

		m_iOffsetY = nPos;
		break;
	}

	a_Clamp(m_iOffsetY, 0, m_iNumRow - m_iNumVisRow);
	SetScrollPos(SB_VERT, m_iOffsetY);

	InvalidateRect(NULL);

	CBackBufWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMapPieceView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode)
	{
	case SB_BOTTOM:		m_iOffsetX = m_iNumCol - m_iNumVisCol;	break;
	case SB_TOP:		m_iOffsetX = 0;	break;
	case SB_LINEDOWN:	m_iOffsetX++;	break;
	case SB_LINEUP:		m_iOffsetX--;	break;
	case SB_PAGEDOWN:	m_iOffsetX += m_iNumVisCol;	break;
	case SB_PAGEUP:		m_iOffsetX -= m_iNumVisCol;	break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:

		m_iOffsetX = nPos;
		break;
	}

	a_Clamp(m_iOffsetX, 0, m_iNumCol - m_iNumVisCol);
	SetScrollPos(SB_HORZ, m_iOffsetX);

	InvalidateRect(NULL);
	
	CBackBufWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CMapPieceView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
//	return CBackBufWnd::OnEraseBkgnd(pDC);
}

void CMapPieceView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iTile = GetTileIndex(point.x, point.y);
	if (iTile >= 0 && iTile != m_iSelTile)
		m_pExpDlg->ChangeSelectedProject(iTile);
	
	CBackBufWnd::OnLButtonDown(nFlags, point);
}

void CMapPieceView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int iTile = GetTileIndex(point.x, point.y);
	if (iTile >= 0 && iTile != m_iSelTile)
		m_pExpDlg->ChangeSelectedProject(iTile);

	CBackBufWnd::OnLButtonDblClk(nFlags, point);
}

//	Get tile index of specified position
int CMapPieceView::GetTileIndex(int x, int y)
{
	if (!m_iNumTile || x <= SIZE_RULERTHICK || y <= SIZE_RULERTHICK)
		return -1;

	int iCol = m_iOffsetX + (x - SIZE_RULERTHICK) / m_iTileSize;
	if (iCol >= m_iNumCol)
		return -1;

	int iRow = m_iOffsetY + (y - SIZE_RULERTHICK) / m_iTileSize;
	if (iRow >= m_iNumRow)
		return -1;

	return iRow * m_iNumCol + iCol;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgExpGameData dialog

CDlgExpGameData::CDlgExpGameData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExpGameData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExpGameData)
	m_dwNumCol = 0;
	m_bLighting = TRUE;
	//}}AFX_DATA_INIT

	m_iProjMaskCol	= 0;
	m_iProjMaskRow	= 0;
	m_iSubTrnGrid	= 0;
}


void CDlgExpGameData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExpGameData)
	DDX_Control(pDX, IDC_BTN_SAVEPROJLIST, m_SaveProjBtn);
	DDX_Control(pDX, IDC_BTN_MOVEUPPROJ, m_MUProjBtn);
	DDX_Control(pDX, IDC_BTN_MOVEDNPROJ, m_MDProjBtn);
	DDX_Control(pDX, IDC_BTN_LOADPROJLIST, m_LoadProjBtn);
	DDX_Control(pDX, IDC_LIST_PROJECT, m_ProjList);
	DDX_Text(pDX, IDC_EDIT_COLUMN, m_dwNumCol);
	DDV_MinMaxUInt(pDX, m_dwNumCol, 1, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExpGameData, CDialog)
	//{{AFX_MSG_MAP(CDlgExpGameData)
	ON_BN_CLICKED(IDC_BTN_LOADPROJLIST, OnBtnLoadprojlist)
	ON_BN_CLICKED(IDC_BTN_SAVEPROJLIST, OnBtnSaveprojlist)
	ON_BN_CLICKED(IDC_BTN_MOVEDNPROJ, OnBtnMovednproj)
	ON_BN_CLICKED(IDC_BTN_MOVEUPPROJ, OnBtnMoveupproj)
	ON_BN_DOUBLECLICKED(IDC_BTN_MOVEDNPROJ, OnDoubleclickedBtnMovednproj)
	ON_BN_DOUBLECLICKED(IDC_BTN_MOVEUPPROJ, OnDoubleclickedBtnMoveupproj)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROJECT, OnItemchangedListProject)
	ON_BN_CLICKED(IDC_BUTTON_TERRAIN_LINK, OnButtonTerrainLink)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExpGameData message handlers

BOOL CDlgExpGameData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	bool bFileExist = true;
	char szNum[10];
	
	// Get projlist 
	CString str;
	str.Format("%s%s",g_szWorkDir,g_szEditMapDir);
	g_VSS.SetWorkFolder(str);
	g_VSS.SetProjectPath(str);
	str.Format("%s\\ProjList.dat",g_szEditMapDir);
	g_VSS.GetFile(str);

	//	Change edit boxes to flat style
	AUX_FlatEditBox((CEdit*)GetDlgItem(IDC_EDIT_COLUMN));

	m_dwNumCol = g_Configs.iTExpColNum;

	//	Set icon for buttons
	m_LoadProjBtn.SetButtonIcon(16, IDI_OPEN);
	m_SaveProjBtn.SetButtonIcon(16, IDI_SAVE);
	m_MDProjBtn.SetButtonIcon(16, IDI_MOVEDOWN);
	m_MUProjBtn.SetButtonIcon(16, IDI_MOVEUP);

	//	Initialize project list
	m_ProjList.InsertColumn(0, "索引", LVCFMT_LEFT, 40);
	m_ProjList.InsertColumn(1, "项目名称", LVCFMT_LEFT, 140);
	m_ProjList.SetExtendedStyle(m_ProjList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	
	
	// Load project file list
	AScriptFile File;
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	if (!File.Open(szProj))
	{
		g_Log.Log("Open file %s failed!",szProj);
		bFileExist = false;
	}
	if(bFileExist)
	{
		//	Clear current project list
		m_ProjList.DeleteAllItems();
		if(File.GetNextToken(true))
		{
			CString strCol(File.m_szToken);
			if(strCol.Find("col=")!=-1)
			{
				strCol.Replace("col=","");
				m_dwNumCol = (int)atof(strCol);
			}
			else 
			{
				m_dwNumCol = 1;
				File.ResetScriptFile();
			}
			
		}
		
		while (File.GetNextToken(true))
		{
			int iItem = m_ProjList.GetItemCount();
			itoa(iItem, szNum, 10);
			iItem = m_ProjList.InsertItem(iItem, szNum);
			
			m_ProjList.SetItemText(iItem, 1, File.m_szToken);
		}
		
		File.Close();
	}
	//	Create map view window
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_MAPPIECE)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	if (!m_MapView.Create(this, rcWnd, 40))
	{
		g_Log.Log("CDlgExpGameData::OnInitDialog, Failed to create map piece view window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	//	Add all project name to list
	if(!bFileExist)
	{
		
		CString strFilter;
		strFilter.Format("%s%s\\*.*", g_szWorkDir, g_szEditMapDir);
		WIN32_FIND_DATA FindData;
		char szName[MAX_PATH];
		
		HANDLE hHandle = FindFirstFile(strFilter, &FindData);
		while (hHandle != INVALID_HANDLE_VALUE)
		{
			if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				FindData.cFileName[0] !=  '.' && 
				stricmp(g_szTempMap, FindData.cFileName))
			{
				int iItem = m_ProjList.GetItemCount();
				itoa(iItem, szNum, 10);
				iItem = m_ProjList.InsertItem(iItem, szNum);
				
				af_GetFileTitle(FindData.cFileName, szName, MAX_PATH);
				m_ProjList.SetItemText(iItem, 1, szName);
			}
			
			if (!FindNextFile(hHandle, &FindData))
				break;
		}
		
		FindClose(hHandle);
	}
	//	Initalize map piece view
	m_MapView.SetTileAndRow(m_ProjList.GetItemCount(), (int)m_dwNumCol);
	m_MapView.InvalidateRect(NULL);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//	Load project list
void CDlgExpGameData::OnBtnLoadprojlist() 
{
	
	//	Let user select a text file
	char szFilter[] = "Text Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.txt", NULL, dwFlags, szFilter, NULL);

	if (FileDia.DoModal() == IDCANCEL)
		return;
	
	// Load project file list
	AScriptFile File;
	if (!File.Open(FileDia.GetPathName()))
	{
		g_Log.Log("Open file %s failed!",FileDia.GetPathName());
		return;
	}
	
	//	Clear current project list
	m_ProjList.DeleteAllItems();
	if(File.GetNextToken(true))
	{
		CString strCol(File.m_szToken);
		if(strCol.Find("col=")!=-1)
		{
			strCol.Replace("col=","");
			m_dwNumCol = (int)atof(strCol);
		}
		else File.ResetScriptFile();
		
	}
	
	char szNum[10];
	while (File.GetNextToken(true))
	{
		int iItem = m_ProjList.GetItemCount();
		itoa(iItem, szNum, 10);
		iItem = m_ProjList.InsertItem(iItem, szNum);
		
		m_ProjList.SetItemText(iItem, 1, File.m_szToken);
	}
	
	File.Close();

	//	Update map piece view
	m_MapView.SetTileAndRow(m_ProjList.GetItemCount(), (int)m_dwNumCol);
	m_MapView.ChangeSelect(-1);
	
}

//	Save project list
void CDlgExpGameData::OnBtnSaveprojlist() 
{
	
	char szFilter[] = "Text Files (*.txt)|*.txt||";
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(FALSE, "*.txt", NULL, dwFlags, szFilter, NULL);
		
	if (FileDia.DoModal() == IDCANCEL)
		return;

	FILE* fp = fopen(FileDia.GetPathName(), "w");
	if (!fp)
	{
		MessageBox("创建文件失败!");
		return;
	}

	CString str;
	int i, iNumItem = m_ProjList.GetItemCount();
	str.Format("col=%d",m_dwNumCol);
	fprintf(fp,"%s\n",str);
	for (i=0; i < iNumItem; i++)
	{
		str = m_ProjList.GetItemText(i, 1);
		fprintf(fp, "%s\n", str);
	}

	fclose(fp);
}

//	Move project down
void CDlgExpGameData::OnBtnMovednproj() 
{
	if (!m_ProjList.GetSelectedCount())
		return;

	POSITION pos = m_ProjList.GetFirstSelectedItemPosition();
	int iSel = m_ProjList.GetNextSelectedItem(pos);
	if (iSel == m_ProjList.GetItemCount()-1)
		return;

	CString strTemp = m_ProjList.GetItemText(iSel+1, 1);
	m_ProjList.SetItemText(iSel+1, 1, m_ProjList.GetItemText(iSel, 1));
	m_ProjList.SetItemText(iSel, 1, strTemp);

	m_ProjList.SetItemState(iSel+1, LVIS_SELECTED, LVIS_SELECTED);
}

//	Move project up
void CDlgExpGameData::OnBtnMoveupproj() 
{
	if (!m_ProjList.GetSelectedCount())
		return;

	POSITION pos = m_ProjList.GetFirstSelectedItemPosition();
	int iSel = m_ProjList.GetNextSelectedItem(pos);
	if (!iSel)
		return;

	CString strTemp = m_ProjList.GetItemText(iSel-1, 1);
	m_ProjList.SetItemText(iSel-1, 1, m_ProjList.GetItemText(iSel, 1));
	m_ProjList.SetItemText(iSel, 1, strTemp);

	m_ProjList.SetItemState(iSel-1, LVIS_SELECTED, LVIS_SELECTED);
}

void CDlgExpGameData::OnDoubleclickedBtnMovednproj() 
{
	OnBtnMovednproj();
}

void CDlgExpGameData::OnDoubleclickedBtnMoveupproj() 
{
	OnBtnMoveupproj();	
}

//	Do export
void CDlgExpGameData::OnOK() 
{
	SaveList();
	CDialog::OnOK();
}

bool CDlgExpGameData::SaveList()
{
	UpdateData(true);
	if(!AUX_CheckOutProjList())
	{
		MessageBox("CDlgExpGameData::SaveList(), Failed to check out projlist.dat");
		return false;
	}
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	FILE* fp = fopen(szProj, "w");
	if (!fp)
	{
		MessageBox("创建文件(%s)失败!",szProj);
		return false;
	}

	CString str;
	int i, iNumItem = m_ProjList.GetItemCount();
	str.Format("col=%d",m_dwNumCol);
	fprintf(fp,"%s\n",str);
	for (i=0; i < iNumItem; i++)
	{
		str = m_ProjList.GetItemText(i, 1);
		fprintf(fp, "%s\n", str);
	}
	fclose(fp);	
	if(!AUX_CheckInProjList())
	{
		MessageBox("CDlgExpGameData::OnOK(), Failed to check in projlist.dat");
		return false;
	}
	return true;
}

void CDlgExpGameData::OnItemchangedListProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem != 0)
		return;

	bool bSel1 = (pNMListView->uOldState & LVIS_SELECTED) ? true : false;
	bool bSel2 = (pNMListView->uNewState & LVIS_SELECTED) ? true : false;

	if (bSel1 != bSel2 && bSel2)
	{
		m_MapView.ChangeSelect(pNMListView->iItem);
	}
}

BOOL CDlgExpGameData::PreTranslateMessage(MSG* pMsg) 
{
	bool bBlock = false;
	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_EDIT_COLUMN)->m_hWnd)
		{
			if (UpdateData(TRUE))
				m_MapView.SetTileAndRow(m_ProjList.GetItemCount(), (int)m_dwNumCol);
			
			bBlock = true;
		}
	}
	
	if (bBlock)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

//	Change current selected project
void CDlgExpGameData::ChangeSelectedProject(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_ProjList.GetItemCount())
		return;

	m_ProjList.SetItemState(iIndex, LVIS_SELECTED, LVIS_SELECTED);
	m_ProjList.EnsureVisible(iIndex, FALSE);
}


void CDlgExpGameData::OnButtonTerrainLink() 
{
	// TODO: Add your control notification handler code here
	SaveList();
	CTerrainConnectDlg dlg;
	dlg.m_bLinkAllMap = true;
	dlg.DoModal();	
}
