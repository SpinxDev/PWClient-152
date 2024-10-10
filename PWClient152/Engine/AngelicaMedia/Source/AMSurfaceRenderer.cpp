/*
 * FILE: AMSurfaceRenderer.cpp
 *
 * DESCRIPTION: The class standing for the directshow filter which render video 
 *				into an A3DSurface object of Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/5/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */

#include "AMPlatform.h"
#include "AMPI.h"
#include "AMSurfaceRenderer.h"

AMSurfaceRenderer::AMSurfaceRenderer(LPUNKNOWN pUnk, HRESULT *phr)
                                   : CBaseVideoRenderer(__uuidof(CLSID_SurfaceRenderer), 
                                   NAME("Surface Renderer"), pUnk, phr)
{
    *phr = S_OK;

	m_pDestSprite = NULL;
	m_pSurfaceAccess = NULL;
}

AMSurfaceRenderer::~AMSurfaceRenderer()
{
}


//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT AMSurfaceRenderer::CheckMediaType(const CMediaType *pmt)
{
    HRESULT   hr = E_FAIL;
    VIDEOINFO *pvi;
    
    // Reject the connection if this is not a video type
    if( *pmt->FormatType() != FORMAT_VideoInfo ) {
        return E_INVALIDARG;
    }
    
    // accept RGB24	and RGB565 and RGB555
    pvi = (VIDEOINFO *)pmt->Format();
    if( IsEqualGUID(*pmt->Type(), MEDIATYPE_Video) ) 
	{
		if( IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB32) ||
			IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB565) ||
			IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB555) )
		{	
			hr = S_OK;
		}
	}
    return hr;
}


//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made. 
//-----------------------------------------------------------------------------
HRESULT AMSurfaceRenderer::SetMediaType(const CMediaType *pmt)
{
    // Retrive the size of this media type
    VIDEOINFO *pviBmp;                      // Bitmap info header
    pviBmp = (VIDEOINFO *)pmt->Format();
    m_lVidWidth  = pviBmp->bmiHeader.biWidth;
    m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);

	if( IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB565) )
	{
		m_lVidPitch = (m_lVidWidth * 2 + 3) & ~(3); // We are using RGB565
		m_fmtVideo = A3DFMT_R5G6B5;
	}
	else if( IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB555) )
	{
		m_lVidPitch = (m_lVidWidth * 2 + 3) & ~(3); // We are forcing RGB555
		m_fmtVideo = A3DFMT_X1R5G5B5;
	}
	else if( IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB32) )
	{
		m_lVidPitch = m_lVidWidth * 4; // We are using X8R8G8B8;
		m_fmtVideo = A3DFMT_X8R8G8B8;
	}

    return S_OK;
}


//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT AMSurfaceRenderer::DoRenderSample(IMediaSample * pSample)
{
	static int nCount = 0;
	nCount ++;

	// If have not specified the destination, we just return successfully;
	if( !m_pSurfaceAccess || !m_pDestSprite )
		return S_OK;
	
	EnterCriticalSection(m_pSurfaceAccess);

	BYTE  *pBmpBuffer; // Bitmap buffer
    
    // Get the video bitmap buffer
    pSample->GetPointer( &pBmpBuffer );

	if( !m_pDestSprite->UpdateTextures(pBmpBuffer, m_lVidPitch, m_fmtVideo) )
	{
		AMERRLOG(("AMSurfaceRenderer::DoSample(), Can not update dest sprite's textures!"));
		return false;
	}

	LeaveCriticalSection(m_pSurfaceAccess);
    return S_OK;
}