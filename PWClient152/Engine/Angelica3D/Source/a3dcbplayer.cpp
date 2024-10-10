/*
 * FILE: A3DCBPlayer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 6, 25
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#include "stdio.h"
#include "A3DCBPlayer.h"
#include "A3DCommandBuffer.h"
#include "A3DPlatform.h"
#include "A3DPI.h"
#include "A3DOccQuery.h"

typedef void (A3DCBPlayer9:: *PlayBFuncPTR) ();

// Must access by an (enum - A3DD3D_COMMANDS::NULLCALL)
PlayBFuncPTR	PlaybackFunctionTable[] =
{
	&A3DCBPlayer9::DoNULLCall,
	&A3DCBPlayer9::DoQueryInterface,
	&A3DCBPlayer9::DoAddRef,
	&A3DCBPlayer9::DoRelease,
	&A3DCBPlayer9::DoDrawPrimitive,
	&A3DCBPlayer9::DoDrawIndexedPrimitive,
	&A3DCBPlayer9::DoSetStreamSource,
	&A3DCBPlayer9::DoSetStreamSourceFreq,
	&A3DCBPlayer9::DoSetVertexDeclaration,
	&A3DCBPlayer9::DoSetIndices,
	&A3DCBPlayer9::DoTestCooperativeLevel,
	&A3DCBPlayer9::DoEvictManagedResources,
	&A3DCBPlayer9::DoBeginScene,
	&A3DCBPlayer9::DoEndScene,
	&A3DCBPlayer9::DoBeginStateBlock,
	&A3DCBPlayer9::DoSetCursorProperties,
	&A3DCBPlayer9::DoSetCursorPosition,
	&A3DCBPlayer9::DoSetDialogBoxMode,
	&A3DCBPlayer9::DoGetDeviceCaps,
	&A3DCBPlayer9::DoGetDisplayMode,
	&A3DCBPlayer9::DoGetCreationParameters,
	&A3DCBPlayer9::DoCreateAdditionalSwapChain,
	&A3DCBPlayer9::DoGetSwapChain,
	&A3DCBPlayer9::DoReset,
	&A3DCBPlayer9::DoPresent,
	&A3DCBPlayer9::DoGetBackBuffer,
	&A3DCBPlayer9::DoGetRasterStatus,
	&A3DCBPlayer9::DoSetGammaRamp,
	&A3DCBPlayer9::DoGetGammaRamp,
	&A3DCBPlayer9::DoCreateTexture,
	&A3DCBPlayer9::DoCreateVolumeTexture,
	&A3DCBPlayer9::DoCreateCubeTexture,
	&A3DCBPlayer9::DoCreateVertexBuffer,
	&A3DCBPlayer9::DoCreateIndexBuffer,
	&A3DCBPlayer9::DoCreateRenderTarget,
	&A3DCBPlayer9::DoCreateDepthStencilSurface,
	&A3DCBPlayer9::DoUpdateSurface,
	&A3DCBPlayer9::DoUpdateTexture,
	&A3DCBPlayer9::DoGetRenderTargetData,
	&A3DCBPlayer9::DoGetFrontBufferData,
	&A3DCBPlayer9::DoStretchRect,
	&A3DCBPlayer9::DoColorFill,
	&A3DCBPlayer9::DoCreateOffscreenPlainSurface,
	&A3DCBPlayer9::DoSetRenderTarget,
	&A3DCBPlayer9::DoGetRenderTarget,
	&A3DCBPlayer9::DoSetDepthStencilSurface,
	&A3DCBPlayer9::DoGetDepthStencilSurface,
	&A3DCBPlayer9::DoClear,
	&A3DCBPlayer9::DoSetTransform,
	&A3DCBPlayer9::DoGetTransform,
	&A3DCBPlayer9::DoMultiplyTransform,
	&A3DCBPlayer9::DoSetViewport,
	&A3DCBPlayer9::DoGetViewport,
	&A3DCBPlayer9::DoSetMaterial,
	&A3DCBPlayer9::DoGetMaterial,
	&A3DCBPlayer9::DoSetLight,
	&A3DCBPlayer9::DoGetLight,
	&A3DCBPlayer9::DoLightEnable,
	&A3DCBPlayer9::DoGetLightEnable,
	&A3DCBPlayer9::DoSetClipPlane,
	&A3DCBPlayer9::DoGetClipPlane,
	&A3DCBPlayer9::DoSetRenderState,
	&A3DCBPlayer9::DoGetRenderState,
	&A3DCBPlayer9::DoCreateStateBlock,
	&A3DCBPlayer9::DoEndStateBlock,
	&A3DCBPlayer9::DoSetClipStatus,
	&A3DCBPlayer9::DoGetClipStatus,
	&A3DCBPlayer9::DoGetTexture,
	&A3DCBPlayer9::DoSetTexture,
	&A3DCBPlayer9::DoGetTextureStageState,
	&A3DCBPlayer9::DoSetTextureStageState,
	&A3DCBPlayer9::DoGetSamplerState,
	&A3DCBPlayer9::DoSetSamplerState,
	&A3DCBPlayer9::DoValidateDevice,
	&A3DCBPlayer9::DoSetPaletteEntries,
	&A3DCBPlayer9::DoGetPaletteEntries,
	&A3DCBPlayer9::DoSetCurrentTexturePalette,
	&A3DCBPlayer9::DoGetCurrentTexturePalette,
	&A3DCBPlayer9::DoSetScissorRect,
	&A3DCBPlayer9::DoGetScissorRect,
	&A3DCBPlayer9::DoSetSoftwareVertexProcessing,
	&A3DCBPlayer9::DoSetNPatchMode,
	&A3DCBPlayer9::DoDrawPrimitiveUP,
	&A3DCBPlayer9::DoDrawIndexedPrimitiveUP,
	&A3DCBPlayer9::DoProcessVertices,
	&A3DCBPlayer9::DoCreateVertexDeclaration,
	&A3DCBPlayer9::DoGetVertexDeclaration,
	&A3DCBPlayer9::DoSetFVF,
	&A3DCBPlayer9::DoGetFVF,
	&A3DCBPlayer9::DoCreateVertexShader,
	&A3DCBPlayer9::DoSetVertexShader,
	&A3DCBPlayer9::DoGetVertexShader,
	&A3DCBPlayer9::DoSetVertexShaderConstantF,
	&A3DCBPlayer9::DoGetVertexShaderConstantF,
	&A3DCBPlayer9::DoSetVertexShaderConstantI,
	&A3DCBPlayer9::DoGetVertexShaderConstantI,
	&A3DCBPlayer9::DoSetVertexShaderConstantB,
	&A3DCBPlayer9::DoGetVertexShaderConstantB,
	&A3DCBPlayer9::DoGetStreamSource,
	&A3DCBPlayer9::DoGetStreamSourceFreq,
	&A3DCBPlayer9::DoGetIndices,
	&A3DCBPlayer9::DoCreatePixelShader,
	&A3DCBPlayer9::DoSetPixelShader,
	&A3DCBPlayer9::DoGetPixelShader,
	&A3DCBPlayer9::DoSetPixelShaderConstantF,
	&A3DCBPlayer9::DoGetPixelShaderConstantF,
	&A3DCBPlayer9::DoSetPixelShaderConstantI,
	&A3DCBPlayer9::DoGetPixelShaderConstantI,
	&A3DCBPlayer9::DoSetPixelShaderConstantB,
	&A3DCBPlayer9::DoGetPixelShaderConstantB,
	&A3DCBPlayer9::DoDrawRectPatch,
	&A3DCBPlayer9::DoDrawTriPatch,
	&A3DCBPlayer9::DoDeletePatch,
	&A3DCBPlayer9::DoCreateQuery,
	&A3DCBPlayer9::DoGetDirect3D,	
	
	// Last supported func, if changed must fix A3DCBPlayer9::Playback logic
    &A3DCBPlayer9::DoLockVertexBuffer,
    &A3DCBPlayer9::DoUnlockVertexBuffer,
    &A3DCBPlayer9::DoLockIndexBuffer,
    &A3DCBPlayer9::DoUnlockIndexBuffer,
    &A3DCBPlayer9::DoLockTexture,
    &A3DCBPlayer9::DoUnlockTexture,
    &A3DCBPlayer9::DoLockSurface,
    &A3DCBPlayer9::DoUnlockSurface,
	&A3DCBPlayer9::DoReleaseResource,
    &A3DCBPlayer9::DoSaveSurface,
    &A3DCBPlayer9::DoCopySurface,
	&A3DCBPlayer9::DoSwapChainPresent,
    &A3DCBPlayer9::DoOccQuery,

	// not implemented
	(PlayBFuncPTR) & A3DCBPlayer9::GetAvailableTextureMem,
	(PlayBFuncPTR) & A3DCBPlayer9::GetNumberOfSwapChains,
	(PlayBFuncPTR) & A3DCBPlayer9::ShowCursor,
	(PlayBFuncPTR) & A3DCBPlayer9::GetSoftwareVertexProcessing,
	(PlayBFuncPTR) & A3DCBPlayer9::GetNPatchMode,


};


A3DCBPlayer9::A3DCBPlayer9() :
	m_pCB(NULL),
	m_pDevice(NULL),
	m_bNeedReset(false)
{
}

//FIXME!! 待整理
void A3DCBPlayer9::Playback(IDirect3DDevice9 *pDevice, A3DCommandBuffer *pCB)
{
	m_pDevice = pDevice;
	m_pCB = pCB;

	bool	target = false;
	m_pCB->ResetNext();

	DWORD	dwLastToken = 0;
	DWORD	dwLastFuncToken = 0;
	DWORD	dwInstructionCount = 0;
	bool	bParsingCommand = true;
	while(bParsingCommand && m_pCB->GetAvailableMemorySize() > 0)
	{
		//if( m_bNeedReset)
		//	break;

		DWORD	dwFunc = m_pCB->DoGetDWORD();
		if(dwFunc == A3DD3D_COMMANDS::NULLCALL) //FIXME!! 不支持的指令
		{
			m_pCB->ResetNext();
			bParsingCommand = false;
		}
		else if(dwFunc < A3DD3D_COMMANDS::NULLCALL || dwFunc > A3DD3D_COMMANDS::CommondLastSupported) 
		{
			// this allows you to recover from buffer corruption.
			m_pCB->ResetNext();
			bParsingCommand = false;

			// OutputDebugStringA(__FUNCTION__ ": Invalid function token in command buffer,
			// buffer must be corrupt\n");
			// CBD3D_BREAK();
		}
		else
		{
			target = false;

			static bool bGo = false;
			if(bGo && dwFunc == A3DD3D_COMMANDS::SetTextureStageState) //FIXME!!调试输出纹理层状态
			{
				target = true;

				char	temp[300];
				DWORD	*pNext = m_pCB->DoGetDWORDPTR();
				sprintf_s
				(
					temp,
					299,
					"%s  ( %d %d %d ) \n",
					A3DCommandBuffer::Index2FunctionName(dwFunc),
					pNext[0],
					pNext[1],
					pNext[2]
				);
				OutputDebugStringA(temp);
			}
			(this->*PlaybackFunctionTable[dwFunc - A3DD3D_COMMANDS::NULLCALL]) ();
			dwLastFuncToken = dwFunc;
		}

		dwLastToken = dwFunc;
		dwInstructionCount++;
	}

	m_pDevice = NULL;
	m_pCB = NULL;
}


void A3DCBPlayer9::DoNULLCall()
{
	OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
	__debugbreak();
	return;
}

void A3DCBPlayer9::DoQueryInterface()
{
	OutputDebugStringA("A3DCommandBuffer: " __FUNCTION__ " not implemented via command buffers.\n");
	__debugbreak();
	return;
}

void A3DCBPlayer9::DoAddRef()
{ 
	if(FAILED(m_pDevice->AddRef())) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoRelease()
{ 
	if(FAILED(m_pDevice->Release())) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoDrawPrimitive()
{ 
	D3DPRIMITIVETYPE	arg1;
	m_pCB->GetDWORD(&arg1);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	UINT	arg3;
	m_pCB->GetDWORD(&arg3);
	if(FAILED(m_pDevice->DrawPrimitive(arg1, arg2, arg3))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoDrawIndexedPrimitive()
{ ;
	D3DPRIMITIVETYPE	arg1;
	m_pCB->GetDWORD(&arg1);

	INT arg2;
	m_pCB->GetDWORD(&arg2);

	UINT	arg3;
	m_pCB->GetDWORD(&arg3);

	UINT	arg4;
	m_pCB->GetDWORD(&arg4);

	UINT	arg5;
	m_pCB->GetDWORD(&arg5);

	UINT	arg6;
	m_pCB->GetDWORD(&arg6);
	if(FAILED(m_pDevice->DrawIndexedPrimitive(arg1, arg2, arg3, arg4, arg5, arg6)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetStreamSource()
{ ;
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	IDirect3DVertexBuffer9	*arg2;
	m_pCB->GetDWORD(&arg2);

	UINT	arg3;
	m_pCB->GetDWORD(&arg3);

	UINT	arg4;
	m_pCB->GetDWORD(&arg4);
	if(FAILED(m_pDevice->SetStreamSource(arg1, arg2, arg3, arg4)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetStreamSourceFreq()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->SetStreamSourceFreq(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetVertexDeclaration()
{ 
	IDirect3DVertexDeclaration9 *arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetVertexDeclaration(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetIndices()
{ 
	IDirect3DIndexBuffer9	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetIndices(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoTestCooperativeLevel()
{ 
	if(FAILED(m_pDevice->TestCooperativeLevel())) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoEvictManagedResources()
{ 
	unsigned int nNumMem = m_pDevice->GetAvailableTextureMem()/(1024 * 1024);
	g_A3DErrLog.Log("A3DDevice:EvictManagedResources  Before AvailableTextureMem : %dMB", nNumMem);
	HRESULT hr = m_pDevice->EvictManagedResources();
	nNumMem = m_pDevice->GetAvailableTextureMem()/(1024 * 1024);
	g_A3DErrLog.Log("A3DDevice:EvictManagedResources  Afater AvailableTextureMem : %dMB", nNumMem);

	if( hr == D3D_OK)
	{	
		g_A3DErrLog.Log("A3DDevice:EvictManagedResources OK!!");		
	}
	else if( hr == D3DERR_OUTOFVIDEOMEMORY)
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log("A3DDevice:EvictManagedResources OutOfVideoMemory!!");
	}
	else // if( hr == D3DERR_COMMAND_UNPARSED) //DXSDK bug ?! undefine D3DERR_COMMAND_UNPARSED
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log("A3DDevice:EvictManagedResources Command UnParsed!!");
	}

}

void A3DCBPlayer9::DoBeginScene()
{ 
	if(FAILED(m_pDevice->BeginScene())) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoEndScene()
{ 
	if(FAILED(m_pDevice->EndScene())) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoBeginStateBlock()
{ 
	if(FAILED(m_pDevice->BeginStateBlock())) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetCursorProperties()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	IDirect3DSurface9	*arg3;
	m_pCB->GetDWORD(&arg3);
	if(FAILED(m_pDevice->SetCursorProperties(arg1, arg2, arg3)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetCursorPosition()
{ 
	int arg1;
	m_pCB->GetDWORD(&arg1);

	int arg2;
	m_pCB->GetDWORD(&arg2);

	DWORD	arg3;
	m_pCB->GetDWORD(&arg3);
	m_pDevice->SetCursorPosition(arg1, arg2, arg3);
}


void A3DCBPlayer9::DoSetDialogBoxMode()
{ 
	BOOL	arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetDialogBoxMode(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoReset()
{ 
	D3DPRESENT_PARAMETERS	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->Reset(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoPresent()
{ 
	CONST RECT	*arg1;
	m_pCB->GetDWORD(&arg1);

	CONST RECT	*arg2;
	m_pCB->GetDWORD(&arg2);

	HWND	arg3;
	m_pCB->GetDWORD(&arg3);

	CONST RGNDATA	*arg4;
	m_pCB->GetDWORD(&arg4);
	HRESULT hrVal;
	hrVal = m_pDevice->Present(arg1, arg2, arg3, arg4);
	if( FAILED(hrVal))
	{
		if( hrVal == D3DERR_DEVICELOST)
		{
			HRESULT hr = m_pDevice->TestCooperativeLevel();
			if (hr == D3DERR_DEVICENOTRESET)
			{
				m_bNeedReset = true;
				OutputDebugStringA(__FUNCTION__ " failed in playback\n");
				g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
			}
		}
	}
}

void A3DCBPlayer9::DoSetGammaRamp()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	DWORD	arg2;
	m_pCB->GetDWORD(&arg2);

	CONST D3DGAMMARAMP	*arg3;
	m_pCB->GetDWORD(&arg3);
	m_pDevice->SetGammaRamp(arg1, arg2, arg3);
}

void A3DCBPlayer9::DoUpdateSurface()
{ 
	IDirect3DSurface9	*arg1;
	m_pCB->GetDWORD(&arg1);

	CONST RECT	*arg2;
	m_pCB->GetDWORD(&arg2);

	IDirect3DSurface9	*arg3;
	m_pCB->GetDWORD(&arg3);

	CONST POINT *arg4;
	m_pCB->GetDWORD(&arg4);
	if(FAILED(m_pDevice->UpdateSurface(arg1, arg2, arg3, arg4)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoUpdateTexture()
{ 
	IDirect3DBaseTexture9	*arg1;
	m_pCB->GetDWORD(&arg1);

	IDirect3DBaseTexture9	*arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->UpdateTexture(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoStretchRect()
{ 
	IDirect3DSurface9	*arg1;
	m_pCB->GetDWORD(&arg1);

	CONST RECT	*arg2;
	m_pCB->GetDWORD(&arg2);

	IDirect3DSurface9	*arg3;
	m_pCB->GetDWORD(&arg3);

	CONST RECT	*arg4;
	m_pCB->GetDWORD(&arg4);

	D3DTEXTUREFILTERTYPE	arg5;
	m_pCB->GetDWORD(&arg5);
	if(FAILED(m_pDevice->StretchRect(arg1, arg2, arg3, arg4, arg5)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoColorFill()
{ 
	IDirect3DSurface9	*arg1;
	m_pCB->GetDWORD(&arg1);

	CONST RECT	*arg2;
	m_pCB->GetDWORD(&arg2);

	D3DCOLOR	arg3;
	m_pCB->GetDWORD(&arg3);
	if(FAILED(m_pDevice->ColorFill(arg1, arg2, arg3)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetRenderTarget()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	IDirect3DSurface9	*arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->SetRenderTarget(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetDepthStencilSurface()
{ 
	IDirect3DSurface9	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetDepthStencilSurface(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoClear()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST D3DRECT	*arg2;
	m_pCB->GetDWORD(&arg2);

	DWORD	arg3;
	m_pCB->GetDWORD(&arg3);

	D3DCOLOR	arg4;
	m_pCB->GetDWORD(&arg4);

	float	arg5;
	m_pCB->GetDWORD(&arg5);

	DWORD	arg6;
	m_pCB->GetDWORD(&arg6);
	if(FAILED(m_pDevice->Clear(arg1, arg2, arg3, arg4, arg5, arg6)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetTransform()
{ 
	D3DTRANSFORMSTATETYPE	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST D3DMATRIX *arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->SetTransform(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoMultiplyTransform()
{ 
	D3DTRANSFORMSTATETYPE	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST D3DMATRIX *arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->MultiplyTransform(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetViewport()
{ 
	CONST D3DVIEWPORT9	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetViewport(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetMaterial()
{ 
	CONST D3DMATERIAL9	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetMaterial(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetLight()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST D3DLIGHT9 *arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->SetLight(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoLightEnable()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	BOOL	arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->LightEnable(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetClipPlane()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST float* arg2 = (CONST float*) m_pCB->DoGetMem(sizeof(float) * 4);
	if(FAILED(m_pDevice->SetClipPlane(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetRenderState()
{ 
	D3DRENDERSTATETYPE	arg1;
	m_pCB->GetDWORD(&arg1);

	DWORD	arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->SetRenderState(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoEndStateBlock()
{ 
	IDirect3DStateBlock9	**arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->EndStateBlock(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetClipStatus()
{ 
	CONST D3DCLIPSTATUS9	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetClipStatus(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetTexture()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	IDirect3DBaseTexture9	*arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->SetTexture(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetTextureStageState()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	D3DTEXTURESTAGESTATETYPE	arg2;
	m_pCB->GetDWORD(&arg2);

	DWORD	arg3;
	m_pCB->GetDWORD(&arg3);
	if(FAILED(m_pDevice->SetTextureStageState(arg1, arg2, arg3)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetSamplerState()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);

	D3DSAMPLERSTATETYPE arg2;
	m_pCB->GetDWORD(&arg2);

	DWORD	arg3;
	m_pCB->GetDWORD(&arg3);
	if(FAILED(m_pDevice->SetSamplerState(arg1, arg2, arg3))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoValidateDevice()
{ 
	DWORD	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->ValidateDevice(arg1)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetPaletteEntries()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST PALETTEENTRY	*arg2;
	m_pCB->GetDWORD(&arg2);
	if(FAILED(m_pDevice->SetPaletteEntries(arg1, arg2))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetCurrentTexturePalette()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetCurrentTexturePalette(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetScissorRect()
{ 
	CONST RECT	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetScissorRect(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetSoftwareVertexProcessing()
{ 
	BOOL	arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetSoftwareVertexProcessing(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetNPatchMode()
{ 
	float	arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetNPatchMode(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoDrawPrimitiveUP()
{ 
	D3DPRIMITIVETYPE	PrimitiveType;
	m_pCB->GetDWORD(&PrimitiveType);

	UINT	PrimitiveCount;
	m_pCB->GetDWORD(&PrimitiveCount);

	UINT	VertexStreamZeroStride;
	m_pCB->GetDWORD(&VertexStreamZeroStride);

    UINT	NumVertices;
    m_pCB->GetDWORD(&NumVertices);

    UINT uVertedSize = NumVertices * VertexStreamZeroStride;
    const void* pVertices = m_pCB->DoGetMem(uVertedSize);

    if(FAILED(m_pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertices, VertexStreamZeroStride)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoDrawIndexedPrimitiveUP()
{ 

    D3DPRIMITIVETYPE	PrimitiveType;
    m_pCB->GetDWORD(&PrimitiveType);

    UINT	MinVertexIndex;
    m_pCB->GetDWORD(&MinVertexIndex);

    UINT	NumVertices;
    m_pCB->GetDWORD(&NumVertices);

    UINT	PrimitiveCount;
    m_pCB->GetDWORD(&PrimitiveCount);

    D3DFORMAT	IndexDataFormat;
    m_pCB->GetDWORD(&IndexDataFormat);

    UINT	VertexStreamZeroStride;
    m_pCB->GetDWORD(&VertexStreamZeroStride);

    UINT	IndexSize;
    m_pCB->GetDWORD(&IndexSize);

    UINT uVertedSize = NumVertices * VertexStreamZeroStride;

    const void* pIndexData = m_pCB->DoGetMem(IndexSize);
    const void* pVertexStreamZeroData = m_pCB->DoGetMem(uVertedSize);

	if(FAILED(m_pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, 
                                                PrimitiveCount, pIndexData, IndexDataFormat, 
                                                pVertexStreamZeroData, VertexStreamZeroStride)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoProcessVertices()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	UINT	arg3;
	m_pCB->GetDWORD(&arg3);

	IDirect3DVertexBuffer9	*arg4;
	m_pCB->GetDWORD(&arg4);

	IDirect3DVertexDeclaration9 *arg5;
	m_pCB->GetDWORD(&arg5);

	DWORD	arg6;
	m_pCB->GetDWORD(&arg6);
	if(FAILED(m_pDevice->ProcessVertices(arg1, arg2, arg3, arg4, arg5, arg6)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetFVF()
{ 
	DWORD	arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetFVF(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetVertexShader()
{ 
	IDirect3DVertexShader9	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetVertexShader(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetPixelShader()
{ 
	IDirect3DPixelShader9	*arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->SetPixelShader(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoDrawRectPatch()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST float *arg2;
	m_pCB->GetDWORD(&arg2);

	CONST D3DRECTPATCH_INFO *arg3;
	m_pCB->GetDWORD(&arg3);
	if(FAILED(m_pDevice->DrawRectPatch(arg1, arg2, arg3))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoDrawTriPatch()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);

	CONST float *arg2;
	m_pCB->GetDWORD(&arg2);

	CONST D3DTRIPATCH_INFO	*arg3;
	m_pCB->GetDWORD(&arg3);
	if(FAILED(m_pDevice->DrawTriPatch(arg1, arg2, arg3))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoDeletePatch()
{ 
	UINT	arg1;
	m_pCB->GetDWORD(&arg1);
	if(FAILED(m_pDevice->DeletePatch(arg1))) 
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetVertexShaderConstantF()
{
	UINT	arg0;
	m_pCB->GetDWORD(&arg0);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	int			iSize = arg2 * 4 * sizeof(float);
	CONST float *arg1 = (CONST float *) m_pCB->DoGetMem(iSize);
	if(FAILED(m_pDevice->SetVertexShaderConstantF(arg0, arg1, arg2)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}


void A3DCBPlayer9::DoSetVertexShaderConstantI()
{
	UINT	arg0;
	m_pCB->GetDWORD(&arg0);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	int			iSize = arg2 * 4 * sizeof(int);
	CONST int	*arg1 = (CONST int *) m_pCB->DoGetMem(iSize);
	if(FAILED(m_pDevice->SetVertexShaderConstantI(arg0, arg1, arg2)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetVertexShaderConstantB()
{
	UINT	arg0;
	m_pCB->GetDWORD(&arg0);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	int			iSize = arg2 * 4 * sizeof(BOOL);
	CONST BOOL	*arg1 = (CONST BOOL *) m_pCB->DoGetMem(iSize);
	if(FAILED(m_pDevice->SetVertexShaderConstantB(arg0, arg1, arg2)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetPixelShaderConstantF()
{
	UINT	arg0;
	m_pCB->GetDWORD(&arg0);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	int			iSize = arg2 * 4 * sizeof(float);
	CONST float *arg1 = (CONST float *) m_pCB->DoGetMem(iSize);
	if(FAILED(m_pDevice->SetPixelShaderConstantF(arg0, arg1, arg2)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetPixelShaderConstantI()
{
	UINT	arg0;
	m_pCB->GetDWORD(&arg0);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	int			iSize = arg2 * 4 * sizeof(int);
	CONST int	*arg1 = (CONST int *) m_pCB->DoGetMem(iSize);
	if(FAILED(m_pDevice->SetPixelShaderConstantI(arg0, arg1, arg2)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSetPixelShaderConstantB()
{
	UINT	arg0;
	m_pCB->GetDWORD(&arg0);

	UINT	arg2;
	m_pCB->GetDWORD(&arg2);

	int			iSize = arg2 * 4 * sizeof(BOOL);
	CONST BOOL	*arg1 = (CONST BOOL *) m_pCB->DoGetMem(iSize);
	if(FAILED(m_pDevice->SetPixelShaderConstantB(arg0, arg1, arg2)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoLockVertexBuffer()
{
    IDirect3DVertexBuffer9* pVertexBuffer;
    m_pCB->GetDWORD(&pVertexBuffer);
    DWORD Offset;
    m_pCB->GetDWORD(&Offset);

    DWORD SizeToLock;
    m_pCB->GetDWORD(&SizeToLock);

    DWORD Flags;
    m_pCB->GetDWORD(&Flags);

    void* pDataSrc = m_pCB->DoGetMem(SizeToLock);

    void* pDataDest = NULL;
    if(FAILED(pVertexBuffer->Lock(Offset, SizeToLock, &pDataDest, Flags)))
	{
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
    else
	{
        memcpy(pDataDest, pDataSrc, SizeToLock);
	}
}

void A3DCBPlayer9::DoUnlockVertexBuffer()
{
    IDirect3DVertexBuffer9* pVertexBuffer;
    m_pCB->GetDWORD(&pVertexBuffer);

    if(FAILED(pVertexBuffer->Unlock()))
	{
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoLockIndexBuffer()
{
    IDirect3DIndexBuffer9* pIndexBuffer;
    m_pCB->GetDWORD(&pIndexBuffer);
    DWORD Offset;
    m_pCB->GetDWORD(&Offset);

    DWORD SizeToLock;
    m_pCB->GetDWORD(&SizeToLock);

    DWORD Flags;
    m_pCB->GetDWORD(&Flags);

    void* pDataSrc = m_pCB->DoGetMem(SizeToLock);

    void* pDataDest = NULL;


    if(FAILED(pIndexBuffer->Lock(Offset, SizeToLock, &pDataDest, Flags)))
	{
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
    else
	{
        memcpy(pDataDest, pDataSrc, SizeToLock);
	}
}

void A3DCBPlayer9::DoUnlockIndexBuffer()
{
    IDirect3DIndexBuffer9* pIndexBuffer;
    m_pCB->GetDWORD(&pIndexBuffer);

    if(FAILED(pIndexBuffer->Unlock()))
	{
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoLockTexture()
{
    IDirect3DTexture9* pTexture;
    m_pCB->GetDWORD(&pTexture);
    UINT Level;
    m_pCB->GetDWORD(&Level);

    DWORD SrcPitchWidth;
    m_pCB->GetDWORD(&SrcPitchWidth);

    DWORD SrcPitchHeight;
    m_pCB->GetDWORD(&SrcPitchHeight);

    RECT* pRect;
    m_pCB->GetDWORD(&pRect);

    DWORD Flags;
    m_pCB->GetDWORD(&Flags);

    DWORD dwSize = SrcPitchWidth * SrcPitchHeight;

    BYTE* pDataSrc = (BYTE*)m_pCB->DoGetMem(dwSize);

    D3DLOCKED_RECT rtLock;
    if(FAILED(pTexture->LockRect(Level, &rtLock, pRect, Flags)))
    {
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
        return;
    }
    BYTE* pDataDest = (BYTE*) rtLock.pBits;
    for (DWORD iLine = 0; iLine < SrcPitchHeight; iLine++)
    {
        memcpy(pDataDest, pDataSrc, SrcPitchWidth);
        pDataSrc += SrcPitchWidth;
        pDataDest += rtLock.Pitch;
    }
}

void A3DCBPlayer9::DoUnlockTexture()
{
    IDirect3DTexture9* pTexture;
    m_pCB->GetDWORD(&pTexture);
    UINT Level;
    m_pCB->GetDWORD(&Level);

    if(FAILED(pTexture->UnlockRect(Level)))
	{
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoLockSurface()
{
    IDirect3DSurface9* pSurface;
    m_pCB->GetDWORD(&pSurface);

    DWORD SrcPitchWidth;
    m_pCB->GetDWORD(&SrcPitchWidth);

    DWORD SrcPitchHeight;
    m_pCB->GetDWORD(&SrcPitchHeight);

    RECT* pRect;
    m_pCB->GetDWORD(&pRect);

    DWORD Flags;
    m_pCB->GetDWORD(&Flags);

    DWORD dwSize = SrcPitchWidth * SrcPitchHeight;

    BYTE* pDataSrc = (BYTE*)m_pCB->DoGetMem(dwSize);

    D3DLOCKED_RECT rtLock;
    if(FAILED(pSurface->LockRect(&rtLock, pRect, Flags)))
    {
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
        return;
    }
    BYTE* pDataDest = (BYTE*) rtLock.pBits;
    for (DWORD iLine = 0; iLine < SrcPitchHeight; iLine++)
    {
        memcpy(pDataDest, pDataSrc, SrcPitchWidth);
        pDataSrc += SrcPitchWidth;
        pDataDest += rtLock.Pitch;
    }
}

void A3DCBPlayer9::DoUnlockSurface()
{
    IDirect3DSurface9* pSurface;
    m_pCB->GetDWORD(&pSurface);

    if(FAILED(pSurface->UnlockRect()))
	{
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoReleaseResource()
{
	IUnknown* pResource;
	m_pCB->GetDWORD(&pResource);
	if(FAILED(pResource->Release()))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoSaveSurface()
{
    DWORD nLen;
    m_pCB->GetDWORD(&nLen);
    char* szFileName = (char*) m_pCB->DoGetMem(nLen + 1);

    IDirect3DSurface9* pSurface = NULL;
    m_pCB->GetDWORD(&pSurface);

    D3DXIMAGE_FILEFORMAT fmt;
    m_pCB->GetDWORD((DWORD*)&fmt);

    if(FAILED(D3DXSaveSurfaceToFileA(szFileName, fmt, pSurface, NULL, NULL)))
	{
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
}

void A3DCBPlayer9::DoGetRenderTargetData()
{
	IDirect3DSurface9	*pRenderTarget = NULL;
	m_pCB->GetDWORD(&pRenderTarget);

	IDirect3DSurface9	*pDestSurface = NULL;
	m_pCB->GetDWORD(&pDestSurface);

	if(FAILED(m_pDevice->GetRenderTargetData(pRenderTarget, pDestSurface)))
	{
		OutputDebugStringA(__FUNCTION__ " failed in playback\n");
		g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
	}
	return;
};

void A3DCBPlayer9::DoCopySurface()
{
    IDirect3DSurface9* pDestSurface = NULL;
    m_pCB->GetDWORD(&pDestSurface);

    const RECT* pDestRect = NULL;
    m_pCB->GetDWORD(&pDestRect);

    IDirect3DSurface9* pSrcSurface = NULL;
    m_pCB->GetDWORD(&pSrcSurface);

    const RECT* pSrcRect = NULL;
    m_pCB->GetDWORD(&pSrcRect);

    DWORD Filter;
    m_pCB->GetDWORD(&Filter);

    if(FAILED(D3DXLoadSurfaceFromSurface(pDestSurface, NULL, pDestRect, pSrcSurface, NULL, pSrcRect, Filter, 0)))
    {
        OutputDebugStringA(__FUNCTION__ " failed in playback\n");
        g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
    }
}

void A3DCBPlayer9::DoSwapChainPresent()
{
	IDirect3DSwapChain9* pSwapChan = NULL;
	m_pCB->GetDWORD(&pSwapChan);

	CONST RECT	*arg1;
	m_pCB->GetDWORD(&arg1);

	CONST RECT	*arg2;
	m_pCB->GetDWORD(&arg2);

	HWND	arg3;
	m_pCB->GetDWORD(&arg3);

	CONST RGNDATA	*arg4;
	m_pCB->GetDWORD(&arg4);

	DWORD arg5;
	m_pCB->GetDWORD(&arg5);


	HRESULT hrVal;
	hrVal = pSwapChan->Present(arg1, arg2, arg3, arg4, arg5);
	if( FAILED(hrVal))
	{
		if( hrVal == D3DERR_DEVICELOST)
		{
			HRESULT hr = m_pDevice->TestCooperativeLevel();
			if (hr == D3DERR_DEVICENOTRESET)
			{
				m_bNeedReset = true;
				OutputDebugStringA(__FUNCTION__ " failed in playback\n");
				g_A3DErrLog.Log( __FUNCTION__ " failed in playback");
			}
		}
	}
}

void A3DCBPlayer9::DoOccQuery()
{
    IA3DOccQueryImpl* pOccQuery = NULL;
    m_pCB->GetDWORD((DWORD*)&pOccQuery);

    DWORD	arg1;
    m_pCB->GetDWORD(&arg1);

    DWORD	arg2;
    m_pCB->GetDWORD(&arg2);

    switch(arg1)
    {
    case OCC_CMD_BEGIN:
        if (!pOccQuery->BeginQuery())
        {
            OutputDebugStringA(__FUNCTION__ " failed in playback(BEGIN)\n");
            g_A3DErrLog.Log( __FUNCTION__ " failed in playback(BEGIN)");
        }
        break;
    case OCC_CMD_END:
        pOccQuery->EndQuery(arg2);
        break;
    case OCC_CMD_GET:
        pOccQuery->GetData();
        break;
    default:
        OutputDebugStringA(__FUNCTION__ " failed in playback(Unknown command)\n");
        g_A3DErrLog.Log( __FUNCTION__ " failed in playback(Unknown command)");
    }
}