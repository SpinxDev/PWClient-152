// FTGlyphCache.cpp: implementation of the FTGlyphCache class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTGlyphCache.h"
#include "FTFont.h"
#include "ftglyph.h"
#include "ftsynth.h"
#include "ftoutln.h"
#include "MyRaster.h"
#include "FTManager.h"
#include "internal/ftobjs.h"


//typedef struct _FIXED { 
//	WORD  fract; 
//	short value; 
//} FIXED; 

FTGlyphCacheNode::~FTGlyphCacheNode()
{
	FT_Done_Glyph((FT_Glyph) glyph);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FTGlyphCache::FTGlyphCache(FTFont *pFont, int nCacheSize) :
	TheFont(pFont),
	HeadNode(NULL),
	TailNode(NULL),
	NodeCount(0),
	NodeSizeSum(0)
{

}

FTGlyphCache::~FTGlyphCache()
{
	FTGlyphCacheNode *p = NULL;
	while (HeadNode)
	{
		p = HeadNode->pNext;
		delete HeadNode;
		HeadNode = p;
	}
}

FT_BitmapGlyph FTGlyphCache::LookupGlyph(int nIndex, bool bRenderAsOutline)
{
	FTGlyphCacheNode *pNode = FindInCache(nIndex, bRenderAsOutline && TheFont->GetOutlineWidthSetting() > 0);
	if (!pNode)
		pNode = NewNode(nIndex, bRenderAsOutline);
	
	if (!pNode)
		return NULL;
	else
		return pNode->glyph;
}


FTGlyphCacheNode * FTGlyphCache::FindInCache(int nIndex, bool bOutline)
{
	FTGlyphCacheNode *p = HeadNode;
	while (p)
	{
		if (p->nIndex == nIndex && p->bOutline == bOutline)
		{
			// found! 

			// move the node to head
			if (NodeCount > 1 && HeadNode != p) 
			{
				if (p == TailNode)
					TailNode = p->pPre;
				// unlink
				p->pPre->pNext = p->pNext;
				if (p->pNext)
					p->pNext->pPre = p->pPre;
				// put to head
				p->pPre = NULL;
				p->pNext = HeadNode;
				HeadNode->pPre = p;
				HeadNode = p;
			}

			// return result
			return p;
		}
		p = p->pNext;
	}
	return NULL;
}

FTGlyphCacheNode * FTGlyphCache::NewNode(int nIndex, bool bRenderAsOutline)
{
	FT_Glyph ftResultGlyph = NULL;
	FT_Error error = 0;
	bool bCreateAsOutline = bRenderAsOutline && TheFont->GetOutlineWidthSetting() > 0;
	if (bCreateAsOutline)
	{
		// get original bitmap from cache!
		FT_Glyph ftGlyphInCache = (FT_Glyph) LookupGlyph(nIndex, false);
		if (error) return NULL;

		ConvertToOutline((FT_BitmapGlyph) ftGlyphInCache, (FT_BitmapGlyph *) &ftResultGlyph);
	}
	else
	{
		// get outline
		FT_Size ftSize = TheFont->GetFTSize();
		FT_Face ftFace =  ftSize->face;
		error = FT_Load_Glyph(ftFace, nIndex, FT_LOAD_NO_BITMAP | /*FT_LOAD_NO_HINTING |*/ FT_LOAD_NO_AUTOHINT | FT_LOAD_TARGET_NORMAL);
		if (error)	return NULL;
		if (ftFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE) return NULL;
		
		// transform for bold/italic style
		if ((unsigned long)ftFace->generic.data & STYLE_BOLD)
			FT_GlyphSlot_Embolden( ftFace->glyph );
		if ((unsigned long)ftFace->generic.data & STYLE_ITALIC)
			FT_GlyphSlot_Oblique( ftFace->glyph );

		// rasterize
		error = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
		if (error) return NULL;
		
		error = FT_Get_Glyph(ftFace->glyph, &ftResultGlyph);
		if (error) return NULL;

		if (TheFont->GetOutlineWidthSetting() > 0)
		{
			FT_BitmapGlyph glyph;
			ModifyGraphToFitOutlineStyleSize((FT_BitmapGlyph) ftResultGlyph, (FT_BitmapGlyph *) &glyph);
			FT_Done_Glyph(ftResultGlyph);
			ftResultGlyph = (FT_Glyph) glyph;
		}
	}

	// create node and return
	FTGlyphCacheNode *pNode = new FTGlyphCacheNode;
	pNode->nIndex = nIndex;
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph) ftResultGlyph;
	pNode->glyph = bitmapGlyph;
	pNode->nSize += 
		sizeof(bitmapGlyph) + 
		sizeof(bitmapGlyph->bitmap) + 
		bitmapGlyph->bitmap.rows * bitmapGlyph->bitmap.pitch;
	pNode->bOutline = bCreateAsOutline;
	
	AddNode(pNode);

	return pNode;
}


void FTGlyphCache::AddNode(FTGlyphCacheNode *pNode)
{
	
	while (!IsEmpty() && NodeSizeSum + pNode->nSize > TheFont->GetFTManager()->GetCacheSizeMax()) // list is full
	{
		RemoveTail();
	}

	AddHead(pNode);
}

void FTGlyphCache::ConvertToOutline(FT_BitmapGlyph ftGlyphOriginal, FT_BitmapGlyph *pFtGlyphDest)
{
	// note: at this point, the ftGlyphOriginal is already enlarged !

	int nWidth = TheFont->GetOutlineWidthSetting();
	if (nWidth == 0) return;

	int nRows = ftGlyphOriginal->bitmap.rows;
	int nCols = ftGlyphOriginal->bitmap.width;
	FT_Byte *pNewBuffer = new FT_Byte[nRows * nCols];
	ZeroMemory(pNewBuffer, nRows * nCols);


#define BLACKCOLOR 1
#define PUREBLACKCOLOR 255
#define INDEX (i * nCols + j)
#define OLDBIT ftGlyphOriginal->bitmap.buffer[INDEX]
#define HORZ_SETNEWBITS_BEFORE {memset(&pNewBuffer[INDEX - nWidth], PUREBLACKCOLOR, nWidth);}
#define HORZ_SETNEWBITS_AFTER {memset(&pNewBuffer[INDEX], PUREBLACKCOLOR, nWidth);}
#define VERT_SETNEWBITS_BEFORE \
	for (int k = 1; k <= nWidth; k++) \
	{ \
		pNewBuffer[INDEX - nCols * k] = PUREBLACKCOLOR; \
	}
#define VERT_SETNEWBITS_AFTER \
	for (int k = 0; k < nWidth; k++) \
	{ \
		pNewBuffer[INDEX + nCols * k] = PUREBLACKCOLOR; \
	}

	int i, j;
	bool bBlackStarted = false;
	// horz pass
	for (i = 0; i < nRows; i++)
	{
		bBlackStarted = false;
		for (j = 0; j < nCols; j++)
		{
			if (OLDBIT >= BLACKCOLOR && !bBlackStarted)
			{
				HORZ_SETNEWBITS_BEFORE;
				bBlackStarted = true;
			}
			else if (OLDBIT < BLACKCOLOR && bBlackStarted)
			{
				HORZ_SETNEWBITS_AFTER;
				bBlackStarted = false;
			}
		}
	}
	// vert pass
	for (j = 0; j < nCols; j++)
	{
		bBlackStarted = false;
		for (i = 0; i < nRows; i++)
		{
			if (OLDBIT >= BLACKCOLOR && !bBlackStarted)
			{
				VERT_SETNEWBITS_BEFORE;
				bBlackStarted = true;
			}
			else if (OLDBIT < BLACKCOLOR && bBlackStarted)
			{
				VERT_SETNEWBITS_AFTER;
				bBlackStarted = false;
			}
		}
	}

	// build output
	
	// create new one
	FT_BitmapGlyph dest = new FT_BitmapGlyphRec_;
	// copy structure content, not including bitmap buffer
	memcpy(dest, ftGlyphOriginal, sizeof(FT_BitmapGlyphRec_));
	dest->bitmap.buffer = pNewBuffer;
	// output
	*pFtGlyphDest = dest;
}


void FTGlyphCache::RemoveTail()
{
	if (!TailNode) return;

	NodeCount--;
	NodeSizeSum -= TailNode->nSize;

	if (TailNode->pPre)
		TailNode->pPre->pNext = NULL;
	FTGlyphCacheNode *pLast = TailNode;
	TailNode = TailNode->pPre;
	
	delete pLast;

	if (!TailNode)
		HeadNode = NULL;
}

void FTGlyphCache::AddHead(FTGlyphCacheNode *pNode)
{
	if (!HeadNode)	// empty list
	{
		HeadNode = pNode;
		TailNode = pNode;
	}
	else
	{
		pNode->pNext = HeadNode;
		HeadNode->pPre = pNode;
		HeadNode = pNode;
	}
	NodeCount++;
	NodeSizeSum += pNode->nSize;

}

bool FTGlyphCache::IsEmpty()
{
	return (HeadNode == NULL);
}

void FTGlyphCache::ModifyGraphToFitOutlineStyleSize(FT_BitmapGlyph ftGlyphOriginal, FT_BitmapGlyph *pFtGlyphDest)
{
	int nWidth = TheFont->GetOutlineWidthSetting();
	if (nWidth == 0) return;

	// create new bitmap which is larger than original one
	int nRows = ftGlyphOriginal->bitmap.rows;
	int nCols = ftGlyphOriginal->bitmap.width;
	int nNewRows = nRows + nWidth * 2;
	int nNewCols = nCols + nWidth * 2;
	FT_Byte *pNewBuffer = new FT_Byte[nNewRows * nNewCols];
	ZeroMemory(pNewBuffer, sizeof(FT_Byte) * nNewRows * nNewCols);
	
	// copy original bitmap into buffer to center it
	for (int i = 0; i < nRows; i++)
	{
		memcpy(&pNewBuffer[(i + nWidth) * nNewCols + nWidth], &ftGlyphOriginal->bitmap.buffer[i * nCols], nCols);
	}

	// build output
	
	// create new one
	FT_BitmapGlyph dest = new FT_BitmapGlyphRec_;
	// copy structure content, not including bitmap buffer
	memcpy(dest, ftGlyphOriginal, sizeof(FT_BitmapGlyphRec_));
	// attch buffer to output
	dest->bitmap.buffer = pNewBuffer;
	// adjust param
	dest->bitmap.pitch = nNewCols;
	dest->bitmap.width = nNewCols;
	dest->bitmap.rows = nNewRows;
	dest->top += nWidth;
	FIXED *pFixed = (FIXED *)&dest->root.advance.x;
	pFixed->value += nWidth * 2;
	pFixed = (FIXED *)&dest->root.advance.y;
	pFixed->value += nWidth * 2;

	// output
	*pFtGlyphDest = dest;
}
