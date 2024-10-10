/*
 * FILE: A3DCubeTexture.cpp
 *
 * DESCRIPTION: Cube Texture
 *
 * CREATED BY: Hedi, 2005/11/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DCubeTexture.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DPI.h"
#include "A3DConfig.h"
#include "A3DTextureMan.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
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

A3DCubeTexture::A3DCubeTexture() : A3DTexture()
{
	m_dwClassID      = A3D_CID_TEXTURECUBE;
	m_pDXCubeTexture = NULL;
}

A3DCubeTexture::~A3DCubeTexture()
{
}

bool A3DCubeTexture::Init(A3DDevice * pDevice)
{
	if( !A3DTexture::Init(pDevice) )
		return false;

	m_pDXCubeTexture = NULL;

	return true;
}

bool A3DCubeTexture::Release()
{
	if (m_pDXCubeTexture)
	{
		if (m_pA3DDevice)
		{
			m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_TEXTURE);
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(-m_iTexDataSize);
		}
		
		if(m_pA3DDevice->GetNeedResetFlag())
		{
			m_pDXCubeTexture->Release();
		}
		else
		{
			m_pA3DDevice->ReleaseResource(m_pDXCubeTexture);// multiThread render
		}
		m_pDXCubeTexture = NULL;
	}

	m_pA3DDevice = NULL;
	return true;
}

bool A3DCubeTexture::Reload(bool bForceReload)
{
	return true; // Ã»ÊµÏÖ!
}

bool A3DCubeTexture::LoadWithFormat(AFile * pFile, A3DFORMAT fmt)
{
    if(pFile == NULL)
        return false;

    DWORD dwReadSize;
    DWORD dwSize = pFile->GetFileLength();
    if(dwSize == 0)
    {
        g_A3DErrLog.Log("A3DCubeTexture::Load, the texture's size is 0.");
        return false;
    }
    BYTE* pData = new BYTE[dwSize];
    pFile->Read(pData, dwSize, &dwReadSize);

	ASSERT(m_pDXCubeTexture == NULL);

	D3DPOOL Pool = m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

    D3DXCreateCubeTextureFromFileInMemoryEx(m_pA3DDevice->GetD3DDevice(), pData,
        dwSize, D3DX_DEFAULT, 0, 0, (D3DFORMAT)fmt, Pool,
        D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &m_pDXCubeTexture);

    CalcTextureDataSize();
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

bool A3DCubeTexture::Load(AFile * pFile)
{
    return LoadWithFormat(pFile, A3DFMT_UNKNOWN);
}

bool A3DCubeTexture::Load(const char* szFile)
{
	AFileImage File;

	af_GetRelativePath(szFile, "", m_strMapFile);
	m_dwTextureID = a_MakeIDFromLowString(m_strMapFile);

	if(File.Open(szFile, AFILE_OPENEXIST | AFILE_TEMPMEMORY) == false)
	{
		g_A3DErrLog.Log("A3DCubeTexture::Load, load [%s] failed.", szFile);
		return false;
	}

	bool ret = Load(&File);

	File.Close();
	return ret;
}

bool A3DCubeTexture::Appear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	if( m_pDXCubeTexture )
		m_pA3DDevice->raw_SetTexture(nLayer, m_pDXCubeTexture);
	else
		m_pA3DDevice->raw_SetTexture(nLayer, NULL);

	return true;
}

bool A3DCubeTexture::Disappear(int nLayer)
{
	//If this object is created outside D3D;
	if( m_pA3DDevice == NULL || m_bHWITexture )
		return true;

	if( m_pDXCubeTexture )
		m_pA3DDevice->raw_SetTexture(nLayer, NULL);

	return true;
}

bool A3DCubeTexture::Create(A3DDevice * pDevice, int nEdgeSize, A3DFORMAT format, int nMipLevel)
{
	if (m_pDXCubeTexture)
	{
		g_A3DErrLog.Log("A3DCubeTexture::Create This texture has been created, can not recreated!");
		return false;
	}

	if (!Init(pDevice))
		return false;

	m_strMapFile	= "";
	m_nMipLevel		= nMipLevel;
	m_dwTextureID	= 0;

	int nMaxMipLevel = GetMaxMipLevel(nEdgeSize, nEdgeSize);
	if (m_nMipLevel <= 0 || m_nMipLevel > nMaxMipLevel) 
		m_nMipLevel = nMaxMipLevel;
	
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
		m_bHWITexture = true;
		return true;
	}

	D3DPOOL Pool;
	DWORD dwUsage = 0;

	if (m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
	{
		Pool = D3DPOOL_DEFAULT;
#ifndef UNICODE
		dwUsage = D3DUSAGE_DYNAMIC;
#endif 
	}
	else
		Pool = D3DPOOL_MANAGED;

	if (FAILED(m_pA3DDevice->GetD3DDevice()->CreateCubeTexture(nEdgeSize, nMipLevel,
              dwUsage, (D3DFORMAT)format, Pool, &m_pDXCubeTexture, NULL)))
	{
		g_A3DErrLog.Log("A3DCubeTexture::Create(), Fail in creating format[%d], edge[%d], [%d]mip cube texture", format, nEdgeSize, nMipLevel);
		return false;
	}

	CalcTextureDataSize();

	if (m_pA3DDevice)
		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_TEXTURE);

	return true;
}

bool A3DCubeTexture::LockRect(A3DCUBEMAP_FACES idFace, RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags, int nMipLevel)
{
	if (!m_pDXCubeTexture || m_bHWITexture)
		return false;

	if( nMipLevel >= m_nMipLevel )
	{
		g_A3DErrLog.Log("A3DTexture::LockRect(), mip level [%d] larger than m_nMipLevel [%d]", nMipLevel, m_nMipLevel);
		return false;
	}

	D3DLOCKED_RECT LockedRect;

	if (FAILED(m_pDXCubeTexture->LockRect((D3DCUBEMAP_FACES)idFace, nMipLevel, &LockedRect, pRect, 0)))
		return false;

	*ppData = LockedRect.pBits;
	*iPitch = LockedRect.Pitch;
	return true;
}

bool A3DCubeTexture::UnlockRect(A3DCUBEMAP_FACES idFace)
{
	if (!m_pDXCubeTexture || m_bHWITexture)
		return false;

	if (FAILED(m_pDXCubeTexture->UnlockRect((D3DCUBEMAP_FACES)idFace, 0)))
		return false;

	return true;
}

#define PACK_DIR_COLOR(x, y, z) (A3DCOLORRGB(int(((x) + 1.0f) * 127.5f), int(((y) + 1.0f) * 127.5f), int(((z) + 1.0f) * 127.5f)))

bool A3DCubeTexture::CreateNormalizationCubeMap(A3DDevice * pDevice, int nEdgeSize, const A3DMATRIX4& matTM)
{
	if( !Create(pDevice, nEdgeSize, A3DFMT_X8R8G8B8, 1) )
		return false;
	
	const A3DCUBEMAP_FACES faces[] = 
	{
		A3DCUBEMAP_FACE_POSITIVE_X, A3DCUBEMAP_FACE_NEGATIVE_X, A3DCUBEMAP_FACE_POSITIVE_Y,
		A3DCUBEMAP_FACE_NEGATIVE_Y, A3DCUBEMAP_FACE_POSITIVE_Z, A3DCUBEMAP_FACE_NEGATIVE_Z
	};
	const A3DVECTOR3 offsets[] = 
	{ 
		A3DVECTOR3(1.0f, 0.0f, 0.0f), A3DVECTOR3(-1.0f, 0.0f, 0.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f),
		A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(0.0f, 0.0f, -1.0f)
	};
	const A3DVECTOR3 u[] = 
	{
		A3DVECTOR3(0.0f, 0.0f, -1.0f), A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3(1.0f, 0.0f, 0.0f),
		A3DVECTOR3(1.0f, 0.0f, 0.0f), A3DVECTOR3(1.0f, 0.0f, 0.0f), A3DVECTOR3(-1.0f, 0.0f, 0.0f)
	};
	const A3DVECTOR3 v[] = 
	{
		A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3(0.0f, 0.0f, 1.0f),
		A3DVECTOR3(0.0f, 0.0f, -1.0f), A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3(0.0f, -1.0f, 0.0f)
	};

	float	r = 2.0f / nEdgeSize;

	DWORD * pData;
	int		pitch;

	A3DVECTOR3 vecDir;
	for(int i=0; i<6; i++)
	{
		// now lock all faces and fill them with normalization vectors
		if( !LockRect(faces[i], NULL, (void **)&pData, &pitch, 0) )
			return false;

		for(int y=0; y<nEdgeSize; y++)
		{
			for(int x=0; x<nEdgeSize; x++)
			{
				vecDir = offsets[i] + u[i] * (x * r - 1.0f) + v[i] * (y * r - 1.0f);
				vecDir.Normalize();
				vecDir = a3d_VectorMatrix3x3(vecDir, matTM);
				pData[x] = PACK_DIR_COLOR(vecDir.x, vecDir.y, vecDir.z);
			}

			pData = (DWORD *)((BYTE *)pData + pitch);
		}

		UnlockRect(faces[i]);
	}

	//D3DXSaveTextureToFileA("cube.dds", D3DXIFF_DDS, m_pDXCubeTexture, NULL);
	return true;
}

void A3DCubeTexture::CalcTextureDataSize()
{
#ifdef _DEBUG
	m_iTexDataSize = 0;

	if (!m_pDXCubeTexture || !m_pA3DDevice)
		return;
	
	int iLevelCnt = m_pDXCubeTexture->GetLevelCount();

	for (int i=0; i < iLevelCnt; i++)
	{
		D3DSURFACE_DESC	desc;
		m_pDXCubeTexture->GetLevelDesc(i, &desc);
		int nPitch = (desc.Width * 
			A3DDevice::BitsOfDeviceFormat((A3DFORMAT)desc.Format)) / 8;

		m_iTexDataSize += (desc.Height * nPitch) * 6;
	}

	m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->AddTextureDataSize(m_iTexDataSize);
	return;
#endif //_DEBUG
}

IDirect3DBaseTexture9* A3DCubeTexture::GetD3DBaseTexture() const
{
	return m_pDXCubeTexture;
}
