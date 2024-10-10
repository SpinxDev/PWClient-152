// AUITag.cpp: implementation of the AUITag class.
//
//////////////////////////////////////////////////////////////////////

#include "AUI.h"
#include "AUITag.h"
#include "A3DEngine.h"
#include "A3DFlatCollector.h"
#include "A3DWireCollector.h"

extern int _tab_char;


#define INC_INDEX(context, nTagChar) context.index += nTagChar;

void InitTagParams(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams,
				   A3DFTFont *pFont, int x, int y, const ACHAR *pszText,
				   A3DRECT *pRect, int nStartLine, int *pnLines, int nSpace, bool bAutoIndent,
				   int *pnHeight, A2DSprite *pA2DSpriteEmotion,
				   abase::vector<AUITEXTAREA_EMOTION> *pvecEmotion,
				   int *pnLinesThisPage,
				   bool bDrawToScreen, int nShadow, int nAlpha, bool bSmallEmotion,
				   EditBoxItemsSet* pItemsSet,
				   abase::vector<A2DSprite*> *pvecImage,
				   float fWindowScale,
				   bool bAutoReturn,
				   abase::vector<FONT_WITH_SHADOW>* pvecOtherFonts,
				   int nWordWidth,
				   TEXTITEM_RCINFO* pItemRcInfo,
				   bool bVertical)
{
	// In Params
	inParams._pFont = pFont;
	inParams._pszText = pszText;
	inParams._x = x;
	inParams._y = y;
	inParams._bAutoIndent = bAutoIndent;
	inParams._bAutoReturn = bAutoReturn;
	inParams._bDrawToScreen = bDrawToScreen;
	inParams._bSmallEmotion = bSmallEmotion;
	inParams._fWindowScale = fWindowScale;
	inParams._nAlpha = nAlpha;
	inParams._nShadow = nShadow;
	inParams._nSpace = nSpace;
	inParams._nStartLine = nStartLine;
	inParams._nWordWidth = nWordWidth;
	inParams._unEmotionInfo.stSingleEmotion._pA2DSpriteEmotion = pA2DSpriteEmotion;	
	inParams._unEmotionInfo.stSingleEmotion._pVecEmotion = pvecEmotion;
	inParams._pItemsSet = pItemsSet;
	inParams._pRect = pRect;
	inParams._pVecImage = pvecImage;
	inParams._pVecOtherFonts = pvecOtherFonts;
	inParams._bVertical = bVertical;
	
	// Out params
	outParams._pItemRcInfo = pItemRcInfo;
	outParams._pnHeight = pnHeight;
	outParams._pnLines = pnLines;
	outParams._pnLinesThisPage = pnLinesThisPage;
}


//////////////////////////////////////////////////////////////////////////
// AUITextTag
//////////////////////////////////////////////////////////////////////////
const char* AUITextTag::s_szAUITextTag = "AUITextTag";
bool AUITextTag::UpdateNewline( const TAG_OP_IN_PARAMS &inParams, TAG_RESOLVER_CONTEXT &context, bool bManualReturn)
{
	// 更新x，y。并判断是否需要绘制或者跳出循环
	context.nLines++;
	if(inParams._bVertical)
	{
		context.nMaxWidth = a_Max(context.nMaxWidth, context.nCurY - inParams._y);
		context.nOffsetYFromFirstLine += (context.nLineHeightThis + inParams._nSpace);

		context.nCurY = ((bManualReturn && inParams._bAutoIndent) ? (inParams._y + 2 * context._nWidthSpace) : inParams._y);		

		// note: for English char, getting the height is reasonable, for chinese char, we get the width. (vertical text)
		int nCharHeight;
		if(context.bRotateLayout)
			nCharHeight = context.pCurFont->GetFontHeight();
		else
			nCharHeight = context.pCurFont->GetFontWidth();
		if( context.nLines > inParams._nStartLine )
		{
			context.nCurX += context.nLineHeightThis + inParams._nSpace;

			if( inParams._pRect && context.nCurX + nCharHeight > inParams._pRect->right )
			{
				context.bExceedBottom = true;
				if( !context._bCalcTextInfo )
				{
					context.bFinish = true;
					return false;
				}
			}
			else
			{
				context.nLinesThisPage++;
			}
		}
		context.nLineHeightThis = nCharHeight;
	}
	else
	{
		context.nMaxWidth = a_Max(context.nMaxWidth, context.nCurX - inParams._x);
		context.nOffsetYFromFirstLine += (context.nLineHeightThis + inParams._nSpace);

		context.nCurX = ((bManualReturn && inParams._bAutoIndent) ? (inParams._x + 2 * context._nWidthSpace) : inParams._x);		

		if( context.nLines > inParams._nStartLine )
		{
			context.nCurY += context.nLineHeightThis + inParams._nSpace;
			if( inParams._pRect && context.nCurY + context.pCurFont->GetFontHeight() > inParams._pRect->bottom )
			{
				context.bExceedBottom = true;
				if( !context._bCalcTextInfo )
				{
					context.bFinish = true;
					return false;
				}
			}
			else
			{
				context.nLinesThisPage++;
			}
		}
		context.nLineHeightThis = context.pCurFont->GetFontHeight();
	}

	return true;
}

AUITAG_NODE* AUITextTag::CreateNode(AUIRichTextPrinter* pPrinter)
{
	if (!pPrinter) 
		return NULL; 
	m_pNode = pPrinter->GetNode(GetClassName()); 
	if (m_pNode) 
		return m_pNode; 
	m_pNode = new AUITAG_NODE(new AUITextTag, NULL, GetClassName()); 
	return m_pNode;
}

bool AUITextTag::OutputSubTags(const TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams, TAG_RESOLVER_CONTEXT& context) const
{
	if (!m_pNode)
		return false;

	AUITAG_NODE* pNode = m_pNode->_pFirstChild;
	if (!pNode)
		return false;

	while (pNode)
	{
		if (!pNode->_pTag)
			return AUI_ReportError("AUITextTag::OutputSubTags() pTag=NULL");
		if (pNode->_pTag->IsMyType(inParams._pszText, context.index, context._len))
			return pNode->_pTag->Output(inParams, context, outParams);
		pNode = pNode->_pNext;
	}

	return false;
}

void EnumTags(const AUITAG_NODE* pNode, int nLevel, char* szBuffer, int& index, int iLenBuffer)
{
	if (!pNode || !szBuffer || index + 40 > iLenBuffer)
		return;

	char* psz = szBuffer + index;
	memset(psz, ' ', 2 * nLevel);
	if (pNode->_pFirstChild)
		strcat(psz, "-");
	else
		strcat(psz, ".");
	strcat(psz, pNode->_pTag->GetClassName());
	strcat(psz, "\r\n");
	index += 2 * nLevel + 1 + strlen(pNode->_pTag->GetClassName()) + 2;

	AUITAG_NODE* pN = pNode->_pFirstChild;
	while (pN)
	{
		EnumTags(pN, nLevel+1, szBuffer, index, iLenBuffer);
		pN = pN->_pNext;
	}
}

//////////////////////////////////////////////////////////////////////////
// AUITAG_NODE
//////////////////////////////////////////////////////////////////////////
AUITAG_NODE::AUITAG_NODE(AUITextTag* ptg, AUITAG_NODE* pNodeBase, const char* szClassName) :
_pBase(pNodeBase), 
_pTag(ptg), 
_pNext(0), 
_pPrev(NULL), 
_pFirstChild(0), 
_pLastChild(0),
_szClassName(szClassName)
{
	if (_pTag)
		_pTag->m_pNode = this;
	if (_pBase)
		_pBase->AppendChild(this);
}

bool AUITAG_NODE::AppendChild(AUITAG_NODE* pNode)
{
	if (!pNode)
		return false;
	
	if (!_pFirstChild)
	{
		_pFirstChild = pNode;
		_pLastChild = pNode;
	}
	else
	{
		// Check if exists in chain
		AUITAG_NODE* pC = _pFirstChild;
		while (pC)
		{
			if (pC == pNode)
				return false;
			pC = pC->_pNext;
		}

		_pLastChild->_pNext = pNode;
		pNode->_pPrev = _pLastChild;
		_pLastChild = pNode;		
	}

	return true;
}

AUITAG_NODE::~AUITAG_NODE()
{
	if (_pTag)
		delete _pTag;
	_pTag = 0;
}

//////////////////////////////////////////////////////////////////////////
// AUIRichTextPrinter
//////////////////////////////////////////////////////////////////////////
AUIRichTextPrinter AUIRichTextPrinter::ms_TextPrinter;
AUIRichTextPrinter::AUIRichTextPrinter()
{
	// Register built-in tags.
	RegisterTagtypeBuiltIn(AUITextTag());	
	m_pNodeRoot = GetNode(AUITextTag().GetClassName());

	// common ctrl
	RegisterTagtypeBuiltIn(AUITextTagReturn());					//\r
	RegisterTagtypeBuiltIn(AUITextTagTable());					//\v
	RegisterTagtypeBuiltIn(AUITextTagFakReturn());				//\n
	RegisterTagtypeBuiltIn(AUITextTagQuote());					//&

	// ctrl characters
	RegisterTagtypeBuiltIn(AUITextTagCtrl());					//^..				
	RegisterTagtypeBuiltIn(AUITextTagCtrlDefaultColor());		//^N
	RegisterTagtypeBuiltIn(AUITextTagCtrlFont());				//^O023
	RegisterTagtypeBuiltIn(AUITextTagCtrlUnderline());			//^U
	RegisterTagtypeBuiltIn(AUITextTagCtrlUnderlineX());			//^u
	RegisterTagtypeBuiltIn(AUITextTagCtrlWordspace());			//^o
	RegisterTagtypeBuiltIn(AUITextTagCtrlChannel());			//^p世界^p 或者 ^p^ffff00世界^p
	RegisterTagtypeBuiltIn(AUITextTagCtrlColor());				//^ffffff

	// edit box items
	RegisterTagtypeBuiltIn(AUITextTagItem());					//口
	RegisterTagtypeBuiltIn(AUITextTagItemBox());
	RegisterTagtypeBuiltIn(AUITextTagItemEmotion());
	RegisterTagtypeBuiltIn(AUITextTagItemImage());
	RegisterTagtypeBuiltIn(AUITextTagItemText());
}

AUIRichTextPrinter::~AUIRichTextPrinter()
{
	// to set AUITextTagXXX::s_pNode_xxx = 0
	for (abase::hash_map<const char*, AUITAG_NODE*>::iterator itr = m_mpNodesBuildIn.begin(); itr != m_mpNodesBuildIn.end(); ++itr)
	{
		if (itr->second)
			delete itr->second;
	}

	for (abase::hash_map<const char*, AUITAG_NODE*>::iterator itr2 = m_mpNodesCustom.begin(); itr2 != m_mpNodesCustom.end(); ++itr2)
	{
		if (itr2->second)
			delete itr2->second;
	}
}

AUITAG_NODE* AUIRichTextPrinter::GetNode(const char* szClassName) const
{
	abase::hash_map<const char*, AUITAG_NODE*>::const_iterator itr = m_mpNodesBuildIn.find(szClassName);
	if (itr != m_mpNodesBuildIn.end())
		return itr->second;
	itr = m_mpNodesCustom.find(szClassName);
	if (itr != m_mpNodesCustom.end())
		return itr->second;

	return NULL;
}

void AUIRichTextPrinter::RegisterTagtype(AUITextTag& tg)
{
	AUITAG_NODE* pNode = tg.CreateNode(this);
	ASSERT(pNode);
	if (m_mpNodesBuildIn.find(tg.GetClassName()) == m_mpNodesBuildIn.end())
		m_mpNodesCustom[tg.GetClassName()] = pNode;
}

void AUIRichTextPrinter::RegisterTagtypeBuiltIn(AUITextTag& tg)
{
	AUITAG_NODE* pNode = tg.CreateNode(this);	
	ASSERT(pNode);
	m_mpNodesBuildIn[tg.GetClassName()] = pNode;
}

bool AUIRichTextPrinter::UnRegisterTagType(AUITextTag& tg)
{
	AUITAG_NODE* pNode = GetNode(tg.GetClassName());
	if (!pNode)
		return true;

	// can not unregister built-in type
	if (m_mpNodesBuildIn.find(tg.GetClassName()) != m_mpNodesBuildIn.end())
		return false;
	
	/* we keep tg's childhood hierarchy
	AUITAG_NODE* pChild = pNode->_pFirstChild;
	while (pChild)
	{
		ASSERT(pChild->_pTag);
		UnRegisterTagType(*(pChild->_pTag));
		pChild = pChild->_pNext;
	}*/

	// unchain brother and parent
	AUITAG_NODE* pBase = pNode->_pBase;
	if (pBase)
	{
		if (pBase->_pFirstChild == pNode)
		{
			pBase->_pFirstChild = pNode->_pNext;
		}
		if (pBase->_pLastChild == pNode)
		{
			pBase->_pLastChild = pNode->_pPrev;
		}

		if (pNode->_pPrev)
			pNode->_pPrev->_pNext = pNode->_pNext;
		if (pNode->_pNext)
			pNode->_pNext->_pPrev = pNode->_pPrev;
	}

	// self destruction. It'd be better Released 
	// ReleaseNode(pNode);

	return true;
}

void AUIRichTextPrinter::GetClassHierarchyInRuntime(char* szBuffer, int iLenBuffer) const
{
	int index = 0;
	EnumTags(m_pNodeRoot, 0, szBuffer, index, iLenBuffer);
}

bool AUIRichTextPrinter::InitContext(TAG_OP_IN_PARAMS& inParams, const TAG_OP_OUT_PARAMS& outParams, TAG_RESOLVER_CONTEXT& context)
{
	context.clear();
	context._len = a_strlen(inParams._pszText);
	context._nCharH = inParams._pFont->GetFontHeight();
	context._nCharW = inParams._pFont->GetTextExtent(_AL("W"), 1).x;	// not every character's width
	context._nWidthSpace = context._nCharW;								// auto indent
	context._nTabW = _tab_char * context._nCharW;						// /v
	context._clDefault = inParams._clDefault;
	AUI_ConvertColor(inParams._pszText, context._clDefault);			// ^N
	context._bCalcTextInfo = (outParams._pnLines != NULL || outParams._pCacheMan != NULL || outParams._pnWidth != NULL);

	context.nCurX = inParams._x;
	context.nCurY = inParams._y;
	context.pCurFont = inParams._pFont;
	context.nLineHeightThis = inParams._bVertical ? context._nCharW : context._nCharH;	
	context.nCurShadow = (inParams._nShadow & 0xFF) == inParams._nShadow ? inParams._nShadow << 24 : inParams._nShadow; // for compatibility with old versions.
	context.color = context._clDefault;
	context.color_quote = A3DCOLORRGBA(255, 220, 138, inParams._nAlpha);

/*	if (outParams._pCacheMan)
		outParams._pCacheMan->ClearCache(); //clearCache() outside if necessary.
*/
	return true;
}

const AUITextTag* AUIRichTextPrinter::GetTag(const ACHAR* pszText, int index, int len) const
{
	AUITAG_NODE* pNode = m_pNodeRoot->_pFirstChild;
	while (pNode)
	{
		if (pNode->_pTag->IsMyType(pszText, index, len))
			return pNode->_pTag;
		pNode = pNode->_pNext;
	}
	return NULL;
}

const AUITextTag* AUIRichTextPrinter::RenderPlainText(TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, const TAG_OP_OUT_PARAMS& outParams) const
{
	const AUITextTag* pNextTagCtrl = NULL;
	bool bAutoWrap = inParams._bAutoReturn && inParams._pRect;

	const ACHAR* p = inParams._pszText + context.index;
	bool bEnglishWord = IsEnglishCode(*p);
	context.bRotateLayout = IsVerticalRotationNeeded(*p);
	int iTextEOF = context.index;
	int nTextWidth = 0;

	while (*p != '\0')
	{
		// text. vs. ctrl tag
		pNextTagCtrl = GetTag(inParams._pszText, iTextEOF, context._len);
		if (pNextTagCtrl)
			break;			

		// English and Non-English are separately rendered unless words are in quote 
		// NameLink is always rendered in one time
		if (!context.bInQuote && bAutoWrap && (bEnglishWord ^ IsEnglishCode(*(p+(*p=='^')))))
			break;

		// ^^, ^&  ==  ^, &
		bool bFakeTag = false;
		if (*p == '^' && (*(p+1) == '&' || *(p+1) == '^')) 
		{		
			if (iTextEOF == context.index) // if ^^xxx, render "^" this time
			{
				context.index += 1;
				iTextEOF += 2;
			}
			else							// if xxx^^, render "xxx" this time.
				break;
			p++;
			bFakeTag = true;
		}	
		
		// Now, the format system can render text vertical.
		// for vertical text. the English char are rendered 90 degree clockwise,
		// while the Chinese char are rendered un-rotated.
		// the nCurWordWidth is actually the height for Chinese char in vertical mode.
		// now the vertical text is rendered top-to-bottom and left-to-right.
		// TODO: change left-to-right to right-to-left
		int nCurWordWidth;
		if(inParams._bVertical && !context.bRotateLayout)
			nCurWordWidth = context.pCurFont->GetTextExtent(p, 1).y;
		else
			nCurWordWidth = context.pCurFont->GetTextExtent(p, 1).x;		
		if (IsNewLineNeed(inParams, context, nTextWidth + nCurWordWidth))
		{
			if (iTextEOF == context.index || bEnglishWord || context.bInQuote)
			{
				if(inParams._bVertical)
				{
					if (context.nCurY > inParams._y && !AUITextTag::UpdateNewline(inParams, context))
						return NULL;
				}
				else
				{
					if (context.nCurX > inParams._x && !AUITextTag::UpdateNewline(inParams, context))
						return NULL;
				}

				// 截断英文文本
				if(inParams._bVertical)
				{
					if (bEnglishWord && context.nCurY == inParams._y && iTextEOF > context.index)
						break;
				}
				else
				{
					if (bEnglishWord && context.nCurX == inParams._x && iTextEOF > context.index)
						break;
				}

				if (context.bInQuote)
				{
					context.nl.rc.left = context.nCurX;
					context.nl.rc.top = context.nCurY;
				}
			}
			else if (iTextEOF > context.index) // 以免死循环
				break;
		}
		
		nTextWidth += nCurWordWidth;	
		
		if (bFakeTag)
			break;

		p++;
		iTextEOF++;
	}

	// render [index, iTextEOF)
	A3DCOLOR text_color = (inParams._nAlpha << 24) | (0xFFFFFF & context.color);
	if (context.bInQuote)
		text_color = (inParams._nAlpha << 24) | (0xFFFFFF & context.color_quote);
	else if (context.bInChannelQuote)
		text_color = (inParams._nAlpha << 24) | (0xFFFFFF & context.color_clQuote);
	A3DCOLOR shadow_color = (inParams._nAlpha << 24) | (0xFFFFFF & context.nCurShadow);
	if (CanDraw(inParams, context) && iTextEOF > context.index)
	{
		if(inParams._bVertical)
		{
			context.pCurFont->TextOutVert(context.nCurX, context.nCurY, inParams._pszText + context.index, text_color,
				(context.nCurShadow & 0xFF000000) ? true : false, shadow_color, 0, iTextEOF-context.index, 1.0f,
				context.bUnderLine, context.bUnderLineSameColor ? text_color : context.color_underline, 0.08f * inParams._fWindowScale,
				context.bRotateLayout ? 1 : 0);
		}
		else
		{
			if( inParams._pRect )
			{
				context.pCurFont->DrawText(context.nCurX, context.nCurY, inParams._pszText + context.index, text_color, inParams._pRect,
					(context.nCurShadow & 0xFF000000) ? true : false, shadow_color, 0, iTextEOF-context.index, 1.0f,
					context.bUnderLine, context.bUnderLineSameColor ? text_color : context.color_underline, 0.08f * inParams._fWindowScale);
			}
			else
			{
				context.pCurFont->TextOut(context.nCurX, context.nCurY, inParams._pszText + context.index, text_color,
					(context.nCurShadow & 0xFF000000) ? true : false, shadow_color, 0, iTextEOF-context.index, 1.0f,
					context.bUnderLine, context.bUnderLineSameColor ? text_color : context.color_underline, 0.08f * inParams._fWindowScale);
			}
		}
	}

	// cache
	if (outParams._pCacheMan && iTextEOF > context.index)
	{
		A3DRECT rc(0, 0, 0, 0);
		if (inParams._pRect) rc = *(inParams._pRect);
		outParams._pCacheMan->AddCache(
			new AUITextTagTextCache(
				NULL,
				context.pCurFont,
				A3DPOINT2(context.nCurX - inParams._x, context.nCurY - inParams._y/*context.nOffsetYFromFirstLine*/),
				A3DPOINT2(nTextWidth, context.pCurFont->GetFontHeight()),
				context.index, 
				iTextEOF - context.index,
				(context.nCurShadow & 0xFF000000) ? true : false,
				context.nCurShadow,
				text_color&0xffffff, 
				context.bUnderLine,
				context.bUnderLineSameColor ? text_color : context.color_underline, 
				0.08f * inParams._fWindowScale),
			context.nSection, context.nLines);
	}

	if (context._bCalcRcInfo && context.bInQuote)
	{
		context.nl.strName += p;
		if(inParams._bVertical)
			context.nl.rc.bottom = context.nCurY;
		else
			context.nl.rc.right = context.nCurX;
		if( context.nl.rc.Height() == 0 )
			context.nl.rc.bottom += context.nLineHeightThis;
	}
	
	if(inParams._bVertical)
	{
		context.nCurY += nTextWidth;		
		context.nMaxWidth = a_Max(context.nMaxWidth, context.nCurY - inParams._y);
	}
	else
	{
		context.nCurX += nTextWidth;		
		context.nMaxWidth = a_Max(context.nMaxWidth, context.nCurX - inParams._x);
	}
	INC_INDEX(context, iTextEOF - context.index);

	return pNextTagCtrl;
}

bool AUIRichTextPrinter::Output(TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{
	do 
	{
		// 控制字符
		const AUITextTag* pTag = RenderPlainText(inParams, context, outParams);
		if (pTag)
			pTag->Output(inParams, context, outParams);
	} while (!context.bFinish && context.index < context._len);

	if( outParams._pnLines )
		*outParams._pnLines = context.nLines + 1;
	if( outParams._pnLinesThisPage )
		*outParams._pnLinesThisPage = context.nLinesThisPage + 1;
	if( outParams._pnHeight )
		*outParams._pnHeight = context.nCurY + context.nLineHeightThis + inParams._nSpace - inParams._y;
	if( outParams._pnWidth)
		*outParams._pnWidth = context.nMaxWidth;

	return true;
}

bool AUIRichTextPrinter::CalcSize(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagReturn
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagReturn, AUITextTag)

bool AUITextTagReturn::IsMyType(const ACHAR* pszText, int index, int len) const
{
	if (pszText[index] == '\r')
		return true;
	return false;
}

bool AUITextTagReturn::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{
	if (OutputSubTags(inParams, outParams, context))
		return true;

	if (outParams._pCacheMan && outParams._pCacheMan->GetCache().empty())
	{
		// 修复文本为"\r..."时，行数计算误差1的bug
		outParams._pCacheMan->AddCache(
			new AUITextTagTextCache(
				NULL, context.pCurFont,
				A3DPOINT2(context.nCurX - inParams._x, context.nOffsetYFromFirstLine), 
				A3DPOINT2(0, context.nLineHeightThis), 
				context.index, 0, false, 0, 0, false, 0, 0.0f),
			context.nSection, context.nLines);
	}
		
	UpdateNewline(inParams, context, true);
	context.nSection += 1;
	if (outParams._pCacheMan)
	{
		outParams._pCacheMan->AddCache(
			new AUITextTagTextCache(
				NULL, context.pCurFont,
				A3DPOINT2(context.nCurX - inParams._x, context.nOffsetYFromFirstLine), 
				A3DPOINT2(0, context.nLineHeightThis), 
				context.index, 0, false, 0, 0, false, 0, 0.0f),
			context.nSection, context.nLines);
	}
	INC_INDEX(context, 1);
	if (context.bExceedBottom)
		context.nLinesThisPage -= 1;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrl
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrl, AUITextTag)

inline bool AUITextTagCtrl::IsMyType(const ACHAR* pszText, int index, int len) const
{
	// ^*
	return pszText[index] == '^' && (index+1>=len || (pszText[index+1] != '&' && pszText[index+1] != '^'));
}

bool AUITextTagCtrl::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{
	INC_INDEX(context, 1);
	if (!m_pNode)
		return false;
	AUITAG_NODE* pNode = m_pNode->_pFirstChild;
	while (pNode)
	{
		if (!pNode->_pTag)
			return AUI_ReportError("AUITextTagCtrl::Output pTag is null");
		
		if (pNode->_pTag->IsMyType(inParams._pszText, context.index, context._len))
			return pNode->_pTag->Output(inParams, context, outParams);

		pNode = pNode->_pNext;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrlFont
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrlFont, AUITextTagCtrl)

inline bool AUITextTagCtrlFont::IsMyType(const ACHAR* pszText, int index, int len) const
{
	return index + 3 < len && pszText[index] == 'O';
}

bool AUITextTagCtrlFont::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{	
	if (OutputSubTags(inParams, outParams, context))
		return true;

	int nIndex = ((inParams._pszText[context.index + 1] - '0') * 10 + inParams._pszText[context.index + 2] - '0') * 10 + inParams._pszText[context.index + 3] - '0';
	if( nIndex > 0 && inParams._pVecOtherFonts && nIndex <= (int)inParams._pVecOtherFonts->size() && (*inParams._pVecOtherFonts)[nIndex - 1].pFont )
	{
		context.pCurFont = (*inParams._pVecOtherFonts)[nIndex - 1].pFont;
		context.nCurShadow = (*inParams._pVecOtherFonts)[nIndex - 1].nShadow;
		if((context.nCurShadow & 0xFF) == context.nCurShadow)
		{
			context.nCurShadow <<= 24;
		}
	}
	else
	{
		context.pCurFont = inParams._pFont;
		context.nCurShadow = inParams._nShadow;
	}
	INC_INDEX(context, 4);

	int nCurLineHeigh =
		(inParams._bVertical && !context.bRotateLayout)
		? context.pCurFont->GetFontWidth()
		: context.pCurFont->GetFontHeight();
	if (nCurLineHeigh > context.nLineHeightThis)
		context.nLineHeightThis = nCurLineHeigh;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrlUnderline
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrlUnderline, AUITextTagCtrl)

inline bool AUITextTagCtrlUnderline::IsMyType(const ACHAR* pszText, int index, int len) const
{
	// ^UFFFFFF	
	return index + 7 <= len && pszText[index] == 'U';
}

bool AUITextTagCtrlUnderline::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{	
	if (OutputSubTags(inParams, outParams, context))
		return true;
			
	context.bUnderLine = !context.bUnderLine;
	context.bUnderLineSameColor = false;
	
	if( outParams._pCacheMan || (!context.bExceedBottom && inParams._bDrawToScreen ))
	{
		context.color_underline = 0;
		for( int j = 0; j < 6; j++ )
		{
			int nValue = inParams._pszText[context.index + 1 + j];
			if( nValue >= '0' && nValue <= '9' )
				nValue -= '0';
			else if( nValue >= 'a' && nValue <= 'f' )
				nValue = nValue - 'a' + 0xA;
			else if( nValue >= 'A' && nValue <= 'F' )
				nValue = nValue - 'A' + 0xA;
			else
				nValue = 0;
			context.color_underline = context.color_underline * 0x10 + nValue;
		}
		context.color_underline |= inParams._nAlpha << 24;
	}
	
	INC_INDEX(context, 7);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrlUnderlineX
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrlUnderlineX, AUITextTagCtrl)

inline bool AUITextTagCtrlUnderlineX::IsMyType(const ACHAR* pszText, int index, int len) const
{
	// ^u
	return pszText[index] == 'u';
}

bool AUITextTagCtrlUnderlineX::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{			
	if (OutputSubTags(inParams, outParams, context))
		return true;
		
	context.bUnderLine = !context.bUnderLine;
	context.bUnderLineSameColor = true;
	
	INC_INDEX(context, 1);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrlWordspace
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrlWordspace, AUITextTagCtrl)

inline bool AUITextTagCtrlWordspace::IsMyType(const ACHAR* pszText, int index, int len) const
{
	//^o
	return pszText[index] == 'o';
}

bool AUITextTagCtrlWordspace::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{	
	if (OutputSubTags(inParams, outParams, context))
		return true;

	context.nCurX += inParams._nWordWidth * inParams._fWindowScale;	
	
	INC_INDEX(context, 1);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrlChannel
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrlChannel, AUITextTagCtrl)

inline bool AUITextTagCtrlChannel::IsMyType(const ACHAR* pszText, int index, int len) const
{
	//^p
	return pszText[index] == 'p';
}

bool AUITextTagCtrlChannel::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{
	if (OutputSubTags(inParams, outParams, context))
		return true;

	context.bInChannelQuote = !context.bInChannelQuote;
	if (!context.bInChannelQuote)
	{
		// channel quote finished
		if (context.iChannelQuoteBeginPos < context.index)
			context.cl.strName = ACString(inParams._pszText+context.iChannelQuoteBeginPos, context.index - context.iChannelQuoteBeginPos - 1);
		context.cl.rc.right = context.nCurX;
		context.cl.rc.bottom = context.nCurY + context.nLineHeightThis;
		if (outParams._pItemRcInfo && outParams._pItemRcInfo->pVecChannelLinks)
			outParams._pItemRcInfo->pVecChannelLinks->push_back(context.cl);
		if (outParams._pCacheMan)
			outParams._pCacheMan->AddCache(
			new AUITextTagCtrlChannelCache(
			context.iChannelQuoteBeginPos,
			A3DPOINT2(context.cl.rc.left - inParams._x, context.nOffsetYFromFirstLine), 
			A3DPOINT2(context.cl.rc.Width(), context.cl.rc.Height()),
			context.cl.strName.GetLength()), context.nSection, context.nLines);
	}
	else
	{
		context.color_clQuote = context.color;		
		if (AUI_ConvertColor(inParams._pszText + context.index + 1, context.color_clQuote))
			INC_INDEX(context, 7);
		
		context.cl.strName = _AL("");
		context.iChannelQuoteBeginPos = context.index + 1;
		context.cl.rc.SetRect(context.nCurX, context.nCurY, context.nCurX, context.nCurY);
	}
	
	INC_INDEX(context, 1);
	return true;
}


bool AUITextTagCtrlChannel::AUITextTagCtrlChannelCache::Output(const ACHAR* pText, const POINT& ptOffset, 
															   const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const
{
	if (outParams._pItemRcInfo && outParams._pItemRcInfo->pVecChannelLinks)
	{
		AUITEXTAREA_CHANNEL_LINK cl;
		cl.rc = A3DRECT(m_ptPos.x + ptOffset.x, m_ptPos.y + ptOffset.y, m_ptPos.x + ptOffset.x + m_ptSize.x, m_ptPos.y + ptOffset.y + m_ptSize.y);
		cl.strName = ACString(pText+m_IndexInStr, m_nLenstr);
		outParams._pItemRcInfo->pVecChannelLinks->push_back(cl);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrlDefaultColor
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrlDefaultColor, AUITextTagCtrl)

inline bool AUITextTagCtrlDefaultColor::IsMyType(const ACHAR* pszText, int index, int len) const
{
	//^N
	return (pszText[index] == 'N');
}

bool AUITextTagCtrlDefaultColor::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{	
	if (OutputSubTags(inParams, outParams, context))
		return true;

	context.color = context._clDefault;		
	INC_INDEX(context, 1);
	return true;
}


//////////////////////////////////////////////////////////////////////////
// AUITextTagCtrlDefaultColor
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagCtrlColor, AUITextTagCtrl)

inline bool AUITextTagCtrlColor::IsMyType(const ACHAR* pszText, int index, int len) const
{
	//^FFFFFF
	if (index+6 > len)
		return false;
	for (int i=index; i<index+6; i++)
	{
		if ((pszText[i] >= '0' && pszText[i] <= '9') || 
			(pszText[i] >= 'a' && pszText[i] <= 'f') ||
			(pszText[i] >= 'A' && pszText[i] <= 'F') )
			continue;
		return false;
	}
	return true;
}

bool AUITextTagCtrlColor::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{	
	if (OutputSubTags(inParams, outParams, context))
		return true;

	if( outParams._pCacheMan || (!context.bExceedBottom && inParams._bDrawToScreen ))
	{
		context.color = 0;
		for( int j = 0; j < 6; j++ )
		{
			int nValue = inParams._pszText[context.index + j];
			if( nValue >= '0' && nValue <= '9' )
				nValue -= '0';
			else if( nValue >= 'a' && nValue <= 'z' )
				nValue = nValue - 'a' + 0xA;
			else if( nValue >= 'A' && nValue <= 'Z' )
				nValue = nValue - 'A' + 0xA;
			else
				nValue = 0;
			context.color = context.color * 0x10 + nValue;
		}
		context.color |= inParams._nAlpha << 24;
	}

	INC_INDEX(context, 6);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// AUITextTagItem
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagItem, AUITextTag)

inline bool AUITextTagItem::IsMyType(const ACHAR* pszText, int index, int len) const
{
	return IsEditboxItemCode(pszText[index]);
}

bool AUITextTagItem::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{		
	EditBoxItemBase* pItem = NULL;
	if (!inParams._pItemsSet || NULL == (pItem = inParams._pItemsSet->GetItemByChar(inParams._pszText[context.index])))
	{
		INC_INDEX(context, 1);
		return true;
	}

	ACHAR code = inParams._pszText[context.index];
	INC_INDEX(context, 1);
	
	if (!m_pNode)
		return false;
	AUITAG_NODE* pNode = m_pNode->_pFirstChild;
	while (pNode)
	{
		AUITextTagItem* pTag = dynamic_cast<AUITextTagItem*>(pNode->_pTag);
		if (!pTag)
			return AUI_ReportError("AUITextTagItem::Output pTag=null");
		if (!pTag->IsMyType(pItem))
		{
			pNode = pNode->_pNext;
			continue;
		}

		int oldx = context.nCurX;
		int oldy = context.nCurY;
		bool bval = pTag->Output(inParams, context, outParams, pItem, code);
		if (pItem->GetType() != enumEIEmotion && outParams._pItemRcInfo && outParams._pItemRcInfo->pVecEBItems)
		{
			AUITEXTAREA_EDITBOX_ITEM l;
			l.rc.SetRect(oldx, oldy, context.nCurX, oldy + context.nLineHeightThis);
			l.m_pItem = pItem;
			outParams._pItemRcInfo->pVecEBItems->push_back(l);
		}
		return bval;
	}	

	return true;
}

bool AUITextTagItem::OutputSubTags(const TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams, TAG_RESOLVER_CONTEXT& context, 
								   EditBoxItemBase* pItem, ACHAR code) const
{
	if (!m_pNode)
		return false;
	AUITAG_NODE* pNode = m_pNode->_pFirstChild;
	if (!pNode)
		return false;

	while (pNode)
	{
		AUITextTagItem* pTag = dynamic_cast<AUITextTagItem*> (pNode->_pTag);
		if (!pTag)
			return AUI_ReportError("AUITextTagItem::OutputSubTags() pTag=NULL");
		if (pTag->IsMyType(pItem))
			return pTag->Output(inParams, context, outParams, pItem, code);
		pNode = pNode->_pNext;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagItemImage
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagItemImage, AUITextTagItem)
inline bool AUITextTagItemImage::IsMyType(const EditBoxItemBase* pItem) const
{
	return (pItem && pItem->GetType() == enumEIImage);
}

bool AUITextTagItemImage::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, 
								 EditBoxItemBase* pItem, ACHAR code) const
{		
	if (OutputSubTags(inParams, outParams, context, pItem, code))
		return true;

	if( !pItem || !inParams._pVecImage || pItem->GetImageIndex() >= (int)inParams._pVecImage->size() )
		return true;

	A2DSprite* pSprite = (*inParams._pVecImage)[pItem->GetImageIndex()];
	if( !pSprite )
		return true;
		
	A2DSpriteItem *pSpriteItem = pSprite->GetItem(pItem->GetImageFrame());
	if( !pSpriteItem )
		return true;

	SIZE szItem = {0, 0};	
	float fImgScale = inParams._fWindowScale * pItem->GetImageScale();
	szItem.cx = (int)(pSpriteItem->GetRect().Width() * fImgScale);
	szItem.cy = (int)(pSpriteItem->GetRect().Height() * fImgScale);
	if (inParams._bSmallImage && szItem.cy > context.pCurFont->GetFontHeight())
	{
		fImgScale *= (float)context.pCurFont->GetFontHeight() / szItem.cy;
		szItem.cx = (int)(pSpriteItem->GetRect().Width() * fImgScale);
		szItem.cy = (int)(pSpriteItem->GetRect().Height() * fImgScale);
	}
	pSprite->SetCurrentItem(pItem->GetImageFrame());
	pSprite->SetColor(pItem->GetColor());


	if( IsNewLineNeed(inParams, context, szItem.cx) && !UpdateNewline(inParams, context) )
		return true;
	
	if( szItem.cy > context.nLineHeightThis )
		context.nLineHeightThis = szItem.cy;
	
	// Cache
	if (outParams._pCacheMan)
	{
		AUITextTagCache* pCache = new AUITextTagItemImageCache(
										context.index,
										A3DPOINT2(context.nCurX - inParams._x, context.nOffsetYFromFirstLine),
										A3DPOINT2(szItem.cx, szItem.cy),
										pItem,
										pSprite, 
										pItem->GetImageFrame(), 
										fImgScale, 
										pItem->GetColor());
		pCache->SetItemCode(code);
		outParams._pCacheMan->AddCache(pCache, context.nSection, context.nLines);
	}
	
	if( !CanDraw(inParams, context) )
	{
		context.nCurX += szItem.cx;
		return true;
	}

	if( !inParams._pRect ||
		(context.nCurX < inParams._pRect->right && context.nCurX >= inParams._pRect->left))
	{
		pSprite->SetScaleX(fImgScale);
		pSprite->SetScaleY(fImgScale);
		bool bval = pSprite->DrawToInternalBuffer(context.nCurX, context.nCurY);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	}	
	context.nCurX += szItem.cx;

	return true;
}

bool AUITextTagItemImage::AUITextTagItemImageCache::Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const
{
	if (!m_pSprite)
		return true;
	A2DSpriteItem *pSpriteItem = m_pSprite->GetItem(m_nItem);
	if( !pSpriteItem )
		return true;

	A3DPOINT2 pt(m_ptPos.x + ptOffset.x, m_ptPos.y + ptOffset.y);
	if (inParams._pClip)
	{
		if (pt.x < inParams._pClip->left || pt.y < inParams._pClip->top ||
			pt.x + m_ptSize.x > inParams._pClip->right || pt.y + m_ptSize.y > inParams._pClip->bottom)
			return true;
	}

	if (inParams._bRender)
	{
		m_pSprite->SetCurrentItem(m_nItem);
		m_pSprite->SetColor(m_color);
		m_pSprite->SetScaleX(m_fScale);
		m_pSprite->SetScaleY(m_fScale);
		
		bool bval = m_pSprite->DrawToInternalBuffer(pt.x, pt.y);
		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	}	

	if (m_pItem && outParams._pItemRcInfo && outParams._pItemRcInfo->pVecEBItems)
	{
		AUITEXTAREA_EDITBOX_ITEM l;
		l.rc.SetRect(m_ptPos.x+ptOffset.x, m_ptPos.y+ptOffset.y, m_ptPos.x+ptOffset.x+m_ptSize.x, m_ptPos.y+ptOffset.y+m_ptSize.y);
		l.m_pItem = m_pItem;
		outParams._pItemRcInfo->pVecEBItems->push_back(l);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagItemEmotion
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagItemEmotion, AUITextTagItem)
inline bool AUITextTagItemEmotion::IsMyType(const EditBoxItemBase* pItem) const
{
	return (pItem && pItem->GetType() == enumEIEmotion);
}

bool AUITextTagItemEmotion::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, 
								   EditBoxItemBase* pItem, ACHAR code) const
{	
	if (OutputSubTags(inParams, outParams, context, pItem, code))
		return true;

	if (!pItem)
		return true;

	int nNumber = 0;
	int nSet = 0;
	UnmarshalEmotionInfo(pItem->GetInfo(), nSet, nNumber);
	AUITEXTAREA_EMOTION* pem = NULL;
	A2DSprite* pSprite = NULL;
	if (inParams._bMultiEmotionSet)
	{
		abase::vector<AUITEXTAREA_EMOTION>** ppVecEmotion = inParams._unEmotionInfo.stMultiEmotion._ppVecEmotion;
		if (ppVecEmotion && ppVecEmotion[nSet] && nNumber < (int)ppVecEmotion[nSet]->size())
			pem = &(*(ppVecEmotion[nSet]))[nNumber];
		pSprite = inParams._unEmotionInfo.stMultiEmotion._ppA2DSpriteEmotion[nSet];
	}
	else
	{
		abase::vector<AUITEXTAREA_EMOTION>* pVecEmotion = inParams._unEmotionInfo.stSingleEmotion._pVecEmotion;
		if (pVecEmotion && nNumber < (int)pVecEmotion->size())
			pem = &(*pVecEmotion)[nNumber];
		pSprite = inParams._unEmotionInfo.stSingleEmotion._pA2DSpriteEmotion;
	}

	if( !pem || !pSprite || !pSprite->GetItem(nNumber))
		return true;
	
	A3DRECT rcEmotion = pSprite->GetItem(nNumber)->GetRect();
	float fScaleX = inParams._fWindowScale;
	float fScaleY = inParams._fWindowScale;
	if (inParams._bSmallEmotion && context.pCurFont && rcEmotion.Height() != 0 )
	{
		float f = (float)context.pCurFont->GetFontHeight() / rcEmotion.Height();
		fScaleX = f;
		fScaleY = f;
	}
	pSprite->SetScaleX(fScaleX);
	pSprite->SetScaleY(fScaleY);
	int nEmotionW = rcEmotion.Width() * fScaleX;
	int nEmotionH = rcEmotion.Height() * fScaleY;
	
	if (IsNewLineNeed(inParams, context, inParams._bVertical ? nEmotionH : nEmotionW) && !UpdateNewline(inParams, context))
		return true;

	if( (inParams._bVertical ? nEmotionW : nEmotionH) > context.nLineHeightThis )
		context.nLineHeightThis = inParams._bVertical ? nEmotionW : nEmotionH;

	// cache
	if (outParams._pCacheMan)
	{
		AUITextTagCache* pCache = new AUITextTagItemEmotionCache(
										context.index,
										A3DPOINT2(context.nCurX - inParams._x, context.nOffsetYFromFirstLine), 
										A3DPOINT2(nEmotionW, nEmotionH),
										pSprite, pem);
//		if (inParams._pItemsSet)
//			inParams._pItemsSet->DelItemByChar(code);
		outParams._pCacheMan->AddCache(pCache, context.nSection, context.nLines);
	}

	if( !CanDraw(inParams, context) )
	{
		if(inParams._bVertical)
			context.nCurY += nEmotionH;
		else
			context.nCurX += nEmotionW;
		return true;
	}
	
	if( !inParams._pRect || 
		(context.nCurX >= inParams._pRect->left && context.nCurX + nEmotionW <= inParams._pRect->right) )
	{
		int nBase = pem->nStartPos;
		int nFrame = pem->nNumFrames;
		int nThisTick = int(GetTickCount() * 0.06) % pem->nFrameTick[nFrame - 1];
		int nOffset = 0;
		while( nOffset < nFrame - 1 && nThisTick > pem->nFrameTick[nOffset] )
			nOffset++;

		pSprite->SetCurrentItem(nBase + nOffset);
		int nOldAlpha = pSprite->GetAlpha();
		pSprite->SetAlpha(inParams._nAlpha);
		bool bval = pSprite->DrawToInternalBuffer(context.nCurX, context.nCurY);
		pSprite->SetAlpha(nOldAlpha);

		if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	}
	
	if(inParams._bVertical)
		context.nCurY += nEmotionH;
	else
		context.nCurX += nEmotionW;

	return true;
}

bool AUITextTagItemEmotion::AUITextTagItemEmotionCache::Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const
{
	if (!m_pSprite || !m_pem)
		return true;

	if (!inParams._bRender)
		return true;

	int nBase = m_pem->nStartPos;
	int nFrame = m_pem->nNumFrames;
	int nThisTick = int(GetTickCount() * 0.06) % m_pem->nFrameTick[nFrame - 1];
	int nOffset = 0;
	while (nOffset < nFrame - 1 && nThisTick > m_pem->nFrameTick[nOffset])
		nOffset++;
	m_pSprite->SetCurrentItem(nBase + nOffset);

	A2DSpriteItem* pItem = m_pSprite->GetItem(nBase + nOffset);
	if (!pItem)
		return true;
	A3DRECT rc = pItem->GetRect();
	if (rc.Width() > 0 && rc.Height() > 0)
	{
		m_pSprite->SetScaleX((float)m_ptSize.x / rc.Width());
		m_pSprite->SetScaleY((float)m_ptSize.y / rc.Height());
	}

	A3DPOINT2 pt(m_ptPos.x + ptOffset.x, m_ptPos.y + ptOffset.y);
	if (inParams._pClip)
	{
		if (pt.x < inParams._pClip->left || pt.y < inParams._pClip->top ||
			pt.x + m_ptSize.x > inParams._pClip->right || pt.y + m_ptSize.y > inParams._pClip->bottom)
			return true;
	}

	bool bval = m_pSprite->DrawToInternalBuffer(pt.x, pt.y);
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagItemText
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagItemText, AUITextTagItem)
inline bool AUITextTagItemText::IsMyType(const EditBoxItemBase* pItem) const
{
	return (pItem && (pItem->GetType() != enumEIEmotion && pItem->GetType() != enumEIImage && pItem->GetType() != enumEIBox));
}

bool AUITextTagItemText::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, 
								EditBoxItemBase* pItem, ACHAR code) const
{	
	if (OutputSubTags(inParams, outParams, context, pItem, code))
		return true;

	if (!pItem)
		return true;

	A3DPOINT2 ptSize = context.pCurFont->GetTextExtent(pItem->GetName(), -1, inParams._bVertical);
	A3DCOLOR text_color = (inParams._nAlpha << 24) | (0xFFFFFF & pItem->GetColor());
	A3DCOLOR shadow_color = (inParams._nAlpha << 24) | (0xFFFFFF & context.nCurShadow);

	if (IsNewLineNeed(inParams, context, inParams._bVertical ? ptSize.y : ptSize.x) && !UpdateNewline(inParams, context))
		return true;

	//cache
	if (outParams._pCacheMan)
	{
		AUITextTagCache* pCache = new AUITextTagTextCache(
				pItem,
				context.pCurFont,
				A3DPOINT2(context.nCurX - inParams._x, context.nOffsetYFromFirstLine),
				ptSize,
				context.index, 
				-1,
				(context.nCurShadow & 0xFF000000) ? true : false,
				shadow_color,
				text_color, 
				context.bUnderLine,
				context.bUnderLineSameColor ? text_color : context.color_underline, 
				0.08f * inParams._fWindowScale);
		pCache->SetItemCode(code);
		outParams._pCacheMan->AddCache(pCache, context.nSection, context.nLines);
	}

	if( (inParams._bVertical ? ptSize.x : ptSize.y) > context.nLineHeightThis )
		context.nLineHeightThis = inParams._bVertical ? ptSize.x : ptSize.y;

	if( !CanDraw(inParams, context) )
	{
		if(inParams._bVertical)
			context.nCurY += ptSize.y;
		else
			context.nCurX += ptSize.x;
		return true;
	}

	if(inParams._bVertical)
	{
		context.pCurFont->TextOutVert(context.nCurX, context.nCurY, pItem->GetName(), text_color,
			(context.nCurShadow & 0xFF000000) ? true : false, shadow_color, 0, -1, 1.0f,
			context.bUnderLine, context.bUnderLineSameColor ? text_color : context.color_underline, 0.08f * inParams._fWindowScale, 2);
	}
	else
	{
		if( inParams._pRect )
		{
			context.pCurFont->DrawText(context.nCurX, context.nCurY, pItem->GetName(), text_color, inParams._pRect,
				(context.nCurShadow & 0xFF000000) ? true : false, shadow_color, 0, -1, 1.0f,
				context.bUnderLine, context.bUnderLineSameColor ? text_color : context.color_underline, 0.08f * inParams._fWindowScale);
		}
		else
		{
			context.pCurFont->TextOut(context.nCurX, context.nCurY, pItem->GetName(), text_color,
				(context.nCurShadow & 0xFF000000) ? true : false, shadow_color, 0, -1, 1.0f,
				context.bUnderLine, context.bUnderLineSameColor ? text_color : context.color_underline, 0.08f * inParams._fWindowScale);
		}
	}

	if(inParams._bVertical)
		context.nCurY += ptSize.y;
	else
		context.nCurX += ptSize.x;

	return true;
}


//////////////////////////////////////////////////////////////////////////
// AUITextTagBox
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagItemBox, AUITextTagItem)
inline bool AUITextTagItemBox::IsMyType(const EditBoxItemBase* pItem) const
{
	return (pItem && (pItem->GetType() == enumEIBox));
}

bool AUITextTagItemBox::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, EditBoxItemBase* pItem, ACHAR code) const
{
	if (OutputSubTags(inParams, outParams, context, pItem, code))
		return true;

	if (!pItem || !context.pCurFont || !context.pCurFont->GetA3DEngine())
		return true;

	A3DPOINT2 ptSize(pItem->GetBoxWidth() * inParams._fWindowScale, context.nLineHeightThis * pItem->GetBoxHeightScale());
	int y = context.nCurY + (context.nLineHeightThis - ptSize.y) / 2;

	if (IsNewLineNeed(inParams, context, pItem->GetBoxWidth()) && !UpdateNewline(inParams, context))
		return true;

	A3DFlatCollector* pFC = context.pCurFont->GetA3DEngine()->GetA3DFlatCollector();
	A3DWireCollector* pWC = context.pCurFont->GetA3DEngine()->GetA3DWireCollector();

	// cache
	A3DCOLOR color = (inParams._nAlpha << 24) | (0xFFFFFF & pItem->GetColor());
	if (outParams._pCacheMan)
	{
		SIZE sz = {ptSize.x, ptSize.y};
		AUITextTagCache* pCache = new AUITextTagItemBoxCache(
											context.index,
											A3DPOINT2(context.nCurX - inParams._x, context.nOffsetYFromFirstLine), 
											ptSize,
											pFC, 
											pWC, 
											color, 
											context.color, 
											sz);
		pCache->SetItemCode(code);
		outParams._pCacheMan->AddCache(pCache, context.nSection, context.nLines);
	}

	if( !pFC || !CanDraw(inParams, context) )
	{
		context.nCurX += ptSize.x;
		return true;
	}
	
	A3DVECTOR3 aPos[4];
	aPos[0].Set(context.nCurX, y, 0.0f);
	aPos[1].Set(context.nCurX + ptSize.x, y, 0.0f);
	aPos[2].Set(context.nCurX + ptSize.x, y + ptSize.y, 0.0f);
	aPos[3].Set(context.nCurX, y + ptSize.y, 0.0f);
	
	static const WORD aIndices[6] = {0, 1, 2, 0, 2, 3};
	pFC->AddRenderData_2D(aPos, 4, aIndices, 6, color);

	if (pWC)
		pWC->AddRect_2D(context.nCurX, y, context.nCurX + ptSize.x, y + ptSize.y, context.color);
	
	context.nCurX += ptSize.x;

	return true;
}

bool AUITextTagItemBox::AUITextTagItemBoxCache::Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const
{
	if (!m_pFC && !m_pWC)
		return true;

	if (!inParams._bRender)
		return true;

	A3DVECTOR3 aPos[4];
	aPos[0].Set(m_ptPos.x + ptOffset.x, m_ptPos.y + ptOffset.y, 0.0f);
	aPos[1].Set(m_ptPos.x + ptOffset.x + m_szBox.cx, m_ptPos.y + ptOffset.y, 0.0f);
	aPos[2].Set(m_ptPos.x + ptOffset.x + m_szBox.cx, m_ptPos.y + ptOffset.y + m_szBox.cy, 0.0f);
	aPos[3].Set(m_ptPos.x + ptOffset.x, m_ptPos.y + ptOffset.y + m_szBox.cy, 0.0f);

	if (inParams._pClip)
	{
		if (m_ptPos.x > inParams._pClip->right 
			|| m_ptPos.y > inParams._pClip->bottom 
			|| m_ptPos.x + m_szBox.cx < inParams._pClip->left 
			|| m_ptPos.y + m_szBox.cy < inParams._pClip->top)
			return true;
	}
	
	static const WORD aIndices[6] = {0, 1, 2, 0, 2, 3};

	if (m_pFC)
		m_pFC->AddRenderData_2D(aPos, 4, aIndices, 6, m_clFC);

	if (m_pWC)
		m_pWC->AddRect_2D(m_ptPos.x + ptOffset.x, m_ptPos.y + ptOffset.y, m_ptPos.x + m_szBox.cx, m_ptPos.y + ptOffset.y + m_szBox.cy, m_clWC);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagQuote
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagQuote, AUITextTag)
inline bool AUITextTagQuote::IsMyType(const ACHAR* pszText, int index, int len) const
{
	return pszText[index] == '&';
}

bool AUITextTagQuote::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{		
	if (OutputSubTags(inParams, outParams, context))
		return true;

	context.bInQuote = !context.bInQuote;
	if (!context.bInQuote)
	{
		if (context.iQuoteBeginPos < context.index)
			context.nl.strName = ACString(inParams._pszText+context.iQuoteBeginPos, context.index - context.iQuoteBeginPos);
		context.nl.rc.right = context.nCurX;
		context.nl.rc.bottom = context.nCurY + context.nLineHeightThis;
		if (outParams._pItemRcInfo && outParams._pItemRcInfo->pVecNameLinks)
			outParams._pItemRcInfo->pVecNameLinks->push_back(context.nl);
		if (outParams._pCacheMan)
			outParams._pCacheMan->AddCache(
				new AUITextTagQuoteCache(
						context.iQuoteBeginPos,
						A3DPOINT2(context.nl.rc.left - inParams._x, context.nOffsetYFromFirstLine), 
						A3DPOINT2(context.nl.rc.Width(), context.nl.rc.Height()),
						context.nl.strName.GetLength()), context.nSection, context.nLines);
	}
	else
	{
		context.color_quote = A3DCOLORRGBA(255, 220, 138, inParams._nAlpha);		
		if (AUI_ConvertColor(inParams._pszText + context.index + 1, context.color_quote))
			INC_INDEX(context, 7);

		context.nl.strName = _AL("");
		context.iQuoteBeginPos = context.index + 1;
		context.nl.rc.SetRect(context.nCurX, context.nCurY, context.nCurX, context.nCurY);
	}

	INC_INDEX(context, 1);
	return true;
}

bool AUITextTagQuote::AUITextTagQuoteCache::Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const
{
	if (outParams._pItemRcInfo && outParams._pItemRcInfo->pVecNameLinks)
	{
		AUITEXTAREA_NAME_LINK nl;
		nl.rc = A3DRECT(m_ptPos.x + ptOffset.x, m_ptPos.y + ptOffset.y, m_ptPos.x + ptOffset.x + m_ptSize.x, m_ptPos.y + ptOffset.y + m_ptSize.y);
		nl.strName = ACString(pText+m_IndexInStr, m_nLenstr);
		outParams._pItemRcInfo->pVecNameLinks->push_back(nl);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagFakReturn
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagFakReturn, AUITextTag)
inline bool AUITextTagFakReturn::IsMyType(const ACHAR* pszText, int index, int len) const
{
	return pszText[index] == '\n';
}

bool AUITextTagFakReturn::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{	
	if (OutputSubTags(inParams, outParams, context))
		return true;

	INC_INDEX(context, 1);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagTable
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_AUITEXT_TAG(AUITextTagTable, AUITextTag)
inline bool AUITextTagTable::IsMyType(const ACHAR* pszText, int index, int len) const
{
	return pszText[index] == '\v';
}

bool AUITextTagTable::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
{	
	if (OutputSubTags(inParams, outParams, context))
		return true;

	int nWidthThis = context.nCurX - inParams._x;
	nWidthThis = (nWidthThis / context._nTabW + 1) * context._nTabW;
	INC_INDEX(context, 1);

	context.nCurX = inParams._x + nWidthThis;
	return true;
}


//////////////////////////////////////////////////////////////////////////
// AUITextTagCacheManager
//////////////////////////////////////////////////////////////////////////
AUITextTagCacheManager::AUITextTagCacheManager()
{
}

AUITextTagCacheManager::~AUITextTagCacheManager()
{
	Release();
}

void AUITextTagCacheManager::Release()
{
	for (size_t i=0; i<m_aCaches.size(); i++)
	{
		if (m_aCaches[i])
			delete m_aCaches[i];
	}
	m_context.clear();
	m_aCaches.clear();
	m_nLines = -1;
	m_str.Empty();
}

void AUITextTagCacheManager::ClearCache()
{
	Release();
}

void AUITextTagCacheManager::AddCache(AUITextTagCache* pCache, int nSectionIndex, int nLineIndex)
{
	if (!pCache)
		return;

	if (!m_aCaches.empty() && (int)m_aCaches.size()-1 == nSectionIndex)
	{
		if (m_nLines == nLineIndex + 1)
		{
			m_aCaches.back()->m_aCacheLines.back()->AddCache(pCache);
		}
		else
		{
			m_aCaches.back()->m_aCacheLines.push_back(new cacheLine(pCache->m_ptPos.y, pCache));
		}
	}
	else
	{
		m_aCaches.push_back(new cacheSection(new cacheLine(pCache->m_ptPos.y, pCache)));
	}

	m_nLines = nLineIndex + 1;
}

bool AUITextTagCacheManager::AddCache(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams)
{
	m_str += inParams._pszText;
	inParams._nStartLine = 0;
	inParams._pszText = m_str;

	if (!m_aCaches.empty())
	{
		m_context._len = m_str.GetLength();
	}
	else
	{
		if (!AUIRichTextPrinter::InitContext(inParams, outParams, m_context))
			return false;
	}

	outParams._pCacheMan = this;
	if (!AUIRichTextPrinter::GetSingleton().Output(inParams, m_context, outParams))
		return AUI_ReportError("AUITextTagCacheManager::AddCache.AUI_TextOutFormat Fail");

	return true;
}

bool AUITextTagCacheManager::Output(AUITAG_CACHE_IN_PARAMS& inParam, TAG_OP_OUT_PARAMS& outParams) const
{
	if (outParams._pnLinesThisPage)
		*(outParams._pnLinesThisPage) = 0;

	if (m_aCaches.empty() || m_aCaches[0]->m_aCacheLines.empty())
		return true;

	// Locate section and line
	int idSection = 0;
	int idLine = 0;
	int nCurLine = -1;
	for (idSection=0; idSection<(int)m_aCaches.size(); idSection++)
	{
		for (idLine=0; idLine<(int)m_aCaches[idSection]->m_aCacheLines.size(); idLine++)
		{
			nCurLine++;
			if (nCurLine >= inParam._nStartLine)
				break;
		}		
		if (nCurLine >= inParam._nStartLine)
			break;
	}
	
	if (nCurLine < inParam._nStartLine)
		return true;

	// now start render
	POINT ptOffset = {inParam._x, inParam._y + m_aCaches[0]->m_aCacheLines[0]->m_nOffsetY - m_aCaches[idSection]->m_aCacheLines[idLine]->m_nOffsetY}; //To Screen TopLeft
	for (; idSection < (int)m_aCaches.size(); idSection++)
	{
		for(; idLine < (int)m_aCaches[idSection]->m_aCacheLines.size(); idLine++)
		{
			const cacheLine* pLine = m_aCaches[idSection]->m_aCacheLines[idLine];
			if (inParam._pClip && ptOffset.y + pLine->m_nOffsetY + pLine->m_nLineH > inParam._pClip->bottom)
				return true;

			for (int iTag = 0; iTag < (int)pLine->m_aCaches.size(); iTag++)
			{
				if (pLine->m_aCaches[iTag]->m_ptSize.y + ptOffset.y + pLine->m_nOffsetY > inParam._pClip->bottom)
					return true;

				if (!pLine->m_aCaches[iTag]->Output(m_str, ptOffset, inParam, outParams))
					return false;
			}
			if (outParams._pnLinesThisPage)
				(*outParams._pnLinesThisPage)++;
		}
		idLine = 0;
	}

	return true;
}

int AUITextTagCacheManager::SetMaxSection(int nMaxSection, EditBoxItemsSet& itemsSet)
{
	if (nMaxSection >= (int)m_aCaches.size() || nMaxSection < 0)
		return m_nLines;

	if (nMaxSection == 0)
	{
		ClearCache();
		return m_nLines;
	}

	int nFirstSection = m_aCaches.size() - nMaxSection;
	// update Y
	if (m_aCaches[nFirstSection]->m_aCacheLines.empty())
		return m_nLines;
	int nOffY = m_aCaches[nFirstSection]->m_aCacheLines.back()->m_nOffsetY;
	for (int i=nFirstSection; i<(int)m_aCaches.size(); i++)
	{
		m_aCaches[i]->Offset(-nOffY);
	}
	m_context.nOffsetYFromFirstLine -= nOffY;
	m_context.nCurY -= nOffY;

	// remove sections & update context
	for (int iLimit = 0; iLimit < nFirstSection; iLimit++)
	{
		m_nLines -= m_aCaches[iLimit]->m_aCacheLines.size();
		m_context.nLines -= m_aCaches[iLimit]->m_aCacheLines.size();
		m_context.nSection--;
		m_aCaches[iLimit]->ClearItem(itemsSet);
		delete m_aCaches[iLimit];
	}
	m_aCaches.erase(m_aCaches.begin(), m_aCaches.begin() + nFirstSection);

	return m_nLines;
}

//////////////////////////////////////////////////////////////////////////
// AUITextTagTextCache
//////////////////////////////////////////////////////////////////////////
bool AUITextTagTextCache::Output(const ACHAR* pszText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const
{
	if (!m_pFont || !pszText)
		return true;
	
	if (inParams._pClip && m_pFont->GetFontHeight() + m_ptPos.y + ptOffset.y > inParams._pClip->bottom)
		return true;

	const ACHAR* pszContent = NULL;
	int iTextLen = -1;
	if (m_pItem)
		pszContent = m_pItem->GetName();
	else
	{
		pszContent = pszText + m_IndexInStr;
		iTextLen = m_nLenStr;
	}

	DWORD clr = (m_clText & 0xffffff) | (inParams._nAlpha << 24);
	DWORD clrShadow = m_clShadow;
	if((clrShadow & 0xFF) == clrShadow) clrShadow = A3DCOLORRGBA(0, 0, 0, m_clShadow * inParams._nAlpha / 255);
	else clrShadow = (clrShadow & 0x00FFFFFF) | (((DWORD)(((clrShadow & 0xFF000000) >> 24) * inParams._nAlpha / 255)) << 24);
	if (inParams._bRender)
	{
		if (!inParams._pClip)
		{
			m_pFont->TextOut(m_ptPos.x+ptOffset.x, m_ptPos.y+ptOffset.y, pszContent, clr,
				m_bShadow, clrShadow, 0, iTextLen, 1.0f, m_bUnderline, m_clUnderline, m_fThickUnderline);
		}
		else
		{
			m_pFont->DrawText(m_ptPos.x+ptOffset.x, m_ptPos.y+ptOffset.y, pszContent, clr, inParams._pClip,
				m_bShadow, clrShadow, 0, iTextLen, 1.0f, m_bUnderline, m_clUnderline, m_fThickUnderline);
		}
	}	

	if (m_pItem && outParams._pItemRcInfo && outParams._pItemRcInfo->pVecEBItems)
	{
		AUITEXTAREA_EDITBOX_ITEM l;
		l.rc.SetRect(m_ptPos.x+ptOffset.x, m_ptPos.y+ptOffset.y, m_ptPos.x+ptOffset.x+m_ptSize.x, m_ptPos.y+ptOffset.y+m_ptSize.y);
		l.m_pItem = m_pItem;
		outParams._pItemRcInfo->pVecEBItems->push_back(l);
	}
	
	return true;
}