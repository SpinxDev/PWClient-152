/*
 * FILE: A3DTerrain2Loader.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/3/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2Loader.h"
#include "A3DTerrain2Env.h"
#include "A3DPI.h"
#include "A3DTexture.h"
#include "A3DMacros.h"
#include "AAssist.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2Loader
//	
///////////////////////////////////////////////////////////////////////////

//	Fill light map content
bool A3DTerrain2Loader::FillLightMap(int iMapSize, BYTE* pSrcData, A3DFORMAT DstFmt, A3DTexture* pTexture, int iSrcBitsPerPixel)
{
	BYTE* pDstData;
	int iDstPitch;
	if (!pTexture->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
	{
		g_A3DErrLog.Log("A3DTerrain2Loader::FillLightMap, Failed to lock texture");
		return false;
	}

	BYTE* pSrc = pSrcData;
	BYTE* pDstLine = pDstData;

	switch (DstFmt)
	{
	case A3DFMT_R5G6B5:
	{
		float fScale1 = 31.0f / 255.0f;
		float fScale2 = 63.0f / 255.0f;
		for (int i=0; i < iMapSize; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				WORD b = (WORD)(*pSrc++ * fScale1);
				WORD g = (WORD)(*pSrc++ * fScale2);
				WORD r = (WORD)(*pSrc++ * fScale1);
				*pDst++ = (r << 11) | (g << 5) | b;
			}
		}

		break;
	}
	case A3DFMT_A1R5G5B5:
	case A3DFMT_X1R5G5B5:
	{
		float fScale = 31.0f / 255.0f;
		for (int i=0; i < iMapSize; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				WORD b = (WORD)(*pSrc++ * fScale);
				WORD g = (WORD)(*pSrc++ * fScale);
				WORD r = (WORD)(*pSrc++ * fScale);
				*pDst++ = (r << 10) | (g << 5) | b;
			}
		}

		break;
	}
	case A3DFMT_R8G8B8:
	{
		int iBytes = iMapSize * 3;
		for (int i=0; i < iMapSize; i++)
		{
			memcpy(pDstLine, pSrc, iBytes);
			pDstLine += iDstPitch;
			pSrc += iBytes;
		}

		break;
	}
	case A3DFMT_A8R8G8B8:
	case A3DFMT_X8R8G8B8:
	{
		if (A3D::g_pA3DTerrain2Env->IsLightMap32Bit())
		{
			if (iSrcBitsPerPixel == 32)
			{
				for (int i=0; i < iMapSize; i++)
				{
					BYTE* pDst = pDstLine;
					pDstLine += iDstPitch;
					for (int j=0; j < iMapSize; j++)
					{
						*pDst++ = *pSrc++;
						*pDst++ = *pSrc++;
						*pDst++ = *pSrc++;
						*pDst++ = *pSrc++;
					}
				}
			}
			else if (iSrcBitsPerPixel == 24)
			{
				for (int i=0; i < iMapSize; i++)
				{
					BYTE* pDst = pDstLine;
					pDstLine += iDstPitch;
					for (int j=0; j < iMapSize; j++)
					{
						*pDst++ = *pSrc++;
						*pDst++ = *pSrc++;
						*pDst++ = *pSrc++;
						*pDst++ = 0xff;
					}
				}
			}
		}
		else
		{
			for (int i=0; i < iMapSize; i++)
			{
				BYTE* pDst = pDstLine;
				pDstLine += iDstPitch;
				for (int j=0; j < iMapSize; j++)
				{
					*pDst++ = *pSrc++;
					*pDst++ = *pSrc++;
					*pDst++ = *pSrc++;
					*pDst++ = 0xff;
				}
			}
		}

		break;
	}
	default:
		ASSERT(0);
		break;
	}

	pTexture->UnlockRect();

	return true;
}

//	Fill mask texture content
bool A3DTerrain2Loader::FillMaskTexture(int iMapSize, BYTE* pSrcData, A3DFORMAT DstFmt, A3DTexture* pTexture)
{
	BYTE* pDstData;
	int iDstPitch;
	if (!pTexture->LockRect(NULL, (void**) &pDstData, &iDstPitch, 0))
	{
		g_A3DErrLog.Log("A3DTerrain2Loader::FillMaskTexture, Failed to lock texture");
		return false;
	}

	BYTE* pSrc = pSrcData;
	BYTE* pDstLine = pDstData;

	switch (DstFmt)
	{
	case A3DFMT_A8:
	case A3DFMT_L8:			//	8-bit
	{
		for (int i=0; i < iMapSize; i++)
		{
			memcpy(pDstLine, pSrc, iMapSize);
			pDstLine += iDstPitch;
			pSrc += iMapSize;
		}

		break;
	}
	case A3DFMT_A4L4:		//	8-bit
	{
		float fScale = 15.0f / 255.0f;
		for (int i=0; i < iMapSize; i++)
		{
			BYTE* pDst = pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (BYTE)(c * fScale);
			}
		}

		break;
	}
	case A3DFMT_A8L8:
	case A3DFMT_A8P8:
	case A3DFMT_A8R3G3B2:
	{
		for (int i=0; i < iMapSize; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (((WORD)c) << 8) | c;
			}
		}

		break;
	}
	case A3DFMT_R5G6B5:
	case A3DFMT_A1R5G5B5:
	case A3DFMT_X1R5G5B5:
	{
		float fScale = 31.0f / 255.0f;
		for (int i=0; i < iMapSize; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = (WORD)(c * fScale);
			}
		}

		break;
	}
	case A3DFMT_A4R4G4B4:
	case A3DFMT_X4R4G4B4:	//	16-bit
	{
		float fScale = 15.0f / 255.0f;
		for (int i=0; i < iMapSize; i++)
		{
			WORD* pDst = (WORD*)pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				WORD c = *pSrc++;
				c = (WORD)(c * fScale);
				*pDst++ = (c << 12) | (c << 8) | (c << 4) | c;
			}
		}

		break;
	}
	case A3DFMT_R8G8B8:		//	24-bit
	{
		for (int i=0; i < iMapSize; i++)
		{
			BYTE* pDst = pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = c;
				*pDst++ = c;
				*pDst++ = c;
			}
		}

		break;
	}
	case A3DFMT_A8R8G8B8:
	case A3DFMT_X8R8G8B8:	//	32-bit
	{
		for (int i=0; i < iMapSize; i++)
		{
			DWORD* pDst = (DWORD*)pDstLine;
			pDstLine += iDstPitch;
			for (int j=0; j < iMapSize; j++)
			{
				BYTE c = *pSrc++;
				*pDst++ = A3DCOLORRGBA(c, c, c, c);
			}
		}

		break;
	}
	default:
		ASSERT(0);
		break;
	}

	pTexture->UnlockRect();

	return true;
}
