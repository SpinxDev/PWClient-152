#include "A3DSurfaceMan.h"
#include "A3DPI.h"
#include "A3DConfig.h"
#include "A3DDevice.h"
#include "A3DSurface.h"
#include "AFileImage.h"
#include "AFI.h"

A3DSurfaceMan::A3DSurfaceMan()
{
	m_pA3DDevice = NULL;
}

A3DSurfaceMan::~A3DSurfaceMan()
{
}

bool A3DSurfaceMan::Init(A3DDevice * pA3DDevice)
{	
	m_strFolderName = "Surfaces";
	m_pA3DDevice = pA3DDevice;
	return true;
}

bool A3DSurfaceMan::Release()
{
	return true;
}

bool A3DSurfaceMan::Reset()
{
	return true;
}

bool A3DSurfaceMan::LoadCursorSurfaceFromFile(int nWidth, int nHeight, const char * szFileName, A3DCOLOR colorKey, A3DSurface ** ppSurface)
{
	IDirect3DSurface9 * pDXSurface = NULL;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
	}
	else
	{
		HRESULT hval;
		
		if( strlen(szFileName) < 4 )
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile(), Filename [%s] not valid", szFileName);
			return false;
		}

		D3DFORMAT format = D3DFMT_A8R8G8B8;
		hval = m_pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(nWidth, nHeight, format, D3DPOOL_SYSTEMMEM, &pDXSurface, NULL);
		if (FAILED(hval)) 
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile CreateImageSurface Fail!");
			return false;
		}

		D3DXIMAGE_INFO image_info;

		AFileImage fileImage;
				   
		if( !fileImage.Open(m_strFolderName, szFileName, AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
		{
			pDXSurface->Release();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile(), Can not locate [%s] in package!", szFileName);
			return false;
		}
		
		hval = D3DXLoadSurfaceFromFileInMemory(pDXSurface, NULL, NULL, fileImage.GetFileBuffer(), fileImage.GetFileLength(), NULL, D3DX_FILTER_TRIANGLE, colorKey, &image_info);
		if (FAILED(hval)) 
		{
			fileImage.Close();
			pDXSurface->Release();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile create surface from file [%s] fail!", szFileName);
			return false;
		}
		fileImage.Close();
	}

	A3DSurface * pA3DSurface;
	pA3DSurface = new A3DSurface();
	if( NULL == pA3DSurface )
	{
		g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile Not enough memory!");
		return false;
	}

	if( !pA3DSurface->Init(m_pA3DDevice, pDXSurface, szFileName) )
	{
		pDXSurface->Release();
		g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile Init A3DSurface Fail!");
		return false;
	}

	*ppSurface = pA3DSurface;
	return true;
}

bool A3DSurfaceMan::LoadCursorSurfaceFromFile(const char* szFileName, A3DCOLOR colorKey, A3DSurface** ppSurface)
{
	IDirect3DSurface9* pDXSurface = NULL;

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
	{
	}
	else
	{
		HRESULT hval;
		
		if (strlen(szFileName) < 4)
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile(), Filename [%s] not valid", szFileName);
			return false;
		}

		AFileImage fileImage;
		
		//	Try to open file
		if (!fileImage.Open(m_strFolderName, szFileName, AFILE_OPENEXIST | AFILE_TEMPMEMORY))
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile(), Can not locate [%s] in package!", szFileName);
			return false;
		}

		//	Get file information
		D3DXIMAGE_INFO image_info;
		hval = D3DXGetImageInfoFromFileInMemory(fileImage.GetFileBuffer(), fileImage.GetFileLength(), &image_info);
		if (FAILED(hval))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile, failed to get file information [%s]!", szFileName);
			return false;
		}

		//	Create surface
		D3DFORMAT format = D3DFMT_A8R8G8B8;
		hval = m_pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(image_info.Width, image_info.Height, format, D3DPOOL_SYSTEMMEM, &pDXSurface, NULL);
		if (FAILED(hval))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile CreateImageSurface Fail!");
			return false;
		}

		//	Load image to surface
		hval = D3DXLoadSurfaceFromFileInMemory(pDXSurface, NULL, NULL, fileImage.GetFileBuffer(), fileImage.GetFileLength(), NULL, D3DX_FILTER_TRIANGLE, colorKey, &image_info);
		if (FAILED(hval)) 
		{
			fileImage.Close();
			pDXSurface->Release();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile create surface from file [%s] fail!", szFileName);
			return false;
		}

		fileImage.Close();
	}

	A3DSurface * pA3DSurface;
	if (!(pA3DSurface = new A3DSurface))
	{
		g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile Not enough memory!");
		return false;
	}

	if (!pA3DSurface->Init(m_pA3DDevice, pDXSurface, szFileName))
	{
		pDXSurface->Release();
		g_A3DErrLog.Log("A3DSurfaceMan::LoadCursorSurfaceFromFile Init A3DSurface Fail!");
		return false;
	}

	*ppSurface = pA3DSurface;

	return true;
}

bool A3DSurfaceMan::LoadSurfaceFromFile(int nWidth, int nHeight, const char * szFileName, A3DCOLOR colorKey, A3DSurface** ppA3DSurface)
{
	IDirect3DSurface9 * pDXSurface = NULL;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
	}
	else
	{
		AFileImage fileImage;
				   
		if( !fileImage.Open(m_strFolderName, szFileName, AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFile(), Can not locate [%s] in package!", szFileName);
			return false;
		}

		int iNameLen = strlen(szFileName);
		if (iNameLen < 4)
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFile Filename [%s] not valid", szFileName);
			return false;
		}

		//	Get file information
		D3DXIMAGE_INFO image_info;
		HRESULT hval = D3DXGetImageInfoFromFileInMemory(fileImage.GetFileBuffer(), fileImage.GetFileLength(), &image_info);
		if (FAILED(hval))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFile, failed to get file information [%s]!", szFileName);
			return false;
		}

		D3DFORMAT format = image_info.Format;

		//	checking file ext is neither safe nor valid (user may pass a wrong ext name in)
		if (D3DXIFF_BMP == image_info.ImageFileFormat ||
			D3DXIFF_JPG == image_info.ImageFileFormat)
		{
			if (colorKey == 0)
			{
				format = (D3DFORMAT) m_pA3DDevice->GetNormalTextureFormat();
			}
			else
			{
				//	Has color key, so use A1R5G5B5 or A8R8G8B8
				format = (D3DFORMAT) m_pA3DDevice->GetColorKeyTextureFormat();
			}
		}
		else if (D3DXIFF_TGA == image_info.ImageFileFormat)
		{
			//	Use A4R4G4B4, or A8R8G8B8
			format = (D3DFORMAT) m_pA3DDevice->GetAlphaTextureFormat();
		}
		else if (D3DXIFF_DDS == image_info.ImageFileFormat)
		{
			format = image_info.Format;

			// check if the dds format is supported or not, if not, just use 
			HRESULT hval;
			hval = m_pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (D3DFORMAT)m_pA3DDevice->GetDevFormat().fmtAdapter, 0, D3DRTYPE_TEXTURE, format);
			if( D3D_OK != hval )
			{
				if( format == D3DFMT_DXT3 || format == D3DFMT_DXT5 )
					format = (D3DFORMAT) m_pA3DDevice->GetAlphaTextureFormat();
				else
					format = (D3DFORMAT) m_pA3DDevice->GetNormalTextureFormat();
			}

		}

		if( nWidth == 0 )
			nWidth = image_info.Width;
		if( nHeight == 0 )
			nHeight = image_info.Height;

		hval = m_pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(nWidth, nHeight, format, D3DPOOL_SCRATCH/*D3DPOOL_SYSTEMMEM*/, &pDXSurface, NULL);
		if (FAILED(hval))
		{
			fileImage.Close();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFile CreateImageSurface Fail!");
			return false;
		}

		hval = D3DXLoadSurfaceFromFileInMemory(pDXSurface, NULL, NULL, fileImage.GetFileBuffer(), fileImage.GetFileLength(), NULL, D3DX_FILTER_TRIANGLE, colorKey, &image_info);
		if (FAILED(hval)) 
		{
			fileImage.Close();
			pDXSurface->Release();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFile create surface from file [%s] fail!", szFileName);
			return false;
		}
		fileImage.Close();
	}

	A3DSurface * pA3DSurface;
	pA3DSurface = new A3DSurface();
	if( NULL == pA3DSurface )
	{
		g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFile Not enough memory!");
		return false;
	}

	if( !pA3DSurface->Init(m_pA3DDevice, pDXSurface, szFileName) )
	{
		pDXSurface->Release();
		g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFile Init A3DSurface Fail!");
		return false;
	}

	*ppA3DSurface = pA3DSurface;
	return true;
}

bool A3DSurfaceMan::LoadSurfaceFromFile(const char* szFileName, A3DCOLOR colorKey, A3DSurface** ppA3DSurface)
{
	return LoadSurfaceFromFile(0, 0, szFileName, colorKey, ppA3DSurface);
}

bool A3DSurfaceMan::LoadSurfaceFromFileInMemory(int nWidth, int nHeight, const char * szFileName, const void * pMemFile, DWORD dwFileLength, A3DCOLOR colorKey, A3DSurface** ppA3DSurface)
{
	IDirect3DSurface9 * pDXSurface = NULL;

	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
	{
	}
	else
	{
		int iNameLen = strlen(szFileName);
		if (iNameLen < 4)
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFileInMemory Filename [%s] not valid", szFileName);
			return false;
		}

		//	Get file information
		D3DXIMAGE_INFO image_info;
		HRESULT hval = D3DXGetImageInfoFromFileInMemory(pMemFile, dwFileLength, &image_info);
		if (FAILED(hval))
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFileInMemory, failed to get file information [%s]!", szFileName);
			return false;
		}

		D3DFORMAT format = image_info.Format;

		//	checking file ext is neither safe nor valid (user may pass a wrong ext name in)
		if (D3DXIFF_BMP == image_info.ImageFileFormat ||
			D3DXIFF_JPG == image_info.ImageFileFormat)
		{
			if (colorKey == 0)
			{
				format = (D3DFORMAT) m_pA3DDevice->GetNormalTextureFormat();
			}
			else
			{
				//	Has color key, so use A1R5G5B5 or A8R8G8B8
				format = (D3DFORMAT) m_pA3DDevice->GetColorKeyTextureFormat();
			}
		}
		else if (D3DXIFF_TGA == image_info.ImageFileFormat)
		{
			//	Use A4R4G4B4, or A8R8G8B8
			format = (D3DFORMAT) m_pA3DDevice->GetAlphaTextureFormat();
		}
		else if (D3DXIFF_DDS == image_info.ImageFileFormat)
		{
			format = image_info.Format;

			// check if the dds format is supported or not, if not, just use 
			HRESULT hval;
			hval = m_pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (D3DFORMAT)m_pA3DDevice->GetDevFormat().fmtAdapter, 0, D3DRTYPE_TEXTURE, format);
			if( D3D_OK != hval )
			{
				if( format == D3DFMT_DXT3 || format == D3DFMT_DXT5 )
					format = (D3DFORMAT) m_pA3DDevice->GetAlphaTextureFormat();
				else
					format = (D3DFORMAT) m_pA3DDevice->GetNormalTextureFormat();
			}

			// There is a bug in d3dx8 when a .dds file need colorkey processing. 
			// It is a pointer overrun error and not documented in DXSDK yet.
			// so we currently not use color key for .dds file
			colorKey = 0;
		}
		
		if( nWidth == 0 )
			nWidth = image_info.Width;
		if( nHeight == 0 )
			nHeight = image_info.Height;

		hval = m_pA3DDevice->GetD3DDevice()->CreateOffscreenPlainSurface(nWidth, nHeight, format, D3DPOOL_SYSTEMMEM,  &pDXSurface, NULL);
		if (FAILED(hval))
		{
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFileInMemory CreateImageSurface Fail!");
			return false;
		}

		hval = D3DXLoadSurfaceFromFileInMemory(pDXSurface, NULL, NULL, pMemFile, dwFileLength, NULL, D3DX_FILTER_TRIANGLE, colorKey, &image_info);
		if (FAILED(hval)) 
		{
			pDXSurface->Release();
			g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFileInMemory create surface from file [%s] fail!", szFileName);
			return false;
		}
	}

	A3DSurface * pA3DSurface;
	pA3DSurface = new A3DSurface();
	if( NULL == pA3DSurface )
	{
		g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFileInMemory Not enough memory!");
		return false;
	}

	if( !pA3DSurface->Init(m_pA3DDevice, pDXSurface, szFileName) )
	{
		pDXSurface->Release();
		g_A3DErrLog.Log("A3DSurfaceMan::LoadSurfaceFromFileInMemory Init A3DSurface Fail!");
		return false;
	}

	*ppA3DSurface = pA3DSurface;
	return true;
}


bool A3DSurfaceMan::LoadSurfaceFromFileInMemory(const char* szFileName, const void * pMemFile, DWORD dwFileLength, A3DCOLOR colorKey, A3DSurface** ppA3DSurface)
{
	return LoadSurfaceFromFileInMemory(0, 0, szFileName, pMemFile, dwFileLength, colorKey, ppA3DSurface);
}

bool A3DSurfaceMan::ReleaseSurface(A3DSurface*& pSurface)
{
	pSurface->Release();
	delete pSurface;

	pSurface = NULL;
	return true;
}