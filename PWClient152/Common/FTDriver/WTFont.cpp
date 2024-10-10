// WTFont.cpp: implementation of the WTFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WTFont.h"

CriticalSession WTFont::CSForWindowstype;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WTFont::WTFont()
{
	m_iWidth		= 0;
	m_iHeight		= 0;
	m_iAlign		= 0;
	m_iBPP			= 0;
	m_iBytePP		= 0;
	m_iPitch		= 0;
	m_pMem			= NULL;
	m_pBitmap		= NULL;
	m_hBitmap		= NULL;
	m_hMemDC		= NULL;
	m_pBmpInfo		= NULL;
	m_iUnitPerEM	= 0;
	m_iUnit			= 0;
	m_bItalic		= FALSE;
	m_hFont			= NULL;
	m_hPen			= NULL;
	m_iOutlineWidth	= 0;
}

WTFont::~WTFont()
{
}

void WTFont::Free()
{
	CriticalSession::Scope cs(CSForWindowstype);
	if(m_hMemDC)
	{
		DeleteDC(m_hMemDC);
	}
	if(m_hBitmap)
	{
		DeleteObject(m_hBitmap);
	}
	m_pMem=NULL;
	m_pBitmap=NULL;
	if(m_pBmpInfo)
	{
		free(m_pBmpInfo);
	}
	if(m_hFont)
	{
		DeleteObject(m_hFont);
	}
	if(m_hPen)
	{
		DeleteObject(m_hPen);
	}
	m_hBitmap=NULL;
	m_hMemDC=NULL;
	m_pBmpInfo=NULL;
}

bool WTFont::CreateFontIndirect(const FontParam &fontParam)
{
	HDC tempdc;
	void* bmpmem;
	Free();
	m_iAlign=4;
	m_iBPP=8;
	m_iUnitPerEM= (int)((fontParam.PointHeight*fontParam.VertDPI/72)/10.0+0.5);
	m_iUnit = m_iUnitPerEM+2*fontParam.OutlineWidth;
	m_iOutlineWidth = fontParam.OutlineWidth;
	bool bBold = FALSE,bItalic = FALSE;
	int iFW = FW_REGULAR;
	if(fontParam.FontStyle&STYLE_BOLD)
	{
		iFW = FW_BLACK;
	}
	if(fontParam.FontStyle&STYLE_ITALIC)
	{
		m_bItalic = TRUE;
	}

	tempdc=GetDC(NULL);
	m_hMemDC=CreateCompatibleDC(tempdc);
	ReleaseDC(NULL,tempdc);
	//AddFontResourceEx(fontParam.FontFile,FR_PRIVATE,0);	//Must #define _WIN32_WINNT 0x0500 .But Windows 95/98/Me: Unsupported.
	AddFontResource(fontParam.FontFile);
	m_hFont = CreateFontW(
        -m_iUnitPerEM,	//字体的逻辑高度
        0,				//逻辑平均字符宽度
        0,				//与水平线的角度
        0,				//基线方位角度
        iFW,			//字形：常规
        m_bItalic,		//字形：斜体
        FALSE,			//字形：下划线
        FALSE,			//字形：删除线
        GB2312_CHARSET,          //字符集
        OUT_DEFAULT_PRECIS,      //输出精度
        CLIP_DEFAULT_PRECIS,     //剪截精度
        PROOF_QUALITY,           //输出品质
        FIXED_PITCH | FF_MODERN, //倾斜度
        fontParam.FaceName       //字体
    );
	
	m_hPen = CreatePen(
	  PS_SOLID,							// pen style
	  fontParam.OutlineWidth,			// pen width
	  RGB(255,255,255)					// pen color
	);
    SelectObject(m_hMemDC, m_hPen);		//	For outline
	SelectObject(m_hMemDC,m_hFont);
	SetTextColor(m_hMemDC, 0xffffff);
	SetBkMode(m_hMemDC,TRANSPARENT);	//设置背景模式
	GetTextMetrics (m_hMemDC, &m_tmText) ;
	m_iWidth = m_tmText.tmMaxCharWidth;
	m_iHeight=m_tmText.tmHeight;
	
	m_iBytePP=1;	
	
	m_iPitch=m_iWidth*m_iBytePP;
	if(m_iPitch%m_iAlign)
	{
		m_iPitch+=m_iAlign-(m_iPitch%m_iAlign);
	}
	m_iPitch=-m_iPitch;
	int i;
	m_pBmpInfo=(BITMAPINFO*)malloc(sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256);
	for(i=0;i<256;i++)//Insert gray-scale palette. 
	{
		m_pBmpInfo->bmiColors[i].rgbBlue=i;
		m_pBmpInfo->bmiColors[i].rgbRed=i;
		m_pBmpInfo->bmiColors[i].rgbGreen=i;
		m_pBmpInfo->bmiColors[i].rgbReserved=0;
	}

	m_pBmpInfo->bmiHeader.biSize=sizeof(m_pBmpInfo->bmiHeader);
	m_pBmpInfo->bmiHeader.biWidth=m_iWidth;
	m_pBmpInfo->bmiHeader.biHeight=m_iHeight;
	m_pBmpInfo->bmiHeader.biPlanes=1;
	m_pBmpInfo->bmiHeader.biBitCount=8;
	m_pBmpInfo->bmiHeader.biCompression=BI_RGB;
	m_pBmpInfo->bmiHeader.biClrUsed=256;
	m_pBmpInfo->bmiHeader.biClrImportant=256;
	m_pBmpInfo->bmiHeader.biSizeImage=(-m_iPitch)*m_iHeight;

	m_pBmpInfo->bmiHeader.biXPelsPerMeter=72;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter=72;
	m_pBmpInfo->bmiHeader.biClrUsed=0;
	m_pBmpInfo->bmiHeader.biClrImportant=0;
	m_hBitmap=CreateDIBSection(m_hMemDC,m_pBmpInfo,DIB_RGB_COLORS,&bmpmem,NULL,0);
	SelectObject(m_hMemDC,m_hBitmap);
	m_pMem=bmpmem;
	m_pBitmap=(unsigned char*)bmpmem+(m_iHeight-1)*(-m_iPitch);
	
	return true;
}

bool WTFont::CreatePointFont(const char *fontFile, int fontFileLength, UINT point, int fontStyle, int nOutlineWidth,const wchar_t *strFaceName)
{
	FontParam fontParam;
	fontParam.FontFile			= fontFile;
	fontParam.FontFileLength	= fontFileLength;
	fontParam.FaceIndex			= 0;
	fontParam.FontStyle			= fontStyle;
	fontParam.PointHeight		= point;
	fontParam.PointWidth		= point;
	fontParam.VertDPI			= 96;
	fontParam.HorzDPI			= 96;
	fontParam.OutlineWidth		= nOutlineWidth;
	fontParam.FaceName			= strFaceName;

	return CreateFontIndirect(fontParam);
}

void WTFont::GetMaxSize(int& cx, int& cy)
{
	cx = m_iUnit;
	cy = m_iUnit;
}

BOOL WTFont::GetCharExtent(wchar_t ch, LPFTDSIZE lpOffset, LPFTDSIZE lpSize, LPFTDSIZE lpBmpSize)
{
	CriticalSession::Scope cs(CSForWindowstype);
	MAT2   mat2;         
	mat2.eM11.value=1;   
	mat2.eM11.fract=0;   
	mat2.eM12.value=0;   
	mat2.eM12.fract=0;   
	mat2.eM21.value=0;   
	mat2.eM21.fract=0;   
	mat2.eM22.value=1;   
	mat2.eM22.fract=0; 
	GLYPHMETRICS GlyphMetrics;
	if(GetGlyphOutlineW(
		  m_hMemDC,         // handle to DC
		  ch,				// character to query
		  GGO_METRICS,      // data format
		  &GlyphMetrics,	// glyph metrics
		  0,				// size of data buffer
		  NULL,				// data buffer
		  &mat2				// transformation matrix
		)==GDI_ERROR)
		return FALSE;
	ABC abc;
	GetCharABCWidthsW(
	  m_hMemDC,         // handle to DC
	   ch,				// first character in range
	   ch,				// last character in range
	  &abc			// array of character widths
	);
	SIZE size;
	::GetTextExtentPoint32W(m_hMemDC, &ch, 1,&size );
	lpOffset->cx = abc.abcA-m_iOutlineWidth;
	lpOffset->cy = m_tmText.tmAscent-GlyphMetrics.gmptGlyphOrigin.y-m_iOutlineWidth;

	lpBmpSize->cx = GlyphMetrics.gmBlackBoxX+2*m_iOutlineWidth;
	lpBmpSize->cx = lpBmpSize->cx<m_iWidth?lpBmpSize->cx:m_iWidth;
	lpBmpSize->cy = GlyphMetrics.gmBlackBoxY+2*m_iOutlineWidth;
	lpBmpSize->cy = lpBmpSize->cy<m_iHeight?lpBmpSize->cy:m_iHeight;
	lpSize->cx = size.cx+2*m_iOutlineWidth;
	lpSize->cy = m_tmText.tmHeight; 
	return TRUE;
}

BOOL WTFont::GetTextExtentExPoint(const wchar_t *lpszStr, int cchString,  int nMaxExtent, LPINT lpnFit, LPINT alpDx, LPFTDSIZE lpSize)
{
	CriticalSession::Scope cs(CSForWindowstype);
	SIZE size;
	//	UNICODE
	int outline = (m_iUnit-m_iUnitPerEM)*cchString;
	::GetTextExtentPoint32W(m_hMemDC, lpszStr, cchString,&size );
	lpSize->cx = size.cx+outline;
	lpSize->cy = size.cy;
	return TRUE;
}

BOOL WTFont::TextOut(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, int nWidth, int nHeight)
{	
	CriticalSession::Scope cs(CSForWindowstype);
	FTDSIZE size,offsize,bmpsize;
	ZeroMemory(pBitmap,nWidth*nHeight);
	int h,w;
	const wchar_t* pch;
	int ipitch;
	BYTE* pdit,* psrc,* ptmdt,*ptmsc;
	int xoff =0,ystart;
	int iforspace;
	int totals,totald,xtotald,ytotald,xtotals,ytotals;	
	xoff = nXStart;ystart = nYStart; 
	for (int i = 0; i < cbString; i++)
	{		
		pch = &lpString[i];
#define		WTFONT_DRAW_TEXT
#include	"WTFont.inl"
#undef		WTFONT_DRAW_TEXT
		xoff += size.cx;
	}
	return TRUE;
}


BOOL WTFont::TextOutWithOutline(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, BYTE *pOutline, int nWidth, int nHeight)
{	
	CriticalSession::Scope cs(CSForWindowstype);
	FTDSIZE size,offsize,bmpsize;
	ZeroMemory(pBitmap,nWidth*nHeight);
	ZeroMemory(pOutline,nWidth*nHeight);
	int h,w;
	const wchar_t* pch;
	int ipitch;
	BYTE* pdit,* psrc,* ptmdt,*ptmsc;
	int xoff =0,ystart;
	int iforspace;
	int totals,totald,xtotald,ytotald,xtotals,ytotals;
	xoff = nXStart;ystart = nYStart; 	
	for (int i = 0; i < cbString; i++)
	{		
		pch = &lpString[i];
#define		WTFONT_DRAW_TEXT
#include	"WTFont.inl"
#define		WTFONT_DRAW_OUTLINE
#include	"WTFont.inl"
#undef		WTFONT_DRAW_TEXT
#undef		WTFONT_DRAW_OUTLINE
		xoff += size.cx;
	}
	return TRUE;
}


BOOL WTFont::DrawChar(wchar_t ch, BYTE* pBitmap, int nWidth, int nHeight)
{
	CriticalSession::Scope cs(CSForWindowstype);
	BYTE* pdit,* psrc,* ptmdt,*ptmsc;
	FTDSIZE size,offsize,bmpsize;
	wchar_t* pch = &ch;
	int h,w;
	int ipitch;
	int iforspace;
	int totals,totald,xtotald,ytotald,xtotals,ytotals;	
	ZeroMemory(pBitmap,nWidth*nHeight);
#include	"WTFont.inl"
	return TRUE;
}

BOOL WTFont::DrawCharWithOutline(wchar_t ch, BYTE* pBitmap, BYTE* pOutline, int nWidth, int nHeight)
{
	CriticalSession::Scope cs(CSForWindowstype);
	BYTE* pdit,* psrc,* ptmdt,*ptmsc;
	FTDSIZE size,offsize,bmpsize;
	wchar_t* pch = &ch;
	int h,w;
	int ipitch;
	int iforspace;
	int totals,totald,xtotald,ytotald,xtotals,ytotals;	
	ZeroMemory(pBitmap,nWidth*nHeight);
	ZeroMemory(pOutline,nWidth*nHeight);
#include	"WTFont.inl"
#define		WTFONT_DRAW_OUTLINE
#include	"WTFont.inl"
#undef		WTFONT_DRAW_OUTLINE
	return TRUE;
}

void WTFont::Release()
{
	Free();	
	delete this;
}

