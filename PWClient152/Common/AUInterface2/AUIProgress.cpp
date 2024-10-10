// Filename	: AUIProgress.cpp
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIProgress is the class of static text control.

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
#include "AUIProgress.h"
#include "AUICommon.h"
#include "AUICTranslate.h"
#include "A3DGFXExMan.h"
#include "AFI.h"

#ifdef _ANGELICA22
static const char* cs_sdrfile = NULL;
#else
static const char* cs_sdrfile = "Shaders\\Progress.sdr";
#endif
enum 
{
	eStagePosBg = 0, 
	eStagePos = 1,
	eStagePos2 = 2
};

struct CustTlVtx
{
	float x, y, z, rhw;
	DWORD diffuse;
	float tu1, tv1;
	float tu2, tv2;
	CustTlVtx(){}
	void Set(const A3DVECTOR4& vPos, DWORD _diffuse, float _tu1, float _tv1, float _tu2, float _tv2)
	{
		x = vPos.x;
		y = vPos.y;
		z = vPos.z;
		diffuse = _diffuse;
		rhw = vPos.w;
		tu1 = _tu1;
		tv1 = _tv1;
		tu2 = _tu2;
		tv2 = _tv2;
	}
};
#define CUSTVTX_FMT D3DFVF_XYZRHW|D3DFVF_TEX2|D3DFVF_DIFFUSE

AUIProgress::AUIProgress() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_PROGRESS;
	m_nFrameMode = 0;
	m_pAUIFrame = NULL;
	m_pA2DSpriteBar = NULL;
	m_pA2DSpritePos = NULL;
	m_pA2DSpriteInc = NULL;
	m_pA2DSpriteDec = NULL;
	m_nProgress = 0;
	m_bAutoProgress = false;
	m_bActionProgress = false;
	m_bAcceptMouseMsg = false;
	m_rcFillMargin.SetRect(0, 0, 0, 0);
	m_bVProgress = false;
	m_bReverseDir = false;
	m_nRangeMax = 100;
	m_pShaderAutoPos = NULL;
	m_pStreamAutoPos = NULL;
	m_bCanAutoFixPosImage = false;
	m_pPosGfx = NULL;
}

AUIProgress::~AUIProgress()
{
}

bool AUIProgress::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	Label property line format:
	 *
	 *		PROGRESS				Keyword
	 *		Loader					Name
	 *		""						Command
	 *		50 300 200 20			x y Width Height
	 *		"ProgressFrame.bmp"		Frame image file
	 *		"ProgressFill.bmp"		Fill image file
	 *		5 5 5 5					Fill margin
	 */

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIProgress::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;


	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));

	CHECK_BREAK2(InitIndirect(pASF->m_szToken, 0));

	CHECK_BREAK2(pASF->GetNextToken(true));

	CHECK_BREAK2(InitIndirect(pASF->m_szToken, 1));

	if( m_pParent->GetFileVersion() >= 6 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));

		CHECK_BREAK2(InitIndirect(pASF->m_szToken, 2));
	}

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_rcFillMargin.left = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_rcFillMargin.top = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_rcFillMargin.right = atoi(pASF->m_szToken);

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_rcFillMargin.bottom = atoi(pASF->m_szToken);

	if( m_pParent->GetFileVersion() > 0 )
	{
		CHECK_BREAK2(pASF->GetNextToken(true));
		AUIOBJECT_SETPROPERTY p;
		p.i = atoi(pASF->m_szToken);
		SetProperty("Frame Mode", &p);
	}
	
	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIProgress::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIProgress::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)
	XMLGET_BOOL_VALUE(pItem, _AL("VerticalProgress"), m_bVProgress)
	XMLGET_BOOL_VALUE(pItem, _AL("ReverseDirection"), m_bReverseDir)
	XMLGET_INT_VALUE(pItem, _AL("FillMarginLeft"), m_rcFillMargin.left)
	XMLGET_INT_VALUE(pItem, _AL("FillMarginTop"), m_rcFillMargin.top)
	XMLGET_INT_VALUE(pItem, _AL("FillMarginRight"), m_rcFillMargin.right)
	XMLGET_INT_VALUE(pItem, _AL("FillMarginBottom"), m_rcFillMargin.bottom)
	XMLGET_BOOL_VALUE(pItem, _AL("ForceDynamicRender"), m_bForceDynamicRender)
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
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("FillImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, 1);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("PosImage")) == 0 )
				{
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), m_strPosImage)
					InitIndirect(m_strPosImage, 2);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("IncImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, 3);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("DecImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, 4);
				}
				else if( a_stricmp(pResouceChildItem->GetName(), _AL("PosBgImage")) == 0 )
				{
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), m_strPosBgImage)
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

	if (!m_strPosBgImage.IsEmpty() && !m_strPosImage.IsEmpty() && NULL != cs_sdrfile)
	{
		m_pShaderAutoPos = new A3DShader;
		m_pShaderAutoPos->Init(m_pA3DEngine);
		if (!m_pShaderAutoPos->Load(cs_sdrfile))
		{
			if(AUI_IsLogEnabled())
			{
				a_LogOutput(1, "AUIProgress::InitXML, Failed to load %s", cs_sdrfile);
			}
			return true;
		}

		AString strPosBgImage = AString("Surfaces\\") + m_strPosBgImage;
		AString strPosImage = AString("Surfaces\\") + m_strPosImage;
		if (!m_pShaderAutoPos->ChangeStageTexture(eStagePosBg, A3DSDTEX_NORMAL, (DWORD)((const char*)(strPosBgImage))) ||
			!m_pShaderAutoPos->ChangeStageTexture(eStagePos, A3DSDTEX_NORMAL, (DWORD)((const char*)(strPosImage))) ||
			!m_pShaderAutoPos->ChangeStageTexture(eStagePos2, A3DSDTEX_NORMAL, (DWORD)((const char*)(strPosImage))))
		{
			if(AUI_IsLogEnabled())
			{
				a_LogOutput(1, "AUIProgress::InitXML, Failed to change texture %s or %s", m_strPosBgImage, m_strPosImage);
			}
			return true;
		}

		m_pStreamAutoPos = new A3DStream;
		if (!m_pStreamAutoPos->Init(m_pA3DDevice, sizeof(CustTlVtx), CUSTVTX_FMT, 4, 6, A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
			return false;
		WORD index[] = {0, 1, 3, 3, 1, 2};
		m_pStreamAutoPos->SetIndices(index, 6);
		m_bCanAutoFixPosImage = true;
	}

	return true;
}

bool AUIProgress::InitIndirect(const char *pszFileName, int idType)
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
			return AUI_ReportError(__LINE__, 1, "AUIProgress::InitIndirect(), failed to m_pAUIFrame->Init()");
		}

		if( m_pParent && m_pParent->GetAUIManager() )
			m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	}
	else if( 1 == idType )
	{
		A3DRELEASE(m_pA2DSpriteBar);
		if( strlen(pszFileName) <= 0 ) return true;
		
		m_pA2DSpriteBar = new A2DSprite;
		if( !m_pA2DSpriteBar ) return false;

#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSpriteBar->SetDynamicTex(true);
#endif //_ANGELICA22

		bval = m_pA2DSpriteBar->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSpriteBar)
			return AUI_ReportError(__LINE__, 1, "AUIProgress::InitIndirect(), failed to m_pA2DSpriteBar->Init()");
		}
	}
	else if( 2 == idType )
	{
		A3DRELEASE(m_pA2DSpritePos);
		if( strlen(pszFileName) <= 0 ) return true;

		if(strnicmp(pszFileName, "gfx", 3) == 0 || strnicmp(pszFileName, "gfx", 3) == 0)
		{
			if(pszFileName[3] == '\\' || pszFileName[3] == '/')
				pszFileName += 4;
		}
		if(strnicmp(pszFileName, "surfaces", 8) == 0 || strnicmp(pszFileName, "surfaces", 8) == 0)
		{
			if(pszFileName[8] == '\\' || pszFileName[8] == '/')
				pszFileName += 9;
		}
		
		if(af_CheckFileExt(pszFileName, ".gfx"))
		{ // gfx
			AUI_RELEASE_GFX(m_pPosGfx);
#ifdef _DX9
			m_pPosGfx = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, pszFileName, true, false);
#else
			m_pPosGfx = AfxGetGFXExMan()->LoadGfx(m_pA3DDevice, pszFileName);
#endif
			if( m_pPosGfx )
			{
				m_pPosGfx->Start();
				m_pPosGfx->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
				// Location is set in Render.
			}
		}
		else
		{ // image
			m_pA2DSpritePos = new A2DSprite;
			if( !m_pA2DSpritePos ) return false;

#ifdef _ANGELICA22
			//d3d9ex
			m_pA2DSpritePos->SetDynamicTex(true);
#endif //_ANGELICA22
			bval = m_pA2DSpritePos->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
			if( !bval )
			{
				A3DRELEASE(m_pA2DSpritePos)
				return AUI_ReportError(__LINE__, 1, "AUIProgress::InitIndirect(), failed to m_pA2DSpritePos->Init()");
			}

			m_pA2DSpritePos->SetLocalCenterPos(m_pA2DSpritePos->GetWidth() / 2, m_pA2DSpritePos->GetHeight() / 2);
		}
	}
	else if( 3 == idType )
	{
		A3DRELEASE(m_pA2DSpriteInc);
		if( strlen(pszFileName) <= 0 ) return true;
		
		m_pA2DSpriteInc = new A2DSprite;
		if( !m_pA2DSpriteInc ) return false;

#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSpriteInc->SetDynamicTex(true);
#endif //_ANGELICA22
		bval = m_pA2DSpriteInc->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSpriteInc)
			return AUI_ReportError(__LINE__, 1, "AUIProgress::InitIndirect(), failed to m_pA2DSpriteInc->Init()");
		}
	}
	else if( 4 == idType )
	{
		A3DRELEASE(m_pA2DSpriteDec);
		if( strlen(pszFileName) <= 0 ) return true;
		
		m_pA2DSpriteDec = new A2DSprite;
		if( !m_pA2DSpriteDec ) return false;

#ifdef _ANGELICA22
		//d3d9ex
		m_pA2DSpriteDec->SetDynamicTex(true);
#endif //_ANGELICA22
		bval = m_pA2DSpriteDec->Init(m_pA3DDevice, pszFileName, AUI_COLORKEY);
		if( !bval )
		{
			A3DRELEASE(m_pA2DSpriteDec)
			return AUI_ReportError(__LINE__, 1, "AUIProgress::InitIndirect(), failed to m_pA2DSpriteDec->Init()");
		}
	}

	return true;
}

bool AUIProgress::Release(void)
{
	A3DRELEASE(m_pAUIFrame);
	A3DRELEASE(m_pA2DSpriteBar);
	A3DRELEASE(m_pA2DSpritePos);
	A3DRELEASE(m_pA2DSpriteInc);
	A3DRELEASE(m_pA2DSpriteDec);
	A3DRELEASE(m_pStreamAutoPos);
	A3DRELEASE(m_pShaderAutoPos);
	AUI_RELEASE_GFX(m_pPosGfx);

	return AUIObject::Release();
}

bool AUIProgress::Save(FILE *file)
{
	fprintf(file, "PROGRESS");

	if( !AUIObject::Save(file) ) return false;

	fprintf(file, " \"%s\" \"%s\" %d %d %d %d %d\n",
		m_pAUIFrame ? m_pAUIFrame->GetA2DSprite()->GetName() : "",
		m_pA2DSpriteBar ? m_pA2DSpriteBar->GetName() : "",
		m_rcFillMargin.left, m_rcFillMargin.top,
		m_rcFillMargin.right, m_rcFillMargin.bottom,
		m_nFrameMode);

	return true;
}

bool AUIProgress::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemProgress = new AXMLItem;
	pXMLItemProgress->InsertItem(pXMLItemDialog);
	pXMLItemProgress->SetName(_AL("PROGRESS"));
	XMLSET_STRING_VALUE(pXMLItemProgress, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("Height"), m_nHeight, 0)
	XMLSET_BOOL_VALUE(pXMLItemProgress, _AL("VerticalProgress"), m_bVProgress, false)
	XMLSET_BOOL_VALUE(pXMLItemProgress, _AL("ReverseDirection"), m_bReverseDir, false)
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("FillMarginLeft"), m_rcFillMargin.left, 0)
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("FillMarginTop"), m_rcFillMargin.top, 0)
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("FillMarginRight"), m_rcFillMargin.right, 0)
	XMLSET_INT_VALUE(pXMLItemProgress, _AL("FillMarginBottom"), m_rcFillMargin.bottom, 0)
	XMLSET_BOOL_VALUE(pXMLItemProgress, _AL("ForceDynamicRender"), m_bForceDynamicRender, false)
	XMLSET_STRING_VALUE(pXMLItemProgress, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemProgress);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	AXMLItem *pXMLItemResource = new AXMLItem;
	pXMLItemResource->InsertItem(pXMLItemProgress);
	pXMLItemResource->SetName(_AL("Resource"));
	
	if( m_pAUIFrame )
	{
		AXMLItem *pXMLItemFrameImage = new AXMLItem;
		pXMLItemFrameImage->InsertItem(pXMLItemResource);
		pXMLItemFrameImage->SetName(_AL("FrameImage"));
		XMLSET_STRING_VALUE(pXMLItemFrameImage, _AL("FileName"), AS2AC(m_pAUIFrame->GetA2DSprite()->GetName()), _AL(""))
		XMLSET_INT_VALUE(pXMLItemFrameImage, _AL("FrameMode"), m_nFrameMode, 0)
	}

	if( m_pA2DSpriteBar )
	{
		AXMLItem *pXMLItemFillImage = new AXMLItem;
		pXMLItemFillImage->InsertItem(pXMLItemResource);
		pXMLItemFillImage->SetName(_AL("FillImage"));
		XMLSET_STRING_VALUE(pXMLItemFillImage, _AL("FileName"), AS2AC(m_pA2DSpriteBar->GetName()), _AL(""))
	}

	if( m_pA2DSpritePos )
	{
		AXMLItem *pXMLItemPosImage = new AXMLItem;
		pXMLItemPosImage->InsertItem(pXMLItemResource);
		pXMLItemPosImage->SetName(_AL("PosImage"));
		XMLSET_STRING_VALUE(pXMLItemPosImage, _AL("FileName"), AS2AC(m_pA2DSpritePos->GetName()), _AL(""))
	}

	if( m_pA2DSpriteInc )
	{
		AXMLItem *pXMLItemIncImage = new AXMLItem;
		pXMLItemIncImage->InsertItem(pXMLItemResource);
		pXMLItemIncImage->SetName(_AL("IncImage"));
		XMLSET_STRING_VALUE(pXMLItemIncImage, _AL("FileName"), AS2AC(m_pA2DSpriteInc->GetName()), _AL(""))
	}

	if( m_pA2DSpriteDec )
	{
		AXMLItem *pXMLItemDecImage = new AXMLItem;
		pXMLItemDecImage->InsertItem(pXMLItemResource);
		pXMLItemDecImage->SetName(_AL("DecImage"));
		XMLSET_STRING_VALUE(pXMLItemDecImage, _AL("FileName"), AS2AC(m_pA2DSpriteDec->GetName()), _AL(""))
	}

	if ( !m_strPosBgImage.IsEmpty() )
	{
		AXMLItem *pXMLItemPosBgImage = new AXMLItem;
		pXMLItemPosBgImage->InsertItem(pXMLItemResource);
		pXMLItemPosBgImage->SetName(_AL("PosBgImage"));
		XMLSET_STRING_VALUE(pXMLItemPosBgImage, _AL("FileName"), AS2AC(m_strPosBgImage), _AL(""))
	}

	return true;
}

bool AUIProgress::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIProgress::Tick(void)
{
	if(m_pPosGfx)
		m_pPosGfx->TickAnimation(m_pParent->GetAUIManager()->GetTickTime());

	if( m_bAutoProgress )
	{
		DWORD dwTime = GetTickCount();
		if( dwTime < m_dwTimeEnd )
		{
			m_nProgress = m_nPosAutoStart + (m_nPosAutoEnd - m_nPosAutoStart)
				* (int)(dwTime - m_dwTimeStart) / (int)(m_dwTimeEnd - m_dwTimeStart);
		}
		else
		{
			m_nProgress = m_nPosAutoEnd;
			m_bAutoProgress = false;
		}

		UpdateRenderTarget();
	}

	if( m_bActionProgress )
	{
		DWORD dwTime = GetTickCount();
		if( dwTime < m_dwTimeEnd )
		{
			m_nPosActionTemp = m_nPosActionStart + (m_nProgress - m_nPosActionStart)
				* int(dwTime - m_dwTimeStart) / int(m_dwTimeEnd - m_dwTimeStart);
		}
		else
		{
			m_nPosActionTemp = m_nProgress;
			m_bActionProgress = false;
		}
		UpdateRenderTarget();
	}

	return AUIObject::Tick();
}

bool AUIProgress::RenderProgress(A2DSprite* pA2DSprite, int nStart, int nEnd)
{
	if( !pA2DSprite || nStart == nEnd )
		return true;

	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	pA2DSprite->SetColor(m_color);

	bool bval = true;
	POINT ptPos = GetPos();
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	int H = m_nHeight - m_rcFillMargin.top - m_rcFillMargin.bottom;
	int W = m_nWidth - m_rcFillMargin.left - m_rcFillMargin.right;
	if( m_bVProgress )
	{
		int nHeight = (int)(H * ((float)(nEnd - nStart) / m_nRangeMax)); 
		int nHeightStart;
		if( m_nFrameMode == 1 )
		{
			A3DRECT rect;
			rect.SetRect(0, 0, pA2DSprite->GetWidth(), pA2DSprite->GetHeight());
			pA2DSprite->ResetItems(1, &rect);
			
			float fScaleX = float(W) / float(pA2DSprite->GetWidth());
			float fScaleY = float(nHeight) / float(pA2DSprite->GetHeight());
			pA2DSprite->SetScaleX(fScaleX);
			pA2DSprite->SetScaleY(fScaleY);
			if( m_bReverseDir )
			{
				pA2DSprite->SetLocalCenterPos(0, 0);
				nHeightStart = (int)(H * ((float)(m_nRangeMax - nStart) / m_nRangeMax)); 
			}
			else
			{
				pA2DSprite->SetLocalCenterPos(0, pA2DSprite->GetHeight());
				nHeightStart = (int)(H * ((float)nStart / m_nRangeMax));
			}
		}
		else
		{
			A3DRECT rect;
			if( m_bReverseDir )
			{
				rect.SetRect(0, (int)(pA2DSprite->GetHeight() * ((float)nStart / m_nRangeMax)), 
					pA2DSprite->GetWidth(), (int)(pA2DSprite->GetHeight() * ((float)nEnd / m_nRangeMax)));
				if( pA2DSprite == m_pA2DSpriteInc )
				{
					pA2DSprite->SetLocalCenterPos(0, rect.Height());
					nHeightStart = (int)(H * ((float)nEnd / m_nRangeMax));
				}
				else
				{
					pA2DSprite->SetLocalCenterPos(0, 0);
					nHeightStart = (int)(H * ((float)nStart / m_nRangeMax));
				}
			}
			else
			{
				rect.SetRect(0, (int)(pA2DSprite->GetHeight() * ((float)(m_nRangeMax - nEnd) / m_nRangeMax)), 
					pA2DSprite->GetWidth(), (int)(pA2DSprite->GetHeight() * ((float)(m_nRangeMax - nStart) / m_nRangeMax)));
				if( pA2DSprite == m_pA2DSpriteInc )
				{
					pA2DSprite->SetLocalCenterPos(0, 0);
					nHeightStart = (int)(H * ((float)(m_nRangeMax - nEnd) / m_nRangeMax));
				}
				else
				{
					pA2DSprite->SetLocalCenterPos(0, rect.Height());
					nHeightStart = (int)(H * ((float)(m_nRangeMax - nStart) / m_nRangeMax));
				}
			}

			pA2DSprite->ResetItems(1, &rect);

			float fScaleX = float(W) / float(pA2DSprite->GetWidth());
			float fScaleY = float(H) / float(pA2DSprite->GetHeight());
			pA2DSprite->SetScaleX(fScaleX);
			pA2DSprite->SetScaleY(fScaleY);
		}

		int x = ptPos.x + m_rcFillMargin.left;
		int y = ptPos.y + m_rcFillMargin.top;
		pA2DSprite->SetAlpha(nAlpha);
		if( m_pParent->IsNoFlush() )
			bval = pA2DSprite->DrawToBack(x, y + nHeightStart);
		else
			bval = pA2DSprite->DrawToInternalBuffer(x, y + nHeightStart);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress::RenderProgress(), failed to call pA2DSprite->DrawToBack() or pA2DSprite->DrawToInternalBuffer()");
	}
	else
	{
		int nWidth = (int)(W * ((float)(nEnd - nStart) / m_nRangeMax)); 
		int nWidthStart;
		if( m_nFrameMode == 1 )
		{
			A3DRECT rect;
			rect.SetRect(0, 0, pA2DSprite->GetWidth(), pA2DSprite->GetHeight());
			pA2DSprite->ResetItems(1, &rect);

			float fScaleX = float(nWidth) / float(pA2DSprite->GetWidth());
			float fScaleY = float(H) / float(pA2DSprite->GetHeight());
			pA2DSprite->SetScaleX(fScaleX);
			pA2DSprite->SetScaleY(fScaleY);
			if( m_bReverseDir )
			{
				pA2DSprite->SetLocalCenterPos(pA2DSprite->GetWidth(), 0);
				nWidthStart = (int)(W * ((float)(m_nRangeMax - nStart) / m_nRangeMax));
			}
			else
			{
				pA2DSprite->SetLocalCenterPos(0, 0);
				nWidthStart = (int)(W * ((float)nStart / m_nRangeMax));
			}
		}
		else
		{
			A3DRECT rect;
			if( m_bReverseDir )
			{
				rect.SetRect((int)(pA2DSprite->GetWidth() * ((float)(m_nRangeMax - nEnd) / m_nRangeMax)), 0, 
					(int)(pA2DSprite->GetWidth() * ((float)(m_nRangeMax - nStart) / m_nRangeMax)), pA2DSprite->GetHeight());
				if( pA2DSprite == m_pA2DSpriteInc )
				{
					pA2DSprite->SetLocalCenterPos(0, 0);
					nWidthStart = (int)(W * ((float)(m_nRangeMax - nEnd) / m_nRangeMax));
				}
				else
				{
					pA2DSprite->SetLocalCenterPos(rect.Width(), 0);
					nWidthStart = (int)(W * ((float)(m_nRangeMax - nStart) / m_nRangeMax));
				}
			}
			else
			{
				rect.SetRect((int)(pA2DSprite->GetWidth() * ((float)nStart / m_nRangeMax)), 0,
					(int)(pA2DSprite->GetWidth() * ((float)nEnd / m_nRangeMax)), pA2DSprite->GetHeight());
				if( pA2DSprite == m_pA2DSpriteInc )
				{
					pA2DSprite->SetLocalCenterPos(rect.Width(), 0);
					nWidthStart = (int)(W * ((float)nEnd / m_nRangeMax));
				}
				else
				{
					pA2DSprite->SetLocalCenterPos(0, 0);
					nWidthStart = (int)(W * ((float)nStart / m_nRangeMax));
				}
			}
			pA2DSprite->ResetItems(1, &rect);

			float fScaleX = float(W) / float(pA2DSprite->GetWidth());
			float fScaleY = float(H) / float(pA2DSprite->GetHeight());
			pA2DSprite->SetScaleX(fScaleX);
			pA2DSprite->SetScaleY(fScaleY);
		}

		int x = ptPos.x + m_rcFillMargin.left;
		int y = ptPos.y + m_rcFillMargin.top;
		pA2DSprite->SetAlpha(nAlpha);
		if( m_pParent->IsNoFlush() )
			bval = pA2DSprite->DrawToBack(x + nWidthStart, y);
		else
			bval = pA2DSprite->DrawToInternalBuffer(x + nWidthStart, y);
		if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress::RenderProgress(), failed to call pA2DSprite->DrawToBack() or pA2DSprite->DrawToInternalBuffer()");
	}

	return true;
}

bool AUIProgress::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_NORMAL ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC && !NeedDynamicRender() ||
		m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_DYNAMIC && NeedDynamicRender() )
	{
		int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

		bool bval = true;
		POINT ptPos = GetPos();

		ptPos.x += m_nOffX;
		ptPos.y += m_nOffY;

		int H = m_nHeight - m_rcFillMargin.top - m_rcFillMargin.bottom;
		int W = m_nWidth - m_rcFillMargin.left - m_rcFillMargin.right;

		if( m_pAUIFrame )
		{
			m_pAUIFrame->GetA2DSprite()->SetColor(m_color);
			bval = m_pAUIFrame->Render(ptPos.x, ptPos.y, m_nWidth, m_nHeight, NULL, 0,
				_AL(""), AUIFRAME_ALIGN_LEFT, 0, 0, nAlpha, m_pParent->IsNoFlush(), m_nShadow);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress.Render(), failed to call m_pAUIFrame->Render() or base AUIObject::Render()");
		}

		if( m_bActionProgress && m_nPosActionTemp != m_nProgress )
		{
			if( m_nPosActionTemp < m_nProgress )
			{
				bval = RenderProgress(m_pA2DSpriteBar, 0, m_nPosActionTemp);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress::Render(), failed to render m_pA2DSpriteBar");

				bval = RenderProgress(m_pA2DSpriteInc, m_nPosActionTemp, m_nProgress);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress::Render(), failed to render m_pA2DSpriteInc");
			}
			else if( m_nPosActionTemp > m_nProgress )
			{
				bval = RenderProgress(m_pA2DSpriteBar, 0, m_nProgress);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress::Render(), failed to render m_pA2DSpriteBar");

				bval = RenderProgress(m_pA2DSpriteDec, m_nProgress, m_nPosActionTemp);
				if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress::Render(), failed to render m_pA2DSpriteDec");
			}
		}
		else
		{
			bval = RenderProgress(m_pA2DSpriteBar, 0, m_nProgress);
			if( !bval ) return AUI_ReportError(__LINE__, 1, "AUIProgress::Render(), failed to render m_pA2DSpriteBar");
		}

		RenderPosImage();
	}

	return true;
}

bool AUIProgress::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Frame Mode") )
		Property->i = m_nFrameMode;
	else if( 0 == strcmpi(pszPropertyName, "Frame Image File") )
		strncpy(Property->fn, m_pAUIFrame ? m_pAUIFrame->GetA2DSprite()->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Fill Image File") )
		strncpy(Property->fn, m_pA2DSpriteBar ? m_pA2DSpriteBar->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Pos Image File") )
		strncpy(Property->fn, m_pA2DSpritePos ? m_pA2DSpritePos->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Inc Image File") )
		strncpy(Property->fn, m_pA2DSpriteInc ? m_pA2DSpriteInc->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Dec Image File") )
		strncpy(Property->fn, m_pA2DSpriteDec ? m_pA2DSpriteDec->GetName() : "", AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Vertical Progress") )
		Property->b = m_bVProgress;
	else if( 0 == strcmpi(pszPropertyName, "Reverse Direction") )
		Property->b = m_bReverseDir;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Left") )
		Property->i = m_rcFillMargin.left;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Top") )
		Property->i = m_rcFillMargin.top;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Right") )
		Property->i = m_rcFillMargin.right;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Bottom") )
		Property->i = m_rcFillMargin.bottom;
	else if( 0 == strcmpi(pszPropertyName, "Pos Bg Image File") )
		strncpy(Property->fn, m_strPosBgImage, AFILE_LINEMAXLEN - 1);
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIProgress::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
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
	else if( 0 == strcmpi(pszPropertyName, "Frame Mode") )
	{
		m_nFrameMode = Property->i;
		if( m_pAUIFrame )
			m_pAUIFrame->SetMode(m_nFrameMode);
	}
	else if( 0 == strcmpi(pszPropertyName, "Frame Image File") )
		return InitIndirect(Property->fn, 0);
	else if( 0 == strcmpi(pszPropertyName, "Fill Image File") )
		return InitIndirect(Property->fn, 1);
	else if( 0 == strcmpi(pszPropertyName, "Pos Image File") )
		return InitIndirect(Property->fn, 2);
	else if( 0 == strcmpi(pszPropertyName, "Inc Image File") )
		return InitIndirect(Property->fn, 3);
	else if( 0 == strcmpi(pszPropertyName, "Dec Image File") )
		return InitIndirect(Property->fn, 4);
	else if( 0 == strcmpi(pszPropertyName, "Vertical Progress") )
		m_bVProgress = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Reverse Direction") )
		m_bReverseDir = Property->b;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Left") )
		m_rcFillMargin.left = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Top") )
		m_rcFillMargin.top = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Right") )
		m_rcFillMargin.right = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Fill Margin Bottom") )
		m_rcFillMargin.bottom = Property->i;
	else if( 0 == strcmpi(pszPropertyName, "Pos Bg Image File") )
		m_strPosBgImage = Property->fn;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIProgress::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Frame Mode", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Frame Image File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Fill Image File", AUIOBJECT_PROPERTY_VALUE_CHAR) &&
			SetAUIObjectPropertyType("Fill Margin Left", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Fill Margin Top", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Fill Margin Right", AUIOBJECT_PROPERTY_VALUE_INT) &&
			SetAUIObjectPropertyType("Fill Margin Bottom", AUIOBJECT_PROPERTY_VALUE_INT);
}

int AUIProgress::GetProgress(void)
{
	return m_nProgress;
}

void AUIProgress::SetProgress(int nProgress)
{
	a_Clamp(nProgress, 0, m_nRangeMax);
	if( m_nProgress != nProgress )
		UpdateRenderTarget();

	m_nProgress = nProgress;
	m_bActionProgress = false;
}

void AUIProgress::SetActionProgress(DWORD dwMSecs, int nProgress)
{
	a_Clamp(nProgress, 0, m_nRangeMax);
	if( dwMSecs > 0 && nProgress != m_nProgress )
	{
		if( m_bActionProgress )
			m_nPosActionStart = m_nPosActionTemp;
		else
			m_nPosActionStart = m_nProgress;
		m_bActionProgress = true;
		m_nPosActionTemp = m_nProgress;
		m_dwTimeStart = GetTickCount();
		m_dwTimeEnd = m_dwTimeStart + dwMSecs;
	}

	m_nProgress = nProgress;
	UpdateRenderTarget();
}

void AUIProgress::SetAutoProgress(DWORD dwMSecs, int nPosStart, int nPosEnd)
{
	a_Clamp(nPosStart, 0, m_nRangeMax);
	a_Clamp(nPosEnd, 0, m_nRangeMax);
	if( dwMSecs > 0 && nPosStart != nPosEnd )
	{
		m_bAutoProgress = true;
		m_nPosAutoStart = nPosStart;
		m_nPosAutoEnd = nPosEnd;
		m_nProgress = nPosStart;
		m_dwTimeStart = GetTickCount();
		m_dwTimeEnd = m_dwTimeStart + dwMSecs;
	}
	else
	{
		m_nProgress = 0;
		m_bAutoProgress = false;
	}
	UpdateRenderTarget();
}

bool AUIProgress::Resize()
{
	if( !AUIObject::Resize() )
		return false;

	if( m_pPosGfx )
		m_pPosGfx->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());


	if( m_pAUIFrame )
		m_pAUIFrame->SetScale(m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale());
	return true;
}

void AUIProgress::Show(bool bShow)
{
	if (m_pParent && m_pParent->IsOptimizeResourceLoad())
	{
		OptimizeSpriteResource(bShow);
	}

	AUIObject::Show(bShow);
}

void AUIProgress::OptimizeSpriteResource(bool bLoad)
{
	if (m_nOptimizeResourceState == 0 && !bLoad || m_nOptimizeResourceState == 1 && bLoad)
		return;

	m_nOptimizeResourceState = (bLoad ? 1 : 0);

	if (m_pA2DSpriteBar)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteBar, bLoad);

	if (m_pA2DSpritePos)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpritePos, bLoad);

	if (m_pA2DSpriteInc)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteInc, bLoad);

	if (m_pA2DSpriteDec)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pA2DSpriteDec, bLoad);

	if (m_pAUIFrame)
		glb_OptimizeSpriteResource(m_pA3DEngine, m_pAUIFrame->GetA2DSprite(), bLoad);
}

void AUIProgress::SetRangeMax(int nRangeMax)
{
	assert(nRangeMax>0);
	if (nRangeMax<=0) 
		return;
	m_nRangeMax = nRangeMax;
}

int AUIProgress::GetRangeMax() const
{
	return m_nRangeMax;
}

bool AUIProgress::RenderPosImage()
{
	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);
	
	bool bval = true;
	POINT ptPos = GetPos();
	
	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;
	
	int H = m_nHeight - m_rcFillMargin.top - m_rcFillMargin.bottom;
	int W = m_nWidth - m_rcFillMargin.left - m_rcFillMargin.right;
	if (!m_bCanAutoFixPosImage || !m_pA2DSpriteBar)
	{
		if( m_bVProgress )
		{
			int nHeight = (int)(H * ((float)m_nProgress / m_nRangeMax)); 
			int x = ptPos.x + m_rcFillMargin.left;
			int y = ptPos.y + m_rcFillMargin.top;
			int xx = x + W / 2;
			int yy = y + (m_bReverseDir ? nHeight : H - nHeight);

			if( m_pA2DSpritePos )
			{
				m_pA2DSpritePos->SetColor(m_color);
				m_pA2DSpritePos->SetAlpha(nAlpha);
				m_pA2DSpritePos->SetScaleX(fWindowScale);
				m_pA2DSpritePos->SetScaleY(fWindowScale);
				if( m_pParent->IsNoFlush() )
					bval = m_pA2DSpritePos->DrawToBack(xx, yy);
				else
					bval = m_pA2DSpritePos->DrawToInternalBuffer(xx, yy);
			}
			if( m_pPosGfx )
			{
				m_pPosGfx->SetParentTM(a3d_Translate(xx, yy, 0));
				m_pPosGfx->SetAlpha(float(nAlpha) / 255.0f);
				AfxGetGFXExMan()->RegisterGfx(m_pPosGfx);
				AfxGetGFXExMan()->Render2DGfx(false);
			}
		}
		else
		{
			int nWidth = (int)(W * ((float)m_nProgress / m_nRangeMax)); 
			int x = ptPos.x + m_rcFillMargin.left;
			int y = ptPos.y + m_rcFillMargin.top;
			int xx = x + (m_bReverseDir ? W - nWidth : nWidth);
			int yy = y + H / 2;
			
			if( m_pA2DSpritePos )
			{
				m_pA2DSpritePos->SetColor(m_color);
				m_pA2DSpritePos->SetAlpha(nAlpha);
				m_pA2DSpritePos->SetScaleX(fWindowScale);
				m_pA2DSpritePos->SetScaleY(fWindowScale);
				if( m_pParent->IsNoFlush() )
					bval = m_pA2DSpritePos->DrawToBack(xx, yy);
				else
					bval = m_pA2DSpritePos->DrawToInternalBuffer(xx, yy);
			}
			if( m_pPosGfx )
			{
				m_pPosGfx->SetParentTM(a3d_Translate(xx, yy, 0));
				m_pPosGfx->SetAlpha(float(nAlpha) / 255.0f);
				AfxGetGFXExMan()->RegisterGfx(m_pPosGfx);
				AfxGetGFXExMan()->Render2DGfx(false);
			}
		}
	}
	else if (NULL != m_pShaderAutoPos && NULL != m_pStreamAutoPos)
	{
		int iPosW = 0, iPosH = 0;
		int iPosBgW = 0, iPosBgH = 0;
		APtrArray<void*>& vTexPosBG = m_pShaderAutoPos->GetStage(eStagePosBg).aTextures;
		APtrArray<void*>& vTexPos = m_pShaderAutoPos->GetStage(eStagePos).aTextures;
		if (!(vTexPosBG.GetSize() > 0 && ((A3DTexture*)(vTexPosBG[0]))->GetDimension(&iPosBgW, &iPosBgH)))
			return false;
		if (!(vTexPos.GetSize() > 0 && ((A3DTexture*)(vTexPos[0]))->GetDimension(&iPosW, &iPosH)))
			return false;
		
		CustTlVtx verts[4];
		float	l, t, r, b;
		float fRatio = m_bReverseDir ? (1.0f-float(m_nProgress) / m_nRangeMax):(float(m_nProgress) / m_nRangeMax);
		
		if (!m_bVProgress)
		{
			float fBgScale = H / m_pA2DSpriteBar->GetHeight();
			int yOff = (iPosBgH-m_pA2DSpriteBar->GetHeight())*fBgScale/2;
			t = ptPos.y - yOff;
			b = ptPos.y + H + yOff;

			int xOff = (iPosW * (b - t) / iPosH) / 2;
			l = ptPos.x + W * fRatio - xOff;			
			r = l + xOff * 2;
			
			float fDeltaU = (float)(r-l) / m_nWidth;
			float fLU = fRatio - fDeltaU / 2;
			float fRU = fRatio + fDeltaU / 2;
			a_Clamp(fLU, 0.0f, 1.0f);
			a_Clamp(fRU, 0.0f, 1.0f);
			verts[0].Set(A3DVECTOR4(l, t, 0, 1),	0xffffffff,	fLU,	0.0f,	0.0f,	0.0f);
			verts[1].Set(A3DVECTOR4(r, t, 0, 1),	0xffffffff,	fRU,	0.0f,	1.0f,	0.0f);
			verts[2].Set(A3DVECTOR4(r, b, 0, 1),	0xffffffff,	fRU,	1.0f,	1.0f,	1.0f);
			verts[3].Set(A3DVECTOR4(l, b, 0, 1),	0xffffffff,	fLU,	1.0f,	0.0f,	1.0f);
		}
		else
		{
			float fBgScale = W / m_pA2DSpriteBar->GetWidth();
			int xOff = (iPosBgW-m_pA2DSpriteBar->GetWidth())*fBgScale/2;
			l = ptPos.x - xOff;
			r = ptPos.x + W + xOff;
			
			int yOff = (iPosH * (r - l) / iPosW) / 2;
			t = ptPos.y + H * fRatio - yOff;			
			b = t + yOff * 2;
			
			float fDeltaV = (float)(b-t) / m_nHeight;
			float fUV = fRatio - fDeltaV / 2;
			float fDV = fRatio + fDeltaV / 2;
			a_Clamp(fUV, 0.0f, 1.0f);
			a_Clamp(fDV, 0.0f, 1.0f);
			verts[0].Set(A3DVECTOR4(l, t, 0, 1),	0xffffffff,	0.0f,	fUV,	0.0f,	0.0f);
			verts[1].Set(A3DVECTOR4(r, t, 0, 1),	0xffffffff,	1.0f,	fUV,	1.0f,	0.0f);
			verts[2].Set(A3DVECTOR4(r, b, 0, 1),	0xffffffff,	1.0f,	fDV,	1.0f,	1.0f);
			verts[3].Set(A3DVECTOR4(l, b, 0, 1),	0xffffffff,	0.0f,	fDV,	0.0f,	1.0f);
		}
		
		m_pStreamAutoPos->SetVerts((LPBYTE) verts, 4);
		
		m_pShaderAutoPos->Appear();
		m_pStreamAutoPos->Appear();
		
		m_pA3DDevice->SetTextureCoordIndex(0, eStagePosBg);
		m_pA3DDevice->SetTextureCoordIndex(1, eStagePos);
		m_pA3DDevice->SetTextureCoordIndex(2, eStagePos);
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 4, 0, 2);
		m_pA3DDevice->SetTextureCoordIndex(0, 0);
		m_pA3DDevice->SetTextureCoordIndex(1, 1);
		m_pA3DDevice->SetTextureCoordIndex(2, 2);
		
		m_pShaderAutoPos->Disappear();
	}
	return true;
}