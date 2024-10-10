// Filename	: AUIListBox.cpp
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIListBox is the class of list box control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "AMemFile.h"
#include "CSplit.h"

#include "AUIDef.h"
#include "AUICommon.h"
#include "AUIDialog.h"
#include "AUIListBox.h"
#include "AUIManager.h"
#include "AUIComboBox.h"
#include "AUICTranslate.h"
#include "AUITag.h"

#define AUILISTBOX_SCROLL_DELAY		50
#define AUILISTBOX_LINESPACE		0

AUIListBox::AUIListBox() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_LISTBOX;
	m_bHOver = false;
	m_bAutoWrap = false;
	m_nLinesCurPage = 0;
	m_bDraggingVScroll = false;
	m_bDraggingHScroll = false;
	m_nCurSel = -1;
	m_nCurColSel = 0;
	m_nFirstItem = 0;
	m_nScrollDelay = GetTickCount();
	m_ptVBar.x = m_ptVBar.y = m_nDragDist = 0;
	m_ptHBar.x = m_ptHBar.y = 0;
	m_bRenderHScroll = false;
	m_bRenderVScroll = false;
	m_nHBarPos = 0;
	m_nHBarMax = 0;
	m_pFont = NULL;
	m_nFontHeight = 10;
	m_nFrameMode = 0;
	m_pAUIFrame = NULL;
	m_pComboBox = NULL;
	for( int i = 0; i < AUILISTBOX_RECT_NUM; i++ )
	{
		m_pA2DSprite[i] = NULL;
		m_rcImage[i].Clear();
	}

	m_strCols = "0";
	m_nNumColumns = 0;
	memset(m_nColWidth, 0, sizeof(int) * AUILISTBOX_MAX_COLUMN);
	memset(m_nColAlign, AUIFRAME_ALIGN_LEFT, sizeof(int) * AUILISTBOX_MAX_COLUMN);
	m_nNumSelections = 0;
	m_bMultipleSelection = false;
	m_nState = AUILISTBOX_STATE_NORMAL;
	m_bForceRenderScroll = false;
	m_nScrollLines = 0;
	m_pvecImageList = NULL;
	m_nItemMask = 0;
	m_nLineSpace = AUILISTBOX_LINESPACE;
	m_nAlign = AUIFRAME_ALIGN_TOP;

	m_nRowMaxWidth = 0;
	m_nRowFixedHeight = 0;
	m_bEnableHScroll = false;
	m_bItemTextSmallImage = false;
}

AUIListBox::~AUIListBox()
{
}

bool AUIListBox::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	ListBox property line format:
	 *
	 *		LIST					Keyword
	 *		Server					Name
	 *		1						String ID of command.
	 *		10 10 200 25			x y Width Height
	 *		1						String ID of font name
	 *		20						Font height
	 *		0 0 0 255				Text color, R, G, B, A
	 *		"ListFrame.bmp"			ImageFileName
	 *		"ListHilight.bmp"		ImageFileName
	 *		"ListUp.bmp"			ImageFileName
	 *		"ListDown.bmp"			ImageFileName
	 *		"ListScroll.bmp"		ImageFileName
	 *		"ListBar.bmp"			ImageFileName
	 *		0						Multiple selection or not
	 *		"50 50 100"				Columns
	 */

	
	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIListBox::Init(), failed to call base AUIObject::Init()");

	BEGIN_FAKE_WHILE2;
	
	if( pASF )
	{
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
			return AUI_ReportError(__LINE__, 1, "AUIListBox::Init(), failed to call SetFontAndColor()");

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

		for( int i = AUILISTBOX_RECT_MIN; i < AUILISTBOX_RECT_MAX; i++ )
			if( i != AUILISTBOX_RECT_ITEM )
			{
				if (!pASF->GetNextToken(true))
					return AUI_ReportError(__LINE__, 1, "AUIListBox::Init(), failed to read from file");

				if (!InitIndirect(pASF->m_szToken, i))
					return AUI_ReportError(__LINE__, 1, "AUIListBox::Init(), failed to call InitIndirect()");
			}

		CHECK_BREAK2(pASF->GetNextToken(true));
		m_bMultipleSelection = atoi(pASF->m_szToken) ? true : false;

		CHECK_BREAK2(pASF->GetNextToken(true));
		m_strCols = pASF->m_szToken;

		if (!BuildColumnsInfo(m_strCols))
			return AUI_ReportError(__LINE__, 1, "AUIListBox::Init(), failed to call BuildColumnsInfo()");
		
		if( m_pParent->GetFileVersion() > 0 )
		{
			CHECK_BREAK2(pASF->GetNextToken(true));
			AUIOBJECT_SETPROPERTY p;
			p.i = atoi(pASF->m_szToken);
			SetProperty("Frame Mode", &p);
		}
	
	}

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIListBox::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIListBox::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
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
	XMLGET_INT_VALUE(pItem, _AL("RowFixedHeight"), m_nRowFixedHeight)
	XMLGET_INT_VALUE(pItem, _AL("Align"), m_nAlign)
	XMLGET_BOOL_VALUE(pItem, _AL("AutoWrap"), m_bAutoWrap)
	XMLGET_BOOL_VALUE(pItem, _AL("EnableHScroll"), m_bEnableHScroll)
	XMLGET_BOOL_VALUE(pItem, _AL("MultiSel"), m_bMultipleSelection)
	XMLGET_STRING_VALUE(pItem, _AL("SoundEffect"), m_strSound)
#ifdef _ANGELICA_AUDIO
	LoadAudioInstance(m_strSound, m_pAudio);
#endif
	XMLGET_STRING_VALUE(pItem, _AL("MultiLine"), m_strCols)
	XMLGET_INT_VALUE(pItem, _AL("LineSpace"), m_nLineSpace)
	XMLGET_BOOL_VALUE(pItem, _AL("HoverHilight"), m_bHOver)
	BuildColumnsInfo(m_strCols);

	XMLGET_STRING_VALUE(pItem, _AL("Command"), m_strCommand)
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
					InitIndirect(strFileName, AUILISTBOX_RECT_FRAME);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("HilightImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUILISTBOX_RECT_HILIGHT);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("UpImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUILISTBOX_RECT_UP);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("DownImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUILISTBOX_RECT_DOWN);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUILISTBOX_RECT_VSCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("BarImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUILISTBOX_RECT_VBAR);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ItemImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUILISTBOX_RECT_ITEM);
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
		return AUI_ReportError(__LINE__, 1, "AUIListBox::InitXML(), failed to call SetFontAndColor()");
	
	return true;
}

bool AUIListBox::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUILISTBOX_RECT_MIN && idType < AUILISTBOX_RECT_MAX);

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
			return AUI_ReportError(__LINE__, 1, "AUIListBox::InitIndirect(), failed to call m_pAUIFrame->Init()");
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
			return AUI_ReportError(__LINE__, 1, "AUIListBox::InitIndirect(), failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUILISTBOX_RECT_UP == idType || AUILISTBOX_RECT_DOWN == idType )
		{
			A3DRECT rc[2];
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight();
			rc[0].SetRect(0, 0, W, H / 2);
			rc[1].SetRect(0, H / 2, W, H);
			bval = m_pA2DSprite[idType]->ResetItems(2, rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::InitIndirect(), failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
		
		m_rcImage[idType] = m_pA2DSprite[idType]->GetItem(0)->GetRect();
	}

	UpdateScroll();

	return true;
}

bool AUIListBox::Release(void)
{
	ResetContent();
	if( m_pComboBox )
		m_pComboBox->SetListBox(NULL);

	A3DRELEASE(m_pAUIFrame);
	for( int i = 0; i < AUILISTBOX_RECT_NUM; i++ )
		A3DRELEASE(m_pA2DSprite[i]);

	return AUIObject::Release();
}

bool AUIListBox::Save(FILE *file)
{
	fprintf(file, "LIST");

	if( !AUIObject::Save(file) ) return false;

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_szFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idString = m_pParent->AddStringToTable(szFontName);
	if( idString < 0 ) return false;

	fprintf(file, " %d %d %d %d %d %d", idString, m_nFontHeight,
		A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color),
		A3DCOLOR_GETBLUE(m_color), A3DCOLOR_GETALPHA(m_color));

	for( int i = 0; i < AUILISTBOX_RECT_NUM; i++ )
	{
		if( AUILISTBOX_RECT_FRAME == i )
		{
			if( m_pAUIFrame )
				m_pAUIFrame->Save(file);
			else
				fprintf(file, " \"\"");
		}
		else
			fprintf(file, " \"%s\"", m_pA2DSprite[i] ? m_pA2DSprite[i]->GetName() : "");
	}
	
	fprintf(file, " %d \"%s\" %d\n", m_bMultipleSelection, m_strCols, m_nFrameMode);

	return true;
}

bool AUIListBox::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemListBox = new AXMLItem;
	pXMLItemListBox->InsertItem(pXMLItemDialog);
	pXMLItemListBox->SetName(_AL("LIST"));
	XMLSET_STRING_VALUE(pXMLItemListBox, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemListBox, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemListBox, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemListBox, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemListBox, _AL("Height"), m_nHeight, 0)
	XMLSET_INT_VALUE(pXMLItemListBox, _AL("RowFixedHeight"), m_nRowFixedHeight, 0)
	XMLSET_INT_VALUE(pXMLItemListBox, _AL("Align"), m_nAlign, 0)
	XMLSET_BOOL_VALUE(pXMLItemListBox, _AL("AutoWrap"), m_bAutoWrap, false)
	XMLSET_BOOL_VALUE(pXMLItemListBox, _AL("EnableHScroll"), m_bEnableHScroll, false)
	XMLSET_BOOL_VALUE(pXMLItemListBox, _AL("MultiSel"), m_bMultipleSelection, false)
	XMLSET_STRING_VALUE(pXMLItemListBox, _AL("MultiLine"), AS2AC(m_strCols), _AL("0"))
	XMLSET_STRING_VALUE(pXMLItemListBox, _AL("SoundEffect"), AS2AC(m_strSound), _AL(""))
	XMLSET_INT_VALUE(pXMLItemListBox, _AL("LineSpace"), m_nLineSpace, AUILISTBOX_LINESPACE)
	XMLSET_BOOL_VALUE(pXMLItemListBox, _AL("HoverHilight"), m_bHOver, false)
	XMLSET_STRING_VALUE(pXMLItemListBox, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemListBox);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	XMLSET_STRING_VALUE(pXMLItemListBox, _AL("Command"), AS2AC(m_strCommand), _AL(""))
	
	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemListBox);
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
	pXMLItemResource->InsertItem(pXMLItemListBox);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pAUIFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	if( m_pA2DSprite[AUILISTBOX_RECT_HILIGHT] )
	{
		AXMLItem *pXMLItemHilightImage = new AXMLItem;
		pXMLItemHilightImage->InsertItem(pXMLItemResource);
		pXMLItemHilightImage->SetName(_AL("HilightImage"));
		XMLSET_STRING_VALUE(pXMLItemHilightImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUILISTBOX_RECT_UP] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("UpImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUILISTBOX_RECT_UP]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUILISTBOX_RECT_DOWN] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("DownImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUILISTBOX_RECT_DOWN]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUILISTBOX_RECT_VSCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("ScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUILISTBOX_RECT_VBAR] )
	{
		AXMLItem *pXMLItemBarImage = new AXMLItem;
		pXMLItemBarImage->InsertItem(pXMLItemResource);
		pXMLItemBarImage->SetName(_AL("BarImage"));
		XMLSET_STRING_VALUE(pXMLItemBarImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUILISTBOX_RECT_ITEM] )
	{
		AXMLItem *pXMLItemItemImage = new AXMLItem;
		pXMLItemItemImage->InsertItem(pXMLItemResource);
		pXMLItemItemImage->SetName(_AL("ItemImage"));
		XMLSET_STRING_VALUE(pXMLItemItemImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUILISTBOX_RECT_ITEM]->GetName()), _AL(""))
	}

	return true;
}

bool AUIListBox::BuildColumnsInfo(const char *pszColInfo)
{
	bool bval = true;

	AMemFile m;
	bval = m.Attach((BYTE *)pszColInfo, strlen(pszColInfo), 0);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::BuildColumnsInfo(), failed to call m.Attach(), [m is a object of AMemFile]");

	AScriptFile s;
	bval = s.Open(&m);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::BuildColumnsInfo(), failed to call s.Open(), [m is a object of AScriptFile]");

	m_nNumColumns = 0;
	while( !s.IsEnd() && m_nNumColumns < AUILISTBOX_MAX_COLUMN )
	{
		bval = s.GetNextToken(true);
		if( !bval ) break;		// End of file.

		if( s.m_szToken[0] == '+' )
			m_nColAlign[m_nNumColumns] = AUIFRAME_ALIGN_CENTER;
		else if( s.m_szToken[0] == '-' )
			m_nColAlign[m_nNumColumns] = AUIFRAME_ALIGN_RIGHT;
		else
			m_nColAlign[m_nNumColumns] = AUIFRAME_ALIGN_LEFT;
		m_nColWidth[m_nNumColumns] = abs(atoi(s.m_szToken));
		m_nNumColumns++;
	}

	s.Close();
	m.Detach();

	UpdateScroll();

	return true;
}

bool AUIListBox::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( !m_pAUIFrame ||
		!m_pA2DSprite[AUILISTBOX_RECT_UP] ||
		!m_pA2DSprite[AUILISTBOX_RECT_DOWN] ||
		!m_pA2DSprite[AUILISTBOX_RECT_VSCROLL] )
	{
		return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
	}

	int nLines = m_Item.size();
	SIZE sLimit = { 0, 0 };
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();

	int nLinesPP = GetLinesPerPage();

	m_nHOverItem = -1;
	m_nHOverItemCol = -1;

	if( msg == WM_LBUTTONDOWN ||
		msg == WM_LBUTTONDBLCLK ||
		msg == WM_MOUSEMOVE ||
		msg == WM_LBUTTONUP ||
		msg == WM_RBUTTONUP )
	{
		POINT ptPos = GetPos();
		int x = GET_X_LPARAM(lParam) - m_nOffX - ptPos.x;
		int y = GET_Y_LPARAM(lParam) - m_nOffY - ptPos.y;

		if( msg == WM_LBUTTONDOWN ||
			msg == WM_LBUTTONDBLCLK ||
			msg == WM_RBUTTONUP )
			m_nCurColSel = m_nNumColumns;

		if( WM_LBUTTONUP == msg )
		{
			m_bDraggingVScroll = false;
			m_bDraggingHScroll = false;
			m_nDragDist = 0;
			m_nState = AUILISTBOX_STATE_NORMAL;
			m_pParent->SetCaptureObject(NULL);

			if( m_pComboBox && m_rcImage[AUILISTBOX_RECT_FRAME].PtInRect(x, y) )
			{
				if( m_pComboBox->GetCurSel() != m_nCurSel &&
					m_nCurSel >= 0 && m_nCurSel < (int)m_Item.size() )
				{
					m_pComboBox->SetCurSel(m_nCurSel);
					m_pComboBox->Command();
				}
				m_pParent->ChangeFocus(NULL);
				Show(false);
				m_pComboBox->SetState(AUICOMBOBOX_STATE_NORMAL);
			}
		}
		// Click on VScroll Area.
		else if( m_bDraggingVScroll || m_rcImage[AUILISTBOX_RECT_VBAR].PtInRect(x, y) )
		{

			int nBarH = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height();
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUILISTBOX_STATE_PRESSVBAR == m_nState && 
						( !m_bAutoWrap && nLines > m_nLinesCurPage ||
						m_bAutoWrap && m_nScrollLines != 0 ))
					{
						int H = m_rcImage[AUILISTBOX_RECT_VBAR].Height() - nBarH;
						
						if( H > 0 )
						{
							if( m_bAutoWrap )
							{
								SetFirstItem((y - m_rcImage[AUILISTBOX_RECT_VBAR].top - m_nDragDist) * m_nScrollLines / H);
								a_Clamp(m_nFirstItem, 0, m_nScrollLines);
							}
							else
							{
								int L = nLines - m_nLinesCurPage;
								SetFirstItem((y - m_rcImage[AUILISTBOX_RECT_VBAR].top - m_nDragDist) * (L + 1) / H);
								a_Clamp(m_nFirstItem, 0, int(m_Item.size()) - m_nLinesCurPage);
							}
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
					if( msg == WM_LBUTTONDOWN )
						return OnDlgItemMessage(WM_KEYDOWN, VK_PRIOR, 0);
				}
				// Page down.
				else if( y > m_ptVBar.y + nBarH )
				{
					if( msg == WM_LBUTTONDOWN )
						return OnDlgItemMessage(WM_KEYDOWN, VK_NEXT, 0);
				}
				// VScroll bar.
				else if( !m_bAutoWrap && nLines > m_nLinesCurPage ||
					m_bAutoWrap && m_nScrollLines != 0 )
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDraggingVScroll = true;
						m_nDragDist = y - m_ptVBar.y;
						m_nState = AUILISTBOX_STATE_PRESSVBAR;
						m_pParent->SetCaptureObject(this);
					}
				}
			}
		}
		// Click On HScroll Bar
		else if( m_bDraggingHScroll || m_bRenderHScroll && m_rcImage[AUILISTBOX_RECT_HBAR].PtInRect(x, y) )
		{

			int nBarV = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height();
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUILISTBOX_STATE_PRESSHBAR == m_nState && m_bRenderHScroll )
					{
						int L = m_nHBarMax;
						int H = m_rcImage[AUILISTBOX_RECT_HBAR].Width() - nBarV;
						
						if( H > 0 )
							SetHBarPos((x - m_rcImage[AUILISTBOX_RECT_HBAR].left - m_nDragDist) * (L + 1) / H);
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
				// Page up.
				if( x < m_ptHBar.x )
				{
					if( msg == WM_LBUTTONDOWN )
						SetHBarPos(m_nHBarPos - 1);
				}
				// Page down.
				else if( x > m_ptHBar.x + nBarV )
				{
					if( msg == WM_LBUTTONDOWN )
						SetHBarPos(m_nHBarPos + 1);
				}
				// Scroll bar.
				else if( m_bRenderHScroll)
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDraggingHScroll = true;
						m_nDragDist = x - m_ptHBar.x;
						m_nState = AUILISTBOX_STATE_PRESSHBAR;
						m_pParent->SetCaptureObject(this);
					}
				}
			}
		}
		// Click on item area.
		else if( m_rcImage[AUILISTBOX_RECT_FRAME].PtInRect(x, y) )
		{
			x -= m_rcImage[AUILISTBOX_RECT_FRAME].left;
			y -= m_rcImage[AUILISTBOX_RECT_FRAME].top;

			int nLine = y / GetLineHeight();
			if( nLine < 0 )
				nLine = 0;
			else if( nLine >= nLinesPP )
				nLine = nLinesPP - 1;

			if( msg == WM_MOUSEMOVE ||
				msg == WM_LBUTTONDOWN ||
				msg == WM_LBUTTONDBLCLK ||
				msg == WM_RBUTTONUP )
			{
				int nCurSel = nLine + m_nFirstItem;
				if( m_bAutoWrap )
				{
					int nLinesCount = 0;
					for( int i = m_nFirstItem; i < m_nFirstItem + m_nLinesCurPage; i++ )
					{
						if( nLine >= nLinesCount && nLine < nLinesCount + m_Item[i].nLines )
						{
							nCurSel = i;
							break;
						}
						nLinesCount += m_Item[i].nLines;
					}
				}
				m_nHOverItem = nCurSel;

				int nCurColSel = 0;
				if( nCurSel >= 0 && nCurSel < int(m_Item.size()) )
				{
					while( nCurColSel < m_nNumColumns - 1 && x > m_nColWidth[nCurColSel] * fWindowScale )
					{
						x -= m_nColWidth[nCurColSel] * fWindowScale;
						nCurColSel++;
					}
				}
				else
					nCurColSel = m_nNumColumns;

				m_nHOverItemCol = nCurColSel;

				if( msg == WM_LBUTTONDOWN ||
					msg == WM_LBUTTONDBLCLK ||
					(msg == WM_MOUSEMOVE && m_bHOver) ||
					msg == WM_RBUTTONUP )
				{
					if( nCurSel >= 0 && nCurSel < int(m_Item.size()) )
					{
						if( m_bMultipleSelection )
						{
							if( WM_LBUTTONDOWN == msg || msg == WM_RBUTTONUP )
							{
								if( wParam & MK_CONTROL )
								{
									SetSel(nCurSel, !m_Item[nCurSel].bSelState);
								}
								else if( wParam & MK_SHIFT && m_nCurSel != -1)
								{
									BatchSetSel(0, (int)m_Item.size() - 1, false);
									int nMin = min(m_nCurSel, nCurSel);
									int nMax = max(m_nCurSel, nCurSel);
									BatchSetSel(nMin, nMax, true);
								}
								else
								{
									BatchSetSel(0, (int)m_Item.size() - 1, false);
									SetSel(nCurSel, true);
									SetCurSel(nCurSel);
								}
							}
						}
						else
						{
							if( m_nCurSel >= 0 && m_nCurSel < int(m_Item.size()) )
								m_Item[m_nCurSel].bSelState = false;
							SetCurSel(nCurSel);
							SetSel(m_nCurSel, true);
						}

#ifdef _ANGELICA_AUDIO
						if( m_pAudio )
							m_pParent->GetAUIManager()->PlayAudioInstance(m_pAudio);
						else if( strlen(m_strSound) > 0 )
							m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#else
						if( strlen(m_strSound) > 0 )
							m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#endif

						if( msg == WM_LBUTTONDBLCLK )
						{
							if( m_strCommand != "" )
								m_pParent->SetCommand(m_strCommand);
							else
								m_pParent->SetCommand(m_szName);
						}

						m_nCurColSel = nCurColSel;
					}
					else
					{
						SetCurSel(m_Item.size());
						m_nCurColSel = m_nNumColumns;
					}
				}
			}
		}
		else if( m_bRenderVScroll && x >= m_ptVBar.x && x <= m_ptVBar.x+m_rcImage[AUILISTBOX_RECT_VBAR].Width() )
		{
			int nBarH = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height();
			// Click on up button.
			if( y < m_ptVBar.y )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUILISTBOX_STATE_PRESSUP;
					m_nScrollDelay = 0;
				}
			}
			// Click on down button.
			else if( y > m_ptVBar.y + nBarH)
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUILISTBOX_STATE_PRESSDOWN;
					m_nScrollDelay = 0;
				}
			}
		}
		else if( m_bRenderHScroll && y >= m_rcImage[AUILISTBOX_RECT_HBAR].top && y <= m_rcImage[AUILISTBOX_RECT_HBAR].bottom &&
				 x < m_ptVBar.x)
		{
			int nBarW = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height();

			// Click on left button.
			if( x < m_ptHBar.x || x < cx + m_rcImage[AUILISTBOX_RECT_UP].Height())
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUILISTBOX_STATE_PRESSLEFT;
					m_nScrollDelay = 0;
				}
			}
			// Click on right button.
			else if( x > m_ptHBar.x + nBarW || x > m_rcImage[AUILISTBOX_RECT_HBAR].right - m_rcImage[AUILISTBOX_RECT_DOWN].Height())
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUILISTBOX_STATE_PRESSRIGHT;
					m_nScrollDelay = 0;
				}
			}
		}

		return true;
	}
	else if( msg == WM_KEYDOWN )
	{
		int nVKey = int(wParam);

		if( m_bAutoWrap )
			nLinesPP = m_nLinesCurPage;

		if( VK_UP == nVKey )
		{
			if( m_nCurSel > 0 )
			{
				SetCurSel(m_nCurSel - 1);
				m_nCurColSel = m_nNumColumns;
				if( m_nCurSel < m_nFirstItem )
					SetFirstItem(m_nFirstItem - 1);
			}
			return true;
		}
		else if( VK_DOWN == nVKey )
		{
			if( m_nCurSel < int(m_Item.size()) - 1 )
			{
				SetCurSel(m_nCurSel + 1);
				m_nCurColSel = m_nNumColumns;
				if( m_nCurSel >= m_nFirstItem + nLinesPP )
					SetFirstItem(m_nFirstItem + 1);
			}
			return true;
		}
		if( m_bAutoWrap )
		{
			if( VK_PRIOR == nVKey )
			{
				int nLines = 0;
				while( m_nFirstItem > 0 )
				{
					SetFirstItem(m_nFirstItem - 1);
					nLines += m_Item[m_nFirstItem].nLines;
					if( nLines > nLinesPP )
					{
						SetFirstItem(m_nFirstItem + 1);
						break;
					}
				}

				return true;
			}
			else if( VK_NEXT == nVKey )
			{
				SetFirstItem(m_nFirstItem + m_nLinesCurPage);
				if( m_nFirstItem > m_nScrollLines )
					SetFirstItem(m_nScrollLines);

				return true;
			}
		}
		else
		{
			if( VK_PRIOR == nVKey )
			{
				m_nFirstItem -= nLinesPP;
				if( m_nFirstItem < 0 )
					SetFirstItem(0);
				return true;
			}
			else if( VK_NEXT == nVKey )
			{
				if( int(m_Item.size()) - m_nFirstItem >= 2 * nLinesPP )
					m_nFirstItem += nLinesPP;
				else if( int(m_Item.size()) - m_nFirstItem >= nLinesPP )
					SetFirstItem(m_Item.size() - nLinesPP);

				return true;
			}
		}
	}
	else if( WM_KILLFOCUS == msg )
	{
		m_bDraggingVScroll = false;
		m_bDraggingHScroll = false;
		m_nState = AUILISTBOX_STATE_NORMAL;

		if( m_pComboBox && !m_pComboBox->IsFocus() && (PAUIOBJECT)wParam != m_pComboBox )
		{
			Show(false);
			m_pComboBox->SetState(AUICOMBOBOX_STATE_NORMAL);
		}
	}
	else if( WM_MOUSEWHEEL == msg )
	{
		int zDelta = (short)HIWORD(wParam);

		if( zDelta > 0 && m_nFirstItem > 0 )
			SetFirstItem(m_nFirstItem - 1);
		else if( zDelta < 0 && m_nFirstItem < int(m_Item.size()) )
		{
			if( !m_bAutoWrap && m_nFirstItem + nLinesPP < int(m_Item.size()) ||
				m_bAutoWrap && m_nFirstItem < m_nScrollLines )
				SetFirstItem(m_nFirstItem + 1);
		}

		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIListBox::Tick(void)
{
	if( !m_pAUIFrame || !m_pA2DSprite[AUILISTBOX_RECT_VSCROLL] )
		return true;

	int nLinesPP = GetLinesPerPage();

	DWORD dwTick = GetTickCount();
	if( dwTick - m_nScrollDelay >= AUILISTBOX_SCROLL_DELAY )
	{
		bool bDelay = true;
		if( AUILISTBOX_STATE_PRESSUP == m_nState )
		{
			if( m_nFirstItem > 0 )
				SetFirstItem(m_nFirstItem - 1);
		}
		else if( AUILISTBOX_STATE_PRESSDOWN == m_nState )
		{
			if( m_bAutoWrap && m_nFirstItem < m_nScrollLines ||
				!m_bAutoWrap && m_nFirstItem + nLinesPP < int(m_Item.size()) )
				SetFirstItem(m_nFirstItem + 1);
		}
		else if (AUILISTBOX_STATE_PRESSLEFT == m_nState)
		{
			SetHBarPos(m_nHBarPos - 1);
		}
		else if (AUILISTBOX_STATE_PRESSRIGHT == m_nState)
		{
			SetHBarPos(m_nHBarPos + 1);
		}
		else
		{
			bDelay = false;
		}

		if (bDelay)
		{			
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
	}

	return AUIObject::Tick();
}

bool AUIListBox::Render(void)
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
		int nLines = m_Item.size();
		int cx = 0, cy = 0;
		SIZE sLimit = {0, 0};
		if( m_pAUIFrame )
		{
			sLimit = m_pAUIFrame->GetSizeLimit();
			cx = sLimit.cx / 2;
			cy = sLimit.cy / 2;
		}

		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		FitFont();

		A3DPOINT2 ptSize;
		float fTextH;
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
		if(m_nRowFixedHeight > 0)
		{
			fTextH = float(m_nRowFixedHeight) * fWindowScale;
		}
		else
		{
			ptSize = pFont->GetTextExtent(_AL("Wjg"));
			fTextH = ptSize.y + m_nLineSpace * fWindowScale;
		}

		int nLinesPP = GetLinesPerPage();

		if( m_pComboBox || m_pvecImageList )
		{
			bval = m_pA3DEngine->FlushInternalSpriteBuffer();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

			m_pParent->GetAUIManager()->GetA3DFTFontMan()->Flush();
		}

		// Only render when frames is OK.
		if( m_pAUIFrame )
		{
			int nWidth = (m_bRenderVScroll || m_bForceRenderScroll) ? m_nWidth - m_rcImage[AUILISTBOX_RECT_VSCROLL].Width() * fWindowScale : m_nWidth;
			int nHeight = (m_bRenderHScroll) ? m_nHeight -  m_rcImage[AUILISTBOX_RECT_VSCROLL].Width() * fWindowScale : m_nHeight;
			
			bval = m_pAUIFrame->Render(ptPos.x, ptPos.y, nWidth, nHeight, NULL, 0,
				_AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
		}
		else
			bval = AUIObject::Render();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call m_pAUIFrame->Render() or base AUIObject::Render()");

		A3DRECT rcText = m_rcImage[AUILISTBOX_RECT_FRAME];
		rcText.Offset(ptPos.x, ptPos.y);

		// Show list box items.
		A3DRECT rc;
		A3DCOLOR r, g, b;
		ACHAR szColor[20];
		int nRealLines = 0;
		int x, y;
		int nYLines;

		int nStart = m_nFirstItem;
		int nEnd = m_nFirstItem + nLinesPP - 1;
		if( nEnd >= int(m_Item.size()) )
		{
			nEnd = m_Item.size() - 1;
			if( !m_bAutoWrap )
			{
				SetFirstItem(nEnd - nLinesPP + 1);
				if( m_nFirstItem < 0 )
					SetFirstItem(0);
				nStart = m_nFirstItem;
			}
		}

		// Draw item picture
		if( m_Item.size() > 0 && m_pA2DSprite[AUILISTBOX_RECT_ITEM] )
		{
			float fScaleX = float(m_rcImage[AUILISTBOX_RECT_FRAME].Width()) / float(m_pA2DSprite[AUILISTBOX_RECT_ITEM]->GetWidth());
			float fScaleY = float(fTextH) / float(m_pA2DSprite[AUILISTBOX_RECT_ITEM]->GetHeight());			
			
			m_pA2DSprite[AUILISTBOX_RECT_ITEM]->SetScaleX(fScaleX);
			m_pA2DSprite[AUILISTBOX_RECT_ITEM]->SetScaleY(fScaleY);
			m_pA2DSprite[AUILISTBOX_RECT_ITEM]->SetAlpha(nAlpha);
			
			x = rcText.left;
			y = rcText.top;
			nYLines = 0;
			for( i = nStart; i <= nEnd; i++ )
			{
				if( i > nStart )
				{
					nYLines += m_Item[i - 1].nLines;
					y = rcText.top + fTextH * float(nYLines);
				}
				
				if( m_bAutoWrap )
				{
					float fScaleY = min(float(fTextH * m_Item[i].nLines),
						rcText.bottom - y) / float(m_pA2DSprite[AUILISTBOX_RECT_ITEM]->GetHeight());
					m_pA2DSprite[AUILISTBOX_RECT_ITEM]->SetScaleY(fScaleY);
				}
				else
				{
					if (rcText.bottom - y < fTextH)
					{
						m_pA2DSprite[AUILISTBOX_RECT_ITEM]->SetScaleY(float(rcText.bottom-y)/float(m_pA2DSprite[AUILISTBOX_RECT_ITEM]->GetHeight()));
					}
				}
					
				if( m_pParent->IsNoFlush() )
					bval = m_pA2DSprite[AUILISTBOX_RECT_ITEM]->DrawToBack(x, y);
				else
					bval = m_pA2DSprite[AUILISTBOX_RECT_ITEM]->DrawToInternalBuffer(x, y);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call pA2DSprite->DrawToBack() or pA2DSprite->DrawToInternalBuffer()");
			}
		}

		// Draw hilight bar.
		if( m_Item.size() > 0 && m_pA2DSprite[AUILISTBOX_RECT_HILIGHT] )
		{
			float fScaleX = float(m_rcImage[AUILISTBOX_RECT_FRAME].Width()) / float(m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->GetWidth());
			float fScaleY = float(fTextH) / float(m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->GetHeight());

			m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->SetScaleX(fScaleX);
			m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->SetScaleY(fScaleY);
			m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->SetAlpha(nAlpha);

			x = rcText.left;
			y = rcText.top;
			nYLines = 0;
			for( i = nStart; i <= nEnd; i++ )
			{
				if( i > nStart )
				{
					nYLines += m_Item[i - 1].nLines;
					y = rcText.top + fTextH * float(nYLines);
				}

				if( (m_bMultipleSelection && m_Item[i].bSelState)
					|| (!m_bMultipleSelection && i == m_nCurSel) )
				{
					if( m_bAutoWrap )
					{
						float fScaleY = min(float(fTextH * m_Item[i].nLines),
							rcText.bottom - y) / float(m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->GetHeight());
						m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->SetScaleY(fScaleY);
					}
					else
					{
						if (rcText.bottom - y < fTextH)
						{
							m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->SetScaleY(float(rcText.bottom-y)/float(m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->GetHeight()));
						}
					}

					if( m_pParent->IsNoFlush() )
						bval = m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->DrawToBack(x, y);
					else
						bval = m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->DrawToInternalBuffer(x, y);
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call pA2DSprite->DrawToBack() or pA2DSprite->DrawToInternalBuffer()");
				}
			}
		}

		// Clear Rect info
		ClearItemRcInfo();
		TEXTITEM_RCINFO rcInfo = {&m_vNameLinks, &m_vEBItems};

		m_nLinesCurPage = 0;
		x = rcText.left;
		y = rcText.top;
		nYLines = 0;

		for( i = nStart; i <= nEnd; i++ )
		{
			x = rcText.left;
			if( i > nStart )
			{
				nYLines += m_Item[i - 1].nLines;
				y = rcText.top + fTextH * float(nYLines);
			}

			m_nLinesCurPage++;
			if( m_nNumColumns > 1 )
			{
				CSplit s(m_Item[i].strText);
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				int nNumCols = min(m_nNumColumns, int(vec.size()));

				float px = x;
				float py = y;
				for( int j = 0; j < nNumCols; j++ )
				{
					if (m_bRenderHScroll)
					{
						int l = max(rcText.left, x - m_nHBarPos);
						int r = max(rcText.left, l + m_nColWidth[j] * fWindowScale);
						rc.SetRect(l, y, r, min(y + fTextH * m_Item[i].nLines, rcText.bottom));
					}
					else
					{
						rc.SetRect(x, y, x + m_nColWidth[j] * fWindowScale, min(y + fTextH * m_Item[i].nLines, rcText.bottom));
					}

					if( m_nColAlign[j] != AUIFRAME_ALIGN_LEFT || m_nAlign != AUIFRAME_ALIGN_TOP )
					{
						int nWidth, nHeight, nLines;

						if( m_pvecImageList && (*m_pvecImageList)[0] )
							AUI_GetTextRect(pFont, vec[j], nWidth, nHeight, nLines, 0, &m_Item[i].itemsSet, 0, 0, m_pvecImageList, fWindowScale);
						else
							AUI_GetTextRect(pFont, vec[j], nWidth, nHeight, nLines, 0);
						if( m_nColAlign[j] == AUIFRAME_ALIGN_CENTER )
							x = x + rc.Width() / 2 - nWidth / 2;
						else if( m_nColAlign[j] == AUIFRAME_ALIGN_RIGHT )
							x = x + rc.Width() - nWidth;

						if( m_nAlign == AUIFRAME_ALIGN_CENTER )
							y = y + rc.Height() / 2 - nHeight / 2;
						else if( m_nAlign == AUIFRAME_ALIGN_BOTTOM )
							y = y + rc.Height() - nHeight;
					}

					r = A3DCOLOR_GETRED(m_Item[i].color[j]);
					g = A3DCOLOR_GETGREEN(m_Item[i].color[j]);
					b = A3DCOLOR_GETBLUE(m_Item[i].color[j]);
					a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);

					ACString str = szColor + vec[j];
					TAG_OP_IN_PARAMS InParams(pFont, x, y,
						str, &rc, 0, m_nLineSpace * fWindowScale, m_bAutoIndent, NULL, NULL, true, m_nShadow, nAlpha, true, &m_Item[i].itemsSet, m_pvecImageList, fWindowScale,
						true, NULL,0, A3DCOLORRGB(255,255,255), m_bItemTextSmallImage);
					if (m_bRenderHScroll)
						InParams._x = x - m_nHBarPos;
					TAG_OP_OUT_PARAMS outParams(NULL, NULL, NULL, &rcInfo);
					bval = AUI_TextOutFormat(InParams, outParams);
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call AUI_TextOutFormat()");

					px += m_nColWidth[j] * fWindowScale;
					x = (int)px; 
					y = (int)py; 
				}
			}
			else
			{
				if( m_bAutoWrap )
				{
					rc.SetRect(x, y, rcText.right, rcText.bottom);

					r = A3DCOLOR_GETRED(m_Item[i].color[0]);
					g = A3DCOLOR_GETGREEN(m_Item[i].color[0]);
					b = A3DCOLOR_GETBLUE(m_Item[i].color[0]);
					a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);

					ACString str = szColor + m_Item[i].strText;
					TAG_OP_IN_PARAMS InParams(pFont, x- m_nHBarPos, y,
						str, &rc, 0, m_nLineSpace * fWindowScale, m_bAutoIndent, NULL, NULL, true, m_nShadow, nAlpha, true, &m_Item[i].itemsSet, m_pvecImageList, fWindowScale,
						true, NULL,0, A3DCOLORRGB(255,255,255), m_bItemTextSmallImage);

					TAG_OP_OUT_PARAMS outParams(&m_Item[i].nLines, NULL, NULL, &rcInfo);
					bval = AUI_TextOutFormat(InParams, outParams);
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call AUI_TextOutFormat()");
				}
				else
				{
					rc.SetRect(x, y, rcText.right, y + fTextH);

					r = A3DCOLOR_GETRED(m_Item[i].color[0]);
					g = A3DCOLOR_GETGREEN(m_Item[i].color[0]);
					b = A3DCOLOR_GETBLUE(m_Item[i].color[0]);
					a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);

					ACString str = szColor + m_Item[i].strText;
					TAG_OP_IN_PARAMS InParams(pFont, x- m_nHBarPos, y,
						str, &rc, 0, m_nLineSpace * fWindowScale, m_bAutoIndent, NULL, NULL, true, m_nShadow, nAlpha, true, &m_Item[i].itemsSet, m_pvecImageList, fWindowScale,
						true, NULL,0, A3DCOLORRGB(255,255,255), m_bItemTextSmallImage);

					TAG_OP_OUT_PARAMS outParams(NULL, NULL, NULL, &rcInfo);
					bval = AUI_TextOutFormat(InParams, outParams);

					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call AUI_TextOutFormat()");
				}
			}
			nRealLines += m_Item[i].nLines;
			if( nRealLines > nLinesPP )
				break;
		}

		if( m_bRenderVScroll )
			bval = RenderVScroll();
		if( m_bRenderHScroll )
			bval = RenderHScroll();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::Render(), failed to call RenderScroll()");
	}

	return true;
}

bool AUIListBox::RenderVScroll(void)
{
	if( !m_pA2DSprite[AUILISTBOX_RECT_UP] ||
		!m_pA2DSprite[AUILISTBOX_RECT_DOWN] ||
		!m_pA2DSprite[AUILISTBOX_RECT_VSCROLL] )
	{
		return true;
	}

	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	bool bval = true;
	POINT ptPos = GetPos();
	int nLines = m_Item.size();
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	int nLinesPP = GetLinesPerPage();

	// Draw up button.
	int nItem = (AUILISTBOX_STATE_PRESSUP == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUILISTBOX_RECT_UP]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderScroll(), failed to m_pA2DSprite[AUILISTBOX_RECT_UP]->SetCurrentItem()");

	int W = m_rcImage[AUILISTBOX_RECT_VSCROLL].Width();
	int x = ptPos.x + m_nWidth - W;
	int y = ptPos.y;
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetAlpha(nAlpha);
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetScaleY(fWindowScale);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToInternalBuffer()");

	// Draw scroll area.
	int H = m_rcImage[AUILISTBOX_RECT_UP].Height();
	y += H;

	float fScale = float(m_nHeight - m_rcImage[AUILISTBOX_RECT_UP].Height() - m_rcImage[AUILISTBOX_RECT_DOWN].Height())
		/ m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->GetItem(0)->GetRect().Height();
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetScaleY(fScale);
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUILISTBOX_STATE_PRESSDOWN == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetCurrentItem()");

	y = ptPos.y + m_nHeight - m_rcImage[AUILISTBOX_RECT_DOWN].Height();

	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToInternalBuffer()");

	// Draw scrollbar button.
	if( m_pA2DSprite[AUILISTBOX_RECT_VBAR] )
	{
		int nBarH = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height() * fWindowScale;
		H = m_nHeight - m_rcImage[AUILISTBOX_RECT_UP].Height() - m_rcImage[AUILISTBOX_RECT_DOWN].Height() - nBarH;
		if( H > 0 )
		{
			m_ptVBar.x = m_nWidth - m_rcImage[AUILISTBOX_RECT_VSCROLL].Width();
			if( m_bAutoWrap )
				m_ptVBar.y = m_rcImage[AUILISTBOX_RECT_UP].Height() + m_nFirstItem * H / m_nScrollLines;
			else
				m_ptVBar.y = m_rcImage[AUILISTBOX_RECT_UP].Height() + m_nFirstItem * H / (nLines - nLinesPP);

			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetAlpha(nAlpha);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToBack(
				ptPos.x + m_ptVBar.x, ptPos.y + m_ptVBar.y);
			else
				bval = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToInternalBuffer(
				ptPos.x + m_ptVBar.x, ptPos.y + m_ptVBar.y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToInternalBuffer()");
		}
	}

	return true;
}

bool AUIListBox::RenderHScroll()
{
	if( !m_pA2DSprite[AUILISTBOX_RECT_UP] ||
		!m_pA2DSprite[AUILISTBOX_RECT_DOWN] ||
		!m_pA2DSprite[AUILISTBOX_RECT_VSCROLL] )
	{
		return true;
	}

	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	bool bval = true;
	POINT ptPos = GetPos();
	int nLines = m_Item.size();
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	int nLinesPP = GetLinesPerPage();

	// Draw left button.
	int nItem = (AUILISTBOX_STATE_PRESSLEFT == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUILISTBOX_RECT_UP]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderHScroll(), failed to m_pA2DSprite[AUILISTBOX_RECT_UP]->SetCurrentItem()");

	int W = m_rcImage[AUILISTBOX_RECT_VSCROLL].Width();
	int x = ptPos.x;
	int y = ptPos.y + m_nHeight;
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetAlpha(nAlpha);
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetDegree(270);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToInternalBuffer(x, y);
	m_pA2DSprite[AUILISTBOX_RECT_UP]->SetDegree(0);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderHScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_UP]->DrawToInternalBuffer()");

	// Draw scroll area.
	x = ptPos.x + m_rcImage[AUILISTBOX_RECT_UP].Height();

	float fScale = float(m_rcImage[AUILISTBOX_RECT_HBAR].Width())
		/ m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->GetItem(0)->GetRect().Height();
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetScaleY(fScale);
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetAlpha(nAlpha);
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetDegree(270);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToInternalBuffer(x, y);
	m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->SetDegree(0);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderHScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->DrawToInternalBuffer()");

	// Draw right button.
	nItem = (AUILISTBOX_STATE_PRESSRIGHT == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderHScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetCurrentItem()");

	if (m_bRenderVScroll)
		x = ptPos.x + m_nWidth - 2*m_rcImage[AUILISTBOX_RECT_DOWN].Height();
	else
		x = ptPos.x + m_nWidth - m_rcImage[AUILISTBOX_RECT_DOWN].Height();

	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetAlpha(nAlpha);
	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetDegree(270);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToInternalBuffer(x, y);
	m_pA2DSprite[AUILISTBOX_RECT_DOWN]->SetDegree(0);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderHScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_DOWN]->DrawToInternalBuffer()");

	// Draw scrollbar button.
	if( m_pA2DSprite[AUILISTBOX_RECT_VBAR] )
	{
		int nBarH = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height() * fWindowScale;
		W = m_rcImage[AUILISTBOX_RECT_HBAR].Width() - nBarH;
		if( W > 0 )
		{
			int L = m_nHBarMax;
			m_ptHBar.x = m_rcImage[AUILISTBOX_RECT_UP].Height() + m_nHBarPos * W / L;
			m_ptHBar.y = m_nHeight;
			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetAlpha(nAlpha);
			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetDegree(270);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToBack(
					ptPos.x + m_ptHBar.x, ptPos.y + m_ptHBar.y);
			else
				bval = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToInternalBuffer(
					ptPos.x + m_ptHBar.x, ptPos.y + m_ptHBar.y);
			m_pA2DSprite[AUILISTBOX_RECT_VBAR]->SetDegree(0);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::RenderHScroll(), failed to call m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToBack() or m_pA2DSprite[AUILISTBOX_RECT_VBAR]->DrawToInternalBuffer()");
		}
	}

	return true;
}

int AUIListBox::AddString(const ACHAR *pszString)
{
	UpdateRenderTarget();

	AUILISTBOX_ITEM Item;
	
	Item.strText = UnmarshalEditBoxText(pszString, Item.itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	Item.nLines = 0;
	Item.bSelState = false;
	for( int i = 0; i < AUILISTBOX_MAX_COLUMN; i++ )
		Item.color[i] = m_color;
	memset(Item.dwData, 0, sizeof(DWORD) * AUILISTBOX_DATA_NUM);
	memset(Item.pvData, 0, sizeof(void *) * AUILISTBOX_DATA_NUM);
	memset(Item.uiData64, 0, sizeof(UINT64) * AUILISTBOX_DATA_NUM);
	memset(Item.uiDataPtr64, 0, sizeof(UINT64) * AUILISTBOX_DATA_NUM);

	m_Item.push_back(Item);
	UpdateScroll(m_Item.size() - 1);
	
	return m_Item.size();
}

int AUIListBox::InsertString(int nIndex, const ACHAR *pszString)
{
	if( nIndex < 0 || nIndex > int(m_Item.size()) )
		return AUILISTBOX_ERROR;

	UpdateRenderTarget();

	AUILISTBOX_ITEM Item;

	Item.strText = UnmarshalEditBoxText(pszString, Item.itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	Item.nLines = 0;
	Item.bSelState = false;
	for( int i = 0; i < AUILISTBOX_MAX_COLUMN; i++ )
		Item.color[i] = m_color;
	memset(Item.dwData, 0, sizeof(DWORD) * AUILISTBOX_DATA_NUM);
	memset(Item.pvData, 0, sizeof(void *) * AUILISTBOX_DATA_NUM);
	memset(Item.uiData64, 0, sizeof(UINT64) * AUILISTBOX_DATA_NUM);
	memset(Item.uiDataPtr64, 0, sizeof(UINT64) * AUILISTBOX_DATA_NUM);
	
	m_Item.insert(m_Item.begin() + nIndex, Item);
	UpdateScroll(nIndex);

	return m_Item.size();
}

int AUIListBox::DeleteString(int nIndex)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;

	UpdateRenderTarget();

	if(	m_bMultipleSelection )
		SetSel(nIndex, false);
	if( m_nCurSel > nIndex )
		SetCurSel(m_nCurSel - 1);

	m_Item.erase(m_Item.begin() + nIndex);
	if( m_nCurSel >= m_Item.size() )
		SetCurSel(m_Item.size() - 1);
	if( m_nCurSel < 0 )
		SetCurSel(0);

	UpdateScroll();

	return m_Item.size();
}

int AUIListBox::GetNumColumns(void)
{
	return m_nNumColumns;
}

int AUIListBox::GetCount(void)
{
	return m_Item.size();
}

int AUIListBox::GetCurSel(void)
{
	return m_nCurSel;
}

int AUIListBox::GetCurColSel(void)
{
	return m_nCurColSel;
}

void AUIListBox::SetCurSel(int nIndex)
{
	if( nIndex != m_nCurSel )
		UpdateRenderTarget();

	m_nCurSel = nIndex;
}

const ACHAR * AUIListBox::GetText(int nIndex)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return _AL("");
	else
		return m_Item[nIndex].strText;
}

int AUIListBox::SetText(int nIndex, const ACHAR *pszText)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;

	UpdateRenderTarget();

	m_Item[nIndex].itemsSet.Release();
	m_Item[nIndex].strText = UnmarshalEditBoxText(pszText, m_Item[nIndex].itemsSet, 0, L"", 0xffffffff, m_nItemMask);
	
	return nIndex;
}

A3DCOLOR AUIListBox::GetItemTextColor(int nIndex, int nItem)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return A3DCOLORRGB(0, 0, 0);
	else
		return m_Item[nIndex].color[nItem];
}

int AUIListBox::SetItemTextColor(int nIndex, A3DCOLOR color, int nItem)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;

	UpdateRenderTarget();

	if( nItem >= 0)
		m_Item[nIndex].color[nItem] = color;
	else
		for( int i = 0; i < max(1, m_nNumColumns); i++ )
			m_Item[nIndex].color[i] = color;
	return nIndex;
}

void AUIListBox::ResetContent(void)
{
	UpdateRenderTarget();

	m_Item.clear();
	m_nCurSel = 0;
	m_nFirstItem = 0;
	m_nLinesCurPage = 0;
	m_nNumSelections = 0;
	m_nScrollLines = 0;
	UpdateScroll();
}

int AUIListBox::GetSel(int nIndex)
{
	return m_Item[nIndex].bSelState ? 1 : 0;
}

void AUIListBox::BatchSetSel(int nStart, int nEnd, bool bSel)
{
	ASSERT(nStart <= nEnd && nStart >= 0 && nEnd < int(m_Item.size()));

	UpdateRenderTarget();
	for( int i = nStart; i <= nEnd; i++ )
	{
		if (bSel && !m_Item[i].bSelState)
			m_nNumSelections++;
		else if (!bSel && m_Item[i].bSelState)
			m_nNumSelections--;

		m_Item[i].bSelState = bSel;

	}
}

bool AUIListBox::IsMultipleSelection(void)
{
	return m_bMultipleSelection;
}

int AUIListBox::GetSelCount(void)
{
	return m_nNumSelections;
}

int AUIListBox::GetSelItems(int nMaxItems, int *a_nIndex)
{
	ASSERT(a_nIndex);
	int nRealNum = 0;
	for( int i = 0; i < int(m_Item.size()); i++ )
	{
		if( m_Item[i].bSelState )
		{
			a_nIndex[nRealNum] = i;
			nRealNum++;
			if( nRealNum >= nMaxItems )
				break;
		}
	}
	return nRealNum;
}

int AUIListBox::GetHitArea(int x, int y)
{
	if( !m_pAUIFrame ||
		!m_pA2DSprite[AUILISTBOX_RECT_UP] ||
		!m_pA2DSprite[AUILISTBOX_RECT_DOWN] ||
		!m_pA2DSprite[AUILISTBOX_RECT_VSCROLL] )
	{
		return AUILISTBOX_ERROR;
	}

	int nLines = m_Item.size();
	int cx = 0, cy = 0;
	SIZE sLimit;
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	cx = sLimit.cx / 2;
	cy = sLimit.cy / 2;

	FitFont();

	int nLinesPP = GetLinesPerPage();
	// Click on text item area.
	if( m_rcImage[AUILISTBOX_RECT_FRAME].PtInRect(x, y) )
		return AUILISTBOX_RECT_TEXT;
	// Click on scroll bar.
	else if( m_rcImage[AUILISTBOX_RECT_VBAR].PtInRect(x, y) )
	{
		int nBarH = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height();
		// Page up and page down.
		if( y < m_ptVBar.y || y > m_ptVBar.y + nBarH || nLines > nLinesPP )
			return AUILISTBOX_RECT_VSCROLL;
		// Scroll bar.
		else
			return AUILISTBOX_RECT_VBAR;
	}
	// Click on up button.
	else if( y < m_rcImage[AUILISTBOX_RECT_VBAR].top )
		return AUILISTBOX_RECT_UP;
	// Click on down button.
	else if( y > m_rcImage[AUILISTBOX_RECT_VBAR].bottom )
		return AUILISTBOX_RECT_DOWN;

	// click on HScroll
	else if( m_rcImage[AUILISTBOX_RECT_HBAR].PtInRect(x, y) )
	{
		int nBarH = m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetItem(0)->GetRect().Height();
		// Page up and page down.
		if( x < m_ptVBar.x || x > m_ptVBar.x + nBarH || nLines > nLinesPP )
			return AUILISTBOX_RECT_HSCROLL;
		// Scroll bar.
		else
			return AUILISTBOX_RECT_HBAR;
	}
	// Click on left button.
	else if( x < m_rcImage[AUILISTBOX_RECT_HBAR].left )
		return AUILISTBOX_RECT_LEFT;
	// Click on right button.
	else if( x > m_rcImage[AUILISTBOX_RECT_HBAR].right )
		return AUILISTBOX_RECT_RIGHT;

	return AUILISTBOX_ERROR;
}

void * AUIListBox::GetItemDataPtr(int nIndex, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return NULL;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return NULL;
	
	if (0 != m_Item[nIndex].pvData[nSubIndex] && strName != m_Item[nIndex].strDataPtrName[nSubIndex])
		AUI_ReportError(__LINE__, 1, "AUIListBox::GetItemDataPtr(), data name not match");
	return m_Item[nIndex].pvData[nSubIndex];
}

int AUIListBox::SetItemDataPtr(int nIndex, void *pData, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return AUILISTBOX_ERROR;

	m_Item[nIndex].strDataPtrName[nSubIndex] = strName;
	m_Item[nIndex].pvData[nSubIndex] = pData;
	return nIndex;
}

DWORD AUIListBox::GetItemData(int nIndex, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return AUILISTBOX_ERROR;

	if (0 != m_Item[nIndex].dwData[nSubIndex] && strName != m_Item[nIndex].strDataName[nSubIndex])
		AUI_ReportError(__LINE__, 1, "AUIListBox::GetItemData(), data name not match");
	return m_Item[nIndex].dwData[nSubIndex];
}

int AUIListBox::SetItemData(int nIndex, DWORD dwItemData, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return AUILISTBOX_ERROR;
	
	m_Item[nIndex].strDataName[nSubIndex] = strName;
	m_Item[nIndex].dwData[nSubIndex] = dwItemData;
	return nIndex;
}

UINT64 AUIListBox::GetItemDataPtr64(int nIndex, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return NULL;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return NULL;
	
	if (0 != m_Item[nIndex].uiDataPtr64[nSubIndex] && strName != m_Item[nIndex].strDataPtr64Name[nSubIndex])
		AUI_ReportError(__LINE__, 1, "AUIListBox::GetItemDataPtr64(), data name not match");
	return m_Item[nIndex].uiDataPtr64[nSubIndex];
}

int AUIListBox::SetItemDataPtr64(int nIndex, UINT64 uiDataPtr64, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return AUILISTBOX_ERROR;

	m_Item[nIndex].strDataPtr64Name[nSubIndex] = strName;
	m_Item[nIndex].uiDataPtr64[nSubIndex] = uiDataPtr64;
	return nIndex;
}

UINT64 AUIListBox::GetItemData64(int nIndex, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return AUILISTBOX_ERROR;

	if (0 != m_Item[nIndex].uiData64[nSubIndex] && strName != m_Item[nIndex].strData64Name[nSubIndex])
		AUI_ReportError(__LINE__, 1, "AUIListBox::GetItemData64(), data name not match");
	return m_Item[nIndex].uiData64[nSubIndex];
}

int AUIListBox::SetItemData64(int nIndex, UINT64 uiData64, int nSubIndex, AString strName)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;
	if( nSubIndex < 0 || nSubIndex >= AUILISTBOX_DATA_NUM )
		return AUILISTBOX_ERROR;
	
	m_Item[nIndex].strData64Name[nSubIndex] = strName;
	m_Item[nIndex].uiData64[nSubIndex] = uiData64;
	return nIndex;
}

bool AUIListBox::BringItemToTop(int nItem)
{
	if( nItem < 0 || nItem >= GetCount() )
		return false;
	AUILISTBOX_ITEM Item = m_Item[nItem];
	int i;
	for( i = nItem; i > 0 ; i-- )
		m_Item[i] = m_Item[i - 1];
	m_Item[0] = Item;
	return true;
}

bool AUIListBox::SortItemsByData(int nMode, int nSortType, int nColumn)
{
	if( nColumn >= AUILISTBOX_DATA_NUM )
		return false;

	UpdateRenderTarget();

	bool bSwapped;
	AUILISTBOX_ITEM Item;
	switch(nSortType)
	{
	case AUILISTBOX_DATASORTTYPE_DWORD:
	{
		do
		{
			bSwapped = false;
			for( int i = 0; i < int(m_Item.size()) - 1; i++ )
			{
				if( (AUILISTBOX_SORT_ASCENT == nMode &&
					GetItemData(i, nColumn) > GetItemData(i + 1, nColumn)) ||
					(AUILISTBOX_SORT_DESCENT == nMode &&
					GetItemData(i, nColumn) < GetItemData(i + 1, nColumn)) )
				{
					Item = m_Item[i];
					m_Item[i] = m_Item[i + 1];
					m_Item[i + 1] = Item;
					bSwapped = true;
				}
			}
		} while( bSwapped );
		break;
	}
	case AUILISTBOX_DATASORTTYPE_INT:
	{
		do
		{
			bSwapped = false;
			for( int i = 0; i < int(m_Item.size()) - 1; i++ )
			{
				if( (AUILISTBOX_SORT_ASCENT == nMode &&
					(int)GetItemData(i, nColumn) > (int)GetItemData(i + 1, nColumn)) ||
					(AUILISTBOX_SORT_DESCENT == nMode &&
					(int)GetItemData(i, nColumn) < (int)GetItemData(i + 1, nColumn)) )
				{
					Item = m_Item[i];
					m_Item[i] = m_Item[i + 1];
					m_Item[i + 1] = Item;
					bSwapped = true;
				}
			}
		} while( bSwapped );
		break;
	}
	}

	return true;
}

bool AUIListBox::SortItems(int nMode, int nSortType, int nColumn)
{
	UpdateRenderTarget();

	bool bSwapped;
	AUILISTBOX_ITEM Item;

	switch(nSortType)
	{
	case AUILISTBOX_SORTTYPE_UNISTRING:
	{
		ACString * keyword = new ACString[m_Item.size()];
		ACString temp;
		for( int i = 0; i < int(m_Item.size()); i++ )
		{
			CSplit s(m_Item[i].strText);
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			if( nColumn < vec.size() )
				keyword[i] = vec[nColumn];
			else
				keyword[i] = _AL("");
		}
		do
		{
			bSwapped = false;
			for( int i = 0; i < int(m_Item.size()) - 1; i++ )
			{
				if( (AUILISTBOX_SORT_ASCENT == nMode &&
					a_stricmp(keyword[i], keyword[i + 1]) > 0) ||
					(AUILISTBOX_SORT_DESCENT == nMode &&
					a_stricmp(keyword[i], keyword[i + 1]) < 0) )
				{
					Item = m_Item[i];
					m_Item[i] = m_Item[i + 1];
					m_Item[i + 1] = Item;
					temp = keyword[i];
					keyword[i] = keyword[i + 1];
					keyword[i + 1] = temp;
					bSwapped = true;
				}
			}
		} while( bSwapped );
		delete []keyword;
		break;
	}
	case AUILISTBOX_SORTTYPE_STRING:
	{
		AString * keyword = new AString[m_Item.size()];
		AString temp;
		for( int i = 0; i < int(m_Item.size()); i++ )
		{
			CSplit s(m_Item[i].strText);
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			if( nColumn < vec.size() )
				keyword[i] = AC2AS(vec[nColumn]);
			else
				keyword[i] = "";
		}
		do
		{
			bSwapped = false;
			for( int i = 0; i < int(m_Item.size()) - 1; i++ )
			{
				if( (AUILISTBOX_SORT_ASCENT == nMode &&
					stricmp(keyword[i], keyword[i + 1]) > 0) ||
					(AUILISTBOX_SORT_DESCENT == nMode &&
					stricmp(keyword[i], keyword[i + 1]) < 0) )
				{
					Item = m_Item[i];
					m_Item[i] = m_Item[i + 1];
					m_Item[i + 1] = Item;
					temp = keyword[i];
					keyword[i] = keyword[i + 1];
					keyword[i + 1] = temp;
					bSwapped = true;
				}
			}
		} while( bSwapped );
		delete []keyword;
		break;
	}
	case AUILISTBOX_SORTTYPE_INT:
	{
		int * keyword = new int[m_Item.size()];
		int temp;
		for( int i = 0; i < int(m_Item.size()); i++ )
		{
			CSplit s(m_Item[i].strText);
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			ACString strNum;
			if( nColumn < vec.size() )
				strNum = vec[nColumn];
			else
				strNum = _AL("");
			if( strNum.GetLength() > 0 && isdigit(strNum[0]) ||
				strNum.GetLength() > 1 && (strNum[0] == _AL('+') || strNum[0] == _AL('-')
				&& isdigit(strNum[1])) )
				keyword[i] = strNum.ToInt();
			else
				keyword[i] = INT_MAX;
		}
		do
		{
			bSwapped = false;
			for( int i = 0; i < int(m_Item.size()) - 1; i++ )
			{
				if( (AUILISTBOX_SORT_ASCENT == nMode &&
					keyword[i] > keyword[i + 1]) ||
					(AUILISTBOX_SORT_DESCENT == nMode &&
					keyword[i] < keyword[i + 1]) )
				{
					Item = m_Item[i];
					m_Item[i] = m_Item[i + 1];
					m_Item[i + 1] = Item;
					temp = keyword[i];
					keyword[i] = keyword[i + 1];
					keyword[i + 1] = temp;
					bSwapped = true;
				}
			}
		} while( bSwapped );
		delete []keyword;
		break;
	}
	case AUILISTBOX_SORTTYPE_FLOAT:
	{
		float * keyword = new float[m_Item.size()];
		float temp;
		for( int i = 0; i < int(m_Item.size()); i++ )
		{
			CSplit s(m_Item[i].strText);
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			ACString strNum;
			if( nColumn < vec.size() )
				strNum = vec[nColumn];
			else
				strNum = _AL("");
			if( strNum.GetLength() > 0 && (isdigit(strNum[0]) || strNum[0] == _AL('.')) ||
				strNum.GetLength() > 1 && (strNum[0] == _AL('+') || strNum[0] == _AL('-')
				&& (isdigit(strNum[1])) || strNum[1] == _AL('.')) )
				keyword[i] = strNum.ToFloat();
			else
				keyword[i] = FLT_MAX;
		}
		do
		{
			bSwapped = false;
			for( int i = 0; i < int(m_Item.size()) - 1; i++ )
			{
				if( (AUILISTBOX_SORT_ASCENT == nMode &&
					keyword[i] > keyword[i + 1]) ||
					(AUILISTBOX_SORT_DESCENT == nMode &&
					keyword[i] < keyword[i + 1]) )
				{
					Item = m_Item[i];
					m_Item[i] = m_Item[i + 1];
					m_Item[i + 1] = Item;
					temp = keyword[i];
					keyword[i] = keyword[i + 1];
					keyword[i + 1] = temp;
					bSwapped = true;
				}
			}
		} while( bSwapped );
		delete []keyword;
		break;
	}

	}

	return true;
}

bool AUIListBox::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Row Fixed Height") )
		Property->i = m_nRowFixedHeight;
	if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pAUIFrame )
			strncpy(Property->fn, m_pAUIFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Hilight Image") )
	{
		if( m_pA2DSprite[AUILISTBOX_RECT_HILIGHT] )
			strncpy(Property->fn, m_pA2DSprite[AUILISTBOX_RECT_HILIGHT]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
	{
		if( m_pA2DSprite[AUILISTBOX_RECT_UP] )
			strncpy(Property->fn, m_pA2DSprite[AUILISTBOX_RECT_UP]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
	{
		if( m_pA2DSprite[AUILISTBOX_RECT_DOWN] )
			strncpy(Property->fn, m_pA2DSprite[AUILISTBOX_RECT_DOWN]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
	{
		if( m_pA2DSprite[AUILISTBOX_RECT_VSCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUILISTBOX_RECT_VSCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
	{
		if( m_pA2DSprite[AUILISTBOX_RECT_VBAR] )
			strncpy(Property->fn, m_pA2DSprite[AUILISTBOX_RECT_VBAR]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Item Image") )
	{
		if( m_pA2DSprite[AUILISTBOX_RECT_ITEM] )
			strncpy(Property->fn, m_pA2DSprite[AUILISTBOX_RECT_ITEM]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Auto Wrap") )
		Property->b = m_bAutoWrap;
	else if( 0 == strcmpi(pszPropertyName, "Horizontal Scroll") )
		Property->b = m_bEnableHScroll;
	else if( 0 == strcmpi(pszPropertyName, "Hover Hilight") )
		Property->b = m_bHOver;
	else if( 0 == strcmpi(pszPropertyName, "Multiple Selection") )
		Property->b = m_bMultipleSelection;
	else if( 0 == strcmpi(pszPropertyName, "Multiple Column Info") )
		strncpy(Property->c, m_strCols, AFILE_LINEMAXLEN - 1);
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIListBox::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
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
	else if( 0 == strcmpi(pszPropertyName, "Row Fixed Height") )
	{
		m_nRowFixedHeight = Property->i;
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUILISTBOX_RECT_FRAME);
	else if( 0 == strcmpi(pszPropertyName, "Hilight Image") )
		return InitIndirect(Property->fn, AUILISTBOX_RECT_HILIGHT);
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
		return InitIndirect(Property->fn, AUILISTBOX_RECT_UP);
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
		return InitIndirect(Property->fn, AUILISTBOX_RECT_DOWN);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
		return InitIndirect(Property->fn, AUILISTBOX_RECT_VSCROLL);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
		return InitIndirect(Property->fn, AUILISTBOX_RECT_VBAR);
	else if( 0 == strcmpi(pszPropertyName, "Item Image") )
		return InitIndirect(Property->fn, AUILISTBOX_RECT_ITEM);
	else if( 0 == strcmpi(pszPropertyName, "Auto Wrap") )
		SetAutoWrap(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Horizontal Scroll") )
		EnableHScroll(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Hover Hilight") )
		SetHOver(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Multiple Selection") )
		m_bMultipleSelection = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Multiple Column Info") )
	{
		m_strCols = Property->c;
		bool bval = BuildColumnsInfo(m_strCols);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIListBox::SetProperty(), failed to call BuildColumnsInfo()");
	}
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIListBox::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Hilight Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Up Button Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Down Button Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Scroll Area Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Scroll Bar Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Multiple Selection", AUIOBJECT_PROPERTY_VALUE_BOOL) &&
			SetAUIObjectPropertyType("Multiple Column Info", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Item Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Row Fixed Height", AUIOBJECT_PROPERTY_VALUE_INT);
}

void AUIListBox::SetColor(A3DCOLOR color)
{
	UpdateRenderTarget();

	for( int i = 0; i < (int)m_Item.size(); i++ )
		SetItemTextColor(i, color);

	AUIObject::SetColor(color);
}

void AUIListBox::SetHOver(bool bHOver)
{
	m_bHOver = bHOver;
}

const ACHAR * AUIListBox::GetItemHint(int nIndex)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return _AL("");
	else
		return m_Item[nIndex].strHint;
}

const ACHAR * AUIListBox::GetItemColHint(int nIndex, int nColumn)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) || nColumn < 0 || nColumn >= m_nNumColumns )
		return _AL("");
	else
		return m_Item[nIndex].strColHint[nColumn];
}

int AUIListBox::SetItemHint(int nIndex, const ACHAR *pszHint)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) )
		return AUILISTBOX_ERROR;
	
	m_Item[nIndex].strHint = pszHint;

	return nIndex;
}

bool AUIListBox::SetItemColHint(int nIndex, int nColumn, const ACHAR *pszHint)
{
	if( nIndex < 0 || nIndex >= int(m_Item.size()) || nColumn < 0 || nColumn >= m_nNumColumns )
		return false;
	
	m_Item[nIndex].strColHint[nColumn] = pszHint;

	return true;
}

void AUIListBox::SetForceRenderScroll(bool bForce)
{
	if( bForce != m_bForceRenderScroll )
		UpdateRenderTarget();

	m_bForceRenderScroll = bForce;
}

void AUIListBox::SetAutoWrap(bool bAuto)
{
	if( bAuto != m_bAutoWrap )
	{
		m_bAutoWrap = bAuto;
		UpdateScroll();
	}
}

void AUIListBox::EnableHScroll(bool bEnable/* =true */)
{
	if (m_bEnableHScroll != bEnable)
	{
		m_bEnableHScroll = bEnable;
		UpdateScroll();
	}
}

int AUIListBox::GetHOverItem()
{
	return m_nHOverItem;
}

int AUIListBox::GetHOverItemCol()
{
	return m_nHOverItemCol;
}

void AUIListBox::SetSel(int nIndex, bool bSel)
{
	ASSERT(nIndex >= 0 && nIndex < int(m_Item.size()));

	UpdateRenderTarget();

	if (IsMultipleSelection())
	{
		if (bSel && !m_Item[nIndex].bSelState)
			++m_nNumSelections;
		else if (!bSel && m_Item[nIndex].bSelState)
			--m_nNumSelections;
	}
	m_Item[nIndex].bSelState = bSel;
}

void AUIListBox::SetComboBox(AUIComboBox *pComboBox)
{
	if(pComboBox == m_pComboBox) return;
	m_pComboBox = pComboBox;
	if (pComboBox)
		pComboBox->SetListBox(this);
}

void AUIListBox::SetColAlign(int nColumn, int nAlign)
{
	UpdateRenderTarget();

	if( m_bMultipleSelection && nColumn < m_nNumColumns )
		m_nColAlign[nColumn] = nAlign;
}

void AUIListBox::EnsureVisible(int nIndex)
{
	if( m_bAutoWrap )
		return;
	if( nIndex < 0 || nIndex >= (int)m_Item.size() )
		return;

	FitFont();
	
	int nLinesPP = GetLinesPerPage();

	SetFirstItem(nIndex);
	if( m_nFirstItem + nLinesPP >= (int)m_Item.size() )
		SetFirstItem(m_Item.size() - nLinesPP);
	if( m_nFirstItem < 0 )
		SetFirstItem(0);
}

void AUIListBox::CalcItemLines(int nIndex)
{
	m_Item[nIndex].nLines = 1;
	if (!m_bAutoWrap)
		return;

	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	int iCurLineWidth = 0;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( m_nNumColumns > 1 )
	{
		CSplit s(m_Item[nIndex].strText);
		CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
		int nNumCols = min(m_nNumColumns, int(vec.size()));
		int nLines;
		A3DRECT rc;
		for( int j = 0; j < nNumCols; j++ )
		{
			if( m_nColAlign[j] == AUIFRAME_ALIGN_LEFT && m_bAutoWrap)
			{
				rc.SetRect(0, 0, m_nColWidth[j] * fWindowScale, 1024);

				bool bval = AUI_TextOutFormat(pFont, 0, 0, vec[j],
					&rc, 0, &nLines, m_nLineSpace * fWindowScale, m_bAutoIndent, NULL, NULL, NULL, NULL, false, m_nShadow, 255, true, &m_Item[nIndex].itemsSet, m_pvecImageList, fWindowScale);
				if( !bval ) 
					AUI_ReportError(__LINE__, 1, "AUIListBox::CalcTextLines(), failed to call AUI_TextOutFormat()");
				if( nLines > m_Item[nIndex].nLines )
					m_Item[nIndex].nLines = nLines;
			}
			iCurLineWidth += m_nColWidth[j];
		}
	}
	else
	{
		if (m_bAutoWrap)
		{
			A3DRECT rcText(0, 0, m_rcImage[AUILISTBOX_RECT_FRAME].Width(), m_rcImage[AUILISTBOX_RECT_FRAME].Height());
			AUI_TextOutFormat(pFont, 0, 0, m_Item[nIndex].strText,
				&rcText, 0, &m_Item[nIndex].nLines, m_nLineSpace * fWindowScale, m_bAutoIndent, NULL, NULL, NULL, NULL, false, m_nShadow, 255, true, &m_Item[nIndex].itemsSet, m_pvecImageList, fWindowScale);
		}
	}
}

void AUIListBox::CalcWrapScroll(bool bCalcAllLines)
{
	ASSERT(m_bAutoWrap);

	SIZE sLimit = { 0, 0 };
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	POINT ptPos = GetPos();
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	int nLinesPP = GetLinesPerPage();

	
	// VScroll
	int i;
	int nScrollW = m_rcImage[AUILISTBOX_RECT_VSCROLL].Width();
	int nRealLines = m_Item.size();
	if( !m_bForceRenderScroll && m_Item.size() <= nLinesPP )
	{
		m_rcImage[AUILISTBOX_RECT_FRAME].SetRect(cx, cy, m_nWidth - cx, m_nHeight - cy);
		if( bCalcAllLines )
		{
			for(i = 0; i < m_Item.size(); i++ )
				CalcItemLines(i);
		}
		nRealLines = 0;
		for(i = 0; i < m_Item.size(); i++ )
			nRealLines += m_Item[i].nLines;
	}
	if (nRealLines > nLinesPP) 
		m_bRenderVScroll = true;
	else
		m_bRenderVScroll = false;

	if( m_bForceRenderScroll || m_bRenderVScroll )
	{
		m_rcImage[AUILISTBOX_RECT_FRAME].SetRect(cx, cy, m_nWidth - cx - nScrollW, m_nHeight - cy);
		if( bCalcAllLines )
		{
			m_nHBarMax = 0;
			for(i = 0; i < m_Item.size(); i++ )
				CalcItemLines(i);
		}
	}

	// HScroll
	m_nHBarMax = 0;
	m_bRenderHScroll = false;
	m_nHBarPos = 0;

	int nLines = 0;
	m_nScrollLines = m_Item.size();
	while( m_nScrollLines > 0 )
	{
		m_nScrollLines--;
		nLines += m_Item[m_nScrollLines].nLines;
		if( nLines > nLinesPP )
		{
			m_nScrollLines++;
			break;
		}
	}

	m_rcImage[AUILISTBOX_RECT_FRAME].SetRect(cx, cy, 
		m_nWidth - cx - ((m_bRenderVScroll||m_bForceRenderScroll)?nScrollW:0), 
		m_nHeight - cy);
	m_rcImage[AUILISTBOX_RECT_VBAR].SetRect(m_nWidth - m_rcImage[AUILISTBOX_RECT_VSCROLL].Width(), m_rcImage[AUILISTBOX_RECT_UP].Height(), 
		m_nWidth, m_nHeight - m_rcImage[AUILISTBOX_RECT_DOWN].Height());
	m_rcImage[AUILISTBOX_RECT_HBAR].SetRect(0,0,0,0);
}

void AUIListBox::SetFirstItem(int nFirstItem)
{
	if( nFirstItem < 0 )
		nFirstItem = 0;

	if( m_nFirstItem != nFirstItem )
		UpdateRenderTarget();

	m_nFirstItem = nFirstItem;
}

void AUIListBox::SetImageList(abase::vector<A2DSprite*> *pvecImageList)
{
	m_pvecImageList = pvecImageList;
}

bool AUIListBox::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	for( int i = 0; i < AUILISTBOX_RECT_NUM; i++ )
		if( m_pA2DSprite[i] )
		{
			m_rcImage[i] = m_pA2DSprite[i]->GetItem(0)->GetRect();
			m_rcImage[i].SetRect(m_rcImage[i].left * fWindowScale, m_rcImage[i].top * fWindowScale,
				m_rcImage[i].right * fWindowScale, m_rcImage[i].bottom * fWindowScale);
		}
		
	UpdateScroll();
	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(fWindowScale);

	return true;
}

void AUIListBox::SetSize(int W, int H)
{
	AUIObject::SetSize(W, H);
	UpdateScroll();
}

void AUIListBox::SetDefaultSize(int W, int H)
{
	AUIObject::SetDefaultSize(W, H);
	UpdateScroll();
}

bool AUIListBox::SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
	A3DCOLOR dwColor, int nShadow, int nOutline, bool bBold, bool bItalic, bool bFreeType,
	A3DCOLOR clOutline, A3DCOLOR clInnerText, bool bInterpColor, A3DCOLOR clUpper, A3DCOLOR clLower)
{
	bool bval=AUIObject::SetFontAndColor(pszFontName, nFontHeight, dwColor, nShadow, nOutline, 
							bBold, bItalic, bFreeType, clOutline, clInnerText, bInterpColor, clUpper, clLower);
	UpdateScroll();

	return bval;
}

//无wrap
int AUIListBox::GetRequiredWidth()
{
	ASSERT( ! m_bAutoWrap );
	int nTextW = 0;
	SIZE sLimit = { 0, 0 };
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	for( size_t i = 0; i < m_Item.size(); ++ i )
	{
		A3DPOINT2 ptSize = pFont->GetTextExtent( m_Item[i].strText );
		if( nTextW < ptSize.x )
			nTextW = ptSize.x;
	}
	return nTextW + sLimit.cx + 1;
}

int AUIListBox::GetRequiredHeight()
{
	int nLines = 0;
	for(int i = 0; i < m_Item.size(); i++)
		nLines += m_Item[i].nLines;

	int nTextH = GetLineHeight();
	SIZE sLimit = { 0, 0 };
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();

	return nTextH * nLines + sLimit.cy + 1;
}

void AUIListBox::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIListBox::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	for (int i = 0; i < AUILISTBOX_RECT_NUM; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);
}

void AUIListBox::SetHBarPos(int nPos)
{
	a_Clamp(nPos, 0, m_nHBarMax);

	if( nPos != m_nHBarPos )
		UpdateRenderTarget();

	m_nHBarPos = nPos;
}

int AUIListBox::GetItemWidth(int nIndex)
{
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	int iCurLineWidth = 0;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( m_nNumColumns > 1 )
	{
		CSplit s(m_Item[nIndex].strText);
		CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
		int nNumCols = min(m_nNumColumns, int(vec.size()));
		A3DRECT rc;
		for( int j = 0; j < nNumCols; j++ )
		{
			iCurLineWidth += m_nColWidth[j];
		}
	}
	else
	{
		if (m_bAutoWrap)
		{
			iCurLineWidth = 0;
		}
		else
		{
			A3DPOINT2 ptSize;
			A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
			if( m_pvecImageList && (*m_pvecImageList)[0] )
				AUI_GetTextRect(pFont, m_Item[nIndex].strText, ptSize.x, ptSize.y, m_Item[nIndex].nLines, m_nLineSpace * fWindowScale, 
				&m_Item[nIndex].itemsSet, 0, 0, m_pvecImageList, fWindowScale, NULL, m_bAutoIndent);
			else
				AUI_GetTextRect(pFont, m_Item[nIndex].strText, ptSize.x, ptSize.y, m_Item[nIndex].nLines, m_nLineSpace * fWindowScale,
				NULL, 0, 0, NULL, fWindowScale, NULL, m_bAutoIndent);
			iCurLineWidth = ptSize.x;
		}
	}

	return iCurLineWidth;
}

void AUIListBox::CalcNoWrapScroll(bool bCalcAllLines)
{
	ASSERT(!m_bAutoWrap);
	SIZE sLimit = { 0, 0 };
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();

	int nTextWidth = m_nWidth - sLimit.cx;
	int nTextHeight = m_nHeight - sLimit.cy;

	// HScroll
	int nLines = m_Item.size();
	if (m_bEnableHScroll)
	{
		if (bCalcAllLines)
		{
			m_nRowMaxWidth = 0;
			for (int i=0; i<nLines; i++)
			{
				int nRowWidth = GetItemWidth(i);
				if (nRowWidth > m_nRowMaxWidth)
					m_nRowMaxWidth = nRowWidth;
			}
		}
		if (m_nRowMaxWidth > nTextWidth)
		{
			m_bRenderHScroll = true;
			nTextHeight -= m_rcImage[AUILISTBOX_RECT_VBAR].Width();
		}
		else
		{
			m_bRenderHScroll = false;
		}
	}
	else
	{
		m_bRenderHScroll = false;
	}
	

	// VScroll
	int nLineH = GetLineHeight();
	float fWindowScale = m_pParent->GetScale() * m_pParent->GetAUIManager()->GetWindowScale();
	if (nLineH * nLines > nTextHeight + m_nLineSpace*fWindowScale)
	{
		m_bRenderVScroll = true;
		nTextWidth -= m_rcImage[AUILISTBOX_RECT_VBAR].Width();
		if (m_bEnableHScroll)
		{
			if (!m_bRenderHScroll && m_nRowMaxWidth > nTextWidth) //增加vscroll会导致hscroll有可能需要绘制
			{
				m_bRenderHScroll = true;
				nTextHeight -= m_rcImage[AUILISTBOX_RECT_VBAR].Width();
			}
		}
		
	}
	else
	{
		m_bRenderVScroll = false;
	}

	// Update scrollable lines
	int nLinesPP = nTextHeight / nLineH;
	m_nScrollLines = nLines - nLinesPP;
	if (m_nScrollLines < 0)
		m_nScrollLines = 0;
	if (m_nFirstItem > m_nScrollLines)
		m_nFirstItem = m_nScrollLines;

	m_nHBarMax = m_nRowMaxWidth - nTextWidth;
	if (m_nHBarMax < 0)
		m_nHBarMax = 0;
	if( m_nHBarPos > m_nHBarMax )
		m_nHBarPos = m_nHBarMax;

	// Update rect
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	m_rcImage[AUILISTBOX_RECT_FRAME].SetRect(cx, cy, cx + nTextWidth, cx + nTextHeight);
	
	m_rcImage[AUILISTBOX_RECT_VBAR].SetRect(
		m_nWidth - m_rcImage[AUILISTBOX_RECT_VSCROLL].Width(), m_rcImage[AUILISTBOX_RECT_UP].Height(), 
		m_nWidth, m_nHeight - m_rcImage[AUILISTBOX_RECT_DOWN].Height());

	m_rcImage[AUILISTBOX_RECT_HBAR].SetRect(
		m_rcImage[AUILISTBOX_RECT_UP].Height(), m_nHeight - m_rcImage[AUILISTBOX_RECT_UP].Width(), 
		m_nWidth - m_rcImage[AUILISTBOX_RECT_DOWN].Height() * (1+m_bRenderVScroll), m_nHeight);
}

float AUIListBox::GetLineHeight()
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if(m_nRowFixedHeight > 0)
		return float(m_nRowFixedHeight) * fWindowScale;
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	A3DPOINT2 ptSize = pFont->GetTextExtent(_AL("Wjg"));
	return ptSize.y + m_nLineSpace * fWindowScale;
}

void AUIListBox::UpdateScroll(int index)
{
	if (index == -1)
	{
		if (!m_bAutoWrap)
			CalcNoWrapScroll(true);
		else
			CalcWrapScroll(true);
	}
	else
	{
		if (!m_bAutoWrap)
		{
			int nLineWidth = GetItemWidth(index);
			if (m_nRowMaxWidth < nLineWidth)
				m_nRowMaxWidth = nLineWidth;

			CalcItemLines(index);
			CalcNoWrapScroll(false);
		}
		else
		{
			CalcItemLines(index);
			CalcWrapScroll(false);
		}
	}
}

int AUIListBox::GetLinesPerPage()
{
	int nLinesPP = 0;
	float fWindowScale = m_pParent->GetScale() * m_pParent->GetAUIManager()->GetWindowScale();
	float fTextH = GetLineHeight();
	nLinesPP = (m_rcImage[AUILISTBOX_RECT_FRAME].Height() + m_nLineSpace * fWindowScale) / fTextH;

	return nLinesPP;
}

void AUIListBox::ClearItemRcInfo()
{
	m_vEBItems.clear();
	m_vNameLinks.clear();
}