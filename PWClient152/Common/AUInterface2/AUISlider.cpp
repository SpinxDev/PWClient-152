// Filename	: AUISlider.cpp
// Creator	: Tom Zhou
// Date		: May 14, 2004
// Desc		: AUISlider is the class of static text control.

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

#include "AUIDialog.h"
#include "AUIManager.h"
#include "AUISlider.h"
#include "AUICommon.h"
#include "AUICTranslate.h"

AUISlider::AUISlider() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_SLIDER;
	m_nFrameMode = 0;
	m_pAUIFrame = NULL;
	m_pA2DSprite = NULL;

	m_nDragDist = 0;
	m_bDragging = false;
	m_nLevel = AUISLIDE_MIN;
	m_nBegin = AUISLIDE_MIN;
	m_nTotal = AUISLIDE_MAX;
	m_bVertical = false;
}

AUISlider::~AUISlider()
{
}

bool AUISlider::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	Slider property line format:
	 *
	 *		SLIDER					Keyword
	 *		Volume					Name
	 *		"SETVOLUME"				Command
	 *		50 300 200 20			x y Width Height
	 *		"SliderFrame.bmp"		Frame image file
	 *		"SliderBar.bmp"			Bar image file
	 *		5						Total levels
	 */

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUISlider::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;

	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));

	if (!InitIndirect(pASF->m_szToken, 0))
		return AUI_ReportError(__LINE__, 1, "AUISlider::Init(), failed to call InitIndirect()");

	CHECK_BREAK2(pASF->GetNextToken(true));

	if (!InitIndirect(pASF->m_szToken, 1))
		return AUI_ReportError(__LINE__, 1, "AUISlider::Init(), failed to call InitIndirect()");

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nTotal = atoi(pASF->m_szToken);

	if( m_pParent->GetFileVersion() > 0 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		AUIOBJECT_SETPROPERTY p;
		p.i = atoi(pASF->m_szToken);
		SetProperty("Frame Mode", &p);
	}

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUISlider::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_INT_VALUE(pItem, _AL("TotalLevels"), m_nTotal)
	XMLGET_BOOL_VALUE(pItem, _AL("VerticalSlider"), m_bVertical)

	XMLGET_STRING_VALUE(pItem, _AL("Command"), m_strCommand)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0)
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameImage")) == 0 )
				{
					XMLGET_INT_VALUE(pResouceChildItem, _AL("FrameMode"), m_nFrameMode)
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, 0);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("BarImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, 1);
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

bool AUISlider::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	UpdateRenderTarget();
	if( 0 == idType )
	{
		A3DRELEASE(m_pAUIFrame);
		if( strlen(pszFileName) <= 0 ) return true;
		
		m_pAUIFrame = new AUIFrame;
		if( !m_pAUIFrame ) return false;

		bval = m_pAUIFrame->Init(m_pA3DEngine, m_pA3DDevice, pszFileName, m_nFrameMode);
		if( !bval )
		{
			A3DRELEASE(m_pAUIFrame)
			return AUI_ReportError(__LINE__, 1, "AUISlider::InitIndirect(), failed to call m_pAUIFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( 1 == idType )
	{
		A3DRELEASE(m_pA2DSprite);
		if( strlen(pszFileName) <= 0 ) return true;
		
		m_pA2DSprite = new A2DSprite;
		if( !m_pA2DSprite ) return false;

#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSprite->SetDynamicTex(true);
#endif //_ANGELICA22
		bval = m_pA2DSprite->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSprite)
			return AUI_ReportError(__LINE__, 1, "AUISlider::InitIndirect(), failed to call m_pA2DSprite->Init()");
		}
	}

	return true;
}

bool AUISlider::Release(void)
{
	A3DRELEASE(m_pAUIFrame);
	A3DRELEASE(m_pA2DSprite);

	return AUIObject::Release();
}

bool AUISlider::Save(FILE *file)
{
	fprintf(file, "SLIDER");

	if( !AUIObject::Save(file) ) return false;

	fprintf(file, " \"%s\" \"%s\" %d %d\n",
		m_pAUIFrame ? m_pAUIFrame->GetA2DSprite()->GetName() : "",
		m_pA2DSprite ? m_pA2DSprite->GetName() : "", m_nTotal, m_nFrameMode);

	return true;
}

bool AUISlider::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemSlider = new AXMLItem;
	pXMLItemSlider->InsertItem(pXMLItemDialog);
	pXMLItemSlider->SetName(_AL("SLIDER"));
	XMLSET_STRING_VALUE(pXMLItemSlider, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemSlider, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemSlider, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemSlider, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemSlider, _AL("Height"), m_nHeight, 0)
	XMLSET_INT_VALUE(pXMLItemSlider, _AL("TotalLevels"), m_nTotal, 0)
	XMLSET_BOOL_VALUE(pXMLItemSlider, _AL("VerticalSlider"), m_bVertical, 0)
	XMLSET_STRING_VALUE(pXMLItemSlider, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemSlider);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	XMLSET_STRING_VALUE(pXMLItemSlider, _AL("Command"), AS2AC(m_strCommand), _AL(""))

	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemSlider);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pAUIFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}
	if( m_pA2DSprite )
	{
		AXMLItem *pXMLItemBaeImage = new AXMLItem;
		pXMLItemBaeImage->InsertItem(pXMLItemResource);
		pXMLItemBaeImage->SetName(_AL("BarImage"));
		XMLSET_STRING_VALUE(pXMLItemBaeImage, _AL("FileName"), AS2AC(m_pA2DSprite->GetName()), _AL(""))
	}

	return true;
}

bool AUISlider::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	POINT ptPos = GetPos();

	if( WM_LBUTTONDOWN == msg || WM_LBUTTONDBLCLK == msg )
	{
		int x = GET_X_LPARAM(lParam) - m_nOffX - ptPos.x;
		int y = GET_Y_LPARAM(lParam) - m_nOffY - ptPos.y;

		if( m_rcBar.PtInRect(x, y) )
		{
			if (!m_bVertical)
				SetDragging(true, x - m_rcBar.left);
			else
				SetDragging(true, y - m_rcBar.top);
		}
		else if( ((!m_bVertical && x < m_rcBar.left) || (m_bVertical && y > m_rcBar.bottom)) 
				 && m_nLevel > m_nBegin )
		{
			SetLevel(m_nLevel - 1);
			if( m_strCommand != "" )
				m_pParent->SetCommand(m_strCommand);
			else
				m_pParent->SetCommand(m_szName);
		}
		else if( ((!m_bVertical && x > m_rcBar.right) || (m_bVertical && y < m_rcBar.top)) 
			     && m_nLevel < m_nTotal )
		{
			SetLevel(m_nLevel + 1);
			if( m_strCommand != "" )
				m_pParent->SetCommand(m_strCommand);
			else
				m_pParent->SetCommand(m_szName);
		}

		return true;
	}
	else if( WM_LBUTTONUP == msg )
	{
		SetDragging(false);
		return true;
	}
	else if( WM_MOUSEMOVE == msg )
	{
		if( wParam & MK_LBUTTON )
		{
			int nOffset = 0;
			int nEffectLen = 0;
			if (!m_bVertical)
			{
				int W1 = 0, W2 = 0, W3 = m_pA2DSprite->GetWidth() * fWindowScale;
				if( m_pAUIFrame )
				{
					W1 = m_pAUIFrame->GetSizeLimit().cx / 2;
					W2 = W1;
				}
				nEffectLen = m_nWidth - W1 - W2 - W3;
				nOffset = GET_X_LPARAM(lParam) - ptPos.x - W1 - m_nDragDist;
			}
			else
			{
				int H1 = 0, H2 = 0, H3 = m_pA2DSprite->GetHeight() * fWindowScale;
				if( m_pAUIFrame )
				{
					H1 = m_pAUIFrame->GetSizeLimit().cy / 2;
					H2 = H1;
				}
				nEffectLen = m_nHeight - H1 - H2 - H3;
				nOffset = GET_Y_LPARAM(lParam) - ptPos.y - H1 - m_nDragDist;
				nOffset = nEffectLen - nOffset;
			}
			if( m_bDragging )
			{
				int nTotal = m_nTotal - m_nBegin;
				float fPercent = (float)nOffset / (nEffectLen - nEffectLen / (nTotal * 2));
				int nNewLevel = m_nBegin;
				if (fPercent > 1)
					nNewLevel += nTotal;
				else
					nNewLevel += nTotal * fPercent;
				
				a_Clamp(nNewLevel, m_nBegin, m_nTotal);
				if( nNewLevel != m_nLevel )
				{
					SetLevel(nNewLevel);
					if( m_strCommand != "" )
						m_pParent->SetCommand(m_strCommand);
					else
						m_pParent->SetCommand(m_szName);
				}
			}
		}
		else
		{
			SetDragging(false);
		}

		return true;
	}
	else if( WM_KILLFOCUS == msg )
	{
		SetDragging(false);
		return true;
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUISlider::Tick(void)
{
	return AUIObject::Tick();
}

bool AUISlider::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		if( !m_pA2DSprite ) return AUIObject::Render();
		
		float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

		bool bval = true;
		POINT ptPos = GetPos();

		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		A3DCOLOR color = m_color;
		if( !m_bEnabled )
		{
			A3DCOLOR r = A3DCOLOR_GETRED(m_color) / 2;
			A3DCOLOR g = A3DCOLOR_GETRED(m_color) / 2;
			A3DCOLOR b = A3DCOLOR_GETRED(m_color) / 2;
			color = A3DCOLORRGB(r, g, b);
		}

		if( m_pAUIFrame )
		{
			bval = m_pAUIFrame->Render(ptPos.x, ptPos.y, m_nWidth, m_nHeight, 
				NULL, color, _AL(""), AUIFRAME_ALIGN_LEFT,
				0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
		}
		else
			bval = AUIObject::Render();
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISlider::Render(), failed to call m_pAUIFrame->Render() or base AUIObject::Render()");

		if( m_pA2DSprite )
		{
			int W = m_pA2DSprite->GetWidth() * fWindowScale;
			int H = m_pA2DSprite->GetHeight() * fWindowScale;
			int W1 = 0, W2 = 0;
			if( m_pAUIFrame )
			{
				if( m_nFrameMode == 1 )
				{
					W2 = W1 = 0;
				}
				else
				{
					if (m_bVertical) 
						W1 = m_pAUIFrame->GetSizeLimit().cx / 2;
					else
						W1 = m_pAUIFrame->GetSizeLimit().cy / 2;
					W2 = W1;
				}
			}
			int x = 0, y = 0;
			if (!m_bVertical)
			{
				x = ptPos.x + W1 + (int)((m_nWidth - W1 - W2 - W) * ((float)(m_nLevel - m_nBegin) / (m_nTotal - m_nBegin)));
				y = ptPos.y + m_nHeight / 2 - H / 2;
			}
			else
			{
				x = ptPos.x + m_nWidth / 2 - W / 2;
				y = ptPos.y + W1 + (int)((m_nHeight - W1 - W2 - W) * ((float)(m_nTotal - m_nLevel) / (m_nTotal - m_nBegin)));
			}
			
			m_pA2DSprite->SetColor(color);
			m_pA2DSprite->SetAlpha(nAlpha);
			m_pA2DSprite->SetScaleX(fWindowScale);
			m_pA2DSprite->SetScaleY(fWindowScale);

			if( m_pParent->IsNoFlush() )
				bval = m_pA2DSprite->DrawToBack(x, y);
			else
				bval = m_pA2DSprite->DrawToInternalBuffer(x, y);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUISlider::Render(), failed to call m_pA2DSprite->DrawToBack() or m_pA2DSprite->DrawToInternalBuffer()");

			m_rcBar.left = x - ptPos.x;
			m_rcBar.top = y - ptPos.y;
			m_rcBar.right = m_rcBar.left + W;
			m_rcBar.bottom = m_rcBar.top + H;
		}
	}

	return true;
}

bool AUISlider::GetProperty(const  char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Frame Mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image File") )
		strncpy(Property->fn, m_pAUIFrame ? m_pAUIFrame->GetA2DSprite()->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Bar Image File") )
		strncpy(Property->fn, m_pA2DSprite ? m_pA2DSprite->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Total Levels") )
		Property->i = m_nTotal;
	else if( 0 == strcmpi(pszPropertyName, "Vertical Slider"))
		Property->b = m_bVertical;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUISlider::SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
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
	else if( 0 == strcmpi(pszPropertyName, "Frame mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image File") )
		return InitIndirect(Property->fn, 0);
	else if( 0 == strcmpi(pszPropertyName, "Bar Image File") )
		return InitIndirect(Property->fn, 1);
	else if( 0 == strcmpi(pszPropertyName, "Total Levels") )
		m_nTotal = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Vertical Slider"))
		m_bVertical = Property->b;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUISlider::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Frame Image File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Bar Image File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Total Levels", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame mode", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Vertical Slider", AUIOBJECT_PROPERTY_VALUE_BOOL);
}

int AUISlider::GetLevel(void)
{
	return m_nLevel;
}

void AUISlider::SetLevel(int nLevel)
{
	a_Clamp(nLevel, m_nBegin, m_nTotal);
	if( m_nLevel != nLevel )
		UpdateRenderTarget();

	m_nLevel = nLevel;
}

int AUISlider::GetBegin(void)
{
	return m_nBegin;
}

int AUISlider::GetTotal(void)
{
	return m_nTotal;
}

void AUISlider::SetTotal(int nTotal, int nBegin)
{
	if( nTotal > nBegin && nBegin >= 0)
	{
		if( nBegin != m_nBegin || nTotal != m_nTotal)
			UpdateRenderTarget();

		m_nBegin = nBegin;
		m_nTotal = nTotal;
		a_Clamp(m_nLevel, m_nBegin, m_nTotal);
	}
}

void AUISlider::SetDragging(bool bDragging, int nDragDist)
{
	m_bDragging = bDragging;
	if( bDragging )
	{
		m_pParent->SetCaptureObject(this);
		if( nDragDist == -1 )
			nDragDist = m_rcBar.Width() / 2;
		m_nDragDist = nDragDist;
	}
	else
		m_pParent->SetCaptureObject(NULL);
}

A3DRECT AUISlider::GetRect(void)
{
	if( !m_pA2DSprite)
		return AUIObject::GetRect();
	
	int H = m_pA2DSprite->GetHeight();
	A3DRECT rc = AUIObject::GetRect();
	int nFreeHeight = (H - m_nHeight) / 2;

	if( nFreeHeight > 0 )
	{
		rc.top -= nFreeHeight;
		rc.bottom += nFreeHeight;
	}

	return rc;
}

bool AUISlider::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	return true;
}

void AUISlider::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUISlider::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pA2DSprite)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite, bLoad);

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);
}

bool AUISlider::IsVerticalSlider()
{
	return m_bVertical;
}

void AUISlider::SetVerticalSlider(bool bVertical)
{
	m_bVertical = bVertical;
}