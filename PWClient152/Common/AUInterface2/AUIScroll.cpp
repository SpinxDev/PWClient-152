// Filename	: AUIScroll.cpp
// Creator	: Xiao Zhou
// Date		: May 24, 2005
// Desc		: AUIScroll is the class of scroll control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "AUIDialog.h"
#include "AUIManager.h"
#include "AUIScroll.h"
#include "AUICommon.h"
#include "AUICTranslate.h"

#define AUISCROLL_DELAY	100

AUIScroll::AUIScroll() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_SCROLL;
	m_nFrameMode = 0;
	m_pBarFrame = NULL;
	for( int i = 0; i < AUISCROLL_RECT_NUM; i++ )
		m_pA2DSprite[i] = NULL;

	m_nDragDist = 0;
	m_bDragging = false;
	m_bZoomScroll = false;
	m_nBarLevel = AUISCROLL_MIN;
	m_nScrollBegin = AUISCROLL_MIN;
	m_nScrollEnd = AUISCROLL_MAX;
	m_ptBar.x = m_ptBar.y = m_nDragDist = 0;
	m_nBarLength = -1;
	m_nBarHeight = 0;
	m_bVScroll = true;
	m_nScrollDelay = 0;
	m_nScrollStep = 1;
	m_clrBar = A3DCOLORRGB(255,255,255);
}

AUIScroll::~AUIScroll()
{
}

bool AUIScroll::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	Scroll property line format:
	 *
	 *		Scroll					Keyword
	 *		Page					Name
	 *		""						Command.
	 *		10 10 19 200			x y Width Height
	 *		"ListUp.bmp"			ImageFileName
	 *		"ListDown.bmp"			ImageFileName
	 *		"ListScroll.bmp"		ImageFileName
	 *		"ListBar.bmp"			ImageFileName
	 */

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIScroll::Init(), failed to load base AUIObject::Init()");

	if( pASF )
	{
		for( int i = AUISCROLL_RECT_MIN; i < AUISCROLL_RECT_MAX; i++ )
		{
			if (!pASF->GetNextToken(true))
				return AUI_ReportError(__LINE__, 1, "AUIScroll::Init(), failed to read from file");

			if (!InitIndirect(pASF->m_szToken, i))
				return AUI_ReportError(__LINE__, 1, "AUIScroll::Init(), failed to call InitIndirect()");
		}
		if( m_pParent->GetFileVersion() > 0 )
		{
			if( !pASF->GetNextToken(true) )
				return AUI_ReportError(__LINE__, 1, "AUIScroll::Init(), failed to call InitIndirect()");
			AUIOBJECT_SETPROPERTY p;
			p.i = atoi(pASF->m_szToken);
			SetProperty("Frame Mode", &p);
		}

	}

	return true;
}

bool AUIScroll::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
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
				if( a_stricmp(pResouceChildItem->GetName(), _AL("UpImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISCROLL_RECT_UP);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("DownImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISCROLL_RECT_DOWN);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("ScrollImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISCROLL_RECT_SCROLL);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("BarFrameImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, AUISCROLL_RECT_BAR);
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

	return true;
}

bool AUIScroll::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	ASSERT(idType >= AUISCROLL_RECT_MIN && idType < AUISCROLL_RECT_MAX);

	if( strlen(pszFileName) <= 0 ) return true;

	UpdateRenderTarget();
	if( AUISCROLL_RECT_BAR == idType )
	{
		A3DRELEASE(m_pBarFrame);

		m_pBarFrame = new AUIFrame;
		if( !m_pBarFrame ) return false;

		bval = m_pBarFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
		if( !bval )
		{
			A3DRELEASE(m_pBarFrame)
			return AUI_ReportError(__LINE__, 1, "AUIScroll::InitIndirect(), failed to call m_pBarFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			m_pBarFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else
	{
		A3DRELEASE(m_pA2DSprite[idType]);
	
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
			return AUI_ReportError(__LINE__, 1, "AUIScroll::InitIndirect(), failed to call m_pA2DSprite[%d]->Init()", idType);
		}

		if( AUISCROLL_RECT_UP == idType || AUISCROLL_RECT_DOWN == idType )
		{
			A3DRECT rc[2];
			int W = m_pA2DSprite[idType]->GetWidth();
			int H = m_pA2DSprite[idType]->GetHeight();
			rc[0].SetRect(0, 0, W, H / 2);
			rc[1].SetRect(0, H / 2, W, H);
			bval = m_pA2DSprite[idType]->ResetItems(2, rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::InitIndirect(), failed to call m_pA2DSprite[%d]->ResetItems()", idType);
		}
	}
	if( m_pBarFrame && m_pBarFrame->GetA2DSprite() && m_pA2DSprite[AUISCROLL_RECT_SCROLL] )
	{
		if( m_pA2DSprite[AUISCROLL_RECT_SCROLL]->GetWidth() == m_pBarFrame->GetA2DSprite()->GetWidth() )
		{
			m_nBarHeight = m_pBarFrame->GetA2DSprite()->GetHeight();
			m_bVScroll = true;
		}
		else
		{
			m_nBarHeight = m_pBarFrame->GetA2DSprite()->GetWidth();
			m_bVScroll = false;
		}
	}

	return true;
}

bool AUIScroll::Release(void)
{
	A3DRELEASE(m_pBarFrame);
	for( int i = 0; i < AUISCROLL_RECT_NUM; i++ )
		A3DRELEASE(m_pA2DSprite[i]);

	return AUIObject::Release();
}

bool AUIScroll::Save(FILE *file)
{
	fprintf(file, "SCROLL");

	if( !AUIObject::Save(file) ) return false;

	for( int i = 0; i < AUISCROLL_RECT_NUM; i++ )
	{
		if( AUISCROLL_RECT_BAR == i )
		{
			if( m_pBarFrame )
				m_pBarFrame->Save(file);
			else
				fprintf(file, " \"\"");
		}
		else
			fprintf(file, " \"%s\"", m_pA2DSprite[i] ? m_pA2DSprite[i]->GetName() : "");
	}

	fprintf(file, " %d \n", m_nFrameMode);

	return true;
}

bool AUIScroll::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemScroll = new AXMLItem;
	pXMLItemScroll->InsertItem(pXMLItemDialog);
	pXMLItemScroll->SetName(_AL("SCROLL"));
	XMLSET_STRING_VALUE(pXMLItemScroll, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemScroll, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemScroll, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemScroll, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemScroll, _AL("Height"), m_nHeight, 0)
	XMLSET_STRING_VALUE(pXMLItemScroll, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemScroll);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pA2DSprite[AUISCROLL_RECT_UP] )
	{
		AXMLItem *pXMLItemUpImage = new AXMLItem;
		pXMLItemUpImage->InsertItem(pXMLItemResource);
		pXMLItemUpImage->SetName(_AL("UpImage"));
		XMLSET_STRING_VALUE(pXMLItemUpImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISCROLL_RECT_UP]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUISCROLL_RECT_DOWN] )
	{
		AXMLItem *pXMLItemDownImage = new AXMLItem;
		pXMLItemDownImage->InsertItem(pXMLItemResource);
		pXMLItemDownImage->SetName(_AL("DownImage"));
		XMLSET_STRING_VALUE(pXMLItemDownImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISCROLL_RECT_DOWN]->GetName()), _AL(""))
	}
	if( m_pA2DSprite[AUISCROLL_RECT_SCROLL] )
	{
		AXMLItem *pXMLItemScrollImage = new AXMLItem;
		pXMLItemScrollImage->InsertItem(pXMLItemResource);
		pXMLItemScrollImage->SetName(_AL("ScrollImage"));
		XMLSET_STRING_VALUE(pXMLItemScrollImage, _AL("FileName"), AS2AC(m_pA2DSprite[AUISCROLL_RECT_SCROLL]->GetName()), _AL(""))
	}
	if( m_pBarFrame )
	{
		AXMLItem *pXMLItemBarFrameImage = new AXMLItem;
		pXMLItemBarFrameImage->InsertItem(pXMLItemResource);
		pXMLItemBarFrameImage->SetName(_AL("BarFrameImage"));
		XMLSET_STRING_VALUE(pXMLItemBarFrameImage, _AL("FileName"), AS2AC(m_pBarFrame->GetA2DSprite()->GetName()), _AL(""))
	}

	return true;
}

bool AUIScroll::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( !m_pA2DSprite[AUISCROLL_RECT_UP] ||
		!m_pA2DSprite[AUISCROLL_RECT_DOWN] ||
		!m_pA2DSprite[AUISCROLL_RECT_SCROLL] ||
		!m_pBarFrame)
	{
		return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
	}

	A3DRECT rcUp = m_pA2DSprite[AUISCROLL_RECT_UP]->GetItem(0)->GetRect();
	A3DRECT rcDown = m_pA2DSprite[AUISCROLL_RECT_DOWN]->GetItem(0)->GetRect();
	A3DRECT rcScroll = m_pA2DSprite[AUISCROLL_RECT_SCROLL]->GetItem(0)->GetRect();
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
		rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
	rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
		rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
	rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
		rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);
	A3DRECT rcScrollArea;
	if( m_bVScroll )
		rcScrollArea.SetRect(0, rcUp.Height(), m_nWidth, m_nHeight - rcDown.Height());
	else
		rcScrollArea.SetRect(rcUp.Width(), 0, m_nWidth - rcDown.Width(), m_nHeight);

	if( msg == WM_LBUTTONDOWN ||
		msg == WM_RBUTTONDOWN ||
		msg == WM_LBUTTONDBLCLK ||
		msg == WM_MOUSEMOVE ||
		msg == WM_LBUTTONUP )
	{
		POINT pp = GetPos();
		int x = GET_X_LPARAM(lParam) - m_nOffX - pp.x;
		int y = GET_Y_LPARAM(lParam) - m_nOffY - pp.y;

		if( WM_LBUTTONUP == msg )
		{
			m_bDragging = false;
			m_pParent->SetCaptureObject(NULL);
			m_nState = AUISCROLL_STATE_NORMAL;
		}
		// Click on scroll bar.
		else if( m_bDragging || rcScrollArea.PtInRect(x, y) )
		{
			if( msg == WM_MOUSEMOVE )
			{
				if( wParam & MK_LBUTTON )
				{
					if( AUISCROLL_STATE_PRESSBAR == m_nState )
					{
						int nOldLevel = m_nBarLevel;
						if( m_bVScroll )
						{
							int H = rcScrollArea.Height() - m_nBarHeight;
							int nPos = y - rcScrollArea.top - m_nDragDist;
							if (nPos < H)
								m_nBarLevel = (int)(nPos * ((float)(m_nScrollEnd - m_nScrollBegin) / H));
							else
								m_nBarLevel = m_nScrollEnd - m_nScrollBegin;
						}
						else
						{
							int W = rcScrollArea.Width() - m_nBarHeight;
							int nPos = x - rcScrollArea.left - m_nDragDist;
							if (nPos < W)
								m_nBarLevel = (int)(nPos * ((float)(m_nScrollEnd - m_nScrollBegin) / W));
							else
								m_nBarLevel = m_nScrollEnd - m_nScrollBegin;
						}
						a_Clamp(m_nBarLevel, m_nScrollBegin, m_nScrollEnd);
						if (nOldLevel != m_nBarLevel)
							m_pParent->SetCommand(m_strCommand.GetLength()?m_strCommand:m_szName);
					}
					else if( AUISCROLL_STATE_PRESSCHANGEPAGE == m_nState )
					{
						if( m_bVScroll )
						{
							if( y < m_ptBar.y )
								m_nDesLevel = m_nBarLevel - ((m_ptBar.y - y - 1) / m_nBarHeight + 1) * m_nBarLength;
							else if( y > m_ptBar.y + m_nBarHeight )
								m_nDesLevel = m_nBarLevel + ((y - m_ptBar.y - 1) / m_nBarHeight) * m_nBarLength;
						}
						else
						{
							if( x < m_ptBar.x )
								m_nDesLevel = m_nBarLevel - ((m_ptBar.x - x - 1) / m_nBarHeight + 1) * m_nBarLength;
							else if( x > m_ptBar.x + m_nBarHeight )
								m_nDesLevel = m_nBarLevel + ((x - m_ptBar.x - 1) / m_nBarHeight) * m_nBarLength;
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
				if( m_bVScroll && y < m_ptBar.y || !m_bVScroll && x < m_ptBar.x )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_bDragging = true;
						if( m_bVScroll )
							m_nDesLevel = m_nBarLevel - ((m_ptBar.y - y - 1) / m_nBarHeight + 1) * m_nBarLength;
						else
							m_nDesLevel = m_nBarLevel - ((m_ptBar.x - x - 1) / m_nBarHeight + 1) * m_nBarLength;
						a_ClampFloor(m_nDesLevel, m_nScrollBegin);
						m_pParent->SetCaptureObject(this);
						m_nState = AUISCROLL_STATE_PRESSCHANGEPAGE;
						m_nScrollDelay = 0;
					}
				}
				// Page down.
				else if( m_bVScroll && y > m_ptBar.y + m_nBarHeight || 
						!m_bVScroll && x > m_ptBar.x + m_nBarHeight )
				{
					if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
					{
						m_bDragging = true;
						if( m_bVScroll )
							m_nDesLevel = m_nBarLevel + ((y - m_ptBar.y - 1) / m_nBarHeight) * m_nBarLength;
						else
							m_nDesLevel = m_nBarLevel + ((x - m_ptBar.x - 1) / m_nBarHeight) * m_nBarLength;
						a_ClampRoof(m_nDesLevel, m_nScrollEnd);
						m_pParent->SetCaptureObject(this);
						m_nState = AUISCROLL_STATE_PRESSCHANGEPAGE;
						m_nScrollDelay = 0;
					}
				}
				// Scroll bar.
				else
				{
					if( msg == WM_LBUTTONDOWN )
					{
						m_bDragging = true;
						if( m_bVScroll )
							m_nDragDist = y - m_ptBar.y;
						else
							m_nDragDist = x - m_ptBar.x;
						m_pParent->SetCaptureObject(this);
						m_nState = AUISCROLL_STATE_PRESSBAR;
					}
				}
			}
		}
		else if( m_bVScroll && x >= rcScrollArea.left && x <= rcScrollArea.right ||
				!m_bVScroll && y >= rcScrollArea.top && y <= rcScrollArea.bottom )
		{
			// Click on up button.
			if( m_bVScroll && y < rcScrollArea.top || !m_bVScroll && x < rcScrollArea.left )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUISCROLL_STATE_PRESSUP;
					m_nScrollDelay = 0;
				}
			}
			// Click on down button.
			else if( m_bVScroll && y > rcScrollArea.bottom || !m_bVScroll && x > rcScrollArea.right )
			{
				if( msg == WM_LBUTTONDOWN || WM_LBUTTONDBLCLK == msg )
				{
					m_nState = AUISCROLL_STATE_PRESSDOWN;
					m_nScrollDelay = 0;
				}
			}
		}

		return true;
	}
	else if( WM_KILLFOCUS == msg )
	{
		m_bDragging = false;
		m_nState = AUISCROLL_STATE_NORMAL;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIScroll::Tick(void)
{
	if( !m_pA2DSprite[AUISCROLL_RECT_SCROLL] )
		return true;

	DWORD dwTick = GetTickCount();
	int nOldLevel = m_nBarLevel;
	if( dwTick - m_nScrollDelay >= AUISCROLL_DELAY )
	{
		bool bStateNormal = false;
		int nBarLevel = m_nBarLevel;
		if( AUISCROLL_STATE_PRESSUP == m_nState )
			nBarLevel -= m_nScrollStep;
		else if( AUISCROLL_STATE_PRESSDOWN == m_nState )
			nBarLevel += m_nScrollStep;
		else if( AUISCROLL_STATE_PRESSCHANGEPAGE == m_nState )
		{
			if( nBarLevel < m_nDesLevel)
			{
				nBarLevel += m_nBarLength;
				a_ClampRoof( nBarLevel, m_nDesLevel);
			}
			else if( nBarLevel > m_nDesLevel)
			{
				nBarLevel -= m_nBarLength;
				a_ClampFloor( nBarLevel, m_nDesLevel);
			}
		}
		else	
			bStateNormal = true;
		if( !bStateNormal )
		{
			SetBarLevel(nBarLevel);
			if( m_nScrollDelay == 0 )
				m_nScrollDelay = dwTick + 500;
			else
				m_nScrollDelay = dwTick;
		}
	}
	if (nOldLevel != m_nBarLevel)
	{
		m_pParent->OnCommand(m_strCommand.GetLength()?m_strCommand:m_szName);
	}

	return AUIObject::Tick();
}

bool AUIScroll::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		if( !m_pA2DSprite[AUISCROLL_RECT_UP] ||
			!m_pA2DSprite[AUISCROLL_RECT_DOWN] ||
			!m_pA2DSprite[AUISCROLL_RECT_SCROLL] ||
			!m_pBarFrame)
		{
			return true;
		}

		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
		bool bval = true;
		POINT ptPos = GetPos();
		A2DSprite *pA2DSpriteFrame = NULL;
		A3DRECT rcUp = m_pA2DSprite[AUISCROLL_RECT_UP]->GetItem(0)->GetRect();
		A3DRECT rcDown = m_pA2DSprite[AUISCROLL_RECT_DOWN]->GetItem(0)->GetRect();
		A3DRECT rcScroll = m_pA2DSprite[AUISCROLL_RECT_SCROLL]->GetItem(0)->GetRect();
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
			rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
		rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
			rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
		rcScroll.SetRect(rcScroll.left * fWindowScale, rcScroll.top * fWindowScale,
			rcScroll.right * fWindowScale, rcScroll.bottom * fWindowScale);

		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		if( m_bVScroll )
		{
			// Draw up button.
			int nItem = (AUISCROLL_STATE_PRESSUP == m_nState) ? 1 : 0;
			bval = m_pA2DSprite[AUISCROLL_RECT_UP]->SetCurrentItem(nItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_UP]->SetCurrentItem()");

			int x = ptPos.x;
			int y = ptPos.y;
			m_pA2DSprite[AUISCROLL_RECT_UP]->SetAlpha(nAlpha);
			m_pA2DSprite[AUISCROLL_RECT_UP]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUISCROLL_RECT_UP]->SetScaleY(fWindowScale);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToBack() or m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToInternalBuffer()");

			// Draw scroll area.
			int H = rcUp.Height();
			int W = rcScroll.Width();
			y += H;

			float fScale = float(m_nHeight - rcUp.Height() - rcDown.Height())
				/ m_pA2DSprite[AUISCROLL_RECT_SCROLL]->GetItem(0)->GetRect().Height();
			m_pA2DSprite[AUISCROLL_RECT_SCROLL]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUISCROLL_RECT_SCROLL]->SetScaleY(fScale);
			m_pA2DSprite[AUISCROLL_RECT_SCROLL]->SetAlpha(nAlpha);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToBack() or m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToInternalBuffer()");

			// Draw down button.
			nItem = (AUISCROLL_STATE_PRESSDOWN == m_nState) ? 1 : 0;
			bval = m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetCurrentItem(nItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetCurrentItem()");

			y = ptPos.y + m_nHeight - rcDown.Height();

			m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetAlpha(nAlpha);
			m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetScaleY(fWindowScale);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToInternalBuffer()");

			// Draw scrollbar button if necessary.
			if( m_nBarLength != 0 && m_nScrollEnd > m_nScrollBegin )
			{
				int H;
				H = m_nHeight - rcUp.Height() - rcDown.Height() - m_nBarHeight;
				m_ptBar.y = rcUp.Height() + (int)(m_nBarLevel * ((float)H / (m_nScrollEnd - m_nScrollBegin)));
				m_pBarFrame->SetScale(fWindowScale);
				bval = m_pBarFrame->Render(ptPos.x, ptPos.y + m_ptBar.y, m_nWidth, m_nBarHeight,
					NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pBarFrame->Render()");
			}
		}
		else
		{
			// Draw up button.
			int nItem = (AUISCROLL_STATE_PRESSUP == m_nState) ? 1 : 0;
			bval = m_pA2DSprite[AUISCROLL_RECT_UP]->SetCurrentItem(nItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_UP]->SetCurrentItem()");

			int x = ptPos.x;
			int y = ptPos.y;
			m_pA2DSprite[AUISCROLL_RECT_UP]->SetAlpha(nAlpha);
			m_pA2DSprite[AUISCROLL_RECT_UP]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUISCROLL_RECT_UP]->SetScaleY(fWindowScale);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToBack() or m_pA2DSprite[AUISCROLL_RECT_UP]->DrawToInternalBuffer()");

			// Draw scroll area.
			int W = rcUp.Width();
			int H = rcScroll.Height();
			x += W;

			float fScale = float(m_nWidth - rcUp.Width() - rcDown.Width())
				/ m_pA2DSprite[AUISCROLL_RECT_SCROLL]->GetItem(0)->GetRect().Width();
			m_pA2DSprite[AUISCROLL_RECT_SCROLL]->SetScaleX(fScale);
			m_pA2DSprite[AUISCROLL_RECT_SCROLL]->SetScaleY(fWindowScale);
			m_pA2DSprite[AUISCROLL_RECT_SCROLL]->SetAlpha(nAlpha);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToBack() or m_pA2DSprite[AUISCROLL_RECT_SCROLL]->DrawToInternalBuffer()");

			// Draw down button.
			nItem = (AUISCROLL_STATE_PRESSDOWN == m_nState) ? 1 : 0;
			bval = m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetCurrentItem(nItem);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetCurrentItem()");

			x = ptPos.x + m_nWidth - rcDown.Width();

			m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetAlpha(nAlpha);
			m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetScaleX(fWindowScale);
			m_pA2DSprite[AUISCROLL_RECT_DOWN]->SetScaleY(fWindowScale);
			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToBack(x, y);
			else
				bval = m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToBack() or m_pA2DSprite[AUISCROLL_RECT_DOWN]->DrawToInternalBuffer()");

			// Draw scrollbar button if necessary.
			if( m_nBarLength != 0 && m_nScrollEnd > m_nScrollBegin )
			{
				int W;
				W = m_nWidth - rcUp.Width() - rcDown.Width() - m_nBarHeight;
				m_ptBar.x = rcUp.Width() + (int)(m_nBarLevel * ((float)W / (m_nScrollEnd - m_nScrollBegin)));
				m_pBarFrame->SetScale(fWindowScale);
				if (m_pBarFrame->GetA2DSprite())
					m_pBarFrame->GetA2DSprite()->SetColor(m_clrBar);
				bval = m_pBarFrame->Render(ptPos.x + m_ptBar.x, ptPos.y, m_nBarHeight, m_nHeight,
					NULL, 0, _AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIScroll::Render(), failed to call m_pBarFrame->Render()");
			}
		}
	}

	return true;
}

bool AUIScroll::GetProperty(const  char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
	{
		if( m_pA2DSprite[AUISCROLL_RECT_UP] )
			strncpy(Property->fn, m_pA2DSprite[AUISCROLL_RECT_UP]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
	{
		if( m_pA2DSprite[AUISCROLL_RECT_DOWN] )
			strncpy(Property->fn, m_pA2DSprite[AUISCROLL_RECT_DOWN]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
	{
		if( m_pA2DSprite[AUISCROLL_RECT_SCROLL] )
			strncpy(Property->fn, m_pA2DSprite[AUISCROLL_RECT_SCROLL]->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
	{
		if( m_pBarFrame )
			strncpy(Property->fn, m_pBarFrame->GetA2DSprite()->GetName(), AFILE_LINEMAXLEN - 1);
	}
	else
		return AUIObject::GetProperty(pszPropertyName, Property);
	
	return true;
}

bool AUIScroll::SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Up Button Image") )
		return InitIndirect(Property->fn, AUISCROLL_RECT_UP);
	else if( 0 == strcmpi(pszPropertyName, "Down Button Image") )
		return InitIndirect(Property->fn, AUISCROLL_RECT_DOWN);
	else if( 0 == strcmpi(pszPropertyName, "Scroll Area Image") )
		return InitIndirect(Property->fn, AUISCROLL_RECT_SCROLL);
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pBarFrame )
			m_pBarFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Scroll Bar Image") )
		return InitIndirect(Property->fn, AUISCROLL_RECT_BAR);
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIScroll::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Up Button Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Down Button Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Scroll Area Image", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT);
}

int AUIScroll::GetScrollStep(void)
{
	return m_nScrollStep;
}

void AUIScroll::SetScrollStep(int nStep)
{
	a_Clamp(nStep, 0, m_nScrollEnd - m_nScrollBegin);
	m_nScrollStep = nStep;
}

int AUIScroll::GetBarLength(void)
{
	return m_nBarLength;
}

void AUIScroll::SetBarLength(int nLength)
{
	if( nLength != m_nBarLength )
		UpdateRenderTarget();

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	m_nBarLength = nLength;
	if( !m_bZoomScroll )
	{
		if (m_pBarFrame)
		{
			if( m_bVScroll )
				m_nBarHeight = m_pBarFrame->GetA2DSprite()->GetHeight() * fWindowScale;
			else
				m_nBarHeight = m_pBarFrame->GetA2DSprite()->GetWidth() * fWindowScale;
		}		
	}
	else
	{
		if (m_pA2DSprite[AUISCROLL_RECT_UP] && m_pA2DSprite[AUISCROLL_RECT_DOWN])
		{
			A3DRECT rcUp = m_pA2DSprite[AUISCROLL_RECT_UP]->GetItem(0)->GetRect();
			A3DRECT rcDown = m_pA2DSprite[AUISCROLL_RECT_DOWN]->GetItem(0)->GetRect();
			rcUp.SetRect(rcUp.left * fWindowScale, rcUp.top * fWindowScale,
				rcUp.right * fWindowScale, rcUp.bottom * fWindowScale);
			rcDown.SetRect(rcDown.left * fWindowScale, rcDown.top * fWindowScale,
				rcDown.right * fWindowScale, rcDown.bottom * fWindowScale);
			if( m_nBarLength > 0 )
			{
				if( m_bVScroll )
				{
					m_nBarHeight = m_nBarLength * (m_nHeight - rcUp.Height() - rcDown.Height()) 
						/ (m_nBarLength + m_nScrollEnd - m_nScrollBegin);
				}
				else
				{
					m_nBarHeight = m_nBarLength * (m_nWidth - rcUp.Width() - rcDown.Width()) 
						/ (m_nBarLength + m_nScrollEnd - m_nScrollBegin);
				}
			}
		}		
	}
}

bool AUIScroll::IsZoomScroll()
{
	return m_bZoomScroll;
}

void AUIScroll::SetZoomScroll(bool bZoom)
{
	if( bZoom != m_bZoomScroll )
		UpdateRenderTarget();

	m_bZoomScroll = bZoom;
	SetBarLength(m_nBarLength);
}

int AUIScroll::GetBarLevel(void)
{
	return m_nBarLevel;
}

void AUIScroll::SetBarLevel(int nLevel)
{
	a_Clamp(nLevel, m_nScrollBegin, m_nScrollEnd);
	if( nLevel != m_nBarLevel )
		UpdateRenderTarget();

	m_nBarLevel = nLevel;
}

int AUIScroll::GetScrollBegin(void)
{
	return m_nScrollBegin;
}

int AUIScroll::GetScrollEnd(void)
{
	return m_nScrollEnd;
}

void AUIScroll::SetScrollRange(int nBegin, int nEnd)
{
	if( nEnd >= nBegin && nBegin >= 0)
	{
		if( nBegin != m_nScrollBegin || nEnd != m_nScrollEnd )
			UpdateRenderTarget();

		m_nScrollBegin = nBegin;
		m_nScrollEnd = nEnd;
		a_Clamp(m_nBarLevel, m_nScrollBegin, m_nScrollEnd);
		if( m_nBarLength > 0 )
		{
			A3DRECT rcUp = m_pA2DSprite[AUISCROLL_RECT_UP]->GetItem(0)->GetRect();
			A3DRECT rcDown = m_pA2DSprite[AUISCROLL_RECT_DOWN]->GetItem(0)->GetRect();
			if( m_bVScroll )
			{
				m_nBarHeight = m_nBarLength * (m_nHeight - rcUp.Height() - rcDown.Height()) 
							 / (m_nBarLength + m_nScrollEnd - m_nScrollBegin);
			}
			else
			{
				m_nBarHeight = m_nBarLength * (m_nWidth - rcUp.Width() - rcDown.Width()) 
							 / (m_nBarLength + m_nScrollEnd - m_nScrollBegin);
			}
		}
	}
}

bool AUIScroll::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	if (m_pBarFrame)
		m_pBarFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	SetBarLength(m_nBarLength);
	return true;
}

void AUIScroll::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIScroll::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	for (int i = 0; i < AUISCROLL_RECT_NUM; i++)
	{
		if (m_pA2DSprite[i])
			glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite[i], bLoad);
	}

	if (m_pBarFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pBarFrame->GetA2DSprite(), bLoad);
}

void AUIScroll::SetBarColor(A3DCOLOR color)
{
	m_clrBar = color;
}