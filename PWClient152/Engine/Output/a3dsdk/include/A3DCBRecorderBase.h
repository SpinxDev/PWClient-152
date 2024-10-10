/*
 * FILE: A3DCBRecorderBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 7, 2
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3D_CB_RECORDER_BASE_H_
#define _A3D_CB_RECORDER_BASE_H_

#include "A3DPlatform.h"

#include "A3DCommandBuffer.h"


typedef UINT * LPUINT;
typedef D3DCAPS9	*LPD3DCAPS9;
#pragma warning(disable : 4311)

class A3DCBRecorderBase :public IDirect3DDevice9
{
public:
	A3DCBRecorderBase() :m_pCB(NULL){}

	void SetCommandBuffer(A3DCommandBuffer *pCB)
	{
		m_pCB = pCB;
	}
	A3DCommandBuffer* GetCommandBuffer()
	{
		return m_pCB;
	}
	HRESULT BeginCommandBuffer();
	HRESULT EndCommandBuffer();
	
	/* IUnknown methods */
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObj);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	/* =================================================================================================================
	    Implemnted IDirect3DDevice9 methods ;
	    Note that it is assumed that input values will be valid a record time. ;
	    Also there is no mechnism to retain returned values during playback
	 =================================================================================================================== */
	virtual HRESULT STDMETHODCALLTYPE DrawPrimitive(D3DPRIMITIVETYPE	PrimitiveType,
													UINT				StartVertex,
													UINT				PrimitiveCount);

	virtual HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(D3DPRIMITIVETYPE	PrimitiveType,
															INT					BaseVertexIndex,
															UINT				MinVertexIndex,
															UINT				NumVertices,
															UINT				startIndex,
															UINT				primCount);

	virtual HRESULT STDMETHODCALLTYPE SetStreamSource(	UINT					StreamNumber,
														IDirect3DVertexBuffer9	*pStreamData,
														UINT					OffsetInBytes,
														UINT					Stride);

	virtual HRESULT STDMETHODCALLTYPE SetStreamSourceFreq(UINT StreamNumber, UINT Divider);
	virtual HRESULT STDMETHODCALLTYPE SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl);
	virtual HRESULT STDMETHODCALLTYPE SetIndices(IDirect3DIndexBuffer9 *pIndexData);
	virtual HRESULT STDMETHODCALLTYPE TestCooperativeLevel();
	virtual HRESULT STDMETHODCALLTYPE EvictManagedResources();
	virtual HRESULT STDMETHODCALLTYPE BeginScene();
	virtual HRESULT STDMETHODCALLTYPE EndScene();
	virtual HRESULT STDMETHODCALLTYPE BeginStateBlock();
	virtual HRESULT STDMETHODCALLTYPE SetCursorProperties(	UINT				XHotSpot,
															UINT				YHotSpot,
															IDirect3DSurface9	*pCursorBitmap);

	virtual HRESULT STDMETHODCALLTYPE SetDialogBoxMode(BOOL bEnableDialogs);
	virtual HRESULT STDMETHODCALLTYPE Reset(D3DPRESENT_PARAMETERS *pPresentationParameters);
	virtual HRESULT STDMETHODCALLTYPE Present(CONST RECT		*pSourceRect,
												CONST RECT		*pDestRect,
												HWND			hDestWindowOverride,
												CONST RGNDATA	*pDirtyRegion);

	virtual HRESULT STDMETHODCALLTYPE UpdateSurface(IDirect3DSurface9	*pSourceSurface,
													CONST RECT			*pSourceRect,
													IDirect3DSurface9	*pDestinationSurface,
													CONST POINT			*pDestPoint);

	virtual HRESULT STDMETHODCALLTYPE UpdateTexture(IDirect3DBaseTexture9	*pSourceTexture,
													IDirect3DBaseTexture9	*pDestinationTexture);
	

	virtual HRESULT STDMETHODCALLTYPE StretchRect(	IDirect3DSurface9		*pSourceSurface,
													CONST RECT				*pSourceRect,
													IDirect3DSurface9		*pDestSurface,
													CONST RECT				*pDestRect,
													D3DTEXTUREFILTERTYPE	Filter);

	virtual HRESULT STDMETHODCALLTYPE ColorFill(IDirect3DSurface9 *pSurface, CONST RECT *pRect, D3DCOLOR color);
	virtual HRESULT STDMETHODCALLTYPE SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget);
	virtual HRESULT STDMETHODCALLTYPE SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil);
	virtual HRESULT STDMETHODCALLTYPE Clear(DWORD			Count,
											CONST D3DRECT	*pRects,
											DWORD			Flags,
											D3DCOLOR		Color,
											float			Z,
											DWORD			Stencil);
	virtual HRESULT STDMETHODCALLTYPE SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
	virtual HRESULT STDMETHODCALLTYPE MultiplyTransform(D3DTRANSFORMSTATETYPE type, CONST D3DMATRIX *pMatrix);
	virtual HRESULT STDMETHODCALLTYPE SetViewport(CONST D3DVIEWPORT9 *pViewport);
	virtual HRESULT STDMETHODCALLTYPE SetMaterial(CONST D3DMATERIAL9 *pMaterial);
	virtual HRESULT STDMETHODCALLTYPE SetLight(DWORD Index, CONST D3DLIGHT9 *pLight);
	virtual HRESULT STDMETHODCALLTYPE LightEnable(DWORD Index, BOOL Enable);
	virtual HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD Index, CONST float *pPlane);
	virtual HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	virtual HRESULT STDMETHODCALLTYPE EndStateBlock(IDirect3DStateBlock9 **ppSB);
	virtual HRESULT STDMETHODCALLTYPE SetClipStatus(CONST D3DCLIPSTATUS9 *pClipStatus);
	virtual HRESULT STDMETHODCALLTYPE SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture);
	virtual HRESULT STDMETHODCALLTYPE SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	virtual HRESULT STDMETHODCALLTYPE SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT STDMETHODCALLTYPE ValidateDevice(DWORD *pNumPasses);
	virtual HRESULT STDMETHODCALLTYPE SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY *pEntries);
	virtual HRESULT STDMETHODCALLTYPE SetCurrentTexturePalette(UINT PaletteNumber);
	virtual HRESULT STDMETHODCALLTYPE SetScissorRect(CONST RECT *pRect);
	virtual HRESULT STDMETHODCALLTYPE SetSoftwareVertexProcessing(BOOL bSoftware);
	virtual HRESULT STDMETHODCALLTYPE SetNPatchMode(float nSegments);
	virtual HRESULT STDMETHODCALLTYPE DrawPrimitiveUP(	D3DPRIMITIVETYPE	PrimitiveType,
														UINT				PrimitiveCount,
														CONST void			*pVertexStreamZeroData,
														UINT				VertexStreamZeroStride);

	virtual HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveUP(	D3DPRIMITIVETYPE	PrimitiveType,
																UINT				MinVertexIndex,
																UINT				NumVertices,
																UINT				PrimitiveCount,
																CONST void			*pIndexData,
																D3DFORMAT			IndexDataFormat,
																CONST void			*pVertexStreamZeroData,
																UINT				VertexStreamZeroStride);

	virtual HRESULT STDMETHODCALLTYPE ProcessVertices(  UINT						SrcStartIndex,
														UINT						DestIndex,
														UINT						VertexCount,
														IDirect3DVertexBuffer9		*pDestBuffer,
														IDirect3DVertexDeclaration9 *pVertexDecl,
														DWORD						Flags);
	virtual HRESULT STDMETHODCALLTYPE SetFVF(DWORD FVF);
	virtual HRESULT STDMETHODCALLTYPE SetVertexShader(IDirect3DVertexShader9 *pShader);
	virtual HRESULT STDMETHODCALLTYPE SetPixelShader(IDirect3DPixelShader9 *pShader);
	virtual HRESULT STDMETHODCALLTYPE DrawRectPatch(UINT					Handle,
													CONST float				*pNumSegs,
													CONST D3DRECTPATCH_INFO *pRectPatchInfo);

	virtual HRESULT STDMETHODCALLTYPE DrawTriPatch(	UINT					Handle,
													CONST float				*pNumSegs,
													CONST D3DTRIPATCH_INFO	*pTriPatchInfo);
	virtual HRESULT STDMETHODCALLTYPE DeletePatch(UINT Handle);
	virtual void STDMETHODCALLTYPE SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP *pRamp);
	virtual void STDMETHODCALLTYPE SetCursorPosition(int X, int Y, DWORD Flags);
	

	/* IDirect3DDevice methods that cannot be recorded which throw errors */
	STDMETHOD_ (BOOL, ShowCursor) (BOOL bShow)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return FALSE;
	};
	STDMETHOD_ (UINT, GetAvailableTextureMem) (THIS)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return 0;
	};
	STDMETHOD_ (UINT, GetNumberOfSwapChains) (THIS)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return 0;
	};
	STDMETHOD_ (BOOL, GetSoftwareVertexProcessing) (THIS)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return FALSE;
	};
	STDMETHOD_ (float, GetNPatchMode) (THIS)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return 0.0f;
	};
	STDMETHOD_ (void, GetGammaRamp) (UINT iSwapChain, D3DGAMMARAMP * pRamp)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return;
	};
	STDMETHOD (GetSwapChain) (UINT iSwapChain, IDirect3DSwapChain9 **pSwapChain)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetDeviceCaps) (LPD3DCAPS9 pCaps)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetDisplayMode) (UINT iSwapChain, D3DDISPLAYMODE * pMode)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetCreationParameters) (D3DDEVICE_CREATION_PARAMETERS * pParameters)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetBackBuffer)
		(
			UINT iSwapChain,
			UINT iBackBuffer,
			D3DBACKBUFFER_TYPE Type,
			IDirect3DSurface9 **ppBackBuffer
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetRasterStatus) (UINT iSwapChain, D3DRASTER_STATUS * pRasterStatus)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetPaletteEntries) (UINT PaletteNumber, PALETTEENTRY * pEntries)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetRenderTargetData) (IDirect3DSurface9 * pRenderTarget, IDirect3DSurface9 * pDestSurface)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetFrontBufferData) (UINT iSwapChain, IDirect3DSurface9 * pDestSurface)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetRenderTarget) (DWORD RenderTargetIndex, IDirect3DSurface9 **ppRenderTarget)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetDepthStencilSurface) (IDirect3DSurface9 **ppZStencilSurface)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetTransform) (D3DTRANSFORMSTATETYPE State, D3DMATRIX * pMatrix)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetViewport) (D3DVIEWPORT9 * pViewport)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetMaterial) (D3DMATERIAL9 * pMaterial)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetLightEnable) (DWORD Index, BOOL * pEnable)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetLight) (DWORD Index, D3DLIGHT9 * pLight)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetClipPlane) (DWORD Index, float *pPlane)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetRenderState) (D3DRENDERSTATETYPE State, DWORD * pValue)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetClipStatus) (D3DCLIPSTATUS9 * pClipStatus)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetTexture) (DWORD Stage, IDirect3DBaseTexture9 **ppTexture)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetTextureStageState) (DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD * pValue)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetSamplerState) (DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD * pValue)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetCurrentTexturePalette) (UINT * PaletteNumber)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetScissorRect) (RECT * pRect)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetVertexDeclaration) (IDirect3DVertexDeclaration9 **ppDecl)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetFVF) (DWORD * pFVF)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetPixelShader) (IDirect3DPixelShader9 **ppShader)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetVertexShader) (IDirect3DVertexShader9 **ppShader)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetStreamSource)
		(
			UINT StreamNumber,
			IDirect3DVertexBuffer9 **ppStreamData,
			LPUINT OffsetInBytes,
			LPUINT pStride
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetStreamSourceFreq) (UINT StreamNumber, LPUINT Divider)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetIndices) (IDirect3DIndexBuffer9 **ppIndexData)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateQuery) (D3DQUERYTYPE Type, IDirect3DQuery9 **ppQuery)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetDirect3D) (IDirect3D9 **ppD3D9)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetVertexShaderConstantF) (UINT StartRegister, float *pConstantData, UINT Vector4fCount)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetVertexShaderConstantI) (UINT StartRegister, int *pConstantData, UINT Vector4iCount)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetVertexShaderConstantB) (UINT StartRegister, BOOL * pConstantData, UINT BoolCount)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetPixelShaderConstantF) (UINT StartRegister, float *pConstantData, UINT Vector4fCount)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetPixelShaderConstantI) (UINT StartRegister, int *pConstantData, UINT Vector4iCount)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (GetPixelShaderConstantB) (UINT StartRegister, BOOL * pConstantData, UINT BoolCount)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateAdditionalSwapChain)
		(
			D3DPRESENT_PARAMETERS * pPresentationParameters,
			IDirect3DSwapChain9 **pSwapChain
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateTexture)
		(
			UINT Width,
			UINT Height,
			UINT Levels,
			DWORD Usage,
			D3DFORMAT Format,
			D3DPOOL Pool,
			IDirect3DTexture9 **ppTexture,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateVolumeTexture)
		(
			UINT Width,
			UINT Height,
			UINT Depth,
			UINT Levels,
			DWORD Usage,
			D3DFORMAT Format,
			D3DPOOL Pool,
			IDirect3DVolumeTexture9 **ppVolumeTexture,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateCubeTexture)
		(
			UINT EdgeLength,
			UINT Levels,
			DWORD Usage,
			D3DFORMAT Format,
			D3DPOOL Pool,
			IDirect3DCubeTexture9 **ppCubeTexture,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateVertexBuffer)
		(
			UINT Length,
			DWORD Usage,
			DWORD FVF,
			D3DPOOL Pool,
			IDirect3DVertexBuffer9 **ppVertexBuffer,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateIndexBuffer)
		(
			UINT Length,
			DWORD Usage,
			D3DFORMAT Format,
			D3DPOOL Pool,
			IDirect3DIndexBuffer9 **ppIndexBuffer,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateRenderTarget)
		(
			UINT Width,
			UINT Height,
			D3DFORMAT Format,
			D3DMULTISAMPLE_TYPE MultiSample,
			DWORD MultisampleQuality,
			BOOL Lockable,
			IDirect3DSurface9 **ppSurface,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateDepthStencilSurface)
		(
			UINT Width,
			UINT Height,
			D3DFORMAT Format,
			D3DMULTISAMPLE_TYPE MultiSample,
			DWORD MultisampleQuality,
			BOOL Discard,
			IDirect3DSurface9 **ppSurface,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateOffscreenPlainSurface)
		(
			UINT Width,
			UINT Height,
			D3DFORMAT Format,
			D3DPOOL Pool,
			IDirect3DSurface9 **ppSurface,
			HANDLE * pSharedHandle
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateStateBlock) (D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateVertexDeclaration)
		(
			CONST D3DVERTEXELEMENT9 * pVertexElements,
			IDirect3DVertexDeclaration9 **ppDecl
		)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreateVertexShader) (CONST DWORD * pFunction, IDirect3DVertexShader9 **ppShader)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};
	STDMETHOD (CreatePixelShader) (CONST DWORD * pFunction, IDirect3DPixelShader9 **ppShader)
	{
		OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
		__debugbreak();;
		return D3DERR_INVALIDCALL;
	};

	/* =================================================================================================================
	    IDirect3DDevice9 methods that write variable sized objects
	 =================================================================================================================== */
	virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantF( UINT		StartRegister,
																CONST float *pConstantData,
																UINT		Vector4fCount);

	virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantI( UINT		StartRegister,
																CONST int	*pConstantData,
																UINT		Vector4iCount);

	virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantB( UINT		StartRegister,
																CONST BOOL	*pConstantData,
																UINT		BoolCount);

	virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantF(	UINT		StartRegister,
																CONST float *pConstantData,
																UINT		Vector4fCount);

	virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantI(	UINT		StartRegister,
																CONST int	*pConstantData,
																UINT		Vector4iCount);

	virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantB(	UINT		StartRegister,
																CONST BOOL	*pConstantData,
																UINT		BoolCount);

    DWORD GetCommandSize(DWORD dwCmd) const;

protected:
	A3DCommandBuffer	*m_pCB;
    DWORD m_dwCommandSize[A3DD3D_COMMANDS::NumCommandBuffer];
    void PushCallStack();

    friend class CommandSizeCountWrapper;
};

#define _DEBUG1
#ifdef _DEBUG1
class CommandSizeCountWrapper
{
public:
    CommandSizeCountWrapper(A3DCBRecorderBase* pRecorder, DWORD dwCmd)
    {
        m_iBeginCounter = pRecorder->m_pCB->GetUsedNumDWORD();
        m_pRecorder = pRecorder;
        m_dwCmd = dwCmd;
        pRecorder->m_pCB->PutDWORD(dwCmd);
    }
    ~CommandSizeCountWrapper()
    {
        int iEndSizeCounter = m_pRecorder->m_pCB->GetUsedNumDWORD();
        m_pRecorder->m_dwCommandSize[m_dwCmd - A3DD3D_COMMANDS::NULLCALL] += 1;//(iEndSizeCounter - m_iBeginCounter) * sizeof(DWORD);
    }

private:
    A3DCBRecorderBase* m_pRecorder;
    DWORD m_dwCmd;
    int m_iBeginCounter;
};
#else
class CommandSizeCountWrapper
{
public:
    CommandSizeCountWrapper(A3DCBRecorderBase* pRecorder, DWORD dwCmd)
    {
        pRecorder->m_pCB->PutDWORD(dwCmd);
    }
    ~CommandSizeCountWrapper()
    {
    }
};

#endif

#endif //_A3D_CB_RECORDER_BASE_H_