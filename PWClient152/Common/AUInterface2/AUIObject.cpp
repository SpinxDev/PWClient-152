// Filename	: AUIObject.cpp
// Creator	: Tom Zhou
// Date		: May 12, 2004
// Desc		: AUIObject is the base class of all other AUI controls.

#pragma warning (disable: 4284)

#include "AUI.h"

#include <Windows.h>

#include "A3DGDI.h"
#include "A3DMacros.h"
#include "AMemFile.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "AUICommon.h"
#include "AUIFrame.h"
#include "AUIObject.h"
#include "AUIDialog.h"
#include "AUIManager.h"
#include "AUICTranslate.h"
#include "FTInterface.h"
#include "A3DFTFont.h"
#include "AUIObjectTemplateManager.h"

AUIObject::AUIObject()
{
	m_nType = AUIOBJECT_TYPE_NULL;
	m_bShow = true;
	m_bFocus = false;
	m_bEnabled = true;
	m_bTransparent = false;
	m_bAcceptMouseMsg = true;
	m_bFlash = false;
	m_bAnomaly = false;
	m_nState = AUIOBJECT_STATE_UNKNOWN;
	m_x = 0;
	m_y = 0;
	m_nOffX = 0;
	m_nOffY = 0;
	m_nVPW = 0;
	m_nVPH = 0;
	m_nWidth = 40;
	m_nHeight = 40;

	m_pParent = NULL;
	m_pA3DDevice = NULL;
	m_pA3DEngine = NULL;
	m_pA3DViewport = NULL;

	m_strCommand = "";
	m_strSound = "";
	m_strSoundHover = "";

	char szName[128];
	sprintf(szName, "Control_%d", rand() * rand());
	m_szName = szName;

	m_nHintMaxWidth = 0;
	m_bHintFollow = false;

	m_nFlashTime = m_nTime = GetTickCount();
	m_bBlink = false;
	m_bBlinkText = false;
	m_szText = _AL("");
	m_szFontName = _AL("");
	m_nFontHeight = 15;
	m_fDisplayHeight = 15;
	m_nShadow = 0;
	m_bBold = false;
	m_bItalic = false;
	m_bFreeType = true;
	m_bFreeTypeInit = true;
	m_bAutoIndent = true;
	m_pFont = NULL;
	m_color = A3DCOLORRGB(255, 255, 255);
	m_nAlign = AUIFRAME_ALIGN_CENTER;
	m_nAlpha = 255;
	m_nOldAlpha = 255;
	m_nOutline = 0;
	m_clOutline = A3DCOLORRGB(0, 0, 0);
	m_clInnerText = A3DCOLORRGB(255, 255, 255);
	m_bInterpColor = false;
	m_clUpper = A3DCOLORRGB(255, 255, 255);
	m_clLower = A3DCOLORRGB(255, 255, 255);
	m_bHoverColor = false;
	m_clHover = A3DCOLORRGB(255, 255, 255);
	m_bCheckColor = false;
	m_clCheck = A3DCOLORRGB(255, 255, 255);
	m_nLineSpace = 0;
	m_bDynamicRender = false;
	m_bForceDynamicRender = false;
	m_nOptimizeResourceState = 1;

	m_dwParamCallback1 = 0;
	m_dwParamCallback2 = 0;
	m_dwParamCallback3 = 0;
	m_pRenderHintCallback = NULL;

	m_dwData = 0;
	m_pvData = NULL;
	m_strDataName = "";
	m_strDataPtrName = "";
	m_uiData64 = 0;
	m_uiDataPtr64 = 0;
	m_strData64Name = "";
	m_strDataPtr64Name = "";

	m_nDefaultX = 0;
	m_nDefaultY = 0;
	m_nDefaultWidth = 0;
	m_nDefaultHeight = 0;

	m_strTemplate = "";

#ifdef _ANGELICA_AUDIO
	m_pAudio = NULL;
	m_pAudioHover = NULL;
#endif
}

AUIObject::~AUIObject()
{
}

bool AUIObject::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	ASSERT(pA3DEngine && pA3DDevice);

	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;

	// We only need a empty control to be setted up, if pASF is NULL.
	if (!pASF)
		return true;

	// pASF is not NULL, read param from file
	
	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));
	
	m_szName = pASF->m_szToken;
	if( strlen(m_szName) == 0 || 0 == strcmpi(m_szName, "Static") )
	{
		char szName[AFILE_LINEMAXLEN];
		sprintf(szName, "Static_%d", rand() * rand());
		m_szName = szName;
	}
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	SetCommand(pASF->m_szToken);
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_strSound = pASF->m_szToken;
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_x = atoi(pASF->m_szToken);
	m_nDefaultX = m_x;
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_y = atoi(pASF->m_szToken);
	m_nDefaultY = m_y;
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nWidth = atoi(pASF->m_szToken);
	m_nDefaultWidth= m_nWidth;
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nHeight = atoi(pASF->m_szToken);
	m_nDefaultHeight = m_nHeight;

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIObject::Init, Failed to load from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIObject::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	return true;
}

bool AUIObject::InitIndirect(const char *pszFileName, int idType)
{
	return true;
}

bool AUIObject::Release(void)
{
#ifdef _ANGELICA_AUDIO
	//if (m_pParent && m_pParent->GetAUIManager())
	//{
	//	m_pParent->GetAUIManager()->DestroyAudioEvtInstance(m_pAudio);
	//	m_pParent->GetAUIManager()->DestroyAudioEvtInstance(m_pAudioHover);
	//}
#endif
	return true;
}

POINT AUIObject::GetPos(bool bLocal) const 
{
	POINT ptPos;

	ptPos.x = m_x;
	ptPos.y = m_y;

	if( !bLocal )
	{
		POINT ptPosParent = m_pParent->GetPos();

		ptPos.x += ptPosParent.x;
		ptPos.y += ptPosParent.y;
	}

	return ptPos;
}

void AUIObject::SetPos(int x, int y)
{
	if( m_x != x || m_y != y )
		UpdateRenderTarget();

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();

	// 由当前位置改写未缩放前的位置的风险：舍入误差的积累。
	// 现在暂时+0.5让舍入方向平均化，减少其带来的影响。
	// TODO: 更好的解决方案：DefaultX用float，再存一个float型的基准缩放系数DefaultScale。在SetPos的时候，求出变化量，然后再换算成DefaultScale下的变化量，加到DefaultX上。
	if( x != m_x )
	{
		m_x = x;
		m_nDefaultX = x / fWindowScale + 0.5f;
	}
	if( y != m_y )
	{
		m_y = y;
		m_nDefaultY = y / fWindowScale + 0.5f;
	}
}

void AUIObject::SetName(const char *pszName)
{
	m_pParent->m_AUIObjectTable.erase(m_szName);
	m_szName = pszName;
	m_pParent->m_AUIObjectTable.put(m_szName, this);
}

void AUIObject::Show(bool bShow)
{
	if( m_bShow != bShow )
		UpdateRenderTarget();

	m_bShow = bShow;

	if (!bShow)
	{
		if (m_pParent)
		{
			if (m_pParent->GetCaptureObject() == this)
			{
				m_pParent->SetCaptureObject(NULL);
			}
		}
	}
}

void AUIObject::Enable(bool bEnable)
{
	if( m_bEnabled != bEnable )
		UpdateRenderTarget();

	m_bEnabled = bEnable;
}

bool AUIObject::Save(FILE *file)
{
	// We don't save control keyword here.
	fprintf(file, " \"%s\" \"%s\" \"%s\" %d %d %d %d", m_szName,
		m_strCommand, m_strSound, m_x, m_y, m_nWidth, m_nHeight);

	return true;
}

bool AUIObject::SaveXML(AXMLItem *pXMLItemDialog)
{
	return true;
}

bool AUIObject::Tick(void)
{
	int nTimeNow = GetTickCount();
	if( m_bBlinkText )
	{
		if( nTimeNow - m_nTime >= AUIOBJECT_BLINK_INTERVAL )
		{
			m_bBlink = !m_bBlink;
			m_nTime = nTimeNow;
		}
	}

	if( m_bFlash )
	{
		if( nTimeNow - m_nFlashTime >= AUIOBJECT_FLASH_INTERVAL )
		{
			if( m_nAlpha == m_nOldAlpha )
			{
				if( m_nOldAlpha >= 160 )
					m_nAlpha = m_nOldAlpha - 96;
				else
					m_nAlpha = m_nOldAlpha + 96;
			}
			else
				m_nAlpha = m_nOldAlpha;
			m_nFlashTime = nTimeNow;
		}
	}

	return true;
}

bool AUIObject::Render(void)
{
//	extern A3DGDI *g_pA3DGDI;
//	POINT ptPos = GetPos();
//
//	ptPos.x += m_nOffX;
//	ptPos.y += m_nOffY;
//
//	A3DRECT rect(ptPos.x, ptPos.y, ptPos.x + m_nWidth - 1, ptPos.y + m_nHeight - 1);
//	g_pA3DGDI->Draw2DRectangle(rect, m_color);

	return true;
}

bool AUIObject::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

SIZE AUIObject::GetSize(void)
{
	SIZE sz;
	sz.cx = m_nWidth;
	sz.cy = m_nHeight;
	return sz;
}

void AUIObject::SetSize(int W, int H)
{
	if (W < 0) W = 0;
	if (H < 0) H = 0;

	if( m_nWidth != W || m_nHeight != H )
		UpdateRenderTarget();

	AUIOBJECT_SETPROPERTY p;
	if( W != m_nWidth )
	{
		p.i = W;
		SetProperty("Width", &p);
	}

	if( H != m_nHeight )
	{
		p.i = H;
		SetProperty("Height", &p);
	}
}

A3DRECT AUIObject::GetRect(void)
{
	POINT ptPos = GetPos();
	A3DRECT rc(ptPos.x, ptPos.y, ptPos.x + m_nWidth, ptPos.y + m_nHeight);
	return rc;
}

bool AUIObject::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Name") )
		strncpy(Property->c, m_szName, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Hint") )
	{
		AUICTranslate trans;
		a_strncpy(Property->s, trans.ReverseTranslate(m_szHint), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Command") )
		strncpy(Property->cmd, m_strCommand, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Sound Effect") )
		strncpy(Property->fn, m_strSound, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Hover Sound Effect") )
		strncpy(Property->fn, m_strSoundHover, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "X") )
		Property->i = m_x;
	else if( 0 == strcmpi(pszPropertyName, "Y") )
		Property->i = m_y;
	else if( 0 == strcmpi(pszPropertyName, "Width") )
		Property->i = m_nWidth;
	else if( 0 == strcmpi(pszPropertyName, "Height") )
		Property->i = m_nHeight;
	else if( 0 == strcmpi(pszPropertyName, "Text Color") )
		Property->dw = m_color;
	else if( 0 == strcmpi(pszPropertyName, "Outline Color") )
		Property->dw = m_clOutline;
	else if( 0 == strcmpi(pszPropertyName, "Inner Text Color") )
		Property->dw = m_clInnerText;
	else if( 0 == strcmpi(pszPropertyName, "Text Upper Color") )
		Property->dw = m_clUpper;
	else if( 0 == strcmpi(pszPropertyName, "Text Lower Color") )
		Property->dw = m_clLower;
	else if( 0 == strcmpi(pszPropertyName, "Hover Color") )
		Property->b = m_bHoverColor;
	else if( 0 == strcmpi(pszPropertyName, "Text Hover Color") )
		Property->dw = m_clHover;
	else if( 0 == strcmpi(pszPropertyName, "Check Color") )
		Property->b = m_bCheckColor;
	else if( 0 == strcmpi(pszPropertyName, "Text Check Color") )
		Property->dw = m_clCheck;
	else if( 0 == strcmpi(pszPropertyName, "Font") )
	{
		a_strncpy(Property->font.szName, m_szFontName, AFILE_LINEMAXLEN - 1);
		Property->font.nHeight = m_nFontHeight;
		Property->font.bBold = m_bBold;
		Property->font.bItalic = m_bItalic;
	}
	else if( 0 == strcmpi(pszPropertyName, "Shadow") )
		Property->dw = m_nShadow;
	else if( 0 == strcmpi(pszPropertyName, "Outline") )
		Property->i = m_nOutline;
	else if( 0 == strcmpi(pszPropertyName, "Fade Color") )
		Property->b = m_bInterpColor;
	else if( 0 == strcmpi(pszPropertyName, "FreeType") )
		Property->b = m_bFreeType;
	else if( 0 == strcmpi(pszPropertyName, "Text Content") )
		a_strncpy(Property->s, m_szText, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Text Align") )
		Property->i = m_nAlign;
	else if( 0 == strcmpi(pszPropertyName, "Anomaly") )
		Property->b = m_bAnomaly;
	else if( 0 == strcmpi(pszPropertyName, "Line Space") )
		Property->i = m_nLineSpace;
	else if( 0 == strcmpi(pszPropertyName, "Force Dynamic Render") )
		Property->b = m_bForceDynamicRender;
	else if( 0 == strcmpi(pszPropertyName, "Template") )
		strncpy(Property->c, m_strTemplate, AFILE_LINEMAXLEN - 1);

	return true;
}

bool AUIObject::SetProperty(const  char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Name") )
		SetName(Property->c);
	else if( 0 == strcmpi(pszPropertyName, "Hint") )
	{
		AUICTranslate trans;
		m_szHint = trans.Translate(Property->s);
	}
	else if( 0 == strcmpi(pszPropertyName, "Command") )
		m_strCommand = Property->cmd;
	else if( 0 == strcmpi(pszPropertyName, "Sound Effect") )
	{
		m_strSound = Property->fn;
#ifdef _ANGELICA_AUDIO
		LoadAudioInstance(m_strSound, m_pAudio);
#endif
	}
	else if( 0 == strcmpi(pszPropertyName, "Hover Sound Effect") )
	{
		m_strSoundHover = Property->fn;
#ifdef _ANGELICA_AUDIO
		LoadAudioInstance(m_strSoundHover, m_pAudioHover);
#endif
	}
	else if( 0 == strcmpi(pszPropertyName, "X") )
	{
		m_x = Property->i;
		m_nDefaultX = m_x / (m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( 0 == strcmpi(pszPropertyName, "Y") )
	{
		m_y = Property->i;
		m_nDefaultY = m_y / (m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( 0 == strcmpi(pszPropertyName, "Width") )
	{
		m_nWidth = Property->i;
		m_nDefaultWidth = m_nWidth / (m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( 0 == strcmpi(pszPropertyName, "Height") )
	{
		m_nHeight = Property->i;
		m_nDefaultHeight = m_nHeight / (m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( 0 == strcmpi(pszPropertyName, "Text Color") )
		SetColor(Property->dw);
	else if( 0 == strcmpi(pszPropertyName, "Outline Color") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType, Property->dw, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower);
	else if( 0 == strcmpi(pszPropertyName, "Inner Text Color") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType, m_clOutline, Property->dw, m_bInterpColor, m_clUpper, m_clLower);
	else if( 0 == strcmpi(pszPropertyName, "Text Upper Color") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType, m_clOutline, m_clInnerText, m_bInterpColor, Property->dw, m_clLower);
	else if( 0 == strcmpi(pszPropertyName, "Text Lower Color") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, Property->dw);
	else if( 0 == strcmpi(pszPropertyName, "Hover Color") )
		m_bHoverColor = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Text Hover Color") )
		m_clHover = Property->dw;
	else if( 0 == strcmpi(pszPropertyName, "Check Color") )
		m_bCheckColor = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Text Check Color") )
		m_clCheck = Property->dw;
	else if( 0 == strcmpi(pszPropertyName, "Font") )
	{
		SetFontAndColor(Property->font.szName, Property->font.nHeight, m_color,
			m_nShadow, m_nOutline, Property->font.bBold, Property->font.bItalic, m_bFreeType, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower);
	}
	else if( 0 == strcmpi(pszPropertyName, "Shadow") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, Property->dw, m_nOutline, m_bBold, m_bItalic, m_bFreeType, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower);
	else if( 0 == strcmpi(pszPropertyName, "Outline") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, Property->i, m_bBold, m_bItalic, m_bFreeType, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower);
	else if( 0 == strcmpi(pszPropertyName, "Fade Color") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType, m_clOutline, m_clInnerText, Property->b, m_clUpper, m_clLower);
	else if( 0 == strcmpi(pszPropertyName, "FreeType") )
		SetFontAndColor(m_szFontName, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, Property->b, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower);
	else if( 0 == strcmpi(pszPropertyName, "Text Content") )
		SetText(Property->s);
	else if( 0 == strcmpi(pszPropertyName, "Text Align") )
		m_nAlign = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Anomaly") )
		m_bAnomaly = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Line Space") )
		SetLineSpace(Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Force Dynamic Render") )
		SetForceDynamicRender(Property->b);
	else if( 0 == strcmpi(pszPropertyName, "Template") )
		SetTemplateName(Property->c);

	return true;
}

bool AUIObject::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Name", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Command", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Sound Effect", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("X", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Y", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Width", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Height", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Text Color", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Shadow", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Outline", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Text Content", AUIOBJECT_PROPERTY_VALUE_ACHAR) &&
			SetAUIObjectPropertyType("Text Align", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Anomaly", AUIOBJECT_PROPERTY_VALUE_BOOL) &&
			SetAUIObjectPropertyType("Template", AUIOBJECT_PROPERTY_VALUE_ACHAR);
}

void AUIObject::SetFocus(bool bFocus)
{
	m_bFocus = bFocus;
}

const ACHAR * AUIObject::GetText(void)
{
	return m_szText;
}

void AUIObject::SetText(const ACHAR *pszText)
{
	ASSERT(pszText);

	if( (pszText && a_strcmp(pszText, m_szText) != 0) || (!pszText && !m_szText.IsEmpty()))
		UpdateRenderTarget();

	m_szText = pszText;
}

int AUIObject::RetrievePropertyList(PAUIOBJECT_PROPERTY a_Property)
{
	return AUI_RetrieveControlPropertyList(GetType(), a_Property);
}

bool AUIObject::SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
	A3DCOLOR dwColor, int nShadow, int nOutline, bool bBold, bool bItalic, bool bFreeType,
	A3DCOLOR clOutline, A3DCOLOR clInnerText, bool bInterpColor, A3DCOLOR clUpper, A3DCOLOR clLower)
{
	ACString strFontName = pszFontName;
	if( pszFontName )
	{
		int nStyle = STYLE_NORMAL;
		if( bBold ) nStyle |= STYLE_BOLD;
		if( bItalic ) nStyle |= STYLE_ITALIC;

		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		if( strFontName != _AL("方正细黑一简体") &&
			strFontName != _AL("方正隶二简体") &&
			strFontName != _AL("方正隶变简体") &&
			strFontName != _AL("宋体") )
		{
			nFontHeight = m_pParent->GetAUIManager()->GetDefaultFontSize();
			strFontName = m_pParent->GetAUIManager()->GetDefaultFontName();
			if( strFontName != _AL("方正细黑一简体") &&
				strFontName != _AL("方正隶二简体") &&
				strFontName != _AL("方正隶变简体") &&
				strFontName != _AL("宋体") )
				strFontName = _AL("方正细黑一简体");
			nStyle = STYLE_NORMAL;
			nOutline = 0;
			m_pFont = m_pParent->GetAUIManager()->GetA3DFTFontMan()->
				CreatePointFont(fWindowScale * nFontHeight, strFontName, nStyle, bFreeType ? 0 : 1, nOutline, clOutline, clInnerText, bInterpColor, clUpper, clLower);
		}

		m_pFont = m_pParent->GetAUIManager()->GetA3DFTFontMan()->
			CreatePointFont(fWindowScale * nFontHeight, strFontName, nStyle, bFreeType ? 0 : 1, nOutline, clOutline, clInnerText, bInterpColor, clUpper, clLower);
		if( !m_pFont ) 
		{	
			if(AUI_IsLogEnabled())
			{
				a_LogOutput(1, "AUIObject::SetFontAndColor, Failed to create font");
			}
			return false;
		}
		
		A3DPOINT2 ptSize;
		ptSize = m_pFont->GetTextExtent(_AL("Wjg"));
		m_szFontName = strFontName;
		m_nFontHeight = nFontHeight;
		m_fDisplayHeight = nFontHeight * m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		m_nShadow = nShadow;
		m_nOutline = nOutline;
		m_bBold = bBold;
		m_bItalic = bItalic;
		m_bFreeType = bFreeType;
		m_bFreeTypeInit = m_bFreeType;
		m_clOutline = clOutline;
		m_clInnerText = clInnerText;
		m_bInterpColor = bInterpColor;
		m_clUpper = clUpper;
		m_clLower = clLower;
	}

	SetColor(dwColor);

	return true;
}

bool AUIObject::Resize()
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	m_fDisplayHeight = m_nFontHeight * fWindowScale;
	if( m_pFont && m_fDisplayHeight > 0 )
	{
		if (m_pParent->GetAUIManager()->IsUseFontAdapt())
		{
			FitFont((fWindowScale >1.05f || fWindowScale < 0.95 ) ? 0 : !m_bFreeTypeInit);
// 			int nStyle = m_pFont->GetStyle();
// 			m_pParent->GetAUIManager()->GetA3DFTFontMan()->ReleasePointFont(m_pFont);
// 			m_pFont = m_pParent->GetAUIManager()->GetA3DFTFontMan()->
// 				CreatePointFont(m_fDisplayHeight, m_szFontName, nStyle, (fWindowScale >1.01f || fWindowScale < 0.99 ) ? 0 : !m_bFreeType, m_nOutline, m_clOutline, m_clInnerText, m_bInterpColor, m_clUpper, m_clLower);
		}
		else
		{
			FitFont();
		}
	}
	m_x = m_nDefaultX * fWindowScale;
	m_y = m_nDefaultY * fWindowScale;
	m_nWidth = int((m_nDefaultX + m_nDefaultWidth) * fWindowScale) - m_x;
	m_nHeight = int((m_nDefaultY + m_nDefaultHeight) * fWindowScale) - m_y;
	if( m_nDefaultWidth != 0 && m_nWidth == 0 )
		m_nWidth = 1;
	if( m_nDefaultHeight != 0 && m_nHeight == 0 )
		m_nHeight = 1;

	return true;
}

void AUIObject::SetColor(A3DCOLOR color)
{
	if( m_color != color )
		UpdateRenderTarget();

	m_color = color;
}

DWORD AUIObject::GetData(AString strName)
{
	if (0 != m_dwData && strName != m_strDataName)
		AUI_ReportError(__LINE__, 1, "AUIObject::GetData(), data name not match");
	return m_dwData;
}

void AUIObject::SetData(DWORD dwData, AString strName)
{
	m_strDataName = strName;
	m_dwData = dwData;
}

void * AUIObject::GetDataPtr(AString strName)
{
	if (0 != m_pvData && strName != m_strDataPtrName)
		AUI_ReportError(__LINE__, 1, "AUIObject::GetDataPtr(), data name not match");
	return m_pvData;
}

void AUIObject::SetDataPtr(void *pvData, AString strName)
{
	m_strDataPtrName = strName;
	m_pvData = pvData;
}

void AUIObject::ForceGetData(DWORD &dwData, AString &strName)
{
	dwData = m_dwData;
	strName = m_strDataName;
}

void AUIObject::ForceGetDataPtr(void *&pvData, AString &strName)
{
	pvData = m_pvData;
	strName = m_strDataPtrName;
}

UINT64 AUIObject::GetData64(AString strName)
{
	if (0 != m_uiData64 && strName != m_strData64Name)
		AUI_ReportError(__LINE__, 1, "AUIObject::GetData64(), data name not match");
	return m_uiData64;
}

void AUIObject::SetData64(UINT64 uiData64, AString strName)
{
	m_strData64Name = strName;
	m_uiData64 = uiData64;
}

void AUIObject::ForceGetData64(UINT64 &uiData64, AString &strName)
{
	uiData64 = m_uiData64;
	strName = m_strData64Name;
}

UINT64 AUIObject::GetDataPtr64(AString strName)
{
	if (0 != m_uiDataPtr64 && strName != m_strDataPtr64Name)
		AUI_ReportError(__LINE__, 1, "AUIObject::GetDataPtr64(), data name not match");
	return m_uiDataPtr64;
}

void AUIObject::SetDataPtr64(UINT64 uiDataPtr64, AString strName)
{
	m_strDataPtr64Name = strName;
	m_uiDataPtr64 = uiDataPtr64;
}

void AUIObject::ForceGetDataPtr64(UINT64 &uiDataPtr64, AString &strName)
{
	uiDataPtr64 = m_uiDataPtr64;
	strName = m_strDataPtr64Name;
}

void AUIObject::SetFlash(bool bFlash)
{
	if( bFlash == m_bFlash )
		return;

	SetDynamicRender(bFlash);
	
	if( bFlash )
		m_nOldAlpha = m_nAlpha;
	else
		m_nAlpha = m_nOldAlpha;
	m_bFlash = bFlash;
}

void AUIObject::SetLineSpace(int nSpace)
{
	if( m_nLineSpace != nSpace )
		UpdateRenderTarget();

	m_nLineSpace = nSpace;
}

void AUIObject::SetDynamicRender(bool bDynamic)
{
	if( bDynamic != m_bDynamicRender )
	{
		m_pParent->UpdateRenderTarget();
		m_bDynamicRender = bDynamic;
	}
}

void AUIObject::SetForceDynamicRender(bool bDynamic)
{
	if( bDynamic != m_bForceDynamicRender )
	{
		m_pParent->UpdateRenderTarget();
		m_bForceDynamicRender = bDynamic;
	}
}

void AUIObject::UpdateRenderTarget()
{
	if( !NeedDynamicRender() )
		m_pParent->UpdateRenderTarget();
}

void AUIObject::SetViewportParam(int x, int y, int W, int H)
{
	m_nOffX = x;
	m_nOffY = y;
	m_nVPW = W;
	m_nVPH = H;
}

void AUIObject::SetRenderHintCallback(LPFRENDERHINTCALLBACK pCallback, DWORD param1, DWORD param2, DWORD param3)
{
	m_dwParamCallback1 = param1;
	m_dwParamCallback2 = param2;
	m_dwParamCallback3 = param3;
	m_pRenderHintCallback = pCallback;
}

bool AUIObject::ExecuteRenderHintCallback(POINT ptCursor)
{
	if( !m_pRenderHintCallback )
		return false;

	return (*m_pRenderHintCallback)(ptCursor, m_dwParamCallback1, m_dwParamCallback2, m_dwParamCallback3);
}

bool AUIObject::IsInHitArea(int x, int y, UINT msg)
{
	return GetRect().PtInRect(x, y);
}

int AUIObject::GetRequiredHeight()
{
	return m_nHeight;
}

void AUIObject::SetDefaultPos(int x, int y)
{
	if( m_nDefaultX != x || m_nDefaultY != y )
		UpdateRenderTarget();
	
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( x != m_nDefaultX )
	{
		m_nDefaultX = x;
		m_x = m_nDefaultX * fWindowScale;
	}
	if( y != m_nDefaultY )
	{
		m_nDefaultY = y;
		m_y = m_nDefaultY * fWindowScale;
	}
}

POINT AUIObject::GetDefaultPos() const
{
	POINT pt = {m_nDefaultX, m_nDefaultY};
	return pt;
}

void AUIObject::SetDefaultSize(int W, int H)
{
	if (W < 0) W = 0;
	if (H < 0) H = 0;

	if( m_nDefaultWidth != W || m_nDefaultHeight != H )
		UpdateRenderTarget();

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	if( W != m_nDefaultWidth )
	{
		m_nDefaultWidth = W;
		m_nWidth = m_nDefaultWidth * fWindowScale;
	}

	if( H != m_nDefaultHeight )
	{
		m_nDefaultHeight = H;
		m_nHeight = m_nDefaultHeight * fWindowScale;
	}
}

SIZE AUIObject::GetDefaultSize() const
{
	SIZE sz = {m_nDefaultWidth, m_nDefaultHeight};
	return sz;
}

void AUIObject::FitFont()
{
	if (!m_pFont)
	{ 
		return;
	}
	if (!m_pParent)
	{
		return;
	}
	AUIManager* pAUIMan = m_pParent->GetAUIManager();
	if (!pAUIMan)
	{
		return;
	}
	A3DFTFontMan* pFontMan = pAUIMan->GetA3DFTFontMan();
	if (!pFontMan)
	{
		return;
	}
	m_pFont = pFontMan->CreatePointFont(m_fDisplayHeight, 
										m_pFont->GetFaceName(), 
										m_pFont->GetStyle(),
										m_pFont->GetCreateMode(), 
										m_pFont->GetOutlineWidth(), 
										m_clOutline,  //直接从pfont取有误差
										m_clInnerText,
										m_pFont->IsInterpColor(), 
										m_clUpper,
										m_clLower,
										m_pFont->IsStereo());
}

void AUIObject::FitFont(const int nCreateMode)
{
	if (!m_pFont)
	{ 
		return;
	}
	if(m_pFont->GetPointSize()==int(m_fDisplayHeight)*10 && m_pFont->GetCreateMode()==nCreateMode)
	{
		return;
	}
	if (!m_pParent)
	{
		return;
	}
	AUIManager* pAUIMan = m_pParent->GetAUIManager();
	if (!pAUIMan)
	{
		return;
	}
	A3DFTFontMan* pFontMan = pAUIMan->GetA3DFTFontMan();
	if (!pFontMan)
	{
		return;
	}
	m_pFont = pFontMan->CreatePointFont(m_fDisplayHeight, 
										m_pFont->GetFaceName(), 
										m_pFont->GetStyle(),
										nCreateMode, 
										m_pFont->GetOutlineWidth(), 
										m_clOutline, 
										m_clInnerText,
										m_pFont->IsInterpColor(), 
										m_clUpper,
										m_clLower,
										m_pFont->IsStereo());
}

void AUIObject::SetTemplateName(const char* szTemplate)
{
	m_strTemplate = szTemplate;
}

void AUIObject::AddTemplate(const char* szTemplate)
{
	if (!m_strTemplate.IsEmpty())
		m_strTemplate += AUI_TEMPLATE_TOK;
	m_strTemplate += szTemplate;
}

const char* AUIObject::GetTemplateName() const
{
	return m_strTemplate;
}

bool AUIObject::ApplyTemplate()
{
	if (!AUITemplateManager::GetSingleton().GetCurScheme())
		return false;

	if (m_strTemplate.IsEmpty())
		return true;

	return AUITemplateManager::GetSingleton().GetCurScheme()->TemplateObject(m_pA3DEngine, m_pA3DDevice, *this, m_strTemplate);
}

bool AUIObject::ExportToXml(AXMLItem& xmlItem)
{
	if (!SaveXML(&xmlItem))
		return false;

	return true;
}


bool AUIObject::LoadAudioInstance(const char* szGUID, AudioEngine::Event*& pInst)
{
#ifdef _ANGELICA_AUDIO
	if (!m_pParent || !m_pParent->GetAUIManager())
		return false;

	return m_pParent->GetAUIManager()->LoadAudioEvtInstance(pInst, szGUID);
#else
	return false;
#endif
}