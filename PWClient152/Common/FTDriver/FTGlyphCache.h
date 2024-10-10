// FTGlyphCache.h: interface for the FTGlyphCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTGLYPHCACHE_H__462B9F6A_A76E_4E5C_B639_4ACDDE44C7D7__INCLUDED_)
#define AFX_FTGLYPHCACHE_H__462B9F6A_A76E_4E5C_B639_4ACDDE44C7D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FTFont;

typedef struct FT_BitmapGlyphRec_*  FT_BitmapGlyph;

struct FTGlyphCacheNode 
{
	int nIndex;
	bool bOutline;

	FTGlyphCacheNode * pNext;
	FTGlyphCacheNode * pPre;
	FT_BitmapGlyph glyph;
	int nSize;

	FTGlyphCacheNode()
	{
		pPre = NULL;
		pNext = NULL;
		glyph = NULL;
		nIndex = -1;
		nSize = sizeof(FTGlyphCacheNode);
		bOutline = false;
	}
	~FTGlyphCacheNode();
};

class FTGlyphCache  
{
	int NodeCount;
	int NodeSizeSum;
	FTGlyphCacheNode * HeadNode;
	FTGlyphCacheNode * TailNode;
	FTFont * TheFont;
public:
	bool IsEmpty();
	FTGlyphCache(FTFont *pFont, int nCacheSize);
	virtual ~FTGlyphCache();
	FT_BitmapGlyph LookupGlyph(int nIndex, bool bRenderAsOutline);
protected:
	void ModifyGraphToFitOutlineStyleSize(FT_BitmapGlyph ftGlyphOriginal, FT_BitmapGlyph *pFtGlyphDest);
	void AddHead(FTGlyphCacheNode *pNode);
	void RemoveTail();
	void ConvertToOutline(FT_BitmapGlyph ftGlyphOriginal, FT_BitmapGlyph *pFtGlyphDest);
	void AddNode(FTGlyphCacheNode *pNode);
	FTGlyphCacheNode * NewNode(int nIndex, bool bRenderAsOutline);
	FTGlyphCacheNode * FindInCache(int nIndex, bool bOutline);
};

#endif // !defined(AFX_FTGLYPHCACHE_H__462B9F6A_A76E_4E5C_B639_4ACDDE44C7D7__INCLUDED_)
