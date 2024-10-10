/*
 * FILE: A3DCBRecorder.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 7, 3
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DCBRecorder.h"
#include "A3DPlatform.h"

A3DCBRecorder9::A3DCBRecorder9()
:A3DCBRecorderBase(),
m_pD3DDevice(NULL)
{
}
	
void A3DCBRecorder9::SetD3DDevice(IDirect3DDevice9* pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
}

/* IUnknown methods */
HRESULT STDMETHODCALLTYPE A3DCBRecorder9::QueryInterface (REFIID riid, void **ppvObj)
{
	return m_pD3DDevice->QueryInterface(riid, ppvObj);
};

UINT A3DCBRecorder9::GetIndexCountOfPrimitive(UINT PrimitiveType, UINT PrimitiveCount)
{
    //A3DPT_POINTLIST       = 1,
    //A3DPT_LINELIST        = 2,
    //A3DPT_LINESTRIP       = 3,
    //A3DPT_TRIANGLELIST    = 4,
    //A3DPT_TRIANGLESTRIP   = 5,
    //A3DPT_TRIANGLEFAN     = 6,
    static UINT uMul[] = { 0, 1, 2, 1, 3, 1, 1 }; 
    static UINT uAdd[] = { 0, 0, 0, 1, 0, 2, 2 }; 
    return uMul[PrimitiveType] * PrimitiveCount + uAdd[PrimitiveType];
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::DrawPrimitiveUP(	D3DPRIMITIVETYPE	PrimitiveType,
															UINT				PrimitiveCount,
															CONST void			*pVertexStreamZeroData,
															UINT				VertexStreamZeroStride)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DrawPrimitiveUP);
	m_pCB->PutDWORD(PrimitiveType);
	m_pCB->PutDWORD(PrimitiveCount);
	m_pCB->PutDWORD(VertexStreamZeroStride);
    UINT NumVertices = GetIndexCountOfPrimitive(PrimitiveType, PrimitiveCount);
    m_pCB->PutDWORD(NumVertices);
    UINT uVertedSize = NumVertices * VertexStreamZeroStride;
    m_pCB->DoPutMem(pVertexStreamZeroData, uVertedSize);
	return D3D_OK;
}


HRESULT STDMETHODCALLTYPE A3DCBRecorder9::DrawIndexedPrimitiveUP(	D3DPRIMITIVETYPE	PrimitiveType,
																	UINT				MinVertexIndex,
																	UINT				NumVertices,
																	UINT				PrimitiveCount,
																	CONST void			*pIndexData,
																	D3DFORMAT			IndexDataFormat,
																	CONST void			*pVertexStreamZeroData,
																	UINT				VertexStreamZeroStride)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DrawIndexedPrimitiveUP);
	m_pCB->PutDWORD(PrimitiveType);
	m_pCB->PutDWORD(MinVertexIndex);
	m_pCB->PutDWORD(NumVertices);
	m_pCB->PutDWORD(PrimitiveCount);
	m_pCB->PutDWORD(IndexDataFormat);
	m_pCB->PutDWORD(VertexStreamZeroStride);

    UINT uIndexCount = GetIndexCountOfPrimitive(PrimitiveType, PrimitiveCount);
    UINT uIndexStride = IndexDataFormat == D3DFMT_INDEX32 ? 4 : 2;
    UINT uVertedSize = NumVertices * VertexStreamZeroStride;
    UINT uIndexSize = uIndexCount * uIndexStride;
    m_pCB->PutDWORD(uIndexSize);
    m_pCB->DoPutMem(pIndexData, uIndexSize);
    m_pCB->DoPutMem(pVertexStreamZeroData, uVertedSize);
	return D3D_OK;
}

// 直接调用D3D
HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS * pPresentationParameters,
																	IDirect3DSwapChain9 **pSwapChain)
{
	return m_pD3DDevice->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateTexture(UINT Width,
														UINT Height,
														UINT Levels,
														DWORD Usage,
														D3DFORMAT Format,
														D3DPOOL Pool,
														IDirect3DTexture9 **ppTexture,
														HANDLE * pSharedHandle)
{
	return m_pD3DDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateVolumeTexture(	UINT Width,
																UINT Height,
																UINT Depth,
																UINT Levels,
																DWORD Usage,
																D3DFORMAT Format,
																D3DPOOL Pool,
																IDirect3DVolumeTexture9 **ppVolumeTexture,
																HANDLE * pSharedHandle)
{
	return m_pD3DDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateCubeTexture(UINT EdgeLength,
															UINT Levels,
															DWORD Usage,
															D3DFORMAT Format,
															D3DPOOL Pool,
															IDirect3DCubeTexture9 **ppCubeTexture,
															HANDLE * pSharedHandle)
{
    return m_pD3DDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateVertexBuffer(	UINT Length,
																DWORD Usage,
																DWORD FVF,
																D3DPOOL Pool,
																IDirect3DVertexBuffer9 **ppVertexBuffer,
																HANDLE * pSharedHandle)
{
	return m_pD3DDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}


HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateIndexBuffer(UINT Length,
															DWORD Usage,
															D3DFORMAT Format,
															D3DPOOL Pool,
															IDirect3DIndexBuffer9 **ppIndexBuffer,
															HANDLE * pSharedHandle)
{
	return m_pD3DDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}


HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateRenderTarget(	UINT Width,
																UINT Height,
																D3DFORMAT Format,
																D3DMULTISAMPLE_TYPE MultiSample,
																DWORD MultisampleQuality,
																BOOL Lockable,
																IDirect3DSurface9 **ppSurface,
																HANDLE * pSharedHandle)
{
	return m_pD3DDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}


HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateDepthStencilSurface(UINT Width,
																	UINT Height,
																	D3DFORMAT Format,
																	D3DMULTISAMPLE_TYPE MultiSample,
																	DWORD MultisampleQuality,
																	BOOL Discard,
																	IDirect3DSurface9 **ppSurface,
																	HANDLE * pSharedHandle)
{
	return m_pD3DDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}


HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateOffscreenPlainSurface(UINT Width,
																		UINT Height,
																		D3DFORMAT Format,
																		D3DPOOL Pool,
																		IDirect3DSurface9 **ppSurface,
																		HANDLE * pSharedHandle)
{
	return m_pD3DDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}


HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateStateBlock (D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
{
	return m_pD3DDevice->CreateStateBlock(Type, ppSB);
}


HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateVertexDeclaration(	CONST D3DVERTEXELEMENT9 * pVertexElements,
																	IDirect3DVertexDeclaration9 **ppDecl)
{
	return m_pD3DDevice->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreateVertexShader(CONST DWORD * pFunction, IDirect3DVertexShader9 **ppShader)
{
	return m_pD3DDevice->CreateVertexShader(pFunction, ppShader);
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CreatePixelShader(CONST DWORD * pFunction, IDirect3DPixelShader9 **ppShader)
{
    return m_pD3DDevice->CreatePixelShader(pFunction, ppShader);
}

// New methods
/* =================================================================================================================
 =================================================================================================================== */
HRESULT STDMETHODCALLTYPE A3DCBRecorder9::LockVertexBuffer(IDirect3DVertexBuffer9*	pVertexBuffer,
														   DWORD OffsetToLock, 
														   DWORD SizeToLock, 
														   void** ppbData, 
														   DWORD dwFlags)
{
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::LockVertexBuffer);
    m_pCB->PutDWORD(pVertexBuffer);
    m_pCB->PutDWORD(OffsetToLock);
    m_pCB->PutDWORD(SizeToLock);
    m_pCB->PutDWORD(dwFlags);
    *ppbData = m_pCB->AllocMem(SizeToLock);
	m_pCB->LockVertexBuffer();
    return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::UnlockVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer)
{
	m_pCB->UnlockVertexBuffer();
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::UnlockVertexBuffer);
    m_pCB->PutDWORD(pVertexBuffer);
    return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::LockIndexBuffer(IDirect3DIndexBuffer9*	pIndexBuffer,
														  DWORD OffsetToLock, 
														  DWORD SizeToLock, 
														  void** ppbData, 
														  DWORD dwFlags)
{
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::LockIndexBuffer);
    m_pCB->PutDWORD(pIndexBuffer);
    m_pCB->PutDWORD(OffsetToLock);
    m_pCB->PutDWORD(SizeToLock);
    m_pCB->PutDWORD(dwFlags);
    *ppbData = m_pCB->AllocMem(SizeToLock);
	m_pCB->LockIndexBuffer();
    return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::UnlockIndexBuffer(IDirect3DIndexBuffer9*	pIndexBuffer)
{
	m_pCB->UnlockIndexBuffer();
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::UnlockIndexBuffer);
    m_pCB->PutDWORD(pIndexBuffer);
    return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::LockTexture(	IDirect3DTexture9*	pTexture,
														UINT Level, 
														void** ppbData, 
														DWORD SrcPitchWidth, 
														DWORD SrcPitchHeight, 
														RECT* pRect, 
														DWORD dwFlags)
{
    DWORD dwSize = SrcPitchWidth * SrcPitchHeight;
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::LockTexture);
    m_pCB->PutDWORD(pTexture);
    m_pCB->PutDWORD(Level);
    m_pCB->PutDWORD(SrcPitchWidth);
    m_pCB->PutDWORD(SrcPitchHeight);
    m_pCB->PutDWORD(pRect);
    m_pCB->PutDWORD(dwFlags);
    *ppbData = m_pCB->AllocMem(dwSize);
	m_pCB->LockTexture();

    return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorder9::UnlockTexture(IDirect3DTexture9*	pTexture, UINT Level)
{
	m_pCB->UnlockTexture();
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::UnlockTexture);
    m_pCB->PutDWORD(pTexture);
    m_pCB->PutDWORD(Level);
    return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorder9::LockSurface(	IDirect3DSurface9*	pSurface,
														void** ppbData, 
														DWORD SrcPitchWidth, 
														DWORD SrcPitchHeight, 
														RECT* pRect, 
														DWORD dwFlags)
{
    DWORD dwSize = SrcPitchWidth * SrcPitchHeight;
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::LockSurface);
    m_pCB->PutDWORD(pSurface);
    m_pCB->PutDWORD(SrcPitchWidth);
    m_pCB->PutDWORD(SrcPitchHeight);
    m_pCB->PutDWORD(pRect);
    m_pCB->PutDWORD(dwFlags);
    *ppbData = m_pCB->AllocMem(dwSize);
	m_pCB->LockSurface();

    return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::UnlockSurface(IDirect3DSurface9*	pSurface)
{
	m_pCB->UnlockSurface();
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::UnlockSurface);
    m_pCB->PutDWORD(pSurface);
    return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorder9::GetDirect3D (IDirect3D9 **ppD3D9)
{
	return m_pD3DDevice->GetDirect3D(ppD3D9);
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorder9::GetDeviceCaps (LPD3DCAPS9 pCaps)
{
	return m_pD3DDevice->GetDeviceCaps(pCaps);
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorder9::GetDisplayMode (UINT iSwapChain, D3DDISPLAYMODE * pMode)
{
	return m_pD3DDevice->GetDisplayMode(iSwapChain, pMode);
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorder9::SaveSurface(const char* szFileName, IDirect3DSurface9* pSurface, DWORD fmt)
 {
     CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SaveSurface);
     size_t nLen = strlen(szFileName);
     m_pCB->PutDWORD(nLen);
     m_pCB->DoPutMem(szFileName, nLen + 1);
     m_pCB->PutDWORD(pSurface);
     m_pCB->PutDWORD(fmt);
     return D3D_OK;
 }

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::GetRenderTargetData (IDirect3DSurface9 * pRenderTarget, IDirect3DSurface9 * pDestSurface)
 {
	 CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::GetRenderTargetData);
	 m_pCB->PutDWORD(pRenderTarget);
	 m_pCB->PutDWORD(pDestSurface);
	 return D3D_OK;	
 }

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::CopySurface(IDirect3DSurface9* pDestSurface, 
													  const RECT* pDestRect, 
                                                      IDirect3DSurface9* pSrcSurface, 
													  const RECT* pSrcRect, 
													  DWORD Filter)
{
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::CopySurface);
    m_pCB->PutDWORD(pDestSurface);
    m_pCB->PutDWORD(pDestRect);
    m_pCB->PutDWORD(pSrcSurface);
    m_pCB->PutDWORD(pSrcRect);
    m_pCB->PutDWORD(Filter);
    return D3D_OK;	
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::SwapChainPresent(	IDirect3DSwapChain9* pSwapChain,
															const RECT *pSourceRect,
															const RECT *pDestRect,
															HWND hDestWindowOverride,
															const RGNDATA *pDirtyRegion,
															DWORD dwFlags)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SwapChainPresent);
	m_pCB->PutDWORD(pSwapChain);
	m_pCB->PutDWORD(pSourceRect);
	m_pCB->PutDWORD(pDestRect);
	m_pCB->PutDWORD(hDestWindowOverride);
	m_pCB->PutDWORD(pDirtyRegion);
	m_pCB->PutDWORD(dwFlags);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorder9::OccQuery(IA3DOccQueryImpl* pOccQuery, DWORD dwCmd, DWORD dwID)
{
    CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::OccQuery);
    m_pCB->PutDWORD((DWORD)pOccQuery);
    m_pCB->PutDWORD(dwCmd);
    m_pCB->PutDWORD(dwID);
    return D3D_OK;
}