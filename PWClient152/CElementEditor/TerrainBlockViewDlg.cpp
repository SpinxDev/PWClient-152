// TerrainBlockViewDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "TerrainBlockViewDlg.h"
#include "ElementMap.h"
#include "MainFrm.h"
#include "A3DTerrain2.h"
#include "TerrainRender.h"
#include "A3DTerrain2Blk.h"
#include "Render.h"
#include "TerrainLayer.h"
#include "Bitmap.h"
#include "Terrain.h"
#include "A3DTypes.h"
#include "MaskModifier.h"

#include "AF.h"
#include "AC.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CMapPieceView
//	
///////////////////////////////////////////////////////////////////////////

CMapBlockView::CMapBlockView()
{
	m_iNumRow		= 5;
	m_iNumCol		= 5;
	m_iNumTile		= 0;
	m_iTileSize		= 40;
	m_iOffsetX		= 0;
	m_iOffsetY		= 0;
	m_iSelTile		= -1;
	m_iNumVisRow	= 0;
	m_iNumVisCol	= 0;
	m_nGridSize     = 15;
	for( int i = 0; i < BLOCK_MAX_TEXTURE_LEVEL; i++)
		m_dwColorArray[i] = RGB(0 + i*(255/BLOCK_MAX_TEXTURE_LEVEL),255-i*(255/BLOCK_MAX_TEXTURE_LEVEL),0);
}

CMapBlockView::~CMapBlockView()
{
}

BEGIN_MESSAGE_MAP(CMapBlockView, CBackBufWnd)
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
bool CMapBlockView::Create(CTerrainBlockViewDlg* pParent, const RECT& rc, int iTileSize)
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
	
	m_iTileSize		= iTileSize;
	m_iNumVisRow	= (rcClient.bottom - m_nGridSize) / iTileSize;
	m_iNumVisCol	= (rcClient.right - m_nGridSize) / iTileSize;

	ShowScrollBar(SB_BOTH, TRUE);

	//	Set initial state
	SetTileAndRow(0, 0);

	return true;
}

//	Set tile number and column number
bool CMapBlockView::SetTileAndRow(int iNumTile, int iNumCol)
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
void CMapBlockView::ChangeSelect(int iTile)
{
	m_iSelTile = iTile;
	InvalidateRect(NULL);
}

//	Reset scroll bar
void CMapBlockView::ResetScrollBar()
{
	
	RECT rcClient;
	GetClientRect(&rcClient);
	
	m_iNumVisRow	= (rcClient.bottom - m_nGridSize) / m_iTileSize;
	m_iNumVisCol	= (rcClient.right - m_nGridSize) / m_iTileSize;
	
	
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

void CMapBlockView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawBackBuffer(NULL);
	Flip(dc.m_hDC, NULL);
	
	// Do not call CBackBufWnd::OnPaint() for painting messages
}

//	Draw back buffer
bool CMapBlockView::DrawBackBuffer(RECT* lprc)
{
	//	Clear background
	ClearBackBuffer(RGB(128,128,128));
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap== NULL) return true;

	if (!m_iNumTile)
		return true;

	RECT rcClient;
	GetClientRect(&rcClient);

	int iMaxRow = m_iOffsetY + (rcClient.bottom - m_nGridSize + m_iTileSize - 1) / m_iTileSize;
	int iMaxCol = m_iOffsetX + (rcClient.right - m_nGridSize + m_iTileSize - 1) / m_iTileSize;
	a_ClampRoof(iMaxRow, m_iNumRow);
	a_ClampRoof(iMaxCol, m_iNumCol);

	int i, x, y;

	HPEN hOldPen, hPen;
	hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 192, 192));
	hOldPen = (HPEN)::SelectObject(m_hMemDC, hPen);

	//	Draw horizonal table line
	x = m_nGridSize + (m_iNumCol - m_iOffsetX) * m_iTileSize + 1;
	x = a_Min(x, (int)rcClient.right);
	y = m_nGridSize;
	for (i=m_iOffsetY; i <= iMaxRow; i++, y+=m_iTileSize)
	{
		MoveToEx(m_hMemDC, 0, y, NULL);
		LineTo(m_hMemDC, x, y);
	}

	//	Draw vertical table line
	x = m_nGridSize;
	y = m_nGridSize + (m_iNumRow - m_iOffsetY) * m_iTileSize + 1;
	y = a_Min(y, (int)rcClient.bottom);
	for (i=m_iOffsetX; i <= iMaxCol; i++, x+=m_iTileSize)
	{
		MoveToEx(m_hMemDC, x, 0, NULL);
		LineTo(m_hMemDC, x, y);
	}

	//	Draw horizonal ruler scale
	char szText[100];
	RECT rc = {m_nGridSize, 0, 0, m_nGridSize};
	::SetTextColor(m_hMemDC,RGB(0,0,255));
	::SetBkMode(m_hMemDC,TRANSPARENT);
	for (i=m_iOffsetX; i < iMaxCol; i++)
	{
		_itoa(i, szText, 10);
		rc.right = rc.left + m_iTileSize;
		::DrawText(m_hMemDC, szText, strlen(szText), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		rc.left += m_iTileSize;
	}
	
	//	Draw vertical ruler scale
	::SetRect(&rc, 0, m_nGridSize, m_nGridSize, 0);
	for (i=m_iOffsetY; i < iMaxRow; i++)
	{
		_itoa(i, szText, 10);
		rc.bottom = rc.top + m_iTileSize;
		::DrawText(m_hMemDC, szText, strlen(szText), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		rc.top += m_iTileSize;
	}
	::SetTextColor(m_hMemDC,RGB(0,0,0));
	::SelectObject(m_hMemDC, hOldPen);
	::DeleteObject(hPen);

	//	Fill map tiles
	HBRUSH hBrush, hSelBrush, hEmptyBrush;
	hSelBrush = AUX_CreateBrush(BS_SOLID, RGB(0, 255, 255), 0);
	hEmptyBrush = AUX_CreateBrush(BS_SOLID, RGB(220, 220, 220), 0);
	int nLevel = 9;

	for (y=m_iOffsetY; y < iMaxRow; y++)
	{
		int iCount = y * m_iNumCol + m_iOffsetX;

		rc.left		= m_nGridSize + 1;
		rc.top		= m_nGridSize + (y - m_iOffsetY) * m_iTileSize + 1,
		rc.bottom	= rc.top + m_iTileSize - 1;

		for (x=m_iOffsetX; x < iMaxCol; x++, iCount++)
		{
			rc.right = rc.left + m_iTileSize - 1;
			CMaskModifier* pMM = pMap->GetTerrain()->GetMaskModifier();
			UINT64 llFlags = 0;
			if(m_bShowSmallBlock)
			{
				int idx = y * m_iNumCol + x;
				llFlags = pMM->GetBlockFlags(idx);
			}else
			{
				for(int n = 0; n<4; n++)
				{
					UINT64 temp;
					for( int xx = 0; xx < 4; ++xx)
					{
						int idx = (y*4+n) * m_iNumCol * 4 + (x*4 + xx);
						temp = pMM->GetBlockFlags(idx);
						llFlags |= temp;
					}
				}
			}
			
			nLevel = GetLevelNum(llFlags);
			if(nLevel>BLOCK_MAX_TEXTURE_LEVEL-1) nLevel = BLOCK_MAX_TEXTURE_LEVEL-1;
			hBrush = AUX_CreateBrush(BS_SOLID, m_dwColorArray[nLevel], 0);
			if (iCount == m_iSelTile)
			{
				::FillRect(m_hMemDC, &rc, hSelBrush);
			}
			else if (iCount < m_iNumTile)
			{
				::FillRect(m_hMemDC, &rc, hBrush);
			}
			else
			{
				::FillRect(m_hMemDC, &rc, hEmptyBrush);
			}

			_itoa(nLevel, szText, 10);
			::DrawText(m_hMemDC, szText, strlen(szText), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			//::SetBkColor(m_hMemDC, col);
			::DeleteObject(hBrush);
			rc.left += m_iTileSize;
		}
	}
	/*
	if(m_bShowSmallBlock)
	{//Draw color grid
		hPen = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
		hOldPen = (HPEN)::SelectObject(m_hMemDC, hPen);
		
		//	Draw horizonal table line
		x = m_nGridSize + (m_iNumCol - m_iOffsetX) * m_iTileSize + 1;
		x = a_Min(x, (int)rcClient.right);
		y = m_nGridSize;
		int m = 4;
		m -= m_iOffsetY;
		for (i=m_iOffsetY; i <= iMaxRow/4; i++, y+=m_iTileSize*m,m = 4)
		{
			if(m!=4 && m_iOffsetY!=0) continue;
			MoveToEx(m_hMemDC, 0, y, NULL);
			LineTo(m_hMemDC, x, y);
		}
		
		//	Draw vertical table line
		x = m_nGridSize;
		y = m_nGridSize + (m_iNumRow - m_iOffsetY) * m_iTileSize + 1;
		y = a_Min(y, (int)rcClient.bottom);
		for (i=m_iOffsetX; i <= iMaxCol/4; i++, x+=m_iTileSize*4)
		{
			MoveToEx(m_hMemDC, x, 0, NULL);
			LineTo(m_hMemDC, x, y);
		}
		::SetTextColor(m_hMemDC,RGB(0,0,0));
		::SelectObject(m_hMemDC, hOldPen);
		::DeleteObject(hPen);
	}*/
	
	::DeleteObject(hBrush);
	::DeleteObject(hSelBrush);

	return true;
}

int CMapBlockView::GetLevelNum( UINT64 llFlag)
{
	int n = 0;
	for(int i = 0; i < 64; i++)
	{
		UINT64 lvalue = llFlag >> i;
		if((lvalue & 0x1) == 0x1) 
			n++;
	}
	return n;
}

void CMapBlockView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CMapBlockView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

BOOL CMapBlockView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
//	return CBackBufWnd::OnEraseBkgnd(pDC);
}

void CMapBlockView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iTile = GetTileIndex(point.x, point.y);
	m_iSelTile = iTile;
	g_nCurrentTerrainBlock = m_iSelTile;
	//if (iTile >= 0 && iTile != m_iSelTile)
	//	m_pExpDlg->ChangeSelectedProject(iTile);
	CTerrainBlockViewDlg *parent = (CTerrainBlockViewDlg *)GetParent();
	if(parent) parent->FreshLayBox();
	InvalidateRect(NULL);
	CBackBufWnd::OnLButtonDown(nFlags, point);
}

void CMapBlockView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int iTile = GetTileIndex(point.x, point.y);
	m_iSelTile = iTile;
	//if (iTile >= 0 && iTile != m_iSelTile)
	//	m_pExpDlg->ChangeSelectedProject(iTile);
	InvalidateRect(NULL);
	CTerrainBlockViewDlg *parent = (CTerrainBlockViewDlg *)GetParent();
	if(parent) parent->CloseWindow();
	CBackBufWnd::OnLButtonDblClk(nFlags, point);
}

//	Get tile index of specified position
int CMapBlockView::GetTileIndex(int x, int y)
{
	if (!m_iNumTile || x <= m_nGridSize || y <= m_nGridSize)
		return -1;

	int iCol = m_iOffsetX + (x - m_nGridSize) / m_iTileSize;
	if (iCol >= m_iNumCol)
		return -1;

	int iRow = m_iOffsetY + (y - m_nGridSize) / m_iTileSize;
	if (iRow >= m_iNumRow)
		return -1;

	return iRow * m_iNumCol + iCol;
}


/////////////////////////////////////////////////////////////////////////////
// CTerrainBlockViewDlg dialog


CTerrainBlockViewDlg::CTerrainBlockViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainBlockViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainBlockViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nCurrentBlock = -1;
	m_nCurrentLayer = -1;
	m_bShowSmallBlock = true;
}


void CTerrainBlockViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainBlockViewDlg)
	DDX_Control(pDX, IDC_COMBO_LAY, m_comboLay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainBlockViewDlg, CDialog)
	//{{AFX_MSG_MAP(CTerrainBlockViewDlg)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_VIEW_TYPE, OnButtonChangeViewType)
	ON_CBN_SELCHANGE(IDC_COMBO_LAY, OnSelchangeComboLay)
	ON_BN_CLICKED(IDC_BUTTON_BLOCK_BLACK, OnButtonBlockBlack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainBlockViewDlg message handlers

BOOL CTerrainBlockViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	RECT rcWnd;
	GetDlgItem(IDC_BLOCK_VIEW)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	if (!m_MapView.Create(this, rcWnd, 40))
	{
		g_Log.Log("CDlgExpGameData::OnInitDialog, Failed to create map piece view window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainBlockViewDlg::OnOK()
{
	g_nCurrentTerrainBlock = m_MapView.GetSelectedBlock();
	g_Configs.bShowSmallBlock = m_bShowSmallBlock;
	if(g_nCurrentTerrainBlock!=-1)
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap==NULL) return;
		if(!pMap->GetTerrain()) return;
		
		CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
		if(pTR==NULL) return;
		int h,c;
		A3DTerrain2Block* block = NULL;
		if(m_bShowSmallBlock)
		{
			h = g_nCurrentTerrainBlock / pTR->GetBlockColNum();
			c = g_nCurrentTerrainBlock % pTR->GetBlockColNum();
			block = pTR->GetBlock(h,c);
		}
		else 
		{
			h = g_nCurrentTerrainBlock / (pTR->GetBlockColNum()/4);
			c = g_nCurrentTerrainBlock % (pTR->GetBlockColNum()/4);
			block = pTR->GetBlock(h*4,c*4);
		}
		if(block==NULL) return;
		
		A3DAABB aabb = block->GetBlockAABB();
		
		A3DVECTOR3 pos = aabb.Center + A3DVECTOR3(0.0f,50.0f,0.0f);
		A3DVECTOR3 dir = A3DVECTOR3(0.0f,-1.0f,0.1f);
		CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
		CRenderWnd *pRWnd = pFrame->GetRenderWnd();
		if(pRWnd->GetActiveViewport() == VIEW_PERSPECTIVE)
		{
			g_Render.GetA3DEngine()->GetActiveCamera()->SetPos(pos);
			g_Render.GetA3DEngine()->GetActiveCamera()->SetDirAndUp(dir,A3DVECTOR3(0.0f,1.0f,0.0f));
		}
	}
	CDialog::OnOK();
}

void CTerrainBlockViewDlg::CloseWindow()
{
	OnOK();
}

void CTerrainBlockViewDlg::OnButtonChangeViewType() 
{
	// TODO: Add your control notification handler code here
	bool bOldState = m_bShowSmallBlock;
	m_bShowSmallBlock = !m_bShowSmallBlock;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
		m_MapView.SetShowSmallBlock(m_bShowSmallBlock);
		if(m_bShowSmallBlock)
		{
			if(!bOldState) 
			{//大块变小块
				
				int h = g_nCurrentTerrainBlock / (pTR->GetBlockColNum()/4);
				int c = g_nCurrentTerrainBlock % (pTR->GetBlockColNum()/4);
				m_nCurrentBlock = h*4*pTR->GetBlockColNum() + c*4;
				g_nCurrentTerrainBlock = m_nCurrentBlock;
				m_MapView.SetTileAndRow(pTR->GetBlockRowNum()*pTR->GetBlockColNum(),pTR->GetBlockColNum());
			}
		}else 
		{
			if(bOldState)
			{//小块变大块
				int h = g_nCurrentTerrainBlock / pTR->GetBlockColNum();
				int c = g_nCurrentTerrainBlock % pTR->GetBlockColNum();
				m_nCurrentBlock = (h/4) * 4 + c/4;
				g_nCurrentTerrainBlock = m_nCurrentBlock;
				m_MapView.SetTileAndRow(pTR->GetBlockRowNum()*pTR->GetBlockColNum()/16,pTR->GetBlockColNum()/4);
			}
		}
		//m_nCurrentBlock = -1;
		m_MapView.SetSelectedBlock(m_nCurrentBlock);
		g_Configs.bShowSmallBlock = m_bShowSmallBlock;
		m_MapView.InvalidateRect(NULL);
		FreshLayBox();
	}
}

void CTerrainBlockViewDlg::FreshLayBox()
{
	int count = m_comboLay.GetCount();
	for (int i=0;i < count;i++)
	{
		m_comboLay.DeleteString( 0 );
	}

	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
	if(pTR==NULL) return;
	if(pTR->GetBlockColNum() == 0) return;

	int h,c;
	A3DTerrain2Block* block = NULL;
	if(m_MapView.GetSelectedBlock()==-1) return;
	if(m_bShowSmallBlock)
	{
		h = m_MapView.GetSelectedBlock()/ pTR->GetBlockColNum();
		c = m_MapView.GetSelectedBlock() % pTR->GetBlockColNum();
		//block = pTR->GetBlock(h,c);
	}
	else 
	{
		h = m_MapView.GetSelectedBlock() / (pTR->GetBlockColNum()/4);
		c = m_MapView.GetSelectedBlock() % (pTR->GetBlockColNum()/4);
		//block = pTR->GetBlock(h*map_block_num,c*map_block_num);
	}

	UINT64  llFlags = 0;
	if(m_bShowSmallBlock)
	{
		llFlags = pMap->GetTerrain()->GetMaskModifier()->GetBlockFlags(h,c);
		for( int k = 0; k< pMap->GetTerrain()->GetLayerNum(); k++)
		{
			CTerrainLayer* pLayer = pMap->GetTerrain()->GetLayer(k);
			if(((llFlags>>k)&1))
				m_comboLay.AddString(pLayer->GetName());
		}
	}else
	{
		int nLays,l;
		int n;

		for(n = 0; n<4; n++)
		{
			A3DTerrain2Block* block;

			for (int xx = 0; xx < 4; xx++)
				llFlags |= pMap->GetTerrain()->GetMaskModifier()->GetBlockFlags(h*4+n,c*4 + xx);

		}

		for( int k = 0; k< pMap->GetTerrain()->GetLayerNum(); k++)
		{
			CTerrainLayer* pLayer = pMap->GetTerrain()->GetLayer(k);
			if(((llFlags>>k)&1))
				m_comboLay.AddString(pLayer->GetName());
		}
	}

	if(m_comboLay.GetCount()>0) m_comboLay.SetCurSel(0);
}

bool CTerrainBlockViewDlg::CompareMask(CTerrainRender* pTR,A3DTerrain2Mask* pMask,int iLayer, CTerrainLayer *pLayer)
{
	ASSERT(pTR);
	CString pFile1,pSpecular1;
	A3DTexture *pTexture =	pMask->GetTexture(iLayer);
	if(pTexture) pFile1 =	pTexture->GetMapFile();
	//            pTexture =  pMask->GetSpecularMap(nIndex - pMask->GetFirstIndex());
	//if(pTexture) pSpecular1   =  pTexture->GetName()

	A3DTerrain2Mask::LAYER& Layer = pMask->GetLayer(iLayer);
	int nSize1 =			Layer.iMaskMapSize;
	BYTE proj1 =			Layer.byProjAxis;
	float u1 =				Layer.fUScale;
	float v1 =				Layer.fVScale;
	BYTE weight1 =			Layer.byWeight;

	CString pFile2			= pLayer->GetTextureFile();
	CString pSpecular2		= pLayer->GetSpecularMapFile();
	BYTE proj2				= pLayer->GetProjAxis();
	BYTE weight2			= pLayer->GetLayerWeight();
	float v2				= pLayer->GetScaleV();
	float u2				= pLayer->GetScaleU();

	if(_stricmp(pFile2, pFile1)==0)
	{
		if(proj1 == proj2)
		{
			if(u1==u2 && v1==v2)
			{
				if(weight1==weight2)
					return true;
			}
		}
	}
	return false;
}

bool CTerrainBlockViewDlg::IsAdded(CString str)
{
	int n = m_comboLay.GetCount();
	for(int i = 0; i < n; i++)
	{
		char txt[128];
		m_comboLay.GetLBText(i,txt);
		if(strcmp(txt,str)==0) return true;
	}
	return false;
}

void CTerrainBlockViewDlg::UpdateDlg()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
		m_MapView.SetShowSmallBlock(m_bShowSmallBlock);
		if(m_bShowSmallBlock)
			m_MapView.SetTileAndRow(pTR->GetBlockRowNum()*pTR->GetBlockColNum(),pTR->GetBlockColNum());
		else m_MapView.SetTileAndRow(pTR->GetBlockRowNum()*pTR->GetBlockColNum()/16,pTR->GetBlockColNum()/4);
		m_MapView.SetSelectedBlock(g_nCurrentTerrainBlock);
		FreshLayBox();

		// Recreate mask flag for each layer
		pMap->GetTerrain()->GetMaskModifier()->UpdateLayerFlags();
	}
	m_MapView.SetShowSmallBlock(m_bShowSmallBlock);
	m_MapView.InvalidateRect(NULL);
	m_MapView.pParent = this;
}

void CTerrainBlockViewDlg::OnSelchangeComboLay() 
{
	// TODO: Add your control notification handler code here
	int index = m_comboLay.GetCurSel();
	if(index!=-1)
	{
		char txt[128];
		m_comboLay.GetLBText(index,txt);
		//AUX_GetMainFrame()->GetToolTabWnd()->SetMaskLay(txt);????????
	}
}

void CTerrainBlockViewDlg::OnButtonBlockBlack() 
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	
	//float fCenterX,fCenterZ;
	//m_pBrush->GetCenter(fCenterX,fCenterZ);
	
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();

	int i, iNumLayer = pTerrain->GetLayerNum();
	int index = m_comboLay.GetCurSel();
	char txt[128];
	if(index!=-1) m_comboLay.GetLBText(index,txt);

	if(_stricmp(txt,"Default")==0) return;
	
	bool bFind = false;
	for (i=0; i < iNumLayer; i++)
	{
		CTerrainLayer* pLayer = pTerrain->GetLayer(i);
		if(strcmp(pLayer->GetName(),txt)==0) 
		{
			m_nCurrentLayer = i;
			bFind = true;
			break;
		}
	
	}
	if(!bFind) return; 
	// TODO: Add your control notification handler code here
	if(m_nCurrentLayer>pTerrain->GetLayerNum()-1 || m_nCurrentLayer<0)
		return;

	//update mask data
	CTerrainLayer *pLayer = pTerrain->GetLayer(m_nCurrentLayer);
	CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
	//if(pMaskModifier->HasModify())
	//	pMap->RecreateTerrainRender(false,false);
	if(!pMaskModifier->ChangeEditLayer(m_nCurrentLayer))
	{
		MessageBox("CTerrainBlockViewDlg::OnButtonBlockBlack(),Changed layer failed, please close editor and save map!");
		return;
	}
	if(pLayer)
	{
		CELBitmap *pMaskMap = pLayer->GetMaskBitmap();
		if(pMaskMap)
		{
			m_nMaskHeight = pMaskMap->GetHeight();
			m_nMaskWidth  = pMaskMap->GetWidth();
		}else
		{
			m_nMaskHeight = DEFMASK_SIZE;
			m_nMaskWidth = DEFMASK_SIZE;
		}
		
		
		if(g_nCurrentTerrainBlock==-1) return;
		CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
		if(pTR==NULL) return;
		A3DAABB aabb,sub;
		int h,c,n;
		A3DTerrain2Block* block = NULL; 
		if(g_Configs.bShowSmallBlock)
		{
			h = g_nCurrentTerrainBlock / pTR->GetBlockColNum();
			c = g_nCurrentTerrainBlock % pTR->GetBlockColNum();
			block = pTR->GetBlock(h,c);
			if(block==NULL) return;
			aabb = block->GetBlockAABB();
		}else
		{
			h = g_nCurrentTerrainBlock / (pTR->GetBlockColNum()/4);
			c = g_nCurrentTerrainBlock % (pTR->GetBlockColNum()/4);
			block = pTR->GetBlock(h*4,c*4);
			if(block==NULL) return;
			aabb = block->GetBlockAABB();
			for(n = 0; n < 4; n++)
			{
				block = pTR->GetBlock(h*4 + n,c*4 + 0);
				if(block==NULL) continue;
				sub = block->GetBlockAABB();
				aabb.Merge(sub);
				
				block = pTR->GetBlock(h*4 + n,c*4 + 1);
				if(block==NULL) continue;
				sub = block->GetBlockAABB();
				aabb.Merge(sub);
				
				block = pTR->GetBlock(h*4 + n,c*4 + 2);
				if(block==NULL) continue;
				sub = block->GetBlockAABB();
				aabb.Merge(sub);
				
				block = pTR->GetBlock(h*4 + n,c*4 + 3);
				if(block==NULL) continue;
				sub = block->GetBlockAABB();
				aabb.Merge(sub);
			}
		}
		ARectF rc;
		rc.left = aabb.Mins.x - 0.1f;
		rc.right = aabb.Maxs.x + 0.1f;
		rc.top = aabb.Maxs.z + 0.1f;
		rc.bottom = aabb.Mins.z - 0.1f;
		BeginWaitCursor();
		try
		{
			UpdateMask(rc);
			if(pMaskModifier->HasModify())
			{
				pMaskModifier->UpdateLayerFlags();
				pMap->SetModifiedFlag(true);
			}
		}catch (CUserException er) 
		{
			char msg[128];
			er.GetErrorMessage(msg,128);
			AfxMessageBox(msg,MB_ICONSTOP);
		}
		
			
		EndWaitCursor();
		FreshLayBox();
		m_MapView.InvalidateRect(NULL);
		
	}
}

void CTerrainBlockViewDlg::UpdateMask( ARectF& rc)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;

	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();

	if(!pHeightMap || !pTerrain) return;
	
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();
	float m_fTileSize = pTerrain->GetTileSize();
	float m_fRadius = static_cast<float>(g_Configs.iBrushRadius);

	ARect<float> updateRect = rc;
	
	float fMaskScaleX = (float)m_nMaskWidth/((m_rcTerrain.right - m_rcTerrain.left));
	float fMaskScaleZ = (float)m_nMaskHeight/((m_rcTerrain.top - m_rcTerrain.bottom));

	float fMaskGrid = (pHeightMap->iWidth - 1.0f) / m_nMaskWidth;
	float fMaskLen = fMaskGrid * m_fTileSize;
	
	ARectI maskRect;
	maskRect.left = (int)((updateRect.left - m_rcTerrain.left) / fMaskLen);
	maskRect.top = (int)(-(updateRect.top - m_rcTerrain.top) / fMaskLen);
	maskRect.right = (int)((updateRect.right - m_rcTerrain.left) / fMaskLen) + 1;
	maskRect.bottom = (int)(-(updateRect.bottom - m_rcTerrain.top) / fMaskLen) + 1;
	
	a_Clamp(maskRect.left, 0, m_nMaskWidth-1);
	a_Clamp(maskRect.top, 0, m_nMaskWidth-1);
	a_Clamp(maskRect.right, 0, m_nMaskWidth);
	a_Clamp(maskRect.bottom, 0, m_nMaskWidth);
	maskRect.Normalize();
	
	int NumX = maskRect.Width();
	int NumZ = maskRect.Height();
	
	//分配内存，等更新后释放
	int nSize = NumX*NumZ*sizeof(int); 
	int *pData = new int[NumX*NumZ];
	if(pData == NULL) return;
	memset(pData,-300,nSize);

	CMaskModifier *pMaskModifier = pTerrain->GetMaskModifier();
	if(pMaskModifier)
	{
		pMaskModifier->BeginModify();
		pMaskModifier->Modify(maskRect,pData);
		pMaskModifier->EndModify();
	}
	delete []pData;
}
