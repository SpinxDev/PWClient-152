#include <sys/stat.h>

#include "A3DPlatform.h"
#include "A3DTexture.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DSurface.h"
#include "A3DTextureMan.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DDevice.h"
#include "A3DDDSFile.h"
#include "AFileImage.h"
#include "AFI.h"

#include <ddraw.h>

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DTexture
//
///////////////////////////////////////////////////////////////////////////
int TEXTURE_TOTAL_COUNT = 0;
#ifdef _DEBUG
#include "hashmap.h"
abase::hash_map<A3DTexture*, A3DTexture*> s_TextureMap;

void DumpAllExistTextures()
{
	FILE * fp = fopen("tex_dump.txt", "wb");
	for (abase::hash_map<A3DTexture*, A3DTexture*>::iterator itr = s_TextureMap.begin()
		; itr != s_TextureMap.end()
		; ++itr)
	{
		A3DTexture* pTex = itr->first;
		fprintf(fp, "%10s %s 0x%p\n", pTex->GetClassID() == A3D_CID_HLSLSHADER ? "Effect" : "Texture", pTex->GetMapFile(), pTex);
	}

	fclose(fp);
}
#endif

A3DTexture::A3DTexture()
{
	m_pA3DDevice		= NULL;
	m_dwClassID			= A3D_CID_TEXTURE2D;
	m_bHWITexture		= false;
	m_pDXTexture		= NULL;
	m_bAlphaTexture		= false;
	m_bShaderTexture	= false;
	m_bDetailTexture	= false;
	m_bLightMap			= false;
	m_bDDSTexture		= false;
	m_bNoDownSample		= false;
	m_dwTextureID		= 0;
	m_dwPosInMan		= 0;
	m_iTexDataSize		= 0;
    m_nWidth			= 0;
    m_nHeight			= 0;

	m_dwTimeStamp		= 0;
	m_nDesiredMipLevel	= 0; // Complete mip map chain will be created;
	TEXTURE_TOTAL_COUNT++;
#ifdef _DEBUG
	s_TextureMap[this] = this;
#endif

	m_nLockLevel		= 0;
	m_pLockSurface		= NULL;
	m_bUseRect			= FALSE;
	m_dwUsage			= 0;
}
	
A3DTexture::~A3DTexture()
{
	TEXTURE_TOTAL_COUNT--;
#ifdef _DEBUG
	s_TextureMap.erase(this);
#endif
}

bool A3DTexture::Init(A3DDevice * pDevice)
{
	m_pA3DDevice		= pDevice;
	m_strMapFile		= "";
	m_pDXTexture 		= NULL;
	m_bHWITexture		= false;
	m_bAlphaTexture		= false;
	m_bShaderTexture	= false;
	m_bDetailTexture	= false;
	m_bLightMap			= false;
	m_bDDSTexture		= false;
	m_dwTextureID		= 0;
	m_dwPosInMan		= 0;
	m_nDesiredMipLevel	= 0; // Complete mip map chain will be created;
	m_iTexDataSize		= 0;
	return true;
}

bool A3DTexture::LoadFromFile(A3DDevice* pA3DDevice, const char* szFullpath, const char* szRelativePath)
{
	m_pA3DDevice = pA3DDevice;

	af_GetRelativePath(szFullpath, "", m_strMapFile);
	m_dwTextureID = a_MakeIDFromLowString(m_strMapFile);

	if (af_CheckFileExt(szRelativePath, ".dds", 4))
		m_bDDSTexture = true;
	else if (af_CheckFileExt(szRelativePath, ".tga", 4))
		m_bAlphaTexture = true;

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER || g_pA3DConfig->GetFlagNoTextures())
	{
		m_bHWITexture = true;
		return true;
	}

	if (!m_pA3DDevice)
		return true;

	AFileImage fileImage;

	if( !fileImage.Open(szFullpath, AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
	{
#ifdef _DEBUG
		g_A3DErrLog.Log("A3DTexture::LoadFromFile(), Can not init the file to be a fileimage [%s][%s]!", szFullpath, szRelativePath);
#endif
		m_pDXTexture = NULL;
		return true;
	}

	if (m_bDetailTexture) 
	{
		if (!LoadDetailTexture(fileImage.GetFileBuffer(), fileImage.GetFileLength(), szRelativePath))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DTexture::LoadFromFile(), call LoadDetailTexture() fail!");
			return false;
		}
	}
	else if (m_bLightMap)
	{
		if (!LoadLightMapTexture(fileImage.GetFileBuffer(), fileImage.GetFileLength(), szRelativePath))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DTexture::LoadFromFile(), call LoadLightMapTexture() fail!");
			return false;
		}
	}
	else
	{
		if (!LoadNormalTexture(fileImage.GetFileBuffer(), fileImage.GetFileLength(), szRelativePath))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DTexture::LoadFromFile(), call LoadNormalTexture() fail!");
			return false;
		}
	}

	m_dwTimeStamp = fileImage.GetTimeStamp();
	fileImage.Close();

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_TEXTURE);
    
	return true;
}

bool A3DTexture::Release()
{
	if (m_pDXTexture)
	{
		if (m_pA3DDevice)
		{
			m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_TEXTURE);
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(-m_iTexDataSize);
		}

		if(m_pA3DDevice->GetNeedResetFlag())
		{
			m_pDXTexture->Release();
		}
		else
		{
			m_pA3DDevice->ReleaseResource(m_pDXTexture);// multiThread render
		}
		m_pDXTexture = NULL;
	}

	m_dwTimeStamp = 0;
	m_pA3DDevice = NULL;

	return true;
}

bool A3DTexture::Appear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	//if( m_pDXTexture )
	//	m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, m_pDXTexture);
	//else
	//	m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, NULL);

    // by yaochunhui: use a3ddevice 
    //if (m_bContentChanged)
    //{
    //    m_pA3DDevice->raw_SetTexture(nLayer, NULL);
    //    m_bContentChanged = false;
    //}
    m_pA3DDevice->raw_SetTexture(nLayer, m_pDXTexture);

	return true;
}

bool A3DTexture::Disappear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	//if( m_pDXTexture )
	//	m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, NULL);
    m_pA3DDevice->raw_SetTexture(nLayer, NULL);

	return true;
}

bool A3DTexture::Create(A3DDevice* pDevice, int iWidth, int iHeight, A3DFORMAT Format, int nMipLevel, DWORD dwUsage)
{
	if (m_pDXTexture)
	{
		g_A3DErrLog.Log("A3DTexture::Create This texture has been created, can not recreated!");
		return false;
	}

	if (!Init(pDevice))
		return false;

	m_strMapFile	= "";
	m_nMipLevel		= nMipLevel;
	m_dwTextureID	= 0;
	m_dwTimeStamp	= 0;

	int nMaxMipLevel = GetMaxMipLevel(iWidth, iHeight);
	if (m_nMipLevel <= 0 || m_nMipLevel > nMaxMipLevel) 
		m_nMipLevel = nMaxMipLevel;
	
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bHWITexture = true;
		return true;
	}

	if (!CreateDXTexture(iWidth, iHeight, Format, m_nMipLevel, dwUsage))
		return false;

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_TEXTURE);
    
	return true;
}

bool A3DTexture::LockRect(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel)
{
	if (!m_pDXTexture || m_bHWITexture)
		return false;

	if( nMipLevel >= m_nMipLevel )
	{
		g_A3DErrLog.Log("A3DTexture::LockRect(), mip level [%d] larger than m_nMipLevel [%d]", nMipLevel, m_nMipLevel);
		return false;
	}

	D3DLOCKED_RECT LockedRect;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag() && m_dwUsage != D3DUSAGE_DYNAMIC)
	{
		D3DSURFACE_DESC dsc;
		m_pDXTexture->GetLevelDesc(nMipLevel, &dsc);

		if (pRect)
		{
			int nWidth = pRect->right - pRect->left;
			int nHeight = pRect->bottom - pRect->top;

			if (D3D_OK != m_pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(nWidth, nHeight, dsc.Format, D3DPOOL_SYSTEMMEM, &m_pLockSurface, NULL))
				return false;

			m_bUseRect = TRUE;
			m_rcLock = *pRect;
		}
		else
		{
			if (D3D_OK != m_pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(dsc.Width, dsc.Height, dsc.Format, D3DPOOL_SYSTEMMEM, &m_pLockSurface, NULL))
				return false;

			m_bUseRect = FALSE;
		}

		if (D3D_OK != m_pLockSurface->LockRect(&LockedRect, NULL, 0))
		{
			m_pLockSurface->Release();
			m_pLockSurface = NULL;
			return false;
		}

		m_nLockLevel = nMipLevel;
	}
	else
	{
		if (FAILED(m_pDXTexture->LockRect(nMipLevel, &LockedRect, pRect, 0)))
			return false;
	}

	*ppData = LockedRect.pBits;
	*iPitch = LockedRect.Pitch;
	return true;
}

bool A3DTexture::UnlockRect()
{
	if (!m_pDXTexture || m_bHWITexture)
		return false;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag() && m_dwUsage != D3DUSAGE_DYNAMIC)
	{
		bool bRet = false;

		if (m_pLockSurface)
		{
			m_pLockSurface->UnlockRect();
			IDirect3DSurface9* pDstSurface;

			if (D3D_OK == m_pDXTexture->GetSurfaceLevel(m_nLockLevel, &pDstSurface))
			{
				if (D3D_OK == D3DXLoadSurfaceFromSurface(pDstSurface, NULL, m_bUseRect ? &m_rcLock : NULL, m_pLockSurface, NULL, NULL, D3DX_FILTER_NONE, 0))
				{
					bRet = true;
				}

				pDstSurface->Release();
			}

			m_pLockSurface->Release();
			m_pLockSurface = NULL;
			m_nLockLevel = 0;
			m_bUseRect = FALSE;
		}

		return bRet;
	}
	else
	{
		if (FAILED(m_pDXTexture->UnlockRect(0)))
			return false;
	}

	return true;
}

bool A3DTexture::UpdateFromSurface(A3DSurface * pA3DSurface, A3DRECT & rect)
{
	IDirect3DSurface9* pDestSurf;
	HRESULT hval = m_pDXTexture->GetSurfaceLevel(0, &pDestSurf);
	if (FAILED(hval))
	{
		g_A3DErrLog.Log("A3DTexture::UpdateFromSurface(), GetSurfaceLevel Fail!");
		return false;
	}

	int iWidth = rect.right - rect.left;
	int iHeight = rect.bottom - rect.top;
	if (iWidth <= 0 || iHeight <= 0)
		return false;

	RECT rcSrc;
	rcSrc.left = rect.left;
	rcSrc.right = rect.right;
	rcSrc.top = rect.top;
	rcSrc.bottom = rect.bottom;

	RECT rcDest;
	rcDest.left = 0;
	rcDest.top = 0;
	rcDest.right = rcDest.left + iWidth;
	rcDest.bottom = rcDest.top + iHeight;

	hval = D3DXLoadSurfaceFromSurface(pDestSurf, NULL, &rcDest, pA3DSurface->GetDXSurface(), NULL, &rcSrc, D3DX_FILTER_NONE, 0);
	if (FAILED(hval)) 
	{
		g_A3DErrLog.Log("A3DTexture::UpdateFromSurface(), D3DXLoadSurfaceFromSurface Fail!");
		pDestSurf->Release();
		return false;
	}

	pDestSurf->Release();
	pDestSurf = NULL;

	return true;
}

bool A3DTexture::CreateFromSurface(A3DDevice * pA3DDevice, A3DSurface * pA3DSurface, bool bDynamicTex, A3DRECT& rect, int * pTexWidth, int * pTexHeight, A3DFORMAT fmtSurface)
{
	m_pA3DDevice = pA3DDevice;

	int nTexWidth = rect.right - rect.left;
	int nTexHeight = rect.bottom - rect.top;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWITexture = true;
		*pTexWidth = nTexWidth;
		*pTexHeight = nTexHeight;
		return true;
	}

	A3DFORMAT format;
	// For we can create a texture not from a specified surface, so we must check it here;
	if( pA3DSurface )
		format = pA3DSurface->GetFormat();
	else
		format = fmtSurface;

	//Now determine the texture's width and height;
	int nPowerOf2, i;
	nPowerOf2 = 4;
	for(i=2; i<=11; i++)
	{
		if( nTexWidth <= nPowerOf2 )
		{
			nTexWidth = nPowerOf2;
			break;
		}
		nPowerOf2 *= 2;
	}
	nPowerOf2 = 4;
	for(i=2; i<=11; i++)
	{
		if( nTexHeight <= nPowerOf2 )
		{
			nTexHeight = nPowerOf2;
			break;
		}
		nPowerOf2 *= 2;
	}

	bool bval = Create(pA3DDevice, nTexWidth, nTexHeight, format, 1, bDynamicTex && m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag() ? D3DUSAGE_DYNAMIC : 0);
	if (!bval)
		return false;

	if (pA3DSurface)
	{
		if( !UpdateFromSurface(pA3DSurface, rect) )
		{
			g_A3DErrLog.Log("A3DTexture::CreateFromSurface UpdateFromSurface Fail!");
			return false;
		}
	}

	*pTexWidth = nTexWidth;
	*pTexHeight = nTexHeight;

	return true;
}

bool A3DTexture::TickAnimation()
{
	return true;
}

bool A3DTexture::ExportToFile(const char * szFullpath)
{
	if( m_bHWITexture ) return true;

	HRESULT hval;
	// Require DX 8.1
	hval = D3DXSaveTextureToFileA(szFullpath, D3DXIFF_BMP, m_pDXTexture, NULL);
	if (FAILED(hval)) 
	{
		g_A3DErrLog.Log("A3DTexture::ExportToFile(), Fail to export to file [%s]", szFullpath);
		return false;
	}
	return true;
}

// This is only the wrapper function for creating a DX texture
bool A3DTexture::CreateDXTexture(int nWidth, int nHeight, A3DFORMAT format, int nMipLevel, DWORD dwUsage)
{
	m_dwUsage = dwUsage;
	D3DPOOL Pool;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;

#ifndef UNICODE
		if (m_dwUsage == 0)
			m_dwUsage = D3DUSAGE_DYNAMIC;
#endif
	}
	else
	{
		Pool = D3DPOOL_MANAGED;
		m_dwUsage = 0;
	}

	if (FAILED(m_pA3DDevice->GetD3DDevice()->CreateTexture(nWidth, nHeight, nMipLevel,
              m_dwUsage, (D3DFORMAT)format, Pool, &m_pDXTexture, NULL)))
	{
		g_A3DErrLog.Log("A3DTexture::CreateDXTExture(), Fail in creating format[%d], [%d]x[%d], [%d]mip texture", format, nWidth, nHeight, nMipLevel);
		return false;
	}

	CalcTextureDataSize();

	return true;
}

void A3DTexture::SetMapFile(const char* szFile)
{ 
	m_strMapFile	= szFile; 
	m_dwTextureID	= a_MakeIDFromLowString(m_strMapFile);
}

static bool CreateTextureFromFileInMemoryEx(
									 LPDIRECT3DDEVICE9         pDevice,
									 const BYTE*               pSrcData,
									 UINT                      SrcDataSize,
									 UINT                      Width,
									 UINT                      Height,
									 UINT                      MipLevels,
									 DWORD                     Usage,
									 D3DFORMAT                 Format,
									 D3DPOOL                   Pool,
									 DWORD                     Filter,
									 DWORD                     MipFilter,
									 D3DCOLOR                  ColorKey,
									 D3DXIMAGE_INFO*           pSrcInfo,
									 PALETTEENTRY*             pPalette,
									 LPDIRECT3DTEXTURE9*       ppTexture,
									 UINT                      TextureLevel = 0)
{
	//	Get texture info
	DDS_HEADER* pHeader = NULL;
	BYTE* pBitData = NULL;
	D3DFORMAT fmtInner = D3DFMT_UNKNOWN;
	if (!A3DDDSFile::GetTextureInfoFromBuffer(pSrcData, &pHeader, &pBitData, &fmtInner))
		return false;

	if (TextureLevel==0 || pHeader->dwMipMapCount <= 1)
	{
		return SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, SrcDataSize,
						Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter,
						ColorKey, pSrcInfo, pPalette, ppTexture));
	}

	DWORD dwWidth = pHeader->dwWidth;
	DWORD dwHeight = pHeader->dwHeight;
	DWORD dwDesWidth = dwWidth >> TextureLevel;
	DWORD dwDesHeight = dwHeight >> TextureLevel;

	if (dwDesWidth <= 0)
		dwDesWidth = 1;
	if (dwDesHeight <= 0)
		dwDesHeight = 1;

	if (dwDesWidth != Width || dwDesHeight != Height)
		return false;

	DWORD dwDesLevel = TextureLevel;
	DWORD dwMipMapCount = pHeader->dwMipMapCount;
	if (dwDesLevel >= dwMipMapCount - 1)
		dwDesLevel = dwMipMapCount - 1;

	//	Skip frontal TextureLevel mipmap LODs
	DWORD dwSkipDataSize = 0;
 	for (DWORD i=0; i < dwDesLevel; i++)
	{
		DWORD dwSurfBytes = 0;
		A3DDDSFile::GetSurfaceInfo(dwWidth, dwHeight, fmtInner, &dwSurfBytes, NULL, NULL);
		dwSkipDataSize += dwSurfBytes;
		dwWidth >>= 1;
		dwHeight >>= 1;
	}

	DWORD dwHeadSize = sizeof (DWORD) + pHeader->dwSize;
	if (dwSkipDataSize + dwHeadSize > SrcDataSize)
		return false;

	//	Allocate a temporary buffer
	DWORD dwNewFileSize = SrcDataSize - dwSkipDataSize;
	BYTE* buf = new BYTE [dwNewFileSize];
	if (!buf)
		return false;

	//	Build new dds file data with new mipmap levels
	memcpy(buf, pSrcData, dwHeadSize);
	memcpy(buf+dwHeadSize, pBitData+dwSkipDataSize, dwNewFileSize-dwHeadSize);

	DDS_HEADER* pNewHeader = (DDS_HEADER*)(buf + sizeof (DWORD));
	pNewHeader->dwWidth = dwWidth;
	pNewHeader->dwHeight = dwHeight;
	pNewHeader->dwMipMapCount = dwMipMapCount - dwDesLevel;

	DWORD dwSurfBytes=0, dwRowBytes=0;
	A3DDDSFile::GetSurfaceInfo(dwWidth, dwHeight, fmtInner, &dwSurfBytes, &dwRowBytes, NULL);

	if (pNewHeader->dwHeaderFlags & DDSD_PITCH)
		pNewHeader->dwPitchOrLinearSize = dwRowBytes;
	else if (pNewHeader->dwHeaderFlags & DDSD_LINEARSIZE)
		pNewHeader->dwPitchOrLinearSize = dwSurfBytes;
	
	HRESULT ret = D3DXCreateTextureFromFileInMemoryEx(pDevice, buf, dwNewFileSize,
						dwDesWidth, dwDesHeight, MipLevels, Usage, Format, Pool, Filter, MipFilter,
						ColorKey, pSrcInfo, pPalette, ppTexture);
	delete [] buf;

	return SUCCEEDED(ret);
}

// This function load the texture as a normal texture;
// NOTE: we haven't set the m_nMipLevel variable correctly.
bool A3DTexture::LoadNormalTexture(BYTE* pDataBuf, int iDataLen, const char* szRelativePath)
{
	D3DXIMAGE_INFO  image_info;
	int				mip_level = 0;
	HRESULT			hval;
	
	A3DFORMAT fmtTarget;
	
	if (m_bDDSTexture)
		fmtTarget = (A3DFORMAT) D3DFMT_UNKNOWN;
	else if (!m_bAlphaTexture)
		fmtTarget = m_pA3DDevice->GetNormalTextureFormat();
	else
		fmtTarget = m_pA3DDevice->GetAlphaTextureFormat();

	hval = D3DXGetImageInfoFromFileInMemory(pDataBuf, iDataLen, &image_info);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DTexture::LoadNormalTexture() Can't get image's info: %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	//	Alpha flag of .dds file must be determined from the file
	if (m_bDDSTexture)
	{
		if (image_info.Format == D3DFMT_A8R8G8B8 || image_info.Format == D3DFMT_A4R4G4B4 ||
			image_info.Format == D3DFMT_DXT3 || image_info.Format == D3DFMT_DXT5)
			m_bAlphaTexture = true;

		// use mip level direct from the file
		mip_level = image_info.MipLevels;
	}

	DWORD	dwDownrate = 1;
	int		nSkip = 0;
	if( !m_bNoDownSample )
	{
		if( g_pA3DConfig->GetTextureQuality() == A3DTEXTURE_QUALITY_NULL )
		{
			if (!g_pA3DConfig->GetFlagHQTexture())
			{
				dwDownrate = 2;
				mip_level -= 1;
			}
		}
		else
		{
			if( g_pA3DConfig->GetTextureQuality() == A3DTEXTURE_QUALITY_MEDIUM )
			{
				dwDownrate = 2;
				mip_level -= 1;
				nSkip = 1;
			}
			else if( g_pA3DConfig->GetTextureQuality() == A3DTEXTURE_QUALITY_LOW )
			{
				dwDownrate = 4;
				mip_level -= 2;
				nSkip = 2;
			}
		}
		if( mip_level < 0 )
		{
			if(m_bDDSTexture)
				mip_level = 1;
			else
				mip_level = 0;
		}
	}

	if (m_bDDSTexture)
	{
		// check if the dds texture format is supported!
		HRESULT hval;
		hval = m_pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (D3DFORMAT)m_pA3DDevice->GetDevFormat().fmtAdapter, 0, D3DRTYPE_TEXTURE, image_info.Format);
		if( D3D_OK != hval )
		{
			// dds not supported, so convert into supported format
			if (!m_bAlphaTexture)
				fmtTarget = m_pA3DDevice->GetNormalTextureFormat();
			else
				fmtTarget = m_pA3DDevice->GetAlphaTextureFormat();

			if( !m_bNoDownSample )
			{
				dwDownrate = 4;
				mip_level -= 2;
				nSkip = 2;
				if( mip_level < 0 ) mip_level = 1;
			}
		}
	}

	D3DPOOL Pool;
	m_dwUsage = 0;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;

#ifndef _UNICODE
		m_dwUsage = D3DUSAGE_DYNAMIC;
#endif
	}
	else
		Pool = D3DPOOL_MANAGED;


	if( dwDownrate > 1 && image_info.Width > dwDownrate && image_info.Height > dwDownrate )
	{
		UINT width, height;
		width = image_info.Width / dwDownrate;
		height = image_info.Height / dwDownrate;
		// Now we should load half size textures;
		if( width < 1 ) width = 1;
		if( height < 1 ) height = 1;
		if( m_bDDSTexture )
		{
			// we use our dds read function to skip first x level directly
			if( !CreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pDataBuf, iDataLen, width, height, mip_level, m_dwUsage, (D3DFORMAT) fmtTarget,
				Pool, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXTexture, nSkip) )
			{
				g_A3DErrLog.Log("A3DTexture::LoadNormalTexture(), Can't load %s!", szRelativePath);
				m_pDXTexture = NULL;
				return true;
			}
		}
		else
		{
			hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pDataBuf, iDataLen, width, height, mip_level, m_dwUsage, (D3DFORMAT) fmtTarget,
				Pool, D3DX_FILTER_BOX, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXTexture);
			if( D3D_OK != hval )
			{
				g_A3DErrLog.Log("A3DTexture::LoadNormalTexture(), Can't load %s!", szRelativePath);
				m_pDXTexture = NULL;
				return true;
			}
		}
	}
	else
	{
		UINT width = image_info.Width < 1 ? 1 : image_info.Width;
		UINT height = image_info.Height < 1 ? 1 : image_info.Height;
		//	D3DX_DEFAULT instead of width/height will slow the game
		hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pDataBuf, iDataLen, width, height, mip_level, m_dwUsage, (D3DFORMAT) fmtTarget,
			Pool, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXTexture);
		if( D3D_OK != hval )
		{
			g_A3DErrLog.Log("A3DTexture::LoadNormalTexture() Can't load %s!", szRelativePath);
			m_pDXTexture = NULL;
			return true;
		}
	}

	m_nMipLevel = image_info.MipLevels;

	CalcTextureDataSize();

	return true;
}

bool A3DTexture::LoadDetailTexture(BYTE* pDataBuf, int iDataLen, const char* szRelativePath)
{
	IDirect3DTexture9	*pDXTexture;
	D3DXIMAGE_INFO		image_info;
	HRESULT				hval;

	m_dwUsage = 0;
	D3DPOOL Pool;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;
		m_dwUsage = D3DUSAGE_DYNAMIC;
	}
	else
		Pool = D3DPOOL_MANAGED;

	hval = D3DXGetImageInfoFromFileInMemory(pDataBuf, iDataLen, &image_info);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DTexture::LoadNormalTexture() Can't get image's info: %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	UINT width = image_info.Width < 1 ? 1 : image_info.Width;
	UINT height = image_info.Height < 1 ? 1 :image_info.Height;

	// For detail texture, we always create A3DFMT_X8R8G8B8 (or A3DFMT_A8R8G8B8 for voodoo5) format texture;
	hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pDataBuf, iDataLen, width, height, 1, m_dwUsage, D3DFMT_UNKNOWN,
		Pool, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &pDXTexture);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DTexture::LoadDetailTexture() Can't load %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	D3DSURFACE_DESC	desc;
	pDXTexture->GetLevelDesc(0, &desc);

	if( desc.Format != D3DFMT_X8R8G8B8 && desc.Format != D3DFMT_A8R8G8B8 && desc.Format != D3DFMT_R5G6B5 &&
		desc.Format != D3DFMT_X1R5G5B5 )
	{
		pDXTexture->Release();
		g_A3DErrLog.Log("A3DTexture::LoadDetailTexture(), Texture format unsupported!");
		return false;
	}

	width = desc.Width;
	height = desc.Height;

	// Create a mip map chain, the more deep the level are, the more far the clear range is
	// this max depth should be determined by texture size and the desired max clear distance
	int nMipLevel = GetMaxMipLevel(width, height);

	if( nMipLevel > 7 ) nMipLevel = 7;

	m_nMipLevel = nMipLevel;

	A3DFORMAT texFormat;
	if( desc.Format == D3DFMT_A8R8G8B8 ) // it must be voodoo5
		texFormat = A3DFMT_A8R8G8B8;
	else
		texFormat = A3DFMT_X8R8G8B8;
	if( !CreateDXTexture(width, height, texFormat, nMipLevel) )
	{
		pDXTexture->Release();
		g_A3DErrLog.Log("A3DTexture::LoadDetailTexture(), Can not create mip map chain!");
		return false;
	}
	
	D3DLOCKED_RECT  LockedRectOrg;
	RECT			rectOrg;
	LPBYTE			pDataOrg;
	int				nPitchOrg;
	SetRect(&rectOrg, 0, 0, width, height);

	if (FAILED(pDXTexture->LockRect(0, &LockedRectOrg, &rectOrg, 0)))
	{
		pDXTexture->Release();
		g_A3DErrLog.Log("A3DTexture::LoadDetailTexture(), Lock original texture fail!");
		return false;
	}
	pDataOrg = (LPBYTE) LockedRectOrg.pBits;
	nPitchOrg = LockedRectOrg.Pitch;

	D3DLOCKED_RECT  LockedRectMap;
	RECT			rectMap;
	LPBYTE			pDataMap;
	int				nPitchMap;
	SetRect(&rectMap, 0, 0, width, height);

	int nSubSize = 1;
	for(int idLevel=0; idLevel<m_nMipLevel; idLevel ++)
	{
		if (FAILED(m_pDXTexture->LockRect(idLevel, &LockedRectMap, &rectMap, 0)))
		{
			pDXTexture->Release();
			g_A3DErrLog.Log("A3DTexture::LoadDetailTexture(), Lock mipmap texture fail!");
			return false;
		}
		pDataMap = (LPBYTE) LockedRectMap.pBits;
		nPitchMap = LockedRectMap.Pitch;
	
		for(int x=0; x<rectMap.right; x++)
		{
			for(int y=0; y<rectMap.bottom; y++)
			{
				DWORD * pColor = (DWORD *) (pDataMap + nPitchMap * y + x * 4);
				int		nOX, nOY;
				int		r=0, g=0, b=0;

				if( idLevel == m_nMipLevel - 1 )
				{
					*pColor = A3DCOLORRGBA(128, 128, 128, 255);
					//*pColor = A3DCOLORRGBA(0, 0, 0, 255);
				}
				else
				{
					// we use a box resample algorithm;
					nOY = y * nSubSize;
					for(int sy=0; sy<nSubSize; sy++)
					{
						nOX = x * nSubSize;
						for(int sx=0; sx<nSubSize; sx++)
						{
							switch(desc.Format)
							{
							case D3DFMT_X1R5G5B5:
								WORD colorOrg1;
								colorOrg1 = *(WORD *) (pDataOrg + nPitchOrg * nOY + nOX * 2);
								r += (colorOrg1 & 0x7c00) >> 10 << 3;
								g += (colorOrg1 & 0x03e0) >> 5 << 3;
								b += (colorOrg1 & 0x1f) << 3;
								break;

							case D3DFMT_R5G6B5:
								WORD colorOrg2;
								colorOrg2 = *(WORD *) (pDataOrg + nPitchOrg * nOY + nOX * 2);
								r += (colorOrg2 & 0x8f00) >> 11 << 3;
								g += (colorOrg2 & 0x07e0) >> 5 << 3;
								b += (colorOrg2 & 0x1f) << 3;
								break;

							case D3DFMT_X8R8G8B8:
							case D3DFMT_A8R8G8B8:
								DWORD colorOrg3;
								colorOrg3 = *(DWORD *) (pDataOrg + nPitchOrg * nOY + nOX * 4);
								r += (colorOrg3 & 0x00ff0000) >> 16;
								g += (colorOrg3 & 0xff00) >> 8;
								b += colorOrg3 & 0xff;
								break;
							default:
								break;
							}
							nOX ++;
						}
						nOY ++;
					}
					r /= (nSubSize * nSubSize);
					g /= (nSubSize * nSubSize);
					b /= (nSubSize * nSubSize);
					*pColor = A3DCOLORRGBA(r, g, b, 255);
				}
			}
		}

		m_pDXTexture->UnlockRect(idLevel);

		rectMap.right /= 2;
		rectMap.bottom /= 2;
		nSubSize *= 2;
	}
	
	pDXTexture->UnlockRect(0);
	pDXTexture->Release();
	return true;
}

// When loading a lightmap, we did not do any mip-maping
bool A3DTexture::LoadLightMapTexture(BYTE* pDataBuf, int iDataLen, const char* szRelativePath)
{
	D3DXIMAGE_INFO  image_info;
	HRESULT			hval;

	D3DPOOL Pool;
	m_dwUsage = 0;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;

#ifndef UNICODE
		m_dwUsage = D3DUSAGE_DYNAMIC;
#endif
	}
	else
		Pool = D3DPOOL_MANAGED;

	hval = D3DXGetImageInfoFromFileInMemory(pDataBuf, iDataLen, &image_info);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DTexture::LoadNormalTexture() Can't get image's info: %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	UINT width = image_info.Width < 1 ? 1 : image_info.Width;
	UINT height = image_info.Height < 1 ? 1 :image_info.Height;

	m_nMipLevel = 1;
	A3DFORMAT fmtTarget;
	fmtTarget = m_pA3DDevice->GetNormalTextureFormat();
	
	hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pDataBuf, iDataLen, width, height, 1, m_dwUsage, (D3DFORMAT) fmtTarget,
		Pool, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXTexture);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DTexture::LoadNormalTexture() Can't load %s!", szRelativePath);
		m_pDXTexture = NULL;
		return true;
	}

	CalcTextureDataSize();

	return true;
}

/*	Load texture file with specified size and format

	Return true for success, otherwise return false.

	pA3DDevice: device object
	szFile: texture file name, this can be relative file name or absolute path name
	iWidth, iHeight: size of texture, 0 means to use texture's original size
	Fmt: desired texture format. A3DFMT_UNKNOWN means format is decided by file format
	iMipLevel: desired texture mipmap level.
	aPalEntries (out): If texture is loaded as indexed color texture, this buffer used
		to receive palette information. Can be NULL.
*/
bool A3DTexture::LoadFromFile(A3DDevice* pA3DDevice, const char* szFile, int iWidth, int iHeight, A3DFORMAT Fmt, 
							  int iMipLevel/* 1 */, PALETTEENTRY* aPalEntries/* NULL */)
{
	m_nDesiredMipLevel	= iMipLevel;
	m_pA3DDevice		= pA3DDevice;

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER || g_pA3DConfig->GetFlagNoTextures())
	{
		af_GetRelativePath(szFile, "", m_strMapFile);
		m_dwTextureID	= a_MakeIDFromLowString(m_strMapFile);
		m_bHWITexture	= true;
		return true;
	}

	if (!m_pA3DDevice)
	{
		af_GetRelativePath(szFile, "", m_strMapFile);
		m_dwTextureID	= a_MakeIDFromLowString(m_strMapFile);
		return true;
	}

	AFileImage fileImage;

	if (!fileImage.Open("", szFile, AFILE_OPENEXIST | AFILE_TEMPMEMORY))
	{
		g_A3DErrLog.Log("A3DTexture::LoadFromFile, Cann't open texture file [%s]!", szFile);
		m_pDXTexture = NULL;
		return true;
	}

	m_strMapFile	= fileImage.GetRelativeName();
	m_dwTextureID	= a_MakeIDFromLowString(m_strMapFile);

	D3DXIMAGE_INFO image_info;
	D3DFORMAT D3DFmt = (D3DFORMAT)Fmt;

		D3DPOOL Pool;
	m_dwUsage = 0;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;

#ifndef UNICODE
		m_dwUsage = D3DUSAGE_DYNAMIC;
#endif
	}
	else
		Pool = D3DPOOL_MANAGED;

	HRESULT hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), 
		fileImage.GetFileBuffer(), fileImage.GetFileLength(), iWidth, iHeight, 
		m_nDesiredMipLevel, m_dwUsage, D3DFmt, Pool, 
		D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, aPalEntries, &m_pDXTexture);

	fileImage.Close();

	if (D3D_OK != hval)
	{
		g_A3DErrLog.Log("A3DTexture::LoadFromFile, Can't load [%s] !", szFile);
		m_pDXTexture = NULL;
		return true;
	}

	D3DSURFACE_DESC Desc;
	if (Fmt != A3DFMT_UNKNOWN && SUCCEEDED(m_pDXTexture->GetLevelDesc(0, &Desc)))
	{
		if (D3DFmt != Desc.Format)
			g_A3DErrLog.Log("A3DTexture::LoadFromFile, Texture [%s] desired format [%d] but was given [%d]!", szFile, Fmt, Desc.Format);
	}

	m_nMipLevel = image_info.MipLevels;

	//	Decide texture properties
	switch (image_info.Format)
	{
	case D3DFMT_A8R8G8B8:
    case D3DFMT_R5G6B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_A8:
    case D3DFMT_A8R3G3B2:
    case D3DFMT_A8P8:
    case D3DFMT_A8L8:
    case D3DFMT_A4L4:

		m_bAlphaTexture = true;
		break;

	case D3DFMT_DXT1:

		m_bDDSTexture	= true;
		break;

	case D3DFMT_DXT3:
	case D3DFMT_DXT5:

		m_bAlphaTexture = true;
		m_bDDSTexture	= true;
		break;
	}

	CalcTextureDataSize();

	return true;
}

//	Load texture form memory with specified size and format
bool A3DTexture::LoadFromMemory(A3DDevice* pA3DDevice, BYTE* pDataBuf, int iDataSize,
					int iWidth, int iHeight, A3DFORMAT Fmt, int iMipLevel/* 1 */, PALETTEENTRY* aPalEntries/* NULL */)
{
	m_nDesiredMipLevel	= iMipLevel;
	m_pA3DDevice		= pA3DDevice;
	m_dwTextureID		= 0;

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER || g_pA3DConfig->GetFlagNoTextures())
	{
		m_bHWITexture = true;
		return true;
	}

	if (!m_pA3DDevice)
		return true;

	D3DXIMAGE_INFO image_info;
	D3DFORMAT D3DFmt = (D3DFORMAT)Fmt;

	D3DPOOL Pool;
	m_dwUsage = 0;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;

#ifndef UNICODE
		m_dwUsage = D3DUSAGE_DYNAMIC;
#endif
	}
	else
		Pool = D3DPOOL_MANAGED;

	HRESULT hval = D3DXCreateTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), 
		pDataBuf, iDataSize, iWidth, iHeight, m_nDesiredMipLevel, m_dwUsage, 
		D3DFmt, Pool, D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, 
		aPalEntries, &m_pDXTexture);

	if (D3D_OK != hval)
	{
		g_A3DErrLog.Log("A3DTexture::LoadFromMemory, Failed to call D3DXCreateTextureFromFileInMemoryEx() !");
		m_pDXTexture = NULL;
		return true;
	}

	D3DSURFACE_DESC Desc;
	if (Fmt != A3DFMT_UNKNOWN && SUCCEEDED(m_pDXTexture->GetLevelDesc(0, &Desc)))
	{
		if (D3DFmt != Desc.Format)
			g_A3DErrLog.Log("A3DTexture::LoadFromMemory, Texture desired format [%d] but was given [%d]!", Fmt, Desc.Format);
	}

	m_nMipLevel = image_info.MipLevels;

	//	Decide texture properties
	switch (image_info.Format)
	{
	case D3DFMT_A8R8G8B8:
    case D3DFMT_R5G6B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_A8:
    case D3DFMT_A8R3G3B2:
    case D3DFMT_A8P8:
    case D3DFMT_A8L8:
    case D3DFMT_A4L4:

		m_bAlphaTexture = true;
		break;

	case D3DFMT_DXT1:

		m_bDDSTexture	= true;
		break;

	case D3DFMT_DXT3:
	case D3DFMT_DXT5:

		m_bAlphaTexture = true;
		m_bDDSTexture	= true;
		break;
	}

	CalcTextureDataSize();

	return true;
}

bool A3DTexture::Reload(bool bForceReload)
{
	// check time stamp here
	if( !bForceReload )
	{
		struct stat fileStat;
		stat(m_strMapFile, &fileStat);
		if( m_dwTimeStamp == fileStat.st_mtime )	
			return true;
	}

	if (m_pDXTexture)
	{
		m_pDXTexture->Release();
		m_pDXTexture = NULL;
	}

	AFileImage fileImage;	
	if( !fileImage.Open(m_strMapFile, AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
	{
#ifdef _DEBUG
		g_A3DErrLog.Log("A3DTexture::Reload(), Can not init the file to be a fileimage [%s]!", m_strMapFile);
#endif
		m_pDXTexture = NULL;
		return true;
	}

	if (m_bDetailTexture) 
	{
		if (!LoadDetailTexture(fileImage.GetFileBuffer(), fileImage.GetFileLength(), m_strMapFile))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DTexture::Reload(), call LoadDetailTexture() fail!");
			return false;
		}
	}
	else if (m_bLightMap)
	{
		if (!LoadLightMapTexture(fileImage.GetFileBuffer(), fileImage.GetFileLength(), m_strMapFile))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DTexture::Reload(), call LoadLightMapTexture() fail!");
			return false;
		}
	}
	else
	{
		if (!LoadNormalTexture(fileImage.GetFileBuffer(), fileImage.GetFileLength(), m_strMapFile))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DTexture::Reload(), call LoadNormalTexture() fail!");
			return false;
		}
	}

	m_dwTimeStamp = fileImage.GetTimeStamp();
	fileImage.Close();
	return true;
}

void A3DTexture::GetMapFileInFolder(const char* szFolderName, AString& strName)
{
	af_GetRelativePathNoBase(m_strMapFile, szFolderName, strName);
}

//	Get texture dimension
bool A3DTexture::GetDimension(int* piWidth, int* piHeight, int iLevel/* 0 */)
{
	if (!m_pDXTexture)
		return false;

	D3DSURFACE_DESC Desc;
	if (FAILED(m_pDXTexture->GetLevelDesc(iLevel, &Desc)))
	{
		g_A3DErrLog.Log("A3DTexture::GetSize, Failed to get level %d desc", iLevel);
		return false;
	}

	if (piWidth)
		*piWidth = (int)Desc.Width;

	if (piHeight)
		*piHeight = (int)Desc.Height;

	return true;
}

//	Get texture format
A3DFORMAT A3DTexture::GetFormat(int iLevel/* 0 */)
{
	if (!m_pDXTexture)
		return A3DFMT_UNKNOWN;

	D3DSURFACE_DESC Desc;
	if (FAILED(m_pDXTexture->GetLevelDesc(iLevel, &Desc)))
	{
		g_A3DErrLog.Log("A3DTexture::GetSize, Failed to get level %d desc", iLevel);
		return A3DFMT_UNKNOWN;
	}

	return (A3DFORMAT)Desc.Format;
}

void A3DTexture::CalcTextureDataSize()
{
    if (!m_pDXTexture || !m_pA3DDevice)
        return;

    int iLevelCnt = m_pDXTexture->GetLevelCount();
#ifdef _DEBUG

	if (m_pA3DDevice && m_iTexDataSize)
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(-m_iTexDataSize);

	m_iTexDataSize = 0;

	for (int i=0; i < iLevelCnt; i++)
	{
		D3DSURFACE_DESC	desc;
		m_pDXTexture->GetLevelDesc(i, &desc);
		//	m_iTexDataSize += (int)desc.Size;
		int nPitch = (desc.Width * 
			A3DDevice::BitsOfDeviceFormat((A3DFORMAT)desc.Format)) / 8;

		m_iTexDataSize += (desc.Height * nPitch);
	}

	m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(m_iTexDataSize);

#endif	//	_DEBUG
    for (int i=0; i < iLevelCnt; i++)
    {
        D3DSURFACE_DESC	desc;
        m_pDXTexture->GetLevelDesc(i, &desc);

        if (i == 0)
        {
            m_nWidth = desc.Width;
            m_nHeight = desc.Height;
            m_dwBitsPerPixel = A3DDevice::BitsOfDeviceFormat((A3DFORMAT)desc.Format);
        }
    }

}

//	Create a pure color texture whose format is A3DFMT_A8R8G8B8
A3DTexture* A3DTexture::CreateColorTexture(A3DDevice* pDevice, int iWidth, int iHeight, A3DCOLOR col)
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return NULL;

	if (!pTexture->Create(pDevice, iWidth, iHeight, A3DFMT_A8R8G8B8, 1))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTexture::CreateColorTexture, Failed to create texture.");
		return NULL;
	}

	//	Lock texture
	BYTE* pDstData = NULL;
	int iDstPitch;
	if (!pTexture->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
	{
		delete pTexture;
		g_A3DErrLog.Log("A3DTexture::CreateColorTexture, Failed to lock texture");
		return NULL;
	}

	//	Fill texture data
	BYTE* pDstLine = pDstData;
	for (int i=0; i < iHeight; i++)
	{
		DWORD* pDst = (DWORD*)pDstLine;
		pDstLine += iDstPitch;

		for (int j=0; j < iWidth; j++)
			*pDst++ = col;
	}

	pTexture->UnlockRect();

	return pTexture;
}

IDirect3DBaseTexture9* A3DTexture::GetD3DBaseTexture() const
{
	return m_pDXTexture;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DTextureProxy
//
///////////////////////////////////////////////////////////////////////////

A3DTextureProxy::A3DTextureProxy()
{
	m_pA3DDevice = NULL;
	m_pDXTexture = NULL;
}

A3DTextureProxy::A3DTextureProxy(A3DEngine* pA3DEngine, IDirect3DTexture9* pDXTexture)
{ 
	ASSERT(pA3DEngine);
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	m_pDXTexture = pDXTexture; 
}

IDirect3DTexture9* A3DTextureProxy::SetDXTexture(IDirect3DTexture9* pDXTexture)
{
	IDirect3DTexture9* pOld = m_pDXTexture;
	m_pDXTexture = pDXTexture;
	return pOld;
}

//	Apply texture to device
bool A3DTextureProxy::Appear(int iLayer)
{
	if (!m_pA3DDevice || !m_pDXTexture)
		return true;

	m_pA3DDevice->raw_SetTexture(iLayer, m_pDXTexture);
	return true;
}


bool A3DTexture::LockRectDynamic(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel)
{
    if (!m_pDXTexture || m_bHWITexture)
        return false;

    if( nMipLevel >= m_nMipLevel )
    {
        g_A3DErrLog.Log("A3DTexture::LockRectDynamic(), mip level [%d] larger than m_nMipLevel [%d]", nMipLevel, m_nMipLevel);
        return false;
    }

    DWORD dwWidth = pRect ? pRect->right - pRect->left : m_nWidth;
    DWORD dwHeight = pRect ? pRect->bottom - pRect->top : m_nHeight;

    DWORD dwPitch = dwWidth * m_dwBitsPerPixel / 8;
    //temp
    if (FAILED(m_pA3DDevice->LockTexture(m_pDXTexture, nMipLevel, ppData, dwPitch, dwHeight, pRect, dwFlags, iPitch)))
        return false;

    return true;
}

bool A3DTexture::UnlockRectDynamic()
{
    if (!m_pDXTexture || m_bHWITexture)
        return false;

    if (FAILED(m_pA3DDevice->UnlockTexture(m_pDXTexture, 0)))
        return false;

    return true;
}

bool A3DTexture::UpdateFromSurfaceDynamic(A3DSurface * pA3DSurface, A3DRECT & rect)
{
    if (!m_pDXTexture || !pA3DSurface)
        return false;

    IDirect3DSurface9* pDstSurface = NULL;
    if (FAILED(m_pDXTexture->GetSurfaceLevel(0, &pDstSurface)))
        return false;

    POINT ptDest;
    ptDest.x = ptDest.y = 0;

    RECT rectTex;
    rectTex.left = ptDest.x; rectTex.right = rectTex.left + rect.Width();
    rectTex.top  = ptDest.y; rectTex.bottom = rectTex.top + rect.Height();

    if (FAILED(m_pA3DDevice->CopySurface(pDstSurface, &rectTex, pA3DSurface->GetDXSurface(), &rectTex, D3DX_FILTER_NONE)))
    {
        pDstSurface->Release();
        return false; 
    }

    pDstSurface->Release();
    return true;
}