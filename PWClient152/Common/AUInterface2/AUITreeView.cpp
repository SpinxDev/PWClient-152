// Filename	: AUITreeView.cpp
// Creator	: Tom Zhou
// Date		: December 1, 2004
// Desc		: AUITreeView is the class of tree view control.

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
#include "AUIDialog.h"
#include "AUITreeView.h"
#include "AUIManager.h"
#include "AUICommon.h"
#include "AUICTranslate.h"

#define AUITREEVIEW_INDENT_FREE			8
#define AUITREEVIEW_HEIGHT_FREE			3
#define AUITREEVIEW_SCROLL_DELAY		50

AUITreeView::AUITreeView() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_TREEVIEW;
	m_nState = AUITREEVIEW_STATE_NORMAL;
	m_nCount = 0;
	m_nFirstItem = 0;
	m_nScrollLines = 0;
	m_nRealLines = 0;
	m_nHBarPos = 0;
	m_nHBarMax = 0;
	m_bRenderVScroll = false;
	m_bRenderHScroll = false;
	m_bDraggingVScroll = false;
	m_bDraggingHScroll = false;
	m_nScrollDelay = GetTickCount();
	m_ptVBar.x = m_ptVBar.y = m_nDragDist = 0;
	m_ptHBar.x = m_ptHBar.y = 0;
	m_nItemIndent = m_nItemHeight = 0;
	m_pSelItem = NULL;
	m_RootItem.dwStatus = AUITREEVIEW_IS_EXPANDED;
	m_bTextTransparent = false;
	m_bTransScrollWheel = true;
	m_bOnlyShowText = false;
	m_pvecImageList = NULL;
	m_nItemMask = 0;

	m_nFrameMode = 0;
	m_pAUIFrame = NULL;
	for( int i = 0; i < AUITREEVIEW_RECT_NUM; i++ )
		m_pA2DSprite[i] = NULL;
	
	m_nTextIndent = AUITREEVIEW_INDENT_FREE;
	m_nLineSpace = AUITREEVIEW_HEIGHT_FREE;
	m_bAutoWrap = false;
	m_bLongHilightBar = false;
}

AUITreeView::~AUITreeView()
{
}

bool AUITreeView::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	TreeView property line format:
	 *
	 *		TREE					Keyword
	 *		Server					Name
	 *		1						String ID of command.
	 *		10 10 200 25			x y Width Height
	 *		1						String ID of font name
	 *		20						Font height
	 *		0 0 0 255				Text color, R, G, B, A
	 *		"TreeFrame.bmp"			ImageFileName
	 *		"TreeHilight.bmp"		ImageFileName
	 *		"TreeUp.bmp"			ImageFileName
	 *		"TreeDown.bmp"			ImageFileName
	 *		"TreeScroll.bmp"		ImageFileName
	 *		"TreeBar.bmp"			ImageFileName
	 *		"TreePlus.bmp"			ImageFileName
	 *		"TreeMinus.bmp"			ImageFileName
	 *		"TreeLeaf.bmp"			ImageFileName
	 */
	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUITreeView::Init(), failed to call base AUIObject::Init()");

	if( !pASF )
	{
		A3DPOINT2 ptSize;
		m_pFont = m_pParent->GetAUIManager()->GetDefaultFont();
		ptSize = m_pFont->GetTextExtent(_AL("W"));
		m_nItemIndent = 0;
		m_nItemHeight = ptSize.y;

		return true;
	}

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
		return AUI_ReportError(__LINE__, 1, "AUITreeView::Init(), failed to call SetFontAndColor()");

	A3DPOINT2 ptSize;
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	ptSize = pFont->GetTextExtent(_AL("W"));
	m_nItemIndent = 0;
	m_nItemHeight = ptSize.y;

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

	for( int i = AUITREEVIEW_RECT_MIN; i < AUITREEVIEW_RECT_MAX; i++ )
	{
		if (!pASF->GetNextToken(true))
			return AUI_ReportError(__LINE__, 1, "AUITreeView::Init(), failed to read from file");

		if (!InitIndirect(pASF->m_szToken, i))
			return AUI_ReportError(__LINE__, 1, "AUITreeView::Init(), failed to call InitIndirect()");
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

	AUI_ReportError(DEFAULT_2_PARAM, "AUITreeView::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUITreeView::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
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
	XMLGET_INT_VALUE(pItem, _AL("LineSpace"), m_nLineSpace)
	XMLGET_INT_VALUE(pItem, _AL("Indent"), m_nTextIndent)
	XMLGET_BOOL_VALUE(pItem, _AL("AutoWrap"), m_bAutoWrap)
	XMLGET_BOOL_VALUE(pItem, _AL("LongHilightBar"), m_bLongHilightBar)
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
					InitIndirect(strFileName, AUITREEVIEW_RECT_FRAME);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("UpImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_UP);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("DownImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_DOWN);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("HilightImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_HILIGHT);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_VSCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("BarImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_VBAR);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("PlusSymbolImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_PLUS);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("MinusSymbolImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_MINUS);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("LeafSymbolImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_LEAF);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("LeafCheckSymbolImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUITREEVIEW_RECT_LEAFCHECK);
				}
				
				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Text")) == 0 )
		{
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
		return AUI_ReportError(__LINE__, 1, "AUITreeView::InitXML(), failed to call SetFontAndColor()");
	
	A3DPOINT2 ptSize;
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	ptSize = pFont->GetTextExtent(_AL("W"));
	m_nItemHeight = max(m_nItemHeight, ptSize.y);
	
	return true;
}

bool AUITreeView::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUITREEVIEW_RECT_MIN && idType < AUITREEVIEW_RECT_MAX);

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
			return AUI_ReportError(__LINE__, 1, "AUITreeView::InitIndirect(), failed to call m_pAUIFrame->Init()");
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
			return AUI_ReportError(__LINE__, 1, "AUITreeView::InitIndirect(), failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUITREEVIEW_RECT_UP == idType || AUITREEVIEW_RECT_DOWN == idType )
		{
			A3DRECT rc[2];
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight();
			rc[0].SetRect(0, 0, W, H / 2);
			rc[1].SetRect(0, H / 2, W, H);
			bval = m_pA2DSprite[idType]->ResetItems(2, rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::InitIndirect(), failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
		else if( AUITREEVIEW_RECT_PLUS == idType
			|| AUITREEVIEW_RECT_MINUS == idType
			|| AUITREEVIEW_RECT_LEAF == idType 
			|| AUITREEVIEW_RECT_LEAFCHECK == idType)
		{
			m_nItemIndent = max(m_nItemIndent, m_pA2DSprite[idType]->GetWidth());
			m_nItemHeight = max(m_nItemHeight, m_pA2DSprite[idType]->GetHeight());
		}
	}

	return true;
}

bool AUITreeView::Release(void)
{
	DeleteAllItems();
	A3DRELEASE(m_pAUIFrame);
	for( int i = 0; i < AUITREEVIEW_RECT_NUM; i++ )
		A3DRELEASE(m_pA2DSprite[i]);

	return AUIObject::Release();
}

bool AUITreeView::Save(FILE *file)
{
	fprintf(file, "TREE");

	if( !AUIObject::Save(file) ) return false;

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_szFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idString = m_pParent->AddStringToTable(szFontName);
	if( idString < 0 ) return false;

	fprintf(file, " %d %d %d %d %d %d", idString, m_nFontHeight,
		A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color),
		A3DCOLOR_GETBLUE(m_color), A3DCOLOR_GETALPHA(m_color));

	for( int i = 0; i < AUITREEVIEW_RECT_NUM; i++ )
	{
		if( AUITREEVIEW_RECT_FRAME == i )
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

bool AUITreeView::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemTreeView = new AXMLItem;
	pXMLItemTreeView->InsertItem(pXMLItemDialog);
	pXMLItemTreeView->SetName(_AL("TREE"));
	XMLSET_STRING_VALUE(pXMLItemTreeView, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemTreeView, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemTreeView, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemTreeView, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemTreeView, _AL("Height"), m_nHeight, 0)
	XMLSET_BOOL_VALUE(pXMLItemTreeView, _AL("TextTransparent"), m_bTextTransparent, false)
	XMLSET_BOOL_VALUE(pXMLItemTreeView, _AL("TransScrollWheel"), m_bTransScrollWheel, true)
	XMLSET_INT_VALUE(pXMLItemTreeView, _AL("LineSpace"), m_nLineSpace, AUITREEVIEW_HEIGHT_FREE)
	XMLSET_INT_VALUE(pXMLItemTreeView, _AL("Indent"), m_nTextIndent, AUITREEVIEW_INDENT_FREE)
	XMLSET_BOOL_VALUE(pXMLItemTreeView, _AL("AutoWrap"), m_bAutoWrap, false)
	XMLSET_BOOL_VALUE(pXMLItemTreeView, _AL("LongHilightBar"), m_bLongHilightBar, false)
	XMLSET_STRING_VALUE(pXMLItemTreeView, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemTreeView);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemTreeView);
	pXMLItemText->SetName(_AL("Text"));
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
	pXMLItemResource->InsertItem(pXMLItemTreeView);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pAUIFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_UP] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("UpImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_UP]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_DOWN] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("DownImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_HILIGHT] )
	{
		AXMLItem *pXMLItemHilightImage = new AXMLItem;
		pXMLItemHilightImage->InsertItem(pXMLItemResource);
		pXMLItemHilightImage->SetName(_AL("HilightImage"));
		XMLSET_STRING_VALUE(pXMLItemHilightImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_HILIGHT]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("ScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_VBAR] )
	{
		AXMLItem *pXMLItemBarImage = new AXMLItem;
		pXMLItemBarImage->InsertItem(pXMLItemResource);
		pXMLItemBarImage->SetName(_AL("BarImage"));
		XMLSET_STRING_VALUE(pXMLItemBarImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_PLUS] )
	{
		AXMLItem *pXMLItemPlusImage = new AXMLItem;
		pXMLItemPlusImage->InsertItem(pXMLItemResource);
		pXMLItemPlusImage->SetName(_AL("PlusSymbolImage"));
		XMLSET_STRING_VALUE(pXMLItemPlusImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_PLUS]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_MINUS] )
	{
		AXMLItem *pXMLItemMinusImage = new AXMLItem;
		pXMLItemMinusImage->InsertItem(pXMLItemResource);
		pXMLItemMinusImage->SetName(_AL("MinusSymbolImage"));
		XMLSET_STRING_VALUE(pXMLItemMinusImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_MINUS]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_LEAF] )
	{
		AXMLItem *pXMLItemLeafImage = new AXMLItem;
		pXMLItemLeafImage->InsertItem(pXMLItemResource);
		pXMLItemLeafImage->SetName(_AL("LeafSymbolImage"));
		XMLSET_STRING_VALUE(pXMLItemLeafImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_LEAF]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK] )
	{
		AXMLItem *pXMLItemLeafCheckImage = new AXMLItem;
		pXMLItemLeafCheckImage->InsertItem(pXMLItemResource);
		pXMLItemLeafCheckImage->SetName(_AL("LeafCheckSymbolImage"));
		XMLSET_STRING_VALUE(pXMLItemLeafCheckImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK]->GetName()), _AL(""))
	}

	return true;
}

bool AUITreeView::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( !m_pAUIFrame ||
		!m_pA2DSprite[AUITREEVIEW_RECT_UP] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_DOWN] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] )
	{
		return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
	}

	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	A3DRECT rcUp = m_pA2DSprite[AUITREEVIEW_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect();
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);

	int nItemIndent = max(1, (m_nItemIndent + m_nTextIndent) * fWindowScale);
	int nItemHeight = max(1, m_nItemHeight + m_nLineSpace * fWindowScale);
	int nLinesPP = (int)((m_rcText.Height() + m_nLineSpace * fWindowScale) / nItemHeight);

	A3DRECT rcVScrollArea(m_nWidth - rcScroll.Width(), rcUp.Height(), m_nWidth, m_nHeight - rcDown.Height());
	A3DRECT rcHScrollArea(rcUp.Height(), m_nHeight - rcScroll.Width(), m_rcText.Width(), m_nHeight);

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
			m_bDraggingVScroll = false;
			m_bDraggingHScroll = false;
			m_pParent->SetCaptureObject(NULL);
			m_nState = AUITREEVIEW_STATE_NORMAL;
		}
		// Click on VScroll bar.
		else if( m_bDraggingVScroll || m_bRenderVScroll && rcVScrollArea.PtInRect(x, y) )
		{
			int nBarH = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->GetItem(0)->GetRect().Height() * fWindowScale;
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUITREEVIEW_STATE_PRESSVBAR == m_nState && m_bRenderVScroll )
					{
						int L = m_nScrollLines;
						int H = rcVScrollArea.Height() - nBarH;
						
						if( H > 0 )
						{
							int nFirstItem = (y - rcVScrollArea.top - m_nDragDist) * (L + 1) / H;
							a_Clamp(nFirstItem, 0, m_nScrollLines);
							SetFirstItem(nFirstItem);
						}
					}
				}
				else
				{
					m_bDraggingVScroll = false;
					m_pParent->SetCaptureObject(NULL);
				}
			}
			else
			{
				// Page up.
				if( y < m_ptVBar.y )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUITREEVIEW_STATE_PRESSUP;
						m_nScrollDelay = 0;
					}
				}
				// Page down.
				else if( y > m_ptVBar.y + nBarH )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUITREEVIEW_STATE_PRESSDOWN;
						m_nScrollDelay = 0;
					}
				}
				// VScroll bar.
				else if( m_bRenderVScroll )
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDraggingVScroll = true;
						m_nDragDist = y - m_ptVBar.y;
						m_pParent->SetCaptureObject(this);
						m_nState = AUITREEVIEW_STATE_PRESSVBAR;
					}
				}
			}
		}
		// Click on HScroll bar.
		else if( m_bDraggingHScroll || m_bRenderHScroll && rcHScrollArea.PtInRect(x, y) )
		{
			int nBarH = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->GetItem(0)->GetRect().Height() * fWindowScale;
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUITREEVIEW_STATE_PRESSHBAR == m_nState && m_bRenderHScroll )
					{
						int L = m_nHBarMax;
						int H = rcHScrollArea.Width() - nBarH;
						
						if( H > 0 )
							SetHBarPos((x - rcHScrollArea.left - m_nDragDist) * (L + 1) / H);
					}
				}
				else
				{
					m_bDraggingHScroll = false;
					m_pParent->SetCaptureObject(NULL);
				}
			}
			else
			{
				// Page left.
				if( x < m_ptHBar.x )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUITREEVIEW_STATE_PRESSLEFT;
						m_nScrollDelay = 0;
					}
				}
				// Page right.
				else if( x > m_ptHBar.x + nBarH )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUITREEVIEW_STATE_PRESSRIGHT;
						m_nScrollDelay = 0;
					}
				}
				// Scroll bar.
				else if( m_bRenderHScroll )
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDraggingHScroll = true;
						m_nDragDist = x - m_ptHBar.x;
						m_pParent->SetCaptureObject(this);
						m_nState = AUITREEVIEW_STATE_PRESSHBAR;
					}
				}
			}
		}
		// Click on text item area.
		else if( m_rcText.PtInRect(x, y) )
		{
			if( msg == WM_LBUTTONDOWN ||
				msg == WM_RBUTTONDOWN ||
				msg == WM_LBUTTONDBLCLK )
			{
				bool bOnIcon;
				P_AUITREEVIEW_ITEM pItem = HitTest(x, y, &bOnIcon);
				if( pItem )
				{
					if( bOnIcon )
					{
						if( pItem->pFirstChild )
							Expand(pItem, AUITREEVIEW_EXPAND_TOGGLE);
						else
							pItem->dwStatus ^= AUITREEVIEW_IS_LEAFCHECK;
					}
					else
					{
						if( msg == WM_LBUTTONDBLCLK )
							Expand(pItem, AUITREEVIEW_EXPAND_TOGGLE);
					}
				}
				SelectItem(pItem);
			}
		}
		else if( m_bRenderVScroll && x >= rcVScrollArea.left && x <= rcVScrollArea.right )
		{
			// Click on up button.
			if( y < rcVScrollArea.top )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUITREEVIEW_STATE_PRESSUP;
			}
			// Click on down button.
			else if( y > rcVScrollArea.bottom )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUITREEVIEW_STATE_PRESSDOWN;
			}
		}
		else if( m_bRenderHScroll && y >= rcHScrollArea.top && y <= rcHScrollArea.bottom )
		{
			// Click on left button.
			if( x < rcHScrollArea.left )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUITREEVIEW_STATE_PRESSLEFT;
			}
			// Click on right button.
			else if( x > rcHScrollArea.right )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					m_nState = AUITREEVIEW_STATE_PRESSRIGHT;
			}
		}

		return true;
	}
	else if( WM_KILLFOCUS == msg )
	{
		m_bDraggingVScroll = false;
		m_bDraggingHScroll = false;
		m_nState = AUITREEVIEW_STATE_NORMAL;
	}
	else if( WM_MOUSEWHEEL == msg )
	{
		int zDelta = (short)HIWORD(wParam);
		if( zDelta > 0 && m_nFirstItem > 0 )
			SetFirstItem(m_nFirstItem - 1);
		else if( zDelta < 0 && m_nFirstItem < m_nScrollLines )
			SetFirstItem(m_nFirstItem + 1);

		return true;
	}
	else if( WM_KEYDOWN == msg && m_pSelItem )
	{
		int nVKey = int(wParam);

		if( VK_UP == nVKey )
		{
			for(int i = 1; i < m_vecVisibleItems.size(); i++)
				if( m_vecVisibleItems[i]->dwStatus & AUITREEVIEW_IS_SELECTED )
				{
					SelectItem(m_vecVisibleItems[i - 1]);
					EnsureVisible(i - 1);
					break;
				}
			
			return true;
		}
		else if( VK_DOWN == nVKey )
		{
			for(int i = 0; i < m_vecVisibleItems.size() - 1; i++)
				if( m_vecVisibleItems[i]->dwStatus & AUITREEVIEW_IS_SELECTED )
				{
					SelectItem(m_vecVisibleItems[i + 1]);
					EnsureVisible(i + 1);
					break;
				}

			return true;
		}
		else if( VK_LEFT == nVKey )
		{
			if( m_pSelItem->dwStatus & AUITREEVIEW_IS_EXPANDED )
				Expand(m_pSelItem, AUITREEVIEW_EXPAND_TOGGLE);
			else if( m_pSelItem->pParent != &m_RootItem )
			{
				SelectItem(m_pSelItem->pParent);
				for(int i = 0; i < m_vecVisibleItems.size(); i++)
					if( m_vecVisibleItems[i]->dwStatus & AUITREEVIEW_IS_SELECTED )
					{
						EnsureVisible(i);
						break;
					}
			}

			return true;
		}
		else if( VK_RIGHT == nVKey )
		{
			if( m_pSelItem->pFirstChild )
			{
				if( m_pSelItem->dwStatus & AUITREEVIEW_IS_EXPANDED )
				{
					SelectItem(m_pSelItem->pFirstChild);
					for(int i = 0; i < m_vecVisibleItems.size(); i++)
						if( m_vecVisibleItems[i]->dwStatus & AUITREEVIEW_IS_SELECTED )
						{
							EnsureVisible(i);
							break;
						}
				}
				else
					Expand(m_pSelItem, AUITREEVIEW_EXPAND_TOGGLE);
			}

			return true;
		}
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUITreeView::Tick(void)
{
	if( !m_pAUIFrame || !m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] )
		return true;

	int cx = 0, cy = 0;
	A2DSprite *pA2DSpriteFrame = NULL;
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
	{
		sLimit = m_pAUIFrame->GetSizeLimit();
		cx = sLimit.cx / 2;
		cy = sLimit.cy / 2;
	}
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nItemHeight = m_nItemHeight + m_nLineSpace * fWindowScale;
	int nLinesPP = int((m_rcText.Height() + m_nLineSpace * fWindowScale)/ nItemHeight);

	DWORD dwTick = GetTickCount();
	if( dwTick - m_nScrollDelay >= AUITREEVIEW_SCROLL_DELAY )
	{
		if( AUITREEVIEW_STATE_PRESSUP == m_nState )
		{
			if( m_nFirstItem > 0 )
				SetFirstItem(m_nFirstItem - 1);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUITREEVIEW_STATE_PRESSDOWN == m_nState )
		{
			if( m_nFirstItem < m_nScrollLines )
				SetFirstItem(m_nFirstItem + 1);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUITREEVIEW_STATE_PRESSLEFT == m_nState )
		{
			if( m_nHBarPos > 0 )
				SetHBarPos(m_nHBarPos - 1);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUITREEVIEW_STATE_PRESSRIGHT == m_nState )
		{
			if( m_nHBarPos < m_nHBarMax )
				SetHBarPos(m_nHBarPos + 1);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
	}

	return AUIObject::Tick();
}

bool AUITreeView::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

		int i;
		bool bval = true;
		POINT ptPos = GetPos();
		int cx = 0, cy = 0;
		SIZE sLimit = {0, 0};
		if( m_pAUIFrame )
		{
			sLimit = m_pAUIFrame->GetSizeLimit();
			cx = sLimit.cx / 2;
			cy = sLimit.cy / 2;
		}
		int nItemIndent = max(1, (m_nItemIndent + m_nTextIndent) * fWindowScale);
		int nItemHeight = max(1, m_nItemHeight + m_nLineSpace * fWindowScale);
		
		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		A3DRECT rcScroll(0, 0, 0, 0);
		if( m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] ) 
		{
			rcScroll = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect();
			rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
				rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
		}
		int nLinesPP = (int)((m_rcText.Height() + m_nLineSpace * fWindowScale) / nItemHeight);

		FitFont();
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();

		if( m_pvecImageList )
		{
			bval = m_pA3DEngine->FlushInternalSpriteBuffer();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

			m_pParent->GetAUIManager()->GetA3DFTFontMan()->Flush();
		}

		// Only render when frames is OK.
		if( m_pAUIFrame )
		{
			int nWidth = m_bRenderVScroll ? m_nWidth - rcScroll.Width() : m_nWidth;
			int nHeight = m_bRenderHScroll ? m_nHeight - rcScroll.Width() : m_nHeight;
			bval = m_pAUIFrame->Render(ptPos.x, ptPos.y, nWidth, nHeight, NULL, 0,
				_AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, m_nAlpha, m_pParent->IsNoFlush(), m_nShadow);
		}
		else
			bval = AUIObject::Render();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call m_pAUIFrame->Render() or base AUIObject::Render()");

		// Clear Rect info
		ClearItemRcInfo();
		TEXTITEM_RCINFO rcInfo = {&m_vNameLinks, &m_vEBItems};

		// Show tree view text items.
		int x, y;
		A3DRECT rc;
		A2DSprite *pA2DSprite;
		P_AUITREEVIEW_ITEM pItem;

		int nStart = m_nFirstItem;
		int nEnd = GetLastVisibleItem();

		A3DRECT rcText = m_rcText;
		rcText.Offset(ptPos.x, ptPos.y);
		y = rcText.top;
		for( i = nStart; i <= nEnd; i++ )
		{
			pItem = m_vecVisibleItems[i];
			x = rcText.left + pItem->nIndent * nItemIndent;

			pA2DSprite = NULL;
			if( pItem->pFirstChild )
			{
				if( pItem->dwStatus & AUITREEVIEW_IS_EXPANDED )
					pA2DSprite = m_pA2DSprite[AUITREEVIEW_RECT_MINUS];
				else
					pA2DSprite = m_pA2DSprite[AUITREEVIEW_RECT_PLUS];
			}
			else
			{
				if( m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK] && (pItem->dwStatus & AUITREEVIEW_IS_LEAFCHECK) )
					pA2DSprite = m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK];
				else
					pA2DSprite = m_pA2DSprite[AUITREEVIEW_RECT_LEAF];
			}

			if( pA2DSprite && x - m_nHBarPos * nItemIndent >= rcText.left )
			{
				pA2DSprite->SetScaleX(fWindowScale);
				pA2DSprite->SetScaleY(fWindowScale);
				int nFlagH = pA2DSprite->GetHeight() * fWindowScale;
				if( m_pParent->IsNoFlush() )
					bval = pA2DSprite->DrawToBack(x - m_nHBarPos * nItemIndent, y + (nItemHeight - nFlagH) / 2);
				else
					bval = pA2DSprite->DrawToInternalBuffer(x - m_nHBarPos * nItemIndent, y + (nItemHeight - nFlagH) / 2);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call pA2DSprite->DrawToBack() or pA2DSprite->DrawToInternalBuffer()");
			}

			if( pItem->dwStatus & AUITREEVIEW_IS_BLINKING )
			{
				int nTimeNow = GetTickCount();
				if( nTimeNow - m_nTime >= AUIOBJECT_BLINK_INTERVAL )
				{
					m_bBlink = !m_bBlink;
					m_nTime = nTimeNow;
				}
				if( !m_bBlink ) continue;
			}

			x += nItemIndent;
			
			A3DPOINT2 ptSize;
			int nTextW = 0, nTextH = 0;
			int nLines;
			if( m_pvecImageList && (*m_pvecImageList)[0] )
				AUI_GetTextRect(pFont, pItem->strText, ptSize.x, ptSize.y, nLines, m_nLineSpace * fWindowScale, &pItem->itemsSet, 0, 0, m_pvecImageList, fWindowScale);
			else
				AUI_GetTextRect(pFont, pItem->strText, ptSize.x, ptSize.y, nLines, m_nLineSpace * fWindowScale);
			nTextW = min(ptSize.x, rcText.right - x);

			pItem->rcItem.SetRect(rcText.left, y, x + nTextW, min(y + pItem->nItemHeight, rcText.bottom));
			A3DCOLOR r = A3DCOLOR_GETRED(pItem->color);
			A3DCOLOR g = A3DCOLOR_GETGREEN(pItem->color);
			A3DCOLOR b = A3DCOLOR_GETBLUE(pItem->color);
			ACHAR szColor[10];
			a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);

			A3DPOINT2 ptSizeSingle;
			ptSizeSingle = pFont->GetTextExtent(_AL("W"));
			bval = AUI_TextOutFormat(pFont, x - m_nHBarPos * nItemIndent, y+(nItemHeight-ptSizeSingle.y)/2, szColor + pItem->strText,
				&pItem->rcItem, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, NULL, NULL, NULL, true, m_nShadow, nAlpha, false, &pItem->itemsSet, m_pvecImageList, fWindowScale, m_bAutoWrap,
				NULL, 0, &rcInfo);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call AUI_TextOutFormat()");
			
			if( pItem->dwStatus & AUITREEVIEW_IS_SELECTED )
			{
				pA2DSprite = m_pA2DSprite[AUITREEVIEW_RECT_HILIGHT];
				if( pA2DSprite )
				{
					int x1 = x - m_nHBarPos * nItemIndent;
					int x2 = x1 + ptSize.x;
					if( x1 < rcText.left )
						x1 = rcText.left;

					if( x2 > rcText.right )
						x2 = rcText.right;

					if( m_bLongHilightBar )
						x2 = rcText.right;

					int curItemHeight = nItemHeight * pItem->nLines + 2;
					int iExceedH = y + curItemHeight - rcText.bottom;
					if (iExceedH > 0)
					{
						A3DRECT rcItem(0, 0, pA2DSprite->GetWidth(), pA2DSprite->GetHeight() * (curItemHeight-iExceedH) / curItemHeight);
						pA2DSprite->ResetItems(1, &rcItem);
						pA2DSprite->SetCurrentItem(0);
					}
					else
					{
						A3DRECT rcItem(0, 0, pA2DSprite->GetWidth(), pA2DSprite->GetHeight());
						pA2DSprite->ResetItems(1, &rcItem);
						pA2DSprite->SetCurrentItem(0);
					}
					float fScaleX = float(x2 - x1 + 2) / float(pA2DSprite->GetWidth());
					float fScaleY = float(curItemHeight) / float(pA2DSprite->GetHeight());

					pA2DSprite->SetScaleX(fScaleX);
					pA2DSprite->SetScaleY(fScaleY);

					if( m_pParent->IsNoFlush() )
						bval = pA2DSprite->DrawToBack(x1 - 1, y);
					else
						bval = pA2DSprite->DrawToInternalBuffer(x1 - 1, y);
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call pA2DSprite->DrawToBack() or pA2DSprite->DrawToInternalBuffer()");
				}
			}

			y += pItem->nLines * nItemHeight;
		}

		if( !m_bOnlyShowText )
		{
			if( m_bRenderVScroll )	
			{
				bval = RenderVScroll();
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call RenderVScroll()");
			}

			if( m_bRenderHScroll )	
			{
				bval = RenderHScroll();
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::Render(), failed to call RenderHScroll()");
			}
		}

		//bval = pFont->Flush();
		//if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	}

	return true;
}

bool AUITreeView::RenderVScroll()
{
	if( !m_pA2DSprite[AUITREEVIEW_RECT_UP] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_DOWN] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] )
	{
		return true;
	}

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

	bool bval = true;
	POINT ptPos = GetPos();
	A2DSprite *pA2DSpriteFrame = NULL;
	int cx = 0, cy = 0;
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
	{
		sLimit = m_pAUIFrame->GetSizeLimit();
		cx = sLimit.cx / 2;
		cy = sLimit.cy / 2;
	}
	int nItemIndent = max(1, (m_nItemIndent + m_nTextIndent) * fWindowScale);
	int nItemHeight = max(1, m_nItemHeight + m_nLineSpace * fWindowScale);
	A3DRECT rcUp = m_pA2DSprite[AUITREEVIEW_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect();
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	int nLinesPP = (int)((m_rcText.Height() + m_nLineSpace * fWindowScale) / nItemHeight);

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	// Draw up button.
	int nItem = (AUITREEVIEW_STATE_PRESSUP == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderVScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetCurrentItem()");

	int W = rcScroll.Width();
	int x = ptPos.x + m_nWidth - W;
	int y = ptPos.y;
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetAlpha(nAlpha);
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetScaleY(fWindowScale);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderVScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToInternalBuffer()");

	// Draw scroll area.
	int H = rcUp.Height();
	y += H;

	float fScale = float(m_nHeight - rcUp.Height() - rcDown.Height())
		/ m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect().Height();
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetScaleY(fScale);
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderVScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUITREEVIEW_STATE_PRESSDOWN == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderVScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetCurrentItem()");

	y = ptPos.y + m_nHeight - rcDown.Height();

	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderVScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToInternalBuffer()");

	// Draw scrollbar button if necessary.
	if( m_pA2DSprite[AUITREEVIEW_RECT_VBAR] && m_nRealLines > nLinesPP )
	{
		int nBarH = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->GetItem(0)->GetRect().Height() * fWindowScale;
		int H = m_nHeight - rcUp.Height() - rcDown.Height() - nBarH;
		if( H > 0 )
		{
			int L = m_nScrollLines;
			m_ptVBar.x = m_nWidth - rcScroll.Width();
			m_ptVBar.y = rcUp.Height() + m_nFirstItem * H / L;
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetAlpha(nAlpha);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToBack(ptPos.x + m_ptVBar.x, ptPos.y + m_ptVBar.y);
			else
				bval = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToInternalBuffer(ptPos.x + m_ptVBar.x, ptPos.y + m_ptVBar.y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderVScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToInternalBuffer()");
		}
	}

	return true;
}

bool AUITreeView::RenderHScroll()
{
	if( !m_pA2DSprite[AUITREEVIEW_RECT_UP] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_DOWN] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] )
	{
		return true;
	}

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

	bool bval = true;
	POINT ptPos = GetPos();
	A2DSprite *pA2DSpriteFrame = NULL;
	int cx = 0, cy = 0;
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
	{
		sLimit = m_pAUIFrame->GetSizeLimit();
		cx = sLimit.cx / 2;
		cy = sLimit.cy / 2;
	}
	A3DRECT rcUp = m_pA2DSprite[AUITREEVIEW_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect();
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	// Draw left button.
	int nItem = (AUITREEVIEW_STATE_PRESSLEFT == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderHScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetCurrentItem()");

	int H = rcScroll.Width();
	int x = ptPos.x;
	int y = ptPos.y + m_nHeight - 1;
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetAlpha(nAlpha);
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetDegree(270);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToInternalBuffer(x, y);
	m_pA2DSprite[AUITREEVIEW_RECT_UP]->SetDegree(0);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderHScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_UP]->DrawToInternalBuffer()");

	// Draw scroll area.
	int W = rcUp.Height();
	x += W;

	float fScale = float(m_nWidth - rcUp.Height() - rcDown.Height() - (m_bRenderVScroll ? rcScroll.Width() : 0))
		/ m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect().Height();
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetScaleY(fScale);
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetAlpha(nAlpha);
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetDegree(270);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToInternalBuffer(x, y);
	m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->SetDegree(0);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderHScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUITREEVIEW_STATE_PRESSRIGHT == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderHScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetCurrentItem()");

	x = ptPos.x + m_nWidth - rcDown.Height() - (m_bRenderVScroll ? rcScroll.Width() : 0);

	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetAlpha(nAlpha);
	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetDegree(270);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToInternalBuffer(x, y);
	m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->SetDegree(0);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderHScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->DrawToInternalBuffer()");

	// Draw scrollbar button if necessary.
	if( m_pA2DSprite[AUITREEVIEW_RECT_VBAR] )
	{
		int nBarH = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->GetItem(0)->GetRect().Height() * fWindowScale;
		int H = m_nWidth - rcUp.Height() - rcDown.Height() - nBarH - (m_bRenderVScroll ? rcScroll.Width() : 0);
		if( H > 0 )
		{
			int L = m_nHBarMax;
			m_ptHBar.x = rcUp.Height() + m_nHBarPos * H / L;
			m_ptHBar.y = m_nHeight;
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetAlpha(nAlpha);
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetDegree(270);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToBack(
					ptPos.x + m_ptHBar.x, ptPos.y - 1 + m_ptHBar.y);
			else
				bval = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToInternalBuffer(
					ptPos.x + m_ptHBar.x, ptPos.y - 1 + m_ptHBar.y);
			m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->SetDegree(0);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RenderHScroll(), failed to call m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToBack() or m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->DrawToInternalBuffer()");
		}
	}

	return true;
}

bool AUITreeView::GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Text Transparent") )
		Property->b = m_bTextTransparent;
	else if( 0 == strcmpi(pszPropertyName, "Trans Scroll Wheel") )
		Property->b = m_bTransScrollWheel;
	else if( 0 == strcmpi(pszPropertyName, "Text Indent") )
		Property->i = m_nTextIndent;
	else if( 0 == strcmpi(pszPropertyName, "Auto Wrap") )
		Property->b = m_bAutoWrap;
	else if( 0 == strcmpi(pszPropertyName, "Long Hilight Bar") )
		Property->b = m_bLongHilightBar;
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pAUIFrame )
			strncpy(Property->fn, m_pAUIFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Hilight Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_HILIGHT] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_HILIGHT]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_UP] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_UP]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_DOWN] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_VBAR] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Plus Symbol Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_PLUS] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_PLUS]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Minus Symbol Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_MINUS] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_MINUS]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Leaf Symbol Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_LEAF] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_LEAF]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Leaf Check Symbol Image") )
	{
		if( m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK] )
			strncpy(Property->fn, m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUITreeView::SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
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
	else if( 0 == strcmpi(pszPropertyName, "Font")
		|| 0 == strcmpi(pszPropertyName, "Outline") )
	{
		bool bval = true;

		if( 0 == strcmpi(pszPropertyName, "Font") )
			bval = SetFontAndColor(Property->font.szName, Property->font.nHeight, m_color);
		else
			bval = SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, Property->i);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::SetProperty(), failed to call SetFontAndColor()");

		A3DPOINT2 ptSize;
		ptSize = m_pFont->GetTextExtent(_AL("W"));

		m_nItemHeight = max(m_nItemHeight, ptSize.y);
	}
	else if( 0 == strcmpi(pszPropertyName, "Text Transparent") )
		m_bTextTransparent = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Trans Scroll Wheel") )
		m_bTransScrollWheel = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Text Indent") )
		m_nTextIndent = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Auto Wrap") )
		SetAutoWrap(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Long Hilight Bar") )
		m_bLongHilightBar = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_FRAME);
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_UP);
	else if( 0 == strcmpi(pszPropertyName, "Hilight Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_HILIGHT);
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_DOWN);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_VSCROLL);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_VBAR);
	else if( 0 == strcmpi(pszPropertyName, "Plus Symbol Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_PLUS);
	else if( 0 == strcmpi(pszPropertyName, "Minus Symbol Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_MINUS);
	else if( 0 == strcmpi(pszPropertyName, "Leaf Symbol Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_LEAF);
	else if( 0 == strcmpi(pszPropertyName, "Leaf Check Symbol Image") )
		return InitIndirect(Property->fn, AUITREEVIEW_RECT_LEAFCHECK);
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUITreeView::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Frame Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Up Button Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Hilight Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Down Button Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Scroll Area Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Scroll Bar Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Plus Symbol Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Minus Symbol Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Leaf Symbol Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT);
}

void AUITreeView::SetOnlyShowText(bool bShow)
{
	if( bShow != m_bOnlyShowText )
		UpdateRenderTarget();

	m_bOnlyShowText = bShow;
}

void AUITreeView::SetAutoWrap(bool bAutoWarp)
{
	if( bAutoWarp != m_bAutoWrap )
		UpdateRenderTarget();

	m_bAutoWrap = bAutoWarp;
	RebuildVisibleItems();
}

P_AUITREEVIEW_ITEM AUITreeView::GetRootItem()
{
	return &m_RootItem;
}

P_AUITREEVIEW_ITEM AUITreeView::GetParentItem(P_AUITREEVIEW_ITEM pItem)
{
	if(!pItem)
	{
		ASSERT(0);
		return NULL;
	}
	return pItem->pParent;
}

P_AUITREEVIEW_ITEM AUITreeView::GetFirstChildItem(P_AUITREEVIEW_ITEM pItem)
{
	if(!pItem)
	{
		ASSERT(0);
		return NULL;
	}
	return pItem->pFirstChild;
}

P_AUITREEVIEW_ITEM AUITreeView::GetPrevSiblingItem(P_AUITREEVIEW_ITEM pItem)
{
	if(!pItem)
	{
		ASSERT(0);
		return NULL;
	}
	return pItem->pLast;
}

P_AUITREEVIEW_ITEM AUITreeView::GetNextSiblingItem(P_AUITREEVIEW_ITEM pItem)
{
	if(!pItem)
	{
		ASSERT(0);
		return NULL;
	}
	return pItem->pNext;
}

P_AUITREEVIEW_ITEM AUITreeView::GetItemByIndexArray(int nDepth, int *pIndexArray)
{
	P_AUITREEVIEW_ITEM pItem = &m_RootItem;
	for(int i = 0; i < nDepth; i++)
	{
		pItem = GetFirstChildItem(pItem);
		if( !pItem )
			return NULL;

		for(int j = 0; j < pIndexArray[i]; j++)
		{
			pItem = GetNextSiblingItem(pItem);
			if( !pItem )
				return NULL;
		}
	}

	return pItem;
}

int AUITreeView::GetCount()
{
	return m_nCount;
}

bool AUITreeView::ItemHasChildren(P_AUITREEVIEW_ITEM pItem)
{
	return pItem->pFirstChild ? true : false;
}

const ACHAR * AUITreeView::GetItemText(P_AUITREEVIEW_ITEM pItem)
{
	return pItem->strText;
}

bool AUITreeView::SetItemText(P_AUITREEVIEW_ITEM pItem, const ACHAR *pszText)
{
	UpdateRenderTarget();

	pItem->itemsSet.Release();
	m_vEBItems.clear();
	pItem->strText = UnmarshalEditBoxText(pszText, pItem->itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	return true;
}

DWORD AUITreeView::GetItemData(P_AUITREEVIEW_ITEM pItem, AString strName)
{
	if ( !pItem )
	{
		return AUITREEVIEW_ERROR;
	}
	if (0 != pItem->dwData && strName != pItem->strDataName)
		AUI_ReportError(__LINE__, 1, "AUITreeView::GetItemData(), data name not match");
	return pItem->dwData;
}

void AUITreeView::SetItemData(P_AUITREEVIEW_ITEM pItem, DWORD dwData, AString strName)
{
	pItem->strDataName = strName;
	pItem->dwData = dwData;
}

void * AUITreeView::GetItemDataPtr(P_AUITREEVIEW_ITEM pItem, AString strName)
{
	if (0 != pItem->pvData && strName != pItem->strDataPtrName)
		AUI_ReportError(__LINE__, 1, "AUITreeView::GetItemDataPtr(), data name not match");
	return pItem->pvData;
}

void AUITreeView::SetItemDataPtr(P_AUITREEVIEW_ITEM pItem, void *pvData, AString strName)
{
	pItem->strDataPtrName = strName;
	pItem->pvData = pvData;
}

UINT64 AUITreeView::GetItemData64(P_AUITREEVIEW_ITEM pItem, AString strName)
{
	if (0 != pItem->uiData64 && strName != pItem->strData64Name)
		AUI_ReportError(__LINE__, 1, "AUITreeView::GetItemData64(), data name not match");
	return pItem->uiData64;
}

void AUITreeView::SetItemData64(P_AUITREEVIEW_ITEM pItem, UINT64 uiData64, AString strName)
{
	pItem->strData64Name = strName;
	pItem->uiData64 = uiData64;
}

UINT64 AUITreeView::GetItemDataPtr64(P_AUITREEVIEW_ITEM pItem, AString strName)
{
	if (0 != pItem->uiDataPtr64 && strName != pItem->strDataPtr64Name)
		AUI_ReportError(__LINE__, 1, "AUITreeView::GetItemDataPtr64(), data name not match");
	return pItem->uiDataPtr64;
}

void AUITreeView::SetItemDataPtr64(P_AUITREEVIEW_ITEM pItem, UINT64 uiDataPtr64, AString strName)
{
	pItem->strDataPtr64Name = strName;
	pItem->uiDataPtr64 = uiDataPtr64;
}

A3DCOLOR AUITreeView::GetItemTextColor(P_AUITREEVIEW_ITEM pItem)
{
	return pItem->color;
}

void AUITreeView::SetItemTextColor(P_AUITREEVIEW_ITEM pItem, A3DCOLOR color)
{
	UpdateRenderTarget();

	pItem->color = color;
}

bool AUITreeView::DeleteAllItems()
{
	UpdateRenderTarget();

	bool bval = true;
	P_AUITREEVIEW_ITEM pItem = m_RootItem.pFirstChild;
	P_AUITREEVIEW_ITEM pDel;

	while( pItem )
	{
		pDel = pItem;
		pItem = pItem->pNext;

		bval = DeleteItem(pDel, false);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::DeleteAllItems(), failed to call DeleteItem()");
	}

	m_nCount = 0;
	m_nFirstItem = 0;

	bval = RebuildVisibleItems();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::DeleteAllItems(), failed to call RebuildVisibleItems()");

	return true;
}

bool AUITreeView::DeleteItem(P_AUITREEVIEW_ITEM pItem, bool bRebuildItems)
{
	UpdateRenderTarget();

	bool bval = true;
	P_AUITREEVIEW_ITEM pDel, pChild;

	if( pItem == &m_RootItem ) return false;

	pChild = pItem->pFirstChild;
	while( pChild )
	{
		pDel = pChild;
		pChild = pChild->pNext;

		bval = DeleteItem(pDel, false);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::DeleteItem(), failed to call DeleteItem()");
	}

	if( !pItem->pLast )
		pItem->pParent->pFirstChild = pItem->pNext;
	if( !pItem->pNext )
		pItem->pParent->pLastChild = pItem->pLast;

	if( pItem->pLast )
		pItem->pLast->pNext = pItem->pNext;
	if( pItem->pNext )
		pItem->pNext->pLast = pItem->pLast;
	
	m_nCount--;

	if( m_pSelItem == pItem )
		m_pSelItem = NULL;

	delete pItem;
	
	if( bRebuildItems )
	{
		bval = RebuildVisibleItems();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::DeleteItem(), failed to call RebuildVisibleItems()");
	}

	return true;
}

P_AUITREEVIEW_ITEM AUITreeView::InsertItem(const ACHAR *pszText, P_AUITREEVIEW_ITEM pParent, P_AUITREEVIEW_ITEM pAfter)
{
	P_AUITREEVIEW_ITEM pItem = new AUITREEVIEW_ITEM;
	if( !pItem ) return NULL;

	UpdateRenderTarget();

	pItem->strText = UnmarshalEditBoxText(pszText, pItem->itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	pItem->color = m_color;

	if( AUITREEVIEW_ROOT == pParent || !pParent || pParent == &m_RootItem )
	{
		pItem->pParent = &m_RootItem;
		pItem->nIndent = 0;
	}
	else
	{
		pItem->pParent = pParent;
		pItem->nIndent = pParent->nIndent + 1;
	}

	if( AUITREEVIEW_FIRST == pAfter )
	{
		pItem->pNext = pItem->pParent->pFirstChild;
		if( pItem->pNext )
			pItem->pNext->pLast = pItem;
		
		pItem->pParent->pFirstChild = pItem;
		if( !pItem->pParent->pLastChild )
			pItem->pParent->pLastChild = pItem;
	}
	else if( AUITREEVIEW_LAST == pAfter || !pAfter || !pAfter->pNext )
	{
		pItem->pLast = pItem->pParent->pLastChild;
		if( pItem->pLast )
			pItem->pLast->pNext = pItem;

		pItem->pParent->pLastChild = pItem;
		if( !pItem->pParent->pFirstChild )
			pItem->pParent->pFirstChild = pItem;
	}
	else
	{
		pItem->pLast = pAfter;
		pItem->pNext = pAfter->pNext;
		pAfter->pNext->pLast = pItem;
		pAfter->pNext = pItem;
	}
	
	m_nCount++;

	bool bval = RebuildVisibleItems();
	if( !bval ) 
		AUI_ReportError(__LINE__, 1, "AUITreeView::MoveItem(), failed to call RebuildVisibleItems()");

	return pItem;
}

bool AUITreeView::MoveItem(P_AUITREEVIEW_ITEM pItem, P_AUITREEVIEW_ITEM pParent, P_AUITREEVIEW_ITEM pAfter)
{
	if( pItem == &m_RootItem || pItem == pAfter ) return false;

	if( pParent && pParent != AUITREEVIEW_ROOT )
	{
		P_AUITREEVIEW_ITEM pTempItem = pParent;
		do 
		{
			if( pTempItem == pItem )
				return false;

			pTempItem = pTempItem->pParent;
		} while (pTempItem != NULL);
	}

	UpdateRenderTarget();

	bool bval = true;

	if( !pItem->pLast )
		pItem->pParent->pFirstChild = pItem->pNext;
	if( !pItem->pNext )
		pItem->pParent->pLastChild = pItem->pLast;

	if( pItem->pLast )
		pItem->pLast->pNext = pItem->pNext;
	if( pItem->pNext )
		pItem->pNext->pLast = pItem->pLast;
	
	pItem->pParent = NULL;
	pItem->pLast = NULL;
	pItem->pNext = NULL;

	if( AUITREEVIEW_ROOT == pParent || !pParent || pParent == &m_RootItem )
	{
		pItem->pParent = &m_RootItem;
		pItem->nIndent = 0;
	}
	else
	{
		pItem->pParent = pParent;
		pItem->nIndent = pParent->nIndent + 1;
	}

	if( AUITREEVIEW_FIRST == pAfter )
	{
		pItem->pNext = pItem->pParent->pFirstChild;
		if( pItem->pNext )
			pItem->pNext->pLast = pItem;
		
		pItem->pParent->pFirstChild = pItem;
		if( !pItem->pParent->pLastChild )
			pItem->pParent->pLastChild = pItem;
	}
	else if( AUITREEVIEW_LAST == pAfter || !pAfter || !pAfter->pNext )
	{
		pItem->pLast = pItem->pParent->pLastChild;
		if( pItem->pLast )
			pItem->pLast->pNext = pItem;

		pItem->pParent->pLastChild = pItem;
		if( !pItem->pParent->pFirstChild )
			pItem->pParent->pFirstChild = pItem;
	}
	else
	{
		pItem->pLast = pAfter;
		pItem->pNext = pAfter->pNext;
		pAfter->pNext->pLast = pItem;
		pAfter->pNext = pItem;
	}

	bval = RebuildVisibleItems();
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::MoveItem(), failed to call RebuildVisibleItems()");

	return true;
}

P_AUITREEVIEW_ITEM AUITreeView::HitTest(int x, int y, bool *pbOnIcon)
{
	if( !m_pAUIFrame )
		return NULL;

	SIZE sLimit = {0, 0};
	sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();

	if( !m_rcText.PtInRect(x, y) )
		return NULL;

	int nItemIndent = max(1, (m_nItemIndent + m_nTextIndent) * fWindowScale);
	int nItemHeight = max(1, m_nItemHeight + m_nLineSpace * fWindowScale);

	x -= cx;
	y -= cy;

	int nLines;
	int nCurLine = y / nItemHeight;
	int nIndex = m_nFirstItem;
	int nLastVisibleItem = GetLastVisibleItem();
	while( nIndex <= nLastVisibleItem && nCurLine >= m_vecVisibleItems[nIndex]->nLines )
	{
		nCurLine -= m_vecVisibleItems[nIndex]->nLines;
		nIndex++;
	}
	if( nIndex < 0 || nIndex > nLastVisibleItem )
		return NULL;
	
	P_AUITREEVIEW_ITEM pItem = m_vecVisibleItems[nIndex];
	if( x < (pItem->nIndent - m_nHBarPos) * nItemIndent )
		return NULL;
	
	A3DPOINT2 ptSize;
	int nTextW = 0, nTextH = 0;
	FitFont();
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	if( m_pvecImageList && (*m_pvecImageList)[0] )
		AUI_GetTextRect(pFont, pItem->strText, ptSize.x, ptSize.y, nLines, m_nLineSpace * fWindowScale, &pItem->itemsSet, 0, 0, m_pvecImageList, fWindowScale);
	else
		AUI_GetTextRect(pFont, pItem->strText, ptSize.x, ptSize.y, nLines, m_nLineSpace * fWindowScale);
	nTextW = ptSize.x;
	nTextH = ptSize.y;
	if( x > (pItem->nIndent + 1 - m_nHBarPos) * nItemIndent + nTextW )
		return NULL;

	if( pbOnIcon )
	{
		if( x < (pItem->nIndent + 1 - m_nHBarPos) * nItemIndent )
		{
			if( nCurLine == 0 ) 
				*pbOnIcon = true;
			else
			{
				*pbOnIcon = false;
				return NULL;
			}
		}
		else
			*pbOnIcon = false;
	}

	return pItem;
}

bool AUITreeView::RebuildVisibleItems()
{
	UpdateRenderTarget();

	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nItemIndent = max(1, (m_nItemIndent + m_nTextIndent) * fWindowScale);
	int nItemHeight = max(1, m_nItemHeight + m_nLineSpace * fWindowScale);
	int nLinesPP = (m_nHeight - sLimit.cy + m_nLineSpace * fWindowScale) / nItemHeight;
	A3DRECT rcScroll(0, 0, 0, 0);
	if( m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] ) 
	{
		rcScroll = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect();
		rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
			rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	}
	m_vecVisibleItems.clear();
	FitFont();

	m_nHBarMax = 0;
	m_rcText.SetRect(cx, cy, m_nWidth - cx, m_nHeight - cy);
	bool bval = true;
	P_AUITREEVIEW_ITEM pItem = m_RootItem.pFirstChild;
	while( pItem )
	{
		bval = AddVisibleItem(pItem);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RebuildVisibleItems(), failed to call AddVisibleItem()");

		pItem = pItem->pNext;
	}

	m_nRealLines = 0;
	for(DWORD i = 0; i < m_vecVisibleItems.size(); i++)
		m_nRealLines += m_vecVisibleItems[i]->nLines;

	int nScrollW = 0;
	if( m_nRealLines > nLinesPP )
	{
		m_bRenderVScroll = true;
		nScrollW = rcScroll.Width();
		m_rcText.SetRect(cx, cy, m_nWidth - cx - nScrollW, m_nHeight - cy);
		if( m_bAutoWrap )
		{
			m_nHBarMax = 0;
			m_vecVisibleItems.clear();
			P_AUITREEVIEW_ITEM pItem = m_RootItem.pFirstChild;
			while( pItem )
			{
				bval = AddVisibleItem(pItem);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RebuildVisibleItems(), failed to call AddVisibleItem()");

				pItem = pItem->pNext;
			}
			m_nRealLines = 0;
			for(DWORD i = 0; i < m_vecVisibleItems.size(); i++)
				m_nRealLines += m_vecVisibleItems[i]->nLines;
		}
	}
	else
		m_bRenderVScroll = false;


	if( !m_bAutoWrap && m_nHBarMax - m_rcText.Width() > 0 )
	{
		int nScrollH = rcScroll.Width();
		nLinesPP = (m_nHeight - sLimit.cy - nScrollH + m_nLineSpace * fWindowScale) / nItemHeight;
		if( m_nRealLines > nLinesPP )
		{
			m_bRenderVScroll = true;
			nScrollW = rcScroll.Width();
		}
		else
			m_bRenderVScroll = false;
		m_rcText.SetRect(cx, cy, m_nWidth - cx - nScrollW, m_nHeight - cy - nScrollH);
	}

	m_nHBarMax -= m_rcText.Width();
	if( m_nHBarMax > 0 )
	{
		m_nHBarMax = int((m_nHBarMax - 1) / nItemIndent) + 1;
		m_bRenderHScroll = true;
	}
	else
	{
		m_nHBarMax = 0;
		m_bRenderHScroll = false;
	}

	if( m_nHBarPos > m_nHBarMax )
		m_nHBarPos = m_nHBarMax;

	int nLines = 0;
	m_nScrollLines = m_vecVisibleItems.size();
	while( m_nScrollLines > 0 )
	{
		m_nScrollLines--;
		nLines += m_vecVisibleItems[m_nScrollLines]->nLines;
		if( nLines > nLinesPP )
		{
			m_nScrollLines++;
			break;
		}
	}
	if( m_nFirstItem > m_nScrollLines )
		m_nFirstItem = m_nScrollLines;

	return true;
}

bool AUITreeView::AddVisibleItem(P_AUITREEVIEW_ITEM pItem)
{
	bool bval = true;
	m_vecVisibleItems.push_back(pItem);
	if( m_pFont )
	{
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		int nItemIndent = max(1, (m_nItemIndent + m_nTextIndent) * fWindowScale);
	
		if( m_bAutoWrap )
		{
			A3DRECT rc;
			rc.SetRect(0, 0, m_rcText.Width() - (pItem->nIndent + 1) * nItemIndent, 1024);
			if( m_pvecImageList && (*m_pvecImageList)[0] )
			{
				bval = AUI_TextOutFormat(m_pFont, 0, 0, pItem->strText,
					&rc, 0, &pItem->nLines, m_nLineSpace * fWindowScale, true, &pItem->nItemHeight, NULL, NULL, NULL, false, m_nShadow, 255, true, &pItem->itemsSet, m_pvecImageList, fWindowScale);
				if( !bval ) AUI_ReportError(__LINE__, 1, "AUIListBox::CalcTextLines(), failed to call AUI_TextOutFormat()");
			}
			else
			{
				bval = AUI_TextOutFormat(m_pFont, 0, 0, pItem->strText,
					&rc, 0, &pItem->nLines, m_nLineSpace * fWindowScale, true, &pItem->nItemHeight, NULL, NULL, NULL, false, m_nShadow, 255, true, &pItem->itemsSet, NULL, fWindowScale);
				if( !bval ) AUI_ReportError(__LINE__, 1, "AUIListBox::CalcTextLines(), failed to call AUI_TextOutFormat()");
			}
		}
		else
		{
			A3DPOINT2 ptSize;
			float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
			if( m_pvecImageList && (*m_pvecImageList)[0] )
				AUI_GetTextRect(m_pFont, pItem->strText, ptSize.x, pItem->nItemHeight, pItem->nLines, m_nLineSpace * fWindowScale, &pItem->itemsSet, 0, 0, m_pvecImageList, fWindowScale);
			else
				AUI_GetTextRect(m_pFont, pItem->strText, ptSize.x, pItem->nItemHeight, pItem->nLines, m_nLineSpace * fWindowScale);
			if( (pItem->nIndent + 1) * nItemIndent + ptSize.x > m_nHBarMax )
				m_nHBarMax = (pItem->nIndent + 1) * nItemIndent + ptSize.x;
		}
	}

	if( pItem->dwStatus & AUITREEVIEW_IS_EXPANDED )
	{
		bool bval = true;
		pItem = pItem->pFirstChild;
		while( pItem )
		{
			bval = AddVisibleItem(pItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUITreeView::RebuildVisibleItems(), failed to call AddVisibleItem()");

			pItem = pItem->pNext;
		}
	}

	return true;
}

void AUITreeView::Expand(P_AUITREEVIEW_ITEM pItem, int idAction, bool bRecursive)
{
	if( AUITREEVIEW_EXPAND_EXPAND == idAction )
	{	
		if (bRecursive && pItem->pFirstChild)
		{
			abase::vector<P_AUITREEVIEW_ITEM> vecPtrItem;
			vecPtrItem.push_back(pItem);
			int sz = vecPtrItem.size();
			while (sz > 0)
			{
				for (int i=0; i<sz; i++)
				{
					vecPtrItem[i]->dwStatus |= AUITREEVIEW_IS_EXPANDED;

					if (vecPtrItem[i]->pFirstChild)
					{
						P_AUITREEVIEW_ITEM pT = vecPtrItem[i]->pFirstChild;
						while (true)
						{
							vecPtrItem.push_back(pT);
							if (pT == vecPtrItem[i]->pLastChild)
								break;
							pT = pT->pNext;
						}
					}
				}
				vecPtrItem.erase(vecPtrItem.begin(), vecPtrItem.begin()+sz);
				sz = vecPtrItem.size();
			}
		}
		else
		{
			pItem->dwStatus |= AUITREEVIEW_IS_EXPANDED;
		}
	}
	else if( AUITREEVIEW_EXPAND_COLLAPSE == idAction )
	{
		if (bRecursive && pItem->pFirstChild)
		{
			abase::vector<P_AUITREEVIEW_ITEM> vecPtrItem;
			vecPtrItem.push_back(pItem);
			int sz = vecPtrItem.size();
			while (sz > 0)
			{
				for (int i=0; i<sz; i++)
				{
					vecPtrItem[i]->dwStatus &= ~AUITREEVIEW_IS_EXPANDED;

					if (vecPtrItem[i]->pFirstChild)
					{
						P_AUITREEVIEW_ITEM pT = vecPtrItem[i]->pFirstChild;
						while (true)
						{
							vecPtrItem.push_back(pT);
							if (pT == vecPtrItem[i]->pLastChild)
								break;
							pT = pT->pNext;
						}
					}
				}
				vecPtrItem.erase(vecPtrItem.begin(), vecPtrItem.begin()+sz);
				sz = vecPtrItem.size();
			}
		}
		else
		{
			pItem->dwStatus &= ~AUITREEVIEW_IS_EXPANDED;
		}
	}
	else if( AUITREEVIEW_EXPAND_TOGGLE == idAction )
	{
		if( pItem->dwStatus & AUITREEVIEW_IS_EXPANDED )
			pItem->dwStatus &= ~AUITREEVIEW_IS_EXPANDED;
		else
			pItem->dwStatus |= AUITREEVIEW_IS_EXPANDED;	
	}

	if( pItem->dwStatus & AUITREEVIEW_IS_EXPANDED )
	{
		pItem = pItem->pParent;
		while( pItem )
		{
			pItem->dwStatus |= AUITREEVIEW_IS_EXPANDED;
			pItem = pItem->pParent;
		}
	}

	bool bval = RebuildVisibleItems();
	ASSERT(bval);
}

P_AUITREEVIEW_ITEM AUITreeView::GetSelectedItem()
{
	return m_pSelItem;
}

void AUITreeView::SelectItem(P_AUITREEVIEW_ITEM pItem)
{
	if( pItem != m_pSelItem )
	{
		UpdateRenderTarget();
		m_pParent->SetCommand(m_strCommand.GetLength()>0?m_strCommand:m_szName);
	}
	if( m_pSelItem )
		m_pSelItem->dwStatus &= ~AUITREEVIEW_IS_SELECTED;
	m_pSelItem = pItem;
	if( m_pSelItem )
		m_pSelItem->dwStatus |= AUITREEVIEW_IS_SELECTED;
}

void AUITreeView::BlinkItem(P_AUITREEVIEW_ITEM pItem, bool bBlink)
{
	UpdateRenderTarget();

	if( bBlink )
		pItem->dwStatus |= AUITREEVIEW_IS_BLINKING;
	else
		pItem->dwStatus &= ~AUITREEVIEW_IS_BLINKING;
}

void AUITreeView::SetColor(A3DCOLOR color)
{
	UpdateRenderTarget();

	SetChildrenColor(&m_RootItem, color);
	AUIObject::SetColor(color);
}

void AUITreeView::SetChildrenColor(P_AUITREEVIEW_ITEM pItem, A3DCOLOR color)
{
	UpdateRenderTarget();

	pItem->color = color;
	pItem = pItem->pFirstChild;
	while( pItem )
	{
		SetChildrenColor(pItem, color);
		pItem = pItem->pNext;
	}
}

int AUITreeView::GetItemIndent(P_AUITREEVIEW_ITEM pItem)
{
	return pItem->nIndent;
}

DWORD AUITreeView::GetItemState(P_AUITREEVIEW_ITEM pItem)
{
	return pItem->dwStatus;
}

A3DRECT AUITreeView::GetItemRect(P_AUITREEVIEW_ITEM pItem)
{
	return pItem->rcItem;
}

const ACHAR * AUITreeView::GetItemHint(P_AUITREEVIEW_ITEM pItem)
{
	return pItem->strHint;
}

void AUITreeView::SetItemHint(P_AUITREEVIEW_ITEM pItem, const ACHAR *pszHint)
{
	pItem->strHint = pszHint;
}

bool AUITreeView::IsInHitArea(int x, int y, UINT msg)
{
	if( !AUIObject::IsInHitArea(x, y, msg) )
		return false;

	if( !m_bTextTransparent || !m_bTransScrollWheel && !m_bOnlyShowText && msg == WM_MOUSEWHEEL && m_nScrollLines > 0 )
		return true;

	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;

	POINT ptPos = GetPos();
	x -= m_nOffX + ptPos.x;
	y -= m_nOffY + ptPos.y;
	if( !m_rcText.PtInRect(x, y) )
		return true;

	bool bOnIcon;
	P_AUITREEVIEW_ITEM pItem = HitTest(x, y, &bOnIcon);
	if( pItem )
		return true;

	return false;
}

void AUITreeView::SetImageList(abase::vector<A2DSprite*> *pvecImageList)
{
	m_pvecImageList = pvecImageList;
}

void AUITreeView::SetSize(int W, int H)
{
	if( W != m_nWidth || H != m_nHeight )
	{
		AUIObject::SetSize(W, H);
		RebuildVisibleItems();
	}
	else
		AUIObject::SetSize(W, H);
}

void AUITreeView::SetDefaultSize(int W, int H)
{
	if( W != m_nDefaultWidth || H != m_nDefaultHeight )
	{
		AUIObject::SetDefaultSize(W, H);
		RebuildVisibleItems();
	}
	else
		AUIObject::SetDefaultSize(W, H);
}

bool AUITreeView::SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
	A3DCOLOR dwColor, int nShadow, int nOutline, bool bBold, bool bItalic, bool bFreeType,
	A3DCOLOR clOutline, A3DCOLOR clInnerText, bool bInterpColor, A3DCOLOR clUpper, A3DCOLOR clLower)
{
	bool bval=AUIObject::SetFontAndColor(pszFontName, nFontHeight, dwColor, nShadow, nOutline, 
							bBold, bItalic, bFreeType, clOutline, clInnerText, bInterpColor, clUpper, clLower);
	RebuildVisibleItems();

	return bval;
}

bool AUITreeView::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nTextH = m_pFont->GetFontHeight();
	int nIconH = 0;
	if (m_pA2DSprite[AUITREEVIEW_RECT_PLUS])
		nIconH = max(nIconH, m_pA2DSprite[AUITREEVIEW_RECT_PLUS]->GetHeight());
	if (m_pA2DSprite[AUITREEVIEW_RECT_MINUS])
		nIconH = max(nIconH, m_pA2DSprite[AUITREEVIEW_RECT_MINUS]->GetHeight());
	if (m_pA2DSprite[AUITREEVIEW_RECT_LEAF])
		nIconH = max(nIconH, m_pA2DSprite[AUITREEVIEW_RECT_LEAF]->GetHeight());
	if (m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK])
		nIconH = max(nIconH, m_pA2DSprite[AUITREEVIEW_RECT_LEAFCHECK]->GetHeight());
	m_nItemHeight = max(nTextH, nIconH*fWindowScale);	

	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	RebuildVisibleItems();
	
	return true;
}

const VEC_AUITREEVIEW_ITEM & AUITreeView::GetDisplayItems()
{
	m_vecDisplayItems.clear();
	int nEnd = GetLastVisibleItem();
	for( int i = m_nFirstItem; i <= nEnd; i++ )
		m_vecDisplayItems.push_back(m_vecVisibleItems[i]);
	return m_vecDisplayItems;
}

void AUITreeView::EnsureVisible(int nIndex)
{
	if( nIndex <= m_nFirstItem)
		SetFirstItem(nIndex);
	else
	{
		int nEnd = GetLastVisibleItem();
		if( nIndex > nEnd )
		{
			SetFirstItem(m_nFirstItem + 1);
			EnsureVisible(nIndex);
		}
	}
}

int AUITreeView::GetRequiredHeight()
{
	SIZE sLimit = { 0, 0 };
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nItemHeight = m_nItemHeight + m_nLineSpace * fWindowScale;

	return nItemHeight * m_nRealLines + sLimit.cy + 1;
}

void AUITreeView::SetFirstItem(int nFirstItem)
{
	//a_ClampFloor(nFirstItem, 0);
	a_Clamp(nFirstItem, 0, m_nScrollLines);
	if( nFirstItem != m_nFirstItem )
		UpdateRenderTarget();

	m_nFirstItem = nFirstItem;
}

void AUITreeView::SetHBarPos(int nPos)
{
	a_Clamp(nPos, 0, m_nHBarMax);

	if( nPos != m_nHBarPos )
		UpdateRenderTarget();

	m_nHBarPos = nPos;
}

void AUITreeView::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUITreeView::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	for (int i = 0; i < AUITREEVIEW_RECT_NUM; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);
}

int AUITreeView::GetHitArea(int x, int y)
{
	if( !m_pAUIFrame ||
		!m_pA2DSprite[AUITREEVIEW_RECT_UP] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_DOWN] ||
		!m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL] )
	{
		return AUITREEVIEW_RECT_FRAME;
	}

	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int iUpHeight = m_pA2DSprite[AUITREEVIEW_RECT_UP]->GetItem(0)->GetRect().Height() * fWindowScale;
	int iDownHeight = m_pA2DSprite[AUITREEVIEW_RECT_DOWN]->GetItem(0)->GetRect().Height() * fWindowScale;
	int iScrollThickWidth = m_pA2DSprite[AUITREEVIEW_RECT_VSCROLL]->GetItem(0)->GetRect().Width() * fWindowScale;
	int iBarLength = m_pA2DSprite[AUITREEVIEW_RECT_VBAR]->GetItem(0)->GetRect().Height() * fWindowScale;

	A3DRECT rcText(cx, cy, m_nWidth	- cx, m_nHeight - cy);
	A3DRECT rcScrollAreaH(cx, m_nHeight-iScrollThickWidth-cy, m_nWidth-cx, m_nHeight-cy);
	A3DRECT rcScrollAreaV(m_nWidth-cx-iScrollThickWidth, cy, m_nWidth-cx, m_nHeight-cy);

	if (m_bRenderHScroll)
	{
		rcText.bottom -= iScrollThickWidth;
	}
	if (m_bRenderVScroll)
	{
		rcText.right -= iScrollThickWidth;
		rcScrollAreaH.right -= iScrollThickWidth;
	}

	// Click on text item area.
	if( rcText.PtInRect(x, y) )
		return AUITREEVIEW_RECT_FRAME;

	if (m_bRenderHScroll && rcScrollAreaH.PtInRect(x, y))
	{
		if (x < rcScrollAreaH.left + iUpHeight)
			return AUITREEVIEW_RECT_LEFT;
		if (x > rcScrollAreaH.right - iDownHeight)
			return AUITREEVIEW_RECT_RIGHT;
		if (x >= m_ptHBar.x && x <= m_ptHBar.x + iBarLength)
			return AUITREEVIEW_RECT_HBAR;
		
		return AUITREEVIEW_RECT_HSCROLL;
	}

	if (m_bRenderVScroll && rcScrollAreaV.PtInRect(x, y))
	{
		if (y < rcScrollAreaV.top + iUpHeight)
			return AUITREEVIEW_RECT_UP;
		if (y > rcScrollAreaV.bottom - iDownHeight)
			return AUITREEVIEW_RECT_DOWN;
		if (y >= m_ptVBar.y && y <= m_ptVBar.y + iBarLength)
			return AUITREEVIEW_RECT_VBAR;
		
		return AUITREEVIEW_RECT_VSCROLL;
	}

	return AUITREEVIEW_RECT_FRAME;
}

int AUITreeView::GetLastVisibleItem()
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nItemHeight = max(1, m_nItemHeight + m_nLineSpace * fWindowScale);
	int nLinesPP = (int)((m_rcText.Height() + m_nLineSpace * fWindowScale) / nItemHeight);
	
	int nStart = m_nFirstItem;
	int nEnd = m_nFirstItem;
	int nTotalLines = 0;
	while( nEnd < m_vecVisibleItems.size() && nTotalLines + m_vecVisibleItems[nEnd]->nLines <= nLinesPP )
	{
		nTotalLines += m_vecVisibleItems[nEnd]->nLines;
		nEnd++;
	}
	if (nEnd == m_vecVisibleItems.size() || nTotalLines == nLinesPP)
		nEnd--;

	return nEnd;
}

void AUITreeView::CheckLeafItem(P_AUITREEVIEW_ITEM pItem,bool bCheck)
{
	if (pItem && !pItem->pFirstChild)
	{
		if( bCheck )		
			pItem->dwStatus |= AUITREEVIEW_IS_LEAFCHECK;
		else
			pItem->dwStatus &= ~AUITREEVIEW_IS_LEAFCHECK;
	}
}

bool AUITreeView::GetLeafItemCheck(P_AUITREEVIEW_ITEM pItem)
{
	if (pItem && !pItem->pFirstChild)
		return !!(pItem->dwStatus & AUITREEVIEW_IS_LEAFCHECK);
	else
		return false;
}

void AUITreeView::ClearItemRcInfo()
{
	m_vEBItems.clear();
	m_vNameLinks.clear();
}