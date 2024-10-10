/*
 * FILE: A3DEngine.h
 *
 * DESCRIPTION: The class standing for the A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DENGINE_H_
#define _A3DENGINE_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DControl.h"
#include "AList.h"
#include "ACounter.h"
#include "AAssist.h"

class A2DSpriteTexMan;
class A3DCameraBase;
class A3DCDS;
class A3DFrame;
class A3DFontMan;
class A3DGFXMan;
class A3DLightMan;
class A3DMoxMan;
class A3DTextureMan;
class A3DModelMan;
class A3DSurfaceMan;
class A3DShaderMan;
class A3DImageModelMan;
class A3DViewport;
class A3DRenderTarget;
class A3DWorld;
class A3DFont;
class A3DMeshCollector;
class A3DMeshSorter;
class A3DVertexCollector;
class A3DPlants;
class A3DSkinMan;
class A3DTerrain;
class A3DModel;
class A3DDevice;
class A3DSky;
class A3DFlatCollector;
class A3DWireCollector;
class A3DLitModelRender;
class A3DSkinModelMan;
class A3DHLSLMan;
class A3DPostEffectMan;
class A3DOcclusionMan;
class A3DSkinRenderBase;
class A3DEnvironment;
class A3DShadowMap;

class AMEngine;
class AMSoundEngine;
class AMVideoEngine;

class A3DLitModelSharedDataMan;

//This class is the tool class to contain all a3d specified classes;
class A3DEngine : public A3DControl
{
public:		//	Types

	//	Performance item
	enum
	{
		PERF_DRAWPRIMITIVE = 0,
		PERF_TICK_SKY,
		PERF_TICK_MODEL,
		PERF_TICK_TEXTURE,
		PERF_TICK_GFX,
		PERF_TICK_ANIMATION,
		PERF_TICK_MEDIA,
		PERF_TICK_SCENEUPDATE,
		PERF_TICK_SCENERAYTRACE,
		PERF_TICK_SCENEAABBTRACE,
		PERF_TICK_SCENESPHERETRACE,
		PERF_TICK_SKINMODEL,
		PERF_TICK_SKELETON,
		PERF_TICK_OTHERS,
		PERF_TICK_ALL,
		PERF_RENDER_MESH,
		PERF_RENDER_MESHCOLLECTOR,
		PERF_RENDER_VERTCOLLECTOR,
		PERF_RENDER_SKINMODEL,
		PERF_RENDER_GFX,
		PERF_RENDER_SCENE,
		PERF_RENDER_IMAGEMODEL,
		PERF_RENDER_TEXT,
		PERF_RENDER_OTHERS,
		PERF_RENDER_ALL,
		NUM_PERFORMANCE,
	};

	//	Object counter
	enum
	{
		OBJECTCNT_MODEL	= 0,
		OBJECTCNT_GFX,
		OBJECTCNT_TEXTURE,
		OBJECTCNT_SPRITE, 
		OBJECTCNT_STRING,
		OBJECTCNT_STREAM,
		OBJECTCNT_SKINMODEL,
		NUM_OBJECTCNT,
	};

	//	Frame counters will be cleared every frame
	enum
	{
		FRAMECNT_SCENERAYTRACE = 0,
		FRAMECNT_SCENEAABBTRACE,
		FRAMECNT_SCENESPHERETRACE,
		NUM_FRAMECNT,
	};

	//	Performance item structure
	struct PERFORMANCEITEM
	{
		ACHAR		szTagName[32];
		__int64		i64TimeStart;
		DWORD		dwTimeUsed;
	};

	//	Object counter structure
	struct OBJECTCNTITEM
	{
		ACHAR		szTagName[32];
		int			iObjectCnt;
	};

	//	Frame counters will be cleared every frame
	struct FRAMECNTITEM
	{
		ACHAR		szTagName[32];	//	Description name
		int			iCounter;		//	Counter value
	};

private:

	HWND				m_hDeviceWnd;
	HINSTANCE			m_hInstance;

	A3DDEVFMT			m_devFmt;				//	Device Format	record;

	AList				m_ListViewport;			//	Viewports and Cameras List;
	AList				m_ListRenderTarget;		//	Rendertargets List;

	//	Although here is a model list, but this is used for model editor only;
	//	For game, we must add all models into A3DWorld;
	//	Model List;
	AList				m_ListModel;
	
	//Objects created in this module;
	IDirect3D9*			m_pD3D;

	A3DDevice*			m_pA3DDevice;
	A2DSpriteTexMan*	m_pA2DSpriteTexMan;
	A3DFontMan*			m_pA3DFontMan;
	A3DGFXMan*			m_pA3DGFXMan;
	A3DLightMan*		m_pA3DLightMan;
	A3DMeshCollector*	m_pA3DMeshCollector;
	A3DVertexCollector*	m_pA3DVertexCollector;
	A3DMeshSorter*		m_pA3DMeshSorter;
	A3DMoxMan*			m_pA3DMoxMan;
	A3DModelMan*		m_pA3DModelMan;
	A3DSurfaceMan*		m_pA3DSurfaceMan;
	A3DTextureMan*		m_pA3DTextureMan;
	A3DShaderMan*		m_pA3DShaderMan;
	A3DImageModelMan*	m_pA3DImgModelMan;
	A3DSkinMan*			m_pA3DSkinMan;
	A3DFlatCollector*	m_pA3DFlatCollector;
	A3DWireCollector*	m_pA3DWireCollector;
	A3DLitModelRender*	m_pA3DLitModelRender;		//	Current lit model render
	A3DLitModelRender*	m_pA3DDefLitModelRender;	//	Default lit model render
	A3DSkinModelMan*	m_pA3DSkinModelMan;
	A3DHLSLMan*			m_pHLSLMan;
    A3DOcclusionMan*    m_pA3DOcclusionMan;
	A3DEnvironment*		m_pA3DEnvironment;
	A3DShadowMap*		m_pA3DShadowMap;
    A3DLitModelSharedDataMan*   m_pA3DLitModelSharedDataMan;

	A3DCDS*				m_pA3DCDS;
	//Object Pointer from other module;
	A3DWorld*			m_pA3DWorld;
	A3DViewport*		m_pActiveViewport;
	A3DCameraBase*		m_pActiveCamera;

	//Elements of the scene;
	A3DSky*				m_pSky;
	A3DTerrain*			m_pTerrain;

	//Heart Beat control;
	DWORD				m_dwAnimationTicks;
	DWORD				m_dwRenderTicks;
	DWORD				m_dwLogicTicks;
	ACounter*			m_pRFPSCounter;
	ACounter*			m_pTFPSCounter;

	//	whether we should use optimize algorithm for improve the speed;
	bool				m_bOptimizedEngine;

	//	motion blur flag;
	A3DRenderTarget*	m_pMotionBlurRenderTarget;
	int					m_nMotionBlurValue; // 0~255, corresponding to alpha value remains for each frame;

	//	Font object used to show something;
	A3DFont*			m_pSystemFont;

	//	Debug Section;
	PERFORMANCEITEM		m_aPerformances[NUM_PERFORMANCE];
	OBJECTCNTITEM		m_aObjectCnts[NUM_OBJECTCNT];
	FRAMECNTITEM		m_aFrameCnts[NUM_FRAMECNT];

	// Angelica Media Engine;
	AMEngine *			m_pAMEngine;
	AMSoundEngine *		m_pAMSoundEngine;
	AMVideoEngine *		m_pAMVideoEngine;

	// Angelica Plants Class;
	A3DPlants *			m_pA3DPlants;

	// Some engine state information
	A3DIBLLIGHTPARAM	m_curIBLLightParam;

	//d3d9ex
	bool				m_bSupportD3D9Ex;

protected:

	//	Create system font
	bool CreateSystemFont();
	//	Performace record
	bool DisplayPerformanceRecord();

	//d3d9ex
	IDirect3D9* CheckCreateD3DEx();

public:

	A3DEngine();
	virtual ~A3DEngine();

	bool Init(HINSTANCE hInstance, HWND hDeviceWnd, A3DDEVFMT* pA3DDevFmt, DWORD dwDevFlags, A3DDEVTYPE devType=A3DDEVTYPE_HAL);
	bool Release();
	bool Reset();
	bool ResetResource();

	// Reset some objects that related with world;
	bool ResetWorldRelated();

	bool SetDisplayMode(int nWidth, int nHeight, A3DFORMAT formatTarget, bool bWindowed, bool bVSync, DWORD dwFlags);

	bool BeginRender();
	bool Clear();
	bool EndRender();
	bool RenderScene(A3DViewport * pCurrentViewport);
	bool Present();

	bool PrecacheAllTextures();
	
	bool RenderWorld(A3DViewport * pViewport);
	bool RenderPlants(A3DViewport * pViewport);
	bool BeginCacheAlphaMesh(A3DViewport * pViewport);
	bool FlushCachedAlphaMesh(A3DViewport * pViewport);
	bool RenderGFX(A3DViewport * pViewport, int nCategoryMask);
	bool RenderImageModels(A3DViewport * pViewport);
	bool FlushInternalSpriteBuffer();
	bool FlushVertexCollector(A3DViewport * pViewport);
	bool FlushMeshCollector(A3DViewport * pViewport);

	bool Tick(DWORD dwDeltaTime);
	bool TickAnimation();

	bool SetWorld(A3DWorld * pWorld);
	bool SetSky(A3DSky * pSky);
	bool SetTerrain(A3DTerrain * pTerrain);

	bool AddViewport(A3DViewport * pViewport, ALISTELEMENT ** ppElement);
	bool RemoveViewport(A3DViewport * pViewport);
	bool AddRenderTarget(A3DRenderTarget * pRenderTarget, ALISTELEMENT ** ppElement);
	bool RemoveRenderTarget(A3DRenderTarget * pRenderTarget);
	bool AddModel(A3DModel * pModel, ALISTELEMENT ** ppElement);
	bool RemoveModel(A3DModel * pModel);
	
	// Debug used performance section;
	void BeginPerformanceRecord(int iIndex);
	void EndPerformanceRecord(int iIndex);
	void IncObjectCount(int iIndex);
	void DecObjectCount(int iIndex);
	void IncFrameCount(int iIndex);
	void DecFrameCount(int iIndex);

	inline IDirect3D9 * GetD3D() { return m_pD3D; }
	inline IDirect3D9Ex * GetD3DEx() { return (IDirect3D9Ex*)m_pD3D; }
	inline A3DDevice * GetA3DDevice() { return m_pA3DDevice; }
	inline A2DSpriteTexMan * GetA2DSpriteTexMan() { return m_pA2DSpriteTexMan; }
	inline A3DFontMan * GetA3DFontMan() { return m_pA3DFontMan; }
	inline A3DLightMan * GetA3DLightMan() { return m_pA3DLightMan; }
	inline void	SetActiveViewport(A3DViewport * pViewport) { m_pActiveViewport = pViewport; }
	inline A3DViewport * GetActiveViewport() { return m_pActiveViewport; }
	inline void SetActiveCamera(A3DCameraBase * pCamera) { m_pActiveCamera = pCamera; }
	inline A3DCameraBase * GetActiveCamera();
	inline A3DMeshCollector * GetMeshCollector() { return m_pA3DMeshCollector; }
	inline A3DVertexCollector * GetVertexCollector() { return m_pA3DVertexCollector; }
	inline A3DMeshSorter * GetMeshSorter() { return m_pA3DMeshSorter; }
	inline A3DMoxMan * GetA3DMoxMan() { return m_pA3DMoxMan; };
	inline A3DModelMan * GetA3DModelMan() { return m_pA3DModelMan; }
	inline A3DSurfaceMan * GetA3DSurfaceMan() { return m_pA3DSurfaceMan; }
	inline A3DTextureMan * GetA3DTextureMan() { return m_pA3DTextureMan; }
	inline A3DGFXMan * GetA3DGFXMan() { return m_pA3DGFXMan; }
	inline A3DShaderMan * GetA3DShaderMan() { return m_pA3DShaderMan; }
	inline A3DImageModelMan * GetA3DImageModelMan() { return m_pA3DImgModelMan; }
	inline A3DSkinMan * GetA3DSkinMan() { return m_pA3DSkinMan; }
	inline A3DFlatCollector* GetA3DFlatCollector() { return m_pA3DFlatCollector; }
	inline A3DWireCollector* GetA3DWireCollector() { return m_pA3DWireCollector; }
	inline A3DSkinModelMan* GetA3DSkinModelMan() { return m_pA3DSkinModelMan; }
	inline A3DHLSLMan* GetA3DHLSLMan() { return m_pHLSLMan; }
	inline A3DWorld * GetA3DWorld() { return m_pA3DWorld; }
    inline A3DOcclusionMan* GetA3DOcclusionMan() { return m_pA3DOcclusionMan; }
	inline A3DEnvironment* GetA3DEnvironment();
	inline A3DShadowMap* GetA3DShadowMap(){ return m_pA3DShadowMap; }

	//	Pass NULL to SetA3DLitModelRender means to restore default lit model render
	void SetA3DLitModelRender(A3DLitModelRender* pRender);
	inline A3DLitModelRender* GetA3DLitModelRender() { return m_pA3DLitModelRender; }
    inline A3DLitModelSharedDataMan* GetA3DLitModelSharedDataMan() { return m_pA3DLitModelSharedDataMan; } 


	inline A3DCDS * GetA3DCDS() { return m_pA3DCDS; }
	inline void SetA3DCDS(A3DCDS * pA3DCDS) { m_pA3DCDS = pA3DCDS; }

	inline DWORD GetAnimationTicks() { return m_dwAnimationTicks; }
	inline DWORD GetRenderTicks() { return m_dwRenderTicks; }
	inline DWORD GetLogicTicks() { return m_dwLogicTicks; }
	inline bool IsOptimizedEngine() { return m_bOptimizedEngine; }
	inline void SetOptimizedEngine(bool bOptimized) { m_bOptimizedEngine = bOptimized; }
	inline int GetMotionBlurValue() { return m_nMotionBlurValue; }
	inline void SetMotionBlurValue(int nMotionBlurValue) { m_nMotionBlurValue = min2(255, nMotionBlurValue); }
	inline FLOAT GetFPS() { return m_pRFPSCounter->GetFPS(); }

	// Angelica Media Engine;
	inline AMEngine* GetAMEngine() { return m_pAMEngine; }
	inline AMSoundEngine* GetAMSoundEngine() { return m_pAMSoundEngine; }
	inline AMVideoEngine* GetAMVideoEngine() { return m_pAMVideoEngine; }

	inline A3DPlants* GetA3DPlants() { return m_pA3DPlants; }
	inline A3DFont* GetSystemFont() { return m_pSystemFont; }

	inline A3DIBLLIGHTPARAM GetCurIBLLightParam() { return m_curIBLLightParam; }
	inline void SetCurIBLLightParam(A3DIBLLIGHTPARAM param) { m_curIBLLightParam = param; }

	A3DSkinRenderBase* CreateSkinRender(bool bInit);

	//debug
	inline PERFORMANCEITEM*		GetPerformanceItem() { return m_aPerformances;}
	inline OBJECTCNTITEM* 		GetObjectCntItem() { return m_aObjectCnts;}
	inline FRAMECNTITEM* 		GetFrameCntItem() { return m_aFrameCnts;}

	//d3d9ex
	bool GetSupportD3D9ExFlag() { return m_bSupportD3D9Ex; }
};

inline void A3DEngine::IncObjectCount(int iIndex)
{
	ASSERT(iIndex >= 0 && iIndex < NUM_OBJECTCNT);
	m_aObjectCnts[iIndex].iObjectCnt++;
}

inline void A3DEngine::DecObjectCount(int iIndex)
{
	ASSERT(iIndex >= 0 && iIndex < NUM_OBJECTCNT);
	m_aObjectCnts[iIndex].iObjectCnt--;
}

inline void A3DEngine::IncFrameCount(int iIndex)
{
	ASSERT(iIndex >= 0 && iIndex < NUM_FRAMECNT);
	m_aFrameCnts[iIndex].iCounter++;
}

inline void A3DEngine::DecFrameCount(int iIndex)
{
	ASSERT(iIndex >= 0 && iIndex < NUM_FRAMECNT);
	m_aFrameCnts[iIndex].iCounter++;
}

A3DCameraBase* A3DEngine::GetActiveCamera() 
{ 
	return m_pActiveCamera; 
}
A3DEnvironment* A3DEngine::GetA3DEnvironment()
{
	return m_pA3DEnvironment;
}
#endif
