// Filename	: AUIRadioButton.cpp
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIRadioButton is the class of radio button control.

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

#include "A3DFTFont.h"
#include "AUIDialog.h"
#include "AUIManager.h"
#include "AUIRadioButton.h"
#include "AUICommon.h"
#include "AUICTranslate.h"
#include "CSplit.h"

AUIRadioButton::AUIRadioButton() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_RADIOBUTTON;
	m_idGroup = -1;
	m_idButton = -1;
	m_pA2DSpriteNormal = NULL;
	m_pA2DSpriteChecked = NULL;
	m_pA2DSpriteOnHover = NULL;
	m_pA2DSpriteCheckedOnHover = NULL;
	m_nState = AUIRADIOBUTTON_ITEM_NORMAL;
	m_nAlign = AUIFRAME_ALIGN_CENTER;
	m_nTextOffsetX = 0;
	m_nTextOffsetY = 0;
	m_bHover = false;
	m_bCheck = false;
	m_bVerticalText = false;
}

AUIRadioButton::~AUIRadioButton()
{
}

bool AUIRadioButton::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	RadioButton property line format:
	 *
	 *		RADIO				Keyword
	 *		Volume				Name
	 *		1					String ID of command.
	 *		100 100 25 25		x y Width Height
	 *		"Radio.bmp"			ImageFileName
	 *		0					Button ID
	 *		1					Group ID
	 */

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIRadioButton::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;

	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));

	CHECK_BREAK2(InitIndirect(pASF->m_szToken, 0));

	if( m_nWidth <= 0 )
		m_nWidth = m_pA2DSpriteNormal->GetWidth();
	if( m_nHeight <= 0 )
		m_nHeight = m_pA2DSpriteNormal->GetHeight() / AUIRADIOBUTTON_NUMITEMS;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_idButton = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_idGroup = atoi(pASF->m_szToken);

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIRadioButton::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIRadioButton::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
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
	XMLGET_INT_VALUE(pItem, _AL("Align"), m_nAlign)
	XMLGET_BOOL_VALUE(pItem, _AL("Anomaly"), m_bAnomaly)
	XMLGET_INT_VALUE(pItem, _AL("ButtonID"), m_idButton)
	XMLGET_INT_VALUE(pItem, _AL("GroupID"), m_idGroup)
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
			XMLGET_BOOL_VALUE(pChildItem, _AL("CheckColor"), m_bCheckColor)
			XMLGET_COLOR_VALUE(pChildItem, _AL("TextCheckColor"), m_clCheck)
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0)
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("NormalImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIRADIOBUTTON_ITEM_NORMAL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("CheckedImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIRADIOBUTTON_ITEM_CHECKED);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("OnHoverImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIRADIOBUTTON_ITEM_ONHOVER);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("CheckedOnHoverImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIRADIOBUTTON_ITEM_CHECKEDONHOVER);
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
		return AUI_ReportError(__LINE__, 1, "AUIRadioButton::InitXML(), failed to call SetFontAndColor()");
	
	return true;
}

bool AUIRadioButton::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	UpdateRenderTarget();
	if( AUIRADIOBUTTON_ITEM_NORMAL == idType )
	{
		A3DRELEASE(m_pA2DSpriteNormal);
	}
	else if( AUIRADIOBUTTON_ITEM_CHECKED == idType )
	{
		A3DRELEASE(m_pA2DSpriteChecked);
	}
	else if( AUIRADIOBUTTON_ITEM_ONHOVER == idType )
	{
		A3DRELEASE(m_pA2DSpriteOnHover);
	}
	else if( AUIRADIOBUTTON_ITEM_CHECKEDONHOVER == idType )
	{
		A3DRELEASE(m_pA2DSpriteCheckedOnHover);
	}

	if( strlen(pszFileName) <= 0 ) return true;

	A2DSprite *pA2DSprite = new A2DSprite;
	if( !pA2DSprite ) return AUI_ReportError(__LINE__, 1, "AUIRadioButton::InitIndirect(), no enough memory");

#ifdef _ANGELICA22
	pA2DSprite->SetDynamicTex(true);
#endif //_ANGELICA22

	bval = pA2DSprite->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
	if( !bval )
	{
		A3DRELEASE(pA2DSprite)
		return AUI_ReportError(__LINE__, 1, "AUIRadioButton::InitIndirect(), failed to call A2DSprite->Init()");
	}

	if( AUIRADIOBUTTON_ITEM_NORMAL == idType )
		m_pA2DSpriteNormal = pA2DSprite;
	else if( AUIRADIOBUTTON_ITEM_CHECKED == idType )
		m_pA2DSpriteChecked = pA2DSprite;
	else if( AUIRADIOBUTTON_ITEM_ONHOVER == idType )
		m_pA2DSpriteOnHover = pA2DSprite;
	else if( AUIRADIOBUTTON_ITEM_CHECKEDONHOVER == idType )
		m_pA2DSpriteCheckedOnHover = pA2DSprite;

	return bval;
}

bool AUIRadioButton::Release(void)
{
	A3DRELEASE(m_pA2DSpriteNormal);
	A3DRELEASE(m_pA2DSpriteChecked);
	A3DRELEASE(m_pA2DSpriteOnHover);
	A3DRELEASE(m_pA2DSpriteCheckedOnHover);

	return AUIObject::Release();
}

bool AUIRadioButton::Save(FILE *file)
{
	fprintf(file, "RADIO");

	if( !AUIObject::Save(file) ) return false;

	fprintf(file, " \"%s\" %d %d\n",
		m_pA2DSpriteNormal ? m_pA2DSpriteNormal->GetName() : "", m_idButton, m_idGroup);

	return true;
}

bool AUIRadioButton::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemRadio = new AXMLItem;
	pXMLItemRadio->InsertItem(pXMLItemDialog);
	pXMLItemRadio->SetName(_AL("RADIO"));
	XMLSET_STRING_VALUE(pXMLItemRadio, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("Height"), m_nHeight, 0)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("Align"), m_nAlign, AUIFRAME_ALIGN_CENTER)
	XMLSET_BOOL_VALUE(pXMLItemRadio, _AL("Anomaly"), m_bAnomaly, false)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("ButtonID"), m_idButton, -1)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("GroupID"), m_idGroup, -1)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("TextOffsetX"), m_nTextOffsetX, 0)
	XMLSET_INT_VALUE(pXMLItemRadio, _AL("TextOffsetY"), m_nTextOffsetY, 0)
	XMLSET_STRING_VALUE(pXMLItemRadio, _AL("SoundEffect"), AS2AC(m_strSound), _AL(""))
	XMLSET_STRING_VALUE(pXMLItemRadio, _AL("HoverSoundEffect"), AS2AC(m_strSoundHover), _AL(""))
	XMLSET_STRING_VALUE(pXMLItemRadio, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemRadio);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	XMLSET_STRING_VALUE(pXMLItemRadio, _AL("Command"), AS2AC(m_strCommand), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemRadio);
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
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("CheckColor"), m_bCheckColor, false)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("TextCheckColor"), m_clCheck, _AL("255,255,255,255"))

	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemRadio);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pA2DSpriteNormal )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("NormalImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pA2DSpriteNormal->GetName()), _AL(""))
	}
	if( m_pA2DSpriteChecked )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("CheckedImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pA2DSpriteChecked->GetName()), _AL(""))
	}
	if( m_pA2DSpriteOnHover )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("OnHoverImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pA2DSpriteOnHover->GetName()), _AL(""))
	}
	if( m_pA2DSpriteCheckedOnHover )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("CheckedOnHoverImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pA2DSpriteCheckedOnHover->GetName()), _AL(""))
	}

	return true;
}

bool AUIRadioButton::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( WM_LBUTTONUP == msg )
	{
#ifdef _ANGELICA_AUDIO
		if( m_pAudio )
			m_pParent->GetAUIManager()->PlayAudioInstance(m_pAudio);
		else if( strlen(m_strSound) > 0 )
			m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#else
		if( strlen(m_strSound) > 0 )
			m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Play2DAutoSound(m_strSound);
#endif

		if( !IsChecked() )
		{
			m_pParent->CheckRadioButton(m_idGroup, m_idButton);
			if( m_strCommand != "" )
				m_pParent->SetCommand(m_strCommand);
			else
				m_pParent->SetCommand(m_szName);
		}

		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIRadioButton::Tick(void)
{
	PAUIOBJECT pMouseOnObj;
	m_pParent->GetAUIManager()->GetMouseOn(NULL, &pMouseOnObj);
	PAUIDIALOG pDlgActive = m_pParent->GetAUIManager()->GetActiveDialog();
	if( pMouseOnObj == this && !(pDlgActive && pDlgActive->IsModal() && pDlgActive != m_pParent) )
	{
		if( IsChecked() && m_pA2DSpriteCheckedOnHover || !IsChecked() && m_pA2DSpriteOnHover)
			SetHover(true);
		else
			SetHover(false);
	}
	else
		SetHover(false);

	return AUIObject::Tick();
}

bool AUIRadioButton::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		if( !m_pA2DSpriteNormal && !m_pA2DSpriteChecked )
			return true;

		A3DCOLOR color = GetRenderColor();
		A2DSprite* pA2DSprite = GetRenderSprite();	

		POINT ptText, ptSprite;
		CalcAlignPos(pA2DSprite, ptText, ptSprite);

		if( pA2DSprite )
		{
			bool bval = true;
			if( m_pParent->IsNoFlush() )
				bval = pA2DSprite->DrawToBack(ptSprite.x, ptSprite.y);
			else
				bval = pA2DSprite->DrawToInternalBuffer(ptSprite.x, ptSprite.y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUICheckBox::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer()");
		}

		if( m_szText.GetLength() > 0 )
		{			
			int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);	
			A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
			float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
			if (!m_bVerticalText)
				pFont->TextOut(ptText.x + m_nTextOffsetX * fWindowScale, ptText.y + m_nTextOffsetY * fWindowScale, m_szText, (nAlpha << 24) | (0xFFFFFF & color),
					(m_nShadow & 0xFF000000) ? true : false, (nAlpha << 24) | (0xFFFFFF & m_nShadow));
			else
				pFont->TextOutVert(ptText.x + m_nTextOffsetX * fWindowScale, ptText.y + m_nTextOffsetY * fWindowScale, m_szText, (nAlpha << 24) | (0xFFFFFF & color),
					(m_nShadow & 0xFF000000) ? true : false, (nAlpha << 24) | (0xFFFFFF & m_nShadow));
		}
	}

	return true;
}

bool AUIRadioButton::IsChecked(void)
{
	return m_bCheck;
}

void AUIRadioButton::Check(bool bCheck)
{
	if( m_bCheck != bCheck )
		UpdateRenderTarget();

	m_bCheck = bCheck;
}

void AUIRadioButton::SetHover(bool bHover)
{
	if( m_bHover == bHover )
		return;

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
int AUIRadioButton::GetGroupID(void)
{
	return m_idGroup;
}

void AUIRadioButton::SetGroupID(int idGroup)
{
	m_idGroup = idGroup;
}

int AUIRadioButton::GetButtonID(void)
{
	return m_idButton;
}

void AUIRadioButton::SetButtonID(int idButton)
{
	m_idButton = idButton;
}

bool AUIRadioButton::GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Align") )
		Property->i = m_nAlign;
	else if( 0 == strcmpi(pszPropertyName, "Normal Image File") )
	{
		if( m_pA2DSpriteNormal )
			strncpy(Property->fn, m_pA2DSpriteNormal->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Checked Image File") )
	{
		if( m_pA2DSpriteChecked )
			strncpy(Property->fn, m_pA2DSpriteChecked->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "OnHover Image File") )
	{
		if( m_pA2DSpriteOnHover )
			strncpy(Property->fn, m_pA2DSpriteOnHover->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "CheckedOnHover Image File") )
	{
		if( m_pA2DSpriteCheckedOnHover )
			strncpy(Property->fn, m_pA2DSpriteCheckedOnHover->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Group ID") )
		Property->i = m_idGroup;
	else if( 0 == strcmpi(pszPropertyName, "Button ID") )
		Property->i = m_idButton;
	else if( 0 == strcmpi(pszPropertyName, "Text Offset X") )
		Property->i = m_nTextOffsetX;
	else if( 0 == strcmpi(pszPropertyName, "Text Offset Y") )
		Property->i = m_nTextOffsetY;
	else if( 0 == strcmpi(pszPropertyName, "Vertical Text") )
		Property->b = m_bVerticalText;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIRadioButton::SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Align") )
		m_nAlign = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Normal Image File") )
		return InitIndirect(Property->fn, AUIRADIOBUTTON_ITEM_NORMAL);
	else if( 0 == strcmpi(pszPropertyName, "Checked Image File") )
		return InitIndirect(Property->fn, AUIRADIOBUTTON_ITEM_CHECKED);
	else if( 0 == strcmpi(pszPropertyName, "OnHover Image File") )
		return InitIndirect(Property->fn, AUIRADIOBUTTON_ITEM_ONHOVER);
	else if( 0 == strcmpi(pszPropertyName, "CheckedOnHover Image File") )
		return InitIndirect(Property->fn, AUIRADIOBUTTON_ITEM_CHECKEDONHOVER);
	else if( 0 == strcmpi(pszPropertyName, "Group ID") )
		m_idGroup = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Button ID") )
		m_idButton = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Text Offset X") )
		m_nTextOffsetX = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Text Offset Y") )
		m_nTextOffsetY = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Vertical Text") )
		m_bVerticalText = Property->b;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIRadioButton::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Normal Image File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Checked Image File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Group ID", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Button ID", AUIOBJECT_PROPERTY_VALUE_INT);
}

bool AUIRadioButton::IsInHitArea(int x, int y, UINT msg)
{
	if( !AUIObject::IsInHitArea(x, y, msg) )
		return false;

	if( !m_bAnomaly )
		return true;

	A2DSprite *pA2DSprite = NULL;
	if( IsChecked() )
		pA2DSprite = m_pA2DSpriteChecked;
	else
		pA2DSprite = m_pA2DSpriteNormal;
	if( !pA2DSprite )
		return false;
	
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	DWORD color = AUI_A2DSpriteGetPixel(pA2DSprite, int((x - GetPos().x) / fWindowScale), int((y - GetPos().y) / fWindowScale));
	return A3DCOLOR_GETALPHA(color) > 0;
}

void AUIRadioButton::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIRadioButton::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pA2DSpriteNormal)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteNormal, bLoad);

	if (m_pA2DSpriteChecked)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteChecked, bLoad);

	if (m_pA2DSpriteOnHover)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteOnHover, bLoad);

	if (m_pA2DSpriteCheckedOnHover)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteCheckedOnHover, bLoad);
}

void AUIRadioButton::SetVerticalText(bool bVertical)
{
	m_bVerticalText = bVertical;
}

bool AUIRadioButton::IsVerticalText() const
{
	return m_bVerticalText;
}

void AUIRadioButton::CalcAlignPos(A2DSprite* pA2DSprite, POINT& ptText, POINT& ptSprite)
{
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	A3DPOINT2 ptSize = pFont->GetTextExtent(m_szText);
	
	POINT ptPos = GetPos();	
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;
	
	if (!m_bVerticalText)
	{
		ptText.y = ptPos.y + (m_nHeight - ptSize.y) / 2;
		ptSprite.y = ptPos.y;
		int nSpriteW = (pA2DSprite==NULL ? 0 : (pA2DSprite->GetWidth() * pA2DSprite->GetScaleX()));
		switch (m_nAlign)
		{
		case AUIFRAME_ALIGN_CENTER:
			ptSprite.x = ptPos.x;
			ptText.x = ptPos.x + (m_nWidth - ptSize.x) / 2;
			break;
		case AUIFRAME_ALIGN_LEFT:
			ptSprite.x = ptPos.x + m_nWidth - nSpriteW;
			ptText.x = ptPos.x;
			break;
		case AUIFRAME_ALIGN_RIGHT:
			ptSprite.x = ptPos.x;
			ptText.x = ptPos.x + nSpriteW;
			break;
		}
	}
	else
	{
		ptText.x = ptPos.x + (m_nWidth - pFont->GetFontWidth()) / 2;
		ptSprite.x = ptPos.x;
		int nSpriteH = (pA2DSprite==NULL ? 0 : (pA2DSprite->GetHeight() * pA2DSprite->GetScaleY()));
		switch (m_nAlign)
		{
		case AUIFRAME_ALIGN_CENTER:
			ptSprite.y = ptPos.y;
			ptText.y = ptPos.y + (m_nHeight - pFont->GetFontHeight() * m_szText.GetLength()) / 2;
			break;
		case AUIFRAME_ALIGN_LEFT:
			ptSprite.y = ptPos.y + m_nHeight - nSpriteH;
			ptText.y = ptPos.y;
			break;
		case AUIFRAME_ALIGN_RIGHT:
			ptSprite.y = ptPos.y;
			ptText.y = ptPos.y + nSpriteH;
			break;
		}
	}
}

A2DSprite* AUIRadioButton::GetRenderSprite()
{
	A2DSprite *pA2DSprite = NULL;
	if( IsChecked() )
	{
		if( m_bHover )
			pA2DSprite = m_pA2DSpriteCheckedOnHover;
		else
			pA2DSprite = m_pA2DSpriteChecked;
	}
	else
	{
		if( m_bHover )
			pA2DSprite = m_pA2DSpriteOnHover;
		else
			pA2DSprite = m_pA2DSpriteNormal;
	}
	if (!pA2DSprite)
		return NULL;
	
	float fScaleX = 1.0f;
	float fScaleY = 1.0f;
	if( m_nAlign == AUIFRAME_ALIGN_CENTER )
	{
		fScaleX = float(m_nWidth) / float(pA2DSprite->GetWidth());
		fScaleY = float(m_nHeight) / float(pA2DSprite->GetHeight());
	}
	else
	{
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		if( m_pA2DSpriteNormal )
		{
			fScaleX = float(m_nDefaultWidth) / float(m_pA2DSpriteNormal->GetWidth()) * fWindowScale;
			fScaleY = float(m_nDefaultHeight) / float(m_pA2DSpriteNormal->GetHeight()) * fWindowScale;

			if (!m_bVerticalText)
				fScaleX = fScaleY;
			else
				fScaleY = fScaleX;
		}
		else
		{
			fScaleX = float(m_nDefaultWidth) / float(m_pA2DSpriteChecked->GetWidth()) * fWindowScale;
			fScaleY = float(m_nDefaultHeight) / float(m_pA2DSpriteChecked->GetHeight()) * fWindowScale;
			if (!m_bVerticalText)
				fScaleX = fScaleY;
			else
				fScaleY = fScaleX;
		}
	}
	
	pA2DSprite->SetScaleX(fScaleX);
	pA2DSprite->SetScaleY(fScaleY);

	if( !m_bEnabled )
		pA2DSprite->SetColor(A3DCOLORRGB(160, 160, 160));
	else
		pA2DSprite->SetColor(A3DCOLORRGB(255, 255, 255));

	pA2DSprite->SetAlpha(int(m_pParent->GetWholeAlpha() * m_nAlpha / 255));

	return pA2DSprite;
}

inline A3DCOLOR AUIRadioButton::GetRenderColor()
{
	A3DCOLOR color = m_color;
	if( IsChecked() && m_bCheckColor)
	{
		color = m_clCheck;
	}
		
	if( !m_bEnabled )
	{
		A3DCOLOR r = A3DCOLOR_GETRED(m_color) / 2;
		A3DCOLOR g = A3DCOLOR_GETRED(m_color) / 2;
		A3DCOLOR b = A3DCOLOR_GETRED(m_color) / 2;
		color = A3DCOLORRGB(r, g, b);
	}

	return color;
}