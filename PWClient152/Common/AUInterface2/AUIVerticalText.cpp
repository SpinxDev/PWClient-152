// Filename	: AUIVerticalText.cpp
// Creator	: Xiao Zhou
// Date		: 2009/11/30
// Desc		: AUIVerticalText is the class of vertical text control.

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

#include "AUIDef.h"
#include "AUICommon.h"
#include "AUIFrame.h"
#include "AUIManager.h"
#include "AUIDialog.h"
#include "AUIVerticalText.h"
#include "AUICTranslate.h"

#define AUIVERTICALTEXT_LINESPACE		3
#define AUIVERTICALTEXT_SCROLL_DELAY	50

AUIVerticalText::AUIVerticalText() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_VERTICALTEXT;
	m_nAlign = AUIFRAME_ALIGN_RIGHT;
	m_nItemMask = 0xffffffff;
	m_nLineSpace = AUIVERTICALTEXT_LINESPACE;
	m_bUpScrollBar = false;
	m_bLeftToRight = false;
	m_nTotalLines = m_nFirstLine = m_nLinesPP = 0;
	m_nScrollDelay = GetTickCount();
	m_bDragging = false;
	m_ptBar.x = m_ptBar.y = m_nDragDist = 0;

	m_nFrameMode = 0;
	m_pAUIFrame = NULL;
	for( int i = 0; i < AUIVERTICALTEXT_RECT_NUM; i++ )
		m_pA2DSprite[i] = NULL;
}

AUIVerticalText::~AUIVerticalText()
{
}

bool AUIVerticalText::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
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
	XMLGET_BOOL_VALUE(pItem, _AL("UpScrollBar"), m_bUpScrollBar)
	XMLGET_BOOL_VALUE(pItem, _AL("LeftToRight"), m_bLeftToRight)
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
					InitIndirect(strFileName, AUIVERTICALTEXT_RECT_FRAME);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("LeftImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIVERTICALTEXT_RECT_LEFT);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("RightImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIVERTICALTEXT_RECT_RIGHT);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIVERTICALTEXT_RECT_SCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("BarImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUIVERTICALTEXT_RECT_BAR);
				}
				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Text")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pChildItem, _AL("String"), m_szText)
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
		return AUI_ReportError(__LINE__, 1, "AUIVerticalText::InitXML(), failed to call SetFontAndColor()");

	return true;
}

bool AUIVerticalText::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUIVERTICALTEXT_RECT_MIN && idType < AUIVERTICALTEXT_RECT_MAX);

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
			return AUI_ReportError(__LINE__, 1, "AUIVerticalText::InitIndirect(), failed to call m_pAUIFrame->Init()");
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
			return AUI_ReportError(__LINE__, 1, "AUIVerticalText::InitIndirect(), failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUIVERTICALTEXT_RECT_LEFT == idType || AUIVERTICALTEXT_RECT_RIGHT == idType )
		{
			A3DRECT rc[2];
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight();
			rc[0].SetRect(0, 0, W, H / 2);
			rc[1].SetRect(0, H / 2, W, H);
			bval = m_pA2DSprite[idType]->ResetItems(2, rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::InitIndirect(), failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
	}

	CalcTextLines();

	return true;
}

bool AUIVerticalText::Release(void)
{
	A3DRELEASE(m_pAUIFrame);
	for( int i = 0; i < AUIVERTICALTEXT_RECT_NUM; i++ )
		A3DRELEASE(m_pA2DSprite[i]);

	return AUIObject::Release();
}

bool AUIVerticalText::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemVertialText = new AXMLItem;
	pXMLItemVertialText->InsertItem(pXMLItemDialog);
	pXMLItemVertialText->SetName(_AL("VERTICALTEXT"));
	XMLSET_STRING_VALUE(pXMLItemVertialText, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemVertialText, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemVertialText, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemVertialText, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemVertialText, _AL("Height"), m_nHeight, 0)
	XMLSET_INT_VALUE(pXMLItemVertialText, _AL("Align"), m_nAlign, 0)
	XMLSET_INT_VALUE(pXMLItemVertialText, _AL("LineSpace"), m_nLineSpace, AUIVERTICALTEXT_LINESPACE)
	XMLSET_BOOL_VALUE(pXMLItemVertialText, _AL("UpScrollBar"), m_bUpScrollBar, false)
	XMLSET_BOOL_VALUE(pXMLItemVertialText, _AL("LeftToRight"), m_bLeftToRight, false)
	XMLSET_STRING_VALUE(pXMLItemVertialText, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemVertialText);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemVertialText);
	pXMLItemText->SetName(_AL("Text"));
	XMLSET_STRING_VALUE(pXMLItemText, _AL("String"), m_szText, _AL(""))
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
	pXMLItemResource->InsertItem(pXMLItemVertialText);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pAUIFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	if( m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("LeftImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("RightImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("ScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR] )
	{
		AXMLItem *pXMLItemBarImage = new AXMLItem;
		pXMLItemBarImage->InsertItem(pXMLItemResource);
		pXMLItemBarImage->SetName(_AL("BarImage"));
		XMLSET_STRING_VALUE(pXMLItemBarImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->GetName()), _AL(""))
	}

	return true;
}

bool AUIVerticalText::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( !m_pAUIFrame ||
		!m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT] ||
		!m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT] ||
		!m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL] )
	{
		return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
	}

	int nLines = m_nTotalLines;
	int nLinesPP = m_nLinesPP;
	SIZE sLimit = {0, 0};
	if( m_pAUIFrame )
		sLimit = m_pAUIFrame->GetSizeLimit();
	int cx = sLimit.cx / 2;
	int cy = sLimit.cy / 2;
	A3DRECT rcLeft = m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->GetItem(0)->GetRect();
	A3DRECT rcRight = m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->GetItem(0)->GetRect();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	rcLeft.SetRect(rcLeft.left * fWindowScale, rcLeft.top * fWindowScale,
		rcLeft.right * fWindowScale, rcLeft.bottom * fWindowScale);
	rcRight.SetRect(rcRight.left * fWindowScale, rcRight.top * fWindowScale,
		rcRight.right * fWindowScale, rcRight.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	A3DRECT rcText(cx, cy, m_nWidth	- cx, m_nHeight - cy - rcScroll.Height());
	A3DRECT rcScrollArea(rcLeft.Width(), m_nHeight - rcScroll.Height() , m_nWidth - rcRight.Width(), m_nHeight);

	if( m_bUpScrollBar )
	{
		rcText.Offset(0, rcScroll.Height());
		rcScrollArea.Offset(0, rcScroll.Height() - m_nHeight);
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
			int nBarW = m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->GetItem(0)->GetRect().Width() * fWindowScale;
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUITEXTAREA_STATE_PRESSBAR == m_nState && nLines > nLinesPP )
					{
						int L = nLines - nLinesPP;
						int W = rcScrollArea.Width() - nBarW;

						if( W > 0 )
						{
							if( m_bLeftToRight )
								m_nFirstLine = (x - rcScrollArea.left - m_nDragDist) * (L + 1) / W;
							else
								m_nFirstLine = (rcScrollArea.right + m_nDragDist - x) * (L + 1) / W;
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
				// Page left.
				if( x < m_ptBar.x )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
						return OnDlgItemMessage(WM_KEYDOWN, m_bLeftToRight ? VK_PRIOR : VK_NEXT, 0);
				}
				// Page right.
				else if( x > m_ptBar.x + nBarW )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
						return OnDlgItemMessage(WM_KEYDOWN, m_bLeftToRight ? VK_NEXT : VK_PRIOR, 0);
				}
				// Scroll bar.
				else if( nLines > nLinesPP )
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDragging = true;
						m_nDragDist = x - m_ptBar.x;
						m_pParent->SetCaptureObject(this);
						m_nState = AUITEXTAREA_STATE_PRESSBAR;
					}
				}
			}
		}
		else if( y >= rcScrollArea.top && y <= rcScrollArea.bottom )
		{
			// Click on left button.
			if( x < rcScrollArea.left )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUIVERTICALTEXT_STATE_PRESSLEFT;
					m_nScrollDelay = 0;
				}
			}
			// Click on right button.
			else if( x > rcScrollArea.right )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUIVERTICALTEXT_STATE_PRESSRIGHT;
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
			if( m_nFirstLine > 0 )
				m_nFirstLine--;
			return true;
		}
		else if( VK_DOWN == nVKey )
		{
			if( m_nFirstLine < nLines - nLinesPP )
				m_nFirstLine++;
			return true;
		}
		else if( VK_PRIOR == nVKey )
		{
			m_nFirstLine -= nLinesPP;
			if( m_nFirstLine < 0 )
				m_nFirstLine = 0;
			return true;
		}
		else if( VK_NEXT == nVKey )
		{
			if( nLines - m_nFirstLine >= 2 * nLinesPP )
				m_nFirstLine += nLinesPP;
			else if( nLines - m_nFirstLine >= nLinesPP )
				m_nFirstLine = nLines - nLinesPP;
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
			m_nFirstLine--;
		else if( zDelta < 0 && m_nFirstLine + nLinesPP < nLines )
			m_nFirstLine++;

		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIVerticalText::Tick(void)
{
	if( !m_pAUIFrame || !m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL] )
		return true;

	int nLines = m_nTotalLines;
	int nLinesPP = m_nLinesPP;
	DWORD dwTick = GetTickCount();
	if( dwTick - m_nScrollDelay >= AUIVERTICALTEXT_SCROLL_DELAY )
	{
		if( AUIVERTICALTEXT_STATE_PRESSLEFT == m_nState && m_bLeftToRight ||
			AUIVERTICALTEXT_STATE_PRESSRIGHT == m_nState && !m_bLeftToRight)
		{
			if( m_nFirstLine > 0 )
				m_nFirstLine--;
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
		else if( AUIVERTICALTEXT_STATE_PRESSRIGHT == m_nState && m_bLeftToRight ||
			AUIVERTICALTEXT_STATE_PRESSLEFT == m_nState && !m_bLeftToRight)
		{
			if( m_nFirstLine + nLinesPP < nLines )
				m_nFirstLine++;
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
	}

	return AUIObject::Tick();
}

bool AUIVerticalText::SetFontAndColor(const ACHAR *pszFontName, int nFontHeight,
	A3DCOLOR dwColor, int nShadow, int nOutline, bool bBold, bool bItalic, bool bFreeType,
	A3DCOLOR clOutline, A3DCOLOR clInnerText, bool bInterpColor, A3DCOLOR clUpper, A3DCOLOR clLower)
{
	bool bval=AUIObject::SetFontAndColor(pszFontName, nFontHeight, dwColor, nShadow, nOutline, 
							bBold, bItalic, bFreeType, clOutline, clInnerText, bInterpColor, clUpper, clLower);
	if(bval && GetFont() )
	{
		m_nFontWidth = GetFont()->GetFontWidth();
	}

	CalcTextLines();

	return bval;
}


bool AUIVerticalText::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

		bool bval = true;
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

		A3DFTFont* pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
						
		A3DRECT rcText(0, 0, 0, 0);
		A3DRECT rcScroll(0, 0, 0, 0);
		if( m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL] )
		{
			rcScroll = m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->GetItem(0)->GetRect();
			rcText.SetRect(ptPos.x + cx, ptPos.y + cy, ptPos.x + m_nWidth
				- cx, ptPos.y + m_nHeight - cy - rcScroll.Height());
			if( m_bUpScrollBar ) rcText.Offset(0, rcScroll.Height());
		}
		else
			rcText.SetRect(ptPos.x + cx, ptPos.y + cy, ptPos.x + m_nWidth - cx, ptPos.y + m_nHeight - cy);

		if( m_pAUIFrame )
		{
			int y = ptPos.y;
			if( m_bUpScrollBar )
				y = ptPos.y + rcScroll.Height();

			bval = m_pAUIFrame->Render(ptPos.x, y, m_nWidth, m_nHeight - rcScroll.Height(),
				NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
		}
		else
			bval = AUIObject::Render();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::Render(), failed to call m_pAUIFrame->Render() or base AUIObject::Render()");

		m_EditBoxItems.clear();
		int nFirst = m_nFirstLine;
		int nEnd = m_nFirstLine + m_nLinesPP;
		if( nEnd > m_nTotalLines )
			nEnd = m_nTotalLines;

		int nLineWidth = m_nFontWidth + m_nLineSpace * fWindowScale;
		for(int n = nFirst; n < nEnd; n++)
		{
			int x;
			if( m_bLeftToRight )
				x = rcText.left + (n - nFirst) * nLineWidth;
			else
				x = rcText.right - (n - nFirst) * nLineWidth - m_nFontWidth;
			int y = rcText.top;
			int i = m_vecLines[n];
			while(i < m_vecLines[n + 1])
			{
				if( IsEditboxItemCode(m_szText[i]) )
				{
					EditBoxItemBase *pItem = m_ItemsSet.GetItemByChar(m_szText[i]);
					if( pItem && pItem->GetType() == enumEICoord )
					{
						int nLen = a_strlen(pItem->GetName());
						AUITEXTAREA_EDITBOX_ITEM item;
						item.m_pItem = pItem;
						item.rc.left = x;
						item.rc.right = x + m_nFontWidth;
						item.rc.top = y;
						for(int j = 0; j < nLen; j++ )
						{
							pFont->TextOut(x, y, pItem->GetName() + j, (nAlpha << 24) | (0xFFFFFF & pItem->GetColor()),
								m_nShadow & 0xFF000000, (nAlpha << 24) | (0xFFFFFF & m_nShadow), 0, 1);
							y += pFont->GetTextExtent(pItem->GetName() + j, 1).y;
						}
						item.rc.bottom = y;
						m_EditBoxItems.push_back(item);
					}
				}
				else if( m_szText[i] != '\r' && m_szText[i] != '\n' )
				{
					pFont->TextOut(x, y, &m_szText[i], (nAlpha << 24) | (0xFFFFFF & m_color),
						m_nShadow & 0xFF000000, (nAlpha << 24) | (0xFFFFFF & m_nShadow), 0, 1);
					y += pFont->GetTextExtent(&m_szText[i], 1).y;
				}
				i++;
			}
		}

		bval = RenderScroll();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::Render(), failed to call RenderScroll()");
	}

	return true;
}

bool AUIVerticalText::RenderScroll()
{
	if( !m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT] ||
		!m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT] ||
		!m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL] )
	{
		return true;
	}

	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	bool bval = true;
	POINT ptPos = GetPos();
	A3DRECT rcLeft = m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->GetItem(0)->GetRect();
	A3DRECT rcRight = m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->GetItem(0)->GetRect();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	rcLeft.SetRect(rcLeft.left * fWindowScale, rcLeft.top * fWindowScale,
		rcLeft.right * fWindowScale, rcLeft.bottom * fWindowScale);
	rcRight.SetRect(rcRight.left * fWindowScale, rcRight.top * fWindowScale,
		rcRight.right * fWindowScale, rcRight.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	// Draw up button.
	int nItem = (AUIVERTICALTEXT_STATE_PRESSLEFT == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::RenderScroll(), failed to call m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->SetCurrentItem()");

	int H = rcScroll.Height();
	int x, y;
	x = ptPos.x;
	if( m_bUpScrollBar )
		y = ptPos.y;
	else
		y = ptPos.y + m_nHeight - H;
	m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::RenderScroll(), failed to call m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->DrawToBack() or m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->DrawToInternalBuffer()");

	// Draw scroll area.
	int W = rcLeft.Width();
	x += W;

	float fScale = float(m_nWidth - rcLeft.Width() - rcRight.Width())
		/ m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->GetItem(0)->GetRect().Width();
	m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->SetScaleX(fScale);
	m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::RenderScroll(), failed to call m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->DrawToBack() or m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->DrawToInternalBuffer()");

	// Draw down button.
	nItem = (AUIVERTICALTEXT_STATE_PRESSRIGHT == m_nState) ? 1 : 0;
	bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->SetCurrentItem(nItem);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::RenderScroll(), failed to call m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->SetCurrentItem()");

	x = ptPos.x + m_nWidth - rcRight.Width();

	m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->SetScaleX(fWindowScale);
	m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->SetScaleY(fWindowScale);
	m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->SetAlpha(nAlpha);
	if( m_pParent->IsNoFlush() )
		bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->DrawToBack(x, y);
	else
		bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->DrawToInternalBuffer(x, y);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::RenderScroll(), failed to call m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->DrawToBack() or m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->DrawToInternalBuffer()");

	// Draw scrollbar button if necessary.
	if( m_nTotalLines > m_nLinesPP  )
	{
		int nBarW = m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->GetItem(0)->GetRect().Width() * fWindowScale;
		int W = m_nWidth - rcLeft.Width() - rcRight.Width() - nBarW;
		if( W > 0 )
		{
			if( m_bUpScrollBar )
				m_ptBar.y = 0;
			else
				m_ptBar.y = m_nHeight - rcScroll.Height();
			if( m_bLeftToRight )
				m_ptBar.x = rcLeft.Width() + m_nFirstLine * W / (m_nTotalLines - m_nLinesPP);
			else
				m_ptBar.x = m_nWidth - rcRight.Width() - m_nFirstLine * W / (m_nTotalLines - m_nLinesPP) - nBarW;
			m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->SetAlpha(nAlpha);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->DrawToBack(ptPos.x + m_ptBar.x, ptPos.y + m_ptBar.y);
			else
				bval = m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->DrawToInternalBuffer(ptPos.x + m_ptBar.x, ptPos.y + m_ptBar.y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIVerticalText::RenderScroll(), failed to call m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->DrawToBack() or m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->DrawToInternalBuffer()");
		}
	}

	return true;
}

bool AUIVerticalText::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Align") )
		Property->i = m_nAlign;
	else if( 0 == strcmpi(pszPropertyName, "Up Scroll Bar") )
		Property->b = m_bUpScrollBar;
	else if( 0 == strcmpi(pszPropertyName, "Left To Right") )
		Property->b = m_bLeftToRight;
	else if( 0 == strcmpi(pszPropertyName, "Text Content") )
		a_strncpy(Property->s, m_szText, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
	{
		if( m_pAUIFrame )
			strncpy(Property->fn, m_pAUIFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Left Button Image") )
	{
		if( m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT] )
			strncpy(Property->fn, m_pA2DSprite[AUIVERTICALTEXT_RECT_LEFT]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Right Button Image") )
	{
		if( m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT] )
			strncpy(Property->fn, m_pA2DSprite[AUIVERTICALTEXT_RECT_RIGHT]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
	{
		if( m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
	{
		if( m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR] )
			strncpy(Property->fn, m_pA2DSprite[AUIVERTICALTEXT_RECT_BAR]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIVerticalText::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Align") )
		m_nAlign = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Up Scroll Bar") )
		m_bUpScrollBar = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Left To Right") )
		m_bLeftToRight = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Text Content") )
		SetText(Property->s);
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image") )
		return InitIndirect(Property->fn, AUIVERTICALTEXT_RECT_FRAME);
	else if( 0 == strcmpi(pszPropertyName, "Left Button Image") )
		return InitIndirect(Property->fn, AUIVERTICALTEXT_RECT_LEFT);
	else if( 0 == strcmpi(pszPropertyName, "Right Button Image") )
		return InitIndirect(Property->fn, AUIVERTICALTEXT_RECT_RIGHT);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
		return InitIndirect(Property->fn, AUIVERTICALTEXT_RECT_SCROLL);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
		return InitIndirect(Property->fn, AUIVERTICALTEXT_RECT_BAR);
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIVerticalText::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Text Content", AUIOBJECT_PROPERTY_VALUE_ACHAR);
}

void AUIVerticalText::SetText(const ACHAR *pszText)
{
	ACString strText = UnmarshalEditBoxText(pszText, m_ItemsSet, 0, L"", 0xffffffff, m_nItemMask);
	AUIObject::SetText(strText);

	SetDynamicRender(false);
	CalcTextLines();
}

void AUIVerticalText::SetSize(int W, int H)
{
	AUIObject::SetSize(W, H);

	CalcTextLines();
}

void AUIVerticalText::SetDefaultSize(int W, int H)
{
	AUIObject::SetDefaultSize(W, H);
	CalcTextLines();
}

void AUIVerticalText::SetLineSpace(int nSpace)
{
	AUIObject::SetLineSpace(nSpace);

	CalcTextLines();
}

void AUIVerticalText::CalcTextLines()
{
	int cx = 0, cy = 0;
	if( m_pAUIFrame )
	{
		SIZE sLimit = m_pAUIFrame->GetSizeLimit();
		cx = sLimit.cx / 2;
		cy = sLimit.cy / 2;
	}
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	A3DFTFont* pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	if( !m_pFont )
		m_nFontWidth = m_pParent->GetAUIManager()->GetDefaultFont()->GetFontWidth();

	int nMaxHeight, nMaxWidth;
	A3DRECT rcText, rcScroll(0, 0, 0, 0);
	nMaxWidth = m_nWidth - cx * 2;
	int nLineWidth = m_nFontWidth + m_nLineSpace * fWindowScale;
	m_nLinesPP = (nMaxWidth + m_nLineSpace * fWindowScale) / nLineWidth;
	if( m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL] )
	{
		rcScroll = m_pA2DSprite[AUIVERTICALTEXT_RECT_SCROLL]->GetItem(0)->GetRect();
		rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale, rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
		rcScroll.Offset(GetPos().x, 0);
		nMaxHeight = m_nHeight - cy * 2 - rcScroll.Height();
		if( !m_bUpScrollBar )
			rcScroll.Offset(m_nHeight - rcScroll.Height(), 0);
	}
	else
		nMaxHeight = m_nHeight - cy * 2;

	m_vecLines.clear();
	m_vecLines.push_back(0);
	m_nTotalLines = 0;
	m_nFirstLine = 0;
	int nLineHeight = 0;
	int nHeight = 0;
	for(int i = 0; i < m_szText.GetLength(); i++)
		if( IsEditboxItemCode(m_szText[i]) )
		{
			EditBoxItemBase *pItem = m_ItemsSet.GetItemByChar(m_szText[i]);
			if( pItem && pItem->GetType() == enumEICoord )
			{
				int nLen = a_strlen(pItem->GetName());
				nHeight = 0;
				for(int j = 0; j < nLen; j++ )
					nHeight += pFont->GetTextExtent(pItem->GetName() + j, 1).y;
				if( nHeight + nLineHeight > nMaxHeight )
				{
					nLineHeight = nHeight;
					m_nTotalLines++;
					m_vecLines.push_back(i);
				}
				else
					nLineHeight += nHeight;
			}
		}
		else if( m_szText[i] == '\n' )
			continue;
		else if( m_szText[i] == '\r' )
		{
			nLineHeight = 0;
			m_nTotalLines++;
			m_vecLines.push_back(i + 1);
		}
		else
		{
			nHeight = pFont->GetTextExtent(&m_szText[i], 1).y;
			if( nHeight + nLineHeight > nMaxHeight )
			{
				nLineHeight = nHeight;
				m_nTotalLines++;
				m_vecLines.push_back(i);
			}
			else
				nLineHeight += nHeight;
		}
	m_vecLines.push_back(m_szText.GetLength());
	if( nHeight > 0 )
		m_nTotalLines++;
}

bool AUIVerticalText::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());

	m_nFontWidth = GetFont()->GetFontWidth();
	CalcTextLines();

	return true;
}

void AUIVerticalText::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIVerticalText::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	for (int i = 0; i < AUIVERTICALTEXT_RECT_NUM; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);
}
