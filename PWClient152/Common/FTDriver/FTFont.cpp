// FTFont.cpp: implementation of the FTFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTFont.h"
#include "FTManager.h"

#include "MyFTCache.h"
#include "FTHelper.h"
#include "FTGlyphCache.h"


CriticalSession FTFont::CSForFreetype;

#define DEFAULT_DPI 96

FTFont::FTFont(FTManager *pFTManager, int nCacheSize)
{
	TheFTManager = pFTManager;
	MyFaceID = NULL;
	FTCScaler = new FTC_ScalerRec;
	Height = 0;
	Ascender = 0;
	TheGlyphCache = NULL;
	UnicodeCMapIndex = -1;
	CacheSize = nCacheSize;
}

FTFont::~FTFont()
{
	Destory();
}

bool FTFont::CreateFontIndirect(const FontParam &fontParam)
{
	MyFaceID = TheFTManager->LookupFaceID(fontParam.FontFile, fontParam.FontFileLength, fontParam.FaceIndex, fontParam.FontStyle);

	FT_Face ftFace;
	
	if (FTC_Manager_LookupFace(TheFTManager->GetCacheManager(), (FTC_FaceID)MyFaceID, &ftFace))
		return false;

	if (-1 == (UnicodeCMapIndex = GetUnicodeCMapIndex(ftFace)))
		return false;
	
	FTCScaler->face_id = (FTC_FaceID) MyFaceID;
	FTCScaler->width = (fontParam.PointWidth << 6) / 10;
	FTCScaler->height = (fontParam.PointHeight << 6) / 10;
	FTCScaler->pixel = FALSE;
	if (0 == fontParam.HorzDPI)
		FTCScaler->x_res = DEFAULT_DPI;
	else
		FTCScaler->x_res = fontParam.HorzDPI;
	if (0 == fontParam.VertDPI)
		FTCScaler->y_res = DEFAULT_DPI;
	else
		FTCScaler->y_res = fontParam.VertDPI;

	FT_Size ftSize;
	if (FTC_Manager_LookupSize(TheFTManager->GetCacheManager(), FTCScaler, &ftSize))
		return false;

	TheGlyphCache = new FTGlyphCache(this, CacheSize);

	OutlineWidth = fontParam.OutlineWidth;

	ComputeHeight();
	ComputeAscender();


	return true;
}

bool FTFont::CreatePointFont(const char *fontFile, int fontFileLength, UINT fontPoint, int fontStyle, int nOutlineWidth)
{
	FontParam fontParam;
	fontParam.FontFile = fontFile;
	fontParam.FontFileLength = fontFileLength;
	fontParam.FaceIndex = 0;
	fontParam.FontStyle = fontStyle;
	fontParam.PointHeight = fontPoint;
	fontParam.PointWidth = fontPoint;
	fontParam.VertDPI = 96;
	fontParam.HorzDPI = 96;
	fontParam.OutlineWidth = nOutlineWidth;

	return CreateFontIndirect(fontParam);
}

void FTFont::Destory()
{
	SAFE_DELETE(TheGlyphCache);
	SAFE_DELETE(FTCScaler);
	if( MyFaceID )
		TheFTManager->RemoveFaceID(MyFaceID);
	MyFaceID = NULL;
}

FT_Face FTFont::GetFTFace()
{
	FT_Face ftFace = NULL;
	FTC_Manager_LookupFace(TheFTManager->GetCacheManager(), (FTC_FaceID)MyFaceID, &ftFace);
	return ftFace;
}

FT_Size FTFont::GetFTSize()
{
	FT_Size ftSize = NULL;
	FTC_Manager_LookupSize(TheFTManager->GetCacheManager(), FTCScaler, &ftSize);
	return ftSize;
}


FT_BitmapGlyph FTFont::RenderChar(UINT c, bool bRenderAsOutline)
{
	// get char index
	FTC_CMapCache mapCache = GetFTManager()->GetCMapCache();
	
	FTC_FaceID ftFaceID = (FTC_FaceID) GetMyFaceID();
	
	UINT index = FTC_CMapCache_Lookup(mapCache, ftFaceID, UnicodeCMapIndex, c);
	if (0 == index)
	{
		index = FTC_CMapCache_Lookup(mapCache, ftFaceID, UnicodeCMapIndex, L'¡õ');
		if (0 == index)
			return NULL;
	}

	return TheGlyphCache->LookupGlyph(index, bRenderAsOutline);
}

#define PRINT_TO_BITMAP(src, dest) \
{ \
	if ((src) > 0) \
		(dest) = (src); \
}


BOOL FTFont::GetTextExtentExPoint(const wchar_t *lpszStr, int cchString, int nMaxExtent, LPINT lpnFit, LPINT alpDx, LPFTDSIZE lpSize)
{
	CriticalSession::Scope cs(CSForFreetype);

	// we use max_advance_height and max_advance_width as BBOX
	// instead of old bbbox, since the grid-fitted bitmap may 
	// bigger than bbbox

	if (lpnFit)
		*lpnFit = 0;

	ZeroMemory(lpSize, sizeof(SIZE));
	FT_BitmapGlyph ftGlyph;
	for (int i = 0; i < cchString; i++)
	{
		// render
		if (!(ftGlyph = RenderChar(lpszStr[i], false)))
			continue;

		// compute size->cx
		lpSize->cx += FixedToInt(ftGlyph->root.advance.x);
//		lpSize->cx += ftGlyph->left + ftGlyph->bitmap.width;

		
		if ((lpnFit || alpDx) && lpSize->cx <= nMaxExtent)
		{
			++(*lpnFit);

			if (alpDx)
				alpDx[i] = lpSize->cx;
		}
	}
	// compute size->cy
	lpSize->cy = GetHeight();

	return TRUE;
}

int FTFont::GetUnicodeCMapIndex(FT_Face ftFace)
{
	for (int i = 0; i < ftFace->num_charmaps; i++)
	{
		if (FT_ENCODING_UNICODE == ftFace->charmaps[i]->encoding)
		{
			return i;
		}
	}
	return -1;
}

int FTFont::FontUnitToPixel(int nUnit, bool bHorizontal)
{
	FT_Face ftFace = GetFTFace();
	FTC_Scaler ftcScaler = GetFTCScaler();

	float ret;
	if (bHorizontal)
		ret = (float(ftcScaler->width) / 64) * ftcScaler->x_res / 72 * nUnit / ftFace->units_per_EM;
	else
		ret = (float(ftcScaler->height) / 64) * ftcScaler->y_res / 72 * nUnit / ftFace->units_per_EM;
	int n = int(ret + .5f);
	return n <= 0 ? 1 : n;
}

void FTFont::Release()
{
	CriticalSession::Scope cs(CSForFreetype);

	delete this;
}

FTManager * FTFont::GetFTManager()
{
	return TheFTManager;
}

int FTFont::GetUnicodeMapIndex()
{
	return UnicodeCMapIndex;
}

int FTFont::FixedToInt(int fixed)
{
	FIXED *pFixed = (FIXED *) &fixed;
	if (pFixed->fract >= 0x8000)
		return pFixed->value + 1;
	else
		return pFixed->value;

	return 0;
}

int FTFont::GetHeight()
{
	return Height;
}

int FTFont::GetAscender()
{
	return Ascender;
}

void FTFont::ComputeHeight()
{
	Height = FontUnitToPixel(GetFTFace()->height + GetOutlineWidthSetting() * 2, false);
}

void FTFont::GetMaxSize(int& cx, int& cy)
{
	FT_Face face = GetFTFace();
	cx = FontUnitToPixel(face->bbox.xMax - face->bbox.xMin, true) + GetOutlineWidthSetting() * 2;
	cy = FontUnitToPixel(face->bbox.yMax - face->bbox.yMin, false) + GetOutlineWidthSetting() * 2;
}

BOOL FTFont::GetCharExtent(wchar_t ch, LPFTDSIZE lpOffset, LPFTDSIZE lpSize, LPFTDSIZE lpBmpSize)
{
	CriticalSession::Scope cs(CSForFreetype);
	FT_BitmapGlyph ftGlyph;

	if (!(ftGlyph = RenderChar(ch, false)))
		return FALSE;

	lpOffset->cx = ftGlyph->left;
	lpOffset->cy = GetAscender() - ftGlyph->top;
	lpSize->cx = FixedToInt(ftGlyph->root.advance.x);
	lpSize->cy = GetHeight();
	lpBmpSize->cx = ftGlyph->bitmap.width;
	lpBmpSize->cy = ftGlyph->bitmap.rows;

	return TRUE;
}

void FTFont::ComputeAscender()
{
	if (Height == 0) ComputeHeight();

	FT_Face ftFace = GetFTFace();
	int oldAscender = FontUnitToPixel(ftFace->ascender, false);
	int oldHeight = FontUnitToPixel(ftFace->height, false);
	
	Ascender = GetHeight() * oldAscender / oldHeight + GetOutlineWidthSetting();
}

UINT FTFont::GetOutlineWidthSetting()
{
	return OutlineWidth;
}

BOOL FTFont::TextOut(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, int nWidth, int nHeight)
{
	return TextOutInternal(nXStart, nYStart, lpString, cbString, pBitmap, nWidth, nHeight, false);
}

BOOL FTFont::TextOutWithOutline(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBitmap, BYTE *pOutline, int nWidth, int nHeight)
{
	if (!TextOutInternal(nXStart, nYStart, lpString, cbString, pBitmap, nWidth, nHeight, false))
		return FALSE;

	return TextOutInternal(nXStart, nYStart, lpString, cbString, pOutline, nWidth, nHeight, true);
}

BOOL FTFont::TextOutInternal(int nXStart, int nYStart, const wchar_t *lpString, int cbString, BYTE * pBuffer, int nWidth, int nHeight, bool bRenderAsOutline)
{
	CriticalSession::Scope cs(CSForFreetype);

	int nFit = 0;
	SIZE size;
	if (!GetTextExtentExPoint(lpString, cbString, nWidth - nXStart, &nFit, NULL, (LPFTDSIZE)&size))
		return FALSE;
	if (0 == size.cx || 0 == size.cy)
		return TRUE;
	for (int y = nYStart; y < size.cy && y < nHeight; y++)
	{
		ZeroMemory(&pBuffer[y * nWidth + nXStart], min(size.cx, nWidth - nXStart));
	}
	
	int x = nXStart;
	FT_Face ftFace = GetFTFace();
	FT_BitmapGlyph ftGlyph;
	for (int i = 0; i < cbString; i++)
	{
		if (!(ftGlyph = RenderChar(lpString[i], bRenderAsOutline)))
			continue;
		
		x += ftGlyph->left;
		if (nWidth - x <= 0)
			break;
		int y = (GetAscender() - ftGlyph->top) + nYStart;
		int startJ = 0;
		if (y < 0) 
			startJ = -y;
		for (int j = startJ; j < min(ftGlyph->bitmap.rows, nHeight - y); j++)
		{
			for (int i = 0; i < min(ftGlyph->bitmap.width, nWidth - x); i++)
			{
				PRINT_TO_BITMAP(ftGlyph->bitmap.buffer[j * ftGlyph->bitmap.width + i], pBuffer[(y + j) * nWidth + (x + i)]);
			}
		}
		x += FixedToInt(ftGlyph->root.advance.x) - ftGlyph->left;
		//x += ftGlyph->bitmap.width;
	}
	return TRUE;
}

BOOL FTFont::DrawChar(wchar_t ch, BYTE* pBitmap, int nWidth, int nHeight)
{
	return DrawCharInternal(ch, pBitmap, nWidth, nHeight, false);
}

BOOL FTFont::DrawCharWithOutline(wchar_t ch, BYTE* pBitmap, BYTE* pOutline, int nWidth, int nHeight)
{
	if (!DrawCharInternal(ch, pBitmap, nWidth, nHeight, false))
		return FALSE;

	return DrawCharInternal(ch, pOutline, nWidth, nHeight, true);
}

BOOL FTFont::DrawCharInternal(wchar_t ch, BYTE* pBuffer, int nWidth, int nHeight, bool bRenderAsOutline)
{
	CriticalSession::Scope cs(CSForFreetype);

	FT_Face ftFace = GetFTFace();
	FT_BitmapGlyph ftGlyph;

	if (!(ftGlyph = RenderChar(ch, bRenderAsOutline)))
		return TRUE;

	int min_row = min(ftGlyph->bitmap.rows, nHeight);
	int min_col = min(ftGlyph->bitmap.width, nWidth);

	int i, j;

	for (j = 0; j < min_row; j++)
	{
		for (i = 0; i < min_col; i++)
			pBuffer[j * nWidth + i] = ftGlyph->bitmap.buffer[j * ftGlyph->bitmap.width + i];

		for (; i < nWidth; i++)
			pBuffer[j * nWidth + i] = 0;
	}

	for (; j < nHeight; j++)
		memset(&pBuffer[j * nWidth], 0, nWidth);

	return TRUE;
}
