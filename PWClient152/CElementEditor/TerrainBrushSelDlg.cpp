// TerrainBrushSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "TerrainBrushSelDlg.h"
#include "Global.h"
#include "Bitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerrainBrushSelDlg dialog


CTerrainBrushSelDlg::CTerrainBrushSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainBrushSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainBrushSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pData = 0;
}


void CTerrainBrushSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainBrushSelDlg)
	DDX_Control(pDX, IDC_LIST_PATH, m_listPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainBrushSelDlg, CDialog)
	//{{AFX_MSG_MAP(CTerrainBrushSelDlg)
	ON_LBN_SELCHANGE(IDC_LIST_PATH, OnSelchangeListPath)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainBrushSelDlg message handlers

BOOL CTerrainBrushSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_listPath.AddString("默认刷子");
	
	CString strFilter;
	CFileFind Find;
	strFilter.Format("%sBrush\\*.*",g_szWorkDir);
	BOOL bResult = Find.FindFile(strFilter);
	int nSel = 0;
	while (bResult)
	{
		bResult = Find.FindNextFile();
		if (Find.IsDots() || Find.IsSystem() || Find.IsDirectory())
			continue;
		if(Find.GetFileName() == m_strBrushPath)
			nSel = m_listPath.AddString(Find.GetFileName());
		else m_listPath.AddString(Find.GetFileName());
	}

	m_listPath.SetCurSel(nSel);
	
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_BITMAP)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	if (!m_BitmapView.Create(this, rcWnd))
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}
	RecreateBitmap();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainBrushSelDlg::OnOK()
{
	int sel = m_listPath.GetCurSel();
	if(sel!=-1) m_listPath.GetText(sel,m_strBrushPath);
	
	CDialog::OnOK();
}

void CTerrainBrushSelDlg::OnSelchangeListPath() 
{
	// TODO: Add your control notification handler code here
	RecreateBitmap();
	Draw();
}
void CTerrainBrushSelDlg::RecreateBitmap()
{
	int sel = m_listPath.GetCurSel();
	if(sel==-1) return;
	CString path;
	m_listPath.GetText(sel,path);
	if(m_pData)
	{
		delete []m_pData;
		m_pData = 0;
	}

	if(path=="默认刷子") 
	{
		m_BitmapView.SetBitMap(NULL,0,0);
		return;
	}
	
	CString temp;
	temp.Format("%sBrush\\%s",g_szWorkDir,path);
	if(!temp.IsEmpty())
	{
		HBITMAP hBitmap = (HBITMAP)LoadImage(NULL,temp,IMAGE_BITMAP,LR_DEFAULTSIZE ,LR_DEFAULTSIZE,LR_LOADFROMFILE);
		if(hBitmap == NULL) 
		{
			m_BitmapView.SetBitMap(NULL,0,0);
			return;
		}
		BITMAP 	bmp;
		GetObject(hBitmap,sizeof(BITMAP),&bmp);
		int nWidth = bmp.bmWidth;
		int nHeight = bmp.bmHeight;
		
		COLORREF *pData = new COLORREF[bmp.bmWidth*bmp.bmHeight*(bmp.bmBitsPixel/8)];
		CBitmap bt;
		bt.Attach(hBitmap);
		bt.GetBitmapBits(bmp.bmWidth*bmp.bmHeight*(bmp.bmBitsPixel/8),pData);  
		
		
		m_pData = new BYTE[nHeight*nWidth];
		for (int i=0; i < nHeight; i++)
		{
		
			for (int j=0; j < nWidth; j++)
			{
				COLORREF color = pData[i*nWidth + j];
				int ir = _getRed(color);
				int ig = _getGreen(color);
				int ib = _getBlue(color);
				m_pData[nWidth*i + j] = (int)(0.33f*ir + 0.53f*ig + 0.13f*ib);
			}
		}
		
		m_BitmapView.SetBitMap(m_pData,nWidth,nHeight);
		delete []pData;
	}
	
}

void CTerrainBrushSelDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
}

void CTerrainBrushSelDlg::Draw()
{

}

void CTerrainBrushSelDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	if(	m_pData != 0) 
	{
		delete []m_pData;
		m_pData = NULL;
	}	// TODO: Add your message handler code here
}












/////////////////////////////////


CBitmapView::CBitmapView()
{
	m_pData = 0;
}

CBitmapView::~CBitmapView()
{

}

BEGIN_MESSAGE_MAP(CBitmapView, CBackBufWnd)
	//{{AFX_MSG_MAP(CMapPieceView)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create window
bool CBitmapView::Create(CWnd* pParent, const RECT& rc)
{
	//	Register class name
	CString strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
							LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)),
							NULL);//(HBRUSH)GetStockObject(WHITE_BRUSH));
	
	if (!CBackBufWnd::Create(strClassName, "CurveFilterView", 
						WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						rc, pParent, 0))
		return false;
	return true;
}

void CBitmapView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawBackBuffer(NULL);
	Flip(dc.m_hDC, NULL);
	
}


//	Draw back buffer
bool CBitmapView::DrawBackBuffer(RECT* lprc)
{
	//	Clear background
	ClearBackBuffer(RGB(64,64,64));
	DrawBackground();
	return true;
}

BOOL CBitmapView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////

void CBitmapView::DrawBackground()
{
	if(m_pData==NULL) return;

	RECT rect;
	GetClientRect(&rect);
	
	float sx = rect.right/(float)m_nWidth;
	float sy = rect.bottom/(float)m_nHeight;

	for(int h = 0; h < rect.bottom; ++h)
	{
		for( int w = 0; w < rect.right; ++w)
		{
			int xx = w/sx;
			int yy = h/sy;
			int col = m_pData[yy*m_nWidth + xx];
			::SetPixel(m_hMemDC,w,h,RGB(col,col,col));
		}
	}
	
}

