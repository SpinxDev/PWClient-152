#include <ddraw.h>
#include "A3DSurface.h"
#include "A3DPI.h"
#include "A3DConfig.h"

A3DSurface::A3DSurface()
{
	m_pA3DDevice		= NULL;
	m_pDXSurface		= NULL;
	m_szSurfaceMap[0]	= '\0';
	m_pDXTContainer		= NULL;
	m_nDXTLength		= 0;

	m_bHWISurface		= false;
}

A3DSurface::~A3DSurface()
{
}

bool A3DSurface::Init(A3DDevice * pA3DDevice, IDirect3DSurface9 * pDXSurface, const char * szMapFile)
{
	m_pA3DDevice = pA3DDevice;
	m_pDXSurface = pDXSurface;
	strncpy(m_szSurfaceMap, szMapFile, MAX_PATH);

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWISurface = true;
		return true;
	}

	D3DSURFACE_DESC		desc; 
	pDXSurface->GetDesc(&desc);

	m_nWidth = desc.Width;
	m_nHeight = desc.Height;   
	m_format = (A3DFORMAT) desc.Format;
	return true;
}

bool A3DSurface::InitAsDXTContainer(A3DDevice * pA3DDevice, BYTE * pData, int nDataLen, const char * szMapFile)
{
	m_pA3DDevice = pA3DDevice;
	m_pDXTContainer = new BYTE[nDataLen];
	memcpy(m_pDXTContainer, pData, nDataLen);
	m_nDXTLength = nDataLen;

	strncpy(m_szSurfaceMap, szMapFile, MAX_PATH);
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
		m_bHWISurface = true;
		return true;
	}

	DDSURFACEDESC2 *pDdsFileHeader=(DDSURFACEDESC2*)(m_pDXTContainer + 4);
	if( pDdsFileHeader->ddpfPixelFormat.dwFlags == DDPF_FOURCC )
	{
		switch( pDdsFileHeader->ddpfPixelFormat.dwFourCC )
		{
		case 0x31545844: // DXT1
			m_format = A3DFMT_DXT1;
			break;

		case 0x33545844: // DXT3
			m_format = A3DFMT_DXT3;
			break;

		case 0x35545844: // DXT5
			m_format = A3DFMT_DXT5;
			break;

		default:// now only DXT1 ~ DXT5 can use this container
			return false;
		}
	}
	else // now only DXT1 ~ DXT5 can use this container
		return false;

	m_nWidth = pDdsFileHeader->dwWidth;
	m_nHeight = pDdsFileHeader->dwHeight;   
	return true;
}

bool A3DSurface::Release()
{
	if( m_pDXSurface )
	{
		if(m_pA3DDevice->GetNeedResetFlag())
		{
			m_pDXSurface->Release();
		}
		else
		{
			if(m_pA3DDevice)
				m_pA3DDevice->ReleaseResource(m_pDXSurface);  //MultiThread Render
		}

		m_pDXSurface = NULL;
	}

	if( m_pDXTContainer )
	{
		delete m_pDXTContainer;
		m_pDXTContainer = NULL;
	}
	return true;
}

bool A3DSurface::DrawToBack(int x, int y)
{
	if( m_bHWISurface )		return true;

	bool bval;

	bval = m_pA3DDevice->CopyToBack(x, y, m_nWidth, m_nHeight, m_pDXSurface);
	if( !bval )
		return false;

	return true;
}

bool A3DSurface::Create(A3DDevice* pDevice, int iWidth, int iHeight, A3DFORMAT Format)
{
	if( m_pDXSurface )
	{
		g_A3DErrLog.Log("A3DSurface::Create(), This surface has been created, can not recreated!");
		return false;
	}

	IDirect3DSurface9 * pDXSurface = NULL;
	if (FAILED(pDevice->GetD3DDevice()->CreateOffscreenPlainSurface(iWidth, iHeight, 
		(D3DFORMAT)Format,D3DPOOL_SYSTEMMEM, &pDXSurface, NULL)))
		return false;

	if (!Init(pDevice, pDXSurface, "CREATED_SURFACE"))
		return false;

	return true;
}

bool A3DSurface::LockRect(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags)
{
	if( m_pDXTContainer )
	{
		int bytes_per_block = (m_format == A3DFMT_DXT1 ? 8 : 16);
		int nPitch = (m_nWidth >> 2) * bytes_per_block;

		// manually lock by ourself
		// first see if the rect is times of 4
		if( (pRect->left & 0x3) || (pRect->right & 0x3) || (pRect->top & 0x3) || (pRect->bottom & 0x3) )
			return false;

		if( pRect->left < 0 || pRect->top < 0 || pRect->right > m_nWidth || pRect->bottom > m_nHeight )
			return false;

		*ppData = m_pDXTContainer + 128 + (pRect->top >> 2) * nPitch + (pRect->left >> 2) * bytes_per_block;
		*iPitch = nPitch;
	}
	else if( m_pDXSurface )
	{
		D3DLOCKED_RECT LockedRect;
		if (FAILED(m_pDXSurface->LockRect(&LockedRect, pRect, 0)))
			return false;

		*ppData = LockedRect.pBits;
		*iPitch = LockedRect.Pitch;
	}
	else
		return false;

	return true;

}

bool A3DSurface::UnlockRect()
{
	if( m_pDXTContainer )
		return true;

	if (!m_pDXSurface)
		return false;

	if (FAILED(m_pDXSurface->UnlockRect()))
		return false;

	return true;
}



bool A3DSurface::SaveToFile(const char * szFileName)
{
	D3DXSaveSurfaceToFileA(szFileName, D3DXIFF_DDS, m_pDXSurface, NULL, NULL);
	return true;
}

bool A3DSurface::LockRectDynamic(RECT* pRect, void** ppData, int* iPitch, DWORD dwFlags)
{
    if( m_pDXTContainer )
    {
        int bytes_per_block = (m_format == A3DFMT_DXT1 ? 8 : 16);
        int nPitch = (m_nWidth >> 2) * bytes_per_block;

        // manually lock by ourself
        // first see if the rect is times of 4
        if( (pRect->left & 0x3) || (pRect->right & 0x3) || (pRect->top & 0x3) || (pRect->bottom & 0x3) )
            return false;

        if( pRect->left < 0 || pRect->top < 0 || pRect->right > m_nWidth || pRect->bottom > m_nHeight )
            return false;

        *ppData = m_pDXTContainer + 128 + (pRect->top >> 2) * nPitch + (pRect->left >> 2) * bytes_per_block;
        *iPitch = nPitch;
    }
    else if( m_pDXSurface )
    {
        DWORD dwWidth = pRect ? pRect->right - pRect->left : m_nWidth;
        DWORD dwHeight = pRect ? pRect->bottom - pRect->top : m_nHeight;

        DWORD dwPitch = dwWidth * A3DDevice::BitsOfDeviceFormat(m_format);

        if (FAILED(m_pA3DDevice->LockSurface(m_pDXSurface, ppData, dwPitch, dwHeight, pRect, 0, iPitch)))
            return false;
    }
    else
        return false;

    return true;
}

bool A3DSurface::UnlockRectDynamic()
{
    if( m_pDXTContainer )
        return true;

    if (!m_pDXSurface)
        return false;

    if (FAILED(m_pA3DDevice->UnlockSurface(m_pDXSurface)))
        return false;


    return true;
}
