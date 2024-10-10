/*
 * FILE: A3DCBRecorder.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 6, 25
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3D_CB_RECORDER_H_
#define _A3D_CB_RECORDER_H_

#include "A3DPlatform.h"

#include "A3DCommandBuffer.h"
#include "A3DCBRecorderBase.h"

//typedef UINT * LPUINT;
//typedef D3DCAPS9	*LPD3DCAPS9;
#pragma warning(disable : 4311)
class IA3DOccQueryImpl;

class A3DCBRecorder9 : public A3DCBRecorderBase
{
public:
	A3DCBRecorder9();
	void SetD3DDevice(IDirect3DDevice9* pD3DDevice);
	float GetUsedNumMegaMem() { return m_pCB->GetUsedNumMegaMem();}

	/* IUnknown methods */
	virtual HRESULT STDMETHODCALLTYPE  QueryInterface(REFIID riid, void **ppvObj);
    UINT GetIndexCountOfPrimitive(UINT PrimitiveType, UINT PrimitiveCount);
	virtual HRESULT STDMETHODCALLTYPE DrawPrimitiveUP(	D3DPRIMITIVETYPE	PrimitiveType,
														UINT				PrimitiveCount,
														CONST void			*pVertexStreamZeroData,
														UINT				VertexStreamZeroStride);

	virtual HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE	PrimitiveType,
															UINT				MinVertexIndex,
															UINT				NumVertices,
															UINT				PrimitiveCount,
															CONST void			*pIndexData,
															D3DFORMAT			IndexDataFormat,
															CONST void			*pVertexStreamZeroData,
															UINT				VertexStreamZeroStride);
    // 直接调用D3D
	virtual HRESULT STDMETHODCALLTYPE CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS * pPresentationParameters,
																IDirect3DSwapChain9 **pSwapChain);

	virtual HRESULT STDMETHODCALLTYPE CreateTexture(UINT Width,
													UINT Height,
													UINT Levels,
													DWORD Usage,
													D3DFORMAT Format,
													D3DPOOL Pool,
													IDirect3DTexture9 **ppTexture,
													HANDLE * pSharedHandle);

	virtual HRESULT STDMETHODCALLTYPE CreateVolumeTexture(	UINT Width,
															UINT Height,
															UINT Depth,
															UINT Levels,
															DWORD Usage,
															D3DFORMAT Format,
															D3DPOOL Pool,
															IDirect3DVolumeTexture9 **ppVolumeTexture,
															HANDLE * pSharedHandle);

	virtual HRESULT STDMETHODCALLTYPE CreateCubeTexture(UINT EdgeLength,
														UINT Levels,
														DWORD Usage,
														D3DFORMAT Format,
														D3DPOOL Pool,
														IDirect3DCubeTexture9 **ppCubeTexture,
														HANDLE * pSharedHandle);

	virtual HRESULT STDMETHODCALLTYPE CreateVertexBuffer(	UINT Length,
															DWORD Usage,
															DWORD FVF,
															D3DPOOL Pool,
															IDirect3DVertexBuffer9 **ppVertexBuffer,
															HANDLE * pSharedHandle);
	
	virtual HRESULT STDMETHODCALLTYPE CreateIndexBuffer(	UINT Length,
															DWORD Usage,
															D3DFORMAT Format,
															D3DPOOL Pool,
															IDirect3DIndexBuffer9 **ppIndexBuffer,
															HANDLE * pSharedHandle);
	
	virtual HRESULT STDMETHODCALLTYPE CreateRenderTarget(	UINT Width,
															UINT Height,
															D3DFORMAT Format,
															D3DMULTISAMPLE_TYPE MultiSample,
															DWORD MultisampleQuality,
															BOOL Lockable,
															IDirect3DSurface9 **ppSurface,
															HANDLE * pSharedHandle);
	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilSurface(UINT Width,
																UINT Height,
																D3DFORMAT Format,
																D3DMULTISAMPLE_TYPE MultiSample,
																DWORD MultisampleQuality,
																BOOL Discard,
																IDirect3DSurface9 **ppSurface,
																HANDLE * pSharedHandle);

	virtual HRESULT STDMETHODCALLTYPE CreateOffscreenPlainSurface(	UINT Width,
																	UINT Height,
																	D3DFORMAT Format,
																	D3DPOOL Pool,
																	IDirect3DSurface9 **ppSurface,
																	HANDLE * pSharedHandle);
	
	virtual HRESULT STDMETHODCALLTYPE CreateStateBlock (D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB);
	
	virtual HRESULT STDMETHODCALLTYPE CreateVertexDeclaration(CONST D3DVERTEXELEMENT9 * pVertexElements,
																IDirect3DVertexDeclaration9 **ppDecl);
	virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(CONST DWORD * pFunction, IDirect3DVertexShader9 **ppShader);
	
	virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(CONST DWORD * pFunction, IDirect3DPixelShader9 **ppShader);
    // New methods
    virtual HRESULT STDMETHODCALLTYPE LockVertexBuffer(	IDirect3DVertexBuffer9*	pVertexBuffer,
														DWORD OffsetToLock, 
														DWORD SizeToLock, 
														void** ppbData, 
														DWORD dwFlags);

    virtual HRESULT STDMETHODCALLTYPE UnlockVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer);

    virtual HRESULT STDMETHODCALLTYPE LockIndexBuffer(	IDirect3DIndexBuffer9*	pIndexBuffer,
														DWORD OffsetToLock, DWORD SizeToLock, 
														void** ppbData, 
														DWORD dwFlags);

    virtual HRESULT STDMETHODCALLTYPE UnlockIndexBuffer(IDirect3DIndexBuffer9*	pIndexBuffer);

    virtual HRESULT STDMETHODCALLTYPE LockTexture(	IDirect3DTexture9* pTexture,
													UINT Level, 
													void** ppbData, 
													DWORD SrcPitchWidth, 
													DWORD SrcPitchHeight, 
													RECT* pRect, 
													DWORD dwFlags);

    virtual HRESULT STDMETHODCALLTYPE UnlockTexture (IDirect3DTexture9*	pTexture, UINT Level);

    virtual HRESULT STDMETHODCALLTYPE LockSurface(	IDirect3DSurface9* pSurface,
													void** ppbData, DWORD SrcPitchWidth, 
													DWORD SrcPitchHeight, 
													RECT* pRect, 
													DWORD dwFlags);

    virtual HRESULT STDMETHODCALLTYPE UnlockSurface(IDirect3DSurface9*	pSurface);

	virtual HRESULT STDMETHODCALLTYPE GetDirect3D (IDirect3D9 **ppD3D9);

	virtual HRESULT STDMETHODCALLTYPE GetDeviceCaps (LPD3DCAPS9 pCaps);

	virtual HRESULT STDMETHODCALLTYPE GetDisplayMode (UINT iSwapChain, D3DDISPLAYMODE * pMode);
	
    virtual HRESULT STDMETHODCALLTYPE SaveSurface(const char* szFileName, IDirect3DSurface9* pSurface, DWORD fmt);

	virtual HRESULT STDMETHODCALLTYPE GetRenderTargetData (IDirect3DSurface9 * pRenderTarget, IDirect3DSurface9 * pDestSurface);

    virtual HRESULT STDMETHODCALLTYPE CopySurface(	IDirect3DSurface9* pDestSurface, 
													const RECT* pDestRect,
													IDirect3DSurface9* pSrcSurface, 
													const RECT* pSrcRect, 
													DWORD Filter);
	virtual HRESULT STDMETHODCALLTYPE SwapChainPresent(IDirect3DSwapChain9* pSwapChain,
														const RECT *pSourceRect,
														const RECT *pDestRect,
														HWND hDestWindowOverride,
														const RGNDATA *pDirtyRegion,
														DWORD dwFlags);
    virtual HRESULT STDMETHODCALLTYPE OccQuery(IA3DOccQueryImpl* pOccQuery, DWORD dwCmd, DWORD dwID);

protected:
	IDirect3DDevice9*	m_pD3DDevice;
};

#endif //_A3D_CB_RECORDER_H_