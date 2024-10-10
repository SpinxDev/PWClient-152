// FTFont.h: interface for the FTFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTFONT_H__38A376F6_60BB_4E79_BA03_73283A5B1EC6__INCLUDED_)
#define AFX_FTFONT_H__38A376F6_60BB_4E79_BA03_73283A5B1EC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include FT_FREETYPE_H 
#include <freetype/ftcache.h>

#include "MyFTCache.h"
#include "FTInterface.h"
#include "CriticalSession.h"

class FTManager;
class FTGlyphCache;
typedef struct FT_BitmapGlyphRec_*  FT_BitmapGlyph;

class FTFont : public IFTFont
{
protected:
	My_FaceID MyFaceID;
	FTC_Scaler FTCScaler;
	FTManager * TheFTManager;
	int UnicodeCMapIndex;
	FTGlyphCache * TheGlyphCache;
	UINT OutlineWidth;
	int Height;
	int Ascender;
	int CacheSize;
	static CriticalSession CSForFreetype;
public:
	int GetUnicodeCMapIndex(FT_Face ftFace);
	My_FaceID GetMyFaceID() { return MyFaceID; }
	FTC_Scaler GetFTCScaler() { return FTCScaler; }
	FT_Face GetFTFace();
	FT_Size GetFTSize();

	void Destory();
	FTFont(FTManager *pFTManager, int nCacheSize);
	virtual ~FTFont();

	bool CreateFontIndirect(const FontParam &fontParam);
	bool CreatePointFont(const char *fontFile, int fontFileLength, UINT fontPoint, int fontStyle, int nOutlineWidth);

	FT_BitmapGlyph RenderChar(UINT c, bool bRenderAsOutline);
	int FontUnitToPixel(int nUnit, bool bHorizontal);

	UINT GetOutlineWidthSetting();
	int GetHeight();
	int GetAscender();
	int GetUnicodeMapIndex();
	FTManager * GetFTManager();

	// IFTFont interface implement
	void GetMaxSize(int& cx, int& cy);
	BOOL GetCharExtent(wchar_t ch, LPFTDSIZE lpOffset, LPFTDSIZE lpSize, LPFTDSIZE lpBmpSize);
	BOOL GetTextExtentExPoint(const wchar_t *lpszStr, int cchString,  int nMaxExtent, LPINT lpnFit, LPINT alpDx, LPFTDSIZE lpSize);
	BOOL TextOut(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, int nWidth, int nHeight);
	BOOL TextOutWithOutline(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, BYTE *pOutline, int nWidth, int nHeight);
	BOOL DrawChar(wchar_t ch, BYTE* pBitmap, int nWidth, int nHeight);
	BOOL DrawCharWithOutline(wchar_t ch, BYTE* pBitmap, BYTE* pOutline, int nWidth, int nHeight);
	void Release();
protected:
	BOOL TextOutInternal(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBuffer, int nWidth, int nHeight, bool bRenderAsOutline);
	BOOL DrawCharInternal(wchar_t ch, BYTE* pBuffer, int nWidth, int nHeight, bool bRenderAsOutline);
	void ComputeAscender();
	void ComputeHeight();
	int FixedToInt(int fixed);
};

#endif // !defined(AFX_FTFONT_H__38A376F6_60BB_4E79_BA03_73283A5B1EC6__INCLUDED_)
