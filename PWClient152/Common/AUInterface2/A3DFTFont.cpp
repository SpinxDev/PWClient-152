#include "AUI.h"

#include "A3DFTFont.h"
#include "A3DFTFontMan.h"
#include "FTInterface.h"
#include "vector.h"
#include "AMemory.h"
#include "A3DMacros.h"
#include "A3DDevice.h"
#include "A3DFuncs.h"
#include "A3DPixelShader.h"
#include "AUICommon.h"
#include "shlwapi.h"
#include <A3DVertexShader.h>
#include <A3DEngine.h>
#include <A3DViewport.h>
#include <A3DFlatCollector.h>

static const int _slot_buf_size			= 256;
static const int _slot_shadow_buf_size	= 64;
static const int _font_tex_buf_width	= 128;
static const int _font_tex_buf_height	= 128;
static const float _shadow_offset		= 1.0f;
static const DWORD _char_tex_ttl		= 100;
static const int _spacing				= 1;

static int _a3dftfont_count = 0;
static int _a3dftfont_tex_count = 0;
static int _a3dftfont_char_draw_count = 0;

int A3DFTFont_GetFontCount()
{
	return _a3dftfont_count;
}

int A3DFTFont_GetFontTexCount()
{
	return _a3dftfont_tex_count;
}

int A3DFTFont_GetCharDrawCount()
{
	return _a3dftfont_char_draw_count;
}

void A3DFTFont_ResetCharDrawCount()
{
	_a3dftfont_char_draw_count = 0;
}

extern wchar_t _widest_char;

A3DFTFont::A3DFTFont(A3DFTFontMan* pFontMan) :
m_CharMap(1024),
m_pFTFont(0),
m_pFontMan(pFontMan),
m_iFontWid(0),
m_iFontHei(0),
m_nCreateMode(0),
m_bStereo(false),
m_nRefCnt(0),
m_bLoaded(true),
m_bInLoadThread(false),
m_dwCurNewCharCount(0)
{
	m_pA3DEngine = NULL;
	::InitializeCriticalSection(&m_cs);
}

bool A3DFTFont::Create(
	int nPointSize,
	const char* szFontPath,
	int nStyle,
	int nOutlineWidth,
	A3DCOLOR clOutline,
	A3DCOLOR clInnerText,
	bool bInterpColor,
	A3DCOLOR clUpper,
	A3DCOLOR clLower,
	int nCreateMode,
	const wchar_t* strFaceName,
	bool bStereo,
	A3DEngine* pA3DEngine)
{
	m_pA3DEngine = pA3DEngine;

	if (!::PathFileExistsA(szFontPath))
	{
		AUI_ReportError(__LINE__, szFontPath);
		return false;
	}

	const A3DFTFONT_DATA_BUFFER* pDataBuffer = m_pFontMan->GetFontDataBuffer(strFaceName);

	m_pFTFont = m_pFontMan->m_pFTFontMan->CreateFont(
		pDataBuffer ? pDataBuffer->buffer : szFontPath,
		pDataBuffer ? pDataBuffer->length : 0,
		nPointSize,
		nStyle,
		nOutlineWidth,
		nCreateMode,
		strFaceName);

	if (!m_pFTFont)
	{
		AUI_ReportError(__LINE__, __FILE__);
		return false;
	}

	m_pFTFont->GetMaxSize(m_iFontWid, m_iFontHei);
	FTDSIZE offset, size, bmp_size;

	if (m_pFTFont->GetCharExtent(_widest_char, &offset, &size, &bmp_size))
	{
		if (bmp_size.cx > m_iFontWid)
			m_iFontWid = bmp_size.cx;

		if (bmp_size.cy > m_iFontHei)
			m_iFontHei = bmp_size.cy;
	}

	// set big enough
//	m_iFontWid += 2;
//	m_iFontHei += 2;

	_a3dftfont_count++;

	if (AllocTextureSlot() == NULL)
	{
		Release();
		return false;
	}

	m_iPointSize = nPointSize;
	strcpy(m_szFontPath, szFontPath);
	m_iStyle = nStyle;

	m_iOutline = nOutlineWidth;
	DWORD r = A3DCOLOR_GETRED(clOutline);
	DWORD g = A3DCOLOR_GETGREEN(clOutline);
	DWORD b = A3DCOLOR_GETBLUE(clOutline);
	m_clOutline = ((r << 16) | (g << 8) | b);
	r = A3DCOLOR_GETRED(clInnerText);
	g = A3DCOLOR_GETGREEN(clInnerText);
	b = A3DCOLOR_GETBLUE(clInnerText);
	m_clInner = ((r << 16) | (g << 8) | b);

	m_bInterpColor = bInterpColor;
	m_clUpper[0] = A3DCOLOR_GETRED(clUpper);
	m_clUpper[1] = A3DCOLOR_GETGREEN(clUpper);
	m_clUpper[2] = A3DCOLOR_GETBLUE(clUpper);
	m_clLower[0] = A3DCOLOR_GETRED(clLower);
	m_clLower[1] = A3DCOLOR_GETGREEN(clLower);
	m_clLower[2] = A3DCOLOR_GETBLUE(clLower);

	m_clOutlineReserve = clOutline;
	m_clInnerReserve = clInnerText;
	m_clUpperReserve = clUpper;
	m_clLowerReserve = clLower;
	a3d_RGBToHSV(m_clUpperReserve, &m_clHSVUpperReserve);
	a3d_RGBToHSV(m_clLowerReserve, &m_clHSVLowerReserve);

	m_nCreateMode = nCreateMode;
	m_strFaceName = strFaceName;
	m_bStereo = bStereo;

	m_nRefCnt = 1;
	m_bLoaded = true;
	return true;
}

void A3DFTFont::SetParam(	
	int nPointSize,
	const char* szFontPath,
	int nStyle,
	int nOutlineWidth,
	A3DCOLOR clOutline,
	A3DCOLOR clInnerText,
	bool bInterpColor,
	A3DCOLOR clUpper,
	A3DCOLOR clLower,
	int nCreateMode,
	const wchar_t* strFaceName,
	bool bStereo)
{
	strcpy(m_szFontPath, szFontPath);
	m_iPointSize		= nPointSize;
	m_iStyle			= nStyle;
	m_iOutline			= nOutlineWidth;
	m_clOutlineReserve	= clOutline;
	m_clInnerReserve	= clInnerText;
	m_bInterpColor		= bInterpColor;
	m_clUpperReserve	= clUpper;
	m_clLowerReserve	= clLower;
	m_nCreateMode		= nCreateMode;
	m_strFaceName		= strFaceName;
	m_bStereo			= bStereo;
	m_nRefCnt			= 1;
	m_bLoaded			= false;
}

bool A3DFTFont::Resize(float fPointSize)
{
	if (!m_bLoaded)
		return true;

	bool bRet;
	int nPointSize = int(fPointSize) * 10;

	Lock();

	if (m_iPointSize == nPointSize)
	{
		Unlock();
		return true;
	}

	Release();

	bRet = Create(nPointSize, m_szFontPath, m_iStyle, m_iOutline, m_clOutlineReserve, m_clInnerReserve, m_bInterpColor, m_clUpperReserve, m_clLowerReserve, m_nCreateMode, m_strFaceName, m_bStereo,m_pA3DEngine);

	Unlock();
	return bRet;
}

bool A3DFTFont::Resize(float fPointSize, int nCreateMode)
{
	if (!m_bLoaded)
		return true;

	bool bRet;
	int nPointSize = int(fPointSize) * 10;

	Lock();

	if (m_iPointSize == nPointSize && m_nCreateMode == nCreateMode)
	{
		Unlock();
		return true;
	}

	Release();

	m_nCreateMode = nCreateMode;
	bRet = Create(nPointSize, m_szFontPath, m_iStyle, m_iOutline, m_clOutlineReserve, m_clInnerReserve, m_bInterpColor, m_clUpperReserve, m_clLowerReserve, m_nCreateMode, m_strFaceName, m_bStereo,m_pA3DEngine);

	Unlock();
	return bRet;
}

void A3DFTFont::Release()
{
	m_dwCurNewCharCount = 0;

	if (m_bInLoadThread)
	{
		assert(false);
		return;
	}

	if (m_pFTFont)
	{
		m_pFTFont->Release();
		m_pFTFont = 0;
		_a3dftfont_count--;
	}

	for (size_t i = 0; i < m_aSlotPool.size(); i++)
		FreeTextureSlot(m_aSlotPool[i]);

	m_aSlotPool.clear();
	ResetCharMap();

	m_nRefCnt = 0;
}

A3DFTFont::TEXTURESLOT* A3DFTFont::AllocTextureSlot()
{
	_a3dftfont_tex_count++;
	TEXTURESLOT* pSlot = new TEXTURESLOT;

	//	Create texture
	pSlot->pTexture = new A3DTexture;

	//	Create texture
#ifdef _ANGELICA22
	//d3d9ex
	if (!pSlot->pTexture->Create(
		m_pFontMan->m_pDevice,
		_font_tex_buf_width,
		_font_tex_buf_height,
		A3DFMT_A8R8G8B8,
		1,
		m_pA3DEngine->GetSupportD3D9ExFlag() ? D3DUSAGE_DYNAMIC : 0))
	{
		FreeTextureSlot(pSlot);
		return NULL;
	}
#else //Angelica2.0
	if (!pSlot->pTexture->Create(
		m_pFontMan->m_pDevice,
		_font_tex_buf_width,
		_font_tex_buf_height,
		A3DFMT_A8R8G8B8,
		1))

	{
		FreeTextureSlot(pSlot);
		return NULL;
	}
#endif //_ANGELICA22

	DWORD* pTexBuf;
	int iPitch;

	if (!pSlot->pTexture->LockRect(NULL, (void**)&pTexBuf, &iPitch, 0))
	{
		FreeTextureSlot(pSlot);
		return NULL;
	}

	memset(pTexBuf, 0, _font_tex_buf_height * iPitch);
	pSlot->pTexture->UnlockRect();

	int nWidth	= m_iFontWid + _spacing;
	int nHeight	= m_iFontHei + _spacing;
	int nMaxCol = _font_tex_buf_width / nWidth;
	int nMaxRow = _font_tex_buf_height / nHeight;

	if (nMaxRow < 1 || nMaxCol < 1)
	{
		FreeTextureSlot(pSlot);
		return NULL;
	}

	pSlot->VertBuf.reserve(_slot_buf_size);
	pSlot->ShadowBuf.reserve(_slot_shadow_buf_size);
	pSlot->CharInfo.reserve(nMaxRow * nMaxCol);
	pSlot->FlushCount = _char_tex_ttl + 1;

	for (int i = 0; i < nMaxRow; i++)
	{
		int _y = nHeight * i;

		for (int j = 0; j < nMaxCol; j++)
		{
			CHAR_TEX_INFO info;
			info.ch = 0;
			info.x = j * nWidth;
			info.y = _y;
			info.tl = 0;
			pSlot->CharInfo.push_back(info);
		}
	}

	m_aSlotPool.push_back(pSlot);
	return pSlot;
}

A3DFTFont::TEXTURESLOT* A3DFTFont::GetAvailableSlot(CHAR_TEX_INFO*& pInfo)
{
	TEXTURESLOT* pSlot;

	for (size_t i = 0; i < m_aSlotPool.size(); i++)
	{
		pSlot = m_aSlotPool[i];

		for (size_t j = 0; j < pSlot->CharInfo.size(); j++)
		{
			CHAR_TEX_INFO& info = pSlot->CharInfo[j];

			if (pSlot->FlushCount - info.tl > _char_tex_ttl)
			{
				if (info.ch)
				{
					ResetCharProp(info.ch);
					info.ch = 0;
				}

				pInfo = &info;
				return pSlot;
			}
		}
	}

	if ((pSlot = AllocTextureSlot()) == NULL)
		return NULL;

	pInfo = &pSlot->CharInfo[0];
	return pSlot;
}

void A3DFTFont::FreeTextureSlot(TEXTURESLOT* pSlot)
{
	_a3dftfont_tex_count--;

	if (!pSlot)
		return;

	if (pSlot->pTexture)
	{
		pSlot->pTexture->Release();
		delete pSlot->pTexture;
	}

	delete pSlot;
}

void A3DFTFont::TextOut(
	int x,
	int y,
	const wchar_t* szText,
	A3DCOLOR color,
	bool bShadowed,
	A3DCOLOR ShadowColor,
	float z,
	int nCharCount,
	float rhw,
	bool bUnderLine,
	A3DCOLOR UnderLineColor ,
	float fUnderLineThick
	)
{
	if (!m_bLoaded)
		return;

	const wchar_t* p = szText;
	float xOff  = x - .5f;
	float yOff	= y - .5f;
	A3DTLVERTEX aVerts[4];
	static const DWORD _specular = A3DCOLORRGBA(0, 0, 0, 255);
	bool bNewChar;

	Lock();

	while (*p && nCharCount)
	{
		CHARPROP* pProp = AddCharToMap(*p, true, bNewChar);
		p++;
		nCharCount--;

		if (bNewChar)
			m_dwCurNewCharCount++;

		if (pProp)
		{
			float fLineLeft = xOff, fLineRight = xOff+pProp->size.cx;
			float fThick = pProp->size.cy*fUnderLineThick<1.0f?1.0f:pProp->size.cy*fUnderLineThick;
			float fLineTop = yOff+pProp->size.cy-fThick, fLineBottom = yOff+pProp->size.cy;

			float xOff1 = xOff + pProp->offset.cx;
			float xOff2 = xOff1 + pProp->bmp_size.cx;
			xOff += pProp->size.cx;
			float yOff1 = yOff + pProp->offset.cy;
			float yOff2 = yOff1 + pProp->bmp_size.cy;

			aVerts[0].Set(
				A3DVECTOR4(xOff1, yOff1, z, rhw),
				color,
				_specular,
				pProp->tu1,
				pProp->tv1
				);

			aVerts[1].Set(
				A3DVECTOR4(xOff2, yOff1, z, rhw),
				color,
				_specular,
				pProp->tu2,
				pProp->tv1
				);

			aVerts[2].Set(
				A3DVECTOR4(xOff1, yOff2, z, rhw),
				color,
				_specular,
				pProp->tu1,
				pProp->tv2
				);

			aVerts[3].Set(
				A3DVECTOR4(xOff2, yOff2, z, rhw),
				color,
				_specular,
				pProp->tu2,
				pProp->tv2
				);

			AddVertsToOutputBuf(pProp->m_pSlot, aVerts, bShadowed, ShadowColor);

			// 下划线	By: Niuyadong 2010-1-12
			if (bUnderLine)
			{
				if (m_iOutline)
				{
					fLineRight += (2+2*m_iOutline);
					fLineTop += (2+2*m_iOutline);
					fLineBottom += (2+2*m_iOutline);
				}
				A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();
				pFC->AddRect_2D(fLineLeft, fLineTop, fLineRight, fLineBottom, UnderLineColor);
			}

		}

	}

	Unlock();
}


void A3DFTFont::TextOutVert(
	int x,
	int y,
	const wchar_t* szText,
	A3DCOLOR color,
	bool bShadowed,
	A3DCOLOR ShadowColor,
	float z,
	int nCharCount,
	float rhw,
	bool bUnderLine,
	A3DCOLOR UnderLineColor ,
	float fUnderLineThick,
	DWORD eRotate
	)
{
	if (!m_bLoaded)
		return;

	const wchar_t* p = szText;
	float xOff  = x - .5f;
	float yOff	= y - .5f;
	A3DTLVERTEX aVerts[4];
	static const DWORD _specular = A3DCOLORRGBA(0, 0, 0, 255);
	bool bNewChar;

	Lock();

	if(eRotate == 1)
	{
		while (*p && nCharCount)
		{
			CHARPROP* pProp = AddCharToMap(*p, true, bNewChar);
			p++;
			nCharCount--;

			if (bNewChar)
				m_dwCurNewCharCount++;

			if (pProp)
			{
				float xOff1 = pProp->size.cy + xOff - pProp->offset.cy - pProp->bmp_size.cy;
				float xOff2 = xOff1 + pProp->bmp_size.cy;

				float yOff1 = yOff + pProp->offset.cx;
				float yOff2 = yOff1 + pProp->bmp_size.cx;
				yOff += pProp->size.cx;

				aVerts[0].Set(
					A3DVECTOR4(xOff1, yOff1, z, rhw),
					color,
					_specular,
					pProp->tu1,
					pProp->tv2
					);

				aVerts[1].Set(
					A3DVECTOR4(xOff2, yOff1, z, rhw),
					color,
					_specular,
					pProp->tu1,
					pProp->tv1
					);

				aVerts[2].Set(
					A3DVECTOR4(xOff1, yOff2, z, rhw),
					color,
					_specular,
					pProp->tu2,
					pProp->tv2
					);

				aVerts[3].Set(
					A3DVECTOR4(xOff2, yOff2, z, rhw),
					color,
					_specular,
					pProp->tu2,
					pProp->tv1
					);

				AddVertsToOutputBuf(pProp->m_pSlot, aVerts, bShadowed, ShadowColor);
			}

		}
	}
	else if(eRotate == 2)
	{
		while (*p && nCharCount)
		{
			CHARPROP* pProp = AddCharToMap(*p, true, bNewChar);
			bool bRotate = IsVerticalRotationNeeded(*p);
			p++;
			nCharCount--;

			if (bNewChar)
				m_dwCurNewCharCount++;

			if (pProp)
			{
				if(bRotate)
				{
					float xOff1 = pProp->size.cy + xOff - pProp->offset.cy - pProp->bmp_size.cy;
					float xOff2 = xOff1 + pProp->bmp_size.cy;

					float yOff1 = yOff + pProp->offset.cx;
					float yOff2 = yOff1 + pProp->bmp_size.cx;
					yOff += pProp->size.cx;

					aVerts[0].Set(
						A3DVECTOR4(xOff1, yOff1, z, rhw),
						color,
						_specular,
						pProp->tu1,
						pProp->tv2
						);

					aVerts[1].Set(
						A3DVECTOR4(xOff2, yOff1, z, rhw),
						color,
						_specular,
						pProp->tu1,
						pProp->tv1
						);

					aVerts[2].Set(
						A3DVECTOR4(xOff1, yOff2, z, rhw),
						color,
						_specular,
						pProp->tu2,
						pProp->tv2
						);

					aVerts[3].Set(
						A3DVECTOR4(xOff2, yOff2, z, rhw),
						color,
						_specular,
						pProp->tu2,
						pProp->tv1
						);

					AddVertsToOutputBuf(pProp->m_pSlot, aVerts, bShadowed, ShadowColor);
				}
				else
				{
					float xOff1 = xOff + pProp->offset.cx;
					float xOff2 = xOff1 + pProp->bmp_size.cx;

					float yOff1 = yOff + pProp->offset.cy;
					float yOff2 = yOff1 + pProp->bmp_size.cy;
					yOff += pProp->size.cy;

					aVerts[0].Set(
						A3DVECTOR4(xOff1, yOff1, z, rhw),
						color,
						_specular,
						pProp->tu1,
						pProp->tv1
						);

					aVerts[1].Set(
						A3DVECTOR4(xOff2, yOff1, z, rhw),
						color,
						_specular,
						pProp->tu2,
						pProp->tv1
						);

					aVerts[2].Set(
						A3DVECTOR4(xOff1, yOff2, z, rhw),
						color,
						_specular,
						pProp->tu1,
						pProp->tv2
						);

					aVerts[3].Set(
						A3DVECTOR4(xOff2, yOff2, z, rhw),
						color,
						_specular,
						pProp->tu2,
						pProp->tv2
						);

					AddVertsToOutputBuf(pProp->m_pSlot, aVerts, bShadowed, ShadowColor);
				}
			}

		}
	}
	else /*if(eRotate == 0)*/
	{
		while (*p && nCharCount)
		{
			CHARPROP* pProp = AddCharToMap(*p, true, bNewChar);
			p++;
			nCharCount--;

			if (bNewChar)
				m_dwCurNewCharCount++;

			if (pProp)
			{
				float xOff1 = xOff + pProp->offset.cx;
				float xOff2 = xOff1 + pProp->bmp_size.cx;
				
				float yOff1 = yOff + pProp->offset.cy;
				float yOff2 = yOff1 + pProp->bmp_size.cy;
				yOff += pProp->size.cy;

				aVerts[0].Set(
					A3DVECTOR4(xOff1, yOff1, z, rhw),
					color,
					_specular,
					pProp->tu1,
					pProp->tv1
					);

				aVerts[1].Set(
					A3DVECTOR4(xOff2, yOff1, z, rhw),
					color,
					_specular,
					pProp->tu2,
					pProp->tv1
					);

				aVerts[2].Set(
					A3DVECTOR4(xOff1, yOff2, z, rhw),
					color,
					_specular,
					pProp->tu1,
					pProp->tv2
					);

				aVerts[3].Set(
					A3DVECTOR4(xOff2, yOff2, z, rhw),
					color,
					_specular,
					pProp->tu2,
					pProp->tv2
					);

				AddVertsToOutputBuf(pProp->m_pSlot, aVerts, bShadowed, ShadowColor);
			}

		}
	}

	Unlock();
}

void A3DFTFont::TextOutF(
	float x,
	float y,
	const wchar_t* szText,
	A3DCOLOR color,
	bool bShadowed,
	A3DCOLOR ShadowColor,
	float z,
	int nCharCount,
	float rhw
	)
{
	if (!m_bLoaded)
		return;

	const wchar_t* p = szText;
	float xOff  = x;
	float yOff	= y;
	A3DTLVERTEX aVerts[4];
	static const DWORD _specular = A3DCOLORRGBA(0, 0, 0, 255);

	Lock();

	A3DDevice* pDevice = m_pFontMan->m_pDevice;
	pDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	pDevice->SetTextureAddress(0, A3DTADDR_BORDER, A3DTADDR_BORDER);

#ifdef _DX9
	pDevice->GetD3DDevice()->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
#elif defined(_ANGELICA22)
	pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0);
#else
	pDevice->SetDeviceTextureStageState(0, D3DTSS_BORDERCOLOR, 0);
#endif

	pDevice->SetZWriteEnable(false);
	pDevice->SetLightingEnable(false);
	pDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	pDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	bool bNewChar;

	while (*p && nCharCount)
	{
		CHARPROP* pProp = AddCharToMap(*p, true, bNewChar);
		p++;
		nCharCount--;

		if (bNewChar)
			m_dwCurNewCharCount++;

		if (pProp)
		{
			float xOff1 = xOff + pProp->offset.cx;
			float xOff2 = xOff1 + pProp->bmp_size.cx;
			xOff += pProp->size.cx;
			float yOff1 = yOff + pProp->offset.cy;
			float yOff2 = yOff1 + pProp->bmp_size.cy;

			int nxOff1 = (int)xOff1;
			int nyOff1 = (int)yOff1;
			float dx = xOff1 - nxOff1;
			float dy = yOff1 - nyOff1;
			float u1 = pProp->tu1 - dx / _font_tex_buf_width;
			float u2 = pProp->tu2 + (1.0f - dx) / _font_tex_buf_width;
			float v1 = pProp->tv1 - dy / _font_tex_buf_height;
			float v2 = pProp->tv2 + (1.0f - dy) / _font_tex_buf_height;

			xOff1 = nxOff1 - .5f;
			xOff2 = xOff1 + pProp->bmp_size.cx + 1.0f;
			yOff1 = nyOff1 - .5f;
			yOff2 = yOff1 + pProp->bmp_size.cy + 1.0f;

			aVerts[0].Set(
				A3DVECTOR4(xOff1, yOff1, z, rhw),
				color,
				_specular,
				u1,
				v1
				);

			aVerts[1].Set(
				A3DVECTOR4(xOff2, yOff1, z, rhw),
				color,
				_specular,
				u2,
				v1
				);

			aVerts[2].Set(
				A3DVECTOR4(xOff1, yOff2, z, rhw),
				color,
				_specular,
				u1,
				v2
				);

			aVerts[3].Set(
				A3DVECTOR4(xOff2, yOff2, z, rhw),
				color,
				_specular,
				u2,
				v2
				);

			pProp->m_pSlot->pTexture->Appear(0);
			m_pFontMan->RenderStream(sizeof(A3DTLVERTEX), 1, aVerts, false, m_bStereo);
			pProp->m_pSlot->pTexture->Disappear(0);
		}
	}

	pDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pDevice->SetZWriteEnable(true);
	pDevice->SetLightingEnable(true);

	Unlock();
}

void A3DFTFont::DrawText(
	int x,
	int y,
	const wchar_t* szText,
	A3DCOLOR color,
	A3DRECT* pClip,
	bool bShadowed,
	A3DCOLOR ShadowColor,
	float z,
	int nCharCount,
	float rhw,
	bool bUnderLine,
	A3DCOLOR UnderLineColor ,
	float fUnderLineThick
	)
{
	if (!m_bLoaded)
		return;

	if (y >= pClip->bottom || y + m_iFontHei <= pClip->top || x >= pClip->right)
		return;

	const wchar_t* p = szText;
	float xOff  = x - .5f;
	float yOff	= y - .5f;
	float tv1, tv2;
	A3DTLVERTEX aVerts[4];
	static const DWORD _specular = A3DCOLORRGBA(0, 0, 0, 255);
	bool bNewChar;

	Lock();

	while (*p && nCharCount)
	{
		CHARPROP* pProp = AddCharToMap(*p, true, bNewChar);
		p++;
		nCharCount--;

		if (bNewChar)
			m_dwCurNewCharCount++;

		if (pProp)
		{
			float fLineLeft = xOff, fLineRight = xOff+pProp->size.cx;
			float fThick = pProp->size.cy*fUnderLineThick<1.0f?1.0f:pProp->size.cy*fUnderLineThick;
			float fLineTop = yOff+pProp->size.cy-fThick, fLineBottom = yOff+pProp->size.cy;
			float xOff1 = xOff + pProp->offset.cx;
			float xOff2 = xOff1 + pProp->bmp_size.cx;
			xOff += pProp->size.cx;
			float yOff1 = yOff + pProp->offset.cy;
			float yOff2 = yOff1 + pProp->bmp_size.cy;
			float fWid = 1.0f / pProp->bmp_size.cx;
			float fHei = 1.0f / pProp->bmp_size.cy;
			float fLeft, fRight, fTop, fBottom, tu1, tu2;

			if (xOff2 <= pClip->left)
				continue;

			if (xOff1 >= pClip->right)
				break;

			if (xOff1 < pClip->left)
			{
				fLeft = pClip->left;
				tu1 = (pClip->left - xOff1) * fWid;
			}
			else
			{
				fLeft = xOff1;
				tu1 = 0;
			}

			if (xOff2 > pClip->right)
			{
				fRight = pClip->right;
				tu2 = (xOff2 - pClip->right) * fWid;
			}
			else
			{
				fRight = xOff2;
				tu2 = 0;
			}

			if (yOff1 < pClip->top)
			{
				fTop = pClip->top;
				tv1 = (pClip->top - yOff1) * fHei;
			}
			else
			{
				fTop = yOff1;
				tv1 = 0;
			}

			if (yOff2 > pClip->bottom)
			{
				fBottom = pClip->bottom;
				tv2 = (yOff2 - pClip->bottom) * fHei;
			}
			else
			{
				fBottom = yOff2;
				tv2 = 0;
			}

			float du = pProp->tu2 - pProp->tu1;
			float dv = pProp->tv2 - pProp->tv1;

			aVerts[0].Set(
				A3DVECTOR4(fLeft, fTop, z, rhw),
				color,
				_specular,
				pProp->tu1 + tu1 * du,
				pProp->tv1 + tv1 * dv
				);

			aVerts[1].Set(
				A3DVECTOR4(fRight, fTop, z, rhw),
				color,
				_specular,
				pProp->tu2 - tu2 * du,
				pProp->tv1 + tv1 * dv
				);

			aVerts[2].Set(
				A3DVECTOR4(fLeft, fBottom, z, rhw),
				color,
				_specular,
				pProp->tu1 + tu1 * du,
				pProp->tv2 - tv2 * dv
				);

			aVerts[3].Set(
				A3DVECTOR4(fRight, fBottom, z, rhw),
				color,
				_specular,
				pProp->tu2 - tu2 * du,
				pProp->tv2 - tv2 * dv
				);

			AddVertsToOutputBuf(pProp->m_pSlot, aVerts, bShadowed, ShadowColor);

			// 下划线	By: Niuyadong 2010-1-12
			if (bUnderLine)
			{
				if (m_iOutline)
				{
					fLineRight += (2+2*m_iOutline);
					fLineTop += (2+2*m_iOutline);
					fLineBottom += (2+2*m_iOutline);
				}
				A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();
				pFC->AddRect_2D(fLineLeft, fLineTop, fLineRight, fLineBottom, UnderLineColor);
			}
			
		}
	}

	Unlock();
}

#define _COLOR32(a, c) (((DWORD)a << 24) | ((DWORD)c & 0xFFFFFF))

A3DFTFont::CHARPROP* A3DFTFont::AddCharToMap(wchar_t ch, bool bSetTex, bool& bNewChar)
{
	CHARPROP* pProp;
	CharMap::iterator it = m_CharMap.find(ch);

	if (it != m_CharMap.end())
	{
		bNewChar = false;

		if (!bSetTex)
			return it->second;

		pProp = it->second;
	}
	else
	{
		bNewChar = true;
		pProp = new CHARPROP;

		if (!m_pFTFont->GetCharExtent(ch, &pProp->offset, &pProp->size, &pProp->bmp_size))
		{
			delete pProp;
			return NULL;
		}
		
		if (m_iOutline)	//	注意:按策划要求 轮廓线字体的宽度范围不计算边缘轮廓线 2009-8-6 By:Niuyadong 
		{
			pProp->size.cx-=2;
			pProp->size.cy-=2;
		}
		
		pProp->m_pSlot	= NULL;
		pProp->m_char	= ch;
		m_CharMap[ch] = pProp;

		if (!bSetTex)
			return pProp;
	}

	TEXTURESLOT* pSlot = pProp->m_pSlot;

	if (pSlot)
		pProp->m_pTexInfo->tl = pSlot->FlushCount;
	else
	{
		BYTE btBuf[_font_tex_buf_width * _font_tex_buf_height];
		const FTDSIZE& size = pProp->bmp_size;

		CHAR_TEX_INFO* pInfo;
		pSlot = GetAvailableSlot(pInfo);

		if (!pSlot)
			return NULL;

		DWORD* pTexBuf;
		int iPitch;

#ifdef _DX9

		RECT rc;
		rc.left = pInfo->x;
		rc.right = pInfo->x + m_iFontWid;
		rc.top = pInfo->y;
		rc.bottom = pInfo->y + m_iFontHei;

		if (!pSlot->pTexture->LockRect(&rc, (void**)&pTexBuf, &iPitch, 0))
			return NULL;

#else

		if (!pSlot->pTexture->LockRect(NULL, (void**)&pTexBuf, &iPitch, 0))
			return NULL;

#endif

		pInfo->ch = ch;
		pInfo->tl = pSlot->FlushCount;

		int nXEnd = pInfo->x + size.cx;
		int nYEnd = pInfo->y + size.cy;
		int nXGridEnd = pInfo->x + m_iFontWid;
		int nYGridEnd = pInfo->y + m_iFontHei;

		pProp->m_pSlot = pSlot;
		pProp->m_pTexInfo = pInfo;
		pProp->tu1 = pInfo->x * (1.0f / _font_tex_buf_width);
		pProp->tv1 = pInfo->y * (1.0f / _font_tex_buf_height);
		pProp->tu2 = nXEnd * (1.0f / _font_tex_buf_width);
		pProp->tv2 = nYEnd * (1.0f / _font_tex_buf_height);

		if (m_iOutline)
		{
			m_pFTFont->DrawChar(ch, btBuf, size.cx, size.cy);
			//m_pFTFont->DrawCharWithOutline(ch, btBuf, btOutline, size.cx, size.cy);

			BYTE* pBuf;
			int line = 0;
			int j;
			//for (j = pInfo->y; j < nYGridEnd; j++)
				//::ZeroMemory(pTexBuf + j * _font_tex_buf_width + pInfo->x, m_iFontWid<<1);

			for (j = pInfo->y; j < nYGridEnd; j++)
			{
#ifdef _DX9
				int nOffY = line * _font_tex_buf_width;
#else
				int nOffY = j * _font_tex_buf_width;
#endif

				pBuf = btBuf + line * size.cx;
				line++;

				for (int i = pInfo->x; i < nXGridEnd; i++)
				{
#ifdef _DX9
					int nOff = nOffY + i - pInfo->x;
#else
					int nOff = nOffY + i;
#endif

					bool bTestEdge = (i == pInfo->x || i == (nXEnd-1)
						|| j == pInfo->y || j == (nYEnd-1));//提高效率但是代码变长,防止平移8个方向后越界
					bool bEdge = (i >= nXEnd || j >= nYEnd);

					if (bEdge)
						pTexBuf[nOff] = 0;
					else
					{
						if (bTestEdge)
						{
							BYTE sa, a = *pBuf;
							BYTE* pCur;
							if (j != pInfo->y)
							{
								pCur = pBuf - size.cx - 1;
								if(i != pInfo->x)
								{
									sa = *pCur;
									a = sa > a ? sa : a;
								}	//	Up Left

								sa = *(pCur+1);
								a = sa > a ? sa : a;	//	Up

								if(i != (nXEnd-1))
								{
									sa = *(pCur+2);
									a = sa > a ? sa : a;
								}	//	Right
							}

							if(i != pInfo->x)
							{
								sa = *(pBuf-1);
								a = sa > a ? sa : a;
							}	//	Left

							if(i != (nXEnd-1))
							{
								sa = *(pBuf+1);
								a = sa > a ? sa : a;
							}	//	Right

							if (j != (nYEnd-1))
							{
								pCur = pBuf + size.cx - 1;
								if(i != pInfo->x)
								{
									sa = *pCur;
									a = sa > a ? sa : a;
								}	//	Bottom Left

								sa = *(pCur+1);
								a = sa > a ? sa : a;	// Bottom

								if(i != (nXEnd-1))
								{
									sa = *(pCur+2);
									a = sa > a ? sa : a;
								}	// Bottom Right
							}

							pTexBuf[nOff] = a ? _COLOR32(a, m_clOutline) : 0;
						}
						else
						{
							WORD sa,a = *pBuf;
							BYTE* pCur = pBuf - size.cx - 1;

							sa = *pCur; a = sa > a ? sa : a;	//	Up Left
							sa = *(pCur+1); a = sa > a ? sa : a;	//	Up
							sa = *(pCur+2);  a = sa > a ? sa : a;	//	Right

							sa = *(pBuf-1);  a = sa > a ? sa : a;	//	Left
							sa = *(pBuf+1);  a = sa > a ? sa : a;	//	Right

							pCur = pBuf + size.cx - 1;
							sa = *pCur;  a = sa > a ? sa : a;	//	Bottom Left
							sa = *(pCur+1);  a = sa > a ? sa : a;	// Bottom
							sa = *(pCur+2);  a = sa > a ? sa : a;	// Bottom Right

							if (a)
							{
								pTexBuf[nOff] = _COLOR32(a, m_clOutline);
							}
							else
								pTexBuf[nOff] = 0;
							
						}
						pBuf++;
					}
				}
			}

			line = 0;
			int delta = nYGridEnd - pInfo->y -1; 
			delta = delta>0?delta:1;
			for ( j = pInfo->y; j < nYGridEnd; j++)
			{
				DWORD clInner;

				if (m_bInterpColor)
				{
					float co = (float)line / delta;
					float nco = 1.0f - co;
					nco = nco>=0.0f?(nco<=1.0f?nco:1.0f):0.0f;
					A3DHSVCOLORVALUE clhsv;
					clhsv.h = m_clHSVUpperReserve.h * nco + m_clHSVLowerReserve.h * co;
					clhsv.s = m_clHSVUpperReserve.s * nco + m_clHSVLowerReserve.s * co;
					clhsv.v = m_clHSVUpperReserve.v * nco + m_clHSVLowerReserve.v * co;

					a3d_HSVToRGB(clhsv, &clInner);
					clInner &= 0xFFFFFF;
				}
				else
					clInner = m_clInner;

#ifdef _DX9
				int nOffY = line * _font_tex_buf_width;
#else
				int nOffY = j * _font_tex_buf_width;
#endif

				pBuf = btBuf + line * size.cx;
				line++;

				for (int i = pInfo->x; i < nXGridEnd; i++)
				{
#ifdef _DX9
					int nOff = nOffY + i - pInfo->x;
#else
					int nOff = nOffY + i;
#endif

					bool bEdge = (i >= nXEnd || j >= nYEnd);

					if (bEdge)
					{}
					else
					{
						if (*pBuf)
						{
							DWORD a = *pBuf;
							DWORD sa = (pTexBuf[nOff]>>24);
							DWORD suma = ((a+sa) > 255) ? 255 : (a + sa);
							float inv = a/255.0f;
							float invs = (1.0f - inv)*sa/255.0f;
							if (suma)
							{
								DWORD r = (DWORD)( (float)((clInner>>16)&0xFF)*inv + (float)((pTexBuf[nOff]>>16)&0xFF)*invs );
								DWORD g = (DWORD)( (float)((clInner>>8)&0xFF)*inv + (float)((pTexBuf[nOff]>>8)&0xFF)*invs );
								DWORD b = (DWORD)( (float)(clInner&0xFF)*inv + (float)(pTexBuf[nOff]&0xFF)*invs );

								A3DCOLOR clInnerotl = ((r << 16) | (g << 8) | b);
								pTexBuf[nOff] = _COLOR32(suma, clInnerotl);
							}		
						}

						pBuf++;
					}
				}
			}

		}
		else
		{
			m_pFTFont->DrawChar(ch, btBuf, size.cx, size.cy);

			BYTE* pBuf;
			int line = 0;

			int delta = nYGridEnd - pInfo->y -1; 
			delta = delta>0?delta:1;
			for (int j = pInfo->y; j < nYGridEnd; j++)
			{
				DWORD clInner;

				if (m_bInterpColor)
				{
					float co = (float)line / delta;
					float nco = 1.0f - co;
					nco = nco>=0.0f?(nco<=1.0f?nco:1.0f):0.0f;
					A3DHSVCOLORVALUE clhsv;
					clhsv.h = m_clHSVUpperReserve.h * nco + m_clHSVLowerReserve.h * co;
					clhsv.s = m_clHSVUpperReserve.s * nco + m_clHSVLowerReserve.s * co;
					clhsv.v = m_clHSVUpperReserve.v * nco + m_clHSVLowerReserve.v * co;

					a3d_HSVToRGB(clhsv, &clInner);
					clInner &= 0xFFFFFF;
				}
				else
					clInner = 0xFFFFFF;

#ifdef _DX9
				int nOffY = line * _font_tex_buf_width;
#else
				int nOffY = j * _font_tex_buf_width;
#endif

				pBuf = btBuf + line * size.cx;
				line++;

				for (int i = pInfo->x; i < nXGridEnd; i++)
				{
#ifdef _DX9
					int nOff = nOffY + i - pInfo->x;
#else
					int nOff = nOffY + i;
#endif

					bool bEdge = (i >= nXEnd || j >= nYEnd);

					if (bEdge)
						pTexBuf[nOff] = 0;
					else
					{
						DWORD a = *pBuf;
						pTexBuf[nOff] = _COLOR32(a, clInner);
						pBuf++;
					}
				}
			}
		}

		pSlot->pTexture->UnlockRect();
	}

	return pProp;
}

inline void offset_shadow(TLVERTEX_FONT_SHADOW* p, A3DTLVERTEX* pVert)
{
	memcpy(p, pVert, sizeof(A3DTLVERTEX));
	p->tu2 = p->tu1 - _shadow_offset / _font_tex_buf_width;
	p->tv2 = p->tv1;
	p->tu3 = p->tu1 + _shadow_offset / _font_tex_buf_width;
	p->tv3 = p->tv1;
	p->tu4 = p->tu1;
	p->tv4 = p->tv1 -_shadow_offset / _font_tex_buf_height;
	p->tu5 = p->tu1;
	p->tv5 = p->tv1 + _shadow_offset / _font_tex_buf_height;
}

void A3DFTFont::AddVertsToOutputBuf(TEXTURESLOT* pSlot, A3DTLVERTEX* pVerts, bool bShadowed, A3DCOLOR ShadowColor)
{
	assert(m_bLoaded);
	_a3dftfont_char_draw_count++;
	pSlot->VertBuf.push_back(*pVerts);
	pSlot->VertBuf.push_back(*(pVerts + 1));
	pSlot->VertBuf.push_back(*(pVerts + 2));
	pSlot->VertBuf.push_back(*(pVerts + 3));

	if (bShadowed)
	{
		pVerts[0].x += _shadow_offset;
		pVerts[0].y += _shadow_offset;
		pVerts[0].diffuse = ShadowColor;

		pVerts[1].x += _shadow_offset;
		pVerts[1].y += _shadow_offset;
		pVerts[1].diffuse = ShadowColor;

		pVerts[2].x += _shadow_offset;
		pVerts[2].y += _shadow_offset;
		pVerts[2].diffuse = ShadowColor;

		pVerts[3].x += _shadow_offset;
		pVerts[3].y += _shadow_offset;
		pVerts[3].diffuse = ShadowColor;

		TLVERTEX_FONT_SHADOW v;

		offset_shadow(&v, pVerts);
		pSlot->ShadowBuf.push_back(v);
		pVerts++;

		offset_shadow(&v, pVerts);
		pSlot->ShadowBuf.push_back(v);
		pVerts++;

		offset_shadow(&v, pVerts);
		pSlot->ShadowBuf.push_back(v);
		pVerts++;

		offset_shadow(&v, pVerts);
		pSlot->ShadowBuf.push_back(v);
	}
}

A3DPOINT2 A3DFTFont::GetTextExtent(const wchar_t* szText, int nCharCount, bool bVertical)
{
	if (!m_bLoaded)
		return A3DPOINT2(0, 0);

	A3DPOINT2 sz(0, m_iFontHei);
	const wchar_t* p = szText;
	bool bNewChar;

	Lock();

	if(bVertical)
	{
		sz.x = m_iFontWid;
		sz.y = 0;
		while (*p && nCharCount)
		{
			CHARPROP* pProp = AddCharToMap(*p, false, bNewChar);
			bool bRotate = IsVerticalRotationNeeded(*p);
			if(bRotate && m_iFontHei > sz.x) sz.x = m_iFontHei; 
			p++;
			nCharCount--;

			if (bNewChar)
				m_dwCurNewCharCount++;

			if (!pProp)
				continue;

			sz.y += bRotate ? pProp->size.cx : pProp->size.cy;
		}
	}
	else
	{
		while (*p && nCharCount)
		{
			CHARPROP* pProp = AddCharToMap(*p, false, bNewChar);
			p++;
			nCharCount--;

			if (bNewChar)
				m_dwCurNewCharCount++;

			if (!pProp)
				continue;

			sz.x += pProp->size.cx;
		}
	}

	Unlock();
	return sz;
}

inline void _PresetRenderState(A3DDevice* pDevice)
{
	pDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	pDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	pDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	pDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	pDevice->SetZWriteEnable(false);
	pDevice->SetLightingEnable(false);
	pDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	pDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
}

inline void _PostSetRenderState(A3DDevice* pDevice)
{
	pDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	pDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	pDevice->SetZWriteEnable(true);
	pDevice->SetLightingEnable(true);
}

bool A3DFTFont::FlushOutputBuf(TEXTURESLOT* pSlot)
{
	assert(m_bLoaded);
	pSlot->FlushCount++;

	if (pSlot->VertBuf.size() == 0)
		return true;

	A3DDevice* pDevice = m_pFontMan->m_pDevice;

	_PresetRenderState(pDevice);

	pSlot->pTexture->Appear(0);

	if (pSlot->ShadowBuf.size())
	{
		if (m_pFontMan->m_pShadowShader)
		{
			pSlot->pTexture->Appear(1);
			pSlot->pTexture->Appear(2);
			pSlot->pTexture->Appear(3);
			pSlot->pTexture->Appear(4);

			m_pFontMan->m_pShadowShader->Appear();

			pDevice->SetTextureCoordIndex(0, 0);
			pDevice->SetTextureMatrix(0, IdentityMatrix());
			pDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

			pDevice->SetTextureCoordIndex(1, 1);
			pDevice->SetTextureMatrix(1, IdentityMatrix());
			pDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

			pDevice->SetTextureCoordIndex(2, 2);
			pDevice->SetTextureMatrix(2, IdentityMatrix());
			pDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			
			pDevice->SetTextureCoordIndex(3, 3);
			pDevice->SetTextureMatrix(3, IdentityMatrix());
			pDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

			pDevice->SetTextureCoordIndex(4, 4);
			pDevice->SetTextureMatrix(4, IdentityMatrix());
			pDevice->SetTextureAddress(4, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

			pDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			pDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			pDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
			pDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			pDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);

			m_pFontMan->RenderStream(sizeof(TLVERTEX_FONT_SHADOW), pSlot->ShadowBuf.size() >> 2,  &pSlot->ShadowBuf[0], true, m_bStereo);

			pDevice->SetTextureFilterType(1, A3DTEXF_POINT);
			pDevice->SetTextureFilterType(2, A3DTEXF_POINT);
			pDevice->SetTextureFilterType(3, A3DTEXF_POINT);
			pDevice->SetTextureFilterType(4, A3DTEXF_POINT);
			pDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
			pDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
			pDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
			pDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
			pDevice->ClearTexture(1);
			pDevice->ClearTexture(2);
			pDevice->ClearTexture(3);
			pDevice->ClearTexture(4);

			m_pFontMan->m_pShadowShader->Disappear();
		}
		else
		{
			pDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			m_pFontMan->RenderStream(sizeof(TLVERTEX_FONT_SHADOW), pSlot->ShadowBuf.size() >> 2, pSlot->ShadowBuf.begin(), true, m_bStereo);
		}

		pSlot->ShadowBuf.clear();
	}

	pDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	m_pFontMan->RenderStream(sizeof(A3DTLVERTEX), pSlot->VertBuf.size() >> 2, pSlot->VertBuf.begin(), false, m_bStereo);
	pSlot->VertBuf.clear();

	//	Restore render state
	pSlot->pTexture->Disappear(0);
	_PostSetRenderState(pDevice);

	return true;
}

void A3DFTFont::Flush()
{
	m_dwCurNewCharCount = 0;

	if (!m_bLoaded)
		return;

	Lock();

	for (size_t i = 0; i < m_aSlotPool.size(); i++)
		FlushOutputBuf(m_aSlotPool[i]);

	A3DFlatCollector *pFC = m_pA3DEngine->GetA3DFlatCollector();
	pFC->Flush_2D();

	Unlock();
}

bool A3DFTFont::CanTextOutNow() 
{ 
	return IsLoaded() && m_dwCurNewCharCount < m_pFontMan->m_dwMaxNewCharPerFrame; 
}
