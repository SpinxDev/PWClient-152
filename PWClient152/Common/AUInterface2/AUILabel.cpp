// Filename	: AUILabel.cpp
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUILabel is the class of static text control.

#include "AUI.h"

#include "A3DFTFont.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "CSplit.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "AUICommon.h"
#include "AUIFrame.h"
#include "AUIManager.h"
#include "AUIDialog.h"
#include "AUILabel.h"
#include "AUICTranslate.h"

#define AUILABEL_LINESPACE		3

AUILabel::AUILabel() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_LABEL;
	m_bAcceptMouseMsg = false;
	m_nAlign = AUIFRAME_ALIGN_LEFT;
	m_bMarquee = false;
	m_dwLastTick = 0;
	m_nMarqueePos = 0;
	m_nMarqueeTimes = 0;
	m_nMarqueeSpeed = 0;
	m_pA2DSpriteEmotion = NULL;
	m_pvecImageList = NULL;
	m_pvecOtherFonts = NULL;
	m_nCurEmotionSet = 0;
	m_nItemMask = 0xffffffff;
	m_nLineSpace = AUILABEL_LINESPACE;
	m_bVerticalText = false;
}

AUILabel::~AUILabel()
{
}

bool AUILabel::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	Label property line format:
	 *
	 *		LABEL				Keyword
	 *		Title				Name
	 *		1					String ID of command.
	 *		0 0 200 20			x y Width Height
	 *		3					String ID of font name
	 *		20					Font height
	 *		50					String ID of text content
	 *		0 0 0 255			Text color, R, G, B, A
	 */


	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUILabel::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;

	BEGIN_FAKE_WHILE2;

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
		return AUI_ReportError(__LINE__, 1, "AUILabel::Init(), failed to call SetFontAndColor()");
	
	CHECK_BREAK2(pASF->GetNextToken(true));
	idString = atoi(pASF->m_szToken);
	if( idString >= 0 )
		m_szText = m_pParent->GetStringFromTable(idString);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int r = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int g = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int b = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	int a = atoi(pASF->m_szToken);

	m_color = A3DCOLORRGBA(r, g, b, a);

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUILabel::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUILabel::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	m_szFontName = m_pParent->GetAUIManager()->GetDefaultFontName();
	m_nFontHeight = m_pParent->GetAUIManager()->GetDefaultFontSize();

	m_nWidth = 0;
	m_nHeight = 0;
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Align"), m_nAlign)
	XMLGET_INT_VALUE(pItem, _AL("LineSpace"), m_nLineSpace)
	XMLGET_BOOL_VALUE(pItem, _AL("ForceDynamicRender"), m_bForceDynamicRender)
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
			AUICTranslate trans;
			m_szText = trans.Translate(m_szText);
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
		return AUI_ReportError(__LINE__, 1, "AUILabel::InitXML(), failed to call SetFontAndColor()");

	if( m_pParent )
		SetEmotionList(m_pParent->GetAUIManager()->GetDefaultSpriteEmotion(), m_pParent->GetAUIManager()->GetDefaultEmotion());
	
	return true;
}

bool AUILabel::Release(void)
{
	return AUIObject::Release();
}

bool AUILabel::Save(FILE *file)
{
	fprintf(file, "LABEL");

	if( !AUIObject::Save(file) ) return false;

	ACHAR szFontName[256];
	a_sprintf(szFontName, _AL("%s###%d###%d###%d###%d"),
		m_szFontName, m_nShadow, m_nOutline, m_bBold, m_bItalic);
	int idStrFont = m_pParent->AddStringToTable(szFontName);
	if( idStrFont < 0 ) return false;

	int idStrText = m_pParent->AddStringToTable(m_szText);
	if( idStrText < 0 ) return false;

	fprintf(file, " %d %d %d %d %d %d %d\n", idStrFont, m_nFontHeight,
		idStrText, A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color),
		A3DCOLOR_GETBLUE(m_color), A3DCOLOR_GETALPHA(m_color));

	return true;
}

bool AUILabel::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemLabel = new AXMLItem;
	pXMLItemLabel->InsertItem(pXMLItemDialog);
	pXMLItemLabel->SetName(_AL("LABEL"));
	XMLSET_STRING_VALUE(pXMLItemLabel, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemLabel, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemLabel, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemLabel, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemLabel, _AL("Height"), m_nHeight, 0)
	XMLSET_INT_VALUE(pXMLItemLabel, _AL("Align"), m_nAlign, 0)
	XMLSET_INT_VALUE(pXMLItemLabel, _AL("LineSpace"), m_nLineSpace, AUILABEL_LINESPACE)
	XMLSET_BOOL_VALUE(pXMLItemLabel, _AL("ForceDynamicRender"), m_bForceDynamicRender, false)
	XMLSET_STRING_VALUE(pXMLItemLabel, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemLabel);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemLabel);
	pXMLItemText->SetName(_AL("Text"));
	XMLSET_STRING_VALUE(pXMLItemText, _AL("String"), trans.ReverseTranslate(m_szText), _AL(""))
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

	return true;
}

bool AUILabel::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUILabel::Tick(void)
{
	return AUIObject::Tick();
}

bool AUILabel::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		bool bval = true;
		ACHAR szColor[20];
		POINT ptPos = GetPos();
		A3DCOLOR r = A3DCOLOR_GETRED(m_color);
		A3DCOLOR g = A3DCOLOR_GETGREEN(m_color);
		A3DCOLOR b = A3DCOLOR_GETBLUE(m_color);

		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

		FitFont();
		A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();

		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		A3DRECT *pRect = NULL, Rect;
		if( m_nWidth > 0 && m_nHeight > 0 )
		{
			Rect = GetRect();
			Rect.Offset(m_nOffX, m_nOffY);
			pRect = &Rect;
		}

		if( m_bMarquee )
		{
			// TODO: Vertical Marquee
			// if(m_bVerticalText){}
			if( m_nMarqueeTimes > 0 )
			{
				int nWidth, nHeight, nLines;

				if( m_pA2DSpriteEmotion && m_pA2DSpriteEmotion[m_nCurEmotionSet] )
				{
					A3DRECT rc = m_pA2DSpriteEmotion[m_nCurEmotionSet]->GetItem(0)->GetRect();
					AUI_GetTextRect(pFont, m_szText, nWidth, nHeight, nLines, m_nLineSpace * fWindowScale, &m_ItemsSet, rc.Width() * fWindowScale, rc.Height() * fWindowScale, 
						m_pvecImageList, 1.0f, m_pvecOtherFonts);
				}
				else
					AUI_GetTextRect(pFont, m_szText, nWidth, nHeight, nLines, m_nLineSpace * fWindowScale, &m_ItemsSet, 0, 0,
						m_pvecImageList, 1.0f, m_pvecOtherFonts);
				if( m_nMarqueePos < -nWidth )
				{
					m_nMarqueeTimes--;
					if( m_nMarqueeTimes > 0 )
						m_nMarqueePos = m_nWidth;
				}

				DWORD dwTick = GetTickCount();
				int nPixel = m_nMarqueeSpeed * (dwTick - m_dwLastTick) / 1000;

				if( nPixel > 0 )
				{
					m_nMarqueePos -= nPixel;
					m_dwLastTick = dwTick;
				}
				ptPos.x += m_nMarqueePos;

				a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);
				if( m_pA2DSpriteEmotion && m_vecEmotion )
				{
					bval = AUI_TextOutFormat(pFont, ptPos.x, ptPos.y, szColor + m_szText,
						pRect, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, m_pA2DSpriteEmotion[m_nCurEmotionSet], m_vecEmotion[m_nCurEmotionSet], NULL, 
						true, m_nShadow, nAlpha, false, &m_ItemsSet, m_pvecImageList, fWindowScale, false, m_pvecOtherFonts);
				}
				else
				{
					bval = AUI_TextOutFormat(pFont, ptPos.x, ptPos.y, szColor + m_szText,
						pRect, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, NULL, NULL, NULL, true, m_nShadow, nAlpha, false, &m_ItemsSet,
						m_pvecImageList, 1.0f, false, m_pvecOtherFonts);
				}
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUILabel::Render(), failed to call AUI_TextOutFormat()");
			}
		}
		else
		{
			if(m_bVerticalText)
			{
				//// TODO: Formatted Text. (now, in vertical mode, only render raw text)
				//int nLineHeight = pFont->GetFontHeight() + m_nLineSpace * fWindowScale;
				//for(int i = 0; i < m_szText.GetLength(); ++i)
				//{
				//	pFont->TextOutVert(ptPos.x * fWindowScale, ptPos.y * fWindowScale, m_szText, (nAlpha << 24) | m_color,
				//		m_nShadow > 0, A3DCOLORRGBA(0, 0, 0, m_nShadow * nAlpha / 255));
				//}
				// TODO: UnderLine(Vertical). Vertical BaseLine/Vertical Alignment(Horizontal). Line from right to left(Vertical). Some kinds of tags render(Vertical).
				int nWidth, nHeight, nLines;
				if( m_pA2DSpriteEmotion && m_pA2DSpriteEmotion[m_nCurEmotionSet] )
				{
					A3DRECT rc = m_pA2DSpriteEmotion[m_nCurEmotionSet]->GetItem(0)->GetRect();
					AUI_GetTextRect(pFont, m_szText, nWidth, nHeight, nLines, m_nLineSpace * fWindowScale, &m_ItemsSet, rc.Width() * fWindowScale, rc.Height() * fWindowScale,
						m_pvecImageList, 1.0f, m_pvecOtherFonts);
				}
				else
				{
					AUI_GetTextRect(pFont, m_szText, nWidth, nHeight, nLines, m_nLineSpace * fWindowScale, &m_ItemsSet, 0, 0,
						m_pvecImageList, 1.0f, m_pvecOtherFonts);
				}
				a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);
				if( m_pA2DSpriteEmotion && m_vecEmotion )
				{
					bval = AUI_TextOutFormat(pFont, ptPos.x, ptPos.y, szColor + m_szText,
						pRect, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, m_pA2DSpriteEmotion[m_nCurEmotionSet], m_vecEmotion[m_nCurEmotionSet], NULL, 
						true, m_nShadow, nAlpha, false, &m_ItemsSet, m_pvecImageList, fWindowScale, true, m_pvecOtherFonts, 0, 0, true);
				}
				else
				{
					bval = AUI_TextOutFormat(pFont, ptPos.x, ptPos.y, szColor + m_szText,
						pRect, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, NULL, NULL, NULL, true, m_nShadow, nAlpha, false,
						&m_ItemsSet, m_pvecImageList, 1.0f, true, m_pvecOtherFonts, 0, 0, true);
				}
			}
			else
			{
				if( m_nAlign == AUIFRAME_ALIGN_RIGHT ||
					m_nAlign == AUIFRAME_ALIGN_CENTER )
				{
					int nWidth, nHeight, nLines;

					if( m_pA2DSpriteEmotion && m_pA2DSpriteEmotion[m_nCurEmotionSet] )
					{
						A3DRECT rc = m_pA2DSpriteEmotion[m_nCurEmotionSet]->GetItem(0)->GetRect();
						AUI_GetTextRect(pFont, m_szText, nWidth, nHeight, nLines, m_nLineSpace * fWindowScale, &m_ItemsSet, rc.Width() * fWindowScale, rc.Height() * fWindowScale,
							m_pvecImageList, 1.0f, m_pvecOtherFonts);
					}
					else
						AUI_GetTextRect(pFont, m_szText, nWidth, nHeight, nLines, m_nLineSpace * fWindowScale, &m_ItemsSet, 0, 0,
							m_pvecImageList, 1.0f, m_pvecOtherFonts);
					if( m_nAlign == AUIFRAME_ALIGN_CENTER )
						ptPos.x = ptPos.x + m_nWidth / 2 - nWidth / 2;
					else
						ptPos.x = ptPos.x + m_nWidth - nWidth;
				}

				a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);

				if( m_pA2DSpriteEmotion && m_vecEmotion )
				{
					bval = AUI_TextOutFormat(pFont, ptPos.x, ptPos.y, szColor + m_szText,
						pRect, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, m_pA2DSpriteEmotion[m_nCurEmotionSet], m_vecEmotion[m_nCurEmotionSet], NULL, 
						true, m_nShadow, nAlpha, false, &m_ItemsSet, m_pvecImageList, fWindowScale, true, m_pvecOtherFonts);
				}
				else
				{
					bval = AUI_TextOutFormat(pFont, ptPos.x, ptPos.y, szColor + m_szText,
						pRect, 0, NULL, m_nLineSpace * fWindowScale, false, NULL, NULL, NULL, NULL, true, m_nShadow, nAlpha, false,
						&m_ItemsSet, m_pvecImageList, 1.0f, true, m_pvecOtherFonts);
				}
			}
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUILabel::Render(), failed to call AUI_TextOutFormat()");
		}
	}

	return true;
}

bool AUILabel::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Align") )
		Property->i = m_nAlign;
	else if( 0 == strcmpi(pszPropertyName, "Vertical Text") )
		Property->b = m_bVerticalText;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUILabel::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Align") )
		m_nAlign = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Vertical Text") )
		m_bVerticalText = Property->b;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUILabel::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Text Content", AUIOBJECT_PROPERTY_VALUE_ACHAR);
}

bool AUILabel::IsMarquee()
{
	return m_nMarqueeTimes > 0 ? true : false;
}

void AUILabel::SetMarquee(bool bMarquee, int nTimes, int nSpeed)
{
	SetDynamicRender(bMarquee);

	m_bMarquee = bMarquee;
	m_nMarqueePos = m_nWidth;
	m_nMarqueeTimes = nTimes;
	m_nMarqueeSpeed = nSpeed;
	m_dwLastTick = GetTickCount();
}

void AUILabel::SetText(const ACHAR *pszText)
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
}

void AUILabel::SetCurrentEmotion(int nEmotion)
{
	if( nEmotion >= 0 && nEmotion < AUIMANAGER_MAX_EMOTIONGROUPS )
		m_nCurEmotionSet = nEmotion;
}

void AUILabel::SetEmotionList(A2DSprite **pA2DSpriteEmotion,
	abase::vector<AUITEXTAREA_EMOTION> **pvecEmotion)
{
	m_pA2DSpriteEmotion = pA2DSpriteEmotion;
	m_vecEmotion = pvecEmotion;
}

int AUILabel::GetRequiredHeight()
{
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();

	int nHeight;
	A3DRECT rcText(0, 0, m_nWidth, 1024);
	AUI_TextOutFormat(pFont, 0, 0, m_szText,
		&rcText, 0, NULL, m_nLineSpace * fWindowScale, true, &nHeight, NULL, NULL, NULL, false, m_nShadow, 255, false, &m_ItemsSet, m_pvecImageList, 1.0f, true, m_pvecOtherFonts);

	return nHeight + 1;
}