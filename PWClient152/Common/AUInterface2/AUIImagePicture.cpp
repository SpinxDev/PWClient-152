// Filename	: AUIImagePicture.cpp
// Creator	: Tom Zhou
// Date		: May 19, 2004
// Desc		: AUIImagePicture is the class of still image and animation control.

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
#include "A3DTexture.h"

#include "AUIDialog.h"
#include "AUIImagePicture.h"
#include "AUIManager.h"
#include "AUICommon.h"
#include "AUICTranslate.h"
#include "A3DGFXExMan.h"

AUIImagePicture::sImageRotate::sImageRotate()
{
	memset(this, 0, sizeof(*this));
}

AUIImagePicture::AUIImagePicture() : AUIPicture()
{
	m_nType = AUIOBJECT_TYPE_IMAGEPICTURE;
	m_nInterval = 0;
	m_nLastTime = 0;
	m_nFrameNow = 0;
	m_nNumFrames = 0;
	m_nFixFrame = -1;
	m_pA2DSprite = NULL;
	for(int i = 0; i < AUIIMAGEPICTURE_MAXCOVERS; i++)
	{
		m_nItem[i] = 0;
		m_pA2DSpriteCover[i] = NULL;
	}
	m_nClockLayer = 0;
	m_pA3DTexture = NULL;
	m_bAcceptMouseMsg = false;
	m_dwParamCallback1 = 0;
	m_dwParamCallback2 = 0;
	m_dwParamCallback3 = 0;
	m_pRenderCallback = NULL;
	m_nFadeInterval = 0;
	m_pGfx = NULL;
	m_bTextureIsRenderTarget = false;
	m_bRenderTop = false;
}

AUIImagePicture::~AUIImagePicture()
{
}

bool AUIImagePicture::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	Image Picture property line format:
	 *
	 *		IMAGEPICTURE		Keyword
	 *		Logo				Name
	 *		1					String ID of command.
	 *		0 0 50 50			x y Width Height
	 *		1					NumFrames
	 *		500					Interval, in MS
	 *		"Logo.bmp"			ImageFileName
	 */

	if (!AUIPicture::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Init(), falied to call base AUIPicture::Init()");

	m_ClockCounter.Init(m_pA3DEngine, m_pA3DDevice);

	if( !pASF ) return true;

	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nNumFrames = atoi(pASF->m_szToken);
	if( m_nNumFrames > 0 && m_nNumFrames <= AUIIMAGEPICTURE_MAXFRAMES )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		m_nInterval = atoi(pASF->m_szToken);
		//ASSERT(m_nInterval > 0);

		CHECK_BREAK2(pASF->GetNextToken(true));

		// Maybe no image.
		if( strlen(pASF->m_szToken) > 0 )
		{
			if (!InitIndirect(pASF->m_szToken, 0) || !SetFrameNumber(m_nNumFrames))
				return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Init(), failed to call InitIndirect()");

			if( m_nWidth <= 0 )
			{
				m_nWidth = m_pA2DSprite->GetWidth();
			}
			if( m_nHeight <= 0 )
			{
				m_nHeight = m_pA2DSprite->GetHeight() / m_nNumFrames;
			}
		}
	}
	else
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		CHECK_BREAK2(pASF->GetNextToken(true));
	}

	int nSize = m_pParent->GetAUIManager()->GetDefaultFontSize();
	if( nSize > 0 )
		SetFontAndColor(GetParent()->GetAUIManager()->GetDefaultFontName(), nSize, m_color, 0, 1);

	if (m_pParent->GetFileVersion() >= 3)
	{
		CHECK_BREAK2(pASF->GetNextToken(true));

		if( strlen(pASF->m_szToken) > 0 )
			SetGfx(pASF->m_szToken);
	}

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIImagePicture::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIImagePicture::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	
	m_ClockCounter.Init(m_pA3DEngine, m_pA3DDevice);

	m_nNumFrames = 1;
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_BOOL_VALUE(pItem, _AL("RenderTop"), m_bRenderTop)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;

	FONT_PARAM* fp = GetParent()->GetAUIManager()->GetImagePictureFontParam();
	m_nShadow = fp->nShadow;
	SetFontAndColor(fp->szFontName, fp->nFontSize, m_color, fp->nShadow, fp->nOutline, fp->bBold, fp->bItalic, fp->bFreeType);

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
					XMLGET_INT_VALUE(pResouceChildItem, _AL("Frames"), m_nNumFrames)
					XMLGET_INT_VALUE(pResouceChildItem, _AL("Interval"), m_nInterval)
					int nImageDegree=0;
					XMLGET_INT_VALUE(pResouceChildItem, _AL("Degree"), nImageDegree);
					SetImageDegree(nImageDegree);
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, 0);
					SetFrameNumber(m_nNumFrames);
					if( m_nWidth <= 0 )
					{
						m_nWidth = m_pA2DSprite->GetWidth();
					}
					if( m_nHeight <= 0 )
					{
						m_nHeight = m_pA2DSprite->GetHeight() / m_nNumFrames;
					}
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("Gfx")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					SetGfx(strFileName);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("Clock")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					SetClock(strFileName);
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

	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;
	
	return true;
}

bool AUIImagePicture::InitIndirect(const char *pszFileName, int idType)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);

	UpdateRenderTarget();
	if( pszFileName && strlen(pszFileName) > 0 )
	{
		A2DSprite *pA2DSprite = new A2DSprite;
		if( !pA2DSprite ) return false;
#ifdef _ANGELICA22
		//d3d9ex
		//if (m_bAnomaly) //FIXME!! 编辑器里面没有设置是否需要D3DUSAGE_DYNAMIC，现在默认都是DYNAMIC
			pA2DSprite->SetDynamicTex(true);
#endif//_ANGELICA22

		bval = pA2DSprite->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(pA2DSprite)
			return AUI_ReportError(__LINE__, 1, "AUIImagePicture::InitIndirect(), failed to call m_pA2DSprite->Init()");
		}
		A3DRELEASE(m_pA2DSprite);
		m_pA2DSprite = pA2DSprite;
	}
	else
	{
		A3DRELEASE(m_pA2DSprite);
		m_nNumFrames = 0;
	}

	return true;
}

bool AUIImagePicture::SetFrameNumber(int numFrame)
{
	bool bval = true;

	ASSERT(m_pA3DEngine && m_pA3DDevice);
	if (!m_pA2DSprite || numFrame<0 || numFrame>AUIIMAGEPICTURE_MAXFRAMES)
		return true;

	UpdateRenderTarget();
	if (numFrame > 0)
	{
		int nWidth = m_pA2DSprite->GetWidth();
		int nHeight = m_pA2DSprite->GetHeight() / numFrame;
		
		A3DRECT a_rc[AUIIMAGEPICTURE_MAXFRAMES];
		for( int i = 0; i < numFrame; i++ )
			a_rc[i].SetRect(0, i * nHeight, nWidth, (i + 1) * nHeight);
		bval = m_pA2DSprite->ResetItems(numFrame, a_rc);
	}
	else
	{
		bval = m_pA2DSprite->ResetItems(0, NULL);
	}
	
	if (!bval) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::InitIndirect(), failed to call m_pA2DSprite->ResetItems()");

	m_nNumFrames = numFrame;
	return true;
}

bool AUIImagePicture::Release(void)
{
	A3DRELEASE(m_pA2DSprite);

	if (m_pGfx)
	{
		AfxGetGFXExMan()->CacheReleasedGfx(m_pGfx);
		m_pGfx = NULL;
	}

	m_ClockCounter.Release();

	return AUIPicture::Release();
}

bool AUIImagePicture::Save(FILE *file)
{
	fprintf(file, "IMAGEPICTURE");

	if( !AUIPicture::Save(file) ) return false;

	fprintf(file, " %d %d \"%s\" \"%s\"\n", m_nNumFrames,
		m_nInterval, m_pA2DSprite ? m_pA2DSprite->GetName() : "", m_strGfx);

	return true;
}

bool AUIImagePicture::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemImagePicture = new AXMLItem;
	pXMLItemImagePicture->InsertItem(pXMLItemDialog);
	pXMLItemImagePicture->SetName(_AL("IMAGEPICTURE"));
	XMLSET_STRING_VALUE(pXMLItemImagePicture, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemImagePicture, _AL("Height"), m_nHeight, 0)
	XMLSET_BOOL_VALUE(pXMLItemImagePicture, _AL("RenderTop"), m_bRenderTop, false)
	XMLSET_STRING_VALUE(pXMLItemImagePicture, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemImagePicture);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))
	

	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemImagePicture);
	pXMLItemResource->SetName(_AL("Resource"));

	if( m_pA2DSprite )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pA2DSprite->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("Frames"), m_nNumFrames, 1)
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("Interval"), m_nInterval, 0)
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("Degree"), m_imageRotate.nDegree, 0);
	}

	if( m_strGfx != "" )
	{
		AXMLItem *pXMLItemGfx = new AXMLItem;
		pXMLItemGfx->InsertItem(pXMLItemResource);
		pXMLItemGfx->SetName(_AL("Gfx"));
		XMLSET_STRING_VALUE(pXMLItemGfx, _AL("FileName"), AS2AC(m_strGfx), _AL(""))
	}

	if( m_strClock != "" )
	{
		AXMLItem *pXMLItemClock = new AXMLItem;
		pXMLItemClock->InsertItem(pXMLItemResource);
		pXMLItemClock->SetName(_AL("Clock"));
		XMLSET_STRING_VALUE(pXMLItemClock, _AL("FileName"), AS2AC(m_strClock), _AL(""))
	}

	return true;
}

bool AUIImagePicture::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return AUIPicture::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIImagePicture::Tick(void)
{
	if (m_pGfx)
	{
		DWORD dwTick = GetTickCount();
		POINT ptPos = GetPos();
		ptPos.x += m_nOffX + m_nWidth / 2;
		ptPos.y += m_nOffY + m_nHeight / 2;
		m_pGfx->SetParentTM(a3d_Translate(ptPos.x, ptPos.y, 0));
		m_pGfx->TickAnimation(m_pParent->GetAUIManager()->GetTickTime());
	}
	
	return AUIPicture::Tick();
}

bool AUIImagePicture::Render(void)
{
	bool bval = true;
	int nAlpha;
	POINT ptPos = GetPos();

	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	if( m_nFadeInterval > 0 )
		nAlpha = abs(int(m_nFadeInterval / 2 - GetTickCount() % m_nFadeInterval)) * 2 * m_nAlpha / m_nFadeInterval;
	else
		nAlpha = m_nAlpha;

	nAlpha = int(m_pParent->GetWholeAlpha() * nAlpha / 255);

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender())
	{
		if( !m_pParent->GetImageCoverOrderly() || m_pParent->GetCurImageCover() == 0 )
		{
			if( m_pRenderCallback )
			{
				if( m_pA2DSprite )
				{
					int nFrame = m_nFixFrame >= 0 ? m_nFixFrame : m_nFrameNow;
					if( m_nDefaultWidth < 0 )
						m_nWidth = m_pA2DSprite->GetItem(nFrame)->GetRect().Width() * fWindowScale;
					if( m_nDefaultHeight < 0 )
						m_nHeight = m_pA2DSprite->GetItem(nFrame)->GetRect().Height() * fWindowScale;

					if( m_nWidth >= 0 && m_nHeight >= 0 )
					{
						float fScaleX = float(m_nWidth) / float(m_pA2DSprite->GetItem(nFrame)->GetRect().Width());
						float fScaleY = float(m_nHeight) / float(m_pA2DSprite->GetItem(nFrame)->GetRect().Height());

						m_pA2DSprite->SetScaleX(fScaleX);
						m_pA2DSprite->SetScaleY(fScaleY);

						m_pA2DSprite->SetColor(m_color);
						m_pA2DSprite->SetAlpha(nAlpha);
						m_pA2DSprite->SetCurrentItem(nFrame);

						UpdateImageDegree();
						if( m_pParent->IsNoFlush() )
							bval = m_pA2DSprite->DrawToBack(ptPos.x+m_imageRotate.iOffsetX, ptPos.y+m_imageRotate.iOffsetY);
						else
							bval = m_pA2DSprite->DrawToInternalBuffer(ptPos.x+m_imageRotate.iOffsetX, ptPos.y+m_imageRotate.iOffsetY);
						if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), failed to call m_pA2DSpriteCover->DrawToBack() or m_pA2DSpriteCover->DrawToInternalBuffer()");

						m_pA2DSprite->SetColor(A3DCOLORRGB(255, 255, 255));

						if( m_nInterval > 0 && m_nNumFrames > 1 )
						{
							DWORD dwTick = GetTickCount();
							if( int(dwTick - m_nLastTime) >= m_nInterval )
							{
								m_nFrameNow = (m_nFrameNow + 1) % m_nNumFrames;
								m_nLastTime = dwTick;
							}
						}
					}
				}
			}
			else if( m_pA3DTexture )
			{
				int i;
				A3DTLVERTEX vert[4];
				A3DRECT rc = GetRect();
				static WORD a_wIndex[6] = { 0, 1, 2, 2, 1, 3 };

				for( i = 0; i < 4; i++ )
				{
					vert[i].z = 0.0f;
					vert[i].rhw = 1.0f;
					vert[i].diffuse = 0xFFFFFFFF;
					vert[i].specular = 0xFF000000;
					vert[i].tu = float(i % 2);
					vert[i].tv = float(i / 2);
				}

				rc.Offset(m_nOffX, m_nOffY);
				vert[0].x = float(rc.left);
				vert[0].y = float(rc.top);
				vert[1].x = float(rc.right);
				vert[1].y = float(rc.top);
				vert[2].x = float(rc.left);
				vert[2].y = float(rc.bottom);
				vert[3].x = float(rc.right);
				vert[3].y = float(rc.bottom);

				m_pA3DDevice->SetLightingEnable(false);
#ifdef _ANGELICA22
				m_pA3DDevice->SetFVF(A3DFVF_A3DTLVERTEX);
#else
				m_pA3DDevice->SetDeviceVertexShader(A3DFVF_A3DTLVERTEX);
#endif
				m_pA3DDevice->ClearPixelShader();
				m_pA3DDevice->ClearTexture(0);
				m_pA3DTexture->Appear(0);
				m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

				if (m_bTextureIsRenderTarget)
				{
					m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
					m_pA3DDevice->SetDestAlpha(A3DBLEND_SRCALPHA);
				}

				bval = m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0,
					4, 2, a_wIndex, A3DFMT_INDEX16, vert, sizeof(A3DTLVERTEX));
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), failed to call m_pA3DDevice->DrawIndexedPrimitiveUP()");

				m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
				if (m_bTextureIsRenderTarget)
				{					
					m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
					m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
				}

				m_pA3DTexture->Disappear(0);
				m_pA3DDevice->SetLightingEnable(true);
			}
			else if(m_pA2DSpriteCover[0] && 
					m_nItem[0] >= 0 && 
					m_nItem[0] < m_pA2DSpriteCover[0]->GetItemNum() &&
					m_pA2DSpriteCover[0]->GetItem(m_nItem[0]))
			{
				A3DRECT rc = m_pA2DSpriteCover[0]->GetItem(m_nItem[0])->GetRect();
				if(rc.Width() && rc.Height())
				{
					float fScaleX = float(m_nWidth) / float(rc.Width());
					float fScaleY = float(m_nHeight) / float(rc.Height());

					m_pA2DSpriteCover[0]->SetScaleX(fScaleX);
					m_pA2DSpriteCover[0]->SetScaleY(fScaleY);

					m_pA2DSpriteCover[0]->SetColor(m_color);
					m_pA2DSpriteCover[0]->SetAlpha(nAlpha);
					m_pA2DSpriteCover[0]->SetCurrentItem(m_nItem[0]);

					if( m_pParent->IsNoFlush() )
						bval = m_pA2DSpriteCover[0]->DrawToBack(ptPos.x, ptPos.y);
					else
						bval = m_pA2DSpriteCover[0]->DrawToInternalBuffer(ptPos.x, ptPos.y);
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), failed to call m_pA2DSpriteCover->DrawToBack() or m_pA2DSpriteCover->DrawToInternalBuffer()");
					
					m_pA2DSpriteCover[0]->SetColor(A3DCOLORRGB(255, 255, 255));
				}else
				{
					AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), rc.Width() or rc.Height() is zero");
				}
			}
			else if( m_pA2DSprite )
			{
				int nFrame = m_nFixFrame >= 0 ? m_nFixFrame : m_nFrameNow;
				if( m_nDefaultWidth < 0 )
					m_nWidth = m_pA2DSprite->GetItem(nFrame)->GetRect().Width() * fWindowScale;
				if( m_nDefaultHeight < 0 )
					m_nHeight = m_pA2DSprite->GetItem(nFrame)->GetRect().Height() * fWindowScale;

				if( m_nWidth >= 0 && m_nHeight >= 0 )
				{
					float fScaleX = float(m_nWidth) / float(m_pA2DSprite->GetItem(nFrame)->GetRect().Width());
					float fScaleY = float(m_nHeight) / float(m_pA2DSprite->GetItem(nFrame)->GetRect().Height());

					m_pA2DSprite->SetScaleX(fScaleX);
					m_pA2DSprite->SetScaleY(fScaleY);

					m_pA2DSprite->SetColor(m_color);
					m_pA2DSprite->SetAlpha(nAlpha);
					m_pA2DSprite->SetCurrentItem(m_nFixFrame >= 0 ? m_nFixFrame : m_nFrameNow);

					UpdateImageDegree();
					if( m_pParent->IsNoFlush())
						bval = m_pA2DSprite->DrawToBack(ptPos.x+m_imageRotate.iOffsetX, ptPos.y+m_imageRotate.iOffsetY);
					else
						bval = m_pA2DSprite->DrawToInternalBuffer(ptPos.x+m_imageRotate.iOffsetX, ptPos.y+m_imageRotate.iOffsetY);
					if( !bval ) 
						return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), failed to call m_pA2DSpriteCover->DrawToBack() or m_pA2DSpriteCover->DrawToInternalBuffer()");

					m_pA2DSprite->SetColor(A3DCOLORRGB(255, 255, 255));

					if( m_nInterval > 0 && m_nNumFrames > 1 )
					{
						DWORD dwTick = GetTickCount();
						if( int(dwTick - m_nLastTime) >= m_nInterval )
						{
							m_nFrameNow = (m_nFrameNow + 1) % m_nNumFrames;
							m_nLastTime = dwTick;
						}
					}
				}
			}
			else
			{
				bval = AUIPicture::Render();
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), failed to call base AUIPicture::Render()");
			}
		}
	}

	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL || 
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC)
	{
		if( !m_pParent->GetImageCoverOrderly() || m_pParent->GetCurImageCover() == 0 )
		{
			if( m_pRenderCallback )
			{
				// first flush all sprite here to make the background shown
				bval = m_pA3DEngine->FlushInternalSpriteBuffer();
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

				A3DRECT rc = GetRect();
				rc.Offset(m_nOffX, m_nOffY);
#ifdef _ANGELICA22
				//Reset Render State to Default
				//Editor By Pan Yupin, 2013/1/21
				bool bOldZWriteEnable = m_pA3DDevice->GetZWriteEnable();
				bool bOldZTestEnable = m_pA3DDevice->GetZTestEnable();
				bool bOldAlphaBlendEnable = m_pA3DDevice->GetAlphaBlendEnable();
				bool bOldAlphaTestEnable = m_pA3DDevice->GetAlphaTestEnable();
				A3DCMPFUNC oldAlphaFunc = m_pA3DDevice->GetAlphaFunction();
				DWORD dwOldAlphaRef = m_pA3DDevice->GetAlphaRef();

				m_pA3DDevice->SetZWriteEnable(true);
				m_pA3DDevice->SetZTestEnable(true);
				m_pA3DDevice->SetAlphaBlendEnable(false);
				m_pA3DDevice->SetAlphaTestEnable(true);
				m_pA3DDevice->SetAlphaFunction(A3DCMP_GREATER);
				m_pA3DDevice->SetAlphaRef(84);
#else
				BOOL bOldZWriteEnable = FALSE;
				BOOL bOldZTestEnable = FALSE;
				IDirect3DDevice8* pRawDevice = m_pA3DDevice->GetD3DDevice();
				if(pRawDevice != NULL)
				{
					pRawDevice->GetRenderState(D3DRS_ZWRITEENABLE, (DWORD*)(&bOldZWriteEnable));
					pRawDevice->GetRenderState(D3DRS_ZENABLE, (DWORD*)(&bOldZTestEnable));
				}

				m_pA3DDevice->SetZWriteEnable(true);
				m_pA3DDevice->SetZTestEnable(true);
#endif
				(*m_pRenderCallback)(rc, m_dwParamCallback1, m_dwParamCallback2, m_dwParamCallback3);

#ifdef _ANGELICA22
				m_pA3DDevice->SetZWriteEnable(bOldZWriteEnable);
				m_pA3DDevice->SetZTestEnable(bOldZTestEnable);
				m_pA3DDevice->SetAlphaBlendEnable(bOldAlphaBlendEnable);
				m_pA3DDevice->SetAlphaTestEnable(bOldAlphaTestEnable);
				m_pA3DDevice->SetAlphaFunction(oldAlphaFunc);
				m_pA3DDevice->SetAlphaRef(dwOldAlphaRef);
#else
				m_pA3DDevice->SetZWriteEnable(bOldZWriteEnable);
				m_pA3DDevice->SetZTestEnable(bOldZTestEnable);
#endif

			}

			if (m_nClockLayer == 0)
			{
				m_ClockCounter.SetIconPos(ptPos.x, ptPos.y);
				m_ClockCounter.SetIconSize(m_nWidth, m_nHeight);
				m_ClockCounter.Render();
			}
		}

		int nMin, nMax;
		if( !m_pParent->GetImageCoverOrderly() )
		{
			nMin = 1;
			nMax = AUIIMAGEPICTURE_MAXCOVERS;
		}
		else
		{
			nMin = m_pParent->GetCurImageCover();
			if( nMin > 0 )
				nMax = nMin + 1;
			else
				nMax = 0;
		}
		for(int i = nMin; i < nMax; i++)
		{
			if( m_pA2DSpriteCover[i] && 
				m_nItem[i] >= 0 && 
				m_nItem[i] < m_pA2DSpriteCover[i]->GetItemNum() &&
				m_pA2DSpriteCover[i]->GetItem(m_nItem[i]))
			{
				A3DRECT rc = m_pA2DSpriteCover[i]->GetItem(m_nItem[i])->GetRect();
				if(rc.Width() && rc.Height())
				{
					float fScaleX = float(m_nWidth) / float(rc.Width());
					float fScaleY = float(m_nHeight) / float(rc.Height());

					m_pA2DSpriteCover[i]->SetScaleX(fScaleX);
					m_pA2DSpriteCover[i]->SetScaleY(fScaleY);

					m_pA2DSpriteCover[i]->SetColor(A3DCOLORRGB(255, 255, 255));
					m_pA2DSpriteCover[i]->SetAlpha(nAlpha);
					m_pA2DSpriteCover[i]->SetCurrentItem(m_nItem[i]);

					if( m_pParent->IsNoFlush() )
						bval = m_pA2DSpriteCover[i]->DrawToBack(ptPos.x, ptPos.y);
					else
						bval = m_pA2DSpriteCover[i]->DrawToInternalBuffer(ptPos.x, ptPos.y);
					if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), failed to call m_pA2DSpriteCover->DrawToBack() or m_pA2DSpriteCover->DrawToInternalBuffer()");
				}else
				{
					AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), rc.Width() or rc.Height() is zero");
				}
			}
			if( m_nClockLayer == i )
			{
				m_ClockCounter.SetIconPos(ptPos.x, ptPos.y);
				m_ClockCounter.SetIconSize(m_nWidth, m_nHeight);
				m_ClockCounter.Render();
			}
		}

		if( !m_pParent->GetImageCoverOrderly() || m_pParent->GetCurImageCover() == AUIIMAGEPICTURE_MAXCOVERS - 1 )
		{
			if( m_szText.GetLength() > 0 )
			{
				A3DPOINT2 ptSize;
				int nHeight;
				A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
				ptSize = pFont->GetTextExtent(_AL("Wjg"));
				nHeight = ptSize.y;

				int x = ptPos.x + 1;
				int y = ptPos.y + m_nHeight - nHeight - 1;

				ACHAR szColor[20];
				A3DCOLOR r = A3DCOLOR_GETRED(m_color);
				A3DCOLOR g = A3DCOLOR_GETGREEN(m_color);
				A3DCOLOR b = A3DCOLOR_GETBLUE(m_color);
				a_sprintf(szColor, _AL("^%02X%02X%02X"), r, g, b);
				bval = AUI_TextOutFormat(pFont, x, y, szColor + m_szText,
					NULL, 0, NULL, 3, false, NULL, NULL, NULL, NULL, true, m_nShadow, nAlpha);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), Failed to call AUI_TextOutFormat()");
			}

			if( m_pGfx )
			{
				bval = m_pA3DEngine->FlushInternalSpriteBuffer();
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::Render(), Failed to call m_pA3DEngine->FlushInternalSpriteBuffer()");

				m_pParent->GetAUIManager()->GetA3DFTFontMan()->Flush();

				BOOL bZWrite = FALSE;
#ifdef _ANGELICA22
				bZWrite = m_pA3DDevice->GetZWriteEnable();
#else
				IDirect3DDevice8* pRawDevice = m_pA3DDevice->GetD3DDevice();
				if(pRawDevice != NULL)
				{
					pRawDevice->GetRenderState(D3DRS_ZWRITEENABLE, (DWORD*)(&bZWrite));
				}
#endif
				if(!bZWrite)
					m_pA3DDevice->SetZWriteEnable(true);
				
				m_pGfx->SetAlpha(float(nAlpha) / 255.0f);
				AfxGetGFXExMan()->RegisterGfx(m_pGfx);
				AfxGetGFXExMan()->Render2DGfx(false);
				
				if(!bZWrite)
					m_pA3DDevice->SetZWriteEnable(false);
			}
		}
	}
	
	return true;
}

void AUIImagePicture::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
		OptimizeSpriteResource(bShow);

	if( m_pParent->IsShow() && bShow && !m_bShow )
		StartGfx();

	AUIPicture::Show(bShow);
}

void AUIImagePicture::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pA2DSprite)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSprite, bLoad);
}

bool AUIImagePicture::IsInHitArea(int x, int y, UINT msg /* = WM_LBUTTONDOWN */)
{
	if( !AUIObject::IsInHitArea(x, y, msg) )
		return false;

	if( !m_bAnomaly )
		return true;

	if( !m_pA2DSprite )
		return false;
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int px = int((x - GetPos().x) / fWindowScale);
	int py = int((y - GetPos().y) / fWindowScale);
	DWORD color = AUI_A2DSpriteGetPixel(m_pA2DSprite, px , py);
	return A3DCOLOR_GETALPHA(color) > 0;
}

bool AUIImagePicture::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Image File") )
		strncpy(Property->fn, m_pA2DSprite ? m_pA2DSprite->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Gfx File") )
		strncpy(Property->fn, m_strGfx, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Clock File") )
		strncpy(Property->fn, m_strClock, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Frames Number") )
		Property->i = m_nNumFrames;
	else if( 0 == strcmpi(pszPropertyName, "Frame Interval") )
		Property->i = m_nInterval;
	else if( 0 == strcmpi(pszPropertyName, "Image Degree") )
		Property->i = m_imageRotate.nDegree;
	else if( 0 == strcmpi(pszPropertyName, "Render Top") )
		Property->b = m_bRenderTop;
	else
		return AUIPicture::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIImagePicture::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Image File") )
		return InitIndirect(Property->fn, 0);
	else if( 0 == strcmpi(pszPropertyName, "Gfx File") )
		return SetGfx(Property->fn);
	else if( 0 == strcmpi(pszPropertyName, "Clock File") )
		return SetClock(Property->fn);
	else if( 0 == strcmpi(pszPropertyName, "Frames Number") )
		return SetFrameNumber(Property->i);
	else if( 0 == strcmpi(pszPropertyName, "Frame Interval") )
		m_nInterval = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Image Degree") )
	{
		SetImageDegree(Property->i);
	}
	else if( 0 == strcmpi(pszPropertyName, "Render Top") )
		m_bRenderTop = Property->b;
	else
		return AUIPicture::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIImagePicture::SetGfx(const char* szGfx)
{
	if (m_pGfx)
	{
		AfxGetGFXExMan()->CacheReleasedGfx(m_pGfx);
		m_pGfx = NULL;
	}

	m_strGfx = "";
	if( szGfx && strlen(szGfx) > 0 )
	{
		m_strGfx = szGfx;

#ifdef _DX9
		m_pGfx = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, szGfx, true, false);
#else
		m_pGfx = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, szGfx);
#endif

		if( m_pGfx )
			m_pGfx->Start();

		return m_pGfx != NULL;
	}

	return true;
}

void AUIImagePicture::StartGfx()
{ 
	if( m_pGfx ) 
		m_pGfx->Start(); 
}

bool AUIImagePicture::SetClock(const char* szClock)
{
	m_strClock = szClock;
	return m_ClockCounter.SetClockImage(m_strClock);
}


bool AUIImagePicture::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Image File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Gfx File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Frames Number", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame Interval", AUIOBJECT_PROPERTY_VALUE_INT)&&
			SetAUIObjectPropertyType("Image Degree", AUIOBJECT_PROPERTY_VALUE_INT);
}

void AUIImagePicture::FixFrame(int nIndex)
{
	if( nIndex >= 0 && nIndex < m_nNumFrames )
	{
		if( nIndex != m_nFixFrame )
			UpdateRenderTarget();

		m_nFixFrame = nIndex;
	}
}

void AUIImagePicture::ClearCover()
{
	bool b = false;
	for(int i = 0; i < AUIIMAGEPICTURE_MAXCOVERS; i++)
	{
		if( m_nItem[i] != 0 || m_pA2DSpriteCover[i] )
			b = true;
		m_nItem[i] = 0;
		m_pA2DSpriteCover[i] = NULL;
	}

	if( b )
		UpdateRenderTarget();
}

void AUIImagePicture::SetCover(A2DSprite *pA2DSprite, int nItem, int nLayer)
{
	if( nLayer < 0 || nLayer >= AUIIMAGEPICTURE_MAXCOVERS )
	{
		ASSERT(0);
		return;
	}
	
	if( nLayer == 0 &&
		(pA2DSprite != m_pA2DSpriteCover[nLayer] || m_nItem[nLayer] != nItem) )
		UpdateRenderTarget();

	m_pA2DSpriteCover[nLayer] = pA2DSprite;
	m_nItem[nLayer] = nItem;
}

void AUIImagePicture::GetCover(A2DSprite **ppA2DSprite, int *pnItem, int nLayer)
{
	if( nLayer < 0 || nLayer >= AUIIMAGEPICTURE_MAXCOVERS )
	{
		ASSERT(0);
		return;
	}

	if( ppA2DSprite )
		*ppA2DSprite = m_pA2DSpriteCover[nLayer];
	if( pnItem )
		*pnItem = m_nItem[nLayer];
}

void AUIImagePicture::ClearTexture()
{
	m_pA3DTexture = NULL;
	m_bTextureIsRenderTarget = false;
}

void AUIImagePicture::GetTexture(A3DTexture **ppA3DTexture)
{
	*ppA3DTexture = m_pA3DTexture;
}

void AUIImagePicture::SetTexture(A3DTexture *pA3DTexture, bool bIsRenderTarget)
{
	m_pA3DTexture = pA3DTexture;
	m_bTextureIsRenderTarget = bIsRenderTarget;
}

AUIClockIcon * AUIImagePicture::GetClockIcon()
{
	return &m_ClockCounter;
}

void AUIImagePicture::SetRenderCallback(LPFRENDERCALLBACK pCallback,
	DWORD param1, DWORD param2, DWORD param3)
{
	m_dwParamCallback1 = param1;
	m_dwParamCallback2 = param2;
	m_dwParamCallback3 = param3;
	m_pRenderCallback = pCallback;
}

bool AUIImagePicture::InitIndirectFromMemory(
	const char *pszFileName, const void *pBuf, int nLen)
{
	bool bval = true;

	A3DRELEASE(m_pA2DSprite);

	m_pA2DSprite = new A2DSprite;
	if( !m_pA2DSprite ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::InitIndirectFromMemory(), no enough memory");

	bval = m_pA2DSprite->InitFromFileInMemory(
		m_pA3DDevice, pszFileName, pBuf, nLen, AUI_COLORKEY);
	if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIImagePicture::InitIndirectFromMemory(), failed to call m_pA2DSprite->InitFromFileInMemory()");

	return true;
}

bool AUIImagePicture::Resize()
{
	if( m_pGfx )
		m_pGfx->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());

	return AUIObject::Resize();
}

void AUIImagePicture::SetImageDegree(const int nDegree)
{
	m_imageRotate.nDegree	= nDegree;
	m_imageRotate.nWidth	= m_nWidth;
	m_imageRotate.nHeight	= m_nHeight;
	m_imageRotate.pSprite   = m_pA2DSprite;
	if(!m_imageRotate.nDegree || m_imageRotate.nWidth<=0 || m_imageRotate.nHeight<=0)
	{
		m_imageRotate.iOffsetX	= 0;
		m_imageRotate.iOffsetY	= 0;
		if(m_imageRotate.pSprite)
		{
			m_imageRotate.pSprite->SetDegree(0);
		}
	}else
	{
		const float  fRadian  = -DEG2RAD(m_imageRotate.nDegree)-atan2((float)m_imageRotate.nHeight, (float)m_imageRotate.nWidth);
		const float  fLen = sqrt((float)m_imageRotate.nWidth*m_imageRotate.nWidth+m_imageRotate.nHeight*m_imageRotate.nHeight)/2;
		const float  fNewCenterX =  fLen*cos(fRadian);
		const float  fNewCenterY =  -fLen*sin(fRadian);
		m_imageRotate.iOffsetX = m_imageRotate.nWidth/2  - fNewCenterX;
		m_imageRotate.iOffsetY = m_imageRotate.nHeight/2 - fNewCenterY;	
		if(m_imageRotate.pSprite)
		{
			m_imageRotate.pSprite->SetDegree(m_imageRotate.nDegree);
		}	
	}
}

void AUIImagePicture::UpdateImageDegree()
{
	if(!m_imageRotate.nDegree)
	{
		return;
	}
	if( m_imageRotate.nWidth==m_nWidth && m_imageRotate.nHeight==m_nHeight && m_imageRotate.pSprite==m_pA2DSprite)
	{
		return;
	}
	SetImageDegree(m_imageRotate.nDegree);
}
