// Filename	: AUITextArea.cpp
// Creator	: Tom Zhou
// Date		: December 17, 2004
// Desc		: AUITextArea is the class of text area control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "CSplit.h"

#include "AUIDef.h"
#include "AUICommon.h"
#include "AUIDialog.h"
#include "AUITextArea.h"
#include "AUIManager.h"
#include "AUICTranslate.h"
#include "A3DFlatCollector.h"
#include "AUITag.h"

//////////////////////////////////////////////////////////////////////////
// AUITextTagQuoteEx
class AUITextTagQuoteTA : public AUITextTagQuote
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagQuoteTA, AUITextTagQuote)
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

IMPLEMENT_AUITEXT_TAG(AUITextTagQuoteTA, AUITextTagQuote)
bool AUITextTagQuoteTA::Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const
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
						context.index - context.nl.strName.GetLength(),
						A3DPOINT2(context.nl.rc.left - inParams._x, context.nOffsetYFromFirstLine), 
						A3DPOINT2(context.nl.rc.Width(), context.nl.rc.Height()),
						context.nl.strName.GetLength()), context.nSection, context.nLines);
	}
	else
	{
		// default
		context.color_quote = A3DCOLORRGBA(255, 220, 138, inParams._nAlpha);		
		if (context.index + 2 < context._len && inParams._pszText[context.index+1] == '^')
		{
			// custom color
			if (AUI_ConvertColor(inParams._pszText + context.index + 2, context.color_quote))
				context.index += 8;
			else
			{
				switch (inParams._pszText[context.index+2])
				{
				case '0': context.color_quote = A3DCOLORRGB(102, 255, 204); break;
				case '1': context.color_quote = A3DCOLORRGB(255, 153, 255); break;
				case '2': context.color_quote = A3DCOLORRGB(255, 227, 161); break;
				}
				context.index += 2;
			}
		}		

		context.nl.strName = _AL("");
		context.iQuoteBeginPos = context.index + 1;
		context.nl.rc.SetRect(context.nCurX, context.nCurY, context.nCurX, context.nCurY);
	}

	context.index += 1;
	return true;
}

//////////////////////////////////////////////////////////////////////////
#define AUITEXTAREA_LINESPACE		2
#define AUITEXTAREA_SCROLL_DELAY	50

AUITextArea::AUITextArea() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_TEXTAREA;
	m_nFrameMode = 0;
	m_pAUIFrame = NULL;
	for( int i = 0; i < AUITEXTAREA_RECT_NUM; i++ )
		m_pA2DSprite[i] = NULL;
	m_pA2DSpriteEmotion = NULL;
	m_vecEmotion = NULL;
	m_pvecImageList = NULL;
	m_pvecOtherFonts = NULL;
	m_nState = AUITEXTAREA_STATE_NORMAL;
	m_bDragging = false;
	m_bOnlyShowText = false;
	m_bLeftScrollBar = false;
	m_bScrollToBottom = false;
	m_nScrollDelay = GetTickCount();
	m_ptBar.x = m_ptBar.y = m_nDragDist = 0;
	m_nFirstLine = m_nLines = m_nLinesPP = 0;
	m_ItemUnderLineMode = UM_NO_UNDERLINE;
	m_clItemUnderLine = A3DCOLORRGB(0, 0, 0);
	m_nMaxLines = -1;
	m_bTextTransparent = false;
	m_bTransScrollWheel = true;
	m_nItemMask = 0xffffffff;
	m_nLineSpace = AUITEXTAREA_LINESPACE;
	m_bForceRenderScroll = true;
	m_pTagCacheMan = NULL;
}

AUITextArea::~AUITextArea()
{
}

bool AUITextArea::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	ListBox property line format:
	 *
	 *		TEXT					Keyword
	 *		Server					Name
	 *		""						Command.
	 *		10 10 200 25			x y Width Height
	 *		1						String ID of font name
	 *		20						Font height
	 *		0 0 0 255				Text color, R, G, B, A
	 *		"ListFrame.bmp"			ImageFileName
	 *		"ListUp.bmp"			ImageFileName
	 *		"ListDown.bmp"			ImageFileName
	 *		"ListScroll.bmp"		ImageFileName
	 *		"ListBar.bmp"			ImageFileName
	 */
	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUITextArea::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;

	BEGIN_FAKE_WHILE2;

	// Font
	CHECK_BREAK2(pASF->GetNextToken(true));
	int idString = atoi(pASF->m_szToken);

	ACString strFontName = m_pParent->GetStringFromTable(idString);
	if( strFontName.GetLength() == 0 ) strFontName = _AL("方正细黑一简体");
	CSplit s(strFontName);
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("###"));
	m_szFontName = vec[0];
	if( vec.size() > 1 ) m_nShadow = a_atoi(vec[1]);
	if( vec.size() > 2 ) m_nOutline = a_atoi(vec[2]);
	if( vec.size() > 3 ) m_bBold = a_atoi(vec[3]) ? true : false;
	if( vec.size() > 4 ) m_bItalic = a_atoi(vec[4]) ? true : false;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nFontHeight = atoi(pASF->m_szToken);

	if (!SetFontAndColor(m_szFontName, m_nFontHeight,
		m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic))
		return AUI_ReportError(__LINE__, 1, "AUITextArea::Init(), failed to call SetFontAndColor()");

	// Text color
	CHECK_BREAK2(pASF->GetNextToken(true));
	int r = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int g = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int b = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int a = atoi(pASF->m_szToken);

	m_color = A3DCOLORRGBA(r, g, b, a);

	for( int i = AUITEXTAREA_RECT_MIN; i < AUITEXTAREA_RECT_MAX; i++ )
	{
		if (!pASF->GetNextToken(true))
			return AUI_ReportError(__LINE__, 1, "AUITextArea::Init(), failed to read from file");

		if (!InitIndirect(pASF->m_szToken, i))
			return AUI_ReportError(__LINE__, 1, "AUITextArea::Init(), failed to call InitIndirect()");
	}
	
	if( m_pParent->GetFileVersion() > 0 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		AUIOBJECT_SETPROPERTY p;
		p.i = atoi(pASF->m_szToken);
		SetProperty("Frame Mode", &p);
	}

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUITextArea::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUITextArea::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	m_szFontName = m_pParent->GetAUIManager()->GetDefaultFontName();
	m_nFontHeight = m_pParent->GetAUIManager()->GetDefaultFontSize();
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_BOOL_VALUE(pItem, _AL("TextTransparent"), m_bTextTransparent)
	XMLGET_BOOL_VALUE(pItem, _AL("TransScrollWheel"), m_bTransScrollWheel)
	XMLGET_BOOL_VALUE(pItem, _AL("LeftScrollBar"), m_bLeftScrollBar)
	XMLGET_INT_VALUE(pItem, _AL("LineSpace"), m_nLineSpace)
	XMLGET_INT_VALUE(pItem, _AL("LinkUnderLineMode"), m_ItemUnderLineMode)
	XMLGET_COLOR_VALUE(pItem, _AL("LinkUnderLineColor"), m_clItemUnderLine)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0 )
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameImage")) == 0 )
				{
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITEXTAREA_RECT_FRAME);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("UpImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITEXTAREA_RECT_UP);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("DownImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITEXTAREA_RECT_DOWN);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITEXTAREA_RECT_SCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("BarImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITEXTAREA_RECT_BAR);
				}
				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Text")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pChildItem, _AL("String"), m_szText);
			XMLGET_WSTRING_VALUE(pChildItem, _AL("FontName"), m_szFontName)
			XMLGET_INT_VALUE(pChildItem, _AL("FontSize"), m_nFontHeight)
			XMLGET_INT_VALUE(pChildItem, _AL("Shadow"), m_nShadow)
			m_nShadow = A3DCOLORRGBA(0, 0, 0, m_nShadow);
			XMLGET_COLOR_VALUE(pChildItem, _AL("ShadowColor"), m_nShadow)
			XMLGET_BOOL_VALUE(pChildItem, _AL("Bold"), m_bBold)
			XMLGET_BOOL_VALUE(pChildItem, _AL("Italic"), m_bItalic)
			XMLGET_BOOL_VALUE(pChildItem, _AL("FreeType"), m_bFreeType)
			XMLGET_COLOR_VALUE(pChildItem, _AL("Color"), m_color)
			XMLGET_INT_VALUE(pChildItem, _AL("Outline"), m_nOutline)
			XMLGET_COLOR_VALUE(pChildItem, _AL("OutlineColor"), m_clOutline)
			XMLGET_COLOR_VALUE(pChildItem, _AL("InnerTextColor"), m_clInnerText)
			XMLGET_BOOL_VALUE(pChildItem, _AL("FadeColor"), m_bInterpColor)
			XMLGET_COLOR_VALUE(pChildItem, _AL("TextUpperColor"), m_clUpper)
			XMLGET_COLOR_VALUE(pChildItem, _AL("TextLowerColor"), m_clLower)
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Hint")) == 0)
		{
			ACString strHint;
			XMLGET_WSTRING_VALUE(pChildItem, _AL("String"), strHint);
			AUICTranslate trans;
			m_szHint = trans.Translate(strHint);
		}
		pChildItem = pChildItem->GetNextItem();
	}

	if (!SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, 
			m_bFreeType, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower))
		return AUI_ReportError(__LINE__, 1, "AUITextArea::InitXML(), failed to call SetFontAndColor()");
	
	if( m_pParent )
		SetEmotionList(m_pParent->GetAUIManager()->GetDefaultSpriteEmotion(), m_pParent->GetAUIManager()->GetDefaultEmotion());

	AUICTranslate translate;
	SetText(translate.Translate(m_szText));
	return true;
}

bool AUITextArea::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUITEXTAREA_RECT_MIN && idType < AUITEXTAREA_RECT_MAX);

	UpdateRenderTarget();
	if( AUIDIALOG_FRAME_FRAME == idType )
	{
		A3DRELEASE(m_pAUIFrame);
		if( strlen(pszFileName) <= 0 ) return true;

		m_pAUIFrame = new AUIFrame;
		if( !m_pAUIFrame ) return false;

		bval = m_pAUIFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
		if( !bval )
		{
			A3DRELEASE(m_pAUIFrame)
			return AUI_ReportError(__LINE__, 1, "AUITextArea::InitIndirect(), failed to call m_pAUIFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else
	{
		A3DRELEASE(m_pA2DSprite[idType]);
		if( strlen(pszFileName) <= 0 ) return true;
	
		m_pA2DSprite[idType] = new A2DSprite;
		if( !m_pA2DSprite[idType] ) return false;
		
#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSprite[idType]->SetDynamicTex(true);
#endif //_ANGELICA22

		bval = m_pA2DSprite[idType]->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSprite[idType])
			return AUI_ReportError(__LINE__, 1, "AUITextArea::InitIndirect(), failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUITEXTAREA_RECT_UP == idType || AUITEXTAREA_RECT_DOWN == idType )
		{
			A3DRECT rc[2];
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight();
			rc[0].SetRect(0, 0, W, H / 2);
			rc[1].SetRect(0, H / 2, W, H);
			bval = m_pA2DSprite[idType]->ResetItems(2, rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::InitIndirect(), failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
	}

	return true;
}

bool AUITextArea::Release(void)
{
	A3DRELEASE(m_pAUIFrame);
	for( int i = 0; i < AUITEXTAREA_RECT_NUM; i++ )
		A3DRELEASE(m_pA2DSprite[i]);
	
	if (m_pTagCacheMan)
		delete m_pTagCacheMan;
	m_pTagCacheMan = NULL;

	return AUIObject::Release();
}

bool AUITextArea::Save(FILE *file)
{
	fprintf(file, "TEXT");

	if( !AUIObject::Save(file) ) return false;

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_szFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idString = m_pParent->AddStringToTable(szFontName);
	if( idString < 0 ) return false;

	fprintf(file, " %d %d %d %d %d %d", idString, m_nFontHeight,
		A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color),
		A3DCOLOR_GETBLUE(m_color), A3DCOLOR_GETALPHA(m_color));

	for( int i = 0; i < AUITEXTAREA_RECT_NUM; i++ )
	{
		if( AUITEXTAREA_RECT_FRAME == i )
		{
			if( m_pAUIFrame )
				m_pAUIFrame->Save(file);
			else
				fprintf(file, " \"\"");
		}
		else
			fprintf(file, " \"%s\"", m_pA2DSprite[i] ? m_pA2DSprite[i]->GetName() : "");
	}

	fprintf(file, " %d\n", m_nFrameMode);

	return true;
}

bool AUITextArea::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemTextArea = new AXMLItem;
	pXMLItemTextArea->InsertItem(pXMLItemDialog);
	pXMLItemTextArea->SetName(_AL("TEXT"));
	XMLSET_STRING_VALUE(pXMLItemTextArea, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemTextArea, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemTextArea, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemTextArea, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemTextArea, _AL("Height"), m_nHeight, 0)
	XMLSET_BOOL_VALUE(pXMLItemTextArea, _AL("LeftScrollBar"), m_bLeftScrollBar, false)
	XMLSET_BOOL_VALUE(pXMLItemTextArea, _AL("TextTransparent"), m_bTextTransparent, false)
	XMLSET_BOOL_VALUE(pXMLItemTextArea, _AL("TransScrollWheel"), m_bTransScrollWheel, true)
	XMLSET_INT_VALUE(pXMLItemTextArea, _AL("LineSpace"), m_nLineSpace, AUITEXTAREA_LINESPACE)
	XMLSET_INT_VALUE(pXMLItemTextArea, _AL("LinkUnderLineMode"), m_ItemUnderLineMode, UM_NO_UNDERLINE)
	XMLSET_COLOR_VALUE(pXMLItemTextArea, _AL("LinkUnderLineColor"), m_clItemUnderLine, _AL("0,0,0,255"))
	XMLSET_STRING_VALUE(pXMLItemTextArea, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemTextArea);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemTextArea);
	pXMLItemText->SetName(_AL("Text"));
	AUICTranslate translate;
	XMLSET_STRING_VALUE(pXMLItemText, _AL("String"), translate.ReverseTranslate(m_szText), _AL(""))
	XMLSET_STRING_VALUE(pXMLItemText, _AL("FontName"), m_szFontName, _AL(""))
	XMLSET_INT_VALUE(pXMLItemText, _AL("FontSize"), m_nFontHeight, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("ShadowColor"), m_nShadow, _AL("0,0,0,0"));
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Bold"), m_bBold, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Italic"), m_bItalic, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("FreeType"), m_bFreeType, true)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("Color"), m_color, _AL("255,255,255,255"));
	XMLSET_INT_VALUE(pXMLItemText, _AL("Outline"), m_nOutline, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("OutlineColor"), m_clOutline, _AL("0,0,0,255"))
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("InnerTextColor"), m_clInnerText, _AL("255,255,255,255"))
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("FadeColor"), m_bInterpColor, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("TextUpperColor"), m_clUpper, _AL("255,255,255,255"))
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("TextLowerColor"), m_clLower, _AL("255,255,255,255"))
	
	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemTextArea);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pAUIFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	if( m_pA2DSprite[AUITEXTAREA_RECT_UP] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("UpImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITEXTAREA_RECT_UP]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITEXTAREA_RECT_DOWN] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("DownImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("ScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITEXTAREA_RECT_BAR] )
	{
		AXMLItem *pXMLItemBarImage = new AXMLItem;
		pXMLItemBarImage->InsertItem(pXMLItemResource);
		pXMLItemBarImage->SetName(_AL("BarImage"));
		XMLSET_STRING_VALUE(pXMLItemBarImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITEXTAREA_RECT_BAR]->GetName()), _AL(""))
	}

	return true;
}

bool AUITextArea::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( !m_pAUIFrame ||
		!m_pA2DSprite[AUITEXTAREA_RECT_UP] ||
		!m_pA2DSprite[AUITEXTAREA_RECT_DOWN] ||
		!m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
	{
		return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
	}

	int nLines = m_nLines;
	int nLinesPP = m_nLinesPP;
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	A3DRECT rcUp = m_pA2DSprite[AUITEXTAREA_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetItem(0)->GetRect();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	A3DRECT rcText(cx, cy, m_nWidth	- cx - rcScroll.Width(), m_nHeight - cy);
	A3DRECT rcScrollArea(m_nWidth - rcScroll.Width(), rcUp.Height(), m_nWidth, m_nHeight - rcDown.Height());

	if( m_bLeftScrollBar )
	{
		rcText.Offset(rcScroll.Width(), 0);
		rcScrollArea.Offset(rcScroll.Width() - m_nWidth, 0);
	}

	if( msg == WM_LBUTTONDOWN ||
		msg == WM_RBUTTONDOWN ||
		msg == WM_LBUTTONDBLCLK ||
		msg == WM_MOUSEMOVE ||
		msg == WM_LBUTTONUP )
	{
		POINT ptPos = GetPos();
		int x = GET_X_LPARAM(lParam) - m_nOffX - ptPos.x;
		int y = GET_Y_LPARAM(lParam) - m_nOffY - ptPos.y;

		if( WM_LBUTTONUP == msg )
		{
			m_bDragging = false;
			m_pParent->SetCaptureObject(NULL);
			m_nState = AUITEXTAREA_STATE_NORMAL;
		}
		// Click on scroll bar.
		else if( m_bDragging || rcScrollArea.PtInRect(x, y) )
		{
			int nBarH = m_pA2DSprite[AUITEXTAREA_RECT_BAR]->GetItem(0)->GetRect().Height() * fWindowScale;
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUITEXTAREA_STATE_PRESSBAR == m_nState && nLines > nLinesPP )
					{
						int L = nLines - nLinesPP;
						int H = rcScrollArea.Height() - nBarH;

						if( H > 0 )
						{
							m_nFirstLine = (y - rcScrollArea.top - m_nDragDist) * (L + 1) / H;
							a_Clamp(m_nFirstLine, 0, nLines - nLinesPP);
						}
					}
				}
				else
				{
					m_bDragging = false;
					m_pParent->SetCaptureObject(NULL);
				}
			}
			else
			{
				// Page up.
				if( y < m_ptBar.y )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
						return OnDlgItemMessage(WM_KEYDOWN, VK_PRIOR, 0);
				}
				// Page down.
				else if( y > m_ptBar.y + nBarH )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
						return OnDlgItemMessage(WM_KEYDOWN, VK_NEXT, 0);
				}
				// Scroll bar.
				else if( nLines > nLinesPP )
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDragging = true;
						m_nDragDist = y - m_ptBar.y;
						m_pParent->SetCaptureObject(this);
						m_nState = AUITEXTAREA_STATE_PRESSBAR;
					}
				}
			}
		}
		else if( x >= rcScrollArea.left && x <= rcScrollArea.right )
		{
			// Click on up button.
			if( y < rcScrollArea.top )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUITEXTAREA_STATE_PRESSUP;
					m_nScrollDelay = 0;
				}
			}
			// Click on down button.
			else if( y > rcScrollArea.bottom )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUITEXTAREA_STATE_PRESSDOWN;
					m_nScrollDelay = 0;
				}
			}
		}

		return true;
	}
	else if( msg == WM_KEYDOWN )
	{
		int nVKey = int(wParam);
		if( VK_UP == nVKey )
		{
			SetFirstLine(m_nFirstLine-1);
			return true;
		}
		else if( VK_DOWN == nVKey )
		{
			SetFirstLine(m_nFirstLine+1);
			return true;
		}
		else if( VK_PRIOR == nVKey )
		{
			SetFirstLine(m_nFirstLine - nLinesPP);
			return true;
		}
		else if( VK_NEXT == nVKey )
		{
			if( nLines - m_nFirstLine >= 2 * nLinesPP )
				SetFirstLine(m_nFirstLine + nLinesPP);
			else if( nLines - m_nFirstLine >= nLinesPP )
				SetFirstLine(nLines - nLinesPP);
			return true;
		}
	}
	else if( WM_KILLFOCUS == msg )
	{
		m_bDragging = false;
		m_nState = AUITEXTAREA_STATE_NORMAL;
	}
	else if( WM_MOUSEWHEEL == msg )
	{
		int zDelta = (short)HIWORD(wParam);
		if( zDelta > 0 && m_nFirstLine > 0 )
			SetFirstLine(m_nFirstLine-1);
		else if( zDelta < 0 && m_nFirstLine + nLinesPP < nLines )
			SetFirstLine(m_nFirstLine+1);

		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUITextArea::Tick(void)
{
	if( !m_pAUIFrame || !m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
		return true;

	int nLines = m_nLines;
	int nLinesPP = m_nLinesPP;

	DWORD dwTick = GetTickCount();
	if( dwTick - m_nScrollDelay >= AUITEXTAREA_SCROLL_DELAY )
	{
		if( AUITEXTAREA_STATE_PRESSUP == m_nState )
		{
			if( m_nFirstLine > 0 )
				SetFirstLine(m_nFirstLine-1);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUITEXTAREA_STATE_PRESSDOWN == m_nState )
		{
			if( m_nFirstLine + nLinesPP < nLines )
				SetFirstLine(m_nFirstLine+1);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
	}

	return AUIObject::Tick();
}

bool AUITextArea::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

		bool bval = true;
		POINT ptPos = GetPos();

		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		A3DRECT rcScroll(0,0,0,0);
		if( m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
			rcScroll = m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetItem(0)->GetRect();
		A3DRECT rcText = GetTextRenderRect();

		if( m_pvecImageList )
		{
			bval = m_pA3DEngine->FlushInternalSpriteBuffer();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

			m_pParent->GetAUIManager()->GetA3DFTFontMan()->Flush();
		}

		// Only render when frames is OK.
		if( !m_bOnlyShowText )
		{
			if( m_pAUIFrame )
			{
				int x = ptPos.x;
				if( m_bLeftScrollBar )
					x = ptPos.x + rcScroll.Width();

				bval = m_pAUIFrame->Render(x, ptPos.y, m_nWidth - rcScroll.Width(), m_nHeight,
					NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
			}
			else
				bval = AUIObject::Render();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::Render(), failed to call m_pAUIFrame->Render() or base AUIObject::Render()");
		}

		// Render and update linesThisPage
		a_Clamp(m_nFirstLine, 0, m_nLines - m_nLinesPP);
		m_vecNameLink.clear();
		m_EditBoxItems.clear();
		m_vecChannelLink.clear();

		if (m_pTagCacheMan)
		{
			TEXTITEM_RCINFO trcinfo = {&m_vecNameLink, &m_EditBoxItems, &m_vecChannelLink};
			TAG_OP_OUT_PARAMS outParams(NULL, NULL, &m_nLinesPP, &trcinfo, NULL);
			AUITAG_CACHE_IN_PARAMS inParams(true, m_nFirstLine, rcText.left, rcText.top, int(m_pParent->GetWholeAlpha() * m_nAlpha / 255), &rcText);
			bval = m_pTagCacheMan->Output(inParams, outParams);
			if ( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::Render(), failed to call AUI_TextOutFormat()");
		}		

		if( m_bForceRenderScroll || m_nLines > m_nLinesPP )
			bval = RenderScroll();

		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::Render(), failed to call RenderScroll()");
	}

	return true;
}

bool AUITextArea::RenderScroll()
{
	if( !m_pA2DSprite[AUITEXTAREA_RECT_UP] ||
		!m_pA2DSprite[AUITEXTAREA_RECT_DOWN] ||
		!m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
	{
		return true;
	}

	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	bool bval = true;
	int nLines = m_nLines;
	POINT ptPos = GetPos();
	A2DSprite *pA2DSpriteFrame = NULL;
	int cx = 0, cy = 0;
	if( m_pAUIFrame )
	{
		SIZE sLimit = m_pAUIFrame->GetSizeLimit();
		cx = sLimit.cx / 2;
		cy = sLimit.cy / 2;
	}
	A3DRECT rcUp = m_pA2DSprite[AUITEXTAREA_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetItem(0)->GetRect();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	A3DRECT rcText(ptPos.x + cx, ptPos.y + cy, ptPos.x + m_nWidth
		- cx - rcScroll.Width(), ptPos.y + m_nHeight - cy);
	if( m_bLeftScrollBar ) rcText.Offset(rcScroll.Width(), 0);

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	A3DPOINT2 ptSize;
	int nTextH;
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	ptSize = pFont->GetTextExtent(_AL("Wjg"));
	nTextH = ptSize.y;

	if( m_pA2DSpriteEmotion && m_pA2DSpriteEmotion[0] )
		nTextH = max(m_pA2DSpriteEmotion[0]->GetItem(0)->GetRect().Height() * fWindowScale, nTextH);
	nTextH += m_nLineSpace * fWindowScale;

	if( m_bOnlyShowText ) return true;

	// Draw up button.
	int nItem = (AUITEXTAREA_STATE_PRESSUP == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUITEXTAREA_RECT_UP]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::RenderScroll(), failed to call m_pA2DSprite[AUITEXTAREA_RECT_UP]->SetCurrentItem()");

	int W = rcScroll.Width();
	int x, y = ptPos.y;
	if( m_bLeftScrollBar )
		x = ptPos.x;
	else
		x = ptPos.x + m_nWidth - W;
	m_pA2DSprite[AUITEXTAREA_RECT_UP]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITEXTAREA_RECT_UP]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUITEXTAREA_RECT_UP]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITEXTAREA_RECT_UP]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITEXTAREA_RECT_UP]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::RenderScroll(), failed to call m_pA2DSprite[AUITEXTAREA_RECT_UP]->DrawToBack() or m_pA2DSprite[AUITEXTAREA_RECT_UP]->DrawToInternalBuffer()");

	// Draw scroll area.
	int H = rcUp.Height();
	y += H;

	float fScale = float(m_nHeight - rcUp.Height() - rcDown.Height())
		/ m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetItem(0)->GetRect().Height();
	m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->SetScaleY(fScale);
	m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::RenderScroll(), failed to call m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->DrawToBack() or m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUITEXTAREA_STATE_PRESSDOWN == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::RenderScroll(), failed to call m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->SetCurrentItem()");

	y = ptPos.y + m_nHeight - rcDown.Height();

	m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::RenderScroll(), failed to call m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->DrawToInternalBuffer()");

	// Draw scrollbar button if necessary.
	if( nLines > m_nLinesPP )
	{
		int nBarH = m_pA2DSprite[AUITEXTAREA_RECT_BAR]->GetItem(0)->GetRect().Height() * fWindowScale;
		int H = m_nHeight - rcUp.Height() - rcDown.Height() - nBarH;
		if( H > 0 )
		{
			int L = nLines - m_nLinesPP;
			if( m_bLeftScrollBar )
				m_ptBar.x = 0;
			else
				m_ptBar.x = m_nWidth - rcScroll.Width();
			m_ptBar.y = rcUp.Height() + m_nFirstLine * H / L;
			m_pA2DSprite[AUITEXTAREA_RECT_BAR]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUITEXTAREA_RECT_BAR]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUITEXTAREA_RECT_BAR]->SetAlpha(nAlpha);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUITEXTAREA_RECT_BAR]->DrawToBack(
				ptPos.x + m_ptBar.x, ptPos.y + m_ptBar.y);
			else
				bval = m_pA2DSprite[AUITEXTAREA_RECT_BAR]->DrawToInternalBuffer(
				ptPos.x + m_ptBar.x, ptPos.y + m_ptBar.y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITextArea::RenderScroll(), failed to call m_pA2DSprite[AUITEXTAREA_RECT_BAR]->DrawToBack() or m_pA2DSprite[AUITEXTAREA_RECT_BAR]->DrawToInternalBuffer()");
		}
	}

	return true;
}

bool AUITextArea::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pAUIFrame )
			strncpy(Property->fn, m_pAUIFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
	{
		if( m_pA2DSprite[AUITEXTAREA_RECT_UP] )
			strncpy(Property->fn, m_pA2DSprite[AUITEXTAREA_RECT_UP]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
	{
		if( m_pA2DSprite[AUITEXTAREA_RECT_DOWN] )
			strncpy(Property->fn, m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
	{
		if( m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
	{
		if( m_pA2DSprite[AUITEXTAREA_RECT_BAR] )
			strncpy(Property->fn, m_pA2DSprite[AUITEXTAREA_RECT_BAR]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Left Scroll Bar") )
		Property->b = m_bLeftScrollBar;
	else if( 0 == strcmpi(pszPropertyName, "Text Transparent") )
		Property->b = m_bTextTransparent;
	else if( 0 == strcmpi(pszPropertyName, "Trans Scroll Wheel") )
		Property->b = m_bTransScrollWheel;
	else if( 0 == strcmpi(pszPropertyName, "Link Underline Mode") )
		Property->i = m_ItemUnderLineMode;
	else if( 0 == strcmpi(pszPropertyName, "Link Underline Color") )
		Property->dw = m_clItemUnderLine;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUITextArea::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Width") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrame )
		{
			SIZE s = m_pAUIFrame->GetSizeLimit();
			m_nWidth = max(m_nWidth, s.cx);
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "Height") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrame )
		{
			SIZE s = m_pAUIFrame->GetSizeLimit();
			m_nHeight = max(m_nHeight, s.cy);
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUITEXTAREA_RECT_FRAME);
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
		return InitIndirect(Property->fn, AUITEXTAREA_RECT_UP);
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
		return InitIndirect(Property->fn, AUITEXTAREA_RECT_DOWN);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
		return InitIndirect(Property->fn, AUITEXTAREA_RECT_SCROLL);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
		return InitIndirect(Property->fn, AUITEXTAREA_RECT_BAR);
	else if( 0 == strcmpi(pszPropertyName, "Text Transparent") )
		m_bTextTransparent = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Trans Scroll Wheel") )
		m_bTransScrollWheel = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Link Underline Mode") )
		m_ItemUnderLineMode = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Link Underline Color") )
		m_clItemUnderLine = Property->dw;
	else if( 0 == strcmpi(pszPropertyName, "Left Scroll Bar") )
		SetLeftScrollBar(Property->b);
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUITextArea::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Up Frame File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Down Frame File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frame Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Scroll Area Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Scroll Bar Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT);
}

void AUITextArea::ScrollToTop()
{
	if( m_nFirstLine != 0 )
		UpdateRenderTarget();

	SetFirstLine(0);
}

void AUITextArea::ScrollToBottom()
{
	if( !m_bScrollToBottom )
		UpdateRenderTarget();

	const abase::vector<AUITextTagCacheManager::cacheSection*>& aCacheSec = m_pTagCacheMan->GetCache();
	int nTextH = GetTextRenderRect().Height();
	int nLastLineY = -1;
	int nLines = 0;
	bool bLocate = false;
	for (int i = (int)aCacheSec.size()-1; i >= 0; i--)
	{
		const AUITextTagCacheManager::cacheSection* pSec = aCacheSec[i];
		if (!pSec)
			return;
		for (int j = (int)pSec->m_aCacheLines.size() - 1; j >=0 ; j--)
		{
			if (nLastLineY == -1)
			{
				nLastLineY = pSec->m_aCacheLines[j]->m_nOffsetY + pSec->m_aCacheLines[j]->m_nLineH;
			}
			
			if (nLastLineY - pSec->m_aCacheLines[j]->m_nOffsetY > nTextH)
			{
				bLocate = true;
				break;
			}

			nLines++;
		}

		if (bLocate)
			break;
	}
	
	m_nLinesPP = nLines;
	SetFirstLine(m_nLines - nLines);
}

void AUITextArea::SetOnlyShowText(bool bShow)
{
	if( bShow != m_bOnlyShowText )
		UpdateRenderTarget();

	m_bOnlyShowText = bShow;
}

void AUITextArea::SetForceRenderScroll(bool bForce)
{
	if( bForce != m_bForceRenderScroll )
		UpdateRenderTarget();

	m_bForceRenderScroll = bForce;
}

void AUITextArea::SetLeftScrollBar(bool bLeft)
{
	if( bLeft != m_bLeftScrollBar )
		UpdateRenderTarget();

	m_bLeftScrollBar = bLeft;
}

void AUITextArea::SetFirstLine(int nFirstLine)
{
	if( nFirstLine != m_nFirstLine )
		UpdateRenderTarget();

	m_nFirstLine = nFirstLine;
	a_ClampRoof(m_nFirstLine, m_nLines - m_nLinesPP);
	a_ClampFloor(m_nFirstLine, 0);

	// update linesPerPage
	if (m_pTagCacheMan)
	{
		A3DRECT rcText = GetTextRenderRect();
		TEXTITEM_RCINFO trcinfo = {&m_vecNameLink, &m_EditBoxItems, &m_vecChannelLink};
		TAG_OP_OUT_PARAMS outParams(NULL, NULL, &m_nLinesPP, &trcinfo, NULL);
		AUITAG_CACHE_IN_PARAMS inParams(false, m_nFirstLine, rcText.left, rcText.top, m_nAlpha * m_pParent->GetAlpha(), &rcText);
		
		m_vecNameLink.clear();
		m_EditBoxItems.clear();
		m_vecChannelLink.clear();
		bool bval = m_pTagCacheMan->Output(inParams, outParams);
		if ( !bval ) AUI_ReportError(__LINE__, 1, "AUITextArea::Render(), failed to call AUI_TextOutFormat()");
	}
}

abase::vector<AUITEXTAREA_NAME_LINK> & AUITextArea::GetNameLink()
{
	return m_vecNameLink;
}

void AUITextArea::SetEmotionList(A2DSprite **pA2DSpriteEmotion,
	abase::vector<AUITEXTAREA_EMOTION> **pvecEmotion)
{
	m_pA2DSpriteEmotion = pA2DSpriteEmotion;
	m_vecEmotion = pvecEmotion;
}

void AUITextArea::SetImageList(abase::vector<A2DSprite*> *pvecImageList)
{
	m_pvecImageList = pvecImageList;
}

void AUITextArea::SetOtherFonts(abase::vector<FONT_WITH_SHADOW>* pvecFonts)
{
	m_pvecOtherFonts = pvecFonts;
}

int AUITextArea::GetHitArea(int x, int y)
{
	if( !m_pAUIFrame ||
		!m_pA2DSprite[AUITEXTAREA_RECT_UP] ||
		!m_pA2DSprite[AUITEXTAREA_RECT_DOWN] ||
		!m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
	{
		return AUITEXTAREA_RECT_FRAME;
	}

	int nLines = m_nLines;
	int nLinesPP = m_nLinesPP;
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	A3DRECT rcUp = m_pA2DSprite[AUITEXTAREA_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUITEXTAREA_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetItem(0)->GetRect();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	A3DRECT rcText(cx, cy, m_nWidth	- cx - rcScroll.Width(), m_nHeight - cy);
	A3DRECT rcScrollArea(m_nWidth - rcScroll.Width(), rcUp.Height(), m_nWidth, m_nHeight - rcDown.Height());

	if( m_bLeftScrollBar )
	{
		rcText.Offset(rcScroll.Width(), 0);
		rcScrollArea.Offset(rcScroll.Width() - m_nWidth, 0);
	}

	// Click on text item area.
	if( rcText.PtInRect(x, y) )
		return AUITEXTAREA_RECT_FRAME;
	// Click on scroll bar.
	else if( rcScrollArea.PtInRect(x, y) )
	{
		int nBarH = m_pA2DSprite[AUITEXTAREA_RECT_BAR]->GetItem(0)->GetRect().Height();
		// Page up and page down.
		if( y < m_ptBar.y || y > m_ptBar.y + nBarH || nLines > nLinesPP )
			return AUITEXTAREA_RECT_SCROLL;
		// Scroll bar.
		else
			return AUITEXTAREA_RECT_BAR;
	}
	// Click on up button.
	else if( y < rcScrollArea.top )
		return AUITEXTAREA_RECT_UP;
	// Click on down button.
	else if( y > rcScrollArea.bottom )
		return AUITEXTAREA_RECT_DOWN;

	return AUITEXTAREA_RECT_FRAME;
}

void AUITextArea::SetText(const ACHAR *pszText)
{
	m_ItemsSet.Release();

	ACString strText = UnmarshalEditBoxText(pszText, m_ItemsSet, 0, L"", 0xffffffff, m_nItemMask);
	if( m_ItemsSet.GetItemCountByType(enumEIEmotion) == 0)
	{
		AUIObject::SetText(strText);
		SetDynamicRender(false);
	}
	else
	{
		SetDynamicRender(true);
		m_szText = strText;
	}
	
	m_nFirstLine = 0;
	BuildRenderCache();
}

void AUITextArea::UpdateAppend()
{
}

void AUITextArea::AppendText(const ACHAR *pszText, int nMsgIndex, const ACHAR* szIvtrItem, A3DCOLOR clIvtrItem, 
							 const EditboxScriptItem** ppScriptItems, int nScriptItemCount,
							 UnderLineMode ulMode, A3DCOLOR clUnderLine)
{
	UpdateRenderTarget();

	bool bUnderLine = false;
	bool bSameColor = true;
	if( ulMode == UM_USE_AUIOBJECT_OPTION )
	{
		if( m_ItemUnderLineMode == UM_NO_UNDERLINE )
		{
			bUnderLine = false;
			bSameColor = true;
		}
		else if( m_ItemUnderLineMode == UM_UNDERLINE_SAME_COLOR )
		{
			bUnderLine = true;
			bSameColor = true;
		}
		else
		{
			bUnderLine = true;
			bSameColor = false;
			clUnderLine = m_clItemUnderLine;
		}
	}
	else if( ulMode == UM_NO_UNDERLINE )
	{
		bUnderLine = false;
		bSameColor = true;
	}
	else if( ulMode == UM_UNDERLINE_SAME_COLOR )
	{
		bUnderLine = true;
		bSameColor = true;
	}
	else if( ulMode == UM_UNDERLINE_SPECIFY_COLOR )
	{
		bUnderLine = true;
		bSameColor = false;
	}

	int index = m_szText.GetLength();
	m_szText += UnmarshalEditBoxText(pszText, m_ItemsSet, nMsgIndex, szIvtrItem, clIvtrItem, m_nItemMask, ppScriptItems, nScriptItemCount, bUnderLine, bSameColor, clUnderLine);

	if (index == 0)
		BuildRenderCache();
	else
		AppendRenderCache(&m_szText[index]);
}

bool AUITextArea::IsInHitArea(int x, int y, UINT msg)
{
	if( !AUIObject::IsInHitArea(x, y, msg) )
		return false;

	if( !m_bTextTransparent || !m_bTransScrollWheel && !m_bOnlyShowText && msg == WM_MOUSEWHEEL && m_nLines > m_nLinesPP )
		return true;

	if( !m_bOnlyShowText && m_nLines > m_nLinesPP )
	{
		int nHitArea = GetHitArea(x, y);
		if( nHitArea == AUITEXTAREA_RECT_SCROLL ||
			nHitArea == AUITEXTAREA_RECT_UP ||
			nHitArea == AUITEXTAREA_RECT_DOWN )
			return true;
	}

	for(int i = 0; i < (int)m_EditBoxItems.size(); i++ )
	{
		if( (m_EditBoxItems[i].m_pItem->GetType() == enumEIIvtrlItem ||
			m_EditBoxItems[i].m_pItem->GetType() == enumEICoord ||
			m_EditBoxItems[i].m_pItem->GetType() == enumEIImage ||
			m_EditBoxItems[i].m_pItem->GetType() == enumEITask) &&
			m_EditBoxItems[i].rc.PtInRect(x, y) )
			return true;
	}

	return false;
}

int AUITextArea::GetRequiredHeight()
{
	int height = 0;
	
	const abase::vector<AUITextTagCacheManager::cacheSection*>& aCache = m_pTagCacheMan->GetCache();
	if (!aCache.empty())
	{
		if (!aCache.back()->m_aCacheLines.empty())
			height = aCache.back()->m_aCacheLines.back()->m_nOffsetY + aCache.back()->m_aCacheLines.back()->m_nLineH;
	}

	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	return height + sLimit.cy + 1;
}

bool AUITextArea::SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
	A3DCOLOR dwColor, int nShadow, int nOutline, bool bBold, bool bItalic, bool bFreeType,
	A3DCOLOR clOutline, A3DCOLOR clInnerText, bool bInterpColor, A3DCOLOR clUpper, A3DCOLOR clLower)
{
	bool bval=AUIObject::SetFontAndColor(pszFontName, nFontHeight, dwColor, nShadow, nOutline, 
							bBold, bItalic, bFreeType, clOutline, clInnerText, bInterpColor, clUpper, clLower);
	
	m_nFirstLine = 0;
	BuildRenderCache();

	return bval;
}

bool AUITextArea::Resize()
{
	A3DFTFont *pOldFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();

	bool b = AUIObject::Resize();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(fWindowScale);

	A3DFTFont *pNewFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	if (pOldFont != pNewFont)
	{
		// rebuild m_ItemSet
		BuildRenderCache();
	}
	return b;
}

void AUITextArea::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUITextArea::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	for (int i = 0; i < AUITEXTAREA_RECT_NUM; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);
}

void AUITextArea::ReserveBuffer(const unsigned int iSize)
{
	m_szText.GetBuffer(iSize);
}

void AUITextArea::SetMaxLines(int nLines)
{
	m_nMaxLines = nLines;
	m_nLines = m_pTagCacheMan->SetMaxSection(nLines, m_ItemsSet);
}

void AUITextArea::BuildRenderCache()
{
	if (!GetParent() || !GetParent()->GetAUIManager())
		return;
	
	m_nLines = 0;
	m_nLinesPP = 0;

	A3DRECT rcText = GetTextRenderRect();
	rcText.Offset(-rcText.left, -rcText.top);
	float fWindowScale = m_pParent->GetScale() * m_pParent->GetAUIManager()->GetWindowScale();
	ACString str;
	A3DCOLOR r = A3DCOLOR_GETRED(m_color);
	A3DCOLOR g = A3DCOLOR_GETGREEN(m_color);
	A3DCOLOR b = A3DCOLOR_GETBLUE(m_color);
	str.Format(_AL("^%02X%02X%02X%s"), r, g, b, m_szText);

	TAG_OP_IN_PARAMS InParams(m_pFont ? m_pFont : GetParent()->GetAUIManager()->GetDefaultFont(),
		0, 0, str, &rcText, m_nFirstLine, m_nLineSpace * fWindowScale, false, NULL,
		NULL, false, m_nShadow, int(m_pParent->GetWholeAlpha() * m_nAlpha / 255), false, &m_ItemsSet, 
		m_pvecImageList, fWindowScale, true, m_pvecOtherFonts,
		GetParent()->GetAUIManager()->GetWordWidth(), m_color);
	InParams.SetMultiEmotionSet(m_pA2DSpriteEmotion, m_vecEmotion);
	
	if (!m_pTagCacheMan)
		m_pTagCacheMan = new AUITextTagCacheManager;
	m_pTagCacheMan->ClearCache();
	m_vecNameLink.clear();
	m_EditBoxItems.clear();
	m_vecChannelLink.clear();
	
	TEXTITEM_RCINFO trcinfo = {&m_vecNameLink, &m_EditBoxItems, &m_vecChannelLink};
	TAG_OP_OUT_PARAMS outParams(&m_nLines, NULL, &m_nLinesPP, &trcinfo, m_pTagCacheMan);

	AUITagRegister<AUITextTagQuoteTA>();
	m_pTagCacheMan->AddCache(InParams, outParams);

	A3DRECT rcNewText = GetTextRenderRect();
	rcNewText.Offset(-rcNewText.left, -rcNewText.top);
	if (rcNewText != rcText) // text rect may change because of scroll bar
	{
		m_pTagCacheMan->ClearCache();
		InParams._pRect = &rcNewText;
		InParams._pszText = str;
		m_pTagCacheMan->AddCache(InParams, outParams);
	}

	AUITagUnregister<AUITextTagQuoteTA>();
	m_nLines = m_pTagCacheMan->SetMaxSection(m_nMaxLines, m_ItemsSet);
}

void AUITextArea::AppendRenderCache(const ACHAR* pszText)
{
	A3DRECT rcText = GetTextRenderRect();
	rcText.Offset(-rcText.left, -rcText.top);
	float fWindowScale = m_pParent->GetScale() * m_pParent->GetAUIManager()->GetWindowScale();
	TAG_OP_IN_PARAMS InParams(m_pFont ? m_pFont : GetParent()->GetAUIManager()->GetDefaultFont(),
		0, 0, pszText, &rcText, 0, m_nLineSpace * fWindowScale, false, NULL,
		NULL, false, m_nShadow, int(m_pParent->GetWholeAlpha() * m_nAlpha / 255), false, &m_ItemsSet, 
		m_pvecImageList, fWindowScale, true, m_pvecOtherFonts,
		GetParent()->GetAUIManager()->GetWordWidth(), m_color);
	InParams.SetMultiEmotionSet(m_pA2DSpriteEmotion, m_vecEmotion);
	
	TAG_OP_OUT_PARAMS outParams(&m_nLines, NULL, &m_nLinesPP, NULL, m_pTagCacheMan);
	
	AUITagRegister<AUITextTagQuoteTA>();
	if (!m_pTagCacheMan)
		m_pTagCacheMan = new AUITextTagCacheManager;
	m_pTagCacheMan->AddCache(InParams, outParams);
	AUITagUnregister<AUITextTagQuoteTA>();

	int nOldLines = m_nLines;

	A3DRECT rcNewText = GetTextRenderRect();
	rcNewText.Offset(-rcNewText.left, -rcNewText.top);
	if (rcNewText != rcText)
	{
		// scroll bar appears/disappears, rebuild render cache
		BuildRenderCache();
	}
	else
	{
		// may exceed limit
		m_nLines = m_pTagCacheMan->SetMaxSection(m_nMaxLines, m_ItemsSet);
	}
	
	m_nFirstLine -= (nOldLines - m_nLines);
	a_ClampFloor(m_nFirstLine, 0);
}


A3DRECT AUITextArea::GetTextRenderRect() const
{
	A3DRECT rc;
	rc.Clear();

	POINT ptPos = GetPos();
	int cx = 0, cy = 0;
	if( m_pAUIFrame )
	{
		SIZE sLimit = m_pAUIFrame->GetSizeLimit();
		cx = sLimit.cx / 2;
		cy = sLimit.cy / 2;
	}
	
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;
	
	A3DRECT rcScroll;
	if( (m_bForceRenderScroll || m_nLines > m_nLinesPP) && m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
	{
		rcScroll = m_pA2DSprite[AUITEXTAREA_RECT_SCROLL]->GetItem(0)->GetRect();
		rc.SetRect(ptPos.x + cx, ptPos.y + cy, ptPos.x + m_nWidth
			- cx - rcScroll.Width(), ptPos.y + m_nHeight - cy);
		if( m_bLeftScrollBar ) rc.Offset(rcScroll.Width(), 0);
	}
	else
		rc.SetRect(ptPos.x + cx, ptPos.y + cy, ptPos.x + m_nWidth - cx, ptPos.y + m_nHeight - cy);

	return rc;
}