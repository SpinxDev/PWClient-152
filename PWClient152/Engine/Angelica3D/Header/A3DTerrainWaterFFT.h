/*
 * FILE: A3DTerrainWaterFFT.h
 *
 * DESCRIPTION: 用来实现波动的海水
 *
 * CREATED BY: Pan Yupin, 2012/6/21
 *
 * HISTORY:
 *
 * Copyright (c) 2012 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTERRAINWATERFFT_H_
#define _A3DTERRAINWATERFFT_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "AArray.h"
#include "AAssist.h"
#include "A3DVertexShader.h"
#include "A3DWaterFFT.h"
#include "A3DTerrainWaterRender.h"
#include "A3DCubeTexture.h"

#include <vector>

class A3DDevice;
class A3DRenderTarget;
class A3DStream;
class A3DHLSL;
class A3DTexture;
class A3DCamera;
class A3DViewport;
class A3DOcclusionProxy;

#define FFT_WATER_RAIN_TEXTURE_NUM 30
#define FFT_WATER_CAUSTIC_TEXTURE_NUM 30
#define FFT_WATER_STREAM_PART_NUM 6				//水域划分为 FFT_WATER_STREAM_PART_NUM * FFT_WATER_STREAM_PART_NUM 的分块
//一块大小是40*40
class A3DTerrainWaterFFT : public A3DDevObject
{
public:
	struct Param
	{
		float m_fWaterLevel;				//水面高度
		float m_fWindDirection;				//风向								0 ~ PI*2
		float m_fGravity;					//重力								1 ~ 20
		
		float m_fWindSpeed;					//风速								0 ~ 16
		float m_fWavesSpeed;				//波浪速度(影响非FFT波浪和泡沫)		0 ~ 10 
		float m_fWavesAmount;				//FFT瓷砖大小						0 ~ 10
		float m_fWavesSize;					//波浪剧烈程度						0 ~ 2
		float m_fSoftIntersectionFactor;	//边缘软化程度						0 ~ 10
		float m_fReflectionAmount;			//反射强度							0 ~ 10
		float m_fWhiteCapsAmount;			//顶部高光							0 ~ 2
		float m_fSunMultiplier;				//太阳反光强度						0 ~ 32
		float m_fFresnelScale;				//菲涅尔缩放						0 ~ 32
		float m_fEdgeFoamAmount;			//边缘泡沫强度						0 ~ 20

		float m_fFogDensity;				//水下雾强度系数					0 ~ 4
		float m_fWaterFogScaleExp;			//水下雾强度指数					0 ~ 10				

		bool m_bUseFFT;						//是否开启FFT
		bool m_bEdgeFoam;					//是否开启边缘泡沫
		bool m_bWaveFoam;					//是否开启波浪上的泡沫
		bool m_bWaveGloss;					//是否开启波浪上的高光
		bool m_bCaustic;					//是否开启水下焦散
		int m_nRain;						//是否开启下雨波纹
		bool m_bHighRefr;					//是否开启高端折射. 有色散效果
		bool m_bUnderGodRay;				//是否开启水下Godray. 高端效果,很费.
		bool m_bUseCustomColor;				//是否使用用户指定的颜色作为水的颜色, 不开启则使用全局雾的颜色
		
		bool m_bEnableWave;					//是否开启波浪

		A3DVECTOR3 m_vWaterFogColor;		//用户指定的水颜色
		
		bool m_bEnableAreaConstrain;		//是否开启范围限制，水会被限制在这一范围内
		A3DVECTOR4 m_vAreaConstrain;		//xy: minXZ,  zw: maxXZ

		bool m_bCubeReflection;				//是否采用CubeMao渲染反射，可提高效率
		bool m_bOnlyReflectSky;				//是否只反射天空,最省方案.
		bool m_bFullReflection;				//是否开启全反射，所有场景物体都会被反射，非常费
		bool m_bEnableFog;					//是否开启雾

		bool m_bEnableBloom;
		Param()
		{
			m_fWaterLevel = 0.f;
			Reset();
		}

		void Reset()
		{	
			m_fGravity = 9.81f;
			m_fWindDirection = 1.f;
			m_fWindSpeed = 4.f;
			m_fWavesSpeed = 1.33333f;
			m_fWavesAmount = 1.5f;
			m_fWavesSize = 0.75f;

			m_fSoftIntersectionFactor = 1.0;
			m_fReflectionAmount = 0.5;
			m_fWhiteCapsAmount = 0.75;
			m_fSunMultiplier = 12.0;
			m_fFresnelScale = 1.0;
			m_fEdgeFoamAmount = 1.0;
			m_bUseFFT = true;
			m_bEnableWave = true;

			m_fFogDensity = 0.8f;
			m_fWaterFogScaleExp = 5;

			m_bEdgeFoam		= true;
			m_bWaveFoam		= true;
			m_bWaveGloss	= true;
			m_bCaustic		= true;
			m_nRain = 0;
			m_bHighRefr		= false;
			m_bUnderGodRay	= true;
			m_bUseCustomColor = false;
			m_vWaterFogColor = A3DVECTOR3(1,1,1);

			m_bEnableAreaConstrain = false;
			m_vAreaConstrain = A3DVECTOR4(-1e10f, -1e10f, 1e10f, 1e10f);

			m_bCubeReflection	= false;
			m_bFullReflection	= false;
			m_bOnlyReflectSky	= false;
			m_bEnableFog		= true;
			m_bEnableBloom		= false;
		}
	};

	struct RenderParam
	{
		A3DCamera* pCamera;
		A3DRenderTarget* pInputRT;
		A3DVECTOR3 vecSunDir;
		A3DViewport* pCurrentViewport;
		A3DTerrain2* pTerrain;
		A3DSky* pSky;
		A3DTerrainOutline* pTerrainOutline;
		LPRENDERFORREFLECT pReflectCallBack;
		void * pArg;
		LPRENDERONSKY pfnRenderOnSky;
		void* pArgSky;
		A3DRenderTarget* pBloomRT;

		RenderParam()
		{
			pCamera = NULL;
			pInputRT = NULL;
			pCurrentViewport = NULL;
			pTerrain = NULL;
			pSky = NULL;
			pTerrainOutline = NULL;
			pReflectCallBack = NULL;
			pArg = NULL;
			pfnRenderOnSky = NULL;
			pArgSky = NULL;
			pBloomRT = NULL;
			vecSunDir = A3DVECTOR3(0,1,0);
		}
	};

public:
	A3DTerrainWaterFFT();
	virtual ~A3DTerrainWaterFFT();
	//初始化
	bool Init(A3DDevice* pDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera);
	//释放
	void Release();
	//渲染
	void Render(RenderParam* pRenderParam);
	//获取某一点的水面高度. 当采用FFT方式进行波动的时候, 该值仅是一个近似值
	float GetWaveHeight(const A3DVECTOR3& vPos);
	//恢复默认参数
	void ResetParams();
	//设置参数
	void SetParam(const Param& param);
	//获取当前参数
	const Param& GetParam() const;
	
	bool LoadCubeReflectionTexture(AFile* pCubeMapFile);

	//	After device reset
	virtual bool AfterDeviceReset();
private:
	struct ColorVertex
	{
		A3DVECTOR3 m_p;
		A3DVECTOR3 m_n;
		A3DCOLOR m_dwordColor;
		float m_tu, m_tv;
		ColorVertex();
		ColorVertex(float x, float y, float z,float nx,float ny,float nz, A3DCOLOR color, float u, float v);
		void Set(A3DVECTOR3 position,A3DVECTOR3 normal,A3DCOLOR color);
		void Set(A3DVECTOR3 position,A3DVECTOR3 normal,A3DCOLOR color, D3DXVECTOR2 uv);
	};

	Param m_param;

	bool m_bWireframe;
	float m_fAConstant;			//波动计算中的A常数

	A3DDevice* m_pA3DDevice;

	int m_nWaterTesselationAmount;
	int m_nWaterTesselationSwathWidth;

	A3DStream* m_pSmallWaterStream;

	A3DHLSL* m_pHLSLCaustic;
	A3DHLSL* m_pHLSLWater;
	A3DHLSL* m_pHLSLLineraDepth;
	A3DHLSL* m_pHLSLUnderWater;
	A3DHLSL* m_pHLSLDownSamp;
	A3DHLSL* m_pHLSLGodray;
	A3DHLSL* m_pHLSLCopyRT;
	A3DHLSL* m_pHLSL3DScreenDepth;

	A3DWaterFFT m_cryWaterFFT;

	A3DTexture* m_pFFTTexture;

	void MakeReflectionRT(A3DRenderTarget* pReflectionRT, bool bUnderWater,
		A3DCamera* pCamera,
		const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, 
		LPRENDERFORREFLECT pReflectCallBack, void * pArg,
		LPRENDERONSKY pfnRenderOnSky, void * pArgSky);
	void MakeDepthRT(A3DViewport* pViewport, A3DRenderTarget* pDepthRT);
	void Make3DScreenDepthRT(A3DRenderTarget* pScreenDepthRT, A3DCamera* pCamera);


	void CopyRenderTarget(A3DRenderTarget* pSrc, A3DRenderTarget* pDst);

	A3DTexture* m_pOceanWaveNormalTex;
	A3DTexture* m_pWaterFoamTex;
	A3DTexture* m_pWaterGlossTex;
	A3DTexture* m_pRainNormalTex[FFT_WATER_RAIN_TEXTURE_NUM];
	A3DTexture* m_pRainXLNormalTex[FFT_WATER_RAIN_TEXTURE_NUM];
	A3DTexture* m_pCausticTex[FFT_WATER_CAUSTIC_TEXTURE_NUM];
	A3DTexture* m_pBlackTex;

	A3DViewport*		m_pReflectViewport;		// water reflect viewport
	A3DCamera*			m_pCameraReflect;		// camera with which render the reflect scene

	A3DStream*			m_pStream;
	ColorVertex			m_aVertices[8];
	A3DVertexDeclCommon* m_pVertDecl;
	float				m_fRainDuration;

	bool				m_bCurEnableNew;
	int					m_nCurEnableRain;
	bool				m_bCurEnableCaustic;
	bool				m_bCurEnableHighRefr;
	bool				m_bCurEnableFFT;
	bool				m_bCurEnableWave;
	bool				m_bCurEnableFog;
	bool				m_bCurEnableBloom;
	float				m_fCurWaveAmount;

	A3DTexture*			m_pSmallMap1x1;
	A3DCubeTexture*		m_pCubeTexture;
	bool ReloadShader();
	
	bool ReloadTextuer(bool bIsNew);	//仅用来测试

	bool m_bSupportSM30;

	unsigned int ComputeSmallWaterLOD(float fCameraHeight);

	A3DTerrain2* m_pCurrentTerrain;
	A3DTerrain2Render*	m_pTerrainRender;	//	Terrain render
	bool CreateTerrainRender(A3DTerrain2* pTerrain);

	typedef abase::hash_map<int, bool> SmallWaterUnderTerrianMap;
	SmallWaterUnderTerrianMap m_mapSmallWaterVisible[3];

	A3DOcclusionProxy* m_pOccProxy;

	void CreateOccQuery();

};

#endif