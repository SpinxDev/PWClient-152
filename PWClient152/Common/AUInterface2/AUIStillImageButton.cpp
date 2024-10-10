// Filename	: AUIStillImageButton.cpp
// Creator	: Tom Zhou
// Date		: May 18, 2004
// Desc		: AUIStillImageButton is the class of still image button control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A3DGFXExMan.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "CSplit.h"

#include "AUIDialog.h"
#include "AUIStillImageButton.h"
#include "AUIManager.h"
#include "AUICommon.h"
#include "AUICTranslate.h"

#ifdef _ANGELICA_AUDIO
#include "FEventInstance.h"
#endif

AUIStillImageButton::sImageRotate::sImageRotate()
{
	memset(this, 0, sizeof(*this));
}

AUIStillImageButton::AUIStillImageButton() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_STILLIMAGEBUTTON;
	m_nState = AUISTILLIMAGEBUTTON_STATE_NORMAL;
	m_bPushed = false;
	m_bVerticalText = false;
	m_bPushLike = false;
	m_nFrameMode = 0;
	m_pAUIFrameUp = NULL;
	m_pAUIFrameDown = NULL;
	m_pAUIFrameOnHover = NULL;
	m_pAUIFrameDisabled = NULL;
	m_pGfxDown = NULL;
	m_pGfxOnHover = NULL;
	m_nTextOffsetX = 0;
	m_nTextOffsetY = 0;
	m_bHover = false;
}

AUIStillImageButton::~AUIStillImageButton()
{
}

bool AUIStillImageButton::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	StillImageButton property line format:
	 *
	 *		STILLIMAGEBUTTON		Keyword
	 *		OK						Name
	 *		1						String ID of command.
	 *		100 120 50 15			x y Width Height
	 *		0 0 0 0					Response margin
	 *		1						String ID of font name
	 *		20						Font height
	 *		255 255 255 255			Text color
	 *		"ButtonFrameUp.bmp"		ImageFileName
	 *		"ButtonFrameDown.bmp"	ImageFileName
	 *		101						String ID of button text
	 *		1						Align
	 */

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Init(), failed to call base AUIObject::Init()");

	if (!pASF) return true;
	
	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));
//	m_rcTextMargin.left = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
//	m_rcTextMargin.top = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
//	m_rcTextMargin.right = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
//	m_rcTextMargin.bottom = atoi(pASF->m_szToken);

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
		return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Init(), failed to call SetFontAndColor()");

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

	CHECK_BREAK2(pASF->GetNextToken(true));
	if (!InitIndirect(pASF->m_szToken, AUISTILLIMAGEBUTTON_FRAME_UP))
		return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Init(), failed to call InitIndirect()");

	CHECK_BREAK2(pASF->GetNextToken(true));
	if (!InitIndirect(pASF->m_szToken, AUISTILLIMAGEBUTTON_FRAME_DOWN))
		return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Init(), failed to call InitIndirect()");

	if( m_pParent->GetFileVersion() >= 4 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		if (!InitIndirect(pASF->m_szToken, AUISTILLIMAGEBUTTON_FRAME_ONHOVER))
			return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Init(), failed to call InitIndirect()");
	}

	if( m_pParent->GetFileVersion() > 1 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		if (!InitIndirect(pASF->m_szToken, AUISTILLIMAGEBUTTON_FRAME_DISABLED))
			return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Init(), failed to call InitIndirect()");
	}

	CHECK_BREAK2(pASF->GetNextToken(true));
	idString = atoi(pASF->m_szToken);
	m_szText = m_pParent->GetStringFromTable(idString);
	// If button text is image filename, it means it's an image button.
//	if( a_strstr(m_szText, _AL(".bmp")) ||
//		a_strstr(m_szText, _AL(".BMP")) ||
//		a_strstr(m_szText, _AL(".tga")) ||
//		a_strstr(m_szText, _AL(".TGA")) ||
//		a_strstr(m_szText, _AL(".dds")) ||
//		a_strstr(m_szText, _AL(".DDS")) )
//	{
//		if (!InitIndirect((char *)(const char *)AC2AS(m_szText), AUISTILLIMAGEBUTTON_SPRITE))
//			return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Init(), failed to call InitIndirect()");
//	}

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nAlign = atoi(pASF->m_szToken);
	
	if( m_pParent->GetFileVersion() > 0 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		AUIOBJECT_SETPROPERTY p;
		p.i = atoi(pASF->m_szToken);
		SetProperty("Frame Mode", &p);
	}

	END_FAKE_WHILE2;
	
	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIStillImageButton::Init(), failed to read from file");
	
	END_ON_FAIL_FAKE_WHILE2;
	
	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIStillImageButton::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
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
	XMLGET_BOOL_VALUE(pItem, _AL("Anomaly"), m_bAnomaly)
	XMLGET_INT_VALUE(pItem, _AL("TextOffsetX"), m_nTextOffsetX)
	XMLGET_INT_VALUE(pItem, _AL("TextOffsetY"), m_nTextOffsetY)
	XMLGET_STRING_VALUE(pItem, _AL("SoundEffect"), m_strSound)
	XMLGET_STRING_VALUE(pItem, _AL("HoverSoundEffect"), m_strSoundHover)

#ifdef _ANGELICA_AUDIO
	LoadAudioInstance(m_strSound, m_pAudio);
	LoadAudioInstance(m_strSoundHover, m_pAudioHover);
#endif

	XMLGET_STRING_VALUE(pItem, _AL("Command"), m_strCommand)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Text")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pChildItem, _AL("String"), m_szText)
			XMLGET_BOOL_VALUE(pChildItem, _AL("Vertical"), m_bVerticalText)
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
			XMLGET_BOOL_VALUE(pChildItem, _AL("HoverColor"), m_bHoverColor)
			XMLGET_COLOR_VALUE(pChildItem, _AL("TextHoverColor"), m_clHover)
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0)
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameUpImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					InitIndirect(strFileName, AUISTILLIMAGEBUTTON_FRAME_UP);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameDownImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					InitIndirect(strFileName, AUISTILLIMAGEBUTTON_FRAME_DOWN);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameOnHoverImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					InitIndirect(strFileName, AUISTILLIMAGEBUTTON_FRAME_ONHOVER);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameDisabledImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					InitIndirect(strFileName, AUISTILLIMAGEBUTTON_FRAME_DISABLED);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("GfxDown")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					SetGfxDown(strFileName);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("GfxOnHover")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					SetGfxOnHover(strFileName);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ImageDegree")) == 0 )
				{
					int nDegree = 0 ;
					XMLGET_INT_VALUE(pResouceChildItem, _AL("Degree"), nDegree)
					SetImageDegree(nDegree);
				}

				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
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
		return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::InitXML(), failed to call SetFontAndColor()");
	
	return true;
}

bool AUIStillImageButton::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;
	PAUIFRAME pAUIFrame;

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	UpdateRenderTarget();
	if( AUISTILLIMAGEBUTTON_FRAME_UP == idType )
	{
		A3DRELEASE(m_pAUIFrameUp);
	}
	else if( AUISTILLIMAGEBUTTON_FRAME_DOWN == idType )
	{
		A3DRELEASE(m_pAUIFrameDown);
	}
	else if( AUISTILLIMAGEBUTTON_FRAME_ONHOVER == idType )
	{
		A3DRELEASE(m_pAUIFrameOnHover);
	}
	else
	{
		A3DRELEASE(m_pAUIFrameDisabled);
	}

	if( strlen(pszFileName) <= 0 ) return true;

	pAUIFrame = new AUIFrame;
	if( !pAUIFrame ) return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::InitIndirect(), no enough memory");

	bval = pAUIFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
	if( !bval )
	{
		A3DRELEASE(pAUIFrame)
		return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::InitIndirect(), failed to call pAUIFrame->Init()");
	}

	if( m_pParent && m_pParent->GetAUIManager() )
		pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());

	if( AUISTILLIMAGEBUTTON_FRAME_UP == idType )
		m_pAUIFrameUp = pAUIFrame;
	else if( AUISTILLIMAGEBUTTON_FRAME_DOWN == idType)
		m_pAUIFrameDown = pAUIFrame;
	else if( AUISTILLIMAGEBUTTON_FRAME_ONHOVER == idType)
		m_pAUIFrameOnHover = pAUIFrame;
	else
		m_pAUIFrameDisabled = pAUIFrame;

	return true;
}

bool AUIStillImageButton::Release(void)
{
	A3DRELEASE(m_pAUIFrameUp);
	A3DRELEASE(m_pAUIFrameDown);
	A3DRELEASE(m_pAUIFrameOnHover);
	A3DRELEASE(m_pAUIFrameDisabled);

	if (m_pGfxDown)
	{
		AfxGetGFXExMan()->CacheReleasedGfx(m_pGfxDown);
		m_pGfxDown = NULL;
	}

	if (m_pGfxOnHover)
	{
		AfxGetGFXExMan()->CacheReleasedGfx(m_pGfxOnHover);
		m_pGfxOnHover = NULL;
	}

	return AUIObject::Release();
}

bool AUIStillImageButton::Save(FILE *file)
{
	fprintf(file, "STILLIMAGEBUTTON");

	if( !AUIObject::Save(file) ) return false;

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_szFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idString = m_pParent->AddStringToTable(szFontName);
	if( idString < 0 ) return false;

	fprintf(file, " 0 0 0 0");

	fprintf(file, " %d %d %d %d %d %d", idString, m_nFontHeight,
		A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color),
		A3DCOLOR_GETBLUE(m_color), A3DCOLOR_GETALPHA(m_color));

	fprintf(file, " \"%s\" \"%s\" \"%s\" \"%s\"",
		m_pAUIFrameUp ? m_pAUIFrameUp->GetA2DSprite()->GetName() : "",
		m_pAUIFrameDown ? m_pAUIFrameDown->GetA2DSprite()->GetName() : "",
		m_pAUIFrameOnHover ? m_pAUIFrameOnHover->GetA2DSprite()->GetName() : "",
		m_pAUIFrameDisabled ? m_pAUIFrameDisabled->GetA2DSprite()->GetName() : "");

	idString = m_pParent->AddStringToTable(m_szText);
	if( idString < 0 ) return false;
	fprintf(file, " %d", idString);

	fprintf(file, " %d %d\n", m_nAlign, m_nFrameMode);

	return true;
}

bool AUIStillImageButton::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemStillImageButton = new AXMLItem;
	pXMLItemStillImageButton->InsertItem(pXMLItemDialog);
	pXMLItemStillImageButton->SetName(_AL("STILLIMAGEBUTTON"));
	XMLSET_STRING_VALUE(pXMLItemStillImageButton, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemStillImageButton, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemStillImageButton, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemStillImageButton, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemStillImageButton, _AL("Height"), m_nHeight, 0)
	XMLSET_BOOL_VALUE(pXMLItemStillImageButton, _AL("Anomaly"), m_bAnomaly, false)
	XMLSET_INT_VALUE(pXMLItemStillImageButton, _AL("TextOffsetX"), m_nTextOffsetX, 0)
	XMLSET_INT_VALUE(pXMLItemStillImageButton, _AL("TextOffsetY"), m_nTextOffsetY, 0)
	XMLSET_STRING_VALUE(pXMLItemStillImageButton, _AL("SoundEffect"), AS2AC(m_strSound), _AL(""))
	XMLSET_STRING_VALUE(pXMLItemStillImageButton, _AL("HoverSoundEffect"), AS2AC(m_strSoundHover), _AL(""))
	XMLSET_STRING_VALUE(pXMLItemStillImageButton, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemStillImageButton);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	XMLSET_STRING_VALUE(pXMLItemStillImageButton, _AL("Command"), AS2AC(m_strCommand), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemStillImageButton);
	pXMLItemText->SetName(_AL("Text"));
	XMLSET_STRING_VALUE(pXMLItemText, _AL("String"), m_szText, _AL(""))
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Vertical"), m_bVerticalText, false)
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
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("HoverColor"), m_bHoverColor, false)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("TextHoverColor"), m_clHover, _AL("255,255,255,255"))
	
	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemStillImageButton);
	pXMLItemResource->SetName(_AL("Resource"));

	if( m_pAUIFrameUp )
	{
		AXMLItem *pXMLItemFrameUpImage = new AXMLItem;
		pXMLItemFrameUpImage->InsertItem(pXMLItemResource);
		pXMLItemFrameUpImage->SetName(_AL("FrameUpImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameUpImage, _AL("FileName"), AS2AC(m_pAUIFrameUp->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameUpImage, _AL("FrameMode"), m_nFrameMode, 0)
	}

	if( m_pAUIFrameDown )
	{
		AXMLItem *pXMLItemFrameDownImage = new AXMLItem;
		pXMLItemFrameDownImage->InsertItem(pXMLItemResource);
		pXMLItemFrameDownImage->SetName(_AL("FrameDownImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameDownImage, _AL("FileName"), AS2AC(m_pAUIFrameDown->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameDownImage, _AL("FrameMode"), m_nFrameMode, 0)
	}

	if( m_pAUIFrameOnHover )
	{
		AXMLItem *pXMLItemFrameDownImage = new AXMLItem;
		pXMLItemFrameDownImage->InsertItem(pXMLItemResource);
		pXMLItemFrameDownImage->SetName(_AL("FrameOnHoverImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameDownImage, _AL("FileName"), AS2AC(m_pAUIFrameOnHover->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameDownImage, _AL("FrameMode"), m_nFrameMode, 0)
	}

	if( m_pAUIFrameDisabled )
	{
		AXMLItem *pXMLItemFrameDisabledImage = new AXMLItem;
		pXMLItemFrameDisabledImage->InsertItem(pXMLItemResource);
		pXMLItemFrameDisabledImage->SetName(_AL("FrameDisabledImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameDisabledImage, _AL("FileName"), AS2AC(m_pAUIFrameDisabled->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameDisabledImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	
	if(m_imageRotate.nDegree)
	{
		AXMLItem *pXMLItemImageDegree = new AXMLItem;
		pXMLItemImageDegree->InsertItem(pXMLItemResource);
		pXMLItemImageDegree->SetName(_AL("ImageDegree"));
		XMLSET_INT_VALUE(pXMLItemImageDegree, _AL("Degree"), m_imageRotate.nDegree, 0);
	}

	if( m_strGfxDown != "" )
	{
		AXMLItem *pXMLItemGfx = new AXMLItem;
		pXMLItemGfx->InsertItem(pXMLItemResource);
		pXMLItemGfx->SetName(_AL("GfxDown"));
		XMLSET_STRING_VALUE(pXMLItemGfx, _AL("FileName"), AS2AC(m_strGfxDown), _AL(""))
	}

	if( m_strGfxOnHover != "" )
	{
		AXMLItem *pXMLItemGfx = new AXMLItem;
		pXMLItemGfx->InsertItem(pXMLItemResource);
		pXMLItemGfx->SetName(_AL("GfxOnHover"));
		XMLSET_STRING_VALUE(pXMLItemGfx, _AL("FileName"), AS2AC(m_strGfxOnHover), _AL(""))
	}

	return true;
}

bool AUIStillImageButton::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( WM_LBUTTONDOWN == msg || WM_LBUTTONDBLCLK == msg )
	{
#ifdef _ANGELICA_AUDIO
		if(m_pAudio)
			m_pParent->GetAUIManager()->PlayAudioInstance(m_pAudio);
		else if( strlen(m_strSound) > 0 )
			m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#else
		if( strlen(m_strSound) > 0 )
			m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#endif

		m_nState = AUISTILLIMAGEBUTTON_STATE_CLICK;
		UpdateRenderTarget();
		if( m_pGfxDown )
			m_pGfxDown->Start();
		
		return true;
	}
	else if( WM_LBUTTONUP == msg )
	{
		if( AUISTILLIMAGEBUTTON_STATE_CLICK == m_nState )
		{
			m_nState = AUISTILLIMAGEBUTTON_STATE_NORMAL;
			if( m_strCommand != "" )
				m_pParent->SetCommand(m_strCommand);
			else
				m_pParent->SetCommand(m_szName);
			if( m_bPushLike ) m_bPushed = !m_bPushed;
		}
		return true;
	}
	else if( WM_KILLFOCUS == msg )
	{
		m_nState = AUISTILLIMAGEBUTTON_STATE_NORMAL;
		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIStillImageButton::Tick(void)
{
	if (m_pGfxDown && (m_nState == AUISTILLIMAGEBUTTON_STATE_CLICK || m_pGfxDown->GetState() == ST_PLAY) )
	{
		DWORD dwTick = GetTickCount();
		POINT ptPos = GetPos();
		ptPos.x += m_nOffX + m_nWidth / 2;
		ptPos.y += m_nOffY + m_nHeight / 2;
		m_pGfxDown->SetParentTM(a3d_Translate(ptPos.x, ptPos.y, 0));
		m_pGfxDown->TickAnimation(m_pParent->GetAUIManager()->GetTickTime());
	}
	else if (m_pGfxOnHover)
	{
		PAUIOBJECT pMouseOnObj;
		m_pParent->GetAUIManager()->GetMouseOn(NULL, &pMouseOnObj);
		if( pMouseOnObj == this )		
		{
			DWORD dwTick = GetTickCount();
			POINT ptPos = GetPos();
			ptPos.x += m_nOffX + m_nWidth / 2;
			ptPos.y += m_nOffY + m_nHeight / 2;
			m_pGfxOnHover->SetParentTM(a3d_Translate(ptPos.x, ptPos.y, 0));
			m_pGfxOnHover->TickAnimation(m_pParent->GetAUIManager()->GetTickTime());
		}
	}
	PAUIOBJECT pMouseOnObj;
	m_pParent->GetAUIManager()->GetMouseOn(NULL, &pMouseOnObj);
	PAUIDIALOG pDlgActive = m_pParent->GetAUIManager()->GetActiveDialog();
	if( (m_pAUIFrameOnHover || m_pGfxOnHover) && this == pMouseOnObj && !(pDlgActive && pDlgActive->IsModal() && pDlgActive != m_pParent) )
		SetHover(true);
	else
		SetHover(false);

	return AUIObject::Tick();
}

bool AUIStillImageButton::Render(void)
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC )
	{
		A3DGFXEx *pCurGfx = NULL;

		if( m_bEnabled )
		{
			PAUIOBJECT pMouseOnObj;
			m_pParent->GetAUIManager()->GetMouseOn(NULL, &pMouseOnObj);
			PAUIDIALOG pDlgActive = m_pParent->GetAUIManager()->GetActiveDialog();

			if( m_bPushed || AUISTILLIMAGEBUTTON_STATE_CLICK == GetState() )
				pCurGfx = m_pGfxDown;
			else if( m_bHover )
				pCurGfx = m_pGfxOnHover;
		}

		if( m_pGfxDown && m_pGfxDown->GetState() == ST_PLAY )
			pCurGfx = m_pGfxDown;

		if( pCurGfx )
		{
			pCurGfx->SetAlpha(float(nAlpha) / 255.0f);
			AfxGetGFXExMan()->RegisterGfx(pCurGfx);
		}
	}

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		bool bval = true;
		PAUIFRAME pAUIFrame = NULL;
		POINT ptPos = GetPos();
		int xOff = 0, yOff = 0;

		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		A3DCOLOR color = m_color;

		if( !m_bEnabled )
		{	
			if( m_pAUIFrameDisabled )
				pAUIFrame = m_pAUIFrameDisabled;
			else
			{
				pAUIFrame = m_pAUIFrameUp;
				if( pAUIFrame )
					pAUIFrame->GetA2DSprite()->SetColor(A3DCOLORRGB(160, 160, 160));
			}
		}
		else
		{
			PAUIOBJECT pMouseOnObj;
			m_pParent->GetAUIManager()->GetMouseOn(NULL, &pMouseOnObj);
			PAUIDIALOG pDlgActive = m_pParent->GetAUIManager()->GetActiveDialog();

			if( m_bPushed || AUISTILLIMAGEBUTTON_STATE_CLICK == GetState() )
			{
				xOff = yOff = 1;
				pAUIFrame = m_pAUIFrameDown;
			}
			else if( m_bHover )
			{
				pAUIFrame = m_pAUIFrameOnHover;

				if( m_bHoverColor )
					color = m_clHover;
			}
			else
				pAUIFrame = m_pAUIFrameUp;

			if( pAUIFrame )
				pAUIFrame->GetA2DSprite()->SetColor(A3DCOLORRGB(255, 255, 255));
		}

		int nWidth = 0, nHeight = 0;

		if( !m_bEnabled )
			color = A3DCOLORRGB(128, 128, 128);

		UpdateImageDegree();
		FitFont();
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
		if( pAUIFrame )
		{
			bval = pAUIFrame->Render(ptPos.x, ptPos.y, m_nWidth, m_nHeight,
				pFont, color, (m_bBlinkText && m_bBlink)
				? _AL("") : m_szText, m_nAlign, xOff + m_nTextOffsetX * fWindowScale, yOff + m_nTextOffsetY * fWindowScale, nAlpha, m_pParent->IsNoFlush(), 0, m_bVerticalText);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIStillImageButton::Render(), failed to call pAUIFrame->Render()");
		}
		else if( m_szText.GetLength() > 0 && !(m_bBlinkText && m_bBlink) )
		{
			A3DPOINT2 ptSize;
			int nTextW = 0, nTextH = 0;
			int x, y;
			int W = 0;
			if( m_pAUIFrameDown )
			{
				W = m_pAUIFrameDown->GetSizeLimit().cx / 2;
				nWidth = m_nWidth;
				nHeight = m_nHeight;
			}
			ptSize = pFont->GetTextExtent(m_szText);
			nTextW = ptSize.x;
			nTextH = ptSize.y;

			if( AUIFRAME_ALIGN_RIGHT == m_nAlign )
				x = ptPos.x + nWidth - W - nTextW;
			else if( AUIFRAME_ALIGN_CENTER == m_nAlign )
				x = ptPos.x + nWidth / 2 - nTextW / 2;
			else
				x = ptPos.x + W;
			
			y = ptPos.y + (nHeight - nTextH + 1) / 2 - 1;

			if (!m_bVerticalText)
				pFont->TextOut(x + m_nTextOffsetX * fWindowScale, y + m_nTextOffsetY * fWindowScale, m_szText, (nAlpha << 24) | (0xFFFFFF & color),
					(m_nShadow & 0xFF000000) ? true : false, (nAlpha << 24) | (0xFFFFFF & m_nShadow));
			else
				pFont->TextOutVert(x + m_nTextOffsetX * fWindowScale, y + m_nTextOffsetY * fWindowScale, m_szText, (nAlpha << 24) | (0xFFFFFF & color),
					(m_nShadow & 0xFF000000) ? true : false, (nAlpha << 24) | (0xFFFFFF & m_nShadow));
		}
	}

	return true;
}

bool AUIStillImageButton::GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Up Frame File") )
	{
		if( m_pAUIFrameUp )
			strncpy(Property->fn, m_pAUIFrameUp->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Frame File") )
	{
		if( m_pAUIFrameDown )
			strncpy(Property->fn, m_pAUIFrameDown->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "OnHover Frame File") )
	{
		if( m_pAUIFrameOnHover )
			strncpy(Property->fn, m_pAUIFrameOnHover->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Disabled Frame File") )
	{
		if( m_pAUIFrameDisabled )
			strncpy(Property->fn, m_pAUIFrameDisabled->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Gfx File") )
		strncpy(Property->fn, m_strGfxDown, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "OnHover Gfx File") )
		strncpy(Property->fn, m_strGfxOnHover, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Text Offset X") )
		Property->i = m_nTextOffsetX;
	else if( 0 == strcmpi(pszPropertyName, "Text Offset Y") )
		Property->i = m_nTextOffsetY;
	else if( 0 == strcmpi(pszPropertyName, "Image Degree") )
		Property->i = m_imageRotate.nDegree;
	else if( 0 == strcmpi(pszPropertyName, "Vertical Text") )
		Property->b = m_bVerticalText;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIStillImageButton::SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Width") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrameUp )
		{
			SIZE s = m_pAUIFrameUp->GetSizeLimit();
			m_nWidth = max(m_nWidth, s.cx);
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "Height") )
	{
		AUIObject::SetProperty(pszPropertyName, Property);
		if( m_pAUIFrameUp )
		{
			SIZE s = m_pAUIFrameUp->GetSizeLimit();
			m_nHeight = max(m_nHeight, s.cy);
		}
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrameUp )
			m_pAUIFrameUp->SetMode(m_nFrameMode);
		if( m_pAUIFrameDown )
			m_pAUIFrameDown->SetMode(m_nFrameMode);
		if( m_pAUIFrameOnHover )
			m_pAUIFrameOnHover->SetMode(m_nFrameMode);
		if( m_pAUIFrameDisabled )
			m_pAUIFrameDisabled->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Up Frame File") )
		return InitIndirect(Property->fn, AUISTILLIMAGEBUTTON_FRAME_UP);
	else if( 0 == strcmpi(pszPropertyName, "Down Frame File") )
		return InitIndirect(Property->fn, AUISTILLIMAGEBUTTON_FRAME_DOWN);
	else if( 0 == strcmpi(pszPropertyName, "OnHover Frame File") )
		return InitIndirect(Property->fn, AUISTILLIMAGEBUTTON_FRAME_ONHOVER);
	else if( 0 == strcmpi(pszPropertyName, "Disabled Frame File") )
		return InitIndirect(Property->fn, AUISTILLIMAGEBUTTON_FRAME_DISABLED);
	else if( 0 == strcmpi(pszPropertyName, "Down Gfx File") )
		return SetGfxDown(Property->fn);
	else if( 0 == strcmpi(pszPropertyName, "OnHover Gfx File") )
		return SetGfxOnHover(Property->fn);
	else if( 0 == strcmpi(pszPropertyName, "Text Offset X") )
		m_nTextOffsetX = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Text Offset Y") )
		m_nTextOffsetY = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Image Degree") )
	{
		SetImageDegree(Property->i);
	}
	else if( 0 == strcmpi(pszPropertyName, "Vertical Text") )
		m_bVerticalText = Property->b;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIStillImageButton::SetGfxDown(const char* szGfx)
{
	if (m_pGfxDown)
	{
		AfxGetGFXExMan()->CacheReleasedGfx(m_pGfxDown);
		m_pGfxDown = NULL;
	}

	m_strGfxDown = "";
	if( szGfx && strlen(szGfx) > 0 )
	{
		m_strGfxDown = szGfx;

#ifdef _DX9
		m_pGfxDown = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, szGfx, true, false);
#else
		m_pGfxDown = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, szGfx);
#endif
		return m_pGfxDown != NULL;
	}

	return true;
}

bool AUIStillImageButton::SetGfxOnHover(const char* szGfx)
{
	if (m_pGfxOnHover)
	{
		AfxGetGFXExMan()->CacheReleasedGfx(m_pGfxOnHover);
		m_pGfxOnHover = NULL;
	}

	m_strGfxOnHover = "";
	if( szGfx && strlen(szGfx) > 0 )
	{
		m_strGfxOnHover = szGfx;

#ifdef _DX9
		m_pGfxOnHover = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, szGfx, true, false);
#else
		m_pGfxOnHover = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, szGfx);
#endif
		return m_pGfxOnHover != NULL;
	}

	return true;
}

A3DGFXEx* AUIStillImageButton::GetGfxDown()
{
	return m_pGfxDown;
}

A3DGFXEx* AUIStillImageButton::GetGfxOnHover()
{
	return m_pGfxOnHover;
}

void AUIStillImageButton::StartOnHoverGfx()
{
	if( m_pGfxOnHover )
		m_pGfxOnHover->Start();
}

bool AUIStillImageButton::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Up Frame File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Down Frame File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("OnHover Frame File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Disabled Frame File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Image Degree", AUIOBJECT_PROPERTY_VALUE_INT);
}

PAUIFRAME AUIStillImageButton::GetFrame(int type)
{
	switch(type)
	{
	case AUISTILLIMAGEBUTTON_FRAME_DOWN:
		return m_pAUIFrameDown;
	case AUISTILLIMAGEBUTTON_FRAME_DISABLED:
		return m_pAUIFrameDisabled;
	case AUISTILLIMAGEBUTTON_FRAME_ONHOVER:
		return m_pAUIFrameOnHover;
	default:
		return m_pAUIFrameUp;
	}
}

void AUIStillImageButton::SetPushLike(bool bPushLike)
{
	m_bPushLike = bPushLike;
}

bool AUIStillImageButton::IsPushed()
{
	return m_bPushed;
}

void AUIStillImageButton::SetPushed(bool bPushed)
{
	m_bPushed = bPushed;
}

void AUIStillImageButton::SetHover(bool bHover)
{
	if( m_bHover == bHover )
		return;

	if( m_pAUIFrameOnHover )
		UpdateRenderTarget();

	if( bHover )
	{
#ifdef _ANGELICA_AUDIO	
		if (m_pAudioHover)
			m_pParent->GetAUIManager()->PlayAudioInstance(m_pAudioHover);
		else if( strlen(m_strSoundHover) > 0 )
			m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSoundHover);
#else
		if( strlen(m_strSoundHover) > 0 )
			m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSoundHover);
#endif
	}
	else
	{
#ifdef _ANGELICA_AUDIO
		//m_pParent->GetAUIManager()->PlayAudioInstance(NULL);
		//m_pParent->GetAUIManager()->StopAudioInstance(m_pAudioHover);
#endif
	}

	m_bHover = bHover;
}

bool AUIStillImageButton::IsInHitArea(int x, int y, UINT msg)
{
	if( !AUIObject::IsInHitArea(x, y, msg) )
		return false;

	if( !m_bAnomaly )
		return true;

	if( !m_pAUIFrameUp )
		return false;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	DWORD color = m_pAUIFrameUp->GetPixel(int((x - GetPos().x) / fWindowScale), int((y - GetPos().y) / fWindowScale), m_nDefaultWidth, m_nDefaultHeight);
	return A3DCOLOR_GETALPHA(color) > 0;
}

bool AUIStillImageButton::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( m_pAUIFrameUp )
		m_pAUIFrameUp->SetScale(fWindowScale);
	if( m_pAUIFrameDown )
		m_pAUIFrameDown->SetScale(fWindowScale);
	if( m_pAUIFrameOnHover )
		m_pAUIFrameOnHover->SetScale(fWindowScale);
	if( m_pAUIFrameDisabled )
		m_pAUIFrameDisabled->SetScale(fWindowScale);

	if( m_pGfxDown )
		m_pGfxDown->SetScale(fWindowScale);

	if( m_pGfxOnHover )
		m_pGfxOnHover->SetScale(fWindowScale);

	return true;
}

void AUIStillImageButton::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIStillImageButton::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pAUIFrameUp)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrameUp->GetA2DSprite(), bLoad);

	if (m_pAUIFrameDown)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrameDown->GetA2DSprite(), bLoad);

	if (m_pAUIFrameOnHover)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrameOnHover->GetA2DSprite(), bLoad);

	if (m_pAUIFrameDisabled)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrameDisabled->GetA2DSprite(), bLoad);
}

void AUIStillImageButton::SetVerticalText(bool bVertical)
{
	m_bVerticalText = bVertical;
}

bool AUIStillImageButton::IsVerticalText() const
{
	return m_bVerticalText;
}

void AUIStillImageButton::SetImageDegree(const int nDegree)
{
	m_imageRotate.nDegree	= nDegree;
	m_imageRotate.nWidth	= m_nWidth;
	m_imageRotate.nHeight	= m_nHeight;
	m_imageRotate.pSprite[0] = m_pAUIFrameUp ? m_pAUIFrameUp->GetA2DSprite():NULL;
	m_imageRotate.pSprite[1] = m_pAUIFrameDown ? m_pAUIFrameDown->GetA2DSprite():NULL;
	m_imageRotate.pSprite[2] = m_pAUIFrameOnHover ? m_pAUIFrameOnHover->GetA2DSprite():NULL;
	m_imageRotate.pSprite[3] = m_pAUIFrameDisabled ? m_pAUIFrameDisabled->GetA2DSprite():NULL;

	int iOffsetX = 0;
	int iOffsetY = 0;
	if(!m_imageRotate.nDegree || m_imageRotate.nWidth<=0 || m_imageRotate.nHeight<=0)
	{
		iOffsetX	= 0;
		iOffsetY	= 0;
		const int iNum = sizeof(m_imageRotate.pSprite)/sizeof(m_imageRotate.pSprite[0]);
		for (unsigned int i=0; i<iNum; ++i)
		{
			if(m_imageRotate.pSprite[i])
			{
				m_imageRotate.pSprite[i]->SetDegree(0);
			}
		}
	}else
	{
		const float  fRadian  = -DEG2RAD(m_imageRotate.nDegree)-atan2((float)m_imageRotate.nHeight, (float)m_imageRotate.nWidth);
		const float  fLen = sqrt((float)m_imageRotate.nWidth*m_imageRotate.nWidth+(float)m_imageRotate.nHeight*m_imageRotate.nHeight)/2;
		const float  fNewCenterX =  fLen*cos(fRadian);
		const float  fNewCenterY =  -fLen*sin(fRadian);
		iOffsetX = m_imageRotate.nWidth/2  - fNewCenterX;
		iOffsetY = m_imageRotate.nHeight/2 - fNewCenterY;	
		const int iNum = sizeof(m_imageRotate.pSprite)/sizeof(m_imageRotate.pSprite[0]);
		for (unsigned int i=0; i<iNum; ++i)
		{
			if(m_imageRotate.pSprite[i])
			{
				m_imageRotate.pSprite[i]->SetDegree(m_imageRotate.nDegree);
			}
		}
	}
	if(m_pAUIFrameUp)
	{
		m_pAUIFrameUp->SetSpriteOffset(iOffsetX, iOffsetY);
	}
	if(m_pAUIFrameDown)
	{
		m_pAUIFrameDown->SetSpriteOffset(iOffsetX, iOffsetY);
	}
	if(m_pAUIFrameOnHover)
	{
		m_pAUIFrameOnHover->SetSpriteOffset(iOffsetX, iOffsetY);
	}
	if(m_pAUIFrameDisabled)
	{
		m_pAUIFrameDisabled->SetSpriteOffset(iOffsetX, iOffsetY);
	}
}

void AUIStillImageButton::UpdateImageDegree()
{
	if(!m_imageRotate.nDegree)
	{
		return;
	}
	A2DSprite* pSprite1 = m_pAUIFrameUp ? m_pAUIFrameUp->GetA2DSprite():NULL;
	A2DSprite* pSprite2 = m_pAUIFrameDown ? m_pAUIFrameDown->GetA2DSprite():NULL;
	A2DSprite* pSprite3 = m_pAUIFrameOnHover ? m_pAUIFrameOnHover->GetA2DSprite():NULL;
	A2DSprite* pSprite4 = m_pAUIFrameDisabled ? m_pAUIFrameDisabled->GetA2DSprite():NULL;

	if( m_imageRotate.nWidth==m_nWidth && 
		m_imageRotate.nHeight==m_nHeight && 
		m_imageRotate.pSprite[0]==pSprite1 &&
		m_imageRotate.pSprite[1]==pSprite2 &&
		m_imageRotate.pSprite[2]==pSprite3 &&
		m_imageRotate.pSprite[3]==pSprite4)
	{
		return;
	}
	SetImageDegree(m_imageRotate.nDegree);
}