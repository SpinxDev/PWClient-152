/*
 * FILE: A3DDevice.h
 *
 * DESCRIPTION: The important class which represent the hardware in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DDEVICE_H_
#define _A3DDEVICE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "AArray.h"
#include "hashtab.h"

#include "A3DCommandBuffer.h"
#include "A3DCBRecorder.h"
#include "A3DCBPlayer.h"
#include "A3DCBReleaseResRecorder.h"

//	A3D device flags
#define A3DDEV_ALLOWMULTITHREAD		0x0001		//	Allow multi-thread
#define A3DDEV_FORCESOFTWARETL		0x0002		//	Force software T&L
#define A3DDEV_ALPHATARGETFIRST		0x0004		//	Chose A8R8G8B8 first
#define A3DDEV_FPU_PRESERVE			0x0008		//	Create device with D3DCREATE_FPU_PRESERVE flag

#define SDM_WIDTH		0x1
#define SDM_HEIGHT		0x2
#define SDM_FORMAT		0x4
#define SDM_WINDOW		0x8
#define SDM_VSYNC		0x10

#define MAX_TEX_LAYERS	8

class A3DEngine;
class A3DLightMan;
class A3DViewport;
class A3DRenderTarget;
class A3DCursor;
class A3DDevObject;
class A3DHLSLCore;
class A3DDeviceCaps;
class A3DRenderTargetMan;
class A3DSurfaceProxy;
class A3DDeviceRSCache;

class A3DDevice
{
public:		//	Type

	//	Skin model rendering method
	enum
	{
		SMRD_VERTEXSHADER = 0,		//	Vertex shader
		SMRD_INDEXEDVERTMATRIX,		//	Indexed vertex matrix
		SMRD_SOFTWARE,				//	Software
		SMRD_TANGENT,				//  Tangent space 
	};

	struct DISPLAY_MODE
	{
		int		nBitsPerPixel;
		int		nFrequency;
		int		nWidth;
		int		nHeight;
	};

// 	struct RENDERSTATES
// 	{
// 		enum
// 		{
// 			MAX_STATES = 256,
// 		};
// 
// 		bool	hasset[MAX_STATES];
// 		DWORD	values[MAX_STATES];
// 
// 		RENDERSTATES() { Reset(); }
// 		void Reset() { memset(values, 0xff, sizeof(DWORD) * MAX_STATES); memset(hasset, 0, sizeof(bool) * MAX_STATES); }
// 	};

// 	struct TEXTURESTAGESTATES
// 	{
// 		enum
// 		{
// 			MAX_STATES = 32,
// 		};
// 		
// 		bool	hasset[MAX_TEX_LAYERS][MAX_STATES];
// 		DWORD	values[MAX_TEX_LAYERS][MAX_STATES];
// 
// 		TEXTURESTAGESTATES() { Reset(); }
// 		void Reset() { memset(values, 0xff, sizeof(DWORD) * MAX_STATES * MAX_TEX_LAYERS); memset(hasset, 0, sizeof(bool) * MAX_STATES * MAX_TEX_LAYERS); }
// 	};

// 	struct TEXTURECOORDINDEX
// 	{	
// 		int		index[MAX_TEX_LAYERS];
// 
// 		TEXTURECOORDINDEX() { Reset(); }
// 		void Reset() { for(int i=0; i<MAX_TEX_LAYERS; i++) index[i] = i; }
// 	};

	typedef abase::hashtab<A3DDevObject*, int, abase::_hash_function> DevObjTable;
	typedef AArray<D3DDISPLAYMODE, const D3DDISPLAYMODE&> DISPLAYMODEARRAY;

	friend class A3DAdditionalView;

private:

	DWORD					m_dwDeviceFlags;
	HWND					m_hDeviceWnd;
	bool					m_bSwitchOff;

	// Flag indicates whether using a hardware independent device;
	bool					m_bHWIDevice;

	bool					m_bAutoReset;		//	flag indicates we want device to do reset by itself automatically
	bool					m_bNeedReset;		//	flag indicates we need do reset now, this is mainly because a devicelost state

	A3DDEVTYPE				m_A3DDevType;

	A3DDEVFMT				m_A3DDevFmt;
	IDirect3DDevice9*		m_pD3DDevice;
	IDirect3DDevice9*		m_pRealD3DDevice;
	
	// Cursor pointer;
	bool					m_bShowCursor;
	A3DCursor *				m_pA3DCursor;

	DISPLAY_MODE			m_DesktopMode;
	
	D3DPRESENT_PARAMETERS	m_d3dpp;

	A3DSurfaceProxy *		m_pBackBuffer;
	A3DSurfaceProxy *		m_pRenderTarget;
	A3DSurfaceProxy *		m_pDepthBuffer;
	A3DRenderTarget*		m_pIntZRT;			// IntZ depth stencil buffer

	D3DCAPS9		m_d3dcaps;
 
	A3DEngine *		m_pA3DEngine;

	A3DCULLTYPE		m_A3DCullType;
	A3DFILLMODE		m_A3DFillMode;

	A3DFORMAT		m_fmtNormalTexture;
	A3DFORMAT		m_fmtColorKeyTexture;
	A3DFORMAT		m_fmtAlphaTexture;

	A3DFORMAT		m_fmtAdapter16;
	A3DFORMAT		m_fmtAdapter32;
	A3DFORMAT		m_fmtTarget16;
	A3DFORMAT		m_fmtTarget32;

// 	A3DMATRIX4		m_matWorldMatrix;
// 	A3DMATRIX4		m_matViewMatrix;
// 	A3DMATRIX4		m_matProjectionMatrix;

	DWORD			m_dwDrawVertCount;
	DWORD			m_dwDrawFaceCount;
	DWORD			m_dwDrawCount;

	A3DCOLOR		m_colorAmbient;
	A3DCOLORVALUE	m_colorValueAmbient;

	bool			m_bFogEnable;			// if disabled, we just set vertex fog and pixel fog to none, but fogenable true
	bool			m_bFogTableEnable;		// pixel-fog auto calculation
	bool			m_bFogVertEnable;		// vertex-fog auto calculation
	A3DFOGMODE		m_fogMode;				// fog calculation mode, default to linear, can can use exp and exp2
	A3DCOLOR		m_colorFog;
	FLOAT			m_vFogStart;
	FLOAT			m_vFogEnd;
	FLOAT			m_vFogDensity;

	// Gamma Level, range [0~200];
	DWORD			m_dwGammaLevel;

	bool			m_bHardwareTL;		//	true, hardware T&L
	int				m_iSkinModelRender;	//	Skin model render method

	//	Unmanaged device object list;
	CRITICAL_SECTION	m_csDevObj;
	DevObjTable			m_UnmanagedDevObjectTab;
	DISPLAYMODEARRAY	m_DisplayModes;	// display modes supported

	//	Render states blocks.
// 	RENDERSTATES			m_RenderStates;
// 	TEXTURESTAGESTATES		m_TextureStageStates;
// 	TEXTURECOORDINDEX		m_TextureCoordIndex;
// 	A3DMATRIX4				m_matTextureMatrix[MAX_TEX_LAYERS];

	//	Render states blocks.
	A3DDeviceRSCache*		m_pRSCache;

	bool					m_bEnableSetAlphaBlend;

	//	Device caps
	A3DDeviceCaps*			m_pDevCaps;

	A3DRenderTargetMan*		m_pRTMan;


	IDirect3DQuery9*		m_pD3DQuery;
	bool					m_bQueryIssued;

	//-------------------------------------------------------------------------
	//multiThread Render---begin
	A3DCommandBuffer m_ComBuffer[2];
	A3DCBRecorder9	m_CBRecorder[2];
	bool			m_bCBValid[2];
	int				m_nCurRecorderIdx;
	A3DCBRecorder9* m_pCurCBRecorder;

	A3DCommandBuffer m_RResComBuffer;
	A3DCBReleaseResRecorder m_CBRResRecorder;

	A3DCBPlayer9	m_CBPlayer;
	A3DCBPlayer9*	m_pCBPlayer;
	bool			m_bResetting;
	//multiThread Render---end
protected:
	friend class A3DRenderTarget;
	friend class A3DCubeRenderTarget;
	friend class A3DRenderTargetMan;
	friend class A3DCubeTexture;
	friend class A3DStream;
	friend class A3DSurface;
	friend class A3DSurfaceMan;
	friend class A3DAdditionalView;
	friend struct A3DFX;
	friend class A3DPixelShader;
	friend class A3DTexture;
	friend class A3DTextureMan;
	friend class A3DVertexShader;
	friend class A3DVolumeTexture;
	friend class A3DTerrainWaterSimpleAlphaRender;
	friend class A3DTextureProxy;
	friend class A3DDeviceRSCache;
	friend class A3DHLSLCore;
	friend class A3DHLSL;
	friend class A3DHLSLDataSrc;
	//friend class A3DShadowMap;
	//friend class A3DCoverageMap;
	friend class A3DOcclusionProxyHW;
	friend class A3DVertexDecl;
	//friend class A3DLitModelRender;
	friend class CECScaleform;
    friend class A3DOccQueryImpl;
	friend class A3DEffect;

	void DetermineSkinModelRenderMethod();
	//	Handle device lost
	bool HandleDeviceLost();

	bool CreateIntZRenderTarget(int nWidth, int nHeight);
public:
	inline IDirect3DDevice9 * GetD3DDevice() { return m_pD3DDevice; }
protected:

	//	Raw interfaces to set D3D state
	HRESULT raw_SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
	HRESULT raw_SetMaterial(CONST D3DMATERIAL9 *pMaterial);
	HRESULT raw_SetLight(DWORD Index, CONST D3DLIGHT9 *pLight);
	HRESULT raw_LightEnable(DWORD Index, BOOL Enable);
	HRESULT raw_SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	HRESULT raw_SetTexture(DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture);
	HRESULT raw_SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	HRESULT raw_SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	HRESULT raw_SetNPatchMode(FLOAT NumSegments);
	HRESULT raw_SetFVF(DWORD FVF);
	HRESULT raw_SetVertexShader(LPDIRECT3DVERTEXSHADER9 pShader);
	HRESULT raw_SetVertexShaderConstantF(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
	HRESULT raw_SetVertexShaderConstantI(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
	HRESULT raw_SetVertexShaderConstantB(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);
	HRESULT raw_SetPixelShader(LPDIRECT3DPIXELSHADER9 pShader);
	HRESULT raw_SetPixelShaderConstantF(UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
	HRESULT raw_SetPixelShaderConstantI(UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
	HRESULT raw_SetPixelShaderConstantB(UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);
	HRESULT raw_SetClipPlane(DWORD Index, CONST FLOAT *pPlane);
	HRESULT raw_SetVertexDeclaration(IDirect3DVertexDeclaration9 *pDecl);
	HRESULT raw_SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
	HRESULT raw_SetIndices(IDirect3DIndexBuffer9* pIndexData);

public:
	A3DDevice();
	~A3DDevice();
	bool Init(A3DEngine* pA3DEngine, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType=A3DDEVTYPE_HAL);
	bool Release();

	static void EnumDisplayModes(IDirect3D9 * pD3D, A3DFORMAT fmtTarget, DISPLAYMODEARRAY& modes);

	// Init one hardware independent device;
	// usually for a pure server version;
	bool InitAsHWI(A3DEngine* pA3DEngine, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType=A3DDEVTYPE_HAL);
	

	bool SetAutoResetFlag(bool bFlag);
	bool SetNeedResetFlag(bool bFlag);
	bool GetNeedResetFlag()					{ return m_bNeedReset; }
	bool GetAutoResetFlag()					{ return m_bAutoReset; }
	bool GetCanResetFlag();
	
	bool Reset();
	bool SetDisplayMode(int nWidth, int nHeight, A3DFORMAT format, bool bWindowed, bool bVSync, DWORD dwFlags);
	bool SetDeviceWnd(HWND hWnd);

	bool ConfigDevice();
	char * GetFormatString(A3DFORMAT format);

	bool SetSoftwareVertexProcessing(bool bEnable);

	bool SetWorldMatrix(const A3DMATRIX4& matWorld, int iIndex=0);
	bool SetViewMatrix(const A3DMATRIX4& matView);
	bool SetProjectionMatrix(const A3DMATRIX4& matProject);
	bool SetTextureMatrix(int iStage, const A3DMATRIX4& matTexture);

	bool BeginRender();
	bool EndRender();
	bool Present();

	bool Clear(DWORD dwFlags, A3DCOLOR color, FLOAT vZ, DWORD dwStencil);

	//Render State Control;
	
	//Z Buffer Control;
	bool SetZTestEnable(bool bEnable);
	bool SetZWriteEnable(bool bEnable);
	bool GetZTestEnable();
	bool GetZWriteEnable();


	bool SetZBias(float fBias);
	bool SetZFunc(DWORD dwFunc);

	//Alpha Blend State Control;
	inline void SetEnableSetAlphaBlend(bool bSet) { m_bEnableSetAlphaBlend = bSet;}
	bool GetAlphaBlendEnable();
	bool SetAlphaBlendEnable(bool bEnable);
	bool SetSourceAlpha(A3DBLEND alphaBlendParam);
	bool SetDestAlpha(A3DBLEND alphaBlendParam);
	
	//Alpha test control;
	bool SetAlphaTestEnable(bool bEnable);
	bool GetAlphaTestEnable();
	bool SetAlphaFunction(A3DCMPFUNC cmpFunc);
	A3DCMPFUNC GetAlphaFunction();
	bool SetAlphaRef(DWORD dwRefValue);
	DWORD GetAlphaRef();

	//Specular control;
	bool SetSpecularEnable(bool bEnable);
	bool SetDitherEnable(bool bEnable);

	//	Clear texture
	bool ClearTexture(int nLayer);
	//	Set texture factor
	bool SetTextureFactor(DWORD dwValue);

	// Texture Filter Flag;
	bool SetTextureFilterType(int nTextureStage, A3DTEXTUREFILTERTYPE filterType);

	//Texture operation flag;
	bool SetTextureColorOP(int nTextureStage, A3DTEXTUREOP op);
	bool SetTextureColorArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2);
	bool SetTextureAlphaOP(int nTextureStage, A3DTEXTUREOP op);
	bool SetTextureAlphaArgs(int nTextureStage, DWORD dwArg1, DWORD dwArg2);
	bool SetTextureResultArg(int nTextureStage, DWORD dwArg);
	A3DTEXTUREOP GetTextureColorOP(int iTextureStage);
	A3DTEXTUREOP GetTextureAlphaOP(int iTextureStage);

	bool SetSamplerState(int nSampler, D3DSAMPLERSTATETYPE eType, DWORD dwValue);
	DWORD GetSamplerState(int nSampler, D3DSAMPLERSTATETYPE eType);
	// Texture address method
	bool SetTextureAddress(int nTextureStage, A3DTEXTUREADDR UAddrMethod, A3DTEXTUREADDR VAddrMethod);
	bool SetTextureCoordIndex(int nTextureStage, int iIndex);

	// Bump-mapping parameters
	bool SetTextureBumpMat(int nTextureStage, float _00, float _01, float _10, float _11);
	bool SetTextureBumpScale(int nTextureStage, float fScale);
	bool SetTextureBumpOffset(int nTextureStage, float fOffset);

	// Enable or disable texture transform
	bool SetTextureTransformFlags(int nTextureStage, A3DTEXTURETRANSFLAGS Flags);

	// Point sprite control;
	bool SetPointSize(FLOAT vPointSize);
	bool SetPointSpriteEnable(bool bEnable);

	//	indexed vertex matrix blend enable
	bool SetIndexedVertexBlendEnable(bool bEnable);
	//	Set blend matrix used in indexed vertex blending
	bool SetIndexedVertexBlendMatrix(int iIndex, const A3DMATRIX4& mat);
	//	Set vertex blend flag
	bool SetVertexBlendFlag(A3DVERTEXBLENDFLAGS Flag);
	//	Clear vertex stream source
	bool ClearStreamSource(int iStream);

	//	Set vertex shader
	bool SetFVF(DWORD dwHandle);

	//	Set pixel shader constants
	bool SetPixelShaderConstants(DWORD dwStart, const void* pData, DWORD dwNumConst);
	//	Set vertex shader constants
	bool SetVertexShaderConstants(DWORD dwStart, const void* pData, DWORD dwNumConst);
	//	Test current pixel shader version with specified version
	bool TestPixelShaderVersion(int iMajor, int iMinor);
	//	Test current vertex shader version with specified version
	bool TestVertexShaderVersion(int iMajor, int iMinor);
	//	Clear current vertex shader
	bool ClearVertexShader();
	//	Clear current pixel shader
	bool ClearPixelShader();

	//Device Caps;
	bool IsDetailMethodSupported();
	inline D3DCAPS9 GetD3DCaps() { return m_d3dcaps; }
	inline int GetMaxSimultaneousTextures()		{	return m_d3dcaps.MaxSimultaneousTextures;	}
	inline bool IsZBiasSupported() { return false; }//(m_d3dcaps.RasterCaps & D3DPRASTERCAPS_ZBIAS) ? true : false; }

	//Light Values;
	bool SetLightingEnable(bool bEnable);
	bool SetAmbient(A3DCOLOR colorAmbient);
	bool SetLightParam(int nLightID, A3DLIGHTPARAM * pLightParam);
	bool LightEnable(int nLightID, bool bEnable);
	inline A3DCOLORVALUE GetAmbientValue() { return m_colorValueAmbient; }
	inline A3DCOLOR GetAmbientColor() { return m_colorAmbient; }

	//Fog Values;
	bool SetFogEnable(bool bEnable);
	bool SetFogTableEnable(bool bEnable);
	bool SetFogVertEnable(bool bEnable);
	bool SetFogMode(A3DFOGMODE fogMode);
	bool SetFogColor(A3DCOLOR fogColor);
	bool SetFogStart(FLOAT vStart);
	bool SetFogEnd(FLOAT vEnd);
	bool SetFogDensity(FLOAT vDensity);

	inline bool GetFogEnable() { return m_bFogEnable; }
	inline bool GetFogTableEnable() { return m_bFogTableEnable; }
	inline bool GetFogVertEnable()	{ return m_bFogVertEnable; }
	inline A3DFOGMODE GetFogMode()	{ return m_fogMode; }
	inline A3DCOLOR GetFogColor() { return m_colorFog; }
	inline FLOAT GetFogStart() { return m_vFogStart; }
	inline FLOAT GetFogEnd() { return m_vFogEnd; }
	inline FLOAT GetFogDensity() { return m_vFogDensity; }
	bool IsActivatedHLSLVS(A3DHLSLCore* pHLSLCore);
	bool IsActivatedHLSLPS(A3DHLSLCore* pHLSLCore);
	
    // Gamma control;
	// Bright Level control; 
	int GetGammaLevel();
	bool SetGammaLevel(DWORD nLevel);
	
	// display modes query methods.
	inline int GetNumDisplayModes() { return m_DisplayModes.GetSize(); }
	const D3DDISPLAYMODE& GetDisplayMode(int nIndex);

	//Material Control;
	bool SetMaterial(const A3DMATERIALPARAM* pMaterialParam, bool b2Sided=false);

	//Viewports;
	bool SetViewport(A3DVIEWPORTPARAM * pViewportParam);
	bool GetViewport(A3DVIEWPORTPARAM * pViewportParam);

	//Face Culling;
	bool SetFaceCull(A3DCULLTYPE type);

	//Fill Mode;
	bool SetFillMode(A3DFILLMODE mode);
	//Clip plane
	bool GetClipPlane(DWORD Index, float* pPlane);
	bool SetClipPlane(DWORD Index, const float* pPlane);
	// programmable pipeline is used  
	bool SetClipPlaneProgrammable(DWORD Index, const D3DXPLANE* pClipPlaneInWorld, const A3DMATRIX4* matVP);

	//Render States
	bool SetDeviceRenderState(DWORD type, DWORD value);
	bool SetDeviceTextureStageState(int layer, DWORD type, DWORD value);
	DWORD GetDeviceRenderState(DWORD type);
	DWORD GetDeviceTextureStageState(int layer, DWORD type);

	//	Add unmanaged device object
	bool AddUnmanagedDevObject(A3DDevObject* pObject);
	//	Remove unmanaged devcie object
	void RemoveUnmanagedDevObject(A3DDevObject* pObject);

	//Creation of A3D Classes;
	bool CreateViewport(A3DViewport ** ppViewport, DWORD x, DWORD y, DWORD width, DWORD height, FLOAT minZ, FLOAT maxZ,
			bool bClearColor, bool bClearZ, A3DCOLOR colorClear=0);
	bool CreateRenderTarget(A3DRenderTarget ** ppRenderTarget, int width, int height,
			A3DFORMAT fmtTarget, A3DFORMAT fmtDepth, bool bNewTarget, bool bNewDepth);

	//Render;
	bool DrawIndexedPrimitive(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD StartIndex, DWORD PrimitiveCount);
	bool DrawIndexedPrimitive(A3DPRIMITIVETYPE Type, int BaseVertexIndex, DWORD MinIndex, DWORD NumVertices, DWORD StartIndex, DWORD PrimitiveCount);
	bool DrawPrimitive(A3DPRIMITIVETYPE Type, DWORD StartVertex, DWORD PrimitiveCount);


	bool DrawIndexedPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD MinIndex, DWORD NumVertices, DWORD PrimitiveCount, void* pIndexData, A3DFORMAT IndexDataFormat, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride);
	bool DrawPrimitiveUP(A3DPRIMITIVETYPE Type, DWORD PrimitiveCount, void* pVertexStreamZeroData, DWORD VertexStreamZeroStride);

	// Cursor functions;
	void ShowCursor(bool bShow);
	void SetCursor(A3DCursor * pA3DCursor);
	inline A3DCursor * GetCursor() { return m_pA3DCursor; }
	inline bool GetShowCursor() { return m_bShowCursor; }

	//Copy Rects
	bool CopyToBack(int x, int y, int width, int height, IDirect3DSurface9 * pSurface);

	bool ExportColorToFile(char * szFullpath, bool bJPG = false);
	bool ExportDepthToFile(char * szFullpath);

	//Some inline functions;
	inline A3DEngine * GetA3DEngine() { return m_pA3DEngine; }
	inline A3DFORMAT GetNormalTextureFormat() { return m_fmtNormalTexture; }
	inline A3DFORMAT GetColorKeyTextureFormat() { return m_fmtColorKeyTexture; }
	inline A3DFORMAT GetAlphaTextureFormat() { return m_fmtAlphaTexture; }
	inline A3DFORMAT GetTarget16() { return m_fmtTarget16; }
	inline A3DFORMAT GetTarget32() { return m_fmtTarget32; }
	const A3DMATRIX4& GetWorldMatrix();
	const A3DMATRIX4& GetViewMatrix();
	const A3DMATRIX4& GetProjectionMatrix();
	inline DWORD GetDrawVertCount() { return m_dwDrawVertCount; }
	inline DWORD GetDrawFaceCount() { return m_dwDrawFaceCount; }
	inline A3DDEVFMT GetDevFormat() { return m_A3DDevFmt; }
	inline A3DDEVTYPE GetDevType()	{ return m_A3DDevType; }
	inline A3DCULLTYPE GetFaceCull() { return m_A3DCullType; }
	inline A3DFILLMODE GetFillMode() { return m_A3DFillMode; }
	inline DWORD GetDrawCount() { return m_dwDrawCount; }

	inline bool IsHardwareTL() { return m_bHardwareTL; }
	inline HWND GetDeviceWnd() { return m_hDeviceWnd; }

	inline int GetMaxVertBlendMatrix() { return (int)m_d3dcaps.MaxVertexBlendMatrices; }
	inline int GetMaxVertBlendMatrixIndex() { return (int)m_d3dcaps.MaxVertexBlendMatrixIndex; } 
	inline int GetVSConstantNum() { return (int)m_d3dcaps.MaxVertexShaderConst; }
	inline bool SupportIndex32() { return m_d3dcaps.MaxVertexIndex > 0xffff ? true : false; }
	inline DWORD GetMaxVertexIndex() { return m_d3dcaps.MaxVertexIndex; }
	inline int GetMaxTextureWidth() { return (int)m_d3dcaps.MaxTextureWidth; }
	inline int GetMaxTextureHeight() { return (int)m_d3dcaps.MaxTextureHeight; }
	A3DFORMAT GetDepthFormat() { return (A3DFORMAT)m_d3dpp.AutoDepthStencilFormat; }

	int GetTextureCoordIndex(int nTextureStage);
	const A3DMATRIX4& GetTextureMatrix(int nTextureStage);

	//	Get current display mode
	inline const DISPLAY_MODE& GetDesktopDisplayMode() { return m_DesktopMode; }
	
	//	Get skin model render method
	inline int GetSkinModelRenderMethod() { return m_iSkinModelRender; } 

	//	Get D3D present parameters
	const D3DPRESENT_PARAMETERS& GetD3DPP() { return m_d3dpp; }

	//	Get IntZ render target
	A3DRenderTarget* GetIntZRenderTarget();

	//Get A3D Device Caps
	inline A3DDeviceCaps* GetA3DDeviceCaps() { return m_pDevCaps;}

	inline A3DRenderTargetMan* GetRenderTargetMan() { return m_pRTMan; }

	A3DSurfaceProxy* GetDefRenderTarget() { return m_pRenderTarget; }
	A3DSurfaceProxy* GetDefDepthBuffer() { return m_pDepthBuffer; }
	A3DSurfaceProxy* GetBackBuffer() { return m_pBackBuffer; }
	IDirect3DSurface9* CreateSystemMemSurface(int iWidth, int iHeight, A3DFORMAT fmt);	
	bool UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, 
		IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestinationPoint);
	bool GetRenderTargetData(IDirect3DSurface9* pRenderTargetSurface, IDirect3DSurface9* pDestSurface);
	bool StretchRect(IDirect3DSurface9* pSourceSurface , const RECT* pSourceRect,
		IDirect3DSurface9* pDestSurface, const RECT* pDestRect, A3DTEXTUREFILTERTYPE Filter);

	void EnableInstancing( int Stream, int num );
	void DisableInstancing( int Stream );

	//multiThread Render --- begin

	void InvalidAllCBRecorder();
	void SetCurCBRecorderValid(bool bValid) { m_bCBValid[m_nCurRecorderIdx] = bValid;}

	void BeginCBRecord(); //在Record之前
	void EndCBRecord();

	void SwitchCBRecorder(); //在CBPlay之前调用

	A3DCBRecorder9* GetCBRecorder();	

	void CopyRResRec2CBRec();

	void Playback(bool bCurRecorder = false);

	HRESULT LockVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer, DWORD OffsetToLock, DWORD SizeToLock, void** ppbData, DWORD dwFlags);
	HRESULT UnlockVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer);
	HRESULT LockIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer, DWORD OffsetToLock, DWORD SizeToLock, void** ppbData, DWORD dwFlags);
	HRESULT UnlockIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer);

    HRESULT LockSurface(IDirect3DSurface9* pSurface,
        void** ppbData, DWORD SrcPitchWidth, DWORD SrcPitchHeight, RECT* pRect, DWORD dwFlags, int* pOutPitch);
    HRESULT UnlockSurface(IDirect3DSurface9* pSurface);

	HRESULT LockTexture(IDirect3DTexture9* pTexture,
		UINT Level, void** ppbData, DWORD SrcPitchWidth, DWORD SrcPitchHeight, RECT* pRect, DWORD dwFlags, int* pOutPitch);
	HRESULT UnlockTexture(IDirect3DTexture9* pTexture, UINT Level);
	static DWORD BitsOfDeviceFormat(const A3DFORMAT eFmt);

	void ReleaseResource(IUnknown* pResource);

	bool SaveSurfaceToFile(const char* szFileName, IDirect3DSurface9* pSurface, D3DXIMAGE_FILEFORMAT fmt);

	bool IsReseting() { return m_bResetting;}

	bool IsCBPlayerNeedReset() { return m_pCBPlayer->IsNeedReset();}
	void SetCBPlayerNeedReset(bool bNeedSet) { m_pCBPlayer->SetNeedReset(bNeedSet);}

    HRESULT CopySurface(IDirect3DSurface9* pDestSurface, const RECT* pDestRect, 
        IDirect3DSurface9* pSrcSurface, const RECT* pSrcRect, DWORD Filter);

	HRESULT SwapChainPresent(IDirect3DSwapChain9* pSwapChain,
		const RECT *pSourceRect,
		const RECT *pDestRect,
		HWND hDestWindowOverride,
		const RGNDATA *pDirtyRegion,
		DWORD dwFlags);
	HRESULT EvictManagedResources();

    bool BeginOccQuery(IA3DOccQueryImpl* pOccQuery);
    void EndOccQuery(IA3DOccQueryImpl* pOccQuery, LONG lID);
    void GetOccQueryData(IA3DOccQueryImpl* pOccQuery);

	//multiThread Render --- end
};

typedef A3DDevice * PA3DDevice;
#endif