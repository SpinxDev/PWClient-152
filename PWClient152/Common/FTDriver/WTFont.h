// WTFont.h: interface for the WTFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WTFONT_H__AC8D02F8_D759_4FF6_88CE_F79793A4E67B__INCLUDED_)
#define AFX_WTFONT_H__AC8D02F8_D759_4FF6_88CE_F79793A4E67B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FTInterface.h"
#include "CriticalSession.h"

class WTFont : public IFTFont  
{
protected:
	int					m_iWidth;
	int					m_iHeight;
	int					m_iAlign;
	int					m_iBPP;
	int					m_iBytePP;
	int					m_iPitch;
	HDC					m_hMemDC;
	void*				m_pMem;
	unsigned char*		m_pBitmap;
	HBITMAP 			m_hBitmap;
	BITMAPINFO* 		m_pBmpInfo;
	int					m_iUnit;
	int					m_iUnitPerEM;
	int					m_bItalic;
	HFONT				m_hFont;
	HPEN				m_hPen;
	TEXTMETRIC			m_tmText;
	int					m_iOutlineWidth;
	static CriticalSession CSForWindowstype;
public:
	WTFont();
	virtual ~WTFont();

	// IFTFont interface implement
	void GetMaxSize(int& cx, int& cy);
	BOOL GetCharExtent(wchar_t ch, LPFTDSIZE lpOffset, LPFTDSIZE lpSize, LPFTDSIZE lpBmpSize);
	BOOL GetTextExtentExPoint(const wchar_t *lpszStr, int cchString,  int nMaxExtent, LPINT lpnFit, LPINT alpDx, LPFTDSIZE lpSize);
	BOOL TextOut(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, int nWidth, int nHeight);
	BOOL TextOutWithOutline(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, BYTE *pOutline, int nWidth, int nHeight);
	BOOL DrawChar(wchar_t ch, BYTE* pBitmap, int nWidth, int nHeight);
	BOOL DrawCharWithOutline(wchar_t ch, BYTE* pBitmap, BYTE* pOutline, int nWidth, int nHeight);
	void Release();


	//	Normal funcation
	void Free();
	bool CreateFontIndirect(const FontParam &fontParam);
	bool CreatePointFont(const char *fontFile, int fontFileLength, UINT point, int fontStyle, int nOutlineWidth,const wchar_t *strFaceName = NULL);
};

#endif // !defined(AFX_WTFONT_H__AC8D02F8_D759_4FF6_88CE_F79793A4E67B__INCLUDED_)
