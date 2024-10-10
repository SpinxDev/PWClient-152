///*
//* FILE: A3DTerrainWater2NormalRender.coo
//*
//* DESCRIPTION: Class representing for water on the terrain
//*
//* CREATED BY: Liyi, 2012, 3, 15
//*
//* HISTORY:
//*
//* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
//*/

#include "A3DTerrainWater2NormalRender.h"
#include "A3DCamera.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DFuncs.h"
#include "A3DPI.h"
#include "A3DRenderTarget.h"
#include "A3DRenderTargetMan.h"
#include "A3DViewport.h"
#include "A3DVertexShader.h"
#include "A3DTerrainWater.h"
#include "A3DTextureMan.h"
#include "A3DEnvironment.h"
#include "A3DTerrain2Render.h"
#include "A3DTerrain2.h"
#include "A3DConfig.h"
#include "A3DStream.h"
#include "A3DSky.h"

#include "A3DSimpleQuad.h"
#include "A3DTerrainWater2.h"
#include "A3DWaterArea2.h"
#include "A3DHLSL.h"


A3DTerrainWater2NormalRender::A3DTerrainWater2NormalRender() :
m_pA3DDevice(NULL),
m_pHLSLMan(NULL),
m_pWater(NULL),
m_bRenderWave(false),
m_bPerPixelLight(true),
m_bReflectEnabled(false),
m_bCubeReflect(false),
m_pReflectColorRT(NULL),
m_pRefractColorRT(NULL),
m_pDepthRT(NULL),
m_pReflectCamera(NULL),
m_pViewport(NULL),
m_nNumNormalTex(29),
m_idNormalTex(0),
m_nTextureTime(0),
m_fTimeCnt(0.0f),
m_pAboveWaterHLSL(NULL),
m_pUnderWaterHLSL(NULL),
m_pCurWaterHLSL(NULL),
m_pRefractHLSL(NULL),
m_pQuadMesh(NULL),
m_pRefractUnderWaterHLSL(NULL),
m_pRefractUnderWaterRealTimeHLSL(NULL),
m_pTerrainRender(NULL),
m_pCurTerrain(NULL)
{
	m_matTexTrans = Scaling(2.0f, 2.0f, 2.0f);
	m_matTexTrans._31 = 0.05f;
	m_matTexTrans._32 = 0.0f;

	memset(&m_ppNormalTex, 0, sizeof (A3DTexture*) * MAX_WATER_TEX);
}

A3DTerrainWater2NormalRender::~A3DTerrainWater2NormalRender()
{
	Release();
}

bool A3DTerrainWater2NormalRender::Init(A3DTerrainWater2* pWater, 
										bool bPerPixelLight, 
										bool bReflectEnable,
										bool bCubeReflect)
{
	if (pWater == NULL)
		return false;

	m_pWater = pWater;
	m_pA3DDevice = pWater->GetA3DDevice();
	m_pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();

	//	Check device supports
	if (bPerPixelLight && bReflectEnable)
	{
		if (!m_pA3DDevice->TestPixelShaderVersion(3, 0))
			bReflectEnable = false;		//	Disable reflection
	}

	m_bReflectEnabled = bReflectEnable;
	m_bPerPixelLight = bPerPixelLight;
	m_bCubeReflect = bCubeReflect;

	//	Create viewport
	const D3DPRESENT_PARAMETERS& d3dpp = m_pA3DDevice->GetD3DPP();
	m_pViewport = new A3DViewport;
	A3DVIEWPORTPARAM param;
	param.X = 0;
	param.Y = 0;
	param.Height = (int)d3dpp.BackBufferHeight;
	param.Width = (int)d3dpp.BackBufferWidth;
	param.MinZ = 0.0f;
	param.MaxZ = 1.0f;
	if (!m_pViewport->Init(m_pA3DDevice, &param, true, true, 0xFF008FFD))
	{
		g_A3DErrLog.Log("A3DTerrainWaterLPPRender::Init, Failed to create viewport!");
		return false;
	}

	//	Load shaders
	if (!LoadShaders())
		return false;

	//	Load textures
	if (!LoadTextures())
		return false;

	if (bReflectEnable)
	{
		//	Create reflect camera
		m_pReflectCamera = new A3DCamera();
		if (!m_pReflectCamera || !m_pReflectCamera->Init(m_pA3DDevice, DEG2RAD(56.0f), 0.2f, 1900.0f))
		{
			g_A3DErrLog.Log("A3DTerrainWaterLPPRender::Init, Failed to create reflect camera!"); 
			return false;
		}
	}

	//	Create a simple quad mesh
	m_pQuadMesh = new A3DSimpleQuad;
	if (!m_pQuadMesh || !m_pQuadMesh->Init(m_pA3DDevice->GetA3DEngine()))
		return false;

    if (m_pA3DDevice)
        m_pA3DDevice->AddUnmanagedDevObject(this);

    return true;
}

//	Create terrain render
bool A3DTerrainWater2NormalRender::CreateTerrainRender(A3DTerrain2* pTerrain)
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

void A3DTerrainWater2NormalRender::Release()
{
    if (m_pA3DDevice)
        m_pA3DDevice->RemoveUnmanagedDevObject(this);

	A3DRELEASE(m_pQuadMesh);
	A3DRELEASE(m_pReflectCamera);
	A3DRELEASE(m_pViewport);
    A3DRELEASE(m_pTerrainRender);
	ReleaseShaders();
	ReleaseTextures();

    m_pA3DDevice = NULL;
}

bool A3DTerrainWater2NormalRender::LoadShaders()
{

	m_pAboveWaterHLSL = m_pHLSLMan->LoadShader( "shaders\\2.2\\HLSL\\Water\\NormalWater.hlsl", 
												"vs_AboveWater", 
												"shaders\\2.2\\HLSL\\Water\\NormalWater.hlsl",
												"ps_AboveWater",
												"_FOG_", 
												0);
	if( !m_pAboveWaterHLSL)
	{
		g_A3DErrLog.Log("A3DTerrainWater2NormalRender::LoadShaders, failed to load NormalWater.hlsl shader!");
		return false;
	}

	m_pUnderWaterHLSL = m_pHLSLMan->LoadShader("shaders\\2.2\\HLSL\\Water\\NormalWater.hlsl", 
												"vs_AboveWater", 
												"shaders\\2.2\\HLSL\\Water\\NormalWater.hlsl",
												"ps_UnderWater",
												"_FOG_", 
												0);
	if( !m_pUnderWaterHLSL)
	{
		g_A3DErrLog.Log("A3DTerrainWater2NormalRender::LoadShaders, failed to load NormalWater.hlsl shader!");
		return false;
	}

	m_pRefractHLSL = m_pHLSLMan->LoadShader("shaders\\2.2\\HLSL\\Water\\FrameRefract.hlsl", 
											"vs_FrameRefract", 
											"shaders\\2.2\\HLSL\\Water\\FrameRefract.hlsl",
											"ps_FrameRefract",
											NULL, 
											0);
	if(!m_pRefractHLSL)
	{
		g_A3DErrLog.Log("A3DTerrainWater2NormalRender::LoadShaders, failed to load FrameRefract.hlsl shader!");
		return false;
	}

	m_pRefractUnderWaterHLSL = m_pHLSLMan->LoadShader("shaders\\2.2\\HLSL\\Water\\FrameRefract.hlsl", 
														"vs_FrameRefract", 
														"shaders\\2.2\\HLSL\\Water\\FrameRefract.hlsl",
														"ps_FrameRefract_UnderWater",
														NULL, 
														0);
	if(!m_pRefractUnderWaterHLSL)
	{
		g_A3DErrLog.Log("A3DTerrainWater2NormalRender::LoadShaders, failed to load FrameRefract.hlsl shader!");
		return false;
	}

	m_pRefractUnderWaterRealTimeHLSL = m_pHLSLMan->LoadShader( "shaders\\2.2\\HLSL\\Water\\FrameRefract.hlsl", 
																"vs_FrameRefract", 
																"shaders\\2.2\\HLSL\\Water\\FrameRefract.hlsl",
																"ps_FrameRefract_UnderWater_RealTime",
																NULL, 
																0);
	if(!m_pRefractUnderWaterRealTimeHLSL)
	{
		g_A3DErrLog.Log("A3DTerrainWater2NormalRender::LoadShaders, failed to load FrameRefract.hlsl shader!");
		return false;
	}

	A3DCCDBinder* pBinder = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCCDBinder();
	m_pAboveWaterHLSL->BindCommConstData(pBinder);
	m_pUnderWaterHLSL->BindCommConstData(pBinder);

	return true;
}

void A3DTerrainWater2NormalRender::ReleaseShaders()
{
	if( m_pAboveWaterHLSL)
	{
		m_pHLSLMan->ReleaseShader(m_pAboveWaterHLSL);
		m_pAboveWaterHLSL = NULL;
	}
	if( m_pUnderWaterHLSL)
	{
		m_pHLSLMan->ReleaseShader(m_pUnderWaterHLSL);
		m_pUnderWaterHLSL = NULL;
	}

	if( m_pRefractHLSL)
	{
		m_pHLSLMan->ReleaseShader(m_pRefractHLSL);
		m_pRefractHLSL = NULL;
	}

	if( m_pRefractUnderWaterHLSL)
	{
		m_pHLSLMan->ReleaseShader(m_pRefractUnderWaterHLSL);
		m_pRefractUnderWaterHLSL = NULL;
	}

	if( m_pRefractUnderWaterRealTimeHLSL)
	{	
		m_pHLSLMan->ReleaseShader(m_pRefractUnderWaterRealTimeHLSL);
		m_pRefractUnderWaterRealTimeHLSL = NULL;
	}
}

//FIXME!! 把normalmap改成_Y_X DXT5格式
bool A3DTerrainWater2NormalRender::LoadTextures()
{
	A3DTextureMan* pA3DTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

	if (m_bPerPixelLight)
	{
		m_nNumNormalTex  = 2;

		if (!pA3DTexMan->LoadTextureFromFile("shaders\\textures\\water\\WaterWave0.dds", &m_ppNormalTex[0]))
		//if (!pA3DTexMan->LoadTextureFromFile("shaders\\textures\\water\\wave.dds", &m_ppNormalTex[0]))
		{
			g_A3DErrLog.Log("A3DTerrainWaterLPPRender::LoadTextures(), fail to load normal map 0");
			return false;
		}

		if (!pA3DTexMan->LoadTextureFromFile("shaders\\textures\\water\\WaterWave1.dds", &m_ppNormalTex[1]))
		{
			g_A3DErrLog.Log("A3DTerrainWaterLPPRender::LoadTextures(), fail to load normal map 1");
			return false;
		}
	}
	else	//	Vertex lighting version
	{
		m_nNumNormalTex = 29;

		char szFile[MAX_PATH];
		for (int i=0; i < m_nNumNormalTex; i++)
		{
			sprintf(szFile, "shaders\\textures\\water\\lpp\\water_normal%02d.dds", i);
			if (!pA3DTexMan->LoadTextureFromFile(szFile, &m_ppNormalTex[i]))
			{
				g_A3DErrLog.Log("A3DTerrainWaterLPPRender::LoadTextures(), fail to load texture [%s]", szFile);
				return false;
			}
		}
	}


	return true;
}

void A3DTerrainWater2NormalRender::ReleaseTextures()
{
	for (int i=0; i < m_nNumNormalTex; i++)
	{
		if (m_ppNormalTex[i])
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_ppNormalTex[i]);
			m_ppNormalTex[i] = NULL;
		}
	}
}

bool A3DTerrainWater2NormalRender::Update(int nDeltaTime)
{
	m_fTimeCnt += nDeltaTime * 0.001f;

	if (!m_bPerPixelLight)
	{
		//	Update tex coord
		float vRad = DEG2RAD(nDeltaTime / 40.0f);
		A3DVECTOR3 vecOffset = A3DVECTOR3(m_matTexTrans._31, m_matTexTrans._32, 0.0f);
		vecOffset = vecOffset * RotateZ(vRad);
		m_matTexTrans._31 = vecOffset.x;
		m_matTexTrans._32 = vecOffset.y;

		//	Update tex
		m_nTextureTime += nDeltaTime;
		while (m_nTextureTime >= 33)
		{
			m_idNormalTex ++;
			m_idNormalTex %= m_nNumNormalTex;
			m_nTextureTime -= 33;
		}
	}

	return true;
}

bool A3DTerrainWater2NormalRender::Render(const RenderParam* prp)
{
	int nNumAreaArray = m_pWater->GetWaterAreasArrayNum();
	if (!nNumAreaArray)
		return true;

	if (!prp->pDepthRT || !prp->pFrameRT)
	{
		//ASSERT(prp->pDepthRT && prp->pFrameRT);
		return false;
	}
	

	A3DCameraBase* pCamera = prp->pCurViewport->GetCamera();
	const A3DVECTOR3& vEyePos = pCamera->GetPos();

	//	Build water dimple when per-pixel lighting and reflection is enabled
	//FIXME!!
	/*
	bool bRenderDimple = false;
	if (m_bPerPixelLight && m_bReflectEnabled && m_pWater->IsWaterDimpleEnabled())
	{
		A3DWaterDimple* pWaterDimple = m_pWater->GetWaterDimpleMan();
		if (pWaterDimple->BuildDimpleTexture(vEyePos))
		{
			float fAreaRadius = pWaterDimple->GetAreaRadius();
			m_pMtlInst->paramDimpleTex->SetExtDXTexture(pWaterDimple->GetDimpleTexture());
			m_pMtlInst->paramDimpleArea->Set4f(vEyePos.x - fAreaRadius, vEyePos.z + fAreaRadius,
											fAreaRadius * 2.0f, fAreaRadius * 2.0f);
		}
	}
	*/

	//	Rent render targets
	A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
	m_pRefractColorRT = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_A8R8G8B8);

	if (!m_pRefractColorRT)
	{
		//ASSERT(m_pRefractColorRT);
		pRTMan->ReturnRenderTarget(m_pRefractColorRT);
		m_pRefractColorRT = NULL;
		return false;
	}

	//	Adjust viewport size if necessary
	A3DVIEWPORTPARAM* pVP = m_pViewport->GetParam();
	if (int(pVP->Width) != m_pRefractColorRT->GetWidth() ||
		int(pVP->Height) != m_pRefractColorRT->GetHeight())
	{
		pVP->Width = m_pRefractColorRT->GetWidth();
		pVP->Height = m_pRefractColorRT->GetHeight();
		m_pViewport->SetParam(pVP);
	}

	//	Do render
	if (m_pWater->IsUnderWater(vEyePos))
	{
		for (int i = 0; i < nNumAreaArray; i++)
		{
            if (m_pWater->CullWaterAreasByViewport(prp->pCurViewport, i))
            {
			    //RenderRefractRTUnderWater(prp, i);
			    RenderRefractRTUnderWaterRealTime(prp, i);
			    RenderWaterMesh(prp, i, true, false);
            }
		}
	}
	else
	{
		int iRefIndex = -1;
		if (m_bReflectEnabled)
		{
			//	Find the water area that will do reflection
			for (int i=0; i < nNumAreaArray; i++)
			{
				APtrArray<A3DWaterArea2*>* pArray = m_pWater->GetWaterAreasArray(i);
				for (int j=0; j < pArray->GetSize(); j++)
				{
					A3DWaterArea2* pArea = (*pArray)[j];
					if (!pArea->GetMinorWaterFlag())
					{
						//	Non-minor water area do reflection
						iRefIndex = i;
						break;
					}
				}

				if (iRefIndex >= 0)
					break;
			}
		}

		for (int i=0; i < nNumAreaArray; i++)
		{
            if (m_pWater->CullWaterAreasByViewport(prp->pCurViewport, i))
            {
			    bool bHasReflect = iRefIndex == i;

			    if (bHasReflect && !m_bCubeReflect)
                {
				    RenderReflectRT(prp, i);
                }
    				
			    RenderRefractRT(prp, i);
    			
			    RenderWaterMesh(prp, i, false, bHasReflect);
            }
		}
	}

	//	Return render targets
	if (m_pReflectColorRT)
	{
		pRTMan->ReturnRenderTarget(m_pReflectColorRT);
		m_pReflectColorRT = NULL;
	}

	if (m_pDepthRT)
	{
		pRTMan->ReturnRenderTarget(m_pDepthRT);
		m_pDepthRT = NULL;
	}

	pRTMan->ReturnRenderTarget(m_pRefractColorRT);
	m_pRefractColorRT = NULL;
	m_pDepthRT = NULL;

	return true;
}

bool A3DTerrainWater2NormalRender::RenderReflectRT(const RenderParam* prp, int nWaterAreaArrayIdx)
{
	A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();

	//	Render reflection render target
	if (!m_pReflectColorRT)
		m_pReflectColorRT = pRTMan->RentRenderTargetColor(0, 0, A3DFMT_A8R8G8B8);

	if (!m_pDepthRT)
		m_pDepthRT = pRTMan->RentRenderTargetDepth(0, 0, A3DFMT_UNKNOWN);

	if (!m_pReflectColorRT || !m_pDepthRT)
		return false;

	//	Set render target
	m_pReflectColorRT->ApplyToDevice();
	m_pDepthRT->ApplyToDevice();

	//	Adjust camera parameters
	A3DCameraBase* pCurCamera = prp->pCurViewport->GetCamera();
	if (pCurCamera->IsPerspective())
	{
		A3DCamera* pTempCamera = (A3DCamera*)pCurCamera;
		float fFOV = pTempCamera->GetFOV();
		float fRatio = pTempCamera->GetRatio();

		if (fFOV != m_pReflectCamera->GetFOV() || fRatio != m_pReflectCamera->GetRatio())
			m_pReflectCamera->SetProjectionParam(fFOV, -1.0f, -1.0f, fRatio);
	}

	//	Set mirror info
	A3DVECTOR3 vOrg = A3DVECTOR3( 0.0f, m_pWater->GetWaterAreaHeight(nWaterAreaArrayIdx), 0.0f);
	m_pReflectCamera->SetMirror(prp->pCurViewport->GetCamera(), vOrg, A3D::g_vAxisY);
	A3DMATRIX4 matPost = pCurCamera->GetPostProjectTM();
	m_pReflectCamera->SetPostProjectTM(matPost);

	m_pViewport->SetCamera(m_pReflectCamera);
	m_pViewport->Active();

	//m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()
	//DWORD dwFogCol = m_pWater->GetFogWater().dwColor; //FIXME!!
	//m_pViewport->SetClearColor(0xff000000|dwFogCol);
	m_pViewport->SetClearColor(A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f).ToRGBAColor());
	m_pViewport->ClearDevice();

	//	Inverse face cull
	A3DCULLTYPE cullType = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_CW);


	//	Disable occlusion cull
	//FIXME!!
	/*
	bool bOccCull = A3DConfig::GetInstance()->RT_GetOccluderEnableFlag();
	A3DConfig::GetInstance()->RT_SetOccluderEnableFlag(false);
	*/
	

	//	Set clip plane
	float fClipHeight = m_pWater->GetWaterAreaHeight(nWaterAreaArrayIdx);
	D3DXPLANE clipPlane(0.0f, 1.0f,0.0f, -fClipHeight + 0.2f );
	A3DMATRIX4 matVP = m_pReflectCamera->GetVPTM();
	m_pA3DDevice->SetClipPlaneProgrammable(0, &clipPlane, &matVP);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	//m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->SetIsUnderWater(false);

	//Render sky
	if(prp->pSky)
	{
		prp->pSky->SetCamera(m_pReflectCamera);
		prp->pSky->Render();
	}

    if (prp->pTerrain)
    {
        //	Create terrain water render if necessary
        if (m_pCurTerrain != prp->pTerrain)
        {
            if (!CreateTerrainRender(prp->pTerrain))
                return false;

            m_pCurTerrain = prp->pTerrain;
        }

        //p.d = -m_pWater->GetWaterAreaHeight(nAreaArrayIdx) + 0.0f;
        //m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);

        //	Render Terrain
        {
            m_pCurTerrain->SetExtRender(m_pTerrainRender);
            m_pCurTerrain->Render(m_pViewport);
            m_pCurTerrain->SetExtRender(NULL);
        }
    }


	////	Render Terrain
	//if (prp->pTerrain)
	//{
	//	prp->pTerrain->Render(m_pViewport);
	//}

	D3DXPLANE clipPlane2(0.0f, 1.0f,0.0f, -fClipHeight);
	matVP = m_pReflectCamera->GetVPTM();
	m_pA3DDevice->SetClipPlaneProgrammable(0, &clipPlane2, &matVP);

	//	Render reflect call back
	if (prp->pfnReflectCallBack)
		(*prp->pfnReflectCallBack)(m_pViewport, prp->pArg);

	
	//m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->SetFogParam(&fogParams);
	
	//	restore render target
	m_pDepthRT->WithdrawFromDevice();
	m_pReflectColorRT->WithdrawFromDevice();

	//FIXME!!
	//A3DConfig::GetInstance()->RT_SetOccluderEnableFlag(bOccCull);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
	m_pA3DDevice->SetFaceCull(cullType);

	m_pViewport->SetCamera(NULL);
	
	//FIXME!!
	//if (GetKeyState(VK_F6) & 0x8000)
	//	D3DXSaveSurfaceToFileA("reflect.bmp", D3DXIFF_BMP, m_pReflectColorRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);

	return true;
}

bool A3DTerrainWater2NormalRender::RenderRefractRT(const RenderParam* prp, int nWaterAreaArrayIdx)
{
	
	//ASSERT(m_pRefractColorRT);

	m_pRefractColorRT->ApplyToDevice();

	float fWaterHeight = m_pWater->GetWaterAreaHeight(nWaterAreaArrayIdx);

	//	Find a proper water area as reference
	A3DWaterArea2* pRefArea = m_pWater->GetCurWaterArea();
	if (!pRefArea || fabs(pRefArea->GetWaterHeight() - fWaterHeight) >= 0.001f)
	{
		//	Get first water area of specified index
		APtrArray<A3DWaterArea2*>* pAreas = m_pWater->GetWaterAreasArray(nWaterAreaArrayIdx);
		if (!pAreas)
		{
			//ASSERT(pAreas);
			return false;
		}

		pRefArea = (*pAreas)[0];
	}

	DWORD dwFogCol = pRefArea->GetCurWaterColor();

	m_pViewport->SetCamera(prp->pCurViewport->GetCamera());
	m_pViewport->Active();
	
	m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);//	Don't clear z buffer

	A3DCameraBase* pCamera = prp->pCurViewport->GetCamera();
	A3DVECTOR3 vCameraPos = pCamera->GetPos();
	A3DVECTOR3 aCorners[8];
	pCamera->CalcFrustumCorners(aCorners, false, pCamera->GetZFront(), pCamera->GetZBack());
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_LT, 0, aCorners[5] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_RT, 0, aCorners[6] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_RB, 0, aCorners[7] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_LB, 0, aCorners[4] - vCameraPos);
	m_pQuadMesh->ApplyVertexData();

	//Set shader variables
	m_pRefractHLSL->SetValue3f("g_vCameraPos", &vCameraPos);

	float fWidth = (float)prp->pFrameRT->GetWidth();
	float fHeight = (float)prp->pFrameRT->GetHeight();
	m_pRefractHLSL->SetValue2f("g_vFrameSize", fWidth,fHeight);
	//	Water height
	m_pRefractHLSL->SetValue1f("g_fWaterHeight", fWaterHeight);
	float fEdgeHeight = m_pWater->GetEdgeHeight();
	m_pRefractHLSL->SetValue1f("g_fEdgeHeiScale", 1.0f/fEdgeHeight);

	float fFar = pCamera->GetZBack();
	float fNear = pCamera->GetZFront();
	m_pRefractHLSL->SetValue1f("g_fProjRatioX", fFar / (fFar - fNear));
	m_pRefractHLSL->SetValue1f("g_fProjRatioY", fNear / (fNear - fFar));

	//set	Textures
	m_pRefractHLSL->SetTexture("g_FrameSampler", prp->pFrameRT);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	//	Set render state
	bool alpha_blend = m_pA3DDevice->GetAlphaBlendEnable();
	bool z_write = m_pA3DDevice->GetZWriteEnable();
	bool z_test = m_pA3DDevice->GetZTestEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);

	m_pRefractHLSL->Appear();
	//FIXME!!
	prp->pDepthRT->AppearAsTexture(1, true);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(1, prp->pDepthRT->GetDepthTexture());
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	m_pQuadMesh->RawDraw();

	m_pRefractHLSL->Disappear();

	/*
	//	Then, render other alpha objects that are not in frame buffer
	//	Set clip plane in world space
	D3DXPLANE clipPlane(0.0f, -1.0f, 0.0f, fWaterHeight);
	A3DMATRIX4 matVP = pCamera->GetVPTM();
	m_pA3DDevice->SetClipPlaneProgrammable(0, &clipPlane, &matVP);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	//	-------------------------------------------------------------
	//	Let user render alpha objects under water to refract RT
	//	-------------------------------------------------------------

	//	Prevent alpha channel from writing, because we stored edge height delta in it in
	//	previous draw call.
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|
		D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);

	m_pA3DDevice->SetZTestEnable(true);

	if (prp->pfnRefractCallBack)
		(*prp->pfnRefractCallBack)(m_pViewport, prp->pArg);
	
	//	Restore device states
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|
		D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
	*/
	m_pA3DDevice->SetAlphaBlendEnable(alpha_blend);
	m_pA3DDevice->SetZWriteEnable(z_write);
	m_pA3DDevice->SetZTestEnable(z_test);

	//	restore render target
	m_pRefractColorRT->WithdrawFromDevice();

	//	Restore viewport
	prp->pCurViewport->Active();
	m_pViewport->SetCamera(NULL);

	//FIXME!!
	//if (GetKeyState(VK_F7) & 0x8000) 
	//	D3DXSaveSurfaceToFileA("refract.tga", D3DXIFF_TGA, m_pRefractColorRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
	
	return true;
	
}

bool A3DTerrainWater2NormalRender::RenderRefractRTUnderWater(const RenderParam* prp, int nWaterAreaArrayIdx)
{
	
	//ASSERT(m_pRefractColorRT);

	//	Set render target
	m_pRefractColorRT->ApplyToDevice();

	float fWaterHeight = m_pWater->GetWaterAreaHeight(nWaterAreaArrayIdx);

	//	Set viewport and camera
	m_pViewport->SetCamera(prp->pCurViewport->GetCamera());
	m_pViewport->Active();
	m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);//	Don't clear z buffer


	//Set QuadMesh verts data	
	A3DCameraBase* pCamera = prp->pCurViewport->GetCamera();
	A3DVECTOR3 vCameraPos = pCamera->GetPos();
	A3DVECTOR3 aCorners[8];
	pCamera->CalcFrustumCorners(aCorners, false, pCamera->GetZFront(), pCamera->GetZBack());
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_LT, 0, aCorners[5] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_RT, 0, aCorners[6] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_RB, 0, aCorners[7] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_LB, 0, aCorners[4] - vCameraPos);
	m_pQuadMesh->ApplyVertexData();

	//Set shader variables
	m_pRefractUnderWaterHLSL->SetValue3f("g_vCameraPos", &vCameraPos);

	float fWidth = (float)prp->pFrameRT->GetWidth();
	float fHeight = (float)prp->pFrameRT->GetHeight();
	m_pRefractUnderWaterHLSL->SetValue2f("g_vFrameSize", fWidth,fHeight);
	//	Water height
	m_pRefractUnderWaterHLSL->SetValue1f("g_fWaterHeight", fWaterHeight);
	float fEdgeHeight = m_pWater->GetEdgeHeight();
	m_pRefractUnderWaterHLSL->SetValue1f("g_fEdgeHeiScale", 1.0f/fEdgeHeight);

	float fFar = pCamera->GetZBack();
	float fNear = pCamera->GetZFront();
	m_pRefractUnderWaterHLSL->SetValue1f("g_fProjRatioX", fFar / (fFar - fNear));
	m_pRefractUnderWaterHLSL->SetValue1f("g_fProjRatioY", fNear / (fNear - fFar));

	//set	Textures
	m_pRefractUnderWaterHLSL->SetTexture("g_FrameSampler", prp->pFrameRT);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	//	Set render state
	bool alpha_blend = m_pA3DDevice->GetAlphaBlendEnable();
	bool z_write = m_pA3DDevice->GetZWriteEnable();
	bool z_test = m_pA3DDevice->GetZTestEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);

	m_pRefractUnderWaterHLSL->Appear();
	//FIXME!!
	prp->pDepthRT->AppearAsTexture(1, true);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(1, prp->pDepthRT->GetDepthTexture());
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	
	m_pQuadMesh->RawDraw();
	m_pRefractUnderWaterHLSL->Disappear();

	//	-------------------------------------------------------------
	//	Let user render alpha objects above water to refract RT
	//	-------------------------------------------------------------

	//	Prevent alpha channel from writing, because we stored edge height delta in it in
	//	previous draw call.
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|
		D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
	m_pA3DDevice->SetZTestEnable(true);

	//	Set clip plane
	D3DXPLANE clipPlane(0.0f, 1.0f, 0.0f, -fWaterHeight);
	A3DMATRIX4 matVP = pCamera->GetVPTM();
	m_pA3DDevice->SetClipPlaneProgrammable(0, &clipPlane, &matVP);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	if (prp->pfnRefractCallBack)
		(*prp->pfnRefractCallBack)(m_pViewport, prp->pArg);
	
	//	Restore render target
	m_pRefractColorRT->WithdrawFromDevice();

	//	Restore render state
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|
		D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
	m_pA3DDevice->SetAlphaBlendEnable(alpha_blend);
	m_pA3DDevice->SetZWriteEnable(z_write);
	m_pA3DDevice->SetZTestEnable(z_test);

	//	Restore viewport
	prp->pCurViewport->Active();
	m_pViewport->SetCamera(NULL);

	//FIXME!!
	//if (GetKeyState(VK_F7) & 0x8000)
	//	D3DXSaveSurfaceToFileA("refract_UW.dds", D3DXIFF_DDS, m_pRefractColorRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);


	return true;
}

bool A3DTerrainWater2NormalRender::RenderRefractRTUnderWaterRealTime(const RenderParam* prp, int nWaterAreaArrayIdx)
{
	//ASSERT(m_pRefractColorRT);

	//	Set render target
	m_pRefractColorRT->ApplyToDevice();

	float fWaterHeight = m_pWater->GetWaterAreaHeight(nWaterAreaArrayIdx);

	//	Set viewport and camera
	m_pViewport->SetCamera(prp->pCurViewport->GetCamera());
	m_pViewport->Active();
	m_pA3DDevice->Clear(D3DCLEAR_TARGET, A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 0.0f).ToRGBAColor(), 1.0f, 0);//	Don't clear z buffer

	//Set QuadMesh verts data	
	A3DCameraBase* pCamera = prp->pCurViewport->GetCamera();
	A3DVECTOR3 vCameraPos = pCamera->GetPos();
	A3DVECTOR3 aCorners[8];
	pCamera->CalcFrustumCorners(aCorners, false, pCamera->GetZFront(), pCamera->GetZBack());
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_LT, 0, aCorners[5] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_RT, 0, aCorners[6] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_RB, 0, aCorners[7] - vCameraPos);
	m_pQuadMesh->SetVertexData(A3DSimpleQuad::VERT_LB, 0, aCorners[4] - vCameraPos);
	m_pQuadMesh->ApplyVertexData();

	//Set shader variables
	m_pRefractUnderWaterRealTimeHLSL->SetValue3f("g_vCameraPos", &vCameraPos);

	float fWidth = (float)prp->pFrameRT->GetWidth();
	float fHeight = (float)prp->pFrameRT->GetHeight();
	m_pRefractUnderWaterRealTimeHLSL->SetValue2f("g_vFrameSize", fWidth,fHeight);
	//	Water height
	m_pRefractUnderWaterRealTimeHLSL->SetValue1f("g_fWaterHeight", fWaterHeight);
	float fEdgeHeight = m_pWater->GetEdgeHeight();
	m_pRefractUnderWaterRealTimeHLSL->SetValue1f("g_fEdgeHeiScale", 1.0f/fEdgeHeight);

	float fFar = pCamera->GetZBack();
	float fNear = pCamera->GetZFront();
	m_pRefractUnderWaterRealTimeHLSL->SetValue1f("g_fProjRatioX", fFar / (fFar - fNear));
	m_pRefractUnderWaterRealTimeHLSL->SetValue1f("g_fProjRatioY", fNear / (fNear - fFar));

	//	Set render state
	bool alpha_blend = m_pA3DDevice->GetAlphaBlendEnable();
	bool z_write = m_pA3DDevice->GetZWriteEnable();
	bool z_test = m_pA3DDevice->GetZTestEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);

	m_pRefractUnderWaterRealTimeHLSL->Appear();
	//FIXME!!
	prp->pDepthRT->AppearAsTexture(1, true);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(1, prp->pDepthRT->GetDepthTexture());
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);


	m_pQuadMesh->RawDraw();
	m_pRefractUnderWaterRealTimeHLSL->Disappear();

	//	-------------------------------------------------------------
	//	Let user render alpha objects above water to refract RT
	//	-------------------------------------------------------------
	A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
	if (!m_pDepthRT)
		m_pDepthRT = pRTMan->RentRenderTargetDepth(0, 0, A3DFMT_UNKNOWN);
	m_pDepthRT->ApplyToDevice();
	m_pA3DDevice->Clear(D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

	//	Prevent alpha channel from writing, because we stored edge height delta in it in
	//	previous draw call.
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|
		D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
	m_pA3DDevice->SetZTestEnable(true);

	m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->SetIsUnderWater(false);

	//	Set clip plane
	D3DXPLANE clipPlane(0.0f, 1.0f, 0.0f, -fWaterHeight);
	A3DMATRIX4 matVP = pCamera->GetVPTM();
	m_pA3DDevice->SetClipPlaneProgrammable(0, &clipPlane, &matVP);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	//Render sky
	if(prp->pSky)
	{
		prp->pSky->SetCamera(pCamera);
		prp->pSky->Render();
	}

	//	Render Terrain
	if (prp->pTerrain)
	{
		prp->pTerrain->Render(m_pViewport);
	}

	if (prp->pfnRefractCallBack)
		(*prp->pfnRefractCallBack)(m_pViewport, prp->pArg);

	m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->SetIsUnderWater(true);

	//	Restore render target
	m_pDepthRT->WithdrawFromDevice();
	m_pRefractColorRT->WithdrawFromDevice();

	//	Restore render state
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|
		D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
	m_pA3DDevice->SetAlphaBlendEnable(alpha_blend);
	m_pA3DDevice->SetZWriteEnable(z_write);
	m_pA3DDevice->SetZTestEnable(z_test);

	//	Restore viewport
	prp->pCurViewport->Active();
	m_pViewport->SetCamera(NULL);

	//FIXME!!
	//if (GetKeyState(VK_F7) & 0x8000)
	//	D3DXSaveSurfaceToFileA("refract_UW_realtime.dds", D3DXIFF_DDS, m_pRefractColorRT->GetTargetSurface()->m_pD3DSurface, NULL, NULL);


	return true;
}


//	Render water mesh
bool A3DTerrainWater2NormalRender::RenderWaterMesh(const RenderParam* prp, int nWaterAreaArrayIdx, bool bUnderWater, bool bHasReflect)
{
	
	//	Only pixel lighting has bloom effect
	if (prp->pSimuBloomRT && m_bPerPixelLight)
		prp->pSimuBloomRT->ApplyToDevice(1);

	//	Active Viewport
	prp->pCurViewport->Active();


	//	Active Vertex Decl
	m_pWater->GetWaterAreaVertDecl()->Appear();

	//set render state
	bool zwrite_enable = m_pA3DDevice->GetZWriteEnable();
	bool alpha_blend = m_pA3DDevice->GetAlphaBlendEnable();
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	//choice shader
	m_pCurWaterHLSL =  bUnderWater? m_pUnderWaterHLSL: m_pAboveWaterHLSL;
	
	//set shader parameters
	A3DEnvironment* pEnv = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment();
	A3DCameraBase* pCamera = prp->pCurViewport->GetCamera();
	A3DMATRIX4 matWVP = A3DMATRIX4(A3DMATRIX4::IDENTITY) * pCamera->GetVPTM();
	m_pCurWaterHLSL->SetConstantMatrix("g_matWVP", matWVP);

	//FIXME!!
	const A3DEnvironment::DIRLIGHT* dirLight = pEnv->GetDirLightParam();
	A3DVECTOR3 vPos = pCamera->GetPos() - dirLight->Direction * 10000.0f;
	A3DVECTOR4 vMainLightPos = A3DVECTOR4(vPos.x, vPos.y, vPos.z, 1.0f);
	m_pCurWaterHLSL->SetValue4f("g_MainLightPos", &vMainLightPos);

	//set main light color
	A3DCOLORVALUE clrMainLight = dirLight->Diffuse;
	A3DVECTOR4 vMainLightColor(clrMainLight.r, clrMainLight.g, clrMainLight.b, clrMainLight.a);
	m_pCurWaterHLSL->SetValue4f("g_MainLightColor", &vMainLightColor);
	
	A3DVECTOR3 vCameraPos = pCamera->GetPos();
	m_pCurWaterHLSL->SetValue3f("g_vCameraPos", &vCameraPos);

	//	Translate and reflect matrix
	A3DMATRIX4 matScale(A3DMATRIX4::IDENTITY);
	matScale._11 = 0.5f;
	matScale._22 = -0.5f;
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;
	A3DMATRIX4 matReflect = pCamera->GetVPTM() * matScale;
	m_pCurWaterHLSL->SetConstantMatrix("g_matReflect", matReflect);

	//FIXME!!
	A3DVIEWPORTPARAM* pvp = prp->pCurViewport->GetParam();
	m_pCurWaterHLSL->SetValue2f("g_ScreenDim", (float)pvp->Width, (float)pvp->Height);
	m_pCurWaterHLSL->SetValue1f("g_bStaticMesh", m_pWater->IsStaticMesh());

	//set cube reflect
	m_pCurWaterHLSL->SetValue1b("g_bCubeReflect",m_bCubeReflect);

	//set textures
	m_pCurWaterHLSL->SetTexture("g_NormalSampler", m_ppNormalTex[0]);//sampler 0
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pCurWaterHLSL->SetTexture("g_NormalSampler1", m_ppNormalTex[1]);//sampler 1
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);

	
	m_pCurWaterHLSL->SetTexture("g_ReflectSampler", bHasReflect? m_pReflectColorRT : NULL);//sampler 2
	m_pA3DDevice->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	
	m_pCurWaterHLSL->SetTexture("g_RefractSampler", m_pRefractColorRT);//sampler 3
	m_pA3DDevice->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	if( m_bCubeReflect)
	{
		m_pCurWaterHLSL->SetTexture("g_CubeReflectSampler", (A3DObject*)prp->pCubeReflect);//sampler 4
		m_pA3DDevice->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pA3DDevice->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pA3DDevice->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		m_pA3DDevice->SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_pA3DDevice->SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		m_pA3DDevice->SetSamplerState(4, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	}

	//Set shader
	m_pCurWaterHLSL->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());

	//	Do render
	if (m_pWater->IsStaticMesh())
		m_pWater->DrawWaterAreaArrayStatic(prp->pCurViewport, A3DTerrainWater2::DRAW_ALLWATER, nWaterAreaArrayIdx, m_pCurWaterHLSL, m_bPerPixelLight);
	else
		m_pWater->DrawWaterAreaArray(prp->pCurViewport, A3DTerrainWater2::DRAW_ALLWATER, nWaterAreaArrayIdx);

	//unset shader
	m_pCurWaterHLSL->Disappear();

	//restore render state
	m_pA3DDevice->SetZWriteEnable(zwrite_enable);
	m_pA3DDevice->SetAlphaBlendEnable(alpha_blend);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	
	if (prp->pSimuBloomRT && m_bPerPixelLight)
		prp->pSimuBloomRT->WithdrawFromDevice();

	return true;
}

//	This function give a chance to water render to upload parameter for specified water area before render it
void A3DTerrainWater2NormalRender::UploadParamsForWaterArea(A3DWaterArea2* pWaterArea)
{
	//FIXME!!
	//	Water color
	A3DCOLORVALUE colWater(pWaterArea->GetCurWaterColor());
	A3DVECTOR4 vColWater(colWater.r, colWater.g, colWater.b, colWater.a);
	m_pCurWaterHLSL->SetValue4f("g_vWaterColor",&vColWater);

	//	Misc
	float fEdgeScale = 1.0f / pWaterArea->GetEdgeHeight();
	A3DVECTOR4 vWaterMisc(pWaterArea->GetWaterHeight(), fEdgeScale, 0.0f, pWaterArea->GetCurDarkFactor());
	m_pCurWaterHLSL->SetValue4f("g_vMisc", &vWaterMisc);
	
	if (m_bPerPixelLight)
	{
		//	UV offset for normal maps
		float fUSpeed, fVSpeed;
		pWaterArea->GetNMScrollSpeed(0, fUSpeed, fVSpeed);
		float u0 = fUSpeed * m_fTimeCnt;
		float v0 = fVSpeed * m_fTimeCnt;
		pWaterArea->GetNMScrollSpeed(1, fUSpeed, fVSpeed);
		float u1 = fUSpeed * m_fTimeCnt;
		float v1 = fVSpeed * m_fTimeCnt;

		A3DVECTOR4 vWaveNormalMapOffset(u0, v0, u1, v1);
		m_pCurWaterHLSL->SetValue4f("g_vWaveNormalMapOffset", &vWaveNormalMapOffset);
		//	UV scale for normal maps
		m_pCurWaterHLSL->SetValue1f("g_fNMapUVScale", pWaterArea->GetNMScale());
	}
	m_pCurWaterHLSL->Commit();
}

//	Before device reset
bool A3DTerrainWater2NormalRender::BeforeDeviceReset()
{
	return true;
}

//	After device reset
bool A3DTerrainWater2NormalRender::AfterDeviceReset()
{
	if (m_pTerrainRender)
		m_pTerrainRender->SetRebuildStreamFlag(true);

	return true;
}

