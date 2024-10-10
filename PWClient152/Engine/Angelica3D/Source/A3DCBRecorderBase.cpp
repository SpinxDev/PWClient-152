/*
 * FILE: A3DCBRecorderBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 8, 9
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */
#include "A3DCBRecorderBase.h"

HRESULT A3DCBRecorderBase::BeginCommandBuffer()
{
	m_pCB->ResetNext();
    ZeroMemory(m_dwCommandSize, sizeof(m_dwCommandSize));
	return D3D_OK;
}
HRESULT A3DCBRecorderBase::EndCommandBuffer()
{
	if(m_pCB->CheckAvailableMemorySize(1))
	{
		m_pCB->PutDWORD(A3DD3D_COMMANDS::NULLCALL); //设置结束标志
		return D3D_OK;
	}

	return E_OUTOFMEMORY;
}

/* IUnknown methods */
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::QueryInterface(REFIID riid, void **ppvObj)
{
	OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n"); //FIXME!!
	__debugbreak();;
	return D3DERR_INVALIDCALL;
}

ULONG STDMETHODCALLTYPE A3DCBRecorderBase::AddRef()
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::AddRef);
	return 1;
}

ULONG STDMETHODCALLTYPE A3DCBRecorderBase::Release()
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::Release);
	return 1;
}

/* =================================================================================================================
    Implemnted IDirect3DDevice9 methods ;
    Note that it is assumed that input values will be valid a record time. ;
    Also there is no mechnism to retain returned values during playback
 =================================================================================================================== */
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::DrawPrimitive(D3DPRIMITIVETYPE	PrimitiveType,
										UINT				StartVertex,
										UINT				PrimitiveCount)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DrawPrimitive);
	m_pCB->PutDWORD(PrimitiveType);
	m_pCB->PutDWORD(StartVertex);
	m_pCB->PutDWORD(PrimitiveCount);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::DrawIndexedPrimitive(	D3DPRIMITIVETYPE	PrimitiveType,
																	INT					BaseVertexIndex,
																	UINT				MinVertexIndex,
																	UINT				NumVertices,
																	UINT				startIndex,
																	UINT				primCount)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DrawIndexedPrimitive);
	m_pCB->PutDWORD(PrimitiveType);
	m_pCB->PutDWORD(BaseVertexIndex);
	m_pCB->PutDWORD(MinVertexIndex);
	m_pCB->PutDWORD(NumVertices);
	m_pCB->PutDWORD(startIndex);
	m_pCB->PutDWORD(primCount);
	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetStreamSource(UINT					StreamNumber,
															IDirect3DVertexBuffer9	*pStreamData,
															UINT					OffsetInBytes,
															UINT					Stride)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetStreamSource);
	m_pCB->PutDWORD(StreamNumber);
	m_pCB->PutDWORD(pStreamData);
	m_pCB->PutDWORD(OffsetInBytes);
	m_pCB->PutDWORD(Stride);
	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetStreamSourceFreq(UINT StreamNumber, UINT Divider)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetStreamSourceFreq);
	m_pCB->PutDWORD(StreamNumber);
	m_pCB->PutDWORD(Divider);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetVertexDeclaration);
	m_pCB->PutDWORD(pDecl);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetIndices(IDirect3DIndexBuffer9 *pIndexData)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetIndices);
	m_pCB->PutDWORD(pIndexData);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::TestCooperativeLevel()
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::TestCooperativeLevel);
	return D3D_OK;
}
 
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::EvictManagedResources()
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::EvictManagedResources);
	return D3D_OK;
}
 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::BeginScene()
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::BeginScene);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::EndScene()
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::EndScene);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::BeginStateBlock()
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::BeginStateBlock);
	return D3D_OK;
}
 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetCursorProperties(	UINT				XHotSpot,
																	 UINT				YHotSpot,
																	IDirect3DSurface9	*pCursorBitmap)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetCursorProperties);
	m_pCB->PutDWORD(XHotSpot);
	m_pCB->PutDWORD(YHotSpot);
	m_pCB->PutDWORD(pCursorBitmap);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetDialogBoxMode(BOOL bEnableDialogs)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetDialogBoxMode);
	m_pCB->PutDWORD(bEnableDialogs);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::Reset);
	m_pCB->PutDWORD(pPresentationParameters);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::Present(  CONST RECT		*pSourceRect,
														CONST RECT		*pDestRect,
														HWND			hDestWindowOverride,
														CONST RGNDATA	*pDirtyRegion)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::Present);
	m_pCB->PutDWORD(pSourceRect);
	m_pCB->PutDWORD(pDestRect);
	m_pCB->PutDWORD(hDestWindowOverride);
	m_pCB->PutDWORD(pDirtyRegion);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::UpdateSurface(IDirect3DSurface9	*pSourceSurface,
															CONST RECT			*pSourceRect,
															IDirect3DSurface9	*pDestinationSurface,
															CONST POINT			*pDestPoint)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::UpdateSurface);
	m_pCB->PutDWORD(pSourceSurface);
	m_pCB->PutDWORD(pSourceRect);
	m_pCB->PutDWORD(pDestinationSurface);
	m_pCB->PutDWORD(pDestPoint);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::UpdateTexture(IDirect3DBaseTexture9	*pSourceTexture,
										IDirect3DBaseTexture9	*pDestinationTexture)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::UpdateTexture);
	m_pCB->PutDWORD(pSourceTexture);
	m_pCB->PutDWORD(pDestinationTexture);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::StretchRect(	IDirect3DSurface9		*pSourceSurface,
										CONST RECT				*pSourceRect,
										IDirect3DSurface9		*pDestSurface,
										CONST RECT				*pDestRect,
										D3DTEXTUREFILTERTYPE	Filter)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::StretchRect);
	m_pCB->PutDWORD(pSourceSurface);
	m_pCB->PutDWORD(pSourceRect);
	m_pCB->PutDWORD(pDestSurface);
	m_pCB->PutDWORD(pDestRect);
	m_pCB->PutDWORD(Filter);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::ColorFill(IDirect3DSurface9 *pSurface, CONST RECT *pRect, D3DCOLOR color)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::ColorFill);
	m_pCB->PutDWORD(pSurface);
	m_pCB->PutDWORD(pRect);
	m_pCB->PutDWORD(color);
	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetRenderTarget);
	m_pCB->PutDWORD(RenderTargetIndex);
	m_pCB->PutDWORD(pRenderTarget);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetDepthStencilSurface(IDirect3DSurface9 *pNewZStencil)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetDepthStencilSurface);
	m_pCB->PutDWORD(pNewZStencil);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::Clear(DWORD			Count,
													CONST D3DRECT	*pRects,
													DWORD			Flags,
													D3DCOLOR		Color,
													float			Z,
													DWORD			Stencil)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::Clear);
	m_pCB->PutDWORD(Count);
	m_pCB->PutDWORD(pRects);
	m_pCB->PutDWORD(Flags);
	m_pCB->PutDWORD(Color);
	m_pCB->PutDWORD(Z);
	m_pCB->PutDWORD(Stencil);
	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetTransform);
	m_pCB->PutDWORD(State);
	m_pCB->PutDWORD(pMatrix);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::MultiplyTransform(D3DTRANSFORMSTATETYPE type, CONST D3DMATRIX *pMatrix)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::MultiplyTransform);
	m_pCB->PutDWORD(type);
	m_pCB->PutDWORD(pMatrix);
	return D3D_OK;
}


HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetViewport(CONST D3DVIEWPORT9 *pViewport)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetViewport);
	m_pCB->PutDWORD(pViewport);
	return D3D_OK;
}
 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetMaterial(CONST D3DMATERIAL9 *pMaterial)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetMaterial);
	m_pCB->PutDWORD(pMaterial);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetLight(DWORD Index, CONST D3DLIGHT9 *pLight)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetLight);
	m_pCB->PutDWORD(Index);
	m_pCB->PutDWORD(pLight);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::LightEnable(DWORD Index, BOOL Enable)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::LightEnable);
	m_pCB->PutDWORD(Index);
	m_pCB->PutDWORD(Enable);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetClipPlane(DWORD Index, CONST float *pPlane)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetClipPlane);
	m_pCB->PutDWORD(Index);
	m_pCB->DoPutMem(pPlane, sizeof(float) * 4);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetRenderState);
	m_pCB->PutDWORD(State);
	m_pCB->PutDWORD(Value);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::EndStateBlock);
	m_pCB->PutDWORD(ppSB);
	return D3D_OK;
}
 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetClipStatus(CONST D3DCLIPSTATUS9 *pClipStatus)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetClipStatus);
	m_pCB->PutDWORD(pClipStatus);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetTexture);
	m_pCB->PutDWORD(Stage);
	m_pCB->PutDWORD(pTexture);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetTextureStageState);
	m_pCB->PutDWORD(Stage);
	m_pCB->PutDWORD(Type);
	m_pCB->PutDWORD(Value);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetSamplerState);
	m_pCB->PutDWORD(Sampler);
	m_pCB->PutDWORD(Type);
	m_pCB->PutDWORD(Value);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::ValidateDevice(DWORD *pNumPasses)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::ValidateDevice);
	m_pCB->PutDWORD(pNumPasses);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY *pEntries)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetPaletteEntries);
	m_pCB->PutDWORD(PaletteNumber);
	m_pCB->PutDWORD(pEntries);
	return D3D_OK;
}


 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetCurrentTexturePalette(UINT PaletteNumber)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetCurrentTexturePalette);
	m_pCB->PutDWORD(PaletteNumber);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetScissorRect(CONST RECT *pRect)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetScissorRect);
	m_pCB->PutDWORD(pRect);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetSoftwareVertexProcessing(BOOL bSoftware)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetSoftwareVertexProcessing);
	m_pCB->PutDWORD(bSoftware);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetNPatchMode(float nSegments)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetNPatchMode);
	m_pCB->PutDWORD(nSegments);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::DrawPrimitiveUP(	D3DPRIMITIVETYPE	PrimitiveType,
																UINT				PrimitiveCount,
																CONST void			*pVertexStreamZeroData,
																UINT				VertexStreamZeroStride)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DrawPrimitiveUP);
	m_pCB->PutDWORD(PrimitiveType);
	m_pCB->PutDWORD(PrimitiveCount);
	m_pCB->PutDWORD(pVertexStreamZeroData);
	m_pCB->PutDWORD(VertexStreamZeroStride);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE	PrimitiveType,
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
	m_pCB->PutDWORD(pIndexData);
	m_pCB->PutDWORD(IndexDataFormat);
	m_pCB->PutDWORD(pVertexStreamZeroData);
	m_pCB->PutDWORD(VertexStreamZeroStride);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::ProcessVertices(UINT						SrcStartIndex,
															UINT						DestIndex,
															UINT						VertexCount,
															IDirect3DVertexBuffer9		*pDestBuffer,
															IDirect3DVertexDeclaration9 *pVertexDecl,
															DWORD						Flags)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::ProcessVertices);
	m_pCB->PutDWORD(SrcStartIndex);
	m_pCB->PutDWORD(DestIndex);
	m_pCB->PutDWORD(VertexCount);
	m_pCB->PutDWORD(pDestBuffer);
	m_pCB->PutDWORD(pVertexDecl);
	m_pCB->PutDWORD(Flags);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetFVF(DWORD FVF)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetFVF);
	m_pCB->PutDWORD(FVF);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetVertexShader(IDirect3DVertexShader9 *pShader)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetVertexShader);
	m_pCB->PutDWORD(pShader);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetPixelShader(IDirect3DPixelShader9 *pShader)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetPixelShader);
	m_pCB->PutDWORD(pShader);
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::DrawRectPatch(UINT					Handle,
															CONST float				*pNumSegs,
															CONST D3DRECTPATCH_INFO *pRectPatchInfo)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DrawRectPatch);
	m_pCB->PutDWORD(Handle);
	m_pCB->PutDWORD(pNumSegs);
	m_pCB->PutDWORD(pRectPatchInfo);
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::DrawTriPatch(	UINT					Handle,
															CONST float				*pNumSegs,
															CONST D3DTRIPATCH_INFO	*pTriPatchInfo)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DrawTriPatch);
	m_pCB->PutDWORD(Handle);
	m_pCB->PutDWORD(pNumSegs);
	m_pCB->PutDWORD(pTriPatchInfo);
	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::DeletePatch(UINT Handle)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::DeletePatch);
	m_pCB->PutDWORD(Handle);
	return D3D_OK;
}

void STDMETHODCALLTYPE A3DCBRecorderBase::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP *pRamp)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetGammaRamp);
	m_pCB->PutDWORD(iSwapChain);
	m_pCB->PutDWORD(Flags);
	m_pCB->PutDWORD(pRamp);
}

 void STDMETHODCALLTYPE A3DCBRecorderBase::SetCursorPosition(int X, int Y, DWORD Flags)
{ 
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetCursorPosition);
	m_pCB->PutDWORD(X);
	m_pCB->PutDWORD(Y);
	m_pCB->PutDWORD(Flags);
}

/* =================================================================================================================
   IDirect3DDevice9 methods that write variable sized objects
=================================================================================================================== */
 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetVertexShaderConstantF(UINT		StartRegister,
																		CONST float *pConstantData,
																		UINT		Vector4fCount)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetVertexShaderConstantF);
	m_pCB->PutDWORD(StartRegister);
	m_pCB->PutDWORD(Vector4fCount);
	m_pCB->DoPutMem((DWORD *) pConstantData, Vector4fCount * 4 * sizeof(float));
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetVertexShaderConstantI(	UINT		StartRegister,
																		CONST int	*pConstantData,
																		UINT		Vector4iCount)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetVertexShaderConstantI);
	m_pCB->PutDWORD(StartRegister);
	m_pCB->PutDWORD(Vector4iCount);
	m_pCB->DoPutMem((DWORD *) pConstantData, Vector4iCount * 4 * sizeof(int));
	return D3D_OK;
}

 HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetVertexShaderConstantB(	UINT		StartRegister,
																		CONST BOOL	*pConstantData,
																		UINT		BoolCount)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetVertexShaderConstantB);
	m_pCB->PutDWORD(StartRegister);
	m_pCB->PutDWORD(BoolCount);
	m_pCB->DoPutMem((DWORD *) pConstantData, BoolCount * 4 * sizeof(BOOL));
	return D3D_OK;
}


HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetPixelShaderConstantF(UINT		StartRegister,
																	CONST float *pConstantData,
																	UINT		Vector4fCount)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetPixelShaderConstantF);
	m_pCB->PutDWORD(StartRegister);
	m_pCB->PutDWORD(Vector4fCount);
	m_pCB->DoPutMem((DWORD *) pConstantData, Vector4fCount * 4 * sizeof(float));
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetPixelShaderConstantI(	UINT		StartRegister,
																		CONST int	*pConstantData,
																		UINT		Vector4iCount)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetPixelShaderConstantI);
	m_pCB->PutDWORD(StartRegister);
	m_pCB->PutDWORD(Vector4iCount);
	m_pCB->DoPutMem((DWORD *) pConstantData, Vector4iCount * 4 * sizeof(int));
	return D3D_OK;
}


HRESULT STDMETHODCALLTYPE A3DCBRecorderBase::SetPixelShaderConstantB(UINT		StartRegister,
																	CONST BOOL	*pConstantData,
																	UINT		BoolCount)
{
	CommandSizeCountWrapper cscw(this, A3DD3D_COMMANDS::SetPixelShaderConstantB);
	m_pCB->PutDWORD(StartRegister);
	m_pCB->PutDWORD(BoolCount);
	m_pCB->DoPutMem((DWORD *) pConstantData, BoolCount * 4 * sizeof(BOOL));
	return D3D_OK;
}

void A3DCBRecorderBase::PushCallStack()
{

}

DWORD A3DCBRecorderBase::GetCommandSize(DWORD dwCmd) const
{
    if (dwCmd > A3DD3D_COMMANDS::NULLCALL && dwCmd <= A3DD3D_COMMANDS::CommondLastSupported)
    {
        return m_dwCommandSize[dwCmd - A3DD3D_COMMANDS::NULLCALL];
    }
    return 0;
}
