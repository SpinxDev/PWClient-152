// MapGrid.cpp : implementation file
//

#include "Global.h"
#include "DlgMapGrid.h"

#include "AF.h"
#include "AC.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CMapGrid
//	
///////////////////////////////////////////////////////////////////////////

CMapGrid::CMapGrid()
{
	m_iNumRow		= 0;
	m_iNumCol		= 0;
	m_iNumTile		= 0;
	m_iTileSize		= 64;
	m_iOffsetX		= 0;
	m_iOffsetY		= 0;
	m_iNumVisRow	= 0;
	m_iNumVisCol	= 0;
	m_pSelectPolicy = NULL;
}

CMapGrid::~CMapGrid()
{
}

BEGIN_MESSAGE_MAP(CMapGrid, CBackBufWnd)
	//{{AFX_MSG_MAP(CMapGrid)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create window
bool CMapGrid::Create(CWnd* pParent, const RECT& rc, int iTileSize)
{
	//	Register class name
	CString strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
							LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)),
							NULL);//(HBRUSH)GetStockObject(WHITE_BRUSH));
	
	if (!CBackBufWnd::Create(strClassName, "MapGrid", 
						WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_HSCROLL | WS_VSCROLL,
						rc, pParent, 0))
		return false;

	RECT rcClient;
	GetClientRect(&rcClient);

	m_iTileSize		= iTileSize;
	m_iNumVisRow	= (rcClient.bottom - SIZE_RULERTHICK) / iTileSize;
	m_iNumVisCol	= (rcClient.right - SIZE_RULERTHICK) / iTileSize;

	ShowScrollBar(SB_BOTH, TRUE);

	//	Set initial state
	SetTileAndRow(ProjList(), 0, false);

	return true;
}

//	Set tile number and column number
bool CMapGrid::SetTileAndRow(const ProjList &projList, int iNumCol, bool bDefaultCanSelect)
{
	int iNumTile = (int)projList.size();
	if (!iNumTile || !iNumCol)
	{
		m_iNumRow	= 0;
	}
	else
	{
		m_iNumRow	= (iNumTile + iNumCol - 1) / iNumCol;
	}
	m_iNumCol	= iNumCol;
	m_iNumTile	= iNumTile;
	m_projList	= projList;

	m_pSelectPolicy->Reset(m_iNumRow, m_iNumCol, bDefaultCanSelect);

	InvalidateRect(NULL);

	//	Reset scroll bar
	ResetScrollBar();

	return true;
}

void CMapGrid::SetSelectPolicy(CMapGridSelectPolicy *policy)
{
	m_pSelectPolicy = policy;
}

void CMapGrid::ResetSelect()
{
	m_pSelectPolicy->ClearSelect();
	m_pSelectPolicy->ClearSelectEnable(false);
	InvalidateRect(NULL);
}

void CMapGrid::EnableSelect(int iRow, int iCol, bool bEnable)
{
	m_pSelectPolicy->EnableSelect(iRow, iCol, bEnable);
	InvalidateRect(NULL);
}

void CMapGrid::EnableSelect(int iTileIndex, bool bEnable)
{
	if (m_iNumCol > 0)
	{
		m_pSelectPolicy->EnableSelect(iTileIndex, bEnable);
		InvalidateRect(NULL);
	}
}
bool CMapGrid::GetProjName(int iProj,CString& name)
{ 
	if(iProj<0 || iProj>= m_projList.size()) return false;
	
	name = m_projList[iProj];
	
	return true;
}

//	Reset scroll bar
void CMapGrid::ResetScrollBar()
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

void CMapGrid::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawBackBuffer(NULL);
	Flip(dc.m_hDC, NULL);
	
	// Do not call CBackBufWnd::OnPaint() for painting messages
}

//	Draw back buffer
bool CMapGrid::DrawBackBuffer(RECT* lprc)
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
	HBRUSH hBrush, hSelBrush, hEmptyBrush, hDisableBrush;
	hBrush = AUX_CreateBrush(BS_SOLID, RGB(255, 255, 0), 0);
	hSelBrush = AUX_CreateBrush(BS_SOLID, RGB(255, 0, 0), 0);
	hEmptyBrush = AUX_CreateBrush(BS_SOLID, RGB(220, 220, 220), 0);
	hDisableBrush = AUX_CreateBrush(BS_SOLID, RGB(64, 64, 64), 0);

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
			if (IsSelected(iCount))
			{
				col = ::SetBkColor(m_hMemDC, RGB(255, 0, 0));
				::FillRect(m_hMemDC, &rc, hSelBrush);
			}
			else if (iCount < m_iNumTile)
			{
				if (CanSelect(iCount)){
					::FillRect(m_hMemDC, &rc, hBrush);
					col = ::SetBkColor(m_hMemDC,  RGB(255, 255, 0));
				}else{
					::FillRect(m_hMemDC, &rc, hDisableBrush);
					col = ::SetBkColor(m_hMemDC,  RGB(64, 64, 64));
				}
			}
			else
			{
				col = ::SetBkColor(m_hMemDC, RGB(220, 220, 220));
				::FillRect(m_hMemDC, &rc, hEmptyBrush);
			}

			CString strProjName = m_projList[iCount];
			::DrawText(m_hMemDC, strProjName, strProjName.GetLength(), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			::SetBkColor(m_hMemDC, col);

			rc.left += m_iTileSize;
		}
	}

	::DeleteObject(hBrush);
	::DeleteObject(hSelBrush);
	::DeleteObject(hEmptyBrush);
	::DeleteObject(hDisableBrush);

	return true;
}

void CMapGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetFocus();
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

void CMapGrid::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SetFocus();
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

BOOL CMapGrid::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CMapGrid::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	SetCapture();
	if (m_pSelectPolicy->OnLButtonDown(TestTile(point.x, point.y), nFlags)){
		InvalidateRect(NULL);
	}
	CBackBufWnd::OnLButtonDown(nFlags, point);
}

void CMapGrid::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_pSelectPolicy->OnMouseMove(TestTile(point.x, point.y), nFlags)){
		InvalidateRect(NULL);
	}
	CBackBufWnd::OnMouseMove(nFlags, point);
}

void CMapGrid::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	CBackBufWnd::OnLButtonUp(nFlags, point);
}

int CMapGrid::TestTile(int x, int y)
{
	if (!m_iNumTile || x <= SIZE_RULERTHICK || y <= SIZE_RULERTHICK)
		return -1;

	int iCol = m_iOffsetX + (x - SIZE_RULERTHICK) / m_iTileSize;
	if (iCol >= m_iNumCol)
		return -1;

	int iRow = m_iOffsetY + (y - SIZE_RULERTHICK) / m_iTileSize;
	if (iRow >= m_iNumRow)
		return -1;

	return GetTile(iRow, iCol);
}

void CMapGrid::GetRowCol(int iTileIndex, int &iRow, int &iCol)
{
	if (m_iNumCol > 0){
		iRow = iTileIndex / m_iNumCol;
		iCol = iTileIndex % m_iNumCol;
	}
}

int CMapGrid::GetTile(int iRow, int iCol)
{
	return iRow * m_iNumCol + iCol;
}

bool CMapGrid::IsSelected(int iTileIndex)
{
	return m_pSelectPolicy->IsSelected(iTileIndex);
}

bool CMapGrid::CanSelect(int iTileIndex)
{
	return m_pSelectPolicy->CanSelect(iTileIndex);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CMapGridSelectPolicy
//	
///////////////////////////////////////////////////////////////////////////
CMapGridSelectPolicy::CMapGridSelectPolicy()
: m_iNumRow(0)
, m_iNumCol(0)
, m_iNumTile(0)
{
}

void CMapGridSelectPolicy::Reset(int nRow, int nCol, bool bEnableSelect)
{
	m_iNumRow = nRow;
	m_iNumCol = nCol;
	m_iNumTile = m_iNumRow * m_iNumCol;
	ClearSelectEnable(bEnableSelect);
	ClearSelect();
	ResetMore();
}

void CMapGridSelectPolicy::ClearSelectEnable(bool bEnable)
{
	m_bEnableSelect.clear();
	m_bEnableSelect.resize(m_iNumTile, bEnable);
	ClearSelectEnableMore(bEnable);
}

void CMapGridSelectPolicy::GetRowCol(int iTileIndex, int &iRow, int &iCol)const
{
	if (IsValid(iTileIndex)){
		iRow = iTileIndex / m_iNumCol;
		iCol = iTileIndex % m_iNumCol;
	}
}

bool CMapGridSelectPolicy::IsValid(int iRow, int iCol)const
{
	return iRow >= 0 && iRow < GetRowNum();
}

bool CMapGridSelectPolicy::IsValid(int iTileIndex)const
{
	return iTileIndex >= 0 && iTileIndex < m_iNumTile;
}

bool CMapGridSelectPolicy::CanSelect(int iTileIndex)
{
	return IsValid(iTileIndex) && m_bEnableSelect[iTileIndex];
}

void CMapGridSelectPolicy::EnableSelect(int iRow, int iCol, bool bEnable)
{
	if (IsValid(iRow, iCol)){
		m_bEnableSelect[iRow * m_iNumCol + iCol] = bEnable;
	}
}

void CMapGridSelectPolicy::EnableSelect(int iTileIndex, bool bEnable)
{
	if (IsValid(iTileIndex)){
		int iRow(0), iCol(0);
		GetRowCol(iTileIndex, iRow, iCol);
		EnableSelect(iRow, iCol, bEnable);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CRangedMapGridSelectPolicy
//	
///////////////////////////////////////////////////////////////////////////
CRangedMapGridSelectPolicy::CRangedMapGridSelectPolicy()
: m_iStartTile(-1)
, m_iEndTile(-1)
{
}

bool CRangedMapGridSelectPolicy::IsSelected(int iTileIndex)
{
	if (m_iStartTile >= 0 && m_iEndTile >= 0 && iTileIndex >= 0 && iTileIndex < GetTileNum()){
		int srow(0), scol(0), erow(0), ecol(0), trow(0), tcol(0);
		GetRowCol(m_iStartTile, srow, scol);
		GetRowCol(m_iEndTile, erow, ecol);
		GetRowCol(iTileIndex, trow, tcol);
		if (erow < srow){
			std::swap(erow, srow);
		}
		if (ecol < scol){
			std::swap(ecol, scol);
		}
		return trow >= srow && trow <= erow
			&& tcol >= scol && tcol <= ecol;
	}
	return false;
}

void CRangedMapGridSelectPolicy::ClearSelect()
{
	m_iStartTile = m_iEndTile = -1;
}

bool CRangedMapGridSelectPolicy::OnLButtonDown(int iTileMouseOn, UINT nFlags)
{
	bool bRet(false);
	if ((nFlags & MK_SHIFT) && m_iStartTile >= 0){
		bRet = UpdateSelectRange(iTileMouseOn);
	}else{
		m_iStartTile = m_iEndTile = -1;
		if (CanSelect(iTileMouseOn)){
			m_iStartTile = m_iEndTile = iTileMouseOn;
			bRet = true;
		}
	}
	return bRet;
}

bool CRangedMapGridSelectPolicy::OnMouseMove(int iTileMouseOn, UINT nFlags)
{
	bool bRet(false);
	if (m_iStartTile >= 0 && (nFlags & MK_LBUTTON)){
		bRet = UpdateSelectRange(iTileMouseOn);
	}
	return bRet;
}

bool CRangedMapGridSelectPolicy::UpdateSelectRange(int iTileMouseOn)
{	
	if (iTileMouseOn >= 0){
		//	检查由 m_iStartTile 和 iTileMouseOn 构成的矩形范围内是否都是可选择的
		int start(m_iStartTile), end(iTileMouseOn);
		int srow(0), scol(0), erow(0), ecol(0);
		GetRowCol(start, srow, scol);
		GetRowCol(end, erow, ecol);
		if (erow < srow){
			std::swap(erow, srow);
		}
		if (ecol < scol){
			std::swap(ecol, scol);
		}
		bool bInvalid(false);
		for (int i = srow; i <= erow; ++ i)
		{
			for (int j = scol; j <= ecol; ++ j)
			{
				int index = i * GetColNum() + j;
				if (!CanSelect(index)){
					bInvalid = true;
					break;
				}
			}
			if (bInvalid){
				break;
			}
		}
		//	bInvalid 为 false 时，更新选中范围
		if (!bInvalid){
			m_iEndTile = iTileMouseOn;
			return true;
		}
	}
	return false;
}

bool CRangedMapGridSelectPolicy::GetSelect(int &iRow1, int &iRow2, int &iCol1, int &iCol2)
{
	if (m_iStartTile >= 0 && m_iEndTile >= 0)
	{
		GetRowCol(m_iStartTile, iRow1, iCol1);
		GetRowCol(m_iEndTile, iRow2, iCol2);
		if (iRow2 < iRow1){
			std::swap(iRow1, iRow2);
		}
		if (iCol2 < iCol1){
			std::swap(iCol1, iCol2);
		}
		return true;
	}
	return false;
}

void CRangedMapGridSelectPolicy::ResetMore()
{
	m_iStartTile = m_iEndTile = -1;
}

void CRangedMapGridSelectPolicy::ClearSelectEnableMore(bool bEnable)
{
	m_iStartTile = m_iEndTile = -1;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CSeperateMapGridSelectPolicy
//	
///////////////////////////////////////////////////////////////////////////

bool CSeperateMapGridSelectPolicy::IsSelected(int iTileIndex)
{
	return IsValid(iTileIndex) && m_bSelected[iTileIndex];
}

void CSeperateMapGridSelectPolicy::ClearSelect()
{	
	m_bSelected.clear();
	m_bSelected.resize(GetTileNum(), false);
}

bool CSeperateMapGridSelectPolicy::OnLButtonDown(int iTileMouseOn, UINT nFlags)
{
	bool bRet(false);
	if (CanSelect(iTileMouseOn)){
		m_bSelected[iTileMouseOn] = !m_bSelected[iTileMouseOn];
		bRet = true;
	}
	return bRet;
}

bool CSeperateMapGridSelectPolicy::OnMouseMove(int iTileMouseOn, UINT nFlags)
{
	return false;
// 	bool bRet(false);
// 	if ((nFlags & MK_LBUTTON) && CanSelect(iTileMouseOn)){
// 		m_bSelected[iTileMouseOn] = !m_bSelected[iTileMouseOn];
// 		bRet = true;
// 	}
// 	return bRet;
}

void CSeperateMapGridSelectPolicy::ResetMore()
{
	ClearSelect();
}

void CSeperateMapGridSelectPolicy::ClearSelectEnableMore(bool bEnable)
{
	ClearSelect();
}

void CSeperateMapGridSelectPolicy::SelectAll(bool bSelect)
{
	if (!bSelect){
		ClearSelect();
	}else{
		for (int i(0); i < (int)m_bSelected.size(); ++ i)
		{
			if (CanSelect(i)){
				m_bSelected[i] = true;
			}
		}
	}
}
