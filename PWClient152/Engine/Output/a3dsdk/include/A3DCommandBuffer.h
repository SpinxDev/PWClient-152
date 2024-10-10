/*
 * FILE: A3DCommandBuffer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2012, 6, 25
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3D_COMMAND_BUFFER_H_
#define _A3D_COMMAND_BUFFER_H_

#include "A3DPlatform.h"
typedef UINT * LPUINT;
typedef D3DCAPS9	*LPD3DCAPS9;

namespace			A3DD3D_COMMANDS
{
	enum
	{
		NULLCALL					= 0xD3D00000,	// High value to catch command buffer errors
		QueryInterface,
		AddRef,
		Release,
		DrawPrimitive,
		DrawIndexedPrimitive,
		SetStreamSource,
		SetStreamSourceFreq,
		SetVertexDeclaration,
		SetIndices,
		TestCooperativeLevel,
		EvictManagedResources,
		BeginScene,
		EndScene,
		BeginStateBlock,
		SetCursorProperties,
		SetCursorPosition,
		SetDialogBoxMode,
		GetDeviceCaps,
		GetDisplayMode,
		GetCreationParameters,
		CreateAdditionalSwapChain,
		GetSwapChain,
		Reset,
		Present,
		GetBackBuffer,
		GetRasterStatus,
		SetGammaRamp,
		GetGammaRamp,
		CreateTexture,
		CreateVolumeTexture,
		CreateCubeTexture,
		CreateVertexBuffer,
		CreateIndexBuffer,
		CreateRenderTarget,
		CreateDepthStencilSurface,
		UpdateSurface,
		UpdateTexture,
		GetRenderTargetData,
		GetFrontBufferData,
		StretchRect,
		ColorFill,
		CreateOffscreenPlainSurface,
		SetRenderTarget,
		GetRenderTarget,
		SetDepthStencilSurface,
		GetDepthStencilSurface,
		Clear,
		SetTransform,
		GetTransform,
		MultiplyTransform,
		SetViewport,
		GetViewport,
		SetMaterial,
		GetMaterial,
		SetLight,
		GetLight,
		LightEnable,
		GetLightEnable,
		SetClipPlane,
		GetClipPlane,
		SetRenderState,
		GetRenderState,
		CreateStateBlock,
		EndStateBlock,
		SetClipStatus,
		GetClipStatus,
		GetTexture,
		SetTexture,
		GetTextureStageState,
		SetTextureStageState,
		GetSamplerState,
		SetSamplerState,
		ValidateDevice,
		SetPaletteEntries,
		GetPaletteEntries,
		SetCurrentTexturePalette,
		GetCurrentTexturePalette,
		SetScissorRect,
		GetScissorRect,
		SetSoftwareVertexProcessing,
		SetNPatchMode,
		DrawPrimitiveUP,
		DrawIndexedPrimitiveUP,
		ProcessVertices,
		CreateVertexDeclaration,
		GetVertexDeclaration,
		SetFVF,
		GetFVF,
		CreateVertexShader,
		SetVertexShader,
		GetVertexShader,
		SetVertexShaderConstantF,
		GetVertexShaderConstantF,
		SetVertexShaderConstantI,
		GetVertexShaderConstantI,
		SetVertexShaderConstantB,
		GetVertexShaderConstantB,
		GetStreamSource,
		GetStreamSourceFreq,
		GetIndices,
		CreatePixelShader,
		SetPixelShader,
		GetPixelShader,
		SetPixelShaderConstantF,
		GetPixelShaderConstantF,
		SetPixelShaderConstantI,
		GetPixelShaderConstantI,
		SetPixelShaderConstantB,
		GetPixelShaderConstantB,
		DrawRectPatch,
		DrawTriPatch,
		DeletePatch,
		CreateQuery,
		GetDirect3D,
        //New Added
        LockVertexBuffer,
        UnlockVertexBuffer,
        LockIndexBuffer,
        UnlockIndexBuffer,
        LockTexture,
        UnlockTexture,
        LockSurface,
        UnlockSurface,

		//Release Res
		ReleaseResource,
        SaveSurface,
        CopySurface,
		SwapChainPresent,
        OccQuery,

        CommondLastSupported = OccQuery,
        //Not supported        
        GetAvailableTextureMem,
		GetNumberOfSwapChains,
		ShowCursor,
		GetSoftwareVertexProcessing,
		GetNPatchMode,

        NumCommandBuffer = GetNPatchMode - NULLCALL + 1,
	};
}
// disable type conversion errors
#pragma warning(disable : 4312)
#pragma warning(disable : 4311)
class	A3DCommandBuffer
{
public:
	A3DCommandBuffer();
	~A3DCommandBuffer();
	bool			Init(UINT nSize, UINT nGrowSize);
	void			Release();

	bool			Realloc(UINT nNewSize);
	
	void			ResetNext();
	void			GetCBMemory(DWORD * &pMem, UINT &iNumDWORDs);

	int				GetUsedNumDWORD(){ return int(m_pNext - m_pMem );}
	float			GetUsedNumMegaMem() { return float(m_pNext - m_pMem) * 4.0f/(1024.0f * 1024.0f);}

	//避免锁定多个Buffer的同时出现CB内存不足Relloc，造成数据指针写入错误的bug
	void			LockVertexBuffer() { m_bLockVertexBuffer = true;}
	void			UnlockVertexBuffer() { m_bLockVertexBuffer = false;}
	void			LockIndexBuffer() { m_bLockIndexBuffer = true;}
	void			UnlockIndexBuffer() { m_bLockIndexBuffer = false;}
	void			LockTexture() { m_bLockTexture = true;}
	void			UnlockTexture() { m_bLockTexture = false;}
	void			LockSurface() { m_bLockSurface = true;}
	void			UnlockSurface() { m_bLockSurface = false;}

	
	inline DWORD	DoGetDWORD();
	inline DWORD	*DoGetDWORDPTR();	

	inline DWORD	*DoGetMem(DWORD memsize);
    static char		*strFuncNames[A3DD3D_COMMANDS::NumCommandBuffer];

	static const char *Index2FunctionName(unsigned int index)
	{
        if (index < A3DD3D_COMMANDS::NULLCALL || index >= A3DD3D_COMMANDS::NULLCALL + A3DD3D_COMMANDS::NumCommandBuffer)
            return "InvalidCommand";
        else
		    return A3DCommandBuffer::strFuncNames[index - A3DD3D_COMMANDS::NULLCALL];
	}

	template<class TYPE>
	void DoPutDWORD(TYPE val)
	{
		
		assert(sizeof(TYPE) <= sizeof(DWORD));
		assert(m_pNext >= m_pMem);
		
		if(m_pNext >= (m_pMem + m_iSize))
		{
			Realloc(m_iSize + m_nGrowSize);
		}
		*m_pNext = (DWORD) val;
		m_pNext++;
	}

	template<class TYPE>
	void DoPutMem(TYPE *val, DWORD memsize)
	{
		int memsize_in_DWORDs = (memsize + 3) / 4;	
		assert(m_pNext >= m_pMem);
		if((m_pNext + (memsize_in_DWORDs - 1)) > m_pMem + m_iSize )
		{
			Realloc(m_iSize + m_nGrowSize);
		}

		if(val == NULL)    //写入标志位 
			*m_pNext = 0;  
		else
			*m_pNext = 1;
		m_pNext++;
		if(val != NULL)
		{
			memcpy((void *) m_pNext, (const void *) val, memsize);
			m_pNext += memsize_in_DWORDs;
		}

	}

    // Allocate memory
    void* AllocMem(DWORD memsize)
    {
        int memsize_in_DWORDs = (memsize + 3) / 4;
        assert(m_pNext >= m_pMem);
		if((m_pNext + (memsize_in_DWORDs - 1)) > m_pMem + m_iSize )
		{
			Realloc(m_iSize + m_nGrowSize);
		}

        *m_pNext = 1;
        m_pNext++;

        void* pRet = m_pNext;
        m_pNext += memsize_in_DWORDs;
        return pRet;
    }

	void GetDWORD(D3DCAPS9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DCAPS9));
		*arg = reinterpret_cast < D3DCAPS9 * > (temp);
	}

	void PutDWORD(D3DCAPS9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DCAPS9));
	}

	void GetDWORD(D3DDEVICE_CREATION_PARAMETERS **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DDEVICE_CREATION_PARAMETERS));
		*arg = reinterpret_cast < D3DDEVICE_CREATION_PARAMETERS * > (temp);
	}

	void PutDWORD(D3DDEVICE_CREATION_PARAMETERS *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DDEVICE_CREATION_PARAMETERS));
	}


	void GetDWORD(D3DPRESENT_PARAMETERS **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DPRESENT_PARAMETERS));
		*arg = reinterpret_cast < D3DPRESENT_PARAMETERS * > (temp);
	}

	void PutDWORD(D3DPRESENT_PARAMETERS *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DPRESENT_PARAMETERS));
	}

	void GetDWORD(CONST RECT **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST RECT));
		*arg = reinterpret_cast < CONST RECT * > (temp);
	}

	void PutDWORD(CONST RECT *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST RECT));
	}

	void GetDWORD(CONST RGNDATA **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST RGNDATA));
		*arg = reinterpret_cast < CONST RGNDATA * > (temp);
	}

	void PutDWORD(CONST RGNDATA *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST RGNDATA));
	}

	void GetDWORD(CONST D3DGAMMARAMP **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DGAMMARAMP));
		*arg = reinterpret_cast < CONST D3DGAMMARAMP * > (temp);
	}

	void PutDWORD(CONST D3DGAMMARAMP *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DGAMMARAMP));
	}

	void GetDWORD(D3DGAMMARAMP **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DGAMMARAMP));
		*arg = reinterpret_cast < D3DGAMMARAMP * > (temp);
	}

	void PutDWORD(D3DGAMMARAMP *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DGAMMARAMP));
	}

	void GetDWORD(D3DRASTER_STATUS **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DRASTER_STATUS));
		*arg = reinterpret_cast < D3DRASTER_STATUS * > (temp);
	}

	void PutDWORD(D3DRASTER_STATUS *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DRASTER_STATUS));
	}

	void GetDWORD(CONST POINT **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST POINT));
		*arg = reinterpret_cast < CONST POINT * > (temp);
	}

	void PutDWORD(CONST POINT *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST POINT));
	}

	void GetDWORD(CONST D3DRECT **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DRECT));
		*arg = reinterpret_cast < CONST D3DRECT * > (temp);
	}

	void PutDWORD(CONST D3DRECT *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DRECT));
	}

	void GetDWORD(CONST D3DMATRIX **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DMATRIX));
		*arg = reinterpret_cast < CONST D3DMATRIX * > (temp);
	}

	void PutDWORD(CONST D3DMATRIX *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DMATRIX));
	}

	void GetDWORD(D3DMATRIX **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DMATRIX));
		*arg = reinterpret_cast < D3DMATRIX * > (temp);
	}

	void PutDWORD(D3DMATRIX *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DMATRIX));
	}

	void GetDWORD(CONST D3DVIEWPORT9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DVIEWPORT9));
		*arg = reinterpret_cast < CONST D3DVIEWPORT9 * > (temp);
	}

	void PutDWORD(CONST D3DVIEWPORT9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DVIEWPORT9));
	}

	void GetDWORD(D3DVIEWPORT9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DVIEWPORT9));
		*arg = reinterpret_cast < D3DVIEWPORT9 * > (temp);
	}

	void PutDWORD(D3DVIEWPORT9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DVIEWPORT9));
	}

	void GetDWORD(CONST D3DMATERIAL9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DMATERIAL9));
		*arg = reinterpret_cast < CONST D3DMATERIAL9 * > (temp);
	}

	void PutDWORD(CONST D3DMATERIAL9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DMATERIAL9));
	}

	void GetDWORD(D3DMATERIAL9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DMATERIAL9));
		*arg = reinterpret_cast < D3DMATERIAL9 * > (temp);
	}

	void PutDWORD(D3DMATERIAL9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DMATERIAL9));
	}

	void GetDWORD(CONST D3DLIGHT9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DLIGHT9));
		*arg = reinterpret_cast < CONST D3DLIGHT9 * > (temp);
	}

	void PutDWORD(CONST D3DLIGHT9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DLIGHT9));
	}

	void GetDWORD(D3DLIGHT9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DLIGHT9));
		*arg = reinterpret_cast < D3DLIGHT9 * > (temp);
	}

	void PutDWORD(D3DLIGHT9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DLIGHT9));
	}

	void GetDWORD(CONST PALETTEENTRY **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST PALETTEENTRY));
		*arg = reinterpret_cast < CONST PALETTEENTRY * > (temp);
	}

	void PutDWORD(CONST PALETTEENTRY *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST PALETTEENTRY));
	}

	void GetDWORD(PALETTEENTRY **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(PALETTEENTRY));
		*arg = reinterpret_cast < PALETTEENTRY * > (temp);
	}

	void PutDWORD(PALETTEENTRY *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(PALETTEENTRY));
	}

	void GetDWORD(RECT **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(RECT));
		*arg = reinterpret_cast < RECT * > (temp);
	}

	void PutDWORD(RECT *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(RECT));
	}

	void GetDWORD(CONST D3DVERTEXELEMENT9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DVERTEXELEMENT9));
		*arg = reinterpret_cast < CONST D3DVERTEXELEMENT9 * > (temp);
	}

	void PutDWORD(CONST D3DVERTEXELEMENT9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DVERTEXELEMENT9));
	}

	void GetDWORD(CONST D3DCLIPSTATUS9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DCLIPSTATUS9));
		*arg = reinterpret_cast < CONST D3DCLIPSTATUS9 * > (temp);
	}

	void PutDWORD(CONST D3DCLIPSTATUS9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DCLIPSTATUS9));
	}

	void GetDWORD(D3DCLIPSTATUS9 **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(D3DCLIPSTATUS9));
		*arg = reinterpret_cast < D3DCLIPSTATUS9 * > (temp);
	}

	void PutDWORD(D3DCLIPSTATUS9 *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(D3DCLIPSTATUS9));
	}

	void GetDWORD(CONST D3DRECTPATCH_INFO **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DRECTPATCH_INFO));
		*arg = reinterpret_cast < CONST D3DRECTPATCH_INFO * > (temp);
	}

	void PutDWORD(CONST D3DRECTPATCH_INFO *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DRECTPATCH_INFO));
	}

	void GetDWORD(CONST D3DTRIPATCH_INFO **arg)
	{
		DWORD	*temp = DoGetMem(sizeof(CONST D3DTRIPATCH_INFO));
		*arg = reinterpret_cast < CONST D3DTRIPATCH_INFO * > (temp);
	}

	void PutDWORD(CONST D3DTRIPATCH_INFO *arg)
	{
		DoPutMem((DWORD *) arg, sizeof(CONST D3DTRIPATCH_INFO));
	}

	void GetDWORD(D3DDISPLAYMODE **arg)
	{
		*arg = (D3DDISPLAYMODE *) DoGetDWORD();
	}

	void PutDWORD(D3DDISPLAYMODE *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(BOOL **arg)
	{
		*arg = (BOOL *) DoGetDWORD();
	}

	void PutDWORD(BOOL *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(CONST float **arg)
	{
		*arg = (CONST float *) DoGetDWORD();
	}

	void PutDWORD(CONST float *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(float **arg)
	{
		*arg = (float *) DoGetDWORD();
	}

	void PutDWORD(float *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(DWORD **arg)
	{
		*arg = (DWORD *) DoGetDWORD();
	}

	void PutDWORD(DWORD *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(CONST void **arg)
	{
		*arg = (CONST void *) DoGetDWORD();
	}

	void PutDWORD(CONST void *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(CONST DWORD **arg)
	{
		*arg = (CONST DWORD *) DoGetDWORD();
	}

	void PutDWORD(CONST DWORD *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(CONST int **arg)
	{
		*arg = (CONST int *) DoGetDWORD();
	}

	void PutDWORD(CONST int *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(LPUINT *arg)
	{
		*arg = (LPUINT) DoGetDWORD();
	}


	void PutDWORD(LPUINT arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	// =================================================================================================================
	//    types that can be cast as DWORDs
	// =================================================================================================================
	void GetDWORD(void ***arg)
	{
		*arg = (void **) DoGetDWORD();
	}


	void PutDWORD(void **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DPRIMITIVETYPE *arg)
	{
		*arg = (D3DPRIMITIVETYPE) DoGetDWORD();
	}

	void PutDWORD(D3DPRIMITIVETYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(UINT *arg)
	{
		*arg = (UINT) DoGetDWORD();
	}

	void PutDWORD(UINT arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(INT *arg)
	{
		*arg = (INT) DoGetDWORD();
	}

	void PutDWORD(INT arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void PutDWORD(IUnknown *arg)
	{
		DoPutDWORD((DWORD) arg);
	}
	
	void GetDWORD(IUnknown **arg)
	{
		*arg = (IUnknown *) DoGetDWORD();
	}

	void GetDWORD(IDirect3DVertexBuffer9 **arg)
	{
		*arg = (IDirect3DVertexBuffer9 *) DoGetDWORD();
	}

	void PutDWORD(IDirect3DVertexBuffer9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DVertexDeclaration9 **arg)
	{
		*arg = (IDirect3DVertexDeclaration9 *) DoGetDWORD();
	}

	void PutDWORD(IDirect3DVertexDeclaration9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DIndexBuffer9 **arg)
	{
		*arg = (IDirect3DIndexBuffer9 *) DoGetDWORD();
	}

	void PutDWORD(IDirect3DIndexBuffer9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DSurface9 **arg)
	{
		*arg = (IDirect3DSurface9 *) DoGetDWORD();
	}
	void PutDWORD(IDirect3DSurface9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}
	
	void GetDWORD(DWORD *arg)
	{
		*arg = (DWORD) DoGetDWORD();
	}

	void PutDWORD(DWORD arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DSwapChain9 ***arg)
	{
		*arg = (IDirect3DSwapChain9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DSwapChain9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DSwapChain9 **arg)
	{
		*arg = (IDirect3DSwapChain9 *) DoGetDWORD();
	}

	void PutDWORD(IDirect3DSwapChain9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(HWND *arg)
	{
		*arg = (HWND) DoGetDWORD();
	}

	void PutDWORD(HWND arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DBACKBUFFER_TYPE *arg)
	{
		*arg = (D3DBACKBUFFER_TYPE) DoGetDWORD();
	}

	void PutDWORD(D3DBACKBUFFER_TYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DSurface9 ***arg)
	{
		*arg = (IDirect3DSurface9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DSurface9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DFORMAT *arg)
	{
		*arg = (D3DFORMAT) DoGetDWORD();
	}

	void PutDWORD(D3DFORMAT arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DPOOL *arg)
	{
		*arg = (D3DPOOL) DoGetDWORD();
	}

	void PutDWORD(D3DPOOL arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DTexture9 ***arg)
	{
		*arg = (IDirect3DTexture9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DTexture9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

    void GetDWORD(IDirect3DTexture9 **arg)
    {
        *arg = (IDirect3DTexture9 *) DoGetDWORD();
    }

    void PutDWORD(IDirect3DTexture9 *arg)
    {
        DoPutDWORD((DWORD) arg);
    }

	void GetDWORD(IDirect3DVolumeTexture9 ***arg)
	{
		*arg = (IDirect3DVolumeTexture9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DVolumeTexture9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DCubeTexture9 ***arg)
	{
		*arg = (IDirect3DCubeTexture9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DCubeTexture9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DVertexBuffer9 ***arg)
	{
		*arg = (IDirect3DVertexBuffer9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DVertexBuffer9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DIndexBuffer9 ***arg)
	{
		*arg = (IDirect3DIndexBuffer9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DIndexBuffer9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DMULTISAMPLE_TYPE *arg)
	{
		*arg = (D3DMULTISAMPLE_TYPE) DoGetDWORD();
	}

	void PutDWORD(D3DMULTISAMPLE_TYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DBaseTexture9 **arg)
	{
		*arg = (IDirect3DBaseTexture9 *) DoGetDWORD();
	}


	void PutDWORD(IDirect3DBaseTexture9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DTEXTUREFILTERTYPE *arg)
	{
		*arg = (D3DTEXTUREFILTERTYPE) DoGetDWORD();
	}


	void PutDWORD(D3DTEXTUREFILTERTYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}
	
	void GetDWORD(float *arg)
	{
		*arg = (float) DoGetDWORD();
	}

	void PutDWORD(float arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DTRANSFORMSTATETYPE *arg)
	{
		*arg = (D3DTRANSFORMSTATETYPE) DoGetDWORD();
	}

	void PutDWORD(D3DTRANSFORMSTATETYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DRENDERSTATETYPE *arg)
	{
		*arg = (D3DRENDERSTATETYPE) DoGetDWORD();
	}

	void PutDWORD(D3DRENDERSTATETYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DSTATEBLOCKTYPE *arg)
	{
		*arg = (D3DSTATEBLOCKTYPE) DoGetDWORD();
	}

	void PutDWORD(D3DSTATEBLOCKTYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	
	void GetDWORD(IDirect3DStateBlock9 ***arg)
	{
		*arg = (IDirect3DStateBlock9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DStateBlock9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DBaseTexture9 ***arg)
	{
		*arg = (IDirect3DBaseTexture9 **) DoGetDWORD();
	}

	
	void PutDWORD(IDirect3DBaseTexture9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DTEXTURESTAGESTATETYPE *arg)
	{
		*arg = (D3DTEXTURESTAGESTATETYPE) DoGetDWORD();
	}

	void PutDWORD(D3DTEXTURESTAGESTATETYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DSAMPLERSTATETYPE *arg)
	{
		*arg = (D3DSAMPLERSTATETYPE) DoGetDWORD();
	}

	void PutDWORD(D3DSAMPLERSTATETYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DVertexDeclaration9 ***arg)
	{
		*arg = (IDirect3DVertexDeclaration9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DVertexDeclaration9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DVertexShader9 ***arg)
	{
		*arg = (IDirect3DVertexShader9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DVertexShader9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DVertexShader9 **arg)
	{
		*arg = (IDirect3DVertexShader9 *) DoGetDWORD();
	}

	void PutDWORD(IDirect3DVertexShader9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DPixelShader9 ***arg)
	{
		*arg = (IDirect3DPixelShader9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DPixelShader9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DPixelShader9 **arg)
	{
		*arg = (IDirect3DPixelShader9 *) DoGetDWORD();
	}

	void PutDWORD(IDirect3DPixelShader9 *arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(D3DQUERYTYPE *arg)
	{
		*arg = (D3DQUERYTYPE) DoGetDWORD();
	}

	void PutDWORD(D3DQUERYTYPE arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3DQuery9 ***arg)
	{
		*arg = (IDirect3DQuery9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3DQuery9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	void GetDWORD(IDirect3D9 ***arg)
	{
		*arg = (IDirect3D9 **) DoGetDWORD();
	}

	void PutDWORD(IDirect3D9 **arg)
	{
		DoPutDWORD((DWORD) arg);
	}

	int GetAvailableMemorySize()
	{
		int ret = (m_iSize - (m_pNext - m_pMem) / sizeof(DWORD));
		return ret;
	}

	// =================================================================================================================
	//    check if there is iSize DWORDs left
	// =================================================================================================================
	bool CheckAvailableMemorySize(int iSize)
	{
		int AvailableMemory = GetAvailableMemorySize();
		;
		return(iSize <= AvailableMemory);
	}

	// pointer to memory given to us by app
	DWORD	*m_pMem;

	// size of memory in DWORDs.
	UINT	m_iSize;

	// pointer to current token position in memory
	DWORD	*m_pNext;

	UINT	m_nGrowSize;

	bool	m_bLockVertexBuffer;
	bool	m_bLockIndexBuffer;
	bool	m_bLockTexture;
	bool	m_bLockSurface;
};	// class CBMemoryBuffer


//只在CBPlayer9::Playback() 中用于调试SetTextureStageState 时，
//输出了纹理层渲染状态
inline DWORD *A3DCommandBuffer::DoGetDWORDPTR()
{
	return m_pNext;
}


inline DWORD A3DCommandBuffer::DoGetDWORD()
{
	assert(m_pNext >= m_pMem);
	assert(m_pNext < (m_pMem + m_iSize));

	DWORD	ret = *m_pNext;
	m_pNext++;
	return ret;
}

//参数memsize 为字节数
inline DWORD *A3DCommandBuffer::DoGetMem(DWORD memsize)
{
	int memsize_in_DWORDs = (memsize + 3) / 4;
	assert(m_pNext >= m_pMem);
	assert((m_pNext + (memsize_in_DWORDs - 1) ) < (m_pMem + m_iSize));

	DWORD	val = *m_pNext; //标志位 0表示没有数据， 1表示有数据 ，具体看DoPutMem()函数
	m_pNext++;

	DWORD	*ret = m_pNext;
	if(val != 0)
	{
		m_pNext += memsize_in_DWORDs;
	}
	else
	{
		ret = NULL;
	}

	return ret;
}

#endif //_A3D_COMMAND_BUFFER_H_