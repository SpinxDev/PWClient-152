#include "A3DTerrainWaterFFT.h"
#include "A3DRenderTarget.h"
#include "a3drendertargetman.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DTextureMan.h"
#include "A3DCamera.h"
#include "A3DViewport.h"
#include "ATime.h"
#include "A3DPI.h"
#include "A3DSky.h"
#include "A3DTerrain2.h"
#include "A3DTerrainOutline.h"
#include "A3DWireCollector.h"
#include "A3DCollision.h"
#include <map>
#include <algorithm>

//#define _WATER_FFT_DEBUG_

#ifdef _ANGELICA_2_1
#include "A3DHLSLWrapper.h"
#include "A3DSceneRenderConfig.h"

#define FN_Water_FFT_HLSL					"shaders\\HLSL\\water_fft.hlsl"
#define FN_Water_Caustic_HLSL				"shaders\\HLSL\\water_caustic.hlsl"
#define FN_Water_Linear_Depth_HLSL			"shaders\\HLSL\\water_linearDepth.hlsl"
#define FN_Water_UnderWater_HLSL			"shaders\\HLSL\\water_underwater.hlsl"
#define FN_Water_Down_Samp_HLSL				"shaders\\HLSL\\water_down_samp.hlsl"
#define FN_Water_GodRay_HLSL				"shaders\\HLSL\\water_under_godray.hlsl"
#define FN_Copy_RT_HLSL						"shaders\\HLSL\\copy_rt.hlsl"
#define FN_3DVision_Screen_Depth_HLSL		"shaders\\HLSL\\3dvision_screen_depth.hlsl"

#define FN_Oceanwaves_ddn_PNG				"shaders\\textures\\OceanNormal.PNG"
#define FN_WaterFoam_dds					"shaders\\textures\\WaterFoam.dds"
#define FN_WaterGloss_dds					"shaders\\textures\\WaterGloss.dds"
#define FN_Puddle_dds						"shaders\\textures\\Rain\\Rain\\puddle_%05d.dds"
#define FN_PuddleXL_dds						"shaders\\textures\\Rain\\RainXL\\puddle_%05d.dds" 
#define FN_Caustic_dds						"shaders\\textures\\Caustic\\Caustic_%05d.tga" 

#define FN_NewOceanwaves_ddn_PNG			"shaders\\textures\\OceanNormal.PNG"
#define FN_NewWaterFoam_dds					"shaders\\textures\\WaterFoam.dds"
#define FN_NewWaterGloss_dds				"shaders\\textures\\WaterGloss.dds"
#define FN_NewPuddle_dds					"shaders\\textures\\Rain\\Cry\\puddle%d.DDS"

#else
#include "A3DHLSL.h"
#include "A3DEnvironment.h"
#include "A3DOcclusionMan.h"
#include "A3DConfig.h"
#define FN_Water_FFT_HLSL					"shaders\\2.2\\HLSL\\Water\\water_fft.hlsl"
#define FN_Water_Caustic_HLSL				"shaders\\2.2\\HLSL\\Water\\water_caustic.hlsl"
#define FN_Water_Linear_Depth_HLSL			"shaders\\2.2\\HLSL\\Water\\water_linearDepth.hlsl"
#define FN_Water_UnderWater_HLSL			"shaders\\2.2\\HLSL\\Water\\water_underwater.hlsl"
#define FN_Water_Down_Samp_HLSL				"shaders\\2.2\\HLSL\\Water\\water_down_samp.hlsl"
#define FN_Water_GodRay_HLSL				"shaders\\2.2\\HLSL\\Water\\water_under_godray.hlsl"
#define FN_Copy_RT_HLSL						"shaders\\2.2\\HLSL\\Water\\copy_rt.hlsl"
#define FN_3DVision_Screen_Depth_HLSL		"shaders\\2.2\\HLSL\\Water\\3dvision_screen_depth.hlsl"

#define FN_Oceanwaves_ddn_PNG				"Shaders\\Textures\\Water\\fft\\OceanNormal.PNG"
#define FN_WaterFoam_dds					"Shaders\\Textures\\Water\\fft\\WaterFoam.dds"
#define FN_WaterGloss_dds					"Shaders\\Textures\\Water\\fft\\WaterGloss.dds"
#define FN_Puddle_dds						"Shaders\\Textures\\Water\\fft\\Rain\\Rain\\puddle_%05d.dds"
#define FN_PuddleXL_dds						"Shaders\\Textures\\Water\\fft\\Rain\\RainXL\\puddle_%05d.dds" 
#define FN_Caustic_dds						"Shaders\\Textures\\Water\\fft\\Caustic\\Caustic_%05d.tga" 

#define FN_NewOceanwaves_ddn_PNG			"Shaders\\Textures\\Water\\fft\\复件 OceanNormal.PNG"
#define FN_NewWaterFoam_dds					"Shaders\\Textures\\Water\\fft\\复件 WaterFoam.dds"
#define FN_NewWaterGloss_dds				"Shaders\\Textures\\Water\\fft\\复件 WaterGloss.dds"
#define FN_NewPuddle_dds					"Shaders\\Textures\\Water\\fft\\复件 puddle%d.tga"

#endif

#define WATER_FFT_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE| D3DFVF_TEX1)

static const unsigned int NX = 64;
static const unsigned int NY = 64;
static const unsigned g_nSmallWaterWidthLodNum = 3;

static A3DVECTOR4 operator* (const A3DVECTOR4& v, float f)
{
	return A3DVECTOR4(v.x * f, v.y * f, v.z * f, v.w * f);
}

static A3DVECTOR4 operator+ (const A3DVECTOR4& v1, const A3DVECTOR4& v2)
{
	return A3DVECTOR4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

static float A3DVec4Dot(const A3DVECTOR4& v1, const A3DVECTOR4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

static D3DVERTEXELEMENT9 m_declEle[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	{0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

#define TRY_OR_RELEASE_RETURN_FALSE(statement, strError)\
	if(!(statement))\
{\
	g_A3DErrLog.Log("A3DTerrainWaterFFT::Init Failed!");\
	g_A3DErrLog.Log(strError);\
	Release();\
	return false;	\
	}

bool A3DTerrainWaterFFT::Init(A3DDevice* pDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera)
{
	m_pA3DDevice = pDevice;

	if(!m_pA3DDevice->TestVertexShaderVersion(3, 0) ||
		!m_pA3DDevice->TestPixelShaderVersion(3, 0))	
	{
		m_bSupportSM30 = false;
	}
	else
		m_bSupportSM30 = true;//false;//

	m_pSmallMap1x1 = new A3DTexture;
	m_pSmallMap1x1->Create(m_pA3DDevice, 1, 1, A3DFMT_A8R8G8B8);

	m_pCurrentTerrain = NULL;
	m_pTerrainRender = NULL;
	//仅制作一块小的水，绘制多次
	{
		int nSmallGridSize = 40;
		float fRSmallGridSize = 1.f / (float)(nSmallGridSize);
		ColorVertex tmp;
		A3DVECTOR3 vv;
		std::vector<ColorVertex> aSmallVertex;
		for( int y = -1; y <= nSmallGridSize + 1; ++y )
		{      
			int realY = min(max(y, 0), nSmallGridSize);
			vv.y = (float)realY * fRSmallGridSize;

			for( int x = -1; x <= nSmallGridSize + 1; ++x )
			{
				int realX = min(max(x, 0), nSmallGridSize);
				vv.x = (float)realX * fRSmallGridSize;
				vv.z = 0.f;

				tmp.m_p = vv;

				//把UV填进去,用作调试
				tmp.m_tu = float(x) / float(nSmallGridSize);
				tmp.m_tv = float(y) / float(nSmallGridSize);

				aSmallVertex.push_back(tmp);        
			}
		}
		std::vector<WORD> indexList;				//这里换用TriangleList的索引形式，不用Strip了
		for(int y = 0; y < nSmallGridSize + 2; y++)
		{
			for(int x = 0; x < nSmallGridSize + 2; x++)
			{
				indexList.push_back((y    )	* (nSmallGridSize + 3) + x	  );
				indexList.push_back((y + 1) * (nSmallGridSize + 3) + x	  );
				indexList.push_back((y    )	* (nSmallGridSize + 3) + x + 1);

				indexList.push_back((y + 1) * (nSmallGridSize + 3) + x	  );
				indexList.push_back((y + 1) * (nSmallGridSize + 3) + x + 1);
				indexList.push_back((y    )	* (nSmallGridSize + 3) + x + 1);
			}
		}
		m_pSmallWaterStream = new A3DStream;
		TRY_OR_RELEASE_RETURN_FALSE(m_pSmallWaterStream->Init(m_pA3DDevice, sizeof(aSmallVertex[0]), WATER_FFT_FVF, aSmallVertex.size(), indexList.size(),
			A3DSTRM_STATIC, A3DSTRM_STATIC), "Cannot Init m_pCryWaterStream");

		ColorVertex* pVertex = NULL;
		TRY_OR_RELEASE_RETURN_FALSE(m_pSmallWaterStream->LockVertexBuffer(0, aSmallVertex.size() * sizeof(aSmallVertex[0]), (BYTE**) &pVertex, 0),
			"m_pCryWaterStream Lock failed!");
		memcpy(pVertex, &aSmallVertex[0], aSmallVertex.size() * sizeof(aSmallVertex[0]));
		m_pSmallWaterStream->UnlockVertexBuffer();

		WORD* pIndex = NULL;
		TRY_OR_RELEASE_RETURN_FALSE(m_pSmallWaterStream->LockIndexBuffer(0, indexList.size() * sizeof(WORD), (BYTE**)&pIndex, 0),
			"m_pCryWaterStream Lock failed!");
		memcpy(pIndex, &indexList[0], indexList.size() * sizeof(WORD));
		m_pSmallWaterStream->UnlockIndexBuffer();

	}

	const char* szMacros[10];
	int t = 0;

	if(m_bSupportSM30)
	{
		if(m_param.m_nRain)				{	szMacros[t] = "ENABLE_RAIN";		t+=1; }
		if(m_param.m_bCaustic)			{	szMacros[t] = "ENABLE_CAUSTIC";		t+=1; }
		//if(m_param.m_bHighRefr)		{	szMacros[t] = "ENABLE_HIGH_REFRA";	t+=1; }
		//if(m_param.m_bUseFFT)			{	szMacros[t] = "ENABLE_FFT";			t+=1; }
		//if(m_param.m_bEnableBloom)	{	szMacros[t] = "ENABLE_BLOOM";		t+=1; }
	}
	if(m_param.m_bEnableWave)		{	szMacros[t] = "ENABLE_WAVE";		t+=1; }
	if(m_param.m_bEnableFog)		{   szMacros[t] = "ENABLE_FOG";		t+=1; }
	

	m_pHLSLWater = CreateA3DHLSL_Common(FN_Water_FFT_HLSL, "vs", FN_Water_FFT_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSLWater != NULL, "water_fft.hlsl load failed!");

	m_nCurEnableRain = m_param.m_nRain;
	m_bCurEnableCaustic = m_param.m_bCaustic;
	m_bCurEnableHighRefr = m_param.m_bHighRefr;
	m_bCurEnableFFT = m_param.m_bUseFFT;
	m_bCurEnableWave = m_param.m_bEnableWave;
	m_bCurEnableFog = m_param.m_bEnableFog;
	m_bCurEnableBloom = m_param.m_bEnableBloom;

	t = 0;
	if(m_bSupportSM30)
	{
		//szMacros[t] = "SM_3_0";			t+=1;
	}

	m_pHLSLCaustic = CreateA3DHLSL_Common(FN_Water_Caustic_HLSL, "vs", FN_Water_Caustic_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSLCaustic != NULL, "water_caustic.hlsl load failed!");

	m_pHLSLLineraDepth = CreateA3DHLSL_Common(FN_Water_Linear_Depth_HLSL, "vs", FN_Water_Linear_Depth_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSLLineraDepth != NULL, "water_linearDepth.hlsl load failed!");

	m_pHLSLUnderWater = CreateA3DHLSL_Common(FN_Water_UnderWater_HLSL, "vs", FN_Water_UnderWater_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSLUnderWater != NULL, "water_underwater.hlsl load failed!");

	m_pHLSLDownSamp = CreateA3DHLSL_Common(FN_Water_Down_Samp_HLSL, "vs", FN_Water_Down_Samp_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSLDownSamp != NULL, "water_down_samp.hlsl load failed!");

	m_pHLSLGodray = CreateA3DHLSL_Common(FN_Water_GodRay_HLSL, "vs", FN_Water_GodRay_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSLGodray != NULL, "water_under_godray.hlsl load failed!");

	m_pHLSLCopyRT = CreateA3DHLSL_Common(FN_Copy_RT_HLSL, "vs", FN_Copy_RT_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSLCopyRT != NULL, "copy_rt.hlsl load failed!");

	m_pHLSL3DScreenDepth = CreateA3DHLSL_Common(FN_3DVision_Screen_Depth_HLSL, "vs", FN_3DVision_Screen_Depth_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);
	TRY_OR_RELEASE_RETURN_FALSE(m_pHLSL3DScreenDepth != NULL, "3dvision_screen_depth load failed!");


	ResetParams();

	m_cryWaterFFT.Create(9.81f, 1.0, m_param.m_fWindDirection, 1.0f, 1.0f);

	m_pFFTTexture = new A3DTexture;
	TRY_OR_RELEASE_RETURN_FALSE(m_pFFTTexture->Create(m_pA3DDevice, NX, NY, A3DFMT_A32B32G32R32F, 1),
		"m_pFFTTexture Init failed!");

	A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

	TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_Oceanwaves_ddn_PNG, &m_pOceanWaveNormalTex),
		"oceanwaves_ddn.PNG load failed!");

	TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_WaterFoam_dds, &m_pWaterFoamTex),
		"WaterFoam.dds load failed!");

	TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_WaterGloss_dds, &m_pWaterGlossTex),
		"water_gloss.dds load failed!");

	TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile("Shaders\\Textures\\Common\\Black.dds", &m_pBlackTex),
		"m_pBlackTex load failed!");


	for(int i = 0; i < FFT_WATER_RAIN_TEXTURE_NUM; ++i)
	{
		AString strName;
		strName.Format(FN_Puddle_dds, i);
		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(strName, &(m_pRainNormalTex[i])),
			"puddle.dds load failed!");
		strName.Format(FN_PuddleXL_dds, i);
		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(strName, &(m_pRainXLNormalTex[i])),
			"puddle.dds load failed!");	
	}

	for(int i = 0; i < FFT_WATER_CAUSTIC_TEXTURE_NUM; ++i)
	{
		AString strName;
		strName.Format(FN_Caustic_dds, i);
		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(strName, &(m_pCausticTex[i])),
			"caustic.dds load failed!");
	}

	//初始化a3d viewport和camera
	TRY_OR_RELEASE_RETURN_FALSE(m_pA3DDevice->GetD3DCaps().MaxUserClipPlanes != 0, "MaxUserClipPlanes == 0!");

	A3DVIEWPORTPARAM * pViewParam = pHostViewport->GetParam();

	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.iWidth		= pViewParam->Width ;
	rtFmt.iHeight		= pViewParam->Height;

	rtFmt.fmtTarget		= A3DFMT_A8R8G8B8; // we need an alpha channel on refract target to hold water height and terrain height difference
	rtFmt.fmtDepth		= A3DFMT_D24X8;

	float fov = pHostCamera->GetFOV() + DEG2RAD(5.0f);
	float ratio = rtFmt.iWidth * 1.0f / rtFmt.iHeight;

	// now create a reflected camera
	m_pCameraReflect = new A3DCamera();

	TRY_OR_RELEASE_RETURN_FALSE(m_pCameraReflect->Init(m_pA3DDevice, fov, 0.2f, 1900.0f, ratio),
		"Failed to create the reflected camera!");

	m_pCameraReflect->SetMirror(pHostCamera, A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));

	//	Create viewports
	TRY_OR_RELEASE_RETURN_FALSE(m_pA3DDevice->CreateViewport(&m_pReflectViewport, 0, 0, rtFmt.iWidth, rtFmt.iHeight, 
		0.0f, 1.0f, true, true, 0xFF008FFD), "Create reflect viewport fail!");

	m_pReflectViewport->SetCamera(m_pCameraReflect);

	m_aVertices[0].Set(A3DVECTOR3(-1, -1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(0, 1));
	m_aVertices[1].Set(A3DVECTOR3( 1, -1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(1, 1));
	m_aVertices[2].Set(A3DVECTOR3(-1,  1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(0, 0));
	m_aVertices[3].Set(A3DVECTOR3( 1,  1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(1, 0));

	m_aVertices[4] = m_aVertices[0];
	m_aVertices[5] = m_aVertices[1];
	m_aVertices[6] = m_aVertices[2];
	m_aVertices[7] = m_aVertices[3];
	WORD aIndices[] = {0, 2, 1, 1, 2, 3};

	m_pStream = new A3DStream;

	TRY_OR_RELEASE_RETURN_FALSE(m_pStream->Init(m_pA3DDevice, sizeof(ColorVertex), WATER_FFT_FVF, 8, 6, A3DSTRM_STATIC, A3DSTRM_STATIC)
		, "m_pStream init failed!");

	m_pStream->SetVerts((BYTE*)m_aVertices, 8);
	m_pStream->SetIndices(aIndices, 6);

	m_pVertDecl = new A3DVertexDeclCommon;
	TRY_OR_RELEASE_RETURN_FALSE(m_pVertDecl->Init(m_pA3DDevice, m_declEle), "m_pVertDecl init failed!");

	m_param.m_vWaterFogColor = A3DVECTOR3(1,1,1);
	m_fCurWaveAmount = m_param.m_fWavesAmount;

	m_pOccProxy = NULL; 
#ifdef _ANGELICA_2_1
#else
	CreateOccQuery();
	if (m_pOccProxy)
		m_pOccProxy->SetEnabled(false);
#endif

	if (m_pA3DDevice)
		m_pA3DDevice->AddUnmanagedDevObject(this);

	return true;
}

template<class T> T clamp_tpl( T X, T Min, T Max ) 
{	
	return X < Min ? Min : X < Max ? X : Max; 
}

struct SmallWater
{
	int m_iX;
	int m_iZ;
	float m_fStartX;		
	float m_fStartZ;
	A3DAABB m_aabb;
	bool m_bHasNeighbor[4];
	float m_fDistanceToCamera;
	bool m_bRender;					//是否将要被渲染。根据是否在地形下或者视椎体外进行剪裁。
	SmallWater()
	{
		m_iX = 0;
		m_iZ = 0;
		m_fStartX = 0.f;
		m_fStartZ = 0.f;
		for(int i = 0; i < 4; i++)
			m_bHasNeighbor[i] = true;
		m_fDistanceToCamera = 0.f;
		m_bRender = true;
	}
};

bool operator< (const SmallWater& left, const SmallWater& right)
{
	return left.m_fDistanceToCamera > right.m_fDistanceToCamera;
}

struct VisitedItem
{
	bool bVisible;
	float fDistance;
};

int HashSmallWaterPosition(int iX, int iZ)
{
	return iX * 100000 + iZ;
}

void A3DTerrainWaterFFT::Render(RenderParam* pRenderParam)
{
	m_bWireframe = false;
#ifdef _WATER_FFT_DEBUG_
	if(GetAsyncKeyState(VK_F1) & 0x8000)
	{
		m_bWireframe = true;
	}
	if(GetAsyncKeyState(VK_F2) & 0x8000)
	{
		//ReloadTextuer(true);
	}
	if(GetAsyncKeyState(VK_F3) & 0x8000)
	{
		ResetParams();
	}
	if(GetAsyncKeyState(VK_F4) & 0x8000)
	{
		ReloadShader();
	}
// 	if(GetAsyncKeyState(VK_F6) & 0x8000)
// 	{
// 		m_param.m_bEnableWave = !m_param.m_bEnableWave;
// 		//m_param.m_bCaustic =  !m_param.m_bCaustic;
// 	}
// 	if(GetAsyncKeyState(VK_F7) & 0x8000)
// 	{
// 		m_param.m_bUseFFT = !m_param.m_bUseFFT;
// 		//m_param.m_bHighRefr =  !m_param.m_bHighRefr;
// 	}
#endif
	A3DCamera* pCamera = pRenderParam->pCamera;
	A3DViewport* pCurrentViewport = pRenderParam->pCurrentViewport;
	A3DRenderTarget* pInputRT = pRenderParam->pInputRT;

	ASSERT(pCamera && pInputRT && pCurrentViewport);

	if(!m_pHLSLCaustic || !m_pHLSLLineraDepth || !m_pHLSLUnderWater || !m_pHLSLWater)
		return;

	//如果参数发生了改变,则重新读取shader
	if( m_nCurEnableRain != m_param.m_nRain ||
		m_bCurEnableCaustic != m_param.m_bCaustic ||
		m_bCurEnableHighRefr != m_param.m_bHighRefr ||
		m_bCurEnableFFT != m_param.m_bUseFFT ||
		m_bCurEnableWave != m_param.m_bEnableWave ||
		m_bCurEnableFog != m_param.m_bEnableFog ||
		m_bCurEnableBloom != m_param.m_bEnableBloom
		)
	{
		const char* szMacros[10];
		int t = 0;

		if(m_bSupportSM30)
		{ 
			if(m_param.m_nRain)				{	szMacros[t] = "ENABLE_RAIN";		t+=1; }
			if(m_param.m_bCaustic)			{	szMacros[t] = "ENABLE_CAUSTIC";		t+=1; }
			//if(m_param.m_bHighRefr)		{	szMacros[t] = "ENABLE_HIGH_REFRA";	t+=1; }
			//if(m_param.m_bUseFFT)			{	szMacros[t] = "ENABLE_FFT";			t+=1; }
			//if(m_param.m_bEnableBloom)	{	szMacros[t] = "ENABLE_BLOOM";		t+=1; }
		}
		if(m_param.m_bEnableWave)			{	szMacros[t] = "ENABLE_WAVE";		t+=1; }
		if(m_param.m_bEnableFog)			{	szMacros[t] = "ENABLE_FOG";			t+=1; }

		ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLWater);
		m_pHLSLWater = CreateA3DHLSL_Common(FN_Water_FFT_HLSL, "vs", FN_Water_FFT_HLSL, "ps", t, szMacros, m_pA3DDevice, m_declEle);

		m_nCurEnableRain = m_param.m_nRain;
		m_bCurEnableCaustic = m_param.m_bCaustic;
		m_bCurEnableHighRefr = m_param.m_bHighRefr;
		m_bCurEnableFFT = m_param.m_bUseFFT;
		m_bCurEnableWave = m_param.m_bEnableWave;
		m_bCurEnableFog = m_param.m_bEnableFog;
		m_bCurEnableBloom = m_param.m_bEnableBloom;
	}

	if(!m_pHLSLWater)
		return;


	float fWaterHeightAtCameraPos =  m_param.m_fWaterLevel + GetWaveHeight(pCamera->GetPos());
	bool bUnderWater = false;
	if(pCamera->GetPos().y < fWaterHeightAtCameraPos)
	{
		if(m_param.m_bEnableAreaConstrain)
		{
			if(		pCamera->GetPos().x > m_param.m_vAreaConstrain.x && pCamera->GetPos().x < m_param.m_vAreaConstrain.z
				&&  pCamera->GetPos().z > m_param.m_vAreaConstrain.y && pCamera->GetPos().z < m_param.m_vAreaConstrain.w)
				bUnderWater = true;
		}
		else
			bUnderWater = true;
	}
	
	A3DVECTOR3 vMin, vMax;
	if(m_param.m_bEnableAreaConstrain)
	{
		vMin = A3DVECTOR3(m_param.m_vAreaConstrain.x, m_param.m_fWaterLevel - m_param.m_fWavesSize, m_param.m_vAreaConstrain.y);
		vMax = A3DVECTOR3(m_param.m_vAreaConstrain.z, m_param.m_fWaterLevel + m_param.m_fWavesSize, m_param.m_vAreaConstrain.w);
	}
	else
	{
		vMin = A3DVECTOR3(-1e10f, m_param.m_fWaterLevel - m_param.m_fWavesSize, -1e10f);
		vMax = A3DVECTOR3(1e10f,  m_param.m_fWaterLevel + m_param.m_fWavesSize, 1e10f);
	}
	A3DAABB aabbConstrain(vMin, vMax);
	aabbConstrain.CompleteCenterExts();
	bool bWaterSurfaceVisible = (pCamera->GetWorldFrustum()->AABBInFrustum(aabbConstrain) >= 0);

	//调用硬件遮挡剪裁
#ifdef _ANGELICA_2_1
#else
	if(bWaterSurfaceVisible)
	{
		if (m_pOccProxy)
		{
			m_pOccProxy->SetEnabled(true);
			aabbConstrain.Extents.y = 0.f;
			aabbConstrain.Extents.x = 1e4f;
			aabbConstrain.Extents.z = 1e4f;
			aabbConstrain.CompleteMinsMaxs();
			m_pOccProxy->SetAABB(aabbConstrain);
		}

		//occlusion culling
		if (m_pOccProxy && g_pA3DConfig->RT_GetUseOcclusionCullingFlag())
		{
			if(m_pOccProxy->IsOccluded(pRenderParam->pCurrentViewport))
				bWaterSurfaceVisible = false;
		}
	}
#endif



	//统计出需要绘制的SmallWater
	if( (pRenderParam->pTerrain != NULL && m_pCurrentTerrain != pRenderParam->pTerrain)
		|| m_fCurWaveAmount != m_param.m_fWavesAmount)
	{
		m_mapSmallWaterVisible[0].clear();
		m_mapSmallWaterVisible[1].clear();
		m_mapSmallWaterVisible[2].clear();
		m_pCurrentTerrain = pRenderParam->pTerrain;
		m_fCurWaveAmount = m_param.m_fWavesAmount;

		CreateTerrainRender( pRenderParam->pTerrain);
	}
	else if(pRenderParam->pTerrain == NULL)
	{
		m_pCurrentTerrain = NULL;
	}

	float fWidthAmountScale = 1.5f / m_param.m_fWavesAmount;
	float g_fSmallWaterWidthLod[3] = { 20.f * fWidthAmountScale, 45.f * fWidthAmountScale, 90.f * fWidthAmountScale};


	int nCurrentLOD = ComputeSmallWaterLOD(pCamera->GetPos().y);
	float fSmallWaterWidth = g_fSmallWaterWidthLod[nCurrentLOD];
	float fSmallWaterMaxDistance = fSmallWaterWidth * 6.f;
	float fSmallWaterCenterMaxDistance = fSmallWaterWidth * 7.f;
	std::vector<SmallWater> smallWaterList;
	A3DFrustum* pCameraFrustum = pCamera->GetWorldFrustum();
	bool bFrustumState[6];
	for(int i = 0; i < 6; i++)
		bFrustumState[i] = pCameraFrustum->PlaneIsEnable(i);

	{
		const int nMaxRenderSmallWater = 50;
		float fAABBXZLarger = m_param.m_fWavesSize * 1.5f;

		float fMaxY = m_param.m_fWaterLevel + m_param.m_fWavesSize;
		float fMinY = m_param.m_fWaterLevel - m_param.m_fWavesSize;

		int nCameraX = (int)floorf(pCamera->GetPos().x / fSmallWaterWidth);
		int nCameraZ = (int)floorf(pCamera->GetPos().z / fSmallWaterWidth);
		
		SmallWater sw;
		sw.m_iX = nCameraX;
		sw.m_iZ = nCameraZ;
		sw.m_fStartX = float(sw.m_iX) * fSmallWaterWidth;
		sw.m_fStartZ = float(sw.m_iZ) * fSmallWaterWidth;
		sw.m_aabb.Mins = A3DVECTOR3(		sw.m_fStartX - fAABBXZLarger,					
									fMinY,	sw.m_fStartZ - fAABBXZLarger);
		sw.m_aabb.Maxs = A3DVECTOR3(		sw.m_fStartX + fSmallWaterWidth + fAABBXZLarger,
									fMaxY,	sw.m_fStartZ + fSmallWaterWidth + fAABBXZLarger);
		sw.m_aabb.CompleteCenterExts();
		sw.m_fDistanceToCamera = 0.f;

		//只判断视椎的左右平面
		pCameraFrustum->EnablePlane(A3DFrustum::VF_LEFT, true);
		pCameraFrustum->EnablePlane(A3DFrustum::VF_RIGHT, true);
		pCameraFrustum->EnablePlane(A3DFrustum::VF_TOP, false);
		pCameraFrustum->EnablePlane(A3DFrustum::VF_BOTTOM, false);
		pCameraFrustum->EnablePlane(A3DFrustum::VF_NEAR, false);
		pCameraFrustum->EnablePlane(A3DFrustum::VF_FAR, false);
		
		bool swVisible = (pCameraFrustum->AABBInFrustum(sw.m_aabb) >= 0);

		std::vector<SmallWater> tempList;
		tempList.reserve(200);

		tempList.push_back(sw);
		

		abase::hash_map<int, VisitedItem> visitedSet;
		VisitedItem vi;
		vi.bVisible = swVisible;
		vi.fDistance = 0.f;

		visitedSet[HashSmallWaterPosition(sw.m_iX,sw.m_iZ)] = vi;

		while(!tempList.empty() && smallWaterList.size() < nMaxRenderSmallWater)
		{
			std::pop_heap(tempList.begin(), tempList.end());
			SmallWater temp = tempList.back();
			tempList.pop_back();
			
			if(temp.m_fDistanceToCamera >= fSmallWaterCenterMaxDistance)
				break;

			const int offset[4][2] = { {0,1}, {1,0}, {0, -1}, {-1, 0}};
			for(int i = 0; i < 4; i++)
			{
				sw.m_iX = temp.m_iX + offset[i][0];
				sw.m_iZ = temp.m_iZ + offset[i][1];
				
				if(visitedSet.find(HashSmallWaterPosition(sw.m_iX,sw.m_iZ)) != visitedSet.end())
				{
					VisitedItem& vi = visitedSet[HashSmallWaterPosition(sw.m_iX,sw.m_iZ)];
					if(vi.bVisible && vi.fDistance < fSmallWaterCenterMaxDistance)
						temp.m_bHasNeighbor[i] = true;
					else
						temp.m_bHasNeighbor[i] = false;
					continue;
				}

				sw.m_fStartX = float(sw.m_iX) * fSmallWaterWidth;
				sw.m_fStartZ = float(sw.m_iZ) * fSmallWaterWidth;
				sw.m_aabb.Mins = A3DVECTOR3(		sw.m_fStartX - fAABBXZLarger,					
											fMinY,	sw.m_fStartZ - fAABBXZLarger);
				sw.m_aabb.Maxs = A3DVECTOR3(		sw.m_fStartX + fSmallWaterWidth + fAABBXZLarger,
											fMaxY,	sw.m_fStartZ + fSmallWaterWidth + fAABBXZLarger);
				sw.m_aabb.CompleteCenterExts();
				sw.m_fDistanceToCamera = (sw.m_aabb.Center - pCamera->GetPos()).Magnitude();

				bool swVisible = (pCameraFrustum->AABBInFrustum(sw.m_aabb) >= 0);
									
				if(swVisible && sw.m_fDistanceToCamera < fSmallWaterCenterMaxDistance)
					temp.m_bHasNeighbor[i] = true;
				else
					temp.m_bHasNeighbor[i] = false;

				VisitedItem vi;
				vi.bVisible = swVisible;
				vi.fDistance = sw.m_fDistanceToCamera;
				visitedSet[HashSmallWaterPosition(sw.m_iX,sw.m_iZ)] = vi;
				
				if(swVisible)
				{
					tempList.push_back(sw);
					std::push_heap(tempList.begin(), tempList.end());
				}
			}
			if(visitedSet[HashSmallWaterPosition(temp.m_iX,temp.m_iZ)].bVisible)
				smallWaterList.push_back(temp);	
		}
	}
	
	
	for(unsigned int i = 0; i < smallWaterList.size(); i++)
	{
		//m_pA3DDevice->GetA3DEngine()->GetA3DWireCollector()->AddAABB(smallWaterList[i].m_aabb, A3DCOLORRGBA(255, 255, 0, 128));
		SmallWater& sw = smallWaterList[i];

		if(sw.m_fDistanceToCamera > fSmallWaterMaxDistance * 0.6)
			continue;

		const A3DAABB& aabb = smallWaterList[i].m_aabb;
		bool bUnderTerrain = false;
		bool bFrusVisible = true;
		if(pRenderParam->pTerrain)
		{
			SmallWaterUnderTerrianMap& currentMap = m_mapSmallWaterVisible[nCurrentLOD];
			SmallWaterUnderTerrianMap::iterator it = currentMap.find(HashSmallWaterPosition(sw.m_iX,sw.m_iZ));
			if(it == currentMap.end())
			{
				bUnderTerrain = pRenderParam->pTerrain->AABBBelowTerrain(aabb);
				currentMap[HashSmallWaterPosition(sw.m_iX,sw.m_iZ)] = bUnderTerrain;
			}
			else
				bUnderTerrain = it->second;
		}
		
		if(!bUnderTerrain)
		{
			//只判断视椎的上下平面
			pCameraFrustum->EnablePlane(A3DFrustum::VF_LEFT, false);
			pCameraFrustum->EnablePlane(A3DFrustum::VF_RIGHT, false);
			pCameraFrustum->EnablePlane(A3DFrustum::VF_TOP, true);
			pCameraFrustum->EnablePlane(A3DFrustum::VF_BOTTOM, true);
			pCameraFrustum->EnablePlane(A3DFrustum::VF_NEAR, false);
			pCameraFrustum->EnablePlane(A3DFrustum::VF_FAR, false);
			bFrusVisible = (pCameraFrustum->AABBInFrustum(aabb) >= 0);
		}
		if(bUnderTerrain || !bFrusVisible)
			sw.m_bRender = false;
	}

	//还原Frustum设置
	for(int i = 0; i < 6; i++)
		pCameraFrustum->EnablePlane(i, bFrustumState[i]);


	unsigned int nScreenWidth = m_pA3DDevice->GetDevFormat().nWidth;
	unsigned int nScreenHeight = m_pA3DDevice->GetDevFormat().nHeight;

	A3DRenderTarget* m_pReflectionRT = NULL;
	A3DRenderTarget* m_pBackBufferBeforeWater = NULL;
	A3DRenderTarget* m_pBackBufferBeforeUnderwater = NULL;
	A3DRenderTarget* m_pCausticRT = NULL;
	A3DRenderTarget* m_pDepthRT = NULL;
	A3DRenderTarget* m_pDepthStencilRT = NULL;
	A3DRenderTarget* m_pSmallDepthRT = NULL;
	A3DRenderTarget* m_pSmallGodRayRT = NULL;

	A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();

	m_pReflectionRT = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_A8R8G8B8);
	m_pBackBufferBeforeWater = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_A8R8G8B8);
	m_pCausticRT = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_A8R8G8B8);
	m_pDepthRT = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_R16F);
	m_pDepthStencilRT = pRTMan->RentRenderTargetDepth(0, 0, A3DFMT_D24S8);
	if(bUnderWater)
	{
		m_pBackBufferBeforeUnderwater = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_A8R8G8B8);
		m_pSmallDepthRT = pRTMan->RentRenderTargetColor(-4, -4, A3DFMT_R16F);
		m_pSmallGodRayRT = pRTMan->RentRenderTargetColor(-4, -4, A3DFMT_A8R8G8B8);
	}
	if(!m_pReflectionRT || !m_pBackBufferBeforeWater || !m_pCausticRT || !m_pDepthRT || !m_pDepthStencilRT)
	{
		pRTMan->ReturnRenderTarget(m_pReflectionRT);
		pRTMan->ReturnRenderTarget(m_pBackBufferBeforeWater);
		pRTMan->ReturnRenderTarget(m_pBackBufferBeforeUnderwater);
		pRTMan->ReturnRenderTarget(m_pCausticRT);
		pRTMan->ReturnRenderTarget(m_pDepthRT);
		pRTMan->ReturnRenderTarget(m_pDepthStencilRT);
		pRTMan->ReturnRenderTarget(m_pSmallDepthRT);
		pRTMan->ReturnRenderTarget(m_pSmallGodRayRT);

		return;
	}

	float fov = pCamera->GetFOV();
	float ratio = pCamera->GetRatio();

	A3DCULLTYPE cullType = m_pA3DDevice->GetFaceCull();
	bool bZEnable = m_pA3DDevice->GetZTestEnable();
	bool bZWriteEnable = m_pA3DDevice->GetZWriteEnable();
	bool bAlphaBlendEnable = m_pA3DDevice->GetAlphaBlendEnable();
	DWORD dwSrcAlpha = m_pA3DDevice->GetDeviceRenderState(D3DRS_SRCBLEND);
	DWORD dwDestAlpha = m_pA3DDevice->GetDeviceRenderState(D3DRS_DESTBLEND);
	DWORD dwColorWriteEnable = m_pA3DDevice->GetDeviceRenderState(D3DRS_COLORWRITEENABLE);
	DWORD dwZFunc = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZFUNC);

	A3DMATRIX4 lightViewMat;
	A3DVECTOR3 vSunDir = pRenderParam->vecSunDir;
	vSunDir.Normalize();

	A3DVECTOR3 dirZ = -vSunDir;

	dirZ.Normalize();
	A3DVECTOR3 up(0,1,0);
	if(fabsf(DotProduct(dirZ, up)) > 0.999)
	{
		up = A3DVECTOR3(0,0,1);
	}
	A3DVECTOR3 dirX, dirY;
	dirX = CrossProduct(up, dirZ);
	dirX.Normalize();
	dirY = CrossProduct(dirZ, dirX);
	dirY.Normalize();
	dirX = -dirX;
	lightViewMat = A3DMATRIX4 (
		dirX.x, dirX.y, dirX.z, 0,
		dirY.x, dirY.y, dirY.z, 0,
		dirZ.x, dirZ.y, dirZ.z, 0,
		0,		0,		0,		1
		);


	A3DVECTOR3 vWaterFogColor;
	//设置雾的颜色

	if(m_param.m_bUseCustomColor)
		vWaterFogColor = m_param.m_vWaterFogColor;
	else
	{
#ifdef _ANGELICA_2_1
		A3DCOLORVALUE fogColor2 = m_pA3DDevice->GetA3DEngine()->GetSceneRenderConfig()->GetFogColor2();
#else
		A3DCOLORVALUE fogColor2 = A3DCOLORVALUE(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetFogParam()->crFog2);
#endif
		fogColor2 *= 0.5;
		vWaterFogColor = A3DVECTOR3(fogColor2.r, fogColor2.g, fogColor2.b);
	}

	float fCausticOffsetX = 0.f;
	float fCausticOffsetY = 0.f;
	float fct = float(double(a_GetTime()) / 1000.0);
	fCausticOffsetX = 0.3f * sinf(fct * 2.5f) + 0.2f * cosf(fct * 0.4f);
	fCausticOffsetY = -0.3f * sinf(fct * 1.7f) + 0.04f * cosf(fct * 1.6f);

	float fCausticLerp = 0.f;
	int nCausticTexIndex1 = 0, nCausticTexIndex2 = 0;
	float causticDuration = 1.f;
	int durationInMS = int(causticDuration * 1000.0);
	DWORD currentTime = DWORD((double)a_GetTime()
		+ 345.0 * sin((double)a_GetTime() / 5432.0)
		+ 567.f * cos((double)a_GetTime() / 9965.0)
		+ 723.f * sin((double)a_GetTime() / 17777.0));
	double fCurrentTextureIndex = double(currentTime % durationInMS) / double(durationInMS) * (double)FFT_WATER_CAUSTIC_TEXTURE_NUM;
	nCausticTexIndex1 = (int)floor(fCurrentTextureIndex);
	nCausticTexIndex2 = (nCausticTexIndex1 + 1) % FFT_WATER_CAUSTIC_TEXTURE_NUM;
	fCausticLerp = (float)fCurrentTextureIndex - (float)nCausticTexIndex1;

	DWORD fillMode;

	A3DMATRIX4 invView = pCamera->GetViewTM();
	invView.InverseTM();

	float fovy_2 = pCamera->GetFOV() / 2.f;
	float aspRatio = pCamera->GetRatio();

	float f = pCamera->GetZBack();
	float top = f * tan(fovy_2);
	float bottom = -top;
	float right = top * aspRatio;
	float left = -right;
	A3DVECTOR3 v[4];
	v[0] = A3DVECTOR3(left, bottom, f);
	v[1] = A3DVECTOR3(right, bottom, f);
	v[2] = A3DVECTOR3(left, top, f);
	v[3] = A3DVECTOR3(right, top, f);

	ColorVertex* bww;
	bool bLock = m_pStream->LockVertexBufferDynamic(0, m_pStream->GetVertCount() * sizeof(ColorVertex), (BYTE**)&bww, 0);
	ASSERT(bLock);
	bww[0].Set(A3DVECTOR3(-1, -1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(0, 1));
	bww[1].Set(A3DVECTOR3( 1, -1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(1, 1));
	bww[2].Set(A3DVECTOR3(-1,  1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(0, 0));
	bww[3].Set(A3DVECTOR3( 1,  1, 0.5), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(1, 0));
	for(int i = 0; i < 4; i++)
	{
		A3DVECTOR4 temp;
		temp = v[i] * invView;
		A3DVECTOR3 temp2;
		temp2 = A3DVECTOR3(temp.x, temp.y, temp.z) - pCamera->GetPos();
		bww[i].m_n = A3DVECTOR3(temp2.x, temp2.y, temp2.z);
		v[i] = temp2;
	}
	bLock = m_pStream->UnlockVertexBufferDynamic();
	ASSERT(bLock);
	//渲染深度
	MakeDepthRT(pCurrentViewport, m_pDepthRT); //深度从IntZ获取, 转换为线性深度


	//渲染水下焦散到Caustic纹理上  //也许应该渲染到折射纹理的Alpha通道中
	if(m_param.m_bCaustic && (bWaterSurfaceVisible || bUnderWater))
	{
		A3DRenderTarget* p3DScreenDepthRT = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_A32B32G32R32F);

		Make3DScreenDepthRT(p3DScreenDepthRT, pCamera);

		m_pCausticRT->ApplyToDevice();

		m_pA3DDevice->Clear(D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->SetZTestEnable(false);
		m_pA3DDevice->SetZWriteEnable(false);
		m_pA3DDevice->SetAlphaBlendEnable(true);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN
			| D3DCOLORWRITEENABLE_BLUE);

		m_pHLSLCaustic->SetupVec3("g_farConor0", v[2]);
		m_pHLSLCaustic->SetupVec3("g_farConor1", v[3]);
		m_pHLSLCaustic->SetupVec3("g_farConor2", v[0]);
		m_pHLSLCaustic->SetupVec3("g_farConor3", v[1]);

		//float fSeparation = 0.02; //0.00825f; //0.117958;//
		//m_pHLSLCaustic->Setup1f("g_fSeparation", fSeparation * 0.5f);
		m_pHLSLCaustic->Setup1f("g_fFarPlane", f);
		//m_pHLSLCaustic->Setup1f("g_fNearPlane", pCamera->GetZFront());

		float fTanFovX_2 = tan(fovy_2) * aspRatio;
		m_pHLSLCaustic->Setup1f("g_fFovX_2", fTanFovX_2);


		m_pHLSLCaustic->SetupMatrix("g_lightViewMat", lightViewMat);


		m_pStream->Appear();
		m_pHLSLCaustic->SetupVec3("g_PS_SunLightDir", -pRenderParam->vecSunDir);
		m_pHLSLCaustic->SetupVec3("g_vCameraPos", pCamera->GetPos());
		m_pHLSLCaustic->Setup1f("g_fTime", (float)((double)a_GetTime() / 1000.0));

		// 		float2 g_vCausticOffset : register(c7);
		// 		float g_fCausticLerp : register(c8);

		m_pHLSLCaustic->Setup2f("g_vCausticOffset", fCausticOffsetX, fCausticOffsetY);

		m_pHLSLCaustic->Setup1f("g_fCausticLerp", fCausticLerp);

		float fScreenWidth = (float)pInputRT->GetWidth();
		float fScreenHeight = (float)pInputRT->GetHeight();
		A3DVECTOR4 g_vScreenSize(fScreenWidth, fScreenHeight, 0.5f / fScreenWidth, 0.5f / fScreenHeight);
		m_pHLSLCaustic->SetupVec4("g_vScreenSize", g_vScreenSize);

		m_pHLSLCaustic->Appear();

		m_pVertDecl->Appear();

		m_pOceanWaveNormalTex->Appear(0);
		m_pDepthRT->AppearAsTexture(1);
		m_pCausticTex[nCausticTexIndex1]->Appear(2);
		m_pCausticTex[nCausticTexIndex2]->Appear(3);
		p3DScreenDepthRT->AppearAsTexture(4);


		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_POINT);
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(4, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);//RenderQuad();

		m_pHLSLCaustic->Disappear();

		m_pCausticRT->WithdrawFromDevice();

		pRTMan->ReturnRenderTarget(p3DScreenDepthRT);

		m_pA3DDevice->SetFaceCull(cullType);
		m_pA3DDevice->SetZTestEnable(bZEnable);
		m_pA3DDevice->SetZWriteEnable(bZWriteEnable);
		m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlendEnable);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_SRCBLEND, dwSrcAlpha);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_DESTBLEND, dwDestAlpha);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, dwColorWriteEnable);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_ZFUNC, dwZFunc);	
	}


	if(bWaterSurfaceVisible || bUnderWater)
	{
 		m_pA3DDevice->StretchRect(pInputRT->GetTargetSurface()->m_pD3DSurface, NULL, 
 			m_pBackBufferBeforeWater->GetTargetSurface()->m_pD3DSurface, NULL, A3DTEXF_POINT);
 
 		if(bUnderWater)
 			m_pA3DDevice->StretchRect(pInputRT->GetTargetSurface()->m_pD3DSurface, NULL, 
 			m_pBackBufferBeforeUnderwater->GetTargetSurface()->m_pD3DSurface, NULL, A3DTEXF_POINT);
	}
	

	if(bWaterSurfaceVisible)
	{
		m_pCameraReflect->SetProjectionParam(fov, pCamera->GetZFront(), pCamera->GetZBack(), ratio);

		//	Create viewports
		m_pReflectViewport->GetParam()->X = 0;
		m_pReflectViewport->GetParam()->Y = 0;
		m_pReflectViewport->GetParam()->Width = nScreenWidth;
		m_pReflectViewport->GetParam()->Height = nScreenHeight;
		m_pReflectViewport->SetClearColor(0xFF008FFD);

		m_pReflectViewport->SetCamera(m_pCameraReflect);

		m_pDepthStencilRT->ApplyToDevice();

		//制作反射纹理
		if(m_pCubeTexture && m_param.m_bCubeReflection)
			;
		else
			MakeReflectionRT(m_pReflectionRT, bUnderWater, pCamera, pRenderParam->vecSunDir, pCurrentViewport, pRenderParam->pTerrain,
				pRenderParam->pSky, pRenderParam->pTerrainOutline, pRenderParam->pReflectCallBack, 
				pRenderParam->pArg, pRenderParam->pfnRenderOnSky, pRenderParam->pArgSky);
		
		m_pDepthStencilRT->WithdrawFromDevice();


		m_pHLSLWater->SetupVec3("g_waterFogColor", vWaterFogColor);
		m_pHLSLWater->Setup1f("g_fFogDensity", m_param.m_fFogDensity);
		m_pHLSLWater->Setup1f("g_fWaterFogScaleExp", m_param.m_fWaterFogScaleExp);

		static unsigned int nCurrentFrame = 0;
		nCurrentFrame++;

		const float fUpdateTime = 0.75f * 0.125f * (float)((double)a_GetTime() / 1000.0) * 0.75f;

		if(m_param.m_bEnableWave && m_param.m_bUseFFT)	//更新FFT
		{
			if(m_cryWaterFFT.GetWind() != m_param.m_fWindDirection || m_cryWaterFFT.GetGravity() != m_param.m_fGravity || 
				m_cryWaterFFT.GetAConstant() != m_fAConstant)
			{
				m_cryWaterFFT.Create(m_param.m_fGravity, m_fAConstant, m_param.m_fWindDirection, 1.0f, 1.0f);
			}
			m_cryWaterFFT.Update( nCurrentFrame, fUpdateTime );

			const int nGridSize = 64;
			A3DVECTOR4 *pDispGrid = m_cryWaterFFT.GetDisplaceGrid();
			uint32 pitch = 4 * sizeof(float) * nGridSize; 
			uint32 width = nGridSize; 
			uint32 height = nGridSize;

			void* pData = NULL;
			int nPitch = 0;
			m_pFFTTexture->LockRectDynamic(NULL, &pData, &nPitch, 0);
			memcpy(pData, pDispGrid, 4 * width * height * sizeof(float));
			m_pFFTTexture->UnlockRectDynamic();
		}


		A3DMATRIX4 matView = pCamera->GetViewTM();
		A3DMATRIX4 matViewProj = pCamera->GetViewTM() * pCamera->GetProjectionTM();

		m_pHLSLWater->SetupMatrix("g_mViewProj", matViewProj);

		A3DMATRIX4 matRefl;
		matRefl._11 = 0.5f; matRefl._12 = 0.f;		matRefl._13 = 0.f; matRefl._14 = 0.f;
		matRefl._21 = 0.0f; matRefl._22 = -0.5f;	matRefl._23 = 0.f; matRefl._24 = 0.f;
		matRefl._31 = 0.0f; matRefl._32 = 0.0f;		matRefl._33 = 0.5f; matRefl._34 = 0.f;
		matRefl._41 = 0.5f + 0.5f / (float)nScreenWidth;
		matRefl._42 = 0.5f + 0.5f / (float)nScreenHeight;
		matRefl._43 = 0.5f;
		matRefl._44 = 1.f;
		matRefl = matViewProj * matRefl;

		m_pHLSLWater->SetupMatrix("g_mReflProj", matRefl);

		m_pHLSLWater->Setup1f("g_fWaterLevelAtCamera", fWaterHeightAtCameraPos);
		m_pHLSLWater->Setup1f("g_fWaterLevel", m_param.m_fWaterLevel);

		//m_pHLSLWater->Setup1f("g_bUseFFT", (m_param.m_bUseFFT ? 1.f : 0.f));

		m_pHLSLWater->SetupVec3("g_PS_SunLightDir", -vSunDir);
		A3DVECTOR3 g_PS_SkyColor = A3DVECTOR3(1,1,1);
		m_pHLSLWater->SetupVec3("g_PS_SkyColor", g_PS_SkyColor);
		A3DVECTOR3 g_PS_SunColor(1,1,1);
		m_pHLSLWater->SetupVec3("g_PS_SunColor", g_PS_SunColor);

		m_pHLSLWater->Setup1f("g_fWindSpeed", m_param.m_fWindSpeed);
		m_pHLSLWater->Setup1f("g_fWaveSpeed", m_param.m_fWavesSpeed);
		m_pHLSLWater->Setup1f("g_fWaveAmount", m_param.m_fWavesAmount);

		float fDirCos = cosf(m_param.m_fWindDirection);
		float fDirSin = sinf(m_param.m_fWindDirection);
		m_pHLSLWater->Setup1f("g_fWaveSize", m_param.m_fWavesSize);
		m_pHLSLWater->Setup1f("g_fDirCos", fDirCos);
		m_pHLSLWater->Setup1f("g_fDirSin", fDirSin);

		A3DVECTOR4 g_VS_NearFarClipDist(pCamera->GetZFront(), pCamera->GetZBack(), 1.f / pCamera->GetZFront(), 1.f / pCamera->GetZBack());
		m_pHLSLWater->SetupVec4("g_vNearFarDis", g_VS_NearFarClipDist);
		m_pHLSLWater->SetupVec3("g_vCameraPos", pCamera->GetPos());

		A3DVECTOR3 vCameraDir = pCamera->GetDir();
		vCameraDir.Normalize();

		float fHAngle = atan2f(vCameraDir.x, vCameraDir.z);
		fHAngle -= PI * 0.5f; 
		fHAngle += PI * 0.25;
		D3DXVECTOR2 g_vOceanMeshRot(cosf(-fHAngle), sinf(-fHAngle));
		m_pHLSLWater->Setup2f("g_vOceanMeshRot", g_vOceanMeshRot.x, g_vOceanMeshRot.y);

		float fTime = (float)((double)a_GetTime() / 1000.0);
		m_pHLSLWater->Setup1f("g_fTime", fTime);

		float fScreenWidth = (float)pInputRT->GetWidth();
		float fScreenHeight = (float)pInputRT->GetHeight();
		A3DVECTOR4 g_vScreenSize(fScreenWidth, fScreenHeight, 0.5f / fScreenWidth, 0.5f / fScreenHeight);
		m_pHLSLWater->SetupVec4("g_vScreenSize", g_vScreenSize);

		m_pHLSLWater->Setup1f("g_fSoftIntersectionFactor", m_param.m_fSoftIntersectionFactor);
		m_pHLSLWater->Setup1f("g_fReflectionAmount", m_param.m_fReflectionAmount);
		m_pHLSLWater->Setup1f("g_fWhiteCapsAmount", m_param.m_fWhiteCapsAmount);
		m_pHLSLWater->Setup1f("g_fSunMultiplier", m_param.m_fSunMultiplier);
		m_pHLSLWater->Setup1f("g_fFresnelScale", m_param.m_fFresnelScale);
		m_pHLSLWater->Setup1f("g_fEdgeFoamAmount", m_param.m_fEdgeFoamAmount);

		//设置Foam和Gloss的调试参数
		m_pHLSLWater->Setup1f("g_bEdgeFoam", (m_param.m_bEdgeFoam ? 1.f : 0.f));
		m_pHLSLWater->Setup1f("g_bWaveFoam", (m_param.m_bWaveFoam ? 1.f : 0.f));
		m_pHLSLWater->Setup1f("g_bWaveGloss", (m_param.m_bWaveGloss ? 1.f : 0.f));
		m_pHLSLWater->Setup1f("g_bRain", (m_param.m_nRain > 0 ? 1.f : 0.f));
		m_pHLSLWater->Setup1f("g_bCaustic", (m_param.m_bCaustic ? 1.f : 0.f));

#ifdef _WATER_FFT_DEBUG_
		if(GetAsyncKeyState(VK_F10) & 0x8000)
		{
			D3DXSaveSurfaceToFileA("WaterFFT m_pCausticRT.dds", D3DXIFF_DDS, m_pCausticRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA("WaterFFT m_pDepthRT.dds", D3DXIFF_DDS, m_pDepthRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA("WaterFFT m_pReflectionRT.dds", D3DXIFF_DDS, m_pReflectionRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
			D3DXSaveSurfaceToFileA("WaterFFT m_pBackBufferBeforeWater.dds", D3DXIFF_DDS, m_pBackBufferBeforeWater->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
			LPDIRECT3DSURFACE9 surface;
			m_pFFTTexture->GetD3DTexture()->GetSurfaceLevel(0, &surface);
			D3DXSaveSurfaceToFileA("WaterFFT m_pFFTTexture.dds", D3DXIFF_DDS, surface, NULL, NULL);
			surface->Release();


			m_pOceanWaveNormalTex->GetD3DTexture()->GetSurfaceLevel(0, &surface);
			D3DXSaveSurfaceToFileA("WaterFFT m_pOceanWaveNormalTex.dds", D3DXIFF_DDS, surface, NULL, NULL);
			surface->Release();

			m_pWaterFoamTex->GetD3DTexture()->GetSurfaceLevel(0, &surface);
			D3DXSaveSurfaceToFileA("WaterFFT m_pWaterFoamTex.dds", D3DXIFF_DDS, surface, NULL, NULL);
			surface->Release();

			m_pWaterGlossTex->GetD3DTexture()->GetSurfaceLevel(0, &surface);
			D3DXSaveSurfaceToFileA("WaterFFT m_pWaterGlossTex.dds", D3DXIFF_DDS, surface, NULL, NULL);
			surface->Release();
		}
#endif
		//渲染水面
		if(bUnderWater)
			m_pBackBufferBeforeUnderwater->ApplyToDevice();

		if(pRenderParam->pBloomRT)
			pRenderParam->pBloomRT->ApplyToDevice(1);

		m_pA3DDevice->SetZTestEnable(true);
		m_pA3DDevice->SetZFunc(D3DCMP_LESSEQUAL);
		m_pA3DDevice->SetZWriteEnable(true);
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE,  D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);


		m_pA3DDevice->SetAlphaBlendEnable(false);

		//m_pCryWaterStream->Appear();
		m_pSmallWaterStream->Appear();
		m_pVertDecl->Appear();

		if(m_param.m_nRain == 1)
			m_fRainDuration = 2.f;
		else if(m_param.m_nRain == 2)
			m_fRainDuration = 1.f;
		int durationInMS = int(m_fRainDuration * 1000.0);
		double fCurrentTextureIndex = double(a_GetTime() % durationInMS) / double(durationInMS) * (double)FFT_WATER_RAIN_TEXTURE_NUM;
		int nCurrentTextureIndex = (int)floor(fCurrentTextureIndex);
		int nNextTextureIndex = (nCurrentTextureIndex + 1) % FFT_WATER_RAIN_TEXTURE_NUM;
		float fRainLerp = (float)fCurrentTextureIndex - (float)nCurrentTextureIndex;

		//g_A3DErrLog.Log("Current Rain Index: %f", fCurrentTextureIndex);

		m_pHLSLWater->Setup1f("g_fRainLerp", fRainLerp);


		A3DVECTOR4 areaCons = m_param.m_vAreaConstrain;
		if(!m_param.m_bEnableAreaConstrain)
		{
			areaCons = A3DVECTOR4(-1e10f, -1e10f, 1e10f, 1e10f);
		}
		m_pHLSLWater->SetupVec4("g_waterArea", areaCons);
		m_pHLSLWater->Setup1f("g_bEnableArea", m_param.m_bEnableAreaConstrain ? 1.f : 0.f);

		float bReflCube;
		if(m_param.m_bCubeReflection && m_pCubeTexture)
			bReflCube = 1.f;
		else
			bReflCube = 0.f;

		m_pHLSLWater->Setup1f("g_bCubeRefl", bReflCube);

		
		m_pHLSLWater->Setup4f("g_vSmallWaterWidth", fSmallWaterWidth, fSmallWaterMaxDistance * 0.8f, fSmallWaterMaxDistance, 0);


#ifdef _ANGELICA_2_1
		m_pHLSLWater->Appear();
		m_pA3DDevice->GetA3DEngine()->GetSceneRenderConfig()->AppearFog(28, true);
		m_pA3DDevice->GetA3DEngine()->GetSceneRenderConfig()->AppearFog(28, false);
#else
		m_pHLSLWater->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());
#endif

		m_pFFTTexture->Appear(D3DVERTEXTEXTURESAMPLER0);
		m_pOceanWaveNormalTex->Appear(4);
		m_pWaterFoamTex->Appear(1);
		m_pWaterGlossTex->Appear(2);

		m_pBackBufferBeforeWater->AppearAsTexture(6);
		m_pDepthRT->AppearAsTexture(7);
		m_pCausticRT->AppearAsTexture(8);

		if(m_param.m_nRain == 1)
		{
			m_pRainNormalTex[nCurrentTextureIndex]->Appear(9);
			m_pRainNormalTex[nNextTextureIndex]->Appear(10);
		}
		else if(m_param.m_nRain == 2)
		{
			m_pRainXLNormalTex[nCurrentTextureIndex]->Appear(9);
			m_pRainXLNormalTex[nNextTextureIndex]->Appear(10);
		}

		if(m_param.m_bCubeReflection && m_pCubeTexture)
		{
			m_pCubeTexture->Appear(11);
			m_pSmallMap1x1->Appear(5);
		}
		else
		{
			m_pReflectionRT->AppearAsTexture(5);
			//m_pSmallMap1x1->Appear(11);
			m_pA3DDevice->ClearTexture(11);
		}
		//设置纹理采样方式
		m_pA3DDevice->SetTextureFilterType(D3DVERTEXTEXTURESAMPLER0, A3DTEXF_POINT);
		m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
		m_pA3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
		m_pA3DDevice->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureFilterType(6, A3DTEXF_POINT);
		m_pA3DDevice->SetTextureFilterType(7, A3DTEXF_POINT);
		m_pA3DDevice->SetTextureFilterType(8, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureFilterType(9, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureFilterType(10, A3DTEXF_LINEAR);
		m_pA3DDevice->SetSamplerState(9, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		m_pA3DDevice->SetSamplerState(10, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		m_pA3DDevice->SetTextureFilterType(11, A3DTEXF_LINEAR);

		m_pA3DDevice->SetTextureAddress(D3DVERTEXTEXTURESAMPLER0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(5, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureAddress(6, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureAddress(7, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureAddress(8, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(9, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(10, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(11, A3DTADDR_WRAP, A3DTADDR_WRAP);

		fillMode = m_pA3DDevice->GetDeviceRenderState(D3DRS_FILLMODE);
		if(m_bWireframe)
			m_pA3DDevice->SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		//新的绘制函数
		int vertCount = m_pSmallWaterStream->GetVertCount();
		int primCount = m_pSmallWaterStream->GetIndexCount() / 3;
		for(unsigned int i = 0; i < smallWaterList.size(); i++)
		{
			const SmallWater& sw = smallWaterList[i];
			if(!sw.m_bRender)
				continue;

			//m_pHLSLWater->Setup2f("g_vCurrentSmallWaterStart", sw.m_fStartX, sw.m_fStartZ);
			A3DVECTOR4 vCurrentSmallWaterStart = A3DVECTOR4(sw.m_fStartX, sw.m_fStartZ, 0, 0);
			m_pA3DDevice->SetVertexShaderConstants(32, &vCurrentSmallWaterStart, 1);

// 			float fNoNeighbor[4];
// 			for(int j = 0; j < 4; j++)
// 			{
// 				if(sw.m_bHasNeighbor[j])
// 					fNoNeighbor[j] = 0.f;
// 				else
// 					fNoNeighbor[j] = 1.f;
// 			}

			//m_pHLSLWater->Setup4f("g_vSideNoNeighber", fNoNeighbor[0], fNoNeighbor[1], fNoNeighbor[2], fNoNeighbor[3]);

// #ifdef _ANGELICA_2_1
// 			m_pHLSLWater->ApplyParams();
// #else
// 			m_pHLSLWater->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());
// #endif
			
			bool bdp = m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, vertCount, 0, primCount);
			ASSERT(bdp);
		}
		
		m_pA3DDevice->SetDeviceRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		m_pHLSLWater->Disappear();


		if(pRenderParam->pBloomRT)
			pRenderParam->pBloomRT->WithdrawFromDevice();

		if(bUnderWater)
			m_pBackBufferBeforeUnderwater->WithdrawFromDevice();
	
	}
	
	m_pDepthRT->DisappearAsTexture(7);
	//绘制水下雾和焦散
	if(bUnderWater && m_bSupportSM30)
	{
		//重新绘制深度
		MakeDepthRT(pCurrentViewport, m_pDepthRT); //深度从IntZ获取, 转换为线性深度


		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->SetZTestEnable(false);
		m_pA3DDevice->SetZWriteEnable(false);
		m_pA3DDevice->SetAlphaBlendEnable(false);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN
			| D3DCOLORWRITEENABLE_BLUE);

		//DownSample深度RT
		m_pSmallDepthRT->ApplyToDevice();
		m_pStream->Appear();
		m_pVertDecl->Appear();

		m_pHLSLDownSamp->Setup2f("gpost_vPixelSize", 
			4.f / float(pCurrentViewport->GetParam()->Width),
			4.f / float(pCurrentViewport->GetParam()->Height));

		m_pHLSLDownSamp->Appear();

		m_pDepthRT->AppearAsTexture(0);
		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		m_pHLSLDownSamp->Disappear();
		m_pSmallDepthRT->WithdrawFromDevice();

		//把水下GodRay绘制到RT上
		m_pSmallGodRayRT->ApplyToDevice();
		m_pA3DDevice->Clear(D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		m_pStream->Appear();
		m_pVertDecl->Appear();

		m_pHLSLGodray->SetupMatrix("g_lightViewMat", lightViewMat);
		m_pHLSLGodray->SetupVec3("g_PS_SunLightDir", -pRenderParam->vecSunDir);
		m_pHLSLGodray->SetupVec3("g_vCameraPos", pCamera->GetPos());
		m_pHLSLGodray->Setup1f("g_fTime", (float)((double)a_GetTime() / 1000.0));
		m_pHLSLGodray->SetupVec3("g_waterFogColor", vWaterFogColor);
		m_pHLSLGodray->Setup1f("g_fFogDensity", m_param.m_fFogDensity);
		m_pHLSLGodray->Setup1f("g_fWaterFogScaleExp", m_param.m_fWaterFogScaleExp);
		m_pHLSLGodray->Setup1f("g_fWaterHeight", fWaterHeightAtCameraPos);

		m_pHLSLGodray->Setup2f("g_vCausticOffset", fCausticOffsetX, fCausticOffsetY);
		m_pHLSLGodray->Setup1f("g_fCausticLerp", fCausticLerp);

		m_pA3DDevice->SetAlphaBlendEnable(true);
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCCOLOR);

		//渲染4遍,每遍采样8个位置, 
		for(int i = 0; i < 4; i++)
		{
			m_pHLSLGodray->Setup1f("g_fStartDis", float(i * 8));
			m_pHLSLGodray->Appear();

			m_pSmallDepthRT->AppearAsTexture(0);
			m_pOceanWaveNormalTex->Appear(1);
			m_pCausticTex[nCausticTexIndex1]->Appear(2);
			m_pCausticTex[nCausticTexIndex2]->Appear(3);

			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);


			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			m_pHLSLGodray->Disappear();	
		}

		m_pA3DDevice->SetAlphaBlendEnable(false);
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

		m_pSmallGodRayRT->WithdrawFromDevice();


		//绘制水下
		m_pStream->Appear();
		m_pHLSLUnderWater->SetupVec3("g_vCameraPos", pCamera->GetPos());
		m_pHLSLUnderWater->SetupVec3("g_waterFogColor", vWaterFogColor);
		m_pHLSLUnderWater->Setup1f("g_fFogDensity", m_param.m_fFogDensity);
		m_pHLSLUnderWater->Setup1f("g_fWaterFogScaleExp", m_param.m_fWaterFogScaleExp);
		m_pHLSLUnderWater->Setup1f("g_fWaterHeight", fWaterHeightAtCameraPos);
		m_pHLSLUnderWater->Appear();

		m_pVertDecl->Appear();

		m_pDepthRT->AppearAsTexture(0);
		if(m_param.m_bCaustic)
			m_pCausticRT->AppearAsTexture(1);
		else
			m_pBlackTex->Appear(1);

		m_pBackBufferBeforeUnderwater->AppearAsTexture(2);
		m_pSmallGodRayRT->AppearAsTexture(3);

		m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
		m_pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);//RenderQuad();

		m_pHLSLUnderWater->Disappear();


#ifdef _WATER_FFT_DEBUG_
		if(GetAsyncKeyState(VK_F4) & 0x8000)
		{
			D3DXSaveSurfaceToFileA("WaterFFT SmallDepth.dds", D3DXIFF_DDS, m_pSmallDepthRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
		}
#endif

	}

	m_pA3DDevice->SetDeviceRenderState(D3DRS_FILLMODE, fillMode);

	m_pA3DDevice->SetFaceCull(cullType);
	m_pA3DDevice->SetZTestEnable(bZEnable);
	m_pA3DDevice->SetZWriteEnable(bZWriteEnable);
	m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlendEnable);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_SRCBLEND, dwSrcAlpha);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_DESTBLEND, dwDestAlpha);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, dwColorWriteEnable);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_ZFUNC, dwZFunc);	

	pRTMan->ReturnRenderTarget(m_pReflectionRT);
	pRTMan->ReturnRenderTarget(m_pBackBufferBeforeWater);
	pRTMan->ReturnRenderTarget(m_pBackBufferBeforeUnderwater);
	pRTMan->ReturnRenderTarget(m_pCausticRT);
	pRTMan->ReturnRenderTarget(m_pDepthRT);
	pRTMan->ReturnRenderTarget(m_pDepthStencilRT);
	pRTMan->ReturnRenderTarget(m_pSmallDepthRT);
	pRTMan->ReturnRenderTarget(m_pSmallGodRayRT);
}

float A3DTerrainWaterFFT::GetWaveHeight( const A3DVECTOR3& pPos )
{
	if(!m_param.m_bEnableWave)
	{
		return 0;
	}

	if( m_param.m_bUseFFT) 
	{
		A3DVECTOR4 pDispPos = A3DVECTOR4(0,0,0, 0);
		// Get height from FFT grid

		A3DVECTOR4 *pGridFFT = m_cryWaterFFT.GetDisplaceGrid();
		if( !pGridFFT )
			return 0.0f; 

		// match scales used in shader
		float fScaleX = pPos.x* 0.0125f * m_param.m_fWavesAmount* 1.25f;
		float fScaleY = pPos.y* 0.0125f * m_param.m_fWavesAmount* 1.25f;

		float fu = fScaleX * 64.0f;
		float fv = fScaleY * 64.0f;
		int u1 = ((int)fu) & 63;
		int v1 = ((int)fv) & 63;
		int u2 = (u1 + 1) & 63;
		int v2 = (v1 + 1) & 63;

		// Fractional parts
		float fracu = fu - floorf( fu );
		float fracv = fv - floorf( fv );

		// Get weights
		float w1 = (1 - fracu) * (1 - fracv); 
		float w2 = fracu * (1 - fracv);
		float w3 = (1 - fracu) * fracv;
		float w4 = fracu *  fracv;

		A3DVECTOR4 h1 = pGridFFT[u1 + v1 * 64];
		A3DVECTOR4 h2 = pGridFFT[u2 + v1 * 64];
		A3DVECTOR4 h3 = pGridFFT[u1 + v2 * 64];
		A3DVECTOR4 h4 = pGridFFT[u2 + v2 * 64];

		// scale and sum the four heights
		pDispPos  = h1 * w1 + h2 * w2 + h3 * w3 + h4 * w4;

		// match scales used in shader
		return pDispPos.y * 0.06f * m_param.m_fWavesSize;
	}

	// constant to scale down values a bit
	const float fAnimAmplitudeScale = 1.0f / 5.0f;

	static int nFrameID = 0; 
	static A3DVECTOR3 vFlowDir = A3DVECTOR3(0, 0, 0);  
	static A3DVECTOR4 vFrequencies = A3DVECTOR4(0, 0, 0, 0);
	static A3DVECTOR4 vPhases = A3DVECTOR4(0, 0, 0, 0);
	static A3DVECTOR4 vAmplitudes = A3DVECTOR4(0, 0, 0, 0);

	// Update per-frame data
	vFlowDir.y = sinf(m_param.m_fWindDirection);
	vFlowDir.x = cosf(m_param.m_fWindDirection);
	//SinCosf(m_oceanWindDirection, &vFlowDir.y, &vFlowDir.x);

	vFrequencies = A3DVECTOR4( 0.233f, 0.455f, 0.6135f, -0.1467f ) * m_param.m_fWavesSpeed * 5.0f; 
	vPhases = A3DVECTOR4(0.1f, 0.159f, 0.557f, 0.2199f) * m_param.m_fWavesAmount;
	vAmplitudes = A3DVECTOR4(1.0f, 0.5f, 0.25f, 0.5f) * m_param.m_fWavesSize;


	float fPhase = sqrtf(pPos.x * pPos.x + pPos.y * pPos.y);
	A3DVECTOR4 vCosPhase = vPhases * (fPhase + pPos.x);

	A3DVECTOR4 vWaveFreq = vFrequencies * (float)((double)a_GetTime() / 1000.0);//(float)Timer::GetInstance().GetTime() / 1000.f;

	A3DVECTOR4 vCosWave = A3DVECTOR4(
		cosf( vWaveFreq.x * vFlowDir.x + vCosPhase.x ),
		cosf( vWaveFreq.y * vFlowDir.x + vCosPhase.y ),
		cosf( vWaveFreq.z * vFlowDir.x + vCosPhase.z ),
		cosf( vWaveFreq.w * vFlowDir.x + vCosPhase.w ) );


	A3DVECTOR4 vSinPhase = vPhases * (fPhase + pPos.y);
	A3DVECTOR4 vSinWave = A3DVECTOR4( 
		sinf( vWaveFreq.x * vFlowDir.y + vSinPhase.x ),
		sinf( vWaveFreq.y * vFlowDir.y + vSinPhase.y ),
		sinf( vWaveFreq.z * vFlowDir.y + vSinPhase.z ),
		sinf( vWaveFreq.w * vFlowDir.y + vSinPhase.w ) );

	//return  ( vCosWave.Dot( vAmplitudes ) + vSinWave.Dot( vAmplitudes ) ) * fAnimAmplitudeScale; 
	return  (A3DVec4Dot(vCosWave, vAmplitudes) + A3DVec4Dot(vSinWave, vAmplitudes)) * fAnimAmplitudeScale; 
}


void A3DTerrainWaterFFT::MakeReflectionRT(A3DRenderTarget* pReflectionRT, bool bUnderWater,
										  A3DCamera* pCamera,
										  const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, 
										  LPRENDERFORREFLECT pReflectCallBack, void * pArg,
										  LPRENDERONSKY pfnRenderOnSky, void * pArgSky)
{
	pReflectionRT->ApplyToDevice();

	if(false)
	{
		pReflectionRT->WithdrawFromDevice();
		return;
	}

	A3DVECTOR3 vecCamPos = pCamera->GetPos();
	float fHeightBias = 0.f;
	m_pCameraReflect->SetMirror(pCamera, A3DVECTOR3(0.0f, m_param.m_fWaterLevel + fHeightBias, 0.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pReflectViewport->Active();

	//A3DCOLORVALUE fogColor2 = m_pA3DDevice->GetA3DEngine()->GetSceneRenderConfig()->GetFogColor2();
#ifdef _ANGELICA_2_1
	A3DCOLORVALUE fogColor2 = m_pA3DDevice->GetA3DEngine()->GetSceneRenderConfig()->GetFogColor2();
#else
	A3DCOLORVALUE fogColor2 = A3DCOLORVALUE(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetFogParam()->crFog2);
#endif

	m_pReflectViewport->SetClearColor(fogColor2.ToRGBAColor());
	m_pReflectViewport->ClearDevice();

	if(m_param.m_bFullReflection)	//全反射情况下这里不渲染东西
		;
	else
	{
		if(pSky)					//不是全反射,则在这里绘制天空
		{
			pSky->SetCamera(m_pCameraReflect);
			pSky->Render();
			pSky->SetCamera(pCamera);
		}
	}

	// setup water clip plane
	// first clip plane in world space
	float fd = -m_param.m_fWaterLevel - fHeightBias; //-m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
	D3DXPLANE p(0.0f, 1.0f, 0.0f, fd);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	A3DMATRIX4 matVP = m_pCameraReflect->GetVPTM();
	m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);

	if(m_param.m_bFullReflection)	//全反射则在这里调用函数
	{
		if(pSky)
		{
			pSky->SetCamera(m_pCameraReflect);
			pSky->Render();
			pSky->SetCamera(pCamera);
		}
		
		if( pReflectCallBack )
		{
			(*pReflectCallBack)(m_pReflectViewport, pArg);
		}

		if (pTerrainOutline)
			pTerrainOutline->RenderForReflected(m_pReflectViewport);

		if (pTerrain)
		{
			pTerrain->SetExtRender(m_pTerrainRender);
			pTerrain->Render(m_pReflectViewport, false, false);
			pTerrain->SetExtRender(NULL);
		}
	}
	else							//不是全反射
	{
		if (pfnRenderOnSky)
			(*pfnRenderOnSky)(m_pReflectViewport, pArgSky);

		if(!m_param.m_bOnlyReflectSky)	//不是仅渲染天空, 则渲染地形等
		{
			if (pTerrainOutline)
				pTerrainOutline->RenderForReflected(m_pReflectViewport);

			if (pTerrain)
			{
				pTerrain->SetExtRender(m_pTerrainRender);
				pTerrain->Render(m_pReflectViewport, false, false);
				pTerrain->SetExtRender(NULL);
			}
			//	pTerrain->Render(m_pReflectViewport, false, false);
		}

	}

	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);

	pReflectionRT->WithdrawFromDevice();
	pCurrentViewport->Active();
}

void A3DTerrainWaterFFT::MakeDepthRT(A3DViewport* pViewport, A3DRenderTarget* pDepthRT)
{	
	if(!pDepthRT || !pViewport)
		return;

	if(!m_pA3DDevice->GetIntZRenderTarget())
	{
		pDepthRT->ApplyToDevice();
		m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
		pDepthRT->WithdrawFromDevice();
		return;
	}

	//先把当前状态保存下来.
	DWORD dwState;
	bool bOldAlphaBlendEnable;
	bool bOldAlphaTestEnable;
	bool bOldZTestEnable;
	bool bOldZWriteEnable;
	A3DCULLTYPE oldCullType;
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE);
	bOldAlphaBlendEnable = (dwState != 0 ? true : false);
	bOldAlphaTestEnable = m_pA3DDevice->GetAlphaTestEnable();
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZENABLE);
	bOldZTestEnable = (dwState != 0 ? true : false);
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZWRITEENABLE);
	bOldZWriteEnable = (dwState != 0 ? true : false);
	oldCullType = m_pA3DDevice->GetFaceCull();

	bool b = false;

	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 
		D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);

	m_pStream->Appear();
	m_pVertDecl->Appear();
	pDepthRT->ApplyToDevice();

	m_pHLSLLineraDepth->Setup2f("g_vPixelSize", 1.f / float(pDepthRT->GetWidth()), 1.f / float(pDepthRT->GetHeight()));

	m_pHLSLLineraDepth->Setup1f("zn", pViewport->GetCamera()->GetZFront());
	m_pHLSLLineraDepth->Setup1f("zf", pViewport->GetCamera()->GetZBack());

	m_pHLSLLineraDepth->Appear();


	if(m_pA3DDevice->GetIntZRenderTarget())
		m_pA3DDevice->GetIntZRenderTarget()->AppearAsTexture(0, true);

	b = m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);//RenderQuad();

	ASSERT(b);

	m_pHLSLLineraDepth->Disappear();

	pDepthRT->WithdrawFromDevice();

#ifdef _DEBUG_POST_
	// 	if(GetAsyncKeyState(VK_F7) & 0x8000)
	// 	{
	// 		D3DXSaveSurfaceToFileA("LinearZ.dds", D3DXIFF_DDS, m_pLinearDepthTarget->GetTargetSurface(), NULL, NULL);
	// 	}
#endif

	m_pA3DDevice->SetAlphaBlendEnable(bOldAlphaBlendEnable);
	m_pA3DDevice->SetAlphaTestEnable(bOldAlphaTestEnable);
	m_pA3DDevice->SetZTestEnable(bOldZTestEnable);
	m_pA3DDevice->SetZWriteEnable(bOldZWriteEnable);
	m_pA3DDevice->SetFaceCull(oldCullType);

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR );
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
}


void A3DTerrainWaterFFT::Release()
{
	if (m_pA3DDevice)
		m_pA3DDevice->RemoveUnmanagedDevObject(this);

	m_cryWaterFFT.Release();

	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLCaustic);
	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLWater);
	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLLineraDepth);
	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLUnderWater);
	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLDownSamp);
	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLGodray);
	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSLCopyRT);
	ReleaseA3DHLSL_Common(m_pA3DDevice, m_pHLSL3DScreenDepth);

	A3DRELEASE(m_pFFTTexture);

	A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();
	pTexMan->ReleaseTexture(m_pOceanWaveNormalTex);
	pTexMan->ReleaseTexture(m_pWaterFoamTex);
	pTexMan->ReleaseTexture(m_pWaterGlossTex);
	pTexMan->ReleaseTexture(m_pBlackTex);

	for(int i = 0; i < FFT_WATER_RAIN_TEXTURE_NUM; i++)
	{
		pTexMan->ReleaseTexture(m_pRainNormalTex[i]);
		pTexMan->ReleaseTexture(m_pRainXLNormalTex[i]);
	}

	for(int i = 0; i < FFT_WATER_CAUSTIC_TEXTURE_NUM; i++)
	{
		pTexMan->ReleaseTexture(m_pCausticTex[i]);
	}

	A3DRELEASE(m_pReflectViewport);		
	A3DRELEASE(m_pCameraReflect);		
	A3DRELEASE(m_pStream);
	A3DRELEASE(m_pSmallWaterStream);

	A3DRELEASE(m_pVertDecl);
	A3DRELEASE(m_pCubeTexture);
	A3DRELEASE(m_pSmallMap1x1);

	A3DRELEASE(m_pTerrainRender);
#ifdef _ANGELICA_2_1
#else
	if (m_pOccProxy)
	{
		A3DEngine* pA3DEngine = m_pA3DDevice->GetA3DEngine();
		if (pA3DEngine->GetA3DOcclusionMan())
			pA3DEngine->GetA3DOcclusionMan()->ReleaseQuery(m_pOccProxy);
		m_pOccProxy = NULL;
	}
#endif
}

bool A3DTerrainWaterFFT::ReloadShader()
{
	ReloadA3DHLSL_Common(m_pHLSLCaustic);
	ReloadA3DHLSL_Common(m_pHLSLWater);
	ReloadA3DHLSL_Common(m_pHLSLLineraDepth);
	ReloadA3DHLSL_Common(m_pHLSLUnderWater);
	ReloadA3DHLSL_Common(m_pHLSLDownSamp);
	ReloadA3DHLSL_Common(m_pHLSLGodray);

	return true;
}

void A3DTerrainWaterFFT::ResetParams()
{
	m_param.Reset();
}

void A3DTerrainWaterFFT::SetParam( const Param& param )
{
	m_param = param;
	if(!m_bSupportSM30)
	{
		m_param.m_bUseFFT = false;
		m_param.m_bCaustic = false;				
		m_param.m_nRain = 0;					
		m_param.m_bHighRefr = false;			
		m_param.m_bUnderGodRay = false;			
	}
}

const A3DTerrainWaterFFT::Param& A3DTerrainWaterFFT::GetParam() const
{
	return m_param;
}

A3DTerrainWaterFFT::A3DTerrainWaterFFT()
{
	m_nWaterTesselationAmount = 10;
	m_nWaterTesselationSwathWidth = 10;
	m_fAConstant = 1.f;
	m_bWireframe = false;

	m_pSmallWaterStream = NULL;

	m_pHLSLCaustic = NULL;
	m_pHLSLWater = NULL;
	m_pHLSLUnderWater = NULL;
	m_pHLSLLineraDepth = NULL;
	m_pHLSLDownSamp = NULL;
	m_pHLSLGodray = NULL;
	m_pHLSLCopyRT = NULL;

	m_pFFTTexture = NULL;

	m_pOceanWaveNormalTex = NULL;
	m_pWaterFoamTex = NULL;
	m_pWaterGlossTex = NULL;

	for(int i = 0; i < FFT_WATER_RAIN_TEXTURE_NUM; ++i)
	{
		m_pRainNormalTex[i] = NULL;
		m_pRainXLNormalTex[i] = NULL;
	}

	for(int i = 0; i < FFT_WATER_CAUSTIC_TEXTURE_NUM; i++)
	{
		m_pCausticTex[i] = NULL;
	}

	m_pA3DDevice = NULL;

	m_pReflectViewport = NULL;
	m_pCameraReflect = NULL;

	m_pStream = NULL;
	m_pVertDecl = NULL;

	m_fRainDuration = 1.f;

	m_pSmallMap1x1 = NULL;
	m_pCubeTexture = NULL;

	m_bSupportSM30 = false;
}

A3DTerrainWaterFFT::~A3DTerrainWaterFFT()
{
	Release();
}

bool A3DTerrainWaterFFT::ReloadTextuer(bool bIsNew)
{
	A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();
	pTexMan->ReleaseTexture(m_pOceanWaveNormalTex);
	pTexMan->ReleaseTexture(m_pWaterFoamTex);
	pTexMan->ReleaseTexture(m_pWaterGlossTex);
	for(int i = 0; i < FFT_WATER_RAIN_TEXTURE_NUM; i++)
	{
		pTexMan->ReleaseTexture(m_pRainNormalTex[i]);
		pTexMan->ReleaseTexture(m_pRainXLNormalTex[i]);
	}

	if(!bIsNew)
	{
		A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_Oceanwaves_ddn_PNG, &m_pOceanWaveNormalTex),
			"oceanwaves_ddn.PNG load failed!");

		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_WaterFoam_dds, &m_pWaterFoamTex),
			"WaterFoam.dds load failed!");

		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_WaterGloss_dds, &m_pWaterGlossTex),
			"water_gloss.dds load failed!");

		for(int i = 0; i < FFT_WATER_RAIN_TEXTURE_NUM; ++i)
		{
			AString strName;
			strName.Format(FN_Puddle_dds, i);
			TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(strName, &(m_pRainNormalTex[i])),
				"puddle.dds load failed!");
			strName.Format(FN_PuddleXL_dds, i);
			TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(strName, &(m_pRainXLNormalTex[i])),
				"puddle.dds load failed!");

		}
		m_fRainDuration = 1.f;
	}
	else
	{
		A3DTextureMan* pTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_NewOceanwaves_ddn_PNG, &m_pOceanWaveNormalTex),
			"oceanwaves_ddn.PNG load failed!");

		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_NewWaterFoam_dds, &m_pWaterFoamTex),
			"WaterFoam.dds load failed!");

		TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(FN_NewWaterGloss_dds, &m_pWaterGlossTex),
			"water_gloss.dds load failed!");

		for(int i = 0; i < FFT_WATER_RAIN_TEXTURE_NUM; ++i)
		{
			AString strName;
			strName.Format(FN_NewPuddle_dds, i);
			TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(strName, &(m_pRainNormalTex[i])),
				"puddle.dds load failed!");
			TRY_OR_RELEASE_RETURN_FALSE(pTexMan->LoadTextureFromFile(strName, &(m_pRainXLNormalTex[i])),
				"puddle.dds load failed!");
		}
		m_fRainDuration = 1.f;
	}

	m_pOceanWaveNormalTex->Reload(true);
	m_pWaterFoamTex->Reload(true);
	m_pWaterGlossTex->Reload(true);
	for(int i = 0; i < FFT_WATER_RAIN_TEXTURE_NUM; i++)
	{
		m_pRainNormalTex[i]->Reload(true);
		m_pRainXLNormalTex[i]->Reload(true);
	}
	return true;
}

A3DTerrainWaterFFT::ColorVertex::ColorVertex()
{

}

A3DTerrainWaterFFT::ColorVertex::ColorVertex( float x, float y, float z,float nx,float ny,float nz, A3DCOLOR color, float u, float v )
{
	m_p.x = x;	 
	m_p.y = y;  
	m_p.z = z; 

	m_n.x = nx;
	m_n.y = ny;
	m_n.z =nz;

	m_dwordColor = color;
	m_tu = u;
	m_tv = v;
}

void A3DTerrainWaterFFT::ColorVertex::Set( A3DVECTOR3 position,A3DVECTOR3 normal,A3DCOLOR color )
{
	m_p = position;
	m_n = normal;
	m_dwordColor = color;
}

void A3DTerrainWaterFFT::ColorVertex::Set( A3DVECTOR3 position,A3DVECTOR3 normal,A3DCOLOR color, D3DXVECTOR2 uv )
{
	m_p = position;
	m_n = normal;
	m_dwordColor = color;
	m_tu = uv.x;
	m_tv = uv.y;
}

void A3DTerrainWaterFFT::CopyRenderTarget(A3DRenderTarget* pSrc, A3DRenderTarget* pDst)
{
	//先把当前状态保存下来.
	DWORD dwState;
	bool bOldAlphaBlendEnable;
	bool bOldAlphaTestEnable;
	bool bOldZTestEnable;
	bool bOldZWriteEnable;
	A3DCULLTYPE oldCullType;
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE);
	bOldAlphaBlendEnable = (dwState != 0 ? true : false);
	bOldAlphaTestEnable = m_pA3DDevice->GetAlphaTestEnable();
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZENABLE);
	bOldZTestEnable = (dwState != 0 ? true : false);
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZWRITEENABLE);
	bOldZWriteEnable = (dwState != 0 ? true : false);
	oldCullType = m_pA3DDevice->GetFaceCull();

	bool b = false;

	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 
		D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_ALPHA);

	m_pStream->Appear();
	m_pVertDecl->Appear();
	pDst->ApplyToDevice();

	m_pHLSLCopyRT->Setup2f("g_vPixelSize", 1.f / float(pDst->GetWidth()), 1.f / float(pDst->GetHeight()));

	m_pHLSLCopyRT->Appear();

	pSrc->AppearAsTexture(0);

	b = m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	ASSERT(b);

	m_pHLSLCopyRT->Disappear();

	pDst->WithdrawFromDevice();

	m_pA3DDevice->SetAlphaBlendEnable(bOldAlphaBlendEnable);
	m_pA3DDevice->SetAlphaTestEnable(bOldAlphaTestEnable);
	m_pA3DDevice->SetZTestEnable(bOldZTestEnable);
	m_pA3DDevice->SetZWriteEnable(bOldZWriteEnable);
	m_pA3DDevice->SetFaceCull(oldCullType);

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR );
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
}

void A3DTerrainWaterFFT::Make3DScreenDepthRT( A3DRenderTarget* pScreenDepthRT, A3DCamera* pCamera)
{
	//先把当前状态保存下来.
	DWORD dwState;
	bool bOldAlphaBlendEnable;
	bool bOldAlphaTestEnable;
	bool bOldZTestEnable;
	bool bOldZWriteEnable;
	A3DCULLTYPE oldCullType;
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE);
	bOldAlphaBlendEnable = (dwState != 0 ? true : false);
	bOldAlphaTestEnable = m_pA3DDevice->GetAlphaTestEnable();
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZENABLE);
	bOldZTestEnable = (dwState != 0 ? true : false);
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZWRITEENABLE);
	bOldZWriteEnable = (dwState != 0 ? true : false);
	oldCullType = m_pA3DDevice->GetFaceCull();

	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);


	pScreenDepthRT->ApplyToDevice();
	m_pA3DDevice->Clear(D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	m_pVertDecl->Appear();

	float oldZFront = pCamera->GetZFront();
	float oldZBack = pCamera->GetZBack();

	pCamera->SetZFrontAndBack(0.001f,20.f,true);
	m_pHLSL3DScreenDepth->SetConstantMatrix("matProj", pCamera->GetProjectionTM());
	pCamera->SetZFrontAndBack(oldZFront,oldZBack,true);

	m_pHLSL3DScreenDepth->Setup1f("fFarPlane", pCamera->GetZBack());
	m_pHLSL3DScreenDepth->Appear();


	m_aVertices[0].Set(A3DVECTOR3(0.f, 10000.f, -10000.f), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(0, 1));
	m_aVertices[1].Set(A3DVECTOR3(0.f, -10000.f, -10000.f), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(1, 1));
	m_aVertices[2].Set(A3DVECTOR3(0.f,  -10000.f, 10000.f), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(0, 0));
	m_aVertices[3].Set(A3DVECTOR3(0.f,  10000.f, 10000.f), A3DVECTOR3(0, 0, 0), 0xffffffff, D3DXVECTOR2(1, 0));

	m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLESTRIP, 2, m_aVertices, sizeof(ColorVertex));

	m_pHLSL3DScreenDepth->Disappear();
	pScreenDepthRT->WithdrawFromDevice();

	m_pA3DDevice->SetAlphaBlendEnable(bOldAlphaBlendEnable);
	m_pA3DDevice->SetAlphaTestEnable(bOldAlphaTestEnable);
	m_pA3DDevice->SetZTestEnable(bOldZTestEnable);
	m_pA3DDevice->SetZWriteEnable(bOldZWriteEnable);
	m_pA3DDevice->SetFaceCull(oldCullType);

}

bool A3DTerrainWaterFFT::LoadCubeReflectionTexture( AFile* pCubeMapFile )
{
	A3DRELEASE(m_pCubeTexture);
	m_pCubeTexture = new A3DCubeTexture;
	m_pCubeTexture->Init(m_pA3DDevice);
	return m_pCubeTexture->Load(pCubeMapFile);
}

unsigned int A3DTerrainWaterFFT::ComputeSmallWaterLOD(float fCameraHeight)
{
	float height = fabsf(fCameraHeight - m_param.m_fWaterLevel);
	if(height < 10.f)
		return 0;
	else if(height < 20.f)
		return 1;
	else
		return 2;

}

void A3DTerrainWaterFFT::CreateOccQuery()
{
#ifdef _ANGELICA_2_1
#else
	A3DEngine* pA3DEngine = m_pA3DDevice->GetA3DEngine();

	A3DOcclusionMan* pOccMan = pA3DEngine->GetA3DOcclusionMan();

	if (pOccMan)
	{
		if (m_pOccProxy)
			pOccMan->ReleaseQuery(m_pOccProxy);

		m_pOccProxy = pOccMan->CreateQuery();
	}
#endif
}

bool A3DTerrainWaterFFT::CreateTerrainRender( A3DTerrain2* pTerrain )
{
	//	Release current render
	A3DRELEASE(m_pTerrainRender);

	if (!pTerrain)
		return true;

	//	Create terrain render
	m_pTerrainRender = new A3DTerrain2Render;
	if (!m_pTerrainRender || !m_pTerrainRender->Init(pTerrain))
	{
		g_A3DErrLog.Log("A3DTerrainWater2NormalRender, failed to create terrain render");
		return false;
	}

	if (!m_pTerrainRender->OnTerrainLoaded())
	{
		g_A3DErrLog.Log("A3DTerrainWater2NormalRender, failed to create call OnTerrainLoaded");
		return false;
	}

	return true;
}

bool A3DTerrainWaterFFT::AfterDeviceReset()
{
	if (m_pTerrainRender)
		m_pTerrainRender->SetRebuildStreamFlag(true);

	return true;
}
