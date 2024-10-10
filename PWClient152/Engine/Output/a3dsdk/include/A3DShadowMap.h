/*
 * FILE: A3DShadowMap.h
 *
 * DESCRIPTION: shadow map
 *
 * CREATED BY: Liyi, 2010/4/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3D_SHADOW_MAP_H_
#define _A3D_SHADOW_MAP_H_

#include "A3DTypes.h"
#include "A3DTexture.h"
#include "A3DVertex.h"

class A3DDevice;
class A3DRenderTarget;
class A3DVECTOR3;
class A3DViewport;
class A3DCameraBase;
class A3DOrthoCamera;
class A3DSurface;
class A3DAABB;
class A3DHLSL;
class A3DStream;
class A3DVertexDecl;

#define SHADOW_CACHE_NUM 9
#define SHADOW_CACHE_ROW 3
#define SHADOW_CACHE_COL 3

class A3DShadowMap
{
public:
	A3DShadowMap();
	~A3DShadowMap();

	bool Init(A3DDevice* pA3DDevice, int nShadowMapSize, bool bOptimize = false, bool bUseFloatTexture = true, bool bUseTerrain = false,
		bool bUseHWPCF = false, bool bUseVSM = false);
	void Release();

	bool Set(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius,const float fShadowRealRadius);
	bool SetTSM(A3DCameraBase* pEyeCamera, const A3DVECTOR3& vCenter, float fFarDist, const A3DVECTOR3& vLightDir);

	bool Restore();
	bool Cache(int nCurCache);

	bool ClearDevice( A3DCOLOR clearColor = 0xFFFFE000);

	//void AppearAsTexture(int nLayer);
	//void DisappearAsTexture(int nLayer); //

	A3DTexture* GetShadowMapTexture();			//根据是否开启HWPCF决定获取的是ColorTarget还是DepthStencilTarget
	A3DTexture* GetShadowMapFloatTexture();		//获取ColorTarget

	inline bool				IsInit() { return m_bInit;}
	inline bool				IsOptimize() { return m_bOptimize;}

	inline A3DRenderTarget*	GetShadowMapRT(){ return m_pShadowMap;}
	//inline A3DTexture*		GetShadowTexture() { return m_pShadowTexture;}
	inline A3DViewport*		GetShadowViewport() { return m_pShadowViewport;}
	inline A3DOrthoCamera*	GetShadowCamera() { return m_pShadowCamera;}
	inline A3DOrthoCamera*  GetShadowTextureCamera() { return m_pShadowTextureCamera;}

	inline A3DRenderTarget* GetTerrainShadowMap() { return m_pTerrainShadowMapL8; }
	inline A3DViewport*		GetTerrainShadowViewport() { return m_pTerrainShadowViewport; }
	inline A3DOrthoCamera*	GetTerrainShadowCamera() { return m_pTerrainShadowCamera; }
	inline bool				IsTerrainShadowMapReady() const { return m_nTerrainShadowMapCurTile >= m_nTerrainShadowMapTileCount - 1; }
	inline void				SetTerrainShadowMapReady() { m_nTerrainShadowMapCurTile = m_nTerrainShadowMapTileCount - 1; }
	inline void				SetTerrainShadowMapDirty() { m_nTerrainShadowMapCurTile = -2; }
	inline bool				CanRenderTerrainShadowOnePass() const { return m_bTerrainShadowOnePass; }

	inline bool IncTerrainShadowMapTileIndex()
	{
		if (m_nTerrainShadowMapCurTile >= m_nTerrainShadowMapTileCount - 1)
			return false;

		m_nTerrainShadowMapCurTile++;
		return m_nTerrainShadowMapCurTile >= 0;
	}

	void SetupTerrainShadowMapViewport(const A3DVECTOR3& vShadowCenter, float fShadowRealRadius, const A3DVECTOR3& vSunLightDir, bool bUpdateTotal, bool bActive);

	bool SetShadowTextureCamera(const A3DVECTOR3* pShadowRealCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRealRadius);
	
	int GetShadowMapSize();
	void SetShadowMapSize(int iWidth);

	int GetSampleLevel() { return m_nSampleLevel; }
	void SetSampleLevel(int nSampleLevel);

	bool IsTSMEnable(){ return m_bEnableTSM; }
	void SetTSMEnable(bool bEnable){ m_bEnableTSM = bEnable; }

	void SetShadowCamera(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius, const float fShadowRealRadius );
	bool IncorporateShadowTexture();
	
	const A3DMATRIX4& GetShadowMatrix(){ return m_matShadow; }
	bool IsUsingHWPcf(){ return m_bUseHWPCF; }
	bool IsUsingVSM() { return m_bUseVSM; }

	bool BlurVSM();

    float GetShadowRadius() const { return m_fShadowRadius; }

	static bool SupportHWPCF(A3DDevice* pDevice);
protected:

	bool InitOptimize();

	//TSM
	void CalcEyeFrustumCorners(A3DCameraBase* pCamera, A3DVECTOR3* aCorners, 
		float fNear, float fFar, float fScale);
	void CalcLightViewMatrix(const A3DVECTOR3& vCenter, const A3DVECTOR3& vLightDir);
	void CalcFrustumInLightSpace(const A3DVECTOR3* aCorners, const A3DMATRIX4& matLightView, 
		int iMapSize, A3DAABB& aabbInLight);
	void CalcTSMLightViewMatrix(A3DCameraBase* pEyeCamera, const A3DVECTOR3& vLightDir);
	void CalcDirLightViewAndProj(A3DAABB& aabbInLight, A3DMATRIX4& matLightView, A3DMATRIX4& matLightProj);
	void BuildTSMMatrix(A3DCameraBase* pEyeCamera, A3DAABB& aabbInLight);
	A3DMATRIX4 CalcTexScaleTM(int iTexWid, int iTexHei);
	
	bool CreateShadowMapRT();
	bool ReloadBlurShader();
protected:
	A3DDevice*			m_pA3DDevice;

	A3DRenderTarget*	m_pShadowMap;
	A3DSurface*			m_ppShadowCaches[SHADOW_CACHE_NUM];
	A3DTexture*			m_pShadowTexture;
	
	A3DViewport*		m_pShadowViewport;
	A3DOrthoCamera*		m_pShadowCamera;
	A3DOrthoCamera*		m_pShadowTextureCamera;

	A3DRenderTarget*	m_pTerrainShadowMapL8;
	A3DViewport*		m_pTerrainShadowViewport;
	A3DOrthoCamera*		m_pTerrainShadowCamera;
	int					m_nTerrainShadowMapDivision;
	int					m_nTerrainShadowMapTileCount;
	int					m_nTerrainShadowMapCurTile;
	int					m_nTerrainShadowMapTileSize;

	int					m_nShadowMapSize;
	bool				m_bInit;
	bool				m_bOptimize;
	bool				m_bTerrainShadowOnePass;
	bool				m_bUseTerrain;
	
	int					m_nSampleLevel;
	float               m_fShadowRadius;
	

	//TSM
	A3DMATRIX4			m_matLightView;
	A3DMATRIX4			m_matLightProj;
	A3DMATRIX4			m_matTSM;
	A3DMATRIX4			m_matShadow;
	float				m_fShadowRange;
	bool				m_bDegrade;
	float				m_fTSMDelta;
	float				m_fLFFarDist;
	bool				m_bEnableTSM;
	//PCF
	bool m_bUseHWPCF;

	//VSM
	bool m_bUseVSM;
	A3DHLSL* m_pBlurShadowMapVS;
	A3DHLSL* m_pBlurShadowMapXPS;
	A3DHLSL* m_pBlurShadowMapYPS;
	A3DStream*			m_pStream;
	A3DVertexDecl*		m_pStreamDecl;
	A3DIBLVERTEX		m_aVertices[8];



	bool m_bUseFloatTexture;

	class A3DShadowMapTexture : public A3DTexture
	{
	public:
		void SetD3D9Texture(IDirect3DTexture9* pTex){ m_pDXTexture = pTex; }
		virtual bool Release() { return true; }
	};

	A3DShadowMapTexture m_shadowMapTexture;
	A3DShadowMapTexture m_shadowMapFloatTexture;
};

#endif //_A3D_SHADOW_MAP_H_