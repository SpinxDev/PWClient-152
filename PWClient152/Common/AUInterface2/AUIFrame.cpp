// Filename	: AUIFrame.cpp
// Creator	: Tom Zhou
// Date		: June 9, 2004
// Desc		: AUIFrame is the common class of window frame.

#include "AUI.h"

#include <Windows.h>

#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A2DSpriteBuffer.h"

#include "AUIFrame.h"
#include "AUIDialog.h"
#include "AUIObject.h"
#include "AUIManager.h"
#include "AUICommon.h"

AUIFrame::AUIFrame()
{
	m_nMode = MODE_3X3;
	m_pA3DDevice = NULL;
	m_pA3DEngine = NULL;
	m_pA2DSprite = NULL;
	m_fScale = 1.0f;
	m_iSpriteOffsetX = 0;
	m_iSpriteOffsetY = 0;
}

AUIFrame::~AUIFrame()
{
}

bool AUIFrame::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszFilename, int nMode)
{
	ASSERT(pA3DEngine && pA3DDevice);

	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;

	bool bval = InitIndirect(pszFilename, nMode);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Init(), Failed to call InitIndirect()");

	return true;
}

bool AUIFrame::InitIndirect(const char *pszFilename, int nMode)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	A3DRELEASE(m_pA2DSprite);

	if( strlen(pszFilename) <= 0 ) return true;

	m_pA2DSprite = new A2DSprite;
	if( !m_pA2DSprite ) return false;

#ifdef _ANGELICA22
	//d3d9ex
	m_pA2DSprite->SetDynamicTex(true);
#endif //_ANGELICA22
	
	bval = m_pA2DSprite->Init(m_pA3DDevice, pszFilename, AUI_COLORKEY);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::InitIndirect(), failed to call m_pA2DSprite->Init()");
	
	SetMode(nMode);
	return true;
}

bool AUIFrame::SetMode(int nMode)
{
	m_nMode = MODE_1X1;
	if( !m_pA2DSprite )
		return false;

	bool bval = true;
	A3DRECT a_rc[9];
	int W = m_pA2DSprite->GetWidth();
	int H = m_pA2DSprite->GetHeight();
	if(nMode == MODE_3X3 || nMode == MODE_UNKNOWN)
	{
		if( W / 3 * 3 == W && H / 3 * 3 == H )
		{
			W /= 3;
			H /= 3;
			for( int i = 0; i < 3; i++ )
			{
				for( int j = 0; j < 3; j++ )
				{
					a_rc[i * 3 + j].SetRect(W * j, H * i, W * j + W, H * i + H);
				}
			}
			bval = m_pA2DSprite->ResetItems(AUIFRAME_RECT_NUM, a_rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::InitIndirect(), failed to call m_pA2DSprite->ResetItems()");
			m_nMode = MODE_3X3;
			return true;
		}
	}
	if(nMode == MODE_4X4 || nMode == MODE_UNKNOWN)
	{
		if( W / 4 * 4 == W && H / 4 * 4 == H )
		{
			W /= 4;
			H /= 4;
			a_rc[0].SetRect(W * 0, H * 0, W * 1, H * 1);
			a_rc[1].SetRect(W * 1, H * 0, W * 3, H * 1);
			a_rc[2].SetRect(W * 3, H * 0, W * 4, H * 1);
			a_rc[3].SetRect(W * 0, H * 1, W * 1, H * 3);
			a_rc[4].SetRect(W * 1, H * 1, W * 3, H * 3);
			a_rc[5].SetRect(W * 3, H * 1, W * 4, H * 3);
			a_rc[6].SetRect(W * 0, H * 3, W * 1, H * 4);
			a_rc[7].SetRect(W * 1, H * 3, W * 3, H * 4);
			a_rc[8].SetRect(W * 3, H * 3, W * 4, H * 4);
			bval = m_pA2DSprite->ResetItems(AUIFRAME_RECT_NUM, a_rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::InitIndirect(), failed to call m_pA2DSprite->ResetItems()");
			m_nMode = MODE_4X4;
			return true;
		}
	}
	if(nMode == MODE_3X1 || nMode == MODE_UNKNOWN)
	{
		if( W / 3 * 3 == W )
		{
			W /= 3;
			for( int i = 0; i < 3; i++ )
				for( int j = 0; j < 3; j++ )
					a_rc[i * 3 + j].SetRect(W * j, 0, W * j + W, H);
			bval = m_pA2DSprite->ResetItems(AUIFRAME_RECT_NUM, a_rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::InitIndirect(), failed to call m_pA2DSprite->ResetItems()");
			m_nMode = MODE_3X1;
			return true;
		}
	}
	if(nMode == MODE_1X3 || nMode == MODE_UNKNOWN)
	{
		if( H / 3 * 3 == H )
		{
			H /= 3;
			for( int i = 0; i < 3; i++ )
				for( int j = 0; j < 3; j++ )
					a_rc[i * 3 + j].SetRect(0, H * i, W, H * i + H);
			bval = m_pA2DSprite->ResetItems(AUIFRAME_RECT_NUM, a_rc);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::InitIndirect(), failed to call m_pA2DSprite->ResetItems()");
			m_nMode = MODE_1X3;
			return true;
		}
	}
	a_rc[0].SetRect(0, 0, m_pA2DSprite->GetWidth(), m_pA2DSprite->GetHeight());
	bval = m_pA2DSprite->ResetItems(1, a_rc);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::SetMode(), failed to call m_pA2DSprite->ResetItems()");
	m_nMode = MODE_1X1;

	return true;
}

void AUIFrame::SetScale(float fScale)
{
	m_fScale = fScale;
}

bool AUIFrame::Release(void)
{
	A3DRELEASE(m_pA2DSprite);

	return true;
}

bool AUIFrame::Save(FILE *file)
{
	fprintf(file, " \"%s\"", m_pA2DSprite->GetName());

	return true;
}

bool AUIFrame::Render(int x, int y, int nWidth, int nHeight, A3DFTFont *pFont,
	A3DCOLOR color, const ACHAR *pszText, int nAlign, int xOffset, int yOffset,
	int nAlpha, bool bNoFlush, int nShadow, bool bVertical)
{
	// Render when image is OK.
	ASSERT(m_pA2DSprite);

	if( !m_pA2DSprite )
		return true;

	// for Compatibility with old version
	if((nShadow & 0xFF) == nShadow)
	{
		nShadow <<= 24;
	}

	bool bval = true;
	int xPos = x;
	int yPos = y;
	int W = m_pA2DSprite->GetWidth();
	int H = m_pA2DSprite->GetHeight();
	if( m_nMode == MODE_3X3 || m_nMode == MODE_3X1 )
		W /= 3;
	else if( m_nMode == MODE_3X3 || m_nMode == MODE_1X3 )
		H /= 3;
	
	m_pA2DSprite->SetAlpha(nAlpha);

	if( m_nMode == MODE_1X1 )
	{
		m_pA2DSprite->SetScaleX(float(nWidth) / m_pA2DSprite->GetWidth());
		m_pA2DSprite->SetScaleY(float(nHeight) / m_pA2DSprite->GetHeight());
		bval = DrawSprite(xPos, yPos,bNoFlush);
	}
	else
	{
		// Render when image size is OK.
//		ASSERT( nWidth >= W * 2 && nHeight >= H * 2 );

		int x0, x1, x2, x3, y0, y1, y2, y3;
		float sx0, sx1, sx2, sy0, sy1, sy2;
		x0 = xPos;
		if( m_nMode == MODE_1X3 )
		{
			x1 = xPos + nWidth;
			sx0 = float(x1 - x0) / m_pA2DSprite->GetWidth();
		}
		else
		{
			int nLeftW = m_pA2DSprite->GetItem(AUIFRAME_RECT_LEFT)->GetRect().Width() * m_fScale;
			int nRightW = m_pA2DSprite->GetItem(AUIFRAME_RECT_RIGHT)->GetRect().Width() * m_fScale;
			int nLimitW = nLeftW + nRightW;

			if (nWidth >= nLimitW)
			{				
				x1 = x0 + nLeftW;
				x3 = xPos + nWidth;
				x2 = x3 - nRightW;
			}
			else
			{
				if (nLimitW > 0)
				{
					x1 = x0 + nWidth*nLeftW/nLimitW;
					x3 = xPos + nWidth;
					x2 = x1;
				}
				else
				{
					x1 = x0;
					x2 = x0;
					x3 = x0;
				}
			}
			
			sx0 = float(x1 - x0) / m_pA2DSprite->GetItem(AUIFRAME_RECT_UPPERLEFT)->GetRect().Width();
			sx1 = float(x2 - x1) / m_pA2DSprite->GetItem(AUIFRAME_RECT_TOP)->GetRect().Width();
			sx2 = float(x3 - x2) / m_pA2DSprite->GetItem(AUIFRAME_RECT_UPPERRIGHT)->GetRect().Width();
			
		}
		y0 = yPos;
		if( m_nMode == MODE_3X1 )
		{
			y1 = yPos + nHeight;
			sy0 = float(y1 - y0) / m_pA2DSprite->GetHeight();
		}
		else
		{
			int nTopH = m_pA2DSprite->GetItem(AUIFRAME_RECT_TOP)->GetRect().Height() * m_fScale;
			int nBotH = m_pA2DSprite->GetItem(AUIFRAME_RECT_BOTTOM)->GetRect().Height() * m_fScale;
			int nLimitH = nTopH + nBotH;

			if (nHeight >= nLimitH)
			{
				y1 = y0 + nTopH;
				y3 = yPos + nHeight;
				y2 = y3 - nBotH;
			}
			else
			{
				if (nLimitH > 0)
				{
					y1 = y0 + nHeight*nTopH/nLimitH;
					y3 = yPos + nHeight;
					y2 = y1;
				}
				else
				{
					y1 = y0;
					y2 = y0;
					y3 = y0;
				}
			}
			
			
			sy0 = float(y1 - y0) / m_pA2DSprite->GetItem(AUIFRAME_RECT_UPPERLEFT)->GetRect().Height();
			sy1 = float(y2 - y1) / m_pA2DSprite->GetItem(AUIFRAME_RECT_LEFT)->GetRect().Height();
			sy2 = float(y3 - y2) / m_pA2DSprite->GetItem(AUIFRAME_RECT_LOWERLEFT)->GetRect().Height();
		}
		// Upper left corner.
		bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_UPPERLEFT);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw left corner");

		m_pA2DSprite->SetScaleX(sx0);
		m_pA2DSprite->SetScaleY(sy0);
		bval = DrawSprite(x0, y0,bNoFlush);
		if( !bval )
		{
			AString str;
			str.Format("AUIFrame::Render(), failed to call m_pA2DSprite(%s)->DrawToBack or m_pA2DSprite->DrawToInternalBuffer(), when draw left corner",
				m_pA2DSprite->GetName());
			return AUI_ReportError(__LINE__, 1, str);
		}

		// Top border.
		if( m_nMode != MODE_1X3 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_TOP);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw top border");

			m_pA2DSprite->SetScaleX(sx1);
			m_pA2DSprite->SetScaleY(sy0);
			
			bval = DrawSprite(x1, y0,bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw top border");
		}

		// Upper right corner.
		if( m_nMode != MODE_1X3 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_UPPERRIGHT);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw right corner");

			m_pA2DSprite->SetScaleX(sx2);
			m_pA2DSprite->SetScaleY(sy0);
			bval = DrawSprite(x2, y0,bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw right corner");
		}

		// Left border.
		if( m_nMode != MODE_3X1 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_LEFT);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw left border");

			m_pA2DSprite->SetScaleX(sx0);
			m_pA2DSprite->SetScaleY(sy1);
			
			bval = DrawSprite(x0, y1, bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw left border");
		}
		
		// Background.
		if( m_nMode != MODE_3X1 && m_nMode != MODE_1X3 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_BACK);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw background");

			m_pA2DSprite->SetScaleX(sx1);
			m_pA2DSprite->SetScaleY(sy1);
			bval = DrawSprite(x1, y1, bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw background");
		}

		// Right border.
		if( m_nMode != MODE_3X1 && m_nMode != MODE_1X3 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_RIGHT);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw right border");

			m_pA2DSprite->SetScaleX(sx2);
			m_pA2DSprite->SetScaleY(sy1);
			
			bval = DrawSprite(x2, y1, bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw right border");
		}

		// Lower left corner.
		if( m_nMode != MODE_3X1 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_LOWERLEFT);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw left corner");

			m_pA2DSprite->SetScaleX(sx0);
			m_pA2DSprite->SetScaleY(sy2);
			bval = DrawSprite(x0, y2, bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw left corner");
		}

		// Bottom border.
		if( m_nMode != MODE_3X1 && m_nMode != MODE_1X3 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_BOTTOM);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw bottom border");

			m_pA2DSprite->SetScaleX(sx1);
			m_pA2DSprite->SetScaleY(sy2);
			
			bval = DrawSprite(x1, y2, bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw bottom border");
		}
		
		// Lower right corner.
		if( m_nMode != MODE_3X1 && m_nMode != MODE_1X3 )
		{
			bval = m_pA2DSprite->SetCurrentItem(AUIFRAME_RECT_LOWERRIGHT);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->SetCurrentItem(), when draw Lower right corner");

			m_pA2DSprite->SetScaleX(sx2);
			m_pA2DSprite->SetScaleY(sy2);
			bval = DrawSprite(x2, y2, bNoFlush);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer(), when draw Lower right corner");
		}
	}

	if( bNoFlush )
	{
		bval = m_pA2DSprite->GetTextures()->GetSpriteBuffer()->FlushBuffer();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIFrame::Render(), failed to call m_pA2DSprite->GetTextures()->GetSpriteBuffer()->FlushBuffer(), when draw Lower right corner");
	}

	// Text.
	if( pszText && a_strlen(pszText) > 0 )
	{
		ASSERT(pFont);

		if (!bVertical)
		{	
			A3DPOINT2 ptSize;
			int nTextW = 0, nTextH = 0;
			ptSize = pFont->GetTextExtent(pszText);
			nTextW = ptSize.x;
			nTextH = ptSize.y;
			
			y = yPos + (nHeight - nTextH + 1) / 2 - 1;
			if( AUIFRAME_ALIGN_RIGHT == nAlign )
				x = xPos + nWidth - W - nTextW;
			else if( AUIFRAME_ALIGN_CENTER == nAlign )
				x = xPos + nWidth / 2 - nTextW / 2;
			else
				x = xPos + W;
			pFont->TextOut(x + xOffset, y + yOffset, pszText, (nAlpha << 24) | (0xFFFFFF & color),
				(nShadow & 0xFF000000) ? true : false, nShadow);
		}
		else
		{
			int nTextW = pFont->GetFontWidth();
			int nTextH = pFont->GetFontHeight() * a_strlen(pszText);
			
			x = xPos + (nWidth - nTextW + 1) / 2 - 1;
			if( AUIFRAME_ALIGN_RIGHT == nAlign )
				y = yPos + nHeight - H - nTextH;
			else if( AUIFRAME_ALIGN_CENTER == nAlign )
				y = yPos + nHeight / 2 - nTextH / 2;
			else
				y = yPos + H;

			pFont->TextOutVert(x + xOffset, y + yOffset, pszText, (nAlpha << 24) | (0xFFFFFF & color),
				(nShadow & 0xFF000000) ? true : false, nShadow);
		}			
	}

	return true;
}

SIZE AUIFrame::GetSizeLimit(void)
{
	SIZE s = { 0, 0 };

	if( m_pA2DSprite )
	{
		if( m_nMode == MODE_1X1 )
		{
			s.cx = 8 * m_fScale;
			s.cy = 8 * m_fScale;
		}
		else
		{
			int W;
			int H;
			if( m_nMode == MODE_4X4 )
				W = m_pA2DSprite->GetWidth() / 4;
			else if( m_nMode == MODE_3X1 || m_nMode == MODE_3X3 )
				W = m_pA2DSprite->GetWidth() / 3;
			else
				W = 4;

			if( m_nMode == MODE_4X4 )
				H = m_pA2DSprite->GetHeight() / 4;
			else if( m_nMode == MODE_1X3 || m_nMode == MODE_3X3 )
				H = m_pA2DSprite->GetHeight() / 3;
			else
				H = 4;

			s.cx = W * 2 * m_fScale;
			s.cy = H * 2 * m_fScale;
		}
	}

	return s;
}

A2DSprite * AUIFrame::GetA2DSprite(void)
{
	return m_pA2DSprite;
}

DWORD AUIFrame::GetPixel(int x, int y, int nDefRenderWidth, int nDefRenderHeight)
{
	if( !m_pA2DSprite )
		return 0;

	int px = x;
	int py = y;
	if( m_nMode == MODE_4X4 )
	{
		int W;
		W = m_pA2DSprite->GetWidth() / 4;
		if( x < W )
			px = x;
		else if( x >= nDefRenderWidth - W )
			px = m_pA2DSprite->GetWidth() + x - nDefRenderWidth;
		else
			px = W + (x - W) * W * 2 / (nDefRenderWidth - W * 2);
	}
	else if( m_nMode == MODE_3X1 || m_nMode == MODE_3X3 )
	{
		int W;
		W = m_pA2DSprite->GetWidth() / 3;
		if( x < W )
			px = x;
		else if( x >= nDefRenderWidth - W )
			px = m_pA2DSprite->GetWidth() + x - nDefRenderWidth;
		else
			px = W + (x - W) * W / (nDefRenderWidth - W * 2);
	}
	else
		px = x * m_pA2DSprite->GetWidth() / nDefRenderWidth;

	if( m_nMode == MODE_4X4 )
	{
		int H;
		H = m_pA2DSprite->GetHeight() / 4;
		if( y < H )
			py = y;
		else if( y >= nDefRenderHeight - H )
			py = m_pA2DSprite->GetHeight() + y - nDefRenderHeight;
		else
			py = H + (y - H) * H * 2 / (nDefRenderHeight - H * 2);
	}
	else if( m_nMode == MODE_1X3 || m_nMode == MODE_3X3 )
	{
		int H;
		H = m_pA2DSprite->GetHeight() / 3;
		if( y < H )
			py = y;
		else if( y >= nDefRenderHeight - H )
			py = m_pA2DSprite->GetHeight() + y - nDefRenderHeight;
		else
			py = H + (y - H) * H / (nDefRenderHeight - H * 2);
	}
	else
		py = y * m_pA2DSprite->GetHeight() / nDefRenderHeight;

	return AUI_A2DSpriteGetPixel(m_pA2DSprite, px , py);
}

void  AUIFrame::SetSpriteOffset(const int xOffset, const int yOffset)
{
	m_iSpriteOffsetX = xOffset;
	m_iSpriteOffsetY = yOffset;
}

bool  AUIFrame::DrawSprite(const int x, const int y, const bool bNoFlush)
{
	if(!m_pA2DSprite)
	{
		return false;
	}
	if( bNoFlush )
		return m_pA2DSprite->DrawToBack(x+m_iSpriteOffsetX, y+m_iSpriteOffsetY);
	else
		return m_pA2DSprite->DrawToInternalBuffer(x+m_iSpriteOffsetX, y+m_iSpriteOffsetY);
}