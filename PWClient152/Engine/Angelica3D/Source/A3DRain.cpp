/*
 * FILE: A3DRain.cpp
 *
 * DESCRIPTION: Rain system
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DRain.h"
#include "A3DWorld.h"
#include "A3DTexture.h"
#include "A3DDevice.h"
#include "A3DCDS.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DCameraBase.h"
#include "AFile.h"

A3DRain::A3DRain()
{
	m_dwClassID			= A3D_CID_RAIN;
	m_pA3DDevice		= NULL;
	m_pHostCamera		= NULL;
	m_pSplash			= NULL;
	m_vEmitterCenterX	= 0.0f;
	m_vEmitterCenterZ	= 0.0f;
	m_vEmitterWidth		= 150.0f;
	m_vEmitterLength	= 150.0f;
	m_vEmitterHeight	= 60.0f;

	m_pRainCurtainTexture = NULL;
}

A3DRain::~A3DRain()
{
}

bool A3DRain::EmitParticles()
{
	// First follow my host camera;
	A3DVECTOR3 vecCamPos(0.0f);
	A3DVECTOR3 vecCamDirH, vecCamLeftH;
	A3DVECTOR3 vecEmitterCenter;

	if( m_pHostCamera )
	{
		vecCamPos = m_pHostCamera->GetPos();	
		vecCamDirH = m_pHostCamera->GetDir();
		vecCamLeftH = m_pHostCamera->GetLeftH();

		UpdateParentTM(a3d_Translate(vecCamPos.x, vecCamPos.y, vecCamPos.z));
		vecEmitterCenter = vecCamPos + m_pHostCamera->GetDirH() * GetEmitterWidth() * 1.0f;
		m_EmitterAbsoluteVelocity = 0.0f;
	}

	int		n;
	STANDARD_PARTICLE StandardParticle;
	
	int nToNew;
	nToNew = GetNumberToNew();
	
	if( m_vInheritInfluence != 0.0f && m_nTicks <= 1 )
		return true;

	for(n=0; n<nToNew; n++)
	{
		COMMON_PARTICLE * pNewCommonParticle;

		ZeroMemory(&StandardParticle, sizeof(STANDARD_PARTICLE));
		pNewCommonParticle = &StandardParticle.common;

		FLOAT vX, vZ;
		A3DVECTOR3 pos;
		vX = GetNextParticleXOffset() + vecEmitterCenter.x;
		vZ = GetNextParticleZOffset() + vecEmitterCenter.z;
		
		pos.x = vX;
		pos.y = vecCamPos.y + m_vEmitterHeight;
		pos.z = vZ;
		
		A3DVECTOR3 vecDir;
		m_noise.GetValue(m_nTicks * 1.0f + n * 1.0f / nToNew, vecDir.m, 3);
		vecDir.x = (vecDir.x * 0.5f + 0.02f) * 0.5f;
		vecDir.y = -1.0f;
		vecDir.z = (vecDir.z * 0.5f - 0.02f) * 0.5f;

		// We let each particle has its own position in world space;
		pNewCommonParticle->vecPos	 = pos;
		pNewCommonParticle->vecDir   = vecDir;
		pNewCommonParticle->vecSpeed = GetNextParticleSpeedValue() * pNewCommonParticle->vecDir;

		if( m_vInheritInfluence != 0.0f )
		{
			pNewCommonParticle->vecSpeed = pNewCommonParticle->vecSpeed + 
				GetNextParticleInheritInfluence() * 0.01f * m_vInheritMultiplier * m_EmitterAbsoluteVelocity;
		}

		// Fill these common parameters;
		FillCommonParticle(pNewCommonParticle);

		AddParticle((LPBYTE)&StandardParticle);
	}
	return true;
}

bool A3DRain::MakeDead(LPVOID pParticle)
{
	if( !m_pSplash )
		return true;

	A3DVECTOR3 vecPos;
	STANDARD_PARTICLE	* pStdParticle = (STANDARD_PARTICLE *) pParticle;

	// Only visible rain drop will cause splash
	if( m_pA3DDevice->GetA3DEngine()->GetActiveViewport()->Transform(pStdParticle->common.vecPos, vecPos) )
		return true;

	// Some of them will cause splash
	float vDis = (float)(fabs(pStdParticle->common.vecPos.x - m_EmitterAbsoluteTM._41) + fabs(pStdParticle->common.vecPos.z - m_EmitterAbsoluteTM._43));
	if( m_vEmitterWidth - vDis + a_Random(0.0f, m_vEmitterWidth) < m_vEmitterWidth )
		return true;

	A3DCDS* pA3DCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
	if (pA3DCDS)
	{
		RAYTRACERT rayTrace;

		A3DVECTOR3 vecStart = pStdParticle->common.vecPos - pStdParticle->common.vecSpeed * 15.0f;
		A3DVECTOR3 vecEnd = pStdParticle->common.vecPos + pStdParticle->common.vecSpeed * 10.0f;
		
		bool bHit = pA3DCDS->RayTrace(vecStart, vecEnd - vecStart, 1.0f, &rayTrace, TRACEOBJ_RAIN, 0);
		if (bHit)
		{
			m_pSplash->AddSplash(rayTrace.vPoint, rayTrace.vNormal, true);
		}
	}

	return true;
}

bool A3DRain::Save(AFile * pFileToSave)
{
	//Now save common particle system data;
	if( !A3DParticleSystem::Save(pFileToSave) )
		return false;

	//Save super spray specification data first;
	if( pFileToSave->IsBinary() )
	{
		//Save Binary Data here;
	}
	else
	{
		//Save Text Data here;
		char			szLineBuffer[AFILE_LINEMAXLEN];

		sprintf(szLineBuffer, "EmitterCenterX: %f", m_vEmitterCenterX);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "EmitterCenterZ: %f", m_vEmitterCenterZ);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "EmitterWidth: %f", m_vEmitterWidth);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "EmitterLength: %f", m_vEmitterLength);
		pFileToSave->WriteLine(szLineBuffer);
	}

	return true;
}

bool A3DRain::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	//Now load common particle system data;
	if( !A3DParticleSystem::Load(pA3DDevice, pFileToLoad) )
		return false;

	//Load super spray specification data first;
	if( pFileToLoad->IsBinary() )
	{
		//Load Binary Data here;
	}
	else
	{
		//Load Text Data here;
		char			szLineBuffer[AFILE_LINEMAXLEN];
		DWORD			dwReadLen;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "EmitterCenterX: %f", &m_vEmitterCenterX);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "EmitterCenterZ: %f", &m_vEmitterCenterZ);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "EmitterWidth: %f", &m_vEmitterWidth);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "EmitterLength: %f", &m_vEmitterLength);
	}

	return true;	
}

bool A3DRain::Init(A3DDevice * pA3DDevice, FLOAT vAmplitude)
{
	m_noise.Init(256, vAmplitude, 64, 0.3f, 2, 0x12345);
	if( !A3DParticleSystem::Init(pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DRain::Init(), call parent init fail!");
		return false;
	}

	SetStandardParticleType(A3DPARTICLE_STANDARD_PARTICLE_TETRA);

	m_pSplash = NULL;
	return true;
}

bool A3DRain::Release()
{
	m_noise.Release();
	if( m_pRainCurtainTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pRainCurtainTexture);
		m_pRainCurtainTexture = NULL;
	}
	return A3DParticleSystem::Release();
}

bool A3DRain::CreateRain()
{
	return CreateStandard();
}

bool A3DRain::StartRain()
{
	return Start();
}

bool A3DRain::ShowRainCurtain(A3DViewport * pCurrentViewport)
{
	static float du = 0.0f, dv = 0.0f;
	if( !m_pRainCurtainTexture )
		return true;

	// First update the rain curtain vertex buffer;
	A3DVIEWPORTPARAM * pParam = pCurrentViewport->GetParam();
	A3DVECTOR3 vecLT = A3DVECTOR3(float(pParam->X), float(pParam->Y), 0.0f);
	A3DVECTOR3 vecRB = A3DVECTOR3(float(pParam->X + pParam->Width), float(pParam->Y + pParam->Height), 0.0f);
	
	m_pRainCurtainVerts[0].x	= vecLT.x;
	m_pRainCurtainVerts[0].y	= vecLT.y;
	m_pRainCurtainVerts[0].z	= 0.0f;
	m_pRainCurtainVerts[0].rhw	= 1.0f;
	m_pRainCurtainVerts[0].diffuse = 0xffffffff;
	m_pRainCurtainVerts[0].specular = 0xff000000;
	m_pRainCurtainVerts[0].tu	= 0.0f;
	m_pRainCurtainVerts[0].tv	= 0.0f;

	m_pRainCurtainVerts[1].x	= vecRB.x;
	m_pRainCurtainVerts[1].y	= vecLT.y;
	m_pRainCurtainVerts[1].z	= 0.0f;
	m_pRainCurtainVerts[1].rhw	= 1.0f;
	m_pRainCurtainVerts[1].diffuse = 0xffffffff;
	m_pRainCurtainVerts[1].specular = 0xff000000;
	m_pRainCurtainVerts[1].tu	= 1.0f;
	m_pRainCurtainVerts[1].tv	= 0.0f;

	m_pRainCurtainVerts[2].x	= vecLT.x;
	m_pRainCurtainVerts[2].y	= vecRB.y;
	m_pRainCurtainVerts[2].z	= 0.0f;
	m_pRainCurtainVerts[2].rhw	= 1.0f;
	m_pRainCurtainVerts[2].diffuse = 0xffffffff;
	m_pRainCurtainVerts[2].specular = 0xff000000;
	m_pRainCurtainVerts[2].tu	= 0.0f;
	m_pRainCurtainVerts[2].tv	= 1.0f;

	m_pRainCurtainVerts[3]		= m_pRainCurtainVerts[2];
	m_pRainCurtainVerts[4]		= m_pRainCurtainVerts[1];

	m_pRainCurtainVerts[5].x	= vecRB.x;
	m_pRainCurtainVerts[5].y	= vecRB.y;
	m_pRainCurtainVerts[5].z	= 0.0f;
	m_pRainCurtainVerts[5].rhw	= 1.0f;
	m_pRainCurtainVerts[5].diffuse = 0xffffffff;
	m_pRainCurtainVerts[5].specular = 0xff000000;
	m_pRainCurtainVerts[5].tu	= 1.0f;
	m_pRainCurtainVerts[5].tv	= 1.0f;
	
	A3DCameraBase* pCamera = pCurrentViewport->GetCamera();

	// Find a particle's speed;
	if( m_nNumParticles == 0 )
		return true;

	A3DVECTOR3 vecDir = ((STANDARD_PARTICLE *)m_pParticleBuffer)[0].common.vecSpeed + pCamera->GetPos() + pCamera->GetDir() * m_vEmitterWidth / 2.0f;
	pCurrentViewport->Transform(vecDir, vecDir);
	du += (vecDir.x - pParam->Width / 2.0f) / pParam->Width;
	dv += (vecDir.y - pParam->Height / 2.0f) / pParam->Height;
	for(int i=0; i<6; i++)
	{
		m_pRainCurtainVerts[i].tu += pCamera->GetDeg() / 90.0f / 4.0f;
		m_pRainCurtainVerts[i].tv -= pCamera->GetPitch() / 90.0f / 3.0f;

		m_pRainCurtainVerts[i].tu += du / 4.0f;
		m_pRainCurtainVerts[i].tv -= dv / 3.0f;

		m_pRainCurtainVerts[i].tu *= 4.0f;
		m_pRainCurtainVerts[i].tv *= 3.0f;
	}

	// Then render it;
	m_pRainCurtainTexture->Appear(0);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetFVF(A3DFVF_A3DTLVERTEX);

	m_pA3DDevice->DrawPrimitiveUP(A3DPT_TRIANGLELIST, 2, m_pRainCurtainVerts, sizeof(A3DTLVERTEX));

	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	return true;
}

bool A3DRain::RenderStandardParticles(A3DViewport * pCurrentViewport)
{
	if( m_pSplash )
		m_pSplash->RenderStandardParticles(pCurrentViewport);

	// Then show a rain curtain on the screen;
	//ShowRainCurtain(pCurrentViewport);

	return A3DParticleSystem::RenderStandardParticles(pCurrentViewport);
}

bool A3DRain::UpdateStandardParticles()
{
	STANDARD_PARTICLE * pParticle = (STANDARD_PARTICLE *) m_pParticleBuffer;
	for(int i=0; i<m_nNumParticles; ++i)
	{
		if( pParticle->common.nLife <= 0 )
		{
			// This will be dead;
			MakeDead(pParticle);
			DeleteParticle(i);
			--i;
		}
		else
		{
			UpdateCommonParticle(&pParticle->common);
			++pParticle;
		}
	}

	return true;
}

bool A3DRain::TickEmitting()
{
	if( m_pSplash )
	{
		m_pSplash->TickSplash();
		if( rand() % 100 < 40 )
			m_pSplash->AddSplash(m_pHostCamera->GetPos() + m_pHostCamera->GetDirH() * 5.0f, A3DVECTOR3(0.0f, 1.0f, 0.0f), true);
	}

	return A3DParticleSystem::TickEmitting();
}

bool A3DRain::SetRainCurtainTexture(char * szTextureMap)
{
	if( m_pRainCurtainTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pRainCurtainTexture);
		m_pRainCurtainTexture = NULL;
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(szTextureMap, m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), &m_pRainCurtainTexture) )
	{
		g_A3DErrLog.Log("A3DRain::SetRainCurtainTexture(), fail!");
		return false;
	}

	return true;
}

// ******************* A3DRainSplash *******************
A3DRainSplash::A3DRainSplash()
{
	m_nSpawnNumber = 4;
	m_vSplashScatter = 2.0f;
}

A3DRainSplash::~A3DRainSplash()
{

}

bool A3DRainSplash::Init(A3DDevice * pA3DDevice)
{
	if( !A3DSuperSpray::Init(pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DRain::Init(), call parent init fail!");
		return false;
	}

	SetStandardParticleType(A3DPARTICLE_STANDARD_PARTICLE_FACING);
	return true;
}

bool A3DRainSplash::Release()
{
	return A3DSuperSpray::Release();
}

bool A3DRainSplash::Save(AFile * pFileToSave)
{
	return true;
}

bool A3DRainSplash::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	return true;
}

bool A3DRainSplash::AddSplash(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecDir, bool bOnTerrain)
{
	int					n;
	STANDARD_PARTICLE	StandardParticle;
	A3DTerrain *		pTerrain = NULL;
	float				vSplashScatter = m_vSplashScatter;

/*	if (bOnTerrain)
	{
		A3DCDS* pA3DCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
		if (pA3DCDS)
			pTerrain = pA3DCDS->GetA3DTerrain();

		if( !pTerrain )
			return true;
	}
	else
*/		vSplashScatter = 1.0f;

	for(n=0; n<m_nSpawnNumber; n++)
	{
		COMMON_PARTICLE * pNewCommonParticle;

		ZeroMemory(&StandardParticle, sizeof(STANDARD_PARTICLE));
		pNewCommonParticle = &StandardParticle.common;

		FLOAT vOffAxis, vOffPlane;
		vOffAxis = GetNextParticleOffAxis();
		vOffPlane = GetNextParticleOffPlane();
		//Construct the rotation matrix here;
		A3DMATRIX4 matEmitDir = a3d_RotateX(DEG2RAD(vOffAxis - GetOffAxis())) * a3d_RotateZ(DEG2RAD(vOffPlane)) * a3d_RotateX(DEG2RAD(GetOffAxis()));
		
		// We let each particle has its own position in world space;
		pNewCommonParticle->vecPos	 = vecPos;
		pNewCommonParticle->vecDir   = A3DParticleSystem::m_EmittingAxis * matEmitDir * m_EmitterAbsoluteTM - m_EmitterAbsoluteTM.GetRow(3);
		pNewCommonParticle->vecSpeed = GetNextParticleSpeedValue() * pNewCommonParticle->vecDir;
		
		// Fill these common parameters;
		FillCommonParticle(pNewCommonParticle);

		pNewCommonParticle->vecPos.x += pNewCommonParticle->vecDir.x * vSplashScatter;
		pNewCommonParticle->vecPos.z += pNewCommonParticle->vecDir.z * vSplashScatter;

	//	if( pTerrain )
	//		pNewCommonParticle->vecPos.y = pTerrain->GetHeight(pNewCommonParticle->vecPos);
		
		AddParticle((LPBYTE)&StandardParticle);
	}
	return true;
}

bool A3DRainSplash::EmitParticles()
{
	return true;
}

bool A3DRainSplash::MakeDead(LPVOID pParticle)
{
	return true;
}

bool A3DRainSplash::TickSplash()
{
	return UpdateParticles();
}