// Filename	: AUIEditBox.cpp
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIEditBox is the class of edit box control.

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
#include "A3DFlatCollector.h"
#include "A3DGDI.h"
#include "A3DFlatCollector.h"
#include "CSplit.h"

#include "AUIDef.h"
#include "AUIDialog.h"
#include "AUIEditBox.h"
#include "AUIManager.h"
#include "AUITextArea.h"
#include "AUICTranslate.h"
#include <mbctype.h>
#include <malloc.h>
#include "ImeUi.h"
#include "IMEWrapper.h"

bool g_bHideIme = false;

#define DEFAULT_IME_NAME L"E"

static wchar_t _ime_name[256] = DEFAULT_IME_NAME;
static HWND _ime_hwnd = NULL;
static DWORD _ime_thread_id = 0;
static HIMC _cur_imc = 0;
volatile static bool _ime_toggle_flag = true;
volatile static bool _ime_toggle_old_flag = true;

static void reset_ime_name()
{
	wcscpy(_ime_name, DEFAULT_IME_NAME);
}

static void get_ime_name(HKL hKL)
{
	memset(_ime_name, 0, sizeof(_ime_name));
	ImmGetDescriptionW(hKL, _ime_name, sizeof(_ime_name));
}

static void valid_number_str(ACString& strText)
{
	int iValidIndex = 0;
	int iLen = strText.GetLength();
	for(int i = 0; i < iLen; i++)
	{
		if (strText[i] >= '0' && strText[i] <= '9')
		{
			strText[iValidIndex] = strText[i];
			iValidIndex++;
		}
	}

	if (iValidIndex == 0)
		strText.Empty();
	else
		strText.CutRight(iLen - iValidIndex);
}

#define CAN_SEND_MSG (_ime_thread_id == ::GetCurrentThreadId())

// Declare enum type Languages
enum LANGFLAG           
{
	DEFAULT,
	TRADITIONAL_CHINESE,	
	JAPANESE,
	KOREAN,
	SIMPLIFIED_CHINESE
};  

#define AUIEDITBOX_BLINK_DELAY	500
#define AUIEDITBOX_SCROLL_DELAY	50
#define AUIEDITBOX_DEL_PREVNUM	5

AUIEditBox::AUIEditBox() : AUIObject()
{
	m_nType			= AUIOBJECT_TYPE_EDITBOX;
	m_bNumber		= false;
	m_bReadOnly		= false;
	m_bPassword		= false;
	m_nFrameMode	= 0;
	m_pAUIFrame		= NULL;
	m_nMaxLen		= AUIEDITBOX_MAXTEXTLEN;
	m_nMaxLenOrg	= m_nMaxLen;
	m_bMultiline	= false;
	m_bAutoReturn	= false;
	m_bWantReturn	= true;
	m_nLineSpace	= 0;
	m_rcText.Clear();

	// init ime part
	m_bImeOpen		= false;
	m_nCodePage		= CP_ACP;
	m_nCharSet		= DEFAULT_CHARSET;
	m_fIsNT			= false;

	// Initialize buffer position
	m_xEndPos = m_xInsertPos = m_xCaretPos = 0;
	m_xHScrl = m_xVScrl = 0;

	// Initialize member veriables
	m_fStat			= FALSE;				// Do not show dotted underline
	m_nComSize		= 0;					// Clear variable for composition string size in Byte
	m_nComCursorPos = 0;
	m_nComAttrSize	= 0;
	m_nCaretRef		= 0;
	m_dwProperty	= 0;
	memset(m_bComAttr, 0, sizeof(m_bComAttr));
	memset(m_CharWidthBuf, 0, sizeof(m_CharWidthBuf));
	memset(m_CharHeightBuf, 0, sizeof(m_CharHeightBuf));
	memset(m_szComStr, 0, sizeof(m_szComStr));
	m_ime_var.Clear();

	m_CharHeight	= 10;
	m_nBlink		= 0;
	m_bShowCaret	= true;
	m_nUnderlnWidth = 0;
	m_ptCaret.Set(0, 0);
	m_ptUnderlnStart.Set(0, 0);
	m_ptViewOrg.Set(0, 0);

	m_pA2DSpriteEmotion = NULL;
	m_vecEmotion = NULL;
	m_nCurEmotionSet = 0;
	
	m_bHasItem		= false;
	m_nItemPack		= 0;
	m_nItemIndex	= 0;

	for( int i = 0; i < AUIEDITBOX_RECT_MAX; i++ )
		m_pA2DSprite[i] = NULL;
	m_nState = AUIEDITBOX_STATE_NORMAL;
	m_bDragging = false;

	m_bSelecting = false;
	m_xSelBeginPos = -1;
	m_xSelEndPos = 0;
	m_bAllowSelect = false;
	m_clSelectBg = 0xFF000000;

	m_nAlign = AUIEDITBOX_ALIGN_LEFT;
	m_bMaxLenRendered = false;
}

AUIEditBox::~AUIEditBox()
{
}

bool AUIEditBox::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	EditBox property line format:
	 *
	 *		EDIT				Keyword
	 *		Password			Name
	 *		""					Command
	 *		100 200 150 20		x y Width Height
	 *		"EditFrame.bmp"		Frame image
	 *		2					String ID of font name
	 *		20					Font height
	 *		0 0 0				Text color, R, G, B
	 *		PASSWORD			Password or not
	 *		0					Readonly or not
	 *		1					Multiple line or not
	 *		20					Max text length
	 */
	SetHWnd(pA3DDevice->GetDeviceWnd());

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIEditBox::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;


	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));

	if (!InitIndirect(pASF->m_szToken, 0))
		return AUI_ReportError(__LINE__, 1, "AUIEditBox::Init(), failed to call InitIndirect()");

	// Font.
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
		return AUI_ReportError(__LINE__, 1, "AUIEditBox::Init(), failed to call SetFontAndColor()");

	// Font color.
	CHECK_BREAK2(pASF->GetNextToken(true));
	int r = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int g = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int b = atoi(pASF->m_szToken);

	m_color = A3DCOLORRGB(r, g, b);

	// Password or not.
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_bPassword = (0 == strcmpi(pASF->m_szToken, "PASSWORD")) ? 1 : 0;

	// Readonly or not.
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_bReadOnly = atoi(pASF->m_szToken) ? true : false;
	SetAcceptMouseMessage(!m_bReadOnly);

	// Multiple line or not.
	CHECK_BREAK2(pASF->GetNextToken(true));
	SetIsMultipleLine(atoi(pASF->m_szToken) ? true : false);

	CHECK_BREAK2(pASF->GetNextToken(true));
	SetMaxLength(atoi(pASF->m_szToken));

	if( m_pParent->GetFileVersion() > 0 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		AUIOBJECT_SETPROPERTY p;
		p.i = atoi(pASF->m_szToken);
		SetProperty("Frame Mode", &p);
		RebuildTextRect();
	}
	
	if( m_pParent->GetFileVersion() >= 5 )
	{
		for( int i = AUIEDITBOX_RECT_MIN + 1; i < AUIEDITBOX_RECT_MAX; i++ )
		{
			if (!pASF->GetNextToken(true))
				return AUI_ReportError(__LINE__, 1, "AUIEditBox::Init(), failed to read from file");

			if (!InitIndirect(pASF->m_szToken, i))
				return AUI_ReportError(__LINE__, 1, "AUIEditBox::Init(), failed to call InitIndirect()");
		}
	}

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIEditBox::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIEditBox::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	m_szFontName = m_pParent->GetAUIManager()->GetDefaultFontName();
	m_nFontHeight = m_pParent->GetAUIManager()->GetDefaultFontSize();
	
	int nMaxLen = AUIEDITBOX_MAXTEXTLEN;
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_BOOL_VALUE(pItem, _AL("Anomaly"), m_bAnomaly)
	XMLGET_BOOL_VALUE(pItem, _AL("Password"), m_bPassword)
	XMLGET_BOOL_VALUE(pItem, _AL("ReadOnly"), m_bReadOnly)
	XMLGET_BOOL_VALUE(pItem, _AL("NumberOnly"), m_bNumber)
	XMLGET_BOOL_VALUE(pItem, _AL("Multiline"), m_bMultiline)
	XMLGET_BOOL_VALUE(pItem, _AL("AutoReturn"), m_bAutoReturn)
	XMLGET_BOOL_VALUE(pItem, _AL("WantReturn"), m_bWantReturn) 
	XMLGET_INT_VALUE(pItem, _AL("MaxLen"), nMaxLen)
	XMLGET_INT_VALUE(pItem, _AL("LineSpace"), m_nLineSpace);
	XMLGET_BOOL_VALUE(pItem, _AL("EnableSelection"), m_bAllowSelect);
	XMLGET_INT_VALUE(pItem, _AL("TextAlign"), m_nAlign);
	SetMaxLength(nMaxLen);
	SetIsReadOnly(m_bReadOnly, true);

	if (!m_bMultiline)
	{
		m_bAutoReturn = false;
		m_bWantReturn = false;
	}

	XMLGET_STRING_VALUE(pItem, _AL("Command"), m_strCommand)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	RebuildTextRect();

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
					InitIndirect(strFileName, AUIEDITBOX_RECT_FRAME);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("UpImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIEDITBOX_RECT_UP);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("DownImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIEDITBOX_RECT_DOWN);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIEDITBOX_RECT_SCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("BarFrameImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIEDITBOX_RECT_BAR);
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
			XMLGET_COLOR_VALUE(pChildItem, _AL("SelectBgColor"), m_clSelectBg)
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
		return AUI_ReportError(__LINE__, 1, "AUIEditBox::InitXML(), failed to call SetFontAndColor()");
	
	if( m_pParent )
		SetEmotionList(m_pParent->GetAUIManager()->GetDefaultSpriteEmotion(), m_pParent->GetAUIManager()->GetDefaultEmotion());

	return true;
}

bool AUIEditBox::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUIEDITBOX_RECT_MIN && idType < AUIEDITBOX_RECT_MAX);

	UpdateRenderTarget();
	if( AUIEDITBOX_RECT_FRAME == idType )
	{
		A3DRELEASE(m_pAUIFrame);
		if( strlen(pszFileName) <= 0 ) return true;

		m_pAUIFrame = new AUIFrame;
		if( !m_pAUIFrame ) return false;

		bval = m_pAUIFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
		if( !bval )
		{
			A3DRELEASE(m_pAUIFrame)
			return AUI_ReportError(__LINE__, 1, "AUIEditBox::InitIndirect(), failed to call m_pAUIFrame->Init()");
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
			return AUI_ReportError(__LINE__, 1, "AUIEditBox::InitIndirect(), failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUIEDITBOX_RECT_UP == idType || AUIEDITBOX_RECT_DOWN == idType )
		{
			A3DRECT rc[2];
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight();
			rc[0].SetRect(0, 0, W, H / 2);
			rc[1].SetRect(0, H / 2, W, H);
			bval = m_pA2DSprite[idType]->ResetItems(2, rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::InitIndirect(), failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
	}
	RebuildTextRect();

	return true;
}

void AUIEditBox::RebuildTextRect(void)
{
	if( m_pAUIFrame )
	{
		SIZE sLimit = m_pAUIFrame->GetSizeLimit();
		int W = sLimit.cx / 2;
		int H = sLimit.cy / 2;

		m_rcText.SetRect(W, H, m_nWidth - W, m_nHeight - H);
	}
	else
		m_rcText.SetRect(0, 0, m_nWidth, m_nHeight);

	if( m_bMultiline && m_pA2DSprite[AUIEDITBOX_RECT_SCROLL] )
	{
		A3DRECT rcScroll = m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->GetItem(0)->GetRect();
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
			rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
		m_rcText.right -= rcScroll.Width();
	}
}

bool AUIEditBox::Release(void)
{
	if (m_bFocus)
		_ime_toggle_flag = false;

	A3DRELEASE(m_pAUIFrame);
	for( int i = 0; i < AUIEDITBOX_RECT_MAX; i++ )
		A3DRELEASE(m_pA2DSprite[i])
	m_ItemsSet.Release();
	m_bHasItem = false;
	m_nMaxLen = m_nMaxLenOrg;
	return AUIObject::Release();
}

bool AUIEditBox::Save(FILE *file)
{
	fprintf(file, "EDIT");

	if( !AUIObject::Save(file) ) return false;

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_szFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idString = m_pParent->AddStringToTable(szFontName);
	if( idString < 0 ) return false;

	fprintf(file, " \"%s\" %d %d %d %d %d \"%s\" %d %d %d %d",
		m_pAUIFrame ? m_pAUIFrame->GetA2DSprite()->GetName() : "",
		idString, m_nFontHeight, A3DCOLOR_GETRED(m_color),
		A3DCOLOR_GETGREEN(m_color),	A3DCOLOR_GETBLUE(m_color),
		m_bPassword ? "PASSWORD" : "TEXT", m_bReadOnly, m_bMultiline, m_nMaxLen, m_nFrameMode);

	for( int i = 1; i < AUIEDITBOX_RECT_MAX; i++ )
		fprintf(file, " \"%s\"", m_pA2DSprite[i] ? m_pA2DSprite[i]->GetName() : "");

	fprintf(file, " \n");

	return true;
}

bool AUIEditBox::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemEditBox = new AXMLItem;
	pXMLItemEditBox->InsertItem(pXMLItemDialog);
	pXMLItemEditBox->SetName(_AL("EDIT"));
	XMLSET_STRING_VALUE(pXMLItemEditBox, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemEditBox, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemEditBox, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemEditBox, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemEditBox, _AL("Height"), m_nHeight, 0)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("Anomaly"), m_bAnomaly, false)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("Password"), m_bPassword, false)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("ReadOnly"), m_bReadOnly, false)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("NumberOnly"), m_bNumber, false)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("Multiline"), m_bMultiline, false)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("AutoReturn"), m_bAutoReturn, false)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("WantReturn"), m_bWantReturn, true)
	XMLSET_INT_VALUE(pXMLItemEditBox, _AL("MaxLen"), m_nMaxLen, AUIEDITBOX_MAXTEXTLEN)
	XMLSET_INT_VALUE(pXMLItemEditBox, _AL("LineSpace"), m_nLineSpace, 0)
	XMLSET_BOOL_VALUE(pXMLItemEditBox, _AL("EnableSelection"), m_bAllowSelect, false)
	XMLSET_INT_VALUE(pXMLItemEditBox, _AL("TextAlign"), m_nAlign, AUIEDITBOX_ALIGN_LEFT)

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemEditBox);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	XMLSET_STRING_VALUE(pXMLItemEditBox, _AL("Command"), AS2AC(m_strCommand), _AL(""))	
	XMLSET_STRING_VALUE(pXMLItemEditBox, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemEditBox);
	pXMLItemText->SetName(_AL("Text"));
	XMLSET_STRING_VALUE(pXMLItemText, _AL("FontName"), m_szFontName, _AL(""))
	XMLSET_INT_VALUE(pXMLItemText, _AL("FontSize"), m_nFontHeight, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("ShadowColor"), m_nShadow, _AL("0,0,0,0"));
	XMLSET_INT_VALUE(pXMLItemText, _AL("Outline"), m_nOutline, 0)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Bold"), m_bBold, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Italic"), m_bItalic, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("FreeType"), m_bFreeType, true)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("Color"), m_color, _AL("255,255,255,255"));
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("SelectBgColor"), m_clSelectBg, _AL("0,0,0,255"));
	
	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemEditBox);
	pXMLItemResource->SetName(_AL("Resource"));
	AXMLItem *pXMLItemFrameImage = new AXMLItem;
	pXMLItemFrameImage->InsertItem(pXMLItemResource);
	pXMLItemFrameImage->SetName(_AL("FrameImage"));
	if( m_pAUIFrame )
	{
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}

	if( m_pA2DSprite[AUIEDITBOX_RECT_UP] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("UpImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIEDITBOX_RECT_UP]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUIEDITBOX_RECT_DOWN] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("DownImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUIEDITBOX_RECT_SCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("ScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUIEDITBOX_RECT_BAR] )
	{
		AXMLItem *pXMLItemBarFrameImage = new AXMLItem;
		pXMLItemBarFrameImage->InsertItem(pXMLItemResource);
		pXMLItemBarFrameImage->SetName(_AL("BarFrameImage"));
		XMLSET_STRING_VALUE(pXMLItemBarFrameImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIEDITBOX_RECT_BAR]->GetName()), _AL(""))
	}

	return true;
}

const ACHAR * AUIEditBox::GetText(void)
{
	m_szText = MarshalEditBoxText(m_szBuffer, m_ItemsSet);
	ACString strReturn = _AL("");
	if( m_bAutoReturn )
	{
		int i = 1;
		while( i < m_szText.GetLength() )
		{
			if( m_szText[i - 1] == '\r' && m_szText[i] == '\n')
				m_szText = m_szText.Left(i - 1) + m_szText.Mid(i + 1);
			else
				i++;
		}
	}
	return m_szText;
}

void AUIEditBox::SetText(const ACHAR *pszText)
{
	m_nComSize = 0;
	AUIObject::SetText(pszText);

	SetDynamicRender(false);
	m_ItemsSet.Release();
	m_bHasItem = false;
	
	m_szBuffer.Empty();
	m_nMaxLen = m_nMaxLenOrg;
	ACString strUnmarshed = UnmarshalEditBoxText(pszText, m_ItemsSet);
	ValidInsertText(0, strUnmarshed, strUnmarshed.GetLength());
	
	m_xEndPos = m_szBuffer.GetLength();
	m_xInsertPos = m_xCaretPos = m_xEndPos;
	m_ptViewOrg.Set(0, 0);
	
	
	CalcTexWidthBuf();

	if (m_bFocus)
		ShowCaretOnView();
}

void AUIEditBox::SetTextAndItem(const ACHAR *pszText, const ACHAR* szIvtrItem, A3DCOLOR clIvtrItem, const EditboxScriptItem** ppScriptItems, int nScriptItemCount)
{
	AUIObject::SetText(pszText);
	m_ItemsSet.Release();
	m_bHasItem = false;

	ACString strUnmarshed = UnmarshalEditBoxText(pszText, m_ItemsSet, 0, szIvtrItem, clIvtrItem, 0xffffffff, ppScriptItems, nScriptItemCount);	
	m_szBuffer.Empty();
	m_nMaxLen = m_nMaxLenOrg;
	ValidInsertText(0, strUnmarshed, strUnmarshed.GetLength());

	m_xEndPos = m_szBuffer.GetLength();
	m_xInsertPos = m_xCaretPos = m_xEndPos;
	m_ptViewOrg.Set(0, 0);
	
	
	CalcTexWidthBuf();

	if (m_bFocus)
		ShowCaretOnView();
}

bool AUIEditBox::SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
	A3DCOLOR dwColor, int nShadow, int nOutline, bool bBold, bool bItalic, bool bFreeType,
	A3DCOLOR clOutline, A3DCOLOR clInnerText, bool bInterpColor, A3DCOLOR clUpper, A3DCOLOR clLower)
{
	bool bval=AUIObject::SetFontAndColor(pszFontName, nFontHeight, dwColor, nShadow, nOutline, 
							bBold, bItalic, bFreeType, clOutline, clInnerText, bInterpColor, clUpper, clLower);
	if(bval && GetFont() )
	{
		m_CharHeight = GetFont()->GetFontHeight();
		//m_nFontHeight = m_CharHeight;
	}
	return bval;
}

void AUIEditBox::CalcTexWidthBuf()
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	FitFont();

	A3DFTFont* pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	m_CharHeight = pFont->GetFontHeight();

	int nLen = wcslen(m_szBuffer);
	m_CharWidthBuf[0] = 0;
	m_CharHeightBuf[0] = 0;

	int nEmotionW = 0;
	if( m_pA2DSpriteEmotion && m_pA2DSpriteEmotion[0] )
	{
		A3DRECT rc = m_pA2DSpriteEmotion[0]->GetItem(0)->GetRect();
		nEmotionW = rc.Width() * pFont->GetFontHeight() / rc.Height();
	}

	if (m_bPassword)
	{
		WORD wWidth = pFont->GetTextExtent(L"*").x;

		for (int i = 0; i < nLen; i++)
			m_CharWidthBuf[i+1] = m_CharWidthBuf[i] + wWidth;
		memset(m_CharHeightBuf, 0, nLen * sizeof(WORD));
	}
	else
	{
		if(!m_bAutoReturn)
		{
			float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
			for (int i = 0; i < nLen; i++)
			{
				if(m_bMultiline && m_szBuffer[i]=='\r')
				{
					m_CharWidthBuf[i+1] = 0;
					m_CharHeightBuf[i+1] = m_CharHeightBuf[i] + m_CharHeight + m_nLineSpace * fWindowScale;
				}
				else
				{
					if (IsEditboxItemCode(m_szBuffer[i]))
					{
						EditBoxItemBase* pItem = m_ItemsSet.GetItemByChar(m_szBuffer[i]);

						if (pItem)
						{
							if( pItem->GetType() == enumEIEmotion )
								m_CharWidthBuf[i+1] = m_CharWidthBuf[i] + nEmotionW;
							else if (pItem->GetType() == enumEIBox)
								m_CharWidthBuf[i+1] = m_CharWidthBuf[i] + pItem->GetBoxWidth();
							else
								m_CharWidthBuf[i+1] = m_CharWidthBuf[i] + pFont->GetTextExtent(pItem->GetName()).x;
						}
					}
					else
						m_CharWidthBuf[i+1] = m_CharWidthBuf[i] + pFont->GetTextExtent(&m_szBuffer[i], 1).x;

					m_CharHeightBuf[i+1] = m_CharHeightBuf[i];
				}
			}
			int o = pFont->GetTextExtent(&m_szBuffer[0], nLen).x;
		}
		else
		{
			int i=0;
			int j=0;
			int len=m_szBuffer.GetLength();
			ACHAR tempBuffer[AUIEDITBOX_MAXTEXTLEN * 2];
			int xInsert=m_xInsertPos;
			int xCaret=m_xCaretPos;
			while(i<len)
			{
				if(m_szBuffer[i]==L'\r')
				{
					if(i+1==m_szBuffer.GetLength() || m_szBuffer[i+1]!=L'\n')
					{
						m_CharWidthBuf[j+1] = 0;
						m_CharHeightBuf[j+1] = m_CharHeightBuf[j] + m_CharHeight + m_nLineSpace * fWindowScale;
						tempBuffer[j] = L'\r';
						j++;
					}
					else
					{
						i++;
						if(i<xInsert)
							m_xInsertPos-=2;
						if(i<xCaret)
							m_xCaretPos-=2;
						m_xEndPos-=2;
					}
				}
				else
				{
					int width;
					if (IsEditboxItemCode(m_szBuffer[i]))
					{
						EditBoxItemBase* pItem = m_ItemsSet.GetItemByChar(m_szBuffer[i]);

						if (pItem)
						{
							if( pItem->GetType() == enumEIEmotion )
								width = nEmotionW;
							else if (pItem->GetType() == enumEIBox)
								width = pItem->GetBoxWidth();
							else
								width = pFont->GetTextExtent(pItem->GetName()).x;
						}
						else
							width = 0;
					}
					else
						width = pFont->GetTextExtent(&m_szBuffer[i], 1).x;
					if(m_CharWidthBuf[j] + width > m_rcText.Width())
					{
						m_xEndPos+=2;
						if(i<=xInsert)
							m_xInsertPos+=2;
						if(i<=xCaret)
							m_xCaretPos+=2;
						m_CharWidthBuf[j+2] = m_CharWidthBuf[j+1] = 0;
						m_CharHeightBuf[j+2] = m_CharHeightBuf[j+1] = m_CharHeightBuf[j] + m_CharHeight + m_nLineSpace * fWindowScale;
						tempBuffer[j++] = L'\r';
						tempBuffer[j++] = L'\n';
					}
					m_CharWidthBuf[j+1] = m_CharWidthBuf[j] + width;
					m_CharHeightBuf[j+1] = m_CharHeightBuf[j];
					tempBuffer[j] = m_szBuffer[i];
					j++;
				}
				i++;
			}
			tempBuffer[j] = L'\0';
			m_szBuffer = tempBuffer;
			len = j;
			j = 0;
			for(i=0;i<m_xInsertPos;i++)
				if(m_szBuffer[i]!=L'\r')
					tempBuffer[j++] = m_szBuffer[i];
				else if(i+1<m_szBuffer.GetLength()&&m_szBuffer[i+1]==L'\n')
					i++;
				else
					tempBuffer[j++] = m_szBuffer[i];
			for(i=m_xCaretPos;i<len;i++)
				if(m_szBuffer[i]!=L'\r')
					tempBuffer[j++] = m_szBuffer[i];
				else if(i+1<m_szBuffer.GetLength()&&m_szBuffer[i+1]==L'\n')
					i++;
				else
					tempBuffer[j++] = m_szBuffer[i];
			tempBuffer[j] = L'\0';
		}
	}

	DoAlign();
}

void AUIEditBox::CalcInsertPos(int x,int y)
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	x -= m_rcText.left + m_ptViewOrg.x;
	y -= m_rcText.top + m_ptViewOrg.y;

	const wchar_t *p = m_szBuffer;
	int xStart = 0;
	int i = 0;
	int nLineH = m_CharHeight + m_nLineSpace * fWindowScale;

	while (true)
	{
		if (*p == L'\r' || *p == L'\0')
		{
			y -= nLineH;
			if(y < 0)
				break;
			if (*p == L'\0')
				break;
			p++;
			i++;
			continue;
		}
		if( y <= m_CharHeight && *p != L'\n')
		{
			int nCharWidth = m_CharWidthBuf[i+1] - m_CharWidthBuf[i];
			if(x >= m_CharWidthBuf[i] && x <= m_CharWidthBuf[i+1])
			{
				if (x - m_CharWidthBuf[i] > nCharWidth / 2)
					i++;
				break;
			}
			else if ( x < m_CharWidthBuf[i])
			{
				break;
			}
		}
		p++;
		i++;
	}

	if (m_nComSize==0)
		m_xInsertPos = m_xCaretPos = i;
	else
	{
		if(i < m_xInsertPos)
		{
			m_xInsertBackup = i;
			OnImeComposition(0,GCS_COMPSTR);
		}
		else if(i > m_xCaretPos)
		{
			m_xInsertBackup += i - m_xCaretPos;
			OnImeComposition(0,GCS_COMPSTR);
		}
	}
	ShowCaretOnView();
}

bool AUIEditBox::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = m_pA3DDevice->GetDeviceWnd();
	int x,y;
	A3DRECT rcWindow;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();

	if( m_bMultiline && m_pA2DSprite[AUITEXTAREA_RECT_SCROLL] )
	{
		A3DPOINT2 ptSize;
		int nLineH;
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
		ptSize = pFont->GetTextExtent(_AL("Wjg"));
		nLineH = ptSize.y + m_nLineSpace * fWindowScale;

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
		A3DRECT rcScrollArea(m_nWidth - rcScroll.Width(),
			rcUp.Height(), m_nWidth, m_nHeight - rcDown.Height());
		if( msg == WM_LBUTTONDOWN ||
			msg == WM_RBUTTONDOWN ||
			msg == WM_LBUTTONDBLCLK ||
			msg == WM_MOUSEMOVE ||
			msg == WM_LBUTTONUP )
		{
			POINT ptPos = GetPos();
			int x = GET_X_LPARAM(lParam) - m_nOffX - ptPos.x;
			int y = GET_Y_LPARAM(lParam) - m_nOffY - ptPos.y;

			if( WM_LBUTTONUP == msg)
			{
				m_bDragging = false;
				m_pParent->SetCaptureObject(NULL);
				m_nState = AUIEDITBOX_STATE_NORMAL;
				return true;
			}

			// Click on scroll bar.
			else if( m_bDragging || rcScrollArea.PtInRect(x, y) )
			{
				int nBarH = m_pA2DSprite[AUITEXTAREA_RECT_BAR]->GetItem(0)->GetRect().Height();
				if( msg == WM_MOUSEMOVE )
				{
					if( wParam & MK_LBUTTON )
					{
						if( AUIEDITBOX_STATE_PRESSBAR == m_nState && m_CharHeightBuf[m_szBuffer.GetLength()] + nLineH > m_rcText.Height() )
						{
							int L = m_CharHeightBuf[m_szBuffer.GetLength()] + nLineH - m_rcText.Height();
							int H = rcScrollArea.Height() - nBarH;

							if( H > 0 )
							{
								m_ptViewOrg.y = -(y - rcScrollArea.top - m_nDragDist) * (L + 1) / H;
								a_Clamp(m_ptViewOrg.y, m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH, 0);
								UpdateCaretPos();
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
						{
							m_ptViewOrg.y += nLineH;
							a_Clamp(m_ptViewOrg.y, m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH, 0);
							UpdateCaretPos();
						}
					}
					// Page down.
					else if( y > m_ptBar.y + nBarH )
					{
						if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
						{
							m_ptViewOrg.y -= nLineH;
							a_Clamp(m_ptViewOrg.y, m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH, 0);
							UpdateCaretPos();
						}
					}
					// Scroll bar.
					else if( m_CharHeightBuf[m_szBuffer.GetLength()] + nLineH > m_rcText.Height() )
					{
						if( msg == WM_LBUTTONDOWN )
						{
							m_bDragging = true;
							m_nDragDist = y - m_ptBar.y;
							m_pParent->SetCaptureObject(this);
							m_nState = AUIEDITBOX_STATE_PRESSBAR;
						}
					}
				}
				return true;
			}
			else if( x >= rcScrollArea.left && x <= rcScrollArea.right )
			{
				// Click on up button.
				if( y < rcScrollArea.top )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUIEDITBOX_STATE_PRESSUP;
						m_nScrollDelay = 0;
					}
				}
				// Click on down button.
				else if( y > rcScrollArea.bottom )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_nState = AUIEDITBOX_STATE_PRESSDOWN;
						m_nScrollDelay = 0;
					}
				}
				return true;
			}
		}
		else if( WM_MOUSEWHEEL == msg )
		{
			int zDelta = (short)HIWORD(wParam);
			if( zDelta > 0 )
				m_ptViewOrg.y += nLineH;
			else if( zDelta < 0 )
				m_ptViewOrg.y -= nLineH;
			a_Clamp(m_ptViewOrg.y, m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH, 0);
			UpdateCaretPos();

			return true;
		}
	}

	// text selecting operation
	if (m_bAllowSelect)
	{
		if (WM_LBUTTONDOWN == msg)
		{
			int xOldPos = m_xCaretPos;

			POINT ptPos = GetPos();
			rcWindow = m_pParent->GetAUIManager()->GetRect();
			x = GET_X_LPARAM(lParam) - ptPos.x - rcWindow.left;
			y = GET_Y_LPARAM(lParam) - ptPos.y - rcWindow.top;
			m_nCaretWidthBuf = -1;
			CalcTexWidthBuf();
			CalcInsertPos(x,y);

			if (!AUI_PRESS(VK_SHIFT))
				CleanSelection();
			else
			{
				if (HasAnySelection())
					m_xSelEndPos = m_xCaretPos;
				else 
				{
					m_xSelBeginPos = xOldPos;
					m_xSelEndPos = m_xCaretPos;
				}
			}
		
			return true;
		}
		else if (WM_MOUSEMOVE == msg && (wParam & MK_LBUTTON))
		{
			POINT ptPos = GetPos();
			rcWindow = m_pParent->GetAUIManager()->GetRect();
			x = GET_X_LPARAM(lParam) - ptPos.x - rcWindow.left;
			y = GET_Y_LPARAM(lParam) - ptPos.y - rcWindow.top;
			m_nCaretWidthBuf = -1;
			CalcInsertPos(x,y);
			
			if (!m_bSelecting)
			{
				m_pParent->SetCaptureObject(this);
				m_xSelBeginPos = m_xCaretPos;
				m_xSelEndPos = m_xCaretPos;
				m_bSelecting = true;
			}
			else
			{
				m_xSelEndPos = m_xCaretPos;
			}
			return true;
		}
		else if (WM_LBUTTONUP == msg)
		{
			m_bSelecting = false;
			m_pParent->SetCaptureObject(NULL);
			return true;
		}
	}
	
	if((g_bHideIme))
	{
		if(ImeUi_IsEnabled())
		{
			bool bNoFurtherProcessing = g_ImeWrapper.MsgProc(msg,wParam,lParam);
			if(bNoFurtherProcessing)
				return true;
		}
	}

	// handle messages which affect ime's position
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		{
			POINT ptPos = GetPos();
			rcWindow = m_pParent->GetAUIManager()->GetRect();
			x = GET_X_LPARAM(lParam) - ptPos.x - rcWindow.left;
			y = GET_Y_LPARAM(lParam) - ptPos.y - rcWindow.top;
			m_nCaretWidthBuf = -1;
			CalcTexWidthBuf();
			CalcInsertPos(x,y);
			return true;
		}
		case WM_CHAR:
			m_nCaretWidthBuf = -1;
			if (!AUI_PRESS(VK_LBUTTON))
				OnChar(wParam);
			return (wParam < TCHAR(0x20) && wParam != ACHAR('\r')) ? false : true;
		case WM_KEYDOWN:
			OnKeyDown(wParam);
			return !AUI_PRESS(VK_MENU) && wParam != VK_TAB;
		case WM_KEYUP:
			return !AUI_PRESS(VK_MENU);
		case WM_USER_IME_CHANGE_FOCUS:
			return true;
		case WM_IME_CHAR:
			return true;
		// Change the setting if input language is changed
		case WM_INPUTLANGCHANGE:
			if (m_bReadOnly)
				return true;
			if (g_bHideIme)
			{
				m_ime_var.Clear();
				OnChangeIme(hWnd);
			}

			// Create new font for changed input language
			SetFont((HKL) lParam);
			break;
		// Start composition
		case WM_IME_STARTCOMPOSITION: 
			if (m_bReadOnly)
				return true;
			// Turn on composition flag
			m_fStat = TRUE;

			// if IME property does not have IME_PROP_AT_CARET, ignore level 3 feature
			if (!(m_dwProperty & IME_PROP_AT_CARET) )
				break;

			OnImeStartComposition(wParam, lParam);
			return TRUE;
		// Composotion char
		case WM_IME_COMPOSITION:
			if (m_bReadOnly)
				return true;
			if (m_dwProperty & IME_PROP_AT_CARET && g_bHideIme)
				m_ime_var.ClearComLst();
			if (OnImeComposition(wParam, lParam))
				return true;
			break;
		// End conposition
		case WM_IME_ENDCOMPOSITION:
			if (m_bReadOnly)
				return true;
			// Turn off composition flag
			m_fStat = FALSE;

			// if IME property does not have IME_PROP_AT_CARET, ignore level 3 feature
			if (!(m_dwProperty & IME_PROP_AT_CARET) )
				break;

			OnImeEndComposition(wParam, lParam);
			break;
		case WM_IME_NOTIFY:
			if (m_bReadOnly)
				return true;

			switch (wParam)
			{
				// Set candidate window position
				case IMN_OPENCANDIDATE:
					if (m_nLanguage == JAPANESE)
						SetCandiDateWindowPos();
				case IMN_CHANGECANDIDATE:
					if (g_bHideIme)
						GetComCanVal(&m_ime_var, TRUE);
					break;
				case IMN_CLOSECANDIDATE:
					if (g_bHideIme)
						m_ime_var.ClearCanLst();
					if (!(m_dwProperty & IME_PROP_AT_CARET))
						m_fStat = FALSE;
					break;
					// To detect the change of IME mode 
				case IMN_SETCONVERSIONMODE:
				case IMN_SETSENTENCEMODE:
					break;
				// To detect the toggling of Japanese IME
				case IMN_SETOPENSTATUS:
					break;
			}	
			break;
		case WM_IME_SETCONTEXT:
			if (m_bReadOnly)
				return true;

			// Except IME mode control in Janap Win9x, dose not need to send this message
			if ((m_nLanguage ==  JAPANESE) && !m_fIsNT)
			{
			}
			break;
		default:
			return false;
	}
	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIEditBox::Tick(void)
{
	if( m_bMultiline && m_pA2DSprite[AUIEDITBOX_RECT_SCROLL] )
	{
		A3DPOINT2 ptSize;
		int nLineH;
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		ptSize = pFont->GetTextExtent(_AL("Wjg"));
		nLineH = ptSize.y + m_nLineSpace * fWindowScale;

		DWORD dwTick = GetTickCount();
		if( dwTick - m_nScrollDelay >= AUIEDITBOX_SCROLL_DELAY )
		{
			if( AUIEDITBOX_STATE_PRESSUP == m_nState )
			{
				m_ptViewOrg.y += nLineH;
				if( m_ptViewOrg.y > 0 )
					m_ptViewOrg.y = 0;
				if( m_nScrollDelay == 0 )
					m_nScrollDelay = dwTick + 500;
				else
					m_nScrollDelay = dwTick;
				UpdateCaretPos();
			}
			else if( AUIEDITBOX_STATE_PRESSDOWN == m_nState )
			{
				m_ptViewOrg.y -= nLineH;
				if( m_ptViewOrg.y < m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH)
					m_ptViewOrg.y = m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH;
				if( m_nScrollDelay == 0 )
					m_nScrollDelay = dwTick + 500;
				else
					m_nScrollDelay = dwTick;
				UpdateCaretPos();
			}
		}

		if( m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH < 0 &&
			m_ptViewOrg.y + m_CharHeightBuf[m_szBuffer.GetLength()] + nLineH < m_rcText.Height() )
		{
			m_ptViewOrg.y = m_rcText.Height() - m_CharHeightBuf[m_szBuffer.GetLength()] - nLineH;
			UpdateCaretPos();
		}
	}

	return AUIObject::Tick();
}

bool AUIEditBox::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL || 
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		bool bval = true;
		POINT ptPos = GetPos();

		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;
		
		bool bNoFlush = m_pParent->IsNoFlush();
		if( m_pAUIFrame )
		{
			if( m_bMultiline && m_pA2DSprite[AUIEDITBOX_RECT_SCROLL] )
			{
				A3DRECT rcScroll = m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->GetItem(0)->GetRect();
				float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
				rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
					rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
				bval = m_pAUIFrame->Render(ptPos.x, ptPos.y, m_nWidth - rcScroll.Width(), m_nHeight,
					NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, bNoFlush);
			}
			else
			{
				bval = m_pAUIFrame->Render(ptPos.x, ptPos.y, m_nWidth, m_nHeight,
					NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, bNoFlush);
			}
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::Render(), failed to call m_pAUIFrame->Render()");
		}

		if( m_bMultiline )
		{
			bval = RenderScroll();
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::Render(), failed to call RenderScroll()");
		}
	}

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL || 
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC )
	{
		ACString szText;
		if( m_bPassword )
		{
			ACString str('*', wcslen(m_szBuffer));
			szText = str;
		}
		else
		{
			szText = m_szBuffer;
		}
		
		if( a_strlen(szText) > 0 )
		{
			FitFont();

			A3DFTFont* pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
			RenderSelectionTextBg(pFont, szText);
			RenderEditText(pFont, szText);
		}

		if( !m_bReadOnly )
		{
			// Draw dotted line under composition string
			if ((m_fStat) && (m_dwProperty & IME_PROP_AT_CARET) && (m_nLanguage != KOREAN))
				RenderUnderline();

			if (IsFocus() )
			{
				if (g_bHideIme)
					RenderImeWnd();
				DisplayCaret();
			}
		}

	}

	return true;
}

bool AUIEditBox::RenderScroll()
{
	if( !m_pA2DSprite[AUIEDITBOX_RECT_UP] ||
		!m_pA2DSprite[AUIEDITBOX_RECT_DOWN] ||
		!m_pA2DSprite[AUIEDITBOX_RECT_SCROLL] ||
		!m_pA2DSprite[AUIEDITBOX_RECT_BAR] )
	{
		return true;
	}

	bool bval = true;
	POINT ptPos = GetPos();
	A2DSprite *pA2DSpriteFrame = NULL;
	int cx = 0, cy = 0;
	if( m_pAUIFrame )
	{
		SIZE sLimit = m_pAUIFrame->GetSizeLimit();
		cx = sLimit.cx / 2;
		cy = sLimit.cy / 2;
	}
	A3DRECT rcUp = m_pA2DSprite[AUIEDITBOX_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->GetItem(0)->GetRect();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	A3DRECT rcText(ptPos.x + cx, ptPos.y + cy, ptPos.x + m_nWidth
		- cx - rcScroll.Width(), ptPos.y + m_nHeight - cy);

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	// Draw up button.
	int nItem = (AUIEDITBOX_STATE_PRESSUP == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUIEDITBOX_RECT_UP]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::RenderScroll(), failed to call m_pA2DSprite[AUIEDITBOX_RECT_UP]->SetCurrentItem()");

	int W = rcScroll.Width();
	int x, y = ptPos.y;
	x = ptPos.x + m_nWidth - W;
	m_pA2DSprite[AUIEDITBOX_RECT_UP]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUIEDITBOX_RECT_UP]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUIEDITBOX_RECT_UP]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUIEDITBOX_RECT_UP]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUIEDITBOX_RECT_UP]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::RenderScroll(), failed to call m_pA2DSprite[AUIEDITBOX_RECT_UP]->DrawToBack() or m_pA2DSprite[AUIEDITBOX_RECT_UP]->DrawToInternalBuffer()");

	// Draw scroll area.
	int H = rcUp.Height();
	y += H;

	float fScale = float(m_nHeight - rcUp.Height() - rcDown.Height())
		/ m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->GetItem(0)->GetRect().Height();
	m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->SetScaleY(fScale);
	m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::RenderScroll(), failed to call m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->DrawToBack() or m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUIEDITBOX_STATE_PRESSDOWN == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::RenderScroll(), failed to call m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->SetCurrentItem()");

	y = ptPos.y + m_nHeight - rcDown.Height();

	m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::RenderScroll(), failed to call m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->DrawToInternalBuffer()");

	// Draw scrollbar button if necessary.
	A3DPOINT2 ptSize;
	int nLineHeight;
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	ptSize = pFont->GetTextExtent(_AL("Wjg"));
	nLineHeight = ptSize.y + m_nLineSpace * fWindowScale;
	if( m_CharHeightBuf[m_szBuffer.GetLength()] + nLineHeight > rcText.Height() )
	{
		int nBarH = m_pA2DSprite[AUIEDITBOX_RECT_BAR]->GetItem(0)->GetRect().Height() * fWindowScale;
		int H = m_nHeight - rcUp.Height() - rcDown.Height() - nBarH;
		if( H > 0 )
		{
			int L = m_CharHeightBuf[m_szBuffer.GetLength()] + nLineHeight - rcText.Height();
			m_ptBar.x = m_nWidth - rcScroll.Width();
			if( -m_ptViewOrg.y < L )
				m_ptBar.y = rcUp.Height() - m_ptViewOrg.y * H / L;
			else
				m_ptBar.y = rcUp.Height() + H;
			m_pA2DSprite[AUIEDITBOX_RECT_BAR]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUIEDITBOX_RECT_BAR]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUIEDITBOX_RECT_BAR]->SetAlpha(nAlpha);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUIEDITBOX_RECT_BAR]->DrawToBack(ptPos.x + m_ptBar.x, ptPos.y + m_ptBar.y);
			else
				bval = m_pA2DSprite[AUIEDITBOX_RECT_BAR]->DrawToInternalBuffer(ptPos.x + m_ptBar.x, ptPos.y + m_ptBar.y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIEditBox::RenderScroll(), failed to call m_pA2DSprite[AUIEDITBOX_RECT_BAR]->DrawToBack() or m_pA2DSprite[AUIEDITBOX_RECT_BAR]->DrawToInternalBuffer()");
		}
	}

	return true;
}

void AUIEditBox::RenderUnderline()
{
	POINT pt = GetPos();
	int minX = pt.x + m_rcText.left;
	int maxX = pt.x + m_rcText.right;
	int minY = pt.y + m_rcText.top;
	int maxY = pt.y + m_rcText.bottom;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	pt.x += m_rcText.left + m_ptViewOrg.x;
	pt.y += m_rcText.top + m_ptViewOrg.y + m_CharHeight + m_nLineSpace * fWindowScale / 2 - 1;

	// Calculate end position of composition string and draw dotted line
	m_szComStr[m_nComSize / WCHARSIZE] = L'\0';

	int restChar = m_nComSize / WCHARSIZE;
	int start = m_xInsertPos;
	int end = start;
	while(restChar > 0 && end < m_szBuffer.GetLength() )
	{
		if(m_szBuffer[end]==L'\n')
		{
			start++;
			end++;
		}
		while( restChar > 0 && end < m_szBuffer.GetLength() && m_szBuffer[end]!=L'\r' )
		{
			restChar--;
			end++;
		}
		m_ptUnderlnStart.x = m_CharWidthBuf[start] + pt.x;
		m_ptUnderlnStart.y = m_CharHeightBuf[start] + pt.y;
		int nUnderlnEnd = m_CharWidthBuf[end] + pt.x; 
		a_Clamp(m_ptUnderlnStart.x, minX, maxX);
		a_Clamp(nUnderlnEnd, minX, maxX);
		
		m_nUnderlnWidth = nUnderlnEnd - m_ptUnderlnStart.x;

		if( m_ptUnderlnStart.y >= minY &&
			m_ptUnderlnStart.y <= maxY)
			DrawUnderline(A3DPOINT2(m_ptUnderlnStart.x + m_nOffX, m_ptUnderlnStart.y + m_nOffY),
					m_nUnderlnWidth);
		start = end + 1;
		end = start;
	}
}

void AUIEditBox::RenderEditText(A3DFTFont* pFont, const ACHAR* szText)
{
	if (wcslen(szText) == 0) return;

	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	int nMaxRow = m_rcText.Height() / m_CharHeight;
	if (nMaxRow == 0) nMaxRow++;

	int nLine = 0;
	const wchar_t *p, *pHead;
	p = pHead = szText;
	int o = pFont->GetTextExtent(p, a_strlen(p)).x;

	A3DRECT rc = m_rcText;
	rc.Offset(GetPos().x + m_nOffX, GetPos().y + m_nOffY);

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int LineHeight = m_CharHeight + m_nLineSpace * fWindowScale;
	int TextOffsetH = m_nLineSpace * fWindowScale / 2;

	int iSelStartChar=0, iSelEndChar=-1, iCharIndexH=0, iCharIndexT=0;
	A3DRECT rcSel, rcLine;
	if (HasAnySelection())
	{
		iSelStartChar = a_Min(m_xSelBeginPos, m_xSelEndPos);
		iSelEndChar = a_Max(m_xSelBeginPos, m_xSelEndPos);
	}

	A3DCOLOR colorText;
	A3DCOLOR colorSelectBg = (m_clSelectBg ^ 0x00FFFFFF);
	while (true)
	{
		if (*p == L'\r' || *p == L'\0')
		{
			iCharIndexT = iCharIndexH + (p-pHead);
			rcSel.SetRect(iSelStartChar, 0, iSelEndChar, 1);
			rcLine.SetRect(iCharIndexH, 0, iCharIndexT, 1);
			rcSel &= rcLine;
			
			if (!rcSel.IsRectNull())
			{
				if (rcSel.left > (pHead-szText)) //abc####...
				{
					p = szText + rcSel.left;
					colorText = m_color;
				}
				else if (rcSel.left == (pHead-szText)) //###...
				{
					p = szText + rcSel.right;
					colorText = colorSelectBg;
				}
				else
				{
					colorText = m_color;
				}
			}
			else
			{
				colorText = m_color;
			}
			
			wchar_t temp = *p;
			(wchar_t)*p = L'\0';
			int offset = pHead-szText;
			if( m_pA2DSpriteEmotion != NULL )
			{
							
				if( (nLine + 1) *LineHeight + m_ptViewOrg.y >= 0 &&
					nLine * LineHeight + m_ptViewOrg.y <= rc.Height() )
				{
					ACHAR szText[AUIEDITBOX_MAXTEXTLEN + 1];
					if( colorText != A3DCOLORRGB(255, 255, 255) )
					{
						A3DCOLOR r = A3DCOLOR_GETRED(colorText);
						A3DCOLOR g = A3DCOLOR_GETGREEN(colorText);
						A3DCOLOR b = A3DCOLOR_GETBLUE(colorText);
						a_sprintf(szText, _AL("^%02X%02X%02X"), r, g, b);
						AUI_ConvertChatString_S(pHead, a_strlen(pHead), szText+7, AUIEDITBOX_MAXTEXTLEN-7);
					}
					else
						AUI_ConvertChatString_S(pHead, a_strlen(pHead), szText, AUIEDITBOX_MAXTEXTLEN);
					if( m_pA2DSpriteEmotion && m_vecEmotion )
					{
						AUI_TextOutFormat( pFont, m_ptViewOrg.x + rc.left + m_CharWidthBuf[offset], 
							rc.top + nLine * LineHeight + m_ptViewOrg.y + TextOffsetH,
							szText, &rc, 0, NULL, 0, false, NULL,
							m_pA2DSpriteEmotion[m_nCurEmotionSet], m_vecEmotion[m_nCurEmotionSet],
							NULL, true, m_nShadow, nAlpha, true, &m_ItemsSet, NULL, 1.0f, false );
					}
					else
					{
						AUI_TextOutFormat( pFont, m_ptViewOrg.x + rc.left + m_CharWidthBuf[offset], 
							rc.top + nLine * LineHeight + m_ptViewOrg.y + TextOffsetH,
							szText, &rc, 0, NULL, 0, false, NULL,
							NULL, NULL,
							NULL, true, m_nShadow, nAlpha, true, &m_ItemsSet );
					}
				}
				
			}
			else
			{
				if( (nLine + 1) * m_CharHeight + m_ptViewOrg.y >= 0 &&
					nLine * m_CharHeight + m_ptViewOrg.y <= rc.Height() )
				{
					pFont->DrawText(m_ptViewOrg.x + rc.left + m_CharWidthBuf[offset],
						rc.top + nLine * LineHeight + m_ptViewOrg.y + TextOffsetH,
						pHead,
						(nAlpha << 24) | (0xFFFFFF & colorText),
						&rc,
						m_nShadow ? true : false,
						A3DCOLORRGBA(0, 0, 0, m_nShadow),
						0,
						p - pHead
						);
				}
			}

			(wchar_t)*p = temp;

			if (*p == L'\0')
				break;

			if (rcSel.IsRectNull())
			{
				nLine++;
				pHead = p + 1;
			}
			else
			{				
				pHead = p;
				if(*p == L'\r')
				{
					nLine++;
					pHead++;
				}
			}
			if(*pHead == L'\n')
				pHead++;

			iCharIndexH = pHead - szText;
		}

		p++;
	}
}


void AUIEditBox::RenderSelectionTextBg(A3DFTFont* pFont, const ACHAR* szText)
{
	if (!HasAnySelection())
		return;

	int xSelBegPos, xSelEndPos;
	if (m_xSelBeginPos>m_xSelEndPos)
	{
		xSelBegPos = m_xSelEndPos;
		xSelEndPos = m_xSelBeginPos;
	}
	else
	{
		xSelBegPos = m_xSelBeginPos;
		xSelEndPos = m_xSelEndPos;
	}

	A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();
	POINT pt = GetPos();
	A3DRECT rcText = m_rcText;

	rcText.Offset(pt.x,pt.y);
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	pt.x += m_rcText.left + m_ptViewOrg.x;
	pt.y += m_rcText.top + m_ptViewOrg.y + m_nLineSpace * fWindowScale / 2 - 1;
	
	if (!m_bPassword)
	{
		int i = xSelBegPos;
		WORD y = m_CharHeightBuf[i];
		short x = m_CharWidthBuf[i];
		do{
			while (i<xSelEndPos && m_CharHeightBuf[i]==y)
				i++;
			A3DRECT rcBg;
			if (m_CharHeightBuf[i] != y)
				rcBg.SetRect(pt.x+x, pt.y+y, pt.x+m_CharWidthBuf[i-1], pt.y+y+m_CharHeight);
			else
				rcBg.SetRect(pt.x+x, pt.y+y, pt.x+m_CharWidthBuf[i], pt.y+y+m_CharHeight);
			rcBg &= rcText;
			if (!rcBg.IsEmpty())
				pFC->AddRect_2D(rcBg.left, rcBg.top, rcBg.right, rcBg.bottom, m_clSelectBg);
			x = m_CharWidthBuf[i];
			y = m_CharHeightBuf[i];
		}while (i<xSelEndPos);
	}
	else
	{
		int w = pFont->GetTextExtent(_AL("*")).x;
		A3DRECT rcBg(pt.x+w*xSelBegPos, pt.y, pt.x+w*xSelEndPos, pt.y+m_CharHeight);
		rcBg &= rcText;
		if (!rcBg.IsEmpty())
			pFC->AddRect_2D(rcBg.left, rcBg.top, rcBg.right, rcBg.bottom, m_clSelectBg);
	}
}

bool AUIEditBox::IsInHitArea(int x, int y, UINT msg)
{
	if( !AUIObject::IsInHitArea(x, y, msg) )
		return false;
	
	if( !m_bAnomaly )
		return true;
	
	if( !m_nAlpha )
		return false;
	
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( m_pAUIFrame )
	{
		DWORD color = m_pAUIFrame->GetPixel(int((x - GetPos().x) / fWindowScale), int((y - GetPos().y) / fWindowScale), m_nDefaultWidth, m_nDefaultHeight);
		if(A3DCOLOR_GETALPHA(color) > 0)
			return true;
	}
	
	return false;
}

bool AUIEditBox::IsAutoReturn(void)
{
	return m_bAutoReturn;
}

void AUIEditBox::SetIsAutoReturn(bool bAutoReturn)
{
	m_bAutoReturn = bAutoReturn;
	if( !m_bMultiline )
		m_bAutoReturn = false;
}

bool AUIEditBox::IsPassword(void)
{
	return m_bPassword;
}

void AUIEditBox::SetIsPassword(bool bPassword)
{
	m_bPassword = bPassword;
}

bool AUIEditBox::IsReadOnly(void)
{
	return m_bReadOnly;
}

void AUIEditBox::SetIsReadOnly(bool bReadOnly, bool bDisableAllMsg)
{
	m_bReadOnly = bReadOnly;
	if( bReadOnly && IsFocus() )
		m_pParent->ChangeFocus(NULL);

	if( bReadOnly && bDisableAllMsg )
		SetAcceptMouseMessage(false);
	else
		SetAcceptMouseMessage(true);
}

bool AUIEditBox::IsMultipleLine(void)
{
	return m_bMultiline;
}

void AUIEditBox::SetIsMultipleLine(bool bMultiLine, bool bWantReturn)
{
	m_bMultiline = bMultiLine;
	SetIsAutoReturn(bMultiLine);
	SetIsWantReturn(bWantReturn);
	RebuildTextRect();
}

bool AUIEditBox::IsWantReturn() const
{
	return m_bWantReturn;
}

void AUIEditBox::SetIsWantReturn(bool bWantReturn)
{
	m_bWantReturn = bWantReturn;
	if (!m_bMultiline)
		m_bWantReturn = false;
}

int AUIEditBox::GetLineSpace()
{
	return m_nLineSpace;
}

void AUIEditBox::SetLineSpace(int nSpace)
{
	nSpace>=0?m_nLineSpace = nSpace:NULL;
}

bool AUIEditBox::IsEnableSelection()
{
	return m_bAllowSelect;
}

void AUIEditBox::SetIsEnableSelection(bool bEnabled)
{
	m_bAllowSelect = bEnabled;
}

void AUIEditBox::GetSel(int& nStartChar, int& nEndChar)
{
	if (m_xSelBeginPos == m_xSelEndPos || m_xSelBeginPos == -1)
	{
		nStartChar = -1;
		nEndChar = 0;
		return;
	}

	int xSelBeginPos = a_Min(m_xSelBeginPos, m_xSelEndPos);
	int xSelEndPos = a_Max(m_xSelBeginPos, m_xSelEndPos);
	nStartChar = xSelBeginPos;
	nEndChar = xSelEndPos-1;

	if (m_bAutoReturn)
	{
		for (int i=1; i<xSelEndPos; i++)
		{
			if (m_szBuffer[i-1]==L'\r' && m_szBuffer[i]==L'\n')
			{
				if ((i-1)<=xSelBeginPos) nStartChar-=2;
				nEndChar-=2;
			}
		}
		if (m_szBuffer[xSelEndPos-1]==L'\r' && m_szBuffer[xSelEndPos]==L'\n')
			nEndChar-=1;
	}
}

void AUIEditBox::SetSel(int nStartChar, int nEndChar, bool bScrollToCaret)
{
	if (nStartChar<-1 || nEndChar<-1 ||(nStartChar>nEndChar && nEndChar!=-1) || nEndChar>=m_xEndPos || nStartChar>=m_xEndPos)
		return;

	if (nStartChar == -1)
	{
		m_xSelBeginPos = -1;
		m_xSelEndPos = 0;
	}
	else
	{
		m_xSelBeginPos = nStartChar;
		if (nEndChar == -1)
		{
			m_xSelEndPos = m_szBuffer.GetLength();
		}
		else
		{
			m_xSelEndPos = nEndChar+1;
		}
	}
	
	if (m_xSelEndPos != 0)
	{
		m_xCaretPos = m_xInsertPos = m_xSelEndPos;
		ShowCaretOnView(bScrollToCaret);
	}
}

void AUIEditBox::OnSetFocus(bool bIme)
{
}

void AUIEditBox::UpdateCaretPos()
{
	int nX = m_CharWidthBuf[m_xCaretPos] + m_ptViewOrg.x;
	int nY = m_CharHeightBuf[m_xCaretPos] + m_ptViewOrg.y;

	SetCaretPos(
		m_rcText.left + nX,
		m_rcText.top + nY
		);
}

/*****************************************************************************\
* Function: ShowCaretOnView
*
* Calculate caret position and re-display Caret
\*****************************************************************************/
void AUIEditBox::ShowCaretOnView(bool bForceShow)
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( m_pFont )
	{
		FitFont();
		m_CharHeight = m_pFont->GetFontHeight();
	}

	HideCaret();

	int nX = m_CharWidthBuf[m_xCaretPos] + m_ptViewOrg.x;

	if (nX < 0)
	{
		nX = 0;
		m_ptViewOrg.x = nX - m_CharWidthBuf[m_xCaretPos];
	}
	else if (nX > m_rcText.Width())
	{
		nX = m_rcText.Width();
		m_ptViewOrg.x = nX - m_CharWidthBuf[m_xCaretPos];
	}

	int nY = m_CharHeightBuf[m_xCaretPos] + m_ptViewOrg.y;
	int nLineH = m_CharHeight + m_nLineSpace * fWindowScale;
	if(m_bMultiline)
		if( nY < 0 )
		{
			nY = 0;
			m_ptViewOrg.y = nY - m_CharHeightBuf[m_xCaretPos];
		}
		else if( nY + nLineH > m_rcText.Height() )
		{
			nY = m_rcText.Height() - nLineH;
			m_ptViewOrg.y = nY - m_CharHeightBuf[m_xCaretPos];
		}

	SetCaretPos(
		m_rcText.left + nX,
		m_rcText.top + nY
		);

	ShowCaret();
	m_nBlink = 0;
	m_bShowCaret = false;

	// If IME property does not have IME_PROP_AT_CARET, change the position of
	// composition window

	if (!(m_dwProperty & IME_PROP_AT_CARET) && !g_bHideIme && CAN_SEND_MSG)
		SetCompositionWindowPos();
}

/*****************************************************************************\
* Function: GetWidthOfString
*
* Calculate string width in pixel 
*
* Arguments:
*	LPCTSTR szStr - string 
\*****************************************************************************/
int AUIEditBox::GetWidthOfString(const wchar_t *szStr, int* pHeight, A3DFTFont* pFont)
{
	int nWidth;
	if (pFont == NULL) pFont = m_pFont;
	if (pFont == NULL) pFont = m_pParent->GetAUIManager()->GetDefaultFont();
	A3DPOINT2 ptSize;
	int nTextW = 0, nTextH = 0;
	ptSize = pFont->GetTextExtent(szStr);
	nTextW = ptSize.x;
	nTextH = ptSize.y;
	nWidth = nTextW;
	if( pHeight ) *pHeight = nTextH;
	return nWidth;
}

/*****************************************************************************\
* Function: OnImeStartComposition
*
* Receive WM_IME_STARTCOMPOSITION message and start composition
*
* Arguments:
*	WPARAM wParam - not used
*	LPARAM lParam - not used
\*****************************************************************************/
void AUIEditBox::OnImeStartComposition(UINT wParam,LONG lParam)
{
	// Check buffer. If full, clear buffer 
	if (m_xEndPos >= (AUIEDITBOX_MAXTEXTLEN-1))	// Check buffer 
		ClearBuffer();

	ShowCaret();

	// Backup string buffer 
	m_szBackup = m_szBuffer;
	m_xInsertBackup = m_xInsertPos;
	m_xEndBackup = m_xEndPos;
}

/*****************************************************************************\
* Function: OnImeComposition
*
* Receive WM_IME_COMPOSITION message and composition string
*
* Arguments:
*	WPARAM wParam - DBCS character representing the latest change to the 
*					composition string
*	LPARAM lParam - Change flag
\*****************************************************************************/
BOOL AUIEditBox::OnImeComposition(UINT wParam,LONG lParam)
{
	HIMC	hIMC;
	int		i, nTmpInc;

	// recalc underline
	HWND hWnd = m_pA3DDevice->GetDeviceWnd();

	hIMC = ImmGetContext(hWnd);
	if (hIMC == NULL) 
		return TRUE;

	// Restore string buffer
	if (m_fStat && (m_dwProperty & IME_PROP_AT_CARET))
	{
		m_szBuffer = m_szBackup;
		m_xInsertPos = m_xCaretPos = m_xInsertBackup;
		m_xEndPos = m_xEndBackup;
		
		CalcTexWidthBuf();
	}

	m_nComSize = 0;
	
	if (lParam & GCS_RESULTSTR)
	{
		// Get result string
		memset(m_szComStr, 0, sizeof(wchar_t) * (AUIEDITBOX_MAXTEXTLEN + 1));
		m_nComSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, (LPVOID)m_szComStr, WCHARSIZE * (AUIEDITBOX_MAXTEXTLEN+1));

		if (m_nComSize >= 0)
		{
			// Insert composition string to string buffer
			nTmpInc = InsertCompStr(true);

			// Move insert/caret position and increase end position
			if(m_bAutoReturn)
			{
				for(int i=m_xInsertPos;i<m_xInsertPos+nTmpInc;i++)
					if(	m_szBuffer[i] != L'\r' && m_szBuffer[i] != L'\n')
						m_xEndPos++;
			}
			else
				m_xEndPos += nTmpInc;
			m_xInsertPos += nTmpInc;
			if (m_dwProperty & IME_PROP_AT_CARET && m_nComSize==0)
				m_xInsertPos = m_xInsertBackup;
			m_xCaretPos = m_xInsertPos;
			m_nComSize = 0;
			m_xInsertBackup = m_xInsertPos;
			m_xEndBackup = m_xEndPos;
			m_szBackup = m_szBuffer;
			if(!m_bAutoReturn)
				AUIObject::SetText(m_szBuffer);
			else
			{
				int len=m_szBuffer.GetLength();
				int j = 0;
				ACHAR tempBuffer[AUIEDITBOX_MAXTEXTLEN * 2];
				for(int i=0;i<len;i++)
					if(	m_szBuffer[i] != L'\r' ||
						i+1==m_szBuffer.GetLength() || m_szBuffer[i+1]!=L'\n')
						tempBuffer[j++] = m_szBuffer[i];
				tempBuffer[j] = '\0';
				AUIObject::SetText(tempBuffer);
			}
		}
		CalcTexWidthBuf();
		ShowCaretOnView();
	}
	if (lParam & GCS_COMPSTR)
	{
		// if IME property does not have IME_PROP_AT_CARET, ignore level 3 feature
		if (!(m_dwProperty & IME_PROP_AT_CARET) )
		{
			ImmReleaseContext(hWnd, hIMC);
			return FALSE;
		}

		// Get composition string
		memset(m_szComStr, 0, sizeof(wchar_t) * (AUIEDITBOX_MAXTEXTLEN + 1));
		m_nComSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, (LPVOID)m_szComStr, WCHARSIZE * (AUIEDITBOX_MAXTEXTLEN+1));
		if (m_nLanguage != KOREAN)
		{
			// Get composition attribute and cursor position in composition string
			if (lParam & GCS_COMPATTR)
				m_nComAttrSize = ImmGetCompositionStringW(hIMC, GCS_COMPATTR, m_bComAttr, sizeof(m_bComAttr));

			if (lParam & GCS_CURSORPOS)
				m_nComCursorPos = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, NULL, 0);

			if ((m_nLanguage == TRADITIONAL_CHINESE) || (m_nLanguage == SIMPLIFIED_CHINESE))
			{
				nTmpInc = 0;

				for (i = m_nComCursorPos; i < (int)m_nComAttrSize; i++)
				{
					if (m_bComAttr[i] == 1)
						nTmpInc++;
					else
						break;
				}
			}
 		}
		if (m_nComSize >= 0)
		{
			// Insert composition string to string buffer
			nTmpInc = InsertCompStr(false);
		
			// Move caret position
			if (m_nLanguage != KOREAN)
			{
				if (m_nLanguage == JAPANESE)
					m_xCaretPos = m_xInsertPos + m_nComSize / WCHARSIZE;
				else
					if(!m_bAutoReturn)
						m_xCaretPos = m_xInsertPos + m_nComCursorPos;
					else
					{
						if(	m_szBuffer[m_xInsertPos]==L'\r'&&
							m_xInsertPos+1<m_szBuffer.GetLength()&&
							m_szBuffer[m_xInsertPos+1]==L'\n')
							m_xInsertPos+=2;
						m_xCaretPos = m_xInsertPos;
						for(int i=0;i<m_nComCursorPos;i++)
						{
							m_xCaretPos++;
							while(	m_xCaretPos < m_szBuffer.GetLength() &&
									m_szBuffer[m_xCaretPos] == L'\r' &&
									m_xCaretPos+1 < m_szBuffer.GetLength() &&
									m_szBuffer[m_xCaretPos+1] == L'\n')
								m_xCaretPos+=2;
						}
					}
			}
		}
		CalcTexWidthBuf();
		ShowCaretOnView();
	}

	ImmReleaseContext(hWnd, hIMC);

	ShowCaretOnView();
	return TRUE;
}

/*****************************************************************************\
* Function: OnImeEndComposition
*
* Receive WM_IME_ENDCOMPOSITION message and finish composition
*
* Arguments:
*	WPARAM wParam - not used 
*	LPARAM lParam - not used
\*****************************************************************************/
void AUIEditBox::OnImeEndComposition(UINT wParam,LONG lParam)
{
	ShowCaret();
}

/*****************************************************************************\
* Function: InsertCompStr
*
* Insert composition string to string buffer
*
* Arguments:
*	WPARAM wParam - not used 
*	LPARAM lParam - not used
*
* return value:
*	Size of composition string in TCHAR 
\*****************************************************************************/
int AUIEditBox::InsertCompStr(bool bResult)
{
	int nTmpInc;
	HWND hWnd = m_pA3DDevice->GetDeviceWnd();

	int nLenOld = m_szBuffer.GetLength();//a_strlen(GetText());
	int nLenAvailable = m_nMaxLen - nLenOld;

	if (nLenAvailable < 0)
		nLenAvailable = 0;

	int nComLen = (m_nComSize + 1) / 2;

	if (bResult && nComLen > nLenAvailable)
		nComLen = nLenAvailable;

	m_szComStr[nComLen] = L'\0';
	nTmpInc = wcslen(m_szComStr);
	for(int i = 0; i < nTmpInc; i++)
		if( IsEditboxItemCode(m_szComStr[i]) )
			m_szComStr[i] = ' ';

	// Check buffer overflow
	if ((m_nComSize % 2) != 0)
		m_nComSize++;

	int nTrail = m_xEndPos - m_xInsertPos;

	if (nTrail < 0)
		nTrail = 0;
	else if (nTrail > m_szBuffer.GetLength())
		nTrail = m_szBuffer .GetLength();

	ValidInsertText(m_xInsertPos, m_szComStr, wcslen(m_szComStr));

	if(m_bAutoReturn)
	{
		int len = m_szBuffer.GetLength();
		m_xCaretPos = m_xInsertPos + nTmpInc;
		CalcTexWidthBuf();
		return m_xCaretPos - m_xInsertPos;
	}
	else
	{
		CalcTexWidthBuf();
		return nTmpInc;
	}
}

/*****************************************************************************\
* Function: SetFont
*
* Receive WM_INPUTLANGCHANGE message from IME window
* 
* This sample determines the font and code page used for Ansi-Unicode conversion
* by the keyboard layout so that you can try four different IMEs for all East 
* Asian languages without changing font nor codepage. It's not a requirement for 
* support of IME Level 3
*
* Arguments:
*	HKL  hKeyboardLayout - Specifies the character set of the new keyboard layout
*	wchar_t *szSelectedFont - Font name. 
\*****************************************************************************/
void AUIEditBox::SetFont(HKL hKL)
{
	switch (LOWORD(hKL))
	{
	// Traditional Chinese
	case LID_TRADITIONAL_CHINESE:
		m_nLanguage = TRADITIONAL_CHINESE;
   		break;

	// Japanese
	case LID_JAPANESE:
		m_nLanguage = JAPANESE;
		break;

	// Korean
	case LID_KOREAN:
		m_nLanguage = KOREAN;
		break;

	// Simplified Chinese
	case LID_SIMPLIFIED_CHINESE:
		m_nLanguage = SIMPLIFIED_CHINESE;
		break;

	default:
		m_nLanguage = DEFAULT;
		break;
	}

	bool bIme = (ImmIsIME(hKL) != FALSE);

	if (bIme)
		get_ime_name(hKL);
	else
		reset_ime_name();

	// Get IME property
	m_dwProperty = ImmGetProperty(hKL, IGP_PROPERTY);
	m_fStat = FALSE;
	m_nComSize = 0;
	ShowCaretOnView();
}

/*****************************************************************************\
* Function: GetCombinedCharLength
*
* Get the length of combined character. 
*
* This implementation only support Surrogate. You need to handle Combined 
* characters as necessary.  
*
* return value:
*	If UNICIDE, 
*		2 if surrogate char, else return 1
*	If not UNICODE
*		2 if DBCS char, else return 1
\*****************************************************************************/
int AUIEditBox::GetCombinedCharLength(int nTmpPos) 
{
	int	i, nRet = 1;

	// Check surrogate char
	for (i = 0; i <= nTmpPos; i++)
	{
		if ((0xD800 <= m_szBuffer[i]) && (m_szBuffer[i] <= 0xDBFF))
		{
			nRet = 2;
			++i;
		}
		else
			nRet = 1;
	}

	return nRet;
}

/*****************************************************************************\
* Function: ClearBuffer
*
* Clear string buffer
\*****************************************************************************/
void AUIEditBox::ClearBuffer()
{
	m_szBuffer = L"";
	m_xEndPos = m_xInsertPos = m_xCaretPos = 0;
	m_nComSize = 0;
	
}


/*****************************************************************************\
* Function: SetCandiDateWindowPos
*
* Set candidate window position for Japanese/Korean IME
\*****************************************************************************/
void AUIEditBox::SetCandiDateWindowPos() 
{
	HIMC		hIMC;
	CANDIDATEFORM Candidate;
	HWND hWnd = m_pA3DDevice->GetDeviceWnd();

	if (hIMC = ImmGetContext(hWnd)) 
	{
		Candidate.dwIndex = 0;
		Candidate.dwStyle = CFS_POINT;

		if (m_nLanguage == JAPANESE) 
		{
			// Set candidate window position near editable character
			wchar_t *szTmpStr = (wchar_t*)alloca((m_xInsertPos + m_nComCursorPos + 1) * sizeof(wchar_t));//new(wchar_t[m_xInsertPos + m_nComCursorPos + 1]);

			memcpy(szTmpStr, (const wchar_t *)m_szBuffer, WCHARSIZE * (m_xInsertPos + m_nComCursorPos));	 
			szTmpStr[m_xInsertPos + m_nComCursorPos] = L'\0';
			Candidate.ptCurrentPos.x = GetWidthOfString(szTmpStr);
		}
		else
		{
			// Set candidate window position near caret position
			Candidate.ptCurrentPos.x = m_ptCaret.x;
		}
		Candidate.ptCurrentPos.y = m_CharHeight + 1;
		POINT pt = GetPos();
		Candidate.ptCurrentPos.x += pt.x;
		Candidate.ptCurrentPos.y += pt.y;
		ImmSetCandidateWindow(hIMC, &Candidate);
		ImmReleaseContext(hWnd,hIMC);
	}
}

/*****************************************************************************\
* Function: SetCompositionWindowPos
*
* Set composition window position for Traditional Chinese IME
\*****************************************************************************/
void AUIEditBox::SetCompositionWindowPos() 
{
	HIMC		hIMC;
    COMPOSITIONFORM Composition;
	HWND hWnd = m_pA3DDevice->GetDeviceWnd();

	if (hIMC = ImmGetContext(hWnd)) 
	{
		// Set composition window position near caret position
		Composition.dwStyle = CFS_POINT;
		Composition.ptCurrentPos.x = m_ptCaret.x;
		Composition.ptCurrentPos.y = m_ptCaret.y;
		POINT pt = GetPos();
		Composition.ptCurrentPos.x += m_rcText.left + m_nOffX + pt.x;
		Composition.ptCurrentPos.y += m_rcText.top + m_nOffY + pt.y;
		ImmSetCompositionWindow(hIMC, &Composition);
		ImmReleaseContext(hWnd,hIMC);
	}
}

void AUIEditBox::DisplayCaret()
{
	if( m_nCaretRef <= 0 || !m_bFocus || !m_pParent->IsActive() || HasAnySelection()) return;

	DWORD dwTick = GetTickCount();
	if( dwTick - m_nBlink >= AUIEDITBOX_BLINK_DELAY )
	{
		m_nBlink = dwTick;
		m_bShowCaret = !m_bShowCaret;
	}

	if( m_bShowCaret )
	{
		A3DPOINT2 ptEnd;
		POINT pt = GetPos();
		A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();
		
		int nYOff = m_CharHeight / 8;

		A3DRECT rcCaret(m_ptCaret.x,
			nYOff + m_ptCaret.y,
			m_ptCaret.x + 1, 
			nYOff + m_ptCaret.y + (m_CharHeight > 0 ? m_CharHeight : CARET_HEIGHT));

		if( rcCaret.top < m_rcText.top )
			rcCaret.top = m_rcText.top;
		if( rcCaret.bottom > m_rcText.bottom)
			rcCaret.bottom = m_rcText.bottom;
		if( rcCaret.Height() > 0)
		{
			rcCaret.Offset(pt.x + m_nOffX, pt.y + m_nOffY);
			pFC->AddRect_2D(rcCaret.left, rcCaret.top, rcCaret.right, rcCaret.bottom, m_color);
		}
	}
}

void AUIEditBox::SetCaretPos(int x, int y)
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	m_ptCaret.x = x;
	m_ptCaret.y = y + m_nLineSpace*fWindowScale/2;
	POINT pt = GetPos();
	pt.x += m_rcText.left + m_nOffX;
	pt.y += m_rcText.top + m_nOffY;
	::SetCaretPos(x + pt.x, y + pt.y + m_CharHeight);
}

void AUIEditBox::DrawUnderline(A3DPOINT2 ptStart, int nWidth)
{
	const int nInterval = 6;
	const int nHalf = nInterval / 2;
	A3DPOINT2 ptEnd;
	A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();

	while (nWidth > 0)
	{
		if (nWidth <= nInterval) 
		{
			ptEnd.Set(ptStart.x + ((nWidth < nHalf) ? nWidth : nHalf), ptStart.y + 1);
			pFC->AddRect_2D(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y, 0xff00ff00);
			nWidth = 0;
		}
		else 
		{
			ptEnd.Set(ptStart.x + nHalf, ptStart.y + 1);
			pFC->AddRect_2D(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y, 0xff00ff00);
			ptStart.x += nInterval;
			nWidth -= nInterval;
		}
	}
}

void AUIEditBox::SetFocus(bool bFocus)
{
	if (bFocus == m_bFocus)
		return;

	AUIObject::SetFocus(bFocus);
	HWND hHostWnd = m_pA3DDevice->GetDeviceWnd();

	if (bFocus)
	{
		_ime_toggle_flag = CanChangeIme();
		HKL hKL = GetKeyboardLayout(_ime_thread_id);
		SetFont(hKL);

		// Set Caret position
		ShowCaretOnView();
		ShowCaret();
		_ime_toggle_flag = CanChangeIme();
		
		if((g_bHideIme))
		{
			IMEWrapper::SetHWnd(hHostWnd);
			//IMEWrapper::OnFocusIn(_ime_toggle_flag);
		}

	}
	else
	{
		IMEWrapper::OnFocusOut();
		BOOL bCanSendMsg = CAN_SEND_MSG;

		// If composition is not finished, complete the composition string
		if (bCanSendMsg && m_fStat && (m_nLanguage != KOREAN))
		{
			HIMC hIMC = ImmGetContext(hHostWnd);

			if (hIMC)
			{
				ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
				ImmReleaseContext(hHostWnd, hIMC);
			}
		}

		m_fStat = FALSE;

		if (g_bHideIme)
			m_ime_var.Clear();

		HideCaret();
		_ime_toggle_flag = false;
	}
}

void AUIEditBox::OnChar(UINT nChar)
{
	int		nTmpDec;
	wchar_t wChar/*, szTmpStr[AUIEDITBOX_MAXTEXTLEN+1]*/;
	
	if (HandleAccelKey(nChar))
	{
		CalcTexWidthBuf();
		ShowCaretOnView();
		return;
	}

	if (m_bReadOnly)
		return;

	switch(nChar)
	{
	// Back space key
	case ACHAR('\b') :
		// Delete one char
		if(m_xInsertPos > m_xEndPos) break;

		if (IsOpOnSelection())
		{
			// Delete Selection Text
			DeleteText(a_Min(m_xSelBeginPos, m_xSelEndPos), a_Max(m_xSelBeginPos, m_xSelEndPos));
			CleanSelection();
		}
		else
		{
			// Delete one char
			if(m_xInsertPos == 0)
				return;

			if( m_bAutoReturn &&
				m_szBuffer[m_xInsertPos-1]==L'\n')
				nTmpDec = GetCombinedCharLength(m_xInsertPos-3)+2;
			else
				nTmpDec = GetCombinedCharLength(m_xInsertPos-1);

			DeleteText(m_xInsertPos-nTmpDec, m_xInsertPos);
		}
		break;

	default :
		if( nChar == ACHAR('\r') && !m_bWantReturn )
		{
			//m_pParent->SetCommand(m_strCommand);
			return;
		}
		if (nChar < TCHAR(0x20) && nChar != ACHAR('\r'))
			break;
		if( m_bNumber && (nChar < '0' || nChar > '9') )
			break;
#ifdef UNICODE
		wChar = nChar;
#else
		MultiByteToWideChar(m_nCodePage, 0, (char *)&nChar, 1, &wChar, 2);
#endif
		
		if( IsEditboxItemCode(wChar) )
			wChar = ' ';

		// Check buffer overflow
		if( m_xEndPos > (AUIEDITBOX_MAXTEXTLEN-1))
			ClearBuffer();

		// delete selection text
		if (IsOpOnSelection())
		{
			// Delete Selection Text
			DeleteText(a_Min(m_xSelBeginPos, m_xSelEndPos), a_Max(m_xSelBeginPos, m_xSelEndPos));
			CleanSelection();
		}

		// insert char to string buffer
		if (ValidInsertText(m_xInsertPos, &wChar, 1) != 0)
		{
			m_xEndPos++;
			m_xInsertPos++;
			m_xCaretPos++;
		}
		
		//m_szBuffer[m_xEndPos] = L'\0';
		break;
	}

	CalcTexWidthBuf();
	if (nChar == '\r')
	{
		int nFontW = (m_pFont ? m_pFont->GetFontWidth() : m_pParent->GetAUIManager()->GetDefaultFont()->GetFontWidth());
		if (m_nAlign == AUIEDITBOX_ALIGN_CENTER)
			m_ptViewOrg.x = (m_rcText.Width() - nFontW) / 2 - m_rcText.left - m_CharWidthBuf[m_xCaretPos];
	}

	ShowCaretOnView();
}

void AUIEditBox::OnKeyDown(UINT nChar) 
{
	if (g_bHideIme)
		GetComCanVal(&m_ime_var, FALSE);
	
	// Do not accept char during composition if IME property does not have IME_PROP_AT_CARET
	if (!(m_dwProperty & IME_PROP_AT_CARET) && m_fStat)
		return;

	MoveCaretByKey(nChar);

	if (nChar == VK_RETURN && !m_bWantReturn)
		if( m_strCommand != "" )
			m_pParent->SetCommand(m_strCommand);
		else
			m_pParent->SetCommand(m_szName);
}

void AUIEditBox::MoveCaretByKey(UINT uKey)
{
	int nTmpDec;

	if( uKey!=VK_UP && uKey!= VK_DOWN)
		m_nCaretWidthBuf = -1;

	int xPosBeforeMove = m_xInsertPos;
	bool bDirectionKey = true;
	
	switch(uKey)
	{
	// Left arrow key
	case VK_LEFT:
		if(m_xInsertPos <= 0) 
			break;
		if(m_bAutoReturn&&m_szBuffer[m_xInsertPos-1]==L'\n')
			m_xInsertPos -= GetCombinedCharLength(m_xInsertPos-2)+1;
		else
			m_xInsertPos -= GetCombinedCharLength(m_xInsertPos-1);
		m_xCaretPos = m_xInsertPos;
		ShowCaretOnView(TRUE);
		break;

	// Right arrow key
	case VK_RIGHT:
		if(m_xInsertPos >= m_xEndPos) break;
		m_xInsertPos += GetCombinedCharLength(m_xInsertPos);
		if(	m_bAutoReturn &&
			m_xInsertPos+1<m_szBuffer.GetLength() && m_szBuffer[m_xInsertPos]==L'\n')
			m_xInsertPos++;
		m_xCaretPos = m_xInsertPos;
		ShowCaretOnView(TRUE);
		break;

	// Delete key
	case VK_DELETE:
		if (IsOpOnSelection())
		{
			// Delete Selection Text
			DeleteText(a_Min(m_xSelBeginPos, m_xSelEndPos), a_Max(m_xSelBeginPos, m_xSelEndPos));
			CleanSelection();
			CalcTexWidthBuf();
		}
		else
		{
			if(m_xInsertPos >= m_xEndPos) break;
			if( m_bAutoReturn && m_xInsertPos < m_xEndPos-2 &&
				m_szBuffer[m_xInsertPos]==L'\r' && m_xInsertPos+1<m_szBuffer.GetLength() &&
				m_szBuffer[m_xInsertPos+1]==L'\n')
				nTmpDec = GetCombinedCharLength(m_xInsertPos+2)+2;
			else
				nTmpDec = GetCombinedCharLength(m_xInsertPos);
			
			DeleteText(m_xInsertPos, m_xInsertPos+nTmpDec);
			CalcTexWidthBuf();
		}
		ShowCaretOnView();
		break;

	// Home key
	case VK_HOME: {
		if (m_xInsertPos == 0) break;
		if( (m_bMultiline && AUI_PRESS(VK_CONTROL)) || m_bPassword )
			m_xInsertPos = 0;
		else
		{
			do {
				m_xInsertPos--;
				if (m_szBuffer[m_xInsertPos] == L'\r')
				{
					m_xInsertPos++;
					break;
				}
			} while (m_xInsertPos > 0);
		}
		m_xCaretPos = m_xInsertPos;
		ShowCaretOnView(TRUE);
		break; }

	// End key
	case VK_END:
		if (m_xInsertPos == m_xEndPos) break;
		if( (m_bMultiline && AUI_PRESS(VK_CONTROL)) || m_bPassword)
			m_xInsertPos = m_xEndPos;
		else
		{
			while (m_xInsertPos < m_xEndPos)
			{
				if (m_szBuffer[m_xInsertPos] == L'\r')
					break;
				m_xInsertPos++;
			} 
		}
		m_xCaretPos = m_xInsertPos;
		ShowCaretOnView(TRUE);
		break;
	case VK_UP:
		{
			if( m_xInsertPos == 0 || m_bPassword)
				break;
			int i = m_xInsertPos - 1;
			while ( i >= 0 && m_szBuffer[i] != L'\r') 
				i--;
			if( i < 0 )
				break;
			if(m_nCaretWidthBuf < 0)
				m_nCaretWidthBuf = m_CharWidthBuf[m_xInsertPos];
			i--;
			while ( i >= 0 && m_szBuffer[i] != L'\r' && m_szBuffer[i] != L'\n') 
				i--;
			i++;
			int j = i;
			while( m_szBuffer[i] != L'\r' &&
				m_CharWidthBuf[i] - m_CharWidthBuf[j] < m_nCaretWidthBuf)
				i++;
			m_xCaretPos = m_xInsertPos = i;
			ShowCaretOnView(TRUE);
	   }
		break; 
	case VK_DOWN:
		{
			if (m_bPassword)
				break;

			int i = m_xInsertPos;
			int len=m_szBuffer.GetLength();
			while ( i < len && m_szBuffer[i] != L'\r') 
				i++;
			if(i >= len)
				break;
			if(m_nCaretWidthBuf < 0)
				m_nCaretWidthBuf = m_CharWidthBuf[m_xInsertPos];
			i++;
			int j = i;
			while( i < len && m_szBuffer[i] != L'\r' && 
					m_CharWidthBuf[i] - m_CharWidthBuf[j] < m_nCaretWidthBuf)
				i++;
			m_xCaretPos = m_xInsertPos = i;
			ShowCaretOnView(TRUE);
		}
		break; 

	default:
		bDirectionKey = false;
		break;
	}

	if (bDirectionKey)
	{	
		if (AUI_PRESS(VK_SHIFT))
		{
			if (!HasAnySelection())
			{
				m_xSelBeginPos = xPosBeforeMove;
				m_xSelEndPos = m_xInsertPos;
			}
			else
			{
				m_xSelEndPos = m_xInsertPos;
			}
		}
		else
		{
			CleanSelection();
		}
	}
}

bool AUIEditBox::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Is Password") )
		Property->b = m_bPassword;
	else if( 0 == strcmpi(pszPropertyName, "Is ReadOnly") )
		Property->b = m_bReadOnly;
	else if( 0 == strcmpi(pszPropertyName, "Is Number Only") )
		Property->b = m_bNumber;
	else if( 0 == strcmpi(pszPropertyName, "Is Multiple Line") )
		Property->b = m_bMultiline;
	else if( 0 == strcmpi(pszPropertyName, "Is Auto Return") )
		Property->b = m_bAutoReturn;
	else if( 0 == strcmpi(pszPropertyName, "Is Want Return") )
		Property->b = m_bWantReturn;
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pAUIFrame )
			strncpy(Property->fn, m_pAUIFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
	{
		if( m_pA2DSprite[AUIEDITBOX_RECT_UP] )
			strncpy(Property->fn, m_pA2DSprite[AUIEDITBOX_RECT_UP]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
	{
		if( m_pA2DSprite[AUIEDITBOX_RECT_DOWN] )
			strncpy(Property->fn, m_pA2DSprite[AUIEDITBOX_RECT_DOWN]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
	{
		if( m_pA2DSprite[AUIEDITBOX_RECT_SCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUIEDITBOX_RECT_SCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
	{
		if( m_pA2DSprite[AUIEDITBOX_RECT_BAR] )
			strncpy(Property->fn, m_pA2DSprite[AUIEDITBOX_RECT_BAR]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Text Color") )
		Property->dw = m_color;
	else if( 0 == strcmpi(pszPropertyName, "Font Name") )
		a_strncpy(Property->s, m_szFontName, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Font Height") )
		Property->i = m_nFontHeight;
	else if( 0 == strcmpi(pszPropertyName, "Max Text Length") )
		Property->i = m_nMaxLen;
	else if (0 == strcmpi(pszPropertyName, "Line Space"))
		Property->i = m_nLineSpace;
	else if (0 == strcmpi(pszPropertyName, "Enable Selection"))
		Property->b = m_bAllowSelect;
	else if (0 == strcmpi(pszPropertyName, "Selection Background Color"))
		Property->dw = m_clSelectBg;
	else if (0 == strcmpi(pszPropertyName, "Text Align"))
		Property->i = m_nAlign;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIEditBox::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Width") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrame )
		{
			SIZE s = m_pAUIFrame->GetSizeLimit();
			m_nWidth = max(m_nWidth, s.cx);
			RebuildTextRect();
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "Height") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrame )
		{
			SIZE s = m_pAUIFrame->GetSizeLimit();
			m_nHeight = max(m_nHeight, s.cy);
			RebuildTextRect();
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "Is Password") )
		SetIsPassword(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Is ReadOnly") )
		SetIsReadOnly(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Is Number Only") )
		SetIsNumberOnly(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Is Multiple Line") )
		SetIsMultipleLine(Property->b, m_bWantReturn);
	else if( 0 == strcmpi(pszPropertyName, "Is Auto Return") )
		SetIsAutoReturn(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Is Want Return") )
		SetIsWantReturn(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUIEDITBOX_RECT_FRAME);
	else if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
		return InitIndirect(Property->fn, AUIEDITBOX_RECT_UP);
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
		return InitIndirect(Property->fn, AUIEDITBOX_RECT_DOWN);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
		return InitIndirect(Property->fn, AUIEDITBOX_RECT_SCROLL);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
		return InitIndirect(Property->fn, AUIEDITBOX_RECT_BAR);
	else if( 0 == strcmpi(pszPropertyName, "Text Color") )
		return SetFontAndColor(m_szFontName, m_nFontHeight, Property->dw, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType);
	else if( 0 == strcmpi(pszPropertyName, "Font Name") )
		return SetFontAndColor(Property->s, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType);
	else if( 0 == strcmpi(pszPropertyName, "Font Height") )
		return SetFontAndColor(m_szFontName, Property->i, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType);
	else if( 0 == strcmpi(pszPropertyName, "Max Text Length") )
		SetMaxLength(Property->i);
	else if (0 == strcmpi(pszPropertyName, "Line Space"))
		SetLineSpace(Property->i);
	else if (0 == strcmpi(pszPropertyName, "Enable Selection"))
		SetIsEnableSelection(Property->b);
	else if (0 == strcmpi(pszPropertyName, "Selection Background Color"))
		m_clSelectBg = Property->dw;
	else if (0 == strcmpi(pszPropertyName, "Text Align"))
		m_nAlign = Property->i;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIEditBox::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Is Password", AUIOBJECT_PROPERTY_VALUE_BOOL) &&
			SetAUIObjectPropertyType("Is ReadOnly", AUIOBJECT_PROPERTY_VALUE_BOOL) &&
			SetAUIObjectPropertyType("Is Multiple Line", AUIOBJECT_PROPERTY_VALUE_BOOL) &&
			SetAUIObjectPropertyType("Is Want Return", AUIOBJECT_PROPERTY_VALUE_BOOL) &&
			SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Text Color", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Font Name", AUIOBJECT_PROPERTY_VALUE_ACHAR) &&
			SetAUIObjectPropertyType("Font Height", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Max Text Length", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Line Space", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Enable Selection", AUIOBJECT_PROPERTY_VALUE_BOOL) &&
			SetAUIObjectPropertyType("Text Align", AUIOBJECT_PROPERTY_VALUE_INT);
}

void AUIEditBox::SetMaxLength(int nMaxLen)
{
	ASSERT(nMaxLen > 0);
	m_nMaxLen = nMaxLen;
	if (m_nMaxLen < 0 || m_nMaxLen > AUIEDITBOX_MAXTEXTLEN )
		m_nMaxLen = AUIEDITBOX_MAXTEXTLEN;
	m_nMaxLenOrg = m_nMaxLen;
}

int AUIEditBox::GetMaxLength()
{
	return m_nMaxLen;
}

void GetComCanValX(ime_str_val* pVal)
{
	__try
	{
#ifdef UNICODE
		pVal->append_com_wstr(ImeUi_GetCompositionString(),wcslen(ImeUi_GetCompositionString()));
		for(int i=0;i<ImeUi_GetPageCount();i++)
		{
			if((ImeUi_GetCandidate(i)[0]!=0)&&(wcslen(ImeUi_GetCandidate(i))!=0))
			{
				pVal->push_can_wstr(ImeUi_GetCandidate(i),wcslen(ImeUi_GetCandidate(i)));
			}
			else
				break;

		}

#else
		pVal->append_com_str(ImeUi_GetCompositionString(),strlen(ImeUi_GetCompositionString()));
		for(int i=0;i<ImeUi_GetCandidateCount();i++)
		{
			pVal->push_can_str(ImeUi_GetCandidate(i),strlen(ImeUi_GetCandidate(i)));
		}
#endif
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		//		pVal->Clear();
		//		g_bHideIme = false;
	}
}

void AUIEditBox::RenderImeWnd()
{
	BOOL bHasNum = false;
	ime_str_val *pImevar;
	ime_str_val ImeTmp;
	pImevar = &m_ime_var;

	if (m_ime_var.get_com_str() == NULL && m_ime_var.get_size() == 0)
	{
		pImevar = &ImeTmp;
		GetComCanValX(pImevar);
		int x = pImevar->get_size();
		LPWSTR str = pImevar->get_com_str();
		bHasNum = true;
		if(((pImevar->get_com_str() == NULL )||(wcslen(pImevar->get_com_str())==0)) && pImevar->get_size() == 0)
			return;
	}

	m_pA3DEngine->FlushInternalSpriteBuffer();
	m_pParent->GetAUIManager()->GetA3DFTFontMan()->Flush();

	A3DFTFont* pFont = m_pParent->GetAUIManager()->GetDefaultFont();
	const int nOff = 4;
	int nFontHeight;
	int nSequWidth = GetWidthOfString(L"1.", &nFontHeight, pFont);
	int nWndHeight = (nFontHeight + nOff) * ((pImevar->get_com_str() ? 2 : 1) + pImevar->get_size());
	int nMaxWidth = pImevar->get_com_str() ? GetWidthOfString(pImevar->get_com_str(), NULL, pFont) : 0;
	int nLen2 = 0;
	if(wcslen(_xime_name)>0)
		nLen2 = GetWidthOfString(_xime_name, NULL, pFont);
	else
		nLen2 = GetWidthOfString(_ime_name, NULL, pFont);
	if (nLen2 > nMaxWidth) nMaxWidth = nLen2;
	int i;

	for (i = 0; i < pImevar->get_size(); i++)
	{
		int n = GetWidthOfString(pImevar->get_can_str(i), NULL, pFont);
		n += nSequWidth;
		if (n > nMaxWidth)
			nMaxWidth = n;
	}

	POINT pt = GetPos();
	pt.x += m_rcText.left + m_nOffX;
	pt.y += m_rcText.top + m_nOffY;

	int nX = m_ptCaret.x + pt.x;
	if (nX + nMaxWidth > m_nVPW + m_nOffX)
		nX -= nMaxWidth;
		
	int nY = m_ptCaret.y + nFontHeight + nOff + pt.y;
	if (nY + nWndHeight > m_nVPH + m_nOffY)
		nY -= nWndHeight + nFontHeight;

	A3DRECT rc(	nX,
				nY,
				nX + nMaxWidth + 5,
				nY + nWndHeight);

	g_pA3DGDI->Draw2DRectangle(rc, A3DCOLORRGBA(0, 0, 0, 196));

	A3DRECT rc2( nX,
				 nY,
				 nX + nMaxWidth + 5,
				 nY + nFontHeight);

	g_pA3DGDI->Draw2DRectangle(rc2, A3DCOLORRGBA(0, 0, 128, 196));

	if(wcslen(_xime_name)>0)
		pFont->TextOut(nX + nOff, nY, _xime_name, A3DCOLORRGB(255, 255, 255));
	else
		pFont->TextOut(nX + nOff, nY, _ime_name, A3DCOLORRGB(255, 255, 255));

	nY += nFontHeight;

	if (pImevar->get_com_str())
		pFont->TextOut(nX + nOff, nY, pImevar->get_com_str(), A3DCOLORRGB(255, 255, 255));
	else
		nY -= nFontHeight + nOff;

	for (i = 0; i < pImevar->get_size(); i++)
	{
		nY += nFontHeight + nOff;
		if(bHasNum)
			pFont->TextOut(nX + nOff, nY, pImevar->get_can_str(i), A3DCOLORRGB(255, 255, 255));
		else
			pFont->TextOut(nX + nOff, nY, ACString().Format(L"%d.", i+1 > 9 ? 0 : i+1) + pImevar->get_can_str(i), A3DCOLORRGB(255, 255, 255));
	}

	pFont->Flush();
}

void AUIEditBox::SetHWnd(HWND hWnd)
{
	static bool s_bFirst = true;
	if (s_bFirst)
	{
		s_bFirst = false;
		_ime_hwnd = hWnd;
		_ime_thread_id = ::GetWindowThreadProcessId(_ime_hwnd, NULL);
		IMEWrapper::Initialize(hWnd);	
		IMEWrapper::SetHWnd(hWnd);
	}
}

bool AUIEditBox::IsNumberOnly()
{
	return m_bNumber;
}

void AUIEditBox::SetIsNumberOnly(bool bNumber)
{
	m_bNumber = bNumber;
}

bool AUIEditBox::GetCurrentIMEState()
{
	HKL hKL = GetKeyboardLayout(_ime_thread_id);
	return (ImmIsIME(hKL) != FALSE);
}

const ACHAR * AUIEditBox::GetCurrentIMEName()
{
//	HKL hKL = GetKeyboardLayout(_ime_thread_id);
//
//	if (ImmIsIME(hKL))
//		get_ime_name(hKL);
//	else
//		reset_ime_name();
//
	return _ime_name;
}

bool AUIEditBox::IsIMEAlphaNumeric()
{
	return wcscmp(_ime_name, DEFAULT_IME_NAME) == 0;
//	if (!ImmIsIME(GetKeyboardLayout(_ime_thread_id)))
//		return true;
//
//	if (_ime_hwnd == NULL) return true;
//	bool bRet;
//	HIMC hIMC = ImmGetContext(_ime_hwnd);
//	if (hIMC == NULL) return true;
//	DWORD dwConv, dwSent;
//	ImmGetConversionStatus(hIMC, &dwConv, &dwSent);
//	bRet = !(dwConv & IME_CMODE_NATIVE);
//	ImmReleaseContext(_ime_hwnd, hIMC);
//	return bRet;
}

void AUIEditBox::ToggleIME()
{
	if (_ime_toggle_flag == _ime_toggle_old_flag)
		return;

	_ime_toggle_old_flag = _ime_toggle_flag;
	HIMC hIMC = ImmGetContext(_ime_hwnd);

	if (_ime_toggle_flag)
	{
		if (!hIMC && _cur_imc)
			ImmAssociateContext(_ime_hwnd, _cur_imc);
		IMEWrapper::EnableImeSystem(true);
	}
	else if (hIMC)
	{
		_cur_imc = hIMC;
		ImmAssociateContext(_ime_hwnd, NULL);
		IMEWrapper::EnableImeSystem(false);
	}

	if (hIMC)
		ImmReleaseContext(_ime_hwnd, hIMC);
}

void AUIEditBox::SetEmotionList(A2DSprite **pA2DSpriteEmotion,
	abase::vector<AUITEXTAREA_EMOTION> **pvecEmotion)
{
	m_pA2DSpriteEmotion = pA2DSpriteEmotion;
	m_vecEmotion = pvecEmotion;
}

bool AUIEditBox::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());

	m_CharHeight = GetFont()->GetFontHeight();

	RebuildTextRect();
	CalcTexWidthBuf();
	ShowCaretOnView();

	return true;
}

bool AUIEditBox::AppendItem(EditboxItemType type, A3DCOLOR cl, const ACHAR* szName, const ACHAR* szInfo, const void* pExtraData, size_t sz)
{
	if (!InsertItem(m_xInsertPos, type, cl, szName, szInfo, pExtraData, sz))
		return false;

	m_xEndPos++;
	m_xInsertPos++;
	m_xCaretPos++;
	CalcTexWidthBuf();
	ShowCaretOnView();
	return true;
}

void AUIEditBox::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIEditBox::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	for (int i = 0; i < AUIEDITBOX_RECT_MAX; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);
}

// endPos is the the next of last delete char
void AUIEditBox::DeleteText(int iStartPos, int iEndPos)
{
	if (iStartPos>=0 && iEndPos<=m_xEndPos && iStartPos<iEndPos)
	{
		for (int i=iStartPos; i<iEndPos; i++)
		{
			if (IsEditboxItemCode(m_szBuffer[i]))
			{
				EditBoxItemBase* pItem = m_ItemsSet.GetItemByChar(m_szBuffer[i]);
				if (pItem)
				{
					if (!m_bMaxLenRendered)
						m_nMaxLen += pItem->Serialize().GetLength();
					
					if (pItem->GetType() == enumEIIvtrlItem)
						m_bHasItem = false;
				}
				
				m_ItemsSet.DelItemByChar(m_szBuffer[i]);
			}
		}
		m_szBuffer = m_szBuffer.Left(iStartPos) + m_szBuffer.Right(m_xEndPos-iEndPos);

		int nDeletePos = iEndPos-iStartPos;
		m_xEndPos -= nDeletePos;
		m_xCaretPos = iStartPos;
		m_xInsertPos = iStartPos;
			
		//光标删除到最左端时，如果当前行左边还有文本，则m_ptViewOrg自动向左偏移（最多）AUIEDITBOX_DEL_PREVNUM个字符宽度
		if (m_nAlign == AUIEDITBOX_ALIGN_LEFT && m_CharWidthBuf[iStartPos] + m_ptViewOrg.x  <= m_nOffX) 			
		{
			int nOffsetX = m_nOffX - (m_CharWidthBuf[iStartPos] + m_ptViewOrg.x);
			for (int i = 0; i < AUIEDITBOX_DEL_PREVNUM && i < iStartPos; i++)
			{
				if (m_CharWidthBuf[iStartPos-i] <= m_CharWidthBuf[iStartPos-i-1])
					break;
				nOffsetX += (m_CharWidthBuf[iStartPos-i] - m_CharWidthBuf[iStartPos-i-1]);	
			}
			m_ptViewOrg.x += nOffsetX;
		}
		else if (m_nAlign == AUIEDITBOX_ALIGN_RIGHT && 
				iStartPos < m_xEndPos &&
				m_CharWidthBuf[iStartPos]  < m_CharWidthBuf[iStartPos+1] &&
				m_CharWidthBuf[iStartPos+1] + m_ptViewOrg.x  >= m_nOffX + m_rcText.Width())
		{
			int nOffsetX = m_CharWidthBuf[iStartPos+1] + m_ptViewOrg.x - (m_nOffX + m_rcText.Width());
			for (int i = 0; i < AUIEDITBOX_DEL_PREVNUM && i < m_xEndPos; i++)
			{
				if (m_CharWidthBuf[iStartPos+i] >= m_CharWidthBuf[iStartPos+i+1])
					break;
				nOffsetX += m_CharWidthBuf[iStartPos+i+1] - m_CharWidthBuf[iStartPos+i];
			}
			m_ptViewOrg.x -= nOffsetX;
		}
				
		
	}
}

void AUIEditBox::CleanSelection()
{
	m_bSelecting = false;
	m_xSelBeginPos = -1;
	m_xSelEndPos = 0;
}

bool AUIEditBox::HandleAccelKey(UINT nChar)
{
	bool bHandled = true;
	switch(nChar)
	{
	case 0x01: // ctrl+a
		SetSel(0, -1, true);
		break;
	case 0x16: // ctrl+v
		if (m_bReadOnly)
			return bHandled;

		if( OpenClipboard(m_pA3DDevice->GetDeviceWnd()) )
		{
#ifdef UNICODE
			HANDLE hHandle = GetClipboardData(CF_UNICODETEXT);
#else
			HANDLE hHandle = GetClipboardData(CF_TEXT);
#endif
			if( hHandle )
			{
				if (IsOpOnSelection())
					DeleteText(a_Min(m_xSelBeginPos, m_xSelEndPos), a_Max(m_xSelBeginPos, m_xSelEndPos));

				ACHAR *pszText = (ACHAR *)GlobalLock(hHandle);
				int nLen = a_strlen(pszText);
				if( pszText && (m_nMaxLen < 0 || m_szBuffer.GetLength() < m_nMaxLen) )
				{
					ACString strText = pszText;

					if (IsNumberOnly())
					{
						// skip non-number
						valid_number_str(strText);
					}
					else
					{
						for(int i = 0; i < strText.GetLength(); i++)
						{
							if( IsEditboxItemCode(strText[i]) )
								strText[i] = ' ';
						}
					}
					
					int nAddedLen = ValidInsertText(m_xInsertPos, strText, strText.GetLength());
					if( !m_bMultiline )
					{
						nLen = strText.GetLength();
						nLen = nLen + m_xInsertPos;
						if(nLen > m_szBuffer.GetLength())
							nLen = m_szBuffer.GetLength();
						for(int i = m_xInsertPos; i < nLen; i++ )
							if( m_szBuffer[i] == '\r' || m_szBuffer[i] == '\n' )
								m_szBuffer[i] = ' ';
					}
					m_xEndPos += nAddedLen;
					m_xInsertPos += nAddedLen;
					m_xCaretPos += nAddedLen;
					m_szText = m_szBuffer;
				}					
				
				GlobalUnlock(hHandle);
			}
			
			CloseClipboard();
			CleanSelection();
		}
		break;
		
	case 0x18: // ctrl+x
	case 0x03: // ctrl+c 
		if( IsOpOnSelection() && OpenClipboard(m_pA3DDevice->GetDeviceWnd()) )
		{
			int iStartChar = a_Min(m_xSelBeginPos, m_xSelEndPos);
			int iLen = a_Max(m_xSelBeginPos, m_xSelEndPos)-iStartChar;
			HANDLE hHandle = LocalAlloc(LPTR, (iLen+1) * sizeof(m_szBuffer[0]));
			if( !m_bPassword && hHandle )
			{
				ACHAR *pszText = (ACHAR *)GlobalLock(hHandle);
				AWString strCopy = m_szBuffer.Mid(iStartChar, iLen);
				wcscpy(pszText, strCopy.GetBuffer(iLen));
				
				for(int i = 0; i < iLen; i++)
					if( IsEditboxItemCode(pszText[i]) )
						pszText[i] = ' ';
				pszText[iLen]=0;
					
				GlobalUnlock(hHandle);
				SetClipboardData(CF_UNICODETEXT, hHandle);
			}
			
			CloseClipboard();

			if (nChar==0x18 && !m_bPassword && !m_bReadOnly)
			{
				DeleteText(iStartChar, iStartChar+iLen);
				CleanSelection();
			}
		}
		
		break;

	case 0x1B: // esc
		if (HasAnySelection())
			CleanSelection();
		else
			bHandled = false;
		break;

	default:
		bHandled = false;
	}

	return bHandled;
}

bool AUIEditBox::IsOpOnSelection()
{
	return HasAnySelection() && (m_xInsertPos==m_xSelBeginPos || m_xInsertPos==m_xSelEndPos);
}

bool AUIEditBox::HasAnySelection()
{
	return m_bAllowSelect && m_xSelBeginPos!=-1 && m_xSelEndPos!=m_xSelBeginPos;
}

void AUIEditBox::DoAlign()
{
	int nText = m_szBuffer.GetLength()+1;
	if (m_nAlign == AUIEDITBOX_ALIGN_LEFT || nText==0)
		return;

	int nTextWidth = m_rcText.Width();
	int i=0;
	while (i < nText)
	{
		int nLineBegWidth = m_CharWidthBuf[i];
		int nLineEndWidth = nLineBegWidth;
		int nLineBegHeight = m_CharHeightBuf[i];		
		int idxLineBeg = i;
		int idxLineEnd = i;

		while (m_CharHeightBuf[i] == nLineBegHeight && i < nText)
			++i;
		idxLineEnd = i-1;
		nLineEndWidth = m_CharWidthBuf[idxLineEnd];

		int nAlignOffset = 0;
		switch (m_nAlign)
		{
		case AUIEDITBOX_ALIGN_LEFT: break;
		case AUIEDITBOX_ALIGN_CENTER:
			nAlignOffset = (m_rcText.Width() - ((int)m_CharWidthBuf[idxLineEnd]-(int)m_CharWidthBuf[idxLineBeg])) / 2;
			break;
		case AUIEDITBOX_ALIGN_RIGHT:
			nAlignOffset = m_rcText.Width() - ((int)m_CharWidthBuf[idxLineEnd]-(int)m_CharWidthBuf[idxLineBeg]);
			break;
		}

		for (int j=idxLineBeg; j<=idxLineEnd; ++j)
			m_CharWidthBuf[j] += nAlignOffset;
	}
}

void AUIEditBox::ReplaceSel(EditboxItemType type, A3DCOLOR cl, const ACHAR* szName, const ACHAR* szInfo, const void* pExtraData /* = NULL */, size_t sz /* = 0 */)
{
	if (!HasAnySelection())
		return;

	int iFrom = 0;
	int iTo = 0;
	if (!m_szBuffer.IsEmpty())
	{
		iFrom = a_Min(m_xSelBeginPos, m_xSelEndPos);
		iTo = a_Max(m_xSelBeginPos, m_xSelEndPos);		
		a_Clamp(iFrom, 0, m_szBuffer.GetLength()-1);
		a_Clamp(iTo, 0, m_szBuffer.GetLength());
		DeleteText(iFrom, iTo);
	}
	
	CleanSelection();

	InsertItem(iFrom, type, cl, szName, szInfo, pExtraData, sz);

	a_Clamp(m_xInsertPos, 0, m_szBuffer.GetLength());
	a_Clamp(m_xCaretPos, 0, m_szBuffer.GetLength());
	a_Clamp(m_xEndBackup, 0, m_szBuffer.GetLength());
	a_Clamp(m_xInsertBackup, 0, m_szBuffer.GetLength());
	m_xEndPos = m_szBuffer.GetLength();
	
	
	CalcTexWidthBuf();
}

void AUIEditBox::ReplaceSel(const ACHAR* szNewText)
{
	if (!HasAnySelection())
		return;

	int iFrom = 0;
	int iTo = 0;
	if (!m_szBuffer.IsEmpty())
	{
		iFrom = a_Min(m_xSelBeginPos, m_xSelEndPos);
		iTo = a_Max(m_xSelBeginPos, m_xSelEndPos);		
		a_Clamp(iFrom, 0, m_szBuffer.GetLength()-1);
		a_Clamp(iTo, 0, m_szBuffer.GetLength());
		DeleteText(iFrom, iTo);
	}
	CleanSelection();

	if (szNewText)
		ValidInsertText(iFrom, szNewText, a_strlen(szNewText));

	a_Clamp(m_xInsertPos, 0, m_szBuffer.GetLength());
	a_Clamp(m_xCaretPos, 0, m_szBuffer.GetLength());
	a_Clamp(m_xEndBackup, 0, m_szBuffer.GetLength());
	a_Clamp(m_xInsertBackup, 0, m_szBuffer.GetLength());
	m_xEndPos = m_szBuffer.GetLength();
	
	
	CalcTexWidthBuf();
}

bool AUIEditBox::InsertItem(int pos, EditboxItemType type, A3DCOLOR cl, 
							const ACHAR* szName, const ACHAR* szInfo, const void* pExtraData /* = NULL */, size_t sz /* = 0 */)
{
	int nBufLen = m_szBuffer.GetLength();

	if (!m_bMaxLenRendered)
	{
		if (nBufLen >= m_nMaxLen)
			return false;
	}

	if( type == enumEIEmotion )
	{
		SetDynamicRender(true);

		int nEmotionSet, nEmotionIndex;
		UnmarshalEmotionInfo(szInfo, nEmotionSet, nEmotionIndex);
		if( !m_vecEmotion || nEmotionSet >= AUIMANAGER_MAX_EMOTIONGROUPS ||
			!m_vecEmotion[nEmotionSet] || nEmotionIndex >= m_vecEmotion[nEmotionSet]->size() )
			return false;
	}
	
	wchar_t ch = m_ItemsSet.AppendItem(type, cl, szName, szInfo);

	if (ch == 0)
		return false;
	
	if (ValidInsertText(pos, &ch, 1) == 0)
	{
		m_ItemsSet.DelItemByChar(ch);
		return false;
	}
	else
	{
		EditBoxItemBase* pItem = m_ItemsSet.GetItemByChar(ch);
		if (pExtraData)
			pItem->SetExtraData(pExtraData, sz);
		return true;
	}
}

int AUIEditBox::ValidInsertText(int nPos, const ACHAR* szAddedText, int nChar)
{
	// 1. calculate current render length
	int nCurLen = m_szBuffer.GetLength();
	if (nCurLen > AUIEDITBOX_MAXTEXTLEN)
		return 0;

	if (m_bMaxLenRendered)
	{
		int nLenRendered = 0;
		for (int i=0; i<nCurLen; i++)
		{
			if (IsEditboxItemCode(m_szBuffer[i]))
			{
				EditBoxItemBase* pItem = m_ItemsSet.GetItemByChar(m_szBuffer[i]);
				if (pItem)
					nLenRendered += pItem->GetRenderedLength();
				else
					nLenRendered += 1;
			}
			else
			{
				bool bIsAutoReturn = (i < nCurLen - 1 && m_szBuffer[i] == '\r' && m_szBuffer[i+1] == '\n');
				if (bIsAutoReturn)
					i += 1;
				else
					nLenRendered += 1;
			}
		}
		
		nCurLen = nLenRendered; 
	}
	if (nCurLen > m_nMaxLen)
		return 0;

	// 2. add text if not exceed limit
	int nAddedLen = 0;
	AWString strAdded;
	ACHAR szCheck[2] = {0, 0};
	for (int i=0; i<nChar; ++i)
	{
		szCheck[0] = szAddedText[i];
		int nLen = 0;
		
		if (IsEditboxItemCode(szAddedText[i]))
		{
			EditBoxItemBase* pItem = m_ItemsSet.GetItemByChar(szAddedText[i]);
			if (pItem)
			{
				if (m_bMaxLenRendered)
				{
					nLen = pItem->GetRenderedLength();
					if (nCurLen + nLen > m_nMaxLen)
						break;
				}
				else
				{
					nLen = pItem->Serialize().GetLength();
					if (nLen + nCurLen > m_nMaxLen)
						break;
					m_nMaxLen -= nLen;
				}
			}
		}
		else
		{
			if (szAddedText[i] == '\r' && i < nChar - 1 && szAddedText[i+1] =='\n')
			{
				i += 1;
				continue;
			}
			nLen = 1;
			if (nCurLen + nLen > m_nMaxLen)
				break;
		}


		strAdded += szAddedText[i];
		nCurLen += nLen;
	}

	m_szBuffer = m_szBuffer.Left(nPos) + strAdded + m_szBuffer.Right(m_szBuffer.GetLength() - nPos);

	return strAdded.GetLength();
}
