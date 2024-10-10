#ifndef A3DGFXRENDERSLOT_H_
#define A3DGFXRENDERSLOT_H_

#include "A3DTypes.h"
#include "AString.h"
#include <hashmap.h>
#include "A3DDevice.h"
#include "A3DGFXElement.h"

class A3DGFXElement;
class A3DDevice;
class A3DTexture;
class A3DViewport;
class A3DVertexShader;
class A3DPixelShader;
class A3DVertexDecl;

enum GFX_RENDER_MODE
{
	GRMSoftware,
	GRMVertexShader,
	GRMBlendMatrix
};

enum GFX_FOG_FLAG
{
	GFX_FOG_CURRENT,
	GFX_FOG_ZERO,
};

#ifdef _ANGELICA21
#define GFX_VS_CONST_BASE		16
#else
#define GFX_VS_CONST_BASE		10
#endif

#define GFX_MODEL_PRIM_TYPE		0x1000

struct A3DGFXRenderSlot
{
	int					m_nRenderLayer;	
	int					m_nVertType;
	size_t				m_VertSize;
	int					m_PrimType;
	AString				m_strTexPath;
	bool				m_bNoDownSample;
	A3DSHADER			m_Shader;
	bool				m_bTileMode;
	bool				m_bZTestEnable;
	int					m_nRef;
	int					m_nTotalVertCount;
	abase::vector<A3DGFXElement*> m_EleArr;
	A3DTexture*			m_pTexture;
	bool				m_bPixelShader;
	A3DPixelShader*		m_pDefaultShader;
	GFX_FOG_FLAG		m_enumFogFlag;
    bool                m_bSoftEdge;
};

typedef abase::hash_map<DWORD, A3DGFXRenderSlot*> A3DGFXRenderSlotMap;
typedef abase::vector<A3DGFXRenderSlot*> A3DGFXRenderSlotArray;

class A3DGFXRenderMisc;
class A3DGFXRenderSlotMan
{
public:

	A3DGFXRenderSlotMan() :
	m_dwCurIndex(1),
	m_pRenderMisc(NULL),
	m_pDevice(0),
	m_nMaxMatrixCount(0),
	m_bAlphaTestEnable(false),
	m_bFogEnable(false),
	m_bCurFogEnabled(false),
	m_clCurFog(0),
	m_bUserClip(false),
	m_bBloom(false)
	{

		::InitializeCriticalSection(&m_cs);

		for (int i = 0; i < GFX_RENDER_LAYER_COUNT; i++)
			m_DelayedSlots[i].reserve(128);
	}

	~A3DGFXRenderSlotMan() { ::DeleteCriticalSection(&m_cs); }

protected:

	A3DGFXRenderSlotMap m_RenderSlotMap;
	A3DGFXRenderSlotArray m_DelayedSlots[GFX_RENDER_LAYER_COUNT];
	DWORD m_dwCurIndex;
	A3DDevice* m_pDevice;

	A3DGFXRenderMisc* m_pRenderMisc;

	CRITICAL_SECTION m_cs;
	int m_nMaxMatrixCount;
	bool m_bAlphaTestEnable;
	bool m_bFogEnable;
	bool m_bCurFogEnabled;
	A3DCOLOR m_clCurFog;

	bool		m_bUserClip;		//	flag indicates whether user clip plane has been enabled
	D3DXPLANE	m_cp;				//	user clip plane in world space
	D3DXPLANE	m_hcp;				//	user clip plane in homogeneous space
    bool        m_bBloom;           //  require bloom
public:

	static GFX_RENDER_MODE g_RenderMode;

protected:

	void RenderOneSlot(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleStrip(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleList(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTLTriangleList(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleStripSoftware(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderTriangleListSoftware(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderSkinModel(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void RenderPointList(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void PreRender(A3DGFXRenderSlot* pSlot, A3DViewport* pView);
	void PostRender(A3DGFXRenderSlot* pSlot);
	void Lock() { ::EnterCriticalSection(&m_cs); }
	void Unlock() { ::LeaveCriticalSection(&m_cs); }

	void RenderDelayedSlots(int nLayer, A3DViewport* pView)
	{
		A3DGFXRenderSlotArray& arr = m_DelayedSlots[nLayer];

		for (size_t i = 0; i < arr.size(); i++)
			RenderOneSlot(arr[i], pView);

		arr.clear();
	}

public:

	bool Init(A3DDevice* pDevice);
	void Release();
	DWORD RegisterSlot(int nRenderLayer, int nVertType, size_t VertSize, int PrimType, const AString& strTexPath, const A3DSHADER& sh, bool bTileMode, bool bNoDownSample, bool bZTestEnable, bool bPixelShader, bool bSoftEdge, bool bAbsTexPath);
	void UnregisterSlot(DWORD dwIndex);
	void RegisterEleForRender(DWORD dwIndex, A3DGFXElement* pEle);
	void Render(A3DViewport* pView);
	bool IsAlphaTestEnable() const { return m_bAlphaTestEnable; }
	void SetAlphaTestEnable(bool b) { m_bAlphaTestEnable = b; }
	bool IsFogEnable() const { return m_bFogEnable; }
	void SetFogEnable(bool b) { m_bFogEnable = b; }
	bool IsCurFogEnabled() const { return m_bCurFogEnabled; }

	// for editor use
	void ReloadTex();
};

#endif
