/*
 * FILE: A3DVolumeTexture.cpp
 *
 * DESCRIPTION: Volume Texture
 *
 * CREATED BY: Hedi, 2005/11/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DVolumeTexture.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DPI.h"
#include "A3DConfig.h"
#include "A3DTextureMan.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DDDSFile.h"
#include "AFileImage.h"
#include "AFI.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DCubeTexture
//
///////////////////////////////////////////////////////////////////////////

A3DVolumeTexture::A3DVolumeTexture() : A3DTexture()
{
	m_dwClassID        = A3D_CID_TEXTURE3D;
	m_pDXVolumeTexture = NULL;
}

A3DVolumeTexture::~A3DVolumeTexture()
{
}

bool A3DVolumeTexture::Init(A3DDevice * pDevice)
{
	if( !A3DTexture::Init(pDevice) )
		return false;

	m_pDXVolumeTexture = NULL;

	return true;
}

bool A3DVolumeTexture::Reload(bool bForceReload)
{
	return true; // 没实现!
}

bool A3DVolumeTexture::Release()
{
	if (m_pDXVolumeTexture)
	{
		if (m_pA3DDevice)
		{
			m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_TEXTURE);
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(-m_iTexDataSize);
		}
		
		if( m_pA3DDevice->GetNeedResetFlag())
		{
			m_pDXVolumeTexture->Release();
		}
		else
		{
			m_pA3DDevice->ReleaseResource(m_pDXVolumeTexture);// multiThread render
		}
		m_pDXVolumeTexture = NULL;
	}

	m_pA3DDevice = NULL;
	return true;
}

bool A3DVolumeTexture::Load(AFile* pFile)
{
	if(pFile == NULL)
		return false;

	DWORD dwReadSize;
	DWORD dwSize = pFile->GetFileLength();
	if(dwSize == 0)
	{
		g_A3DErrLog.Log("A3DVolumeTexture::Load, the texture's size is 0.");
		return false;
	}
	BYTE* pData = new BYTE[dwSize];
	pFile->Read(pData, dwSize, &dwReadSize);

	ASSERT(m_pDXVolumeTexture == NULL);

	D3DPOOL Pool = m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag()? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	HRESULT hr = D3DXCreateVolumeTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pData,
				dwSize, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, D3DFMT_UNKNOWN, Pool,
				D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &m_pDXVolumeTexture);
	if( FAILED(hr))
	{
		g_A3DErrLog.Log("A3DVolumeTexture::Load, Failed to Create Volume! FileName:%s", pFile->GetFileName());
		A3DINTERFACERELEASE(m_pDXVolumeTexture);
		return false;
	}
	
	//注意：intel 946GZ，G31,G33等垃圾显卡根本不支持VolumeTexture却依然返回成功
	//需要校验纹理height,width,depth
	if(!IsValidVolumeTexture(m_pDXVolumeTexture))
	{
		g_A3DErrLog.Log("A3DVolumeTexture::Load, Failed to Create Volume! FileName:%s", pFile->GetFileName());
		A3DINTERFACERELEASE(m_pDXVolumeTexture);
		return false;
	}

	m_iTexDataSize = 0;
	m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_TEXTURE);
	m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(m_iTexDataSize);

	if(pData != NULL)
	{
		delete[] pData;
		pData = NULL;
	}
	return true;
}

bool A3DVolumeTexture::Load(const char* szFile)
{
	AFileImage File;

	af_GetRelativePath(szFile, "", m_strMapFile);
	m_dwTextureID = a_MakeIDFromLowString(m_strMapFile);

	if(File.Open(szFile, AFILE_OPENEXIST | AFILE_TEMPMEMORY) == false)
	{
		g_A3DErrLog.Log("A3DVolumeTexture::Load, load [%s] failed.", szFile);
		return false;
	}
	return Load(&File);
}

bool A3DVolumeTexture::Appear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	if( m_pDXVolumeTexture )
		m_pA3DDevice->raw_SetTexture(nLayer, m_pDXVolumeTexture);
	else
		m_pA3DDevice->raw_SetTexture(nLayer, NULL);

	return true;
}

bool A3DVolumeTexture::Disappear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	if( m_pDXVolumeTexture )
		m_pA3DDevice->raw_SetTexture(nLayer, NULL);

	return true;
}

void A3DVolumeTexture::CalcTextureDataSize()
{
	m_iTexDataSize = 0;

	if (!m_pDXVolumeTexture || !m_pA3DDevice)
		return;
	
	int iLevelCnt = m_pDXVolumeTexture->GetLevelCount();

	for (int i=0; i < iLevelCnt; i++)
	{
		D3DVOLUME_DESC	desc;
		m_pDXVolumeTexture->GetLevelDesc(i, &desc);

		int nPitch = (desc.Width * 
			A3DDDSFile::BitsPerPixel(desc.Format)) / 8;

		m_iTexDataSize += (nPitch * desc.Height * desc.Depth);
	}

	m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(m_iTexDataSize);
	return;
}

IDirect3DBaseTexture9* A3DVolumeTexture::GetD3DBaseTexture() const
{
	return m_pDXVolumeTexture;
}

bool A3DVolumeTexture::LoadTextureFromBuffer(BYTE* pDataBuf, int iDataLen, DWORD dwTexFlags)
{
	D3DXIMAGE_INFO  image_info;
	int				mip_level = 0;
	HRESULT			hval;

	D3DPOOL Pool = m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag()? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	hval = D3DXGetImageInfoFromFileInMemory(pDataBuf, iDataLen, &image_info);
	if( D3D_OK != hval )
	{
		g_A3DErrLog.Log("A3DVolumeTexture::LoadTextureFromBuffer() Can't get image's info: %s!", m_strMapFile);
		m_pDXTexture = NULL;
		return true;
	}

/*	if (image_info.ResourceType != A3DRTYPE_VOLUMETEXTURE)
	{
		ASSERT(image_info.ResourceType == A3DRTYPE_VOLUMETEXTURE);
		g_A3DErrLog.Log("A3DVolumeTexture::LoadTextureFromBuffer(), %s isn't a volume texture !", m_strMapFile);
		return false;
	}

	m_bDDSTexture = IsDDSFormat((A3DFORMAT)image_info.Format);
*/
	//	Determine texture quality
	DWORD dwDownrate = 1;
	int nSkip = 0;
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
		if( mip_level < 0 ) mip_level = 1;
	}

	if (dwDownrate > 1 && image_info.Width > dwDownrate && image_info.Height > dwDownrate)
	{
		// Now we should load half size textures;
		UINT width, height;
		UINT depth = image_info.Depth;
		width = image_info.Width / dwDownrate;
		height = image_info.Height / dwDownrate;
		if( width < 1 ) width = 1;
		if( height < 1 ) height = 1;
		
		
		hval = D3DXCreateVolumeTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pDataBuf, iDataLen,
						width, height, depth, mip_level, 0, D3DFMT_UNKNOWN, Pool, D3DX_FILTER_BOX,
						D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXVolumeTexture);

		if( FAILED(hval) )
		{
			g_A3DErrLog.Log("A3DVolumeTexture::LoadTextureFromBuffer(), Can't load %s!", m_strMapFile);
			A3DINTERFACERELEASE(m_pDXVolumeTexture);
			return false;
		}

		//注意：intel 946GZ，G31,G33等垃圾显卡根本不支持VolumeTexture却依然返回成功
		//需要校验纹理height,width,depth
		if(!IsValidVolumeTexture(m_pDXVolumeTexture))
		{
			g_A3DErrLog.Log("A3DVolumeTexture::LoadTextureFromBuffer(), Can't load %s!", m_strMapFile);
			A3DINTERFACERELEASE(m_pDXVolumeTexture);
			return false;
		}
	}
	else
	{

		hval = D3DXCreateVolumeTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pDataBuf, iDataLen,
						D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, mip_level, 0, D3DFMT_UNKNOWN, Pool,
						D3DX_DEFAULT, D3DX_DEFAULT, 0, &image_info, NULL, &m_pDXVolumeTexture);
		if( FAILED(hval))
		{
			g_A3DErrLog.Log("A3DVolumeTexture::LoadTextureFromBuffer(), Can't load %s!", m_strMapFile);
			A3DINTERFACERELEASE(m_pDXVolumeTexture);
			return false;
		}
		
		//注意：intel 946GZ，G31,G33等垃圾显卡根本不支持VolumeTexture却依然返回成功
		//需要校验纹理height,width,depth
		if(!IsValidVolumeTexture(m_pDXVolumeTexture))
		{
			g_A3DErrLog.Log("A3DVolumeTexture::LoadTextureFromBuffer(), Can't load %s!", m_strMapFile);
			A3DINTERFACERELEASE(m_pDXVolumeTexture);
			return false;
		}
	}

	m_nMipLevel = mip_level;

	CalcTextureDataSize();
	return true;
}

bool A3DVolumeTexture::IsValidVolumeTexture(IDirect3DVolumeTexture9* pVolumeTex)
{
	if( !pVolumeTex)
		return false;
	D3DVOLUME_DESC desc;

	HRESULT hr = pVolumeTex->GetLevelDesc(0, &desc);
	if( FAILED(hr))
	{
		return false;
	}

	if(desc.Width != desc.Height
	 ||desc.Height != desc.Depth
	 ||desc.Depth != desc.Width)
	{
		return false;
	}

	return true;
}

bool A3DVolumeTexture::Create( A3DDevice * pDevice, int iWidth, int iHeight, int iDepth, A3DFORMAT Format, int nMipLevel/*=1*/ )
{
	if(!Init(pDevice))
		return false;

	D3DPOOL Pool;
	DWORD dwUsage = 0;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;
		dwUsage = D3DUSAGE_DYNAMIC;
	}
	else
		Pool = D3DPOOL_MANAGED;

	HRESULT hr = m_pA3DDevice->GetD3DDevice()->CreateVolumeTexture(iWidth, iHeight, iDepth, nMipLevel,
					dwUsage, (D3DFORMAT)Format, Pool, &m_pDXVolumeTexture, NULL);
	if( FAILED(hr))
	{
		g_A3DErrLog.Log("A3DVolumeTexture::Create, Failed to Create Volume Texture!");
		A3DINTERFACERELEASE(m_pDXVolumeTexture);
		return false;
	}
	
	//注意：intel 946GZ，G31,G33等垃圾显卡根本不支持VolumeTexture却依然返回成功
	//需要校验纹理height,width,depth
	if(!IsValidVolumeTexture(m_pDXVolumeTexture))
	{
		g_A3DErrLog.Log("A3DVolumeTexture::Create, Failed to Create Volume Texture! Intel Card!");
		A3DINTERFACERELEASE(m_pDXVolumeTexture);
		return false;
	}


	return true;
}

bool A3DVolumeTexture::LockBox( D3DBOX* pBox, void** ppData, int* iSlicePitch, int* iRowPitch, DWORD dwFlags, int nMipLevel/*=0*/ )
{
	D3DLOCKED_BOX lockedBox;
	if(FAILED(m_pDXVolumeTexture->LockBox(nMipLevel, &lockedBox, pBox, 0)))
		return false;
	*ppData = lockedBox.pBits;
	*iSlicePitch = lockedBox.SlicePitch;
	*iRowPitch = lockedBox.RowPitch;
	return true;
}

bool A3DVolumeTexture::UnlockBox(int nMipLevel)
{
	m_pDXVolumeTexture->UnlockBox(nMipLevel);
	return true;
}

bool A3DVolumeTexture::ExportToDDSFile( const char* szFileName )
{
	LPDIRECT3DVOLUME9 pVolume;
	m_pDXVolumeTexture->GetVolumeLevel(0, &pVolume);
	HRESULT r = D3DXSaveVolumeToFileA(szFileName, D3DXIFF_DDS, pVolume, NULL, NULL); 
	pVolume->Release();
	return true;
}

