// Filename	: AUIWindowPicture.cpp
// Creator	: Xiao Zhou
// Date		: 2008/11/27
// Desc		: AUIWindowPicture is the class of desktop window control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A2DSprite.h"
#include "A3DTexture.h"

#include "AUIDef.h"
#include "AUIDialog.h"
#include "AUIWindowPicture.h"
#include "AUIManager.h"
#include "AUICommon.h"
#include "WndThreadManager.h"

AUIWindowPicture::AUIWindowPicture() : AUIEditBox()
{
	p_WndThreadManager = NULL;
	p_MessageTranslater = NULL;
	m_nType = AUIOBJECT_TYPE_WINDOWPICTURE;
	m_pA2DSprite = NULL;
	m_hWnd = NULL;
	m_fScale = 1.0f;
	m_rcDisplayRect.SetRect(0, 0, 0, 0);
	m_bBkTransparent = false;
	m_nCaretRef = -1;
	m_bExternalDraw = false;
}

AUIWindowPicture::~AUIWindowPicture()
{
}

bool AUIWindowPicture::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	WindowPicture property line format:
	 *
	 *		WINDOWPICTURE			Keyword
	 *		OK						Name
	 *		100 120 50 15			x y Width Height
	 */

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIWindowPicture::Init(), failed to call base AUIButton::Init()");

	m_pA2DSprite = new A2DSprite;
	if( !m_pA2DSprite ) return AUI_ReportError(__LINE__, 1, "AUIWindowPicture::Init(), no enough memory");

	return true;
}

bool AUIWindowPicture::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_BOOL_VALUE(pItem, _AL("DrawExternal"), m_bExternalDraw)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	m_pA2DSprite = new A2DSprite;
	if( !m_pA2DSprite ) return AUI_ReportError(__LINE__, 1, "AUIWindowPicture::InitXML(), no enough memory");

	return true;
}

bool AUIWindowPicture::Release(void)
{
	A3DRELEASE(m_pA2DSprite);

	return AUIObject::Release();
}

bool AUIWindowPicture::Save(FILE *file)
{
	fprintf(file, "WINDOWPICTURE");

	if( !AUIObject::Save(file) ) return false;

	fprintf(file, " \n");

	return true;
}

bool AUIWindowPicture::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemImagePicture = new AXMLItem;
	pXMLItemImagePicture->InsertItem(pXMLItemDialog);
	pXMLItemImagePicture->SetName(_AL("WINDOWPICTURE"));
	XMLSET_STRING_VALUE(pXMLItemImagePicture, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("Height"), m_nHeight, 0)
	XMLSET_BOOL_VALUE(pXMLItemImagePicture, _AL("DrawExternal"), m_bExternalDraw, 0)

	return true;
}

bool AUIWindowPicture::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{

	CWndThreadManager *pWndThreadManager;
	if(p_WndThreadManager!=NULL)
	{
		pWndThreadManager = p_WndThreadManager;
	}
	else
	{
		pWndThreadManager = GetParent()->GetAUIManager()->GetWndThreadManager();
	}

	if( !(m_dwProperty & IME_PROP_AT_CARET) && m_fStat && msg == WM_KEYDOWN ||
		msg == WM_USER_IME_CHANGE_FOCUS ||
		msg == WM_IME_CHAR ||
		msg == WM_INPUTLANGCHANGE ||
		msg == WM_IME_STARTCOMPOSITION ||
		msg == WM_IME_COMPOSITION ||
		msg == WM_IME_ENDCOMPOSITION ||
		msg == WM_IME_NOTIFY ||
		msg == WM_IME_SETCONTEXT )
	{
		bool bval = AUIEditBox::OnDlgItemMessage(msg, wParam, lParam);
		if( msg == WM_IME_COMPOSITION && (lParam & GCS_RESULTSTR) )
		{
			for(int i = 0;i < m_szText.GetLength(); i++)
				pWndThreadManager->IMECharInPut(m_hWnd, m_szText[i]);
			SetText(_AL(""));
		}

		return bval;
	}
	switch( GetParent()->GetAUIManager()->GetWindowMessageType(msg) )
	{
	case WINDOWMESSAGE_TYPE_MOUSE:
	{

		POINT ptPos = GetPos();
		int x = GET_X_LPARAM(lParam) - m_nOffX - ptPos.x;
		int y = GET_Y_LPARAM(lParam) - m_nOffY - ptPos.y;
		lParam = int(y / m_fScale + m_rcDisplayRect.top) << 16 | int(x / m_fScale + m_rcDisplayRect.left);
		if(p_MessageTranslater!=NULL)
		{
			return p_MessageTranslater->MessageTranslate(GetName(),msg,wParam,lParam);
		}
		pWndThreadManager->MouseEvent(m_hWnd, msg, wParam, lParam);
		return true;
	}

	case WINDOWMESSAGE_TYPE_KEYBOARD:
	{		
		if(p_MessageTranslater!=NULL)
		{
			return p_MessageTranslater->MessageTranslate(GetName(),msg,wParam,lParam);
		}
 		pWndThreadManager->KeyEvent(m_hWnd, msg, wParam, lParam);
		return true;
	}

	default:
		return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
	}
}

bool AUIWindowPicture::Tick(void)
{
	if( !m_bExternalDraw && IsWindow(m_hWnd)  )
	{
		/*
		if( GetParent()->GetAUIManager()->GetWndThreadManager()->FindWindow(m_hWnd) == NULL )
		{
			m_hWnd = NULL;
			return AUIObject::Tick();
		}
		*/
		RECT rc;
		GetWindowRect(m_hWnd, &rc);
		if( !m_pA2DSprite->GetTextures() || m_pA2DSprite->GetWidth() != rc.right - rc.left || m_pA2DSprite->GetHeight() != rc.bottom - rc.top )
		{
			m_pA2DSprite->Release();
			m_pA2DSprite->InitWithoutSurface(m_pA3DDevice, rc.right - rc.left, rc.bottom - rc.top, A3DFMT_A8R8G8B8, 1, NULL);
		}
		HDC hDC = GetWindowDC(m_hWnd);
		if( !hDC )
		{
			BOOL  X =IsWindow(m_hWnd);
			DWORD ERR =GetLastError();
			return true;
		}
		HDC hMemDC = CreateCompatibleDC(hDC);
		if( !hMemDC ) return false;
		HBITMAP hBitmap = CreateCompatibleBitmap(hDC, rc.right - rc.left, rc.bottom - rc.top);
		if( !hBitmap ) return false;
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC ,hBitmap);
		BitBlt(hMemDC, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hDC, 0, 0, SRCCOPY);

		WORD wBitCount=32;
		BITMAP Bitmap;  
		BITMAPINFOHEADER bi;  
		LPBITMAPINFOHEADER lpbi;  
		HANDLE hDib; 

		GetObject(hBitmap, sizeof(Bitmap), &Bitmap);
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = Bitmap.bmWidth;
		bi.biHeight = Bitmap.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrImportant = 0;
		bi.biClrUsed = 0;

		DWORD dwBmBitsSize = Bitmap.bmWidth * wBitCount / 8 * Bitmap.bmHeight;

		hDib = GlobalAlloc(GHND,dwBmBitsSize + sizeof(BITMAPINFOHEADER)); 
		lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib); 
		*lpbi = bi; 

		UINT *p = (UINT*)((unsigned char*)lpbi + sizeof(BITMAPINFOHEADER));
		GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (char*)lpbi + sizeof(BITMAPINFOHEADER) 
			, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
		
		int nPixel = Bitmap.bmWidth * Bitmap.bmHeight;
		if( m_bBkTransparent )
		{
			COLORREF cl = GetSysColor(COLOR_WINDOW);
			UINT cl1 = (GetRValue(cl) << 16) + (GetGValue(cl) << 8) + (GetBValue(cl));
			for(int i = 0; i < nPixel; i++)
			{
				if( *p != cl1 )
					*((unsigned char*)p + 3) = 255;
				p++;
			}
		}
		else
		{
			for(int i = 0; i < nPixel; i++)
			{
				*((unsigned char*)p + 3) = 255;
				p++;
			}
		}

		bool b = m_pA2DSprite->UpdateTextures((unsigned char*)lpbi + sizeof(BITMAPINFOHEADER), Bitmap.bmWidth * wBitCount / 8, A3DFMT_A8R8G8B8);
		GlobalFree(hDib);
		SelectObject(hMemDC ,hOldBitmap);
		DeleteObject(hBitmap);
		DeleteObject(hMemDC);
		ReleaseDC(m_hWnd, hDC);

		HWND hw = GetParent()->GetAUIManager()->GetWndThreadManager()->GetCaretWindowPos(&rc);
		if( hw == m_hWnd )
			m_rcText.SetRect(rc.left * m_fScale, rc.top * m_fScale, rc.right * m_fScale, rc.bottom * m_fScale);
	}

	
	return AUIObject::Tick();
}

bool AUIWindowPicture::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC )
		return true;

	bool bval = true;
	POINT ptPos = GetPos();

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	if( !m_bExternalDraw && m_pA2DSprite && m_pA2DSprite->GetTextures() )
	{
		m_pA3DEngine->FlushInternalSpriteBuffer();
		A3DRECT rc;
		if( m_rcDisplayRect.Width() > 0 && m_rcDisplayRect.Height() > 0 )
			rc = m_rcDisplayRect;
		else
			rc.SetRect(0, 0, m_pA2DSprite->GetWidth(), m_pA2DSprite->GetHeight());
		
		if(	m_pA2DSprite->ResetItems(1, &rc) )
		{
			SIZE sz = GetSize();
			if( m_pA2DSprite->GetWidth() <= sz.cx && m_pA2DSprite->GetHeight() <= sz.cy )
				m_fScale = 1.0f;
			else
			{
				float fScaleX = (float)sz.cx / rc.Width();
				float fScaleY = (float)sz.cy / rc.Height();
				if( fScaleX < fScaleY )
					m_fScale = fScaleX;
				else
					m_fScale = fScaleY;
			}
			m_pA2DSprite->SetScaleX(m_fScale);
			m_pA2DSprite->SetScaleY(m_fScale);

			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite->DrawToBack(ptPos.x, ptPos.y);
			else
				bval = m_pA2DSprite->DrawToInternalBuffer(ptPos.x, ptPos.y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIWindowPicture::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer()");
		}
	}
	
	return AUIEditBox::Render();
}

bool AUIWindowPicture::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Draw External") )
		Property->b = m_bExternalDraw;
	return AUIObject::GetProperty(pszPropertyName, Property);
}

bool AUIWindowPicture::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Draw External") )
		m_bExternalDraw = Property->b;
	return AUIObject::SetProperty(pszPropertyName, Property);
}

void AUIWindowPicture::SetDisplayRect(bool bFullWindow, A3DRECT rcRect)
{
	if( bFullWindow )
		m_rcDisplayRect.SetRect(0, 0, 0, 0);
	else
		m_rcDisplayRect = rcRect;
}

void AUIWindowPicture::SetHWnd(HWND hWnd)
{
	m_hWnd = hWnd;
}
HWND AUIWindowPicture::GetHWnd()
{
	return m_hWnd;
}

void AUIWindowPicture::SetBkTransparent(bool bTransparent)
{
	m_bBkTransparent = bTransparent;
}

A2DSprite * AUIWindowPicture::GetImage()
{
	return m_pA2DSprite;
}

float AUIWindowPicture::GetScale()
{
	return m_fScale;
}

bool AUIWindowPicture::Resize()
{
	return AUIObject::Resize();
}

void AUIWindowPicture::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIWindowPicture::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pA2DSprite)
	{
		if( !bLoad )
			m_pA2DSprite->Release();
	}
}

void AUIWindowPicture::SetExternalDraw(bool bExternalDraw)
{
	m_bExternalDraw = bExternalDraw;
}
	
bool AUIWindowPicture::IsExternalDraw()
{
	return m_bExternalDraw;
}
