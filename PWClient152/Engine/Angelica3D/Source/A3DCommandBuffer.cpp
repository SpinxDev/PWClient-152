/*
 * FILE: A3DCommandBuffer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 6, 25
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPlatform.h"
#include "A3DCommandBuffer.h"
#include "ALog.h"

char *A3DCommandBuffer::strFuncNames[A3DD3D_COMMANDS::NumCommandBuffer] = {
	"NULLCALL",
	"QueryInterface",
	"AddRef",
	"Release",
	"DrawPrimitive",
	"DrawIndexedPrimitive",
	"SetStreamSource",
	"SetStreamSourceFreq",
	"SetVertexDeclaration",
	"SetIndices",
	"TestCooperativeLevel",
	"EvictManagedResources",
	"BeginScene",
	"EndScene",
	"BeginStateBlock",
	"SetCursorProperties",
	"SetCursorPosition",
	"SetDialogBoxMode",
	"GetDeviceCaps",
	"GetDisplayMode",
	"GetCreationParameters",
	"CreateAdditionalSwapChain",
	"GetSwapChain",
	"Reset",
	"Present",
	"GetBackBuffer",
	"GetRasterStatus",
	"SetGammaRamp",
	"GetGammaRamp",
	"CreateTexture",
	"CreateVolumeTexture",
	"CreateCubeTexture",
	"CreateVertexBuffer",
	"CreateIndexBuffer",
	"CreateRenderTarget",
	"CreateDepthStencilSurface",
	"UpdateSurface",
	"UpdateTexture",
	"GetRenderTargetData",
	"GetFrontBufferData",
	"StretchRect",
	"ColorFill",
	"CreateOffscreenPlainSurface",
	"SetRenderTarget",
	"GetRenderTarget",
	"SetDepthStencilSurface",
	"GetDepthStencilSurface",
	"Clear",
	"SetTransform",
	"GetTransform",
	"MultiplyTransform",
	"SetViewport",
	"GetViewport",
	"SetMaterial",
	"GetMaterial",
	"SetLight",
	"GetLight",
	"LightEnable",
	"GetLightEnable",
	"SetClipPlane",
	"GetClipPlane",
	"SetRenderState",
	"GetRenderState",
	"CreateStateBlock",
	"EndStateBlock",
	"SetClipStatus",
	"GetClipStatus",
	"GetTexture",
	"SetTexture",
	"GetTextureStageState",
	"SetTextureStageState",
	"GetSamplerState",
	"SetSamplerState",
	"ValidateDevice",
	"SetPaletteEntries",
	"GetPaletteEntries",
	"SetCurrentTexturePalette",
	"GetCurrentTexturePalette",
	"SetScissorRect",
	"GetScissorRect",
	"SetSoftwareVertexProcessing",
	"SetNPatchMode",
	"DrawPrimitiveUP",
	"DrawIndexedPrimitiveUP",
	"ProcessVertices",
	"CreateVertexDeclaration",
	"GetVertexDeclaration",
	"SetFVF",
	"GetFVF",
	"CreateVertexShader",
	"SetVertexShader",
	"GetVertexShader",
	"SetVertexShaderConstantF",
	"GetVertexShaderConstantF",
	"SetVertexShaderConstantI",
	"GetVertexShaderConstantI",
	"SetVertexShaderConstantB",
	"GetVertexShaderConstantB",
	"GetStreamSource",
	"GetStreamSourceFreq",
	"GetIndices",
	"CreatePixelShader",
	"SetPixelShader",
	"GetPixelShader",
	"SetPixelShaderConstantF",
	"GetPixelShaderConstantF",
	"SetPixelShaderConstantI",
	"GetPixelShaderConstantI",
	"SetPixelShaderConstantB",
	"GetPixelShaderConstantB",
	"DrawRectPatch",
	"DrawTriPatch",
	"DeletePatch",
	"CreateQuery",
	"GetDirect3D",
	//New Added
	"LockVertexBuffer",
	"UnlockVertexBuffer",
	"LockIndexBuffer",
	"UnlockIndexBuffer",
	"LockTexture",
	"UnlockTexture",
	"LockSurface",
	"UnlockSurface",

	//Release Res
	"ReleaseResource",
    "SaveSurface",
    "CopySurface",
    "SwapChainPresent",
    "OccQuery",

	"GetAvailableTextureMem",
	"GetNumberOfSwapChains",
	"ShowCursor",
	"GetSoftwareVertexProcessing",
	"GetNPatchMode"
};


A3DCommandBuffer::A3DCommandBuffer() : 
m_pMem(NULL),
m_iSize(0),
m_pNext(NULL),
m_nGrowSize(0),
m_bLockVertexBuffer(false),
m_bLockIndexBuffer(false),
m_bLockTexture(false),
m_bLockSurface(false)
{
}

A3DCommandBuffer::~A3DCommandBuffer()
{
	Release();
}

bool A3DCommandBuffer::Init(UINT nSize, UINT nGrowSize)
{
	m_iSize = nSize;
	m_nGrowSize= nGrowSize;
	return Realloc(nSize);
}
void A3DCommandBuffer::Release()
{
	if( m_pMem)
	{
		delete[] m_pMem;
		m_pMem = NULL;
	}
}

bool A3DCommandBuffer::Realloc(UINT nNewSize)
{
	assert(!m_bLockVertexBuffer && !m_bLockIndexBuffer && !m_bLockTexture && !m_bLockSurface);
	assert(nNewSize >= 0);
	if (!nNewSize)
		return true;

	DWORD* pMem = new DWORD[nNewSize];
	if (!pMem)
	{
		a_LogOutput(1, "A3DCommandBuffer::Realloc, Not enough memory!");
		return false;
	}

	if( m_pMem)
	{
		//	Copy data
		memcpy((void*)pMem, (void*)m_pMem, m_iSize * sizeof(DWORD));
	}

	m_pNext = pMem + (m_pNext - m_pMem);	
	if (m_pMem)
	{
		delete [] m_pMem;
	}

	m_pMem	= pMem;
	m_iSize	= nNewSize;
	return true;
}

void A3DCommandBuffer::ResetNext()
{
	m_pNext = m_pMem;
}

void A3DCommandBuffer::GetCBMemory( DWORD* &pMem, UINT &iNumDWORDs )
{
	pMem = m_pMem; 
	iNumDWORDs = m_iSize;
}

