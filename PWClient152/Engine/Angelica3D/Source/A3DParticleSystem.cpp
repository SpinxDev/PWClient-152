/*
 * FILE: A3DParticleSystem.cpp
 *
 * DESCRIPTION: Base class of particle system for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DParticleSystem.h"
#include "A3DPI.h"
#include "A3DModel.h"
#include "A3DMathUtility.h"
#include "A3DFrame.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DCamera.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DGraphicsFX.h"
#include "A3DConfig.h"
#include "A3DModelMan.h"
#include "A3DWorld.h"
#include "A3DGFXMan.h"
#include "A3DCDS.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "AFile.h"

A3DVECTOR3 A3DParticleSystem::m_EmittingAxis = A3DVECTOR3(0.0f, 0.0f, 1.0f);

A3DParticleSystem::A3DParticleSystem()
{
	m_pA3DDevice	= NULL;
	m_dwClassID		= A3D_CID_PARTICLESYSTEM;

	m_pParentFrame  = NULL;
	m_bEmitting		= false; 
	m_bExpired		= true;

	m_vGravity		= 0.0f;
	m_vScale		= 1.0f;
	m_vSpeedScale	= 1.0f;

	m_EmitterAbsoluteTM = IdentityMatrix();

	// For standard paritcle system;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer	= NULL;
	m_nVertCount	= 0;
	m_nIndexCount	= 0;
	m_pA3DTexture	= NULL;
	m_pA3DStream	= NULL;
	m_szTextureMap[0] = '\0';

	m_pParticleBuffer = NULL;
	m_nMaxParticles = 0;
	m_nNumParticles = 0;

	// For Object Fragment particle system;
	m_nTextureNum	= 0;
	ZeroMemory(m_pnVertCount, sizeof(int) * A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE);
	ZeroMemory(m_pnIndexCount, sizeof(int) * A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE);
	ZeroMemory(m_ppVertexBuffers, sizeof(A3DLVERTEX *) * A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE);
	ZeroMemory(m_ppIndexBuffers, sizeof(WORD *) * A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE);
	ZeroMemory(m_ppA3DTextures, sizeof(A3DTexture *) * A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE);

	// For instanced geometry particle system;
	m_nInstancedModelNum = 0;
	ZeroMemory(m_szInstancedModelName, MAX_PATH * A3DPARTICLESYSTEM_INSTANCED_MAXMODEL);
	ZeroMemory(m_pInstancedModels, A3DPARTICLESYSTEM_INSTANCED_MAXMODEL);

	m_SrcBlend = A3DBLEND_SRCALPHA;
	m_DestBlend = A3DBLEND_INVSRCALPHA;
}

A3DParticleSystem::~A3DParticleSystem()
{
}

bool A3DParticleSystem::Init(A3DDevice * pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;

	m_nMaxVisibleParticles = A3DPARTICLESYSTEM_MAXPARTICLES_RENDER;
	m_ParticleType = A3DPARTICLE_TYPE_UNKNOWN;

	// Set all members to its default values;
	DefaultValues();
	return true;
}

bool A3DParticleSystem::CreateStandard()
{
	if( !m_pA3DDevice )	return false;

	m_ParticleType = A3DPARTICLE_STANDARD_PARTICLES;
	m_nParticleSize = sizeof(STANDARD_PARTICLE);

	if( g_pA3DConfig->GetRunEnv() != A3DRUNENV_GFXEDITOR )
	{
		if( m_bUseRate )
		{
			int nLongestLife = GetLongestParticleLife();

			if( m_nEmitEnd != -1 && m_nEmitEnd - m_nEmitStart + 1 < nLongestLife )
				nLongestLife = m_nEmitEnd - m_nEmitStart + 1;
			if( m_nDisplayUntil != -1 && m_nDisplayUntil < nLongestLife )
				nLongestLife = m_nDisplayUntil;
			
			m_nMaxParticles = m_nUseRate * (nLongestLife + 1);
		}
		else
			m_nMaxParticles = m_nUseTotal; // The particle are generated at one frame;
	}
	else
		m_nMaxParticles = A3DPARTICLESYSTEM_MAXPARTICLES_TOTAL; // 1024
	
	m_pParticleBuffer = (BYTE *) malloc(m_nParticleSize * m_nMaxParticles);
	if( NULL == m_pParticleBuffer )
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateStandard(), Not enough memory!");
		return false;
	}

	// For particle system may create very small particles and large particles as well,
	// so its texture should not be quite small, so mip map is needed here;
	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(m_szTextureMap, m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), &m_pA3DTexture) )
		return false;

	switch(m_StandardParticleType)
	{
	case A3DPARTICLE_STANDARD_PARTICLE_FACING:
	case A3DPARTICLE_STANDARD_PARTICLE_CONSTANT:
	case A3DPARTICLE_STANDARD_PARTICLE_TETRA:
		m_nMaxVertCount = 4 * m_nMaxParticles;
		m_nMaxIndexCount = 6 * m_nMaxParticles;
		break;
	default:
		g_A3DErrLog.Log("A3DParticleSystem::CreateStandard Unsupport particle type specified!");
		return false;
	}

	m_pVertexBuffer = (A3DTLVERTEX *) malloc(sizeof(A3DTLVERTEX) * m_nMaxVertCount);
	if( NULL == m_pVertexBuffer )
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateStandard Not enough memory!");
		return false;
	}
	m_pIndexBuffer = (WORD *) malloc(sizeof(WORD) * m_nMaxIndexCount);
	if( NULL == m_pIndexBuffer )
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateStandard Not enough memory!");
		return false;
	}

	m_pA3DStream = new A3DStream();
	if( NULL == m_pA3DStream )
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateStandard Not enough memroy!");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_TLVERTEX, m_nMaxVertCount, m_nMaxIndexCount, 
				A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC))
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateStandard m_pA3DStream Init Fail!");
		return false;
	}

	m_nVertCount = m_nIndexCount = 0;

	int			i;
	switch(m_StandardParticleType)
	{
	case A3DPARTICLE_STANDARD_PARTICLE_FACING:
	case A3DPARTICLE_STANDARD_PARTICLE_CONSTANT:
	case A3DPARTICLE_STANDARD_PARTICLE_TETRA:
		// For the index buffer will be constant, so we will initialzie it here;
		for(i=0; i<m_nMaxParticles; i++)
		{
			// 0*-------*1
			//	|	  /	|
			//	|	/	|
			//	| /		|
			// 2*-------*3
			m_pIndexBuffer[i * 6 + 0] = i * 4 + 0;
			m_pIndexBuffer[i * 6 + 1] = i * 4 + 1;
			m_pIndexBuffer[i * 6 + 2] = i * 4 + 2;

			m_pIndexBuffer[i * 6 + 3] = i * 4 + 2;
			m_pIndexBuffer[i * 6 + 4] = i * 4 + 1;
			m_pIndexBuffer[i * 6 + 5] = i * 4 + 3;
		}
		m_pA3DStream->SetIndices(m_pIndexBuffer, m_nMaxIndexCount);
		break;
	}
	return true;
}

bool A3DParticleSystem::CreateMeta()
{
	m_ParticleType = A3DPARTICLE_META_PARTICLES;
	g_A3DErrLog.Log("A3DParticleSystem::CreateMeta This method is not implemented yet!");
	return false;
}

bool A3DParticleSystem::CreateObjectFragment(int nTextureNum, A3DTexture ** ppTextures)
{
	if( !m_pA3DDevice )	return false;

	if( nTextureNum > A3DPARTICLESYSTEM_OBJECTFRAGMENT_MAXTEXTURE )
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateObjectFragment Too many textures used!");
		return false;
	}

	m_ParticleType = A3DPARTICLE_OBJECT_FRAGMENTS;
	m_nParticleSize = sizeof(OBJECT_FRAGMENT_PARTICLE);

	m_nMaxParticles = A3DPARTICLESYSTEM_MAXPARTICLES;
	
	m_pParticleBuffer = (BYTE *) malloc(m_nParticleSize * m_nMaxParticles);
	if( NULL == m_pParticleBuffer )
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateObjectFragment(), Not enough memory!");
		return false;
	}

	// Set max vert count assuming that each particle is a tetra;
	m_nMaxVertCount = 4 * m_nMaxParticles;
	m_nMaxIndexCount = 12 * m_nMaxParticles;

	m_nTextureNum = nTextureNum;
	for(int i=0; i<m_nTextureNum; i++)
	{
		// Record the texture pointer;
		m_ppA3DTextures[i] = ppTextures[i];

		// We need allocate the buffer here;
		m_ppVertexBuffers[i] = (A3DLVERTEX *) malloc(sizeof(A3DLVERTEX) * m_nMaxVertCount);
		if( NULL == m_ppVertexBuffers[i] )
		{
			g_A3DErrLog.Log("A3DParticleSystem::CreateObjectFragment Not enough memory!");
			return false;
		}
		m_ppIndexBuffers[i] = (WORD *) malloc(sizeof(WORD) * m_nMaxIndexCount);
		if( NULL == m_ppIndexBuffers[i] )
		{
			g_A3DErrLog.Log("A3DParticleSystem::CreateObjectFragment Not enough memory!");
			return false;
		}
	}
	// Init stream here, we only need to init one stream;
	m_pA3DStream = new A3DStream();
	if (!m_pA3DStream)
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateObjectFragment Not enough memory!");
		return false;
	}

	if (!m_pA3DStream->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nMaxVertCount, m_nMaxIndexCount,	
				A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateObjectFragment Init Stream Fail!");
		return false;
	}

	return true;
}

bool A3DParticleSystem::CreateInstancedGeometry()
{
	m_ParticleType = A3DPARTICLE_INSTANCED_GEOMETRY;
	m_nParticleSize = sizeof(INSTANCED_GEOMETRY_PARTICLE);

	if( g_pA3DConfig->GetRunEnv() != A3DRUNENV_GFXEDITOR )
		m_nMaxParticles = m_nInstancedModelNum; 
	else
		m_nMaxParticles = A3DPARTICLESYSTEM_MAXPARTICLES_TOTAL; // 1024
	
	m_pParticleBuffer = (BYTE *) malloc(m_nParticleSize * m_nMaxParticles);
	if( NULL == m_pParticleBuffer )
	{
		g_A3DErrLog.Log("A3DParticleSystem::CreateStandard(), Not enough memory!");
		return false;
	}
	return true;
}

bool A3DParticleSystem::Release()
{
	int			i;

	ReleaseParticles();	

	switch( m_ParticleType )
	{
	case A3DPARTICLE_STANDARD_PARTICLES:
		if( m_pVertexBuffer )
		{
			free(m_pVertexBuffer);
			m_pVertexBuffer = NULL;
		}
		if( m_pIndexBuffer )
		{
			free(m_pIndexBuffer);
			m_pIndexBuffer = NULL;
		}
		if( m_pA3DStream )
		{
			m_pA3DStream->Release();
			delete m_pA3DStream;
			m_pA3DStream = NULL;
		}
		if( m_pA3DTexture )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
			m_pA3DTexture = NULL;
		}
		break;
	case A3DPARTICLE_OBJECT_FRAGMENTS:
		for(i=0; i<m_nTextureNum; i++)
		{
			if( m_ppVertexBuffers[i] )
			{
				free(m_ppVertexBuffers[i]);
				m_ppVertexBuffers[i] = NULL;
			}
			if( m_ppIndexBuffers[i] )
			{
				free(m_ppIndexBuffers[i]);
				m_ppIndexBuffers[i] = NULL;
			}
			// For textures are all taken from objects, so we should not release it;
		}
		if( m_pA3DStream )
		{
			m_pA3DStream->Release();
			delete m_pA3DStream;
			m_pA3DStream = NULL;
		}
		ZeroMemory(m_pnVertCount, sizeof(int) * m_nTextureNum);
		ZeroMemory(m_pnIndexCount, sizeof(int) * m_nTextureNum);
		ZeroMemory(m_ppA3DTextures, sizeof(A3DTexture *) * m_nTextureNum);
		ZeroMemory(m_ppVertexBuffers, sizeof(A3DLVERTEX *) * m_nTextureNum);
		ZeroMemory(m_ppIndexBuffers, sizeof(WORD *) * m_nTextureNum);
		m_nTextureNum = 0;
		break;
	case A3DPARTICLE_INSTANCED_GEOMETRY:
		for(i=0; i<m_nInstancedModelNum; i++)
		{
			A3DModel * pInstancedModel = m_pInstancedModels[i];
			if( pInstancedModel->GetContainer() == A3DCONTAINER_NULL )
			{
			}
			else
			{
				m_pA3DDevice->GetA3DEngine()->RemoveModel(pInstancedModel);
			}

			// For by now the modelman has been release, we should not use modelman here;
			// But now we change the releasemodel to a static function
			pInstancedModel->Release();
			delete pInstancedModel;
			pInstancedModel = NULL;
		}
		m_nInstancedModelNum = 0;
		break;
	}

	if( m_pParticleBuffer )
	{
		free(m_pParticleBuffer);
		m_pParticleBuffer = NULL;
	}
	m_nMaxParticles = 0;
	m_nNumParticles = 0;
	return true;
}

bool A3DParticleSystem::ReleaseParticles()
{
	for(int i=0; i<m_nNumParticles; i++)
		MakeDead(m_pParticleBuffer + i * m_nParticleSize);
	
	m_nVertCount = m_nIndexCount = 0;
	m_nNumParticles = 0;
	return true;
}

bool A3DParticleSystem::TickEmitting()
{
	if( m_bExpired ) return true;

	bool		bval;

	m_nTicks ++;
	
	// See if we should start emitting;
	if( !m_bEmitting && m_nTicks == m_nEmitStart )
		m_bEmitting = true;

	//Check current state;
	if( m_nEmitEnd >= 0 && m_bEmitting && m_nTicks > m_nEmitEnd )
	{
		Stop();
	}

	if( m_nDisplayUntil >= 0 && m_nTicks > m_nDisplayUntil )
	{
		Stop(true);
		m_bExpired = true;
		return true;
	}

	if( m_bEmitting )
	{
		bval = EmitParticles();
		if( !bval ) return false;
	}

	bval = UpdateParticles();
	if( !bval ) return false;
	
	if( m_nTicks > m_nEmitStart && m_nNumParticles == 0 )
	{
		// For inherit particles, the first 2 ticks have no particles, so avoid stop it;
		if( m_vInheritInfluence != 0.0f && m_nTicks <= 2 )
			return true;

		// The particles has been empty, so we should stop now, need not to wait display until reached;
		Stop(true);
		m_bExpired = true;
		return true;
	}
	return true;
}

bool A3DParticleSystem::DefaultValues()
{
	m_ParticleType = A3DPARTICLE_STANDARD_PARTICLES;
	
	m_EmitterPos = A3DVECTOR3(0.0f, 0.0f, 0.0f);
	m_EmitterDir = A3DVECTOR3(0.0f, 0.0f, 1.0f);
	m_EmitterUp  = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	m_EmitterParentTM.Identity();

	UpdateRelativeTM();
	m_EmitterAbsoluteVelocity = A3DVECTOR3(0.0f);

	m_nTicks = 0;

	m_nColorNum = 0;
	m_vColorMapRange = 100.0f;
	m_ColorSpace = A3DPARTICLE_COLORSPACE_RGB;
	m_ColorMapPlane = A3DPARTICLE_COLORMAP_PLANE_XY;
	
	// Particle Quantity Control;
	m_bUseRate = true;
	m_nUseRate = 1;
	m_bUseTotal = false;
	m_nUseTotal = 100;

	// Particle Timing Control;
	m_nEmitStart = 0;
	m_nEmitEnd = -1;
	m_nDisplayUntil = -1;
	m_nLife = 100;
	m_nLifeVariation = 0;
	m_bEmitting = false;
	m_bExpired = true;
	
	// Particle Motion Control;
	m_vSpeed = 0.1f;
	m_vSpeedVariation = 0.0f; //%
	m_vDivergence = 0.0f;

	// Particle Size Control;
	m_vSize = 0.2f;
	m_vSizeVariation = 0.0f; //%
	m_nGrowFor = 0;
	m_nFadeFor = 0;

	// Uniqueness
	m_dwSeed = 12345678;

	// Object motion inheritance;
	m_vInheritInfluence = 0.0f; //%
	m_vInheritInfluenceVariation = 0.0f; //%
	m_vInheritMultiplier = 1.0f;

	// Particle Type Control Sets;
	// Standard Particle Control;
	m_StandardParticleType = A3DPARTICLE_STANDARD_PARTICLE_FACING;

	// Meta Particle Control;
	m_vMetaParticleTension = 0.0f; 
	m_vMetaParticleTensionVariation = 0.0f; //%
	
	// Object fragment Control;
	m_vFragmentThickness = 0.4f;
	m_ObjectFragmentType = A3DPARTICLE_FRAGMENT_TYPE_ALLFACES;
	m_nObjectFragmentMinimum = 100;
	m_vObjectFragmentSmoothAngle = 0.0f;
	
	// Particle Spawn Control Sets;
	m_SpawnType = A3DPARTICLE_SPAWN_NONE;
	m_nSpawns = 0;
	m_vSpawnAffects = 0.0f;
	m_nSpawnMultiplier = 0;
	m_vSpawnVariation = 0.0f;

	// Particle Rotation Control Sets;
	m_nSpinTime = 0;
	m_vSpinTimeVariation = 0.0f; //%
	m_vSpinPhase = 0.0f; //Deg
	m_vSpinPhaseVariation = 0.0f; //%
	
	m_SpinAxis = A3DPARTICLE_SPINAXIS_RANDOM;
	m_vecSpinDefinedAxis = A3DVECTOR3(0.0f);
	m_vSpinAxisVariation = 0.0f; //deg
	
	// Bubble Motion Control Sets;
	m_vBubbleAmplitude = 0.0f;
	m_vBubbleAmplitudeVariation = 0.0f; //%
	m_nBubblePeriod = 0;
	m_vBubblePeriodVariation = 0.0f; //%
	m_vBubblePhase = 0.0f;
	m_vBubblePhaseVariation = 0.0f; //%

	// The emitter stops at the beginning;
	m_bEmitting = false;
	
	// Blend flags;
	m_SrcBlend = A3DBLEND_SRCALPHA;
	m_DestBlend = A3DBLEND_INVSRCALPHA;
	return true;
}

bool A3DParticleSystem::Start(int nEmitLasting, int nRenderLasting)
{
	bool bval;

	// For the first time, we must clear our velocity;
	ClearAbsoluteVelocity();

	m_nTicks = 0;
	m_bExpired  = false;
	m_nVertCount = m_nIndexCount = 0;
	m_nNumParticles = 0;
	
	// If this method specify a new ending flag, we use this instead of read from file
	if( nEmitLasting > 0 )
		m_nEmitEnd = nEmitLasting;
	if( nRenderLasting > 0 )
		m_nDisplayUntil = nRenderLasting;

	// The emitting will begin at this very frame;
	if( m_nTicks == m_nEmitStart )
	{
		m_bEmitting = true;
		bval = EmitParticles();
		if( !bval ) return false;
	}

	return true;
}

bool A3DParticleSystem::Stop(bool bDeleteCurrent)
{
	m_bEmitting = false;
	
	if( bDeleteCurrent && m_nNumParticles )
		ReleaseParticles();

	return true;
}

bool A3DParticleSystem::UpdateParticles()
{
	if( m_nNumParticles == 0 )
		return true;

	switch(m_ParticleType)
	{
	case A3DPARTICLE_STANDARD_PARTICLES:
		return UpdateStandardParticles();
	case A3DPARTICLE_META_PARTICLES:
		g_A3DErrLog.Log("Not implemented now!");
		return false;
	case A3DPARTICLE_OBJECT_FRAGMENTS:
		return UpdateObjectFragmentParticles();
	case A3DPARTICLE_INSTANCED_GEOMETRY:
		return UpdateInstancedGeometryParticles();
	}
	return true;
}

bool A3DParticleSystem::UpdateStandardParticles()
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

bool A3DParticleSystem::UpdateMetaParticles()
{
	return true;
}

bool A3DParticleSystem::UpdateObjectFragmentParticles()
{
	OBJECT_FRAGMENT_PARTICLE * pParticle = (OBJECT_FRAGMENT_PARTICLE *) m_pParticleBuffer;
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
			if( pParticle->pGFX )
				pParticle->pGFX->SetPos(pParticle->common.vecPos);

			++pParticle;
		}
	}
	return true;
}

bool A3DParticleSystem::UpdateInstancedGeometryParticles()
{
	INSTANCED_GEOMETRY_PARTICLE * pParticle = (INSTANCED_GEOMETRY_PARTICLE *) m_pParticleBuffer;
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
			A3DVECTOR3 vecStart = pParticle->pModel->GetPos();
			A3DVECTOR3 vecPos = pParticle->common.vecPos;
			
			if( pParticle->nSunkTicks > 0 )
			{
				pParticle->nSunkTicks --;
				if( pParticle->nSunkTicks == 0 )
				{
					MakeDead(pParticle);
					DeleteParticle(i);
					--i;
					continue;
				}
				else
				{
					if( pParticle->nSunkTicks <= 30 )
					{
						// Sinking;
						vecPos = vecStart;
						vecPos.y -= 0.02f;
						pParticle->common.vecPos = vecPos;
						pParticle->pModel->SetPos(vecPos);
					}
				}
			}
			else
			{
				A3DCDS* pA3DCDS = m_pA3DDevice->GetA3DEngine()->GetA3DCDS();
				if (pA3DCDS)
				{
					RAYTRACERT rayTrace;
					A3DVECTOR3 vecSpeed = pParticle->common.vecSpeed;
					bool bHit = pA3DCDS->RayTrace(vecStart, vecSpeed, 1.0f, &rayTrace, TRACEOBJ_PARTICAL, 0);
					if (bHit)
					{
						vecPos = rayTrace.vPoint;
						FLOAT vProj = DotProduct(vecSpeed, rayTrace.vNormal);
						if( vProj < 0.0f )
						{		
							vecSpeed = 0.6f * (vecSpeed + (-1.8f * vProj) * rayTrace.vNormal);
							pParticle->common.vecSpeed = vecSpeed;
							if (vecSpeed.Magnitude() < 0.1f) // Too slow touch speed, about to stop
							{
								// If the paritcle can stay there, we just let it sunk,
								// or we must slide along the plane;
								if( rayTrace.vNormal.y > 0.9f )
								{
									pParticle->nSunkTicks = 60;	
									pParticle->common.vecSpeed = A3DVECTOR3(0.0f);
								}
							}
							else
							{
								// Now we should play an action to invoke the hit ground sound
								if( pParticle->pModel->FindGroupActionByName("[ÂäµØ]", NULL) )
								{
									pParticle->pModel->PlayActionByName("[ÂäµØ]", true);
								}
							}
						}
						
					}
					else
						vecPos = vecStart + vecSpeed;
				}
				if( pParticle->common.nSpinTime != 0 )
					pParticle->pModel->RotateAxisRelative(pParticle->common.vecSpinAxis, DEG2RAD(360.0f / pParticle->common.nSpinTime));
				pParticle->common.vecPos = vecPos;
				pParticle->pModel->SetPos(vecPos);
			}
			++pParticle;
		}
	}
	return true;
}

bool A3DParticleSystem::RenderParticles(A3DViewport * pCurrentViewport)
{
	if( m_bExpired )
		return true;

	if( m_nNumParticles == 0 )
		return true;

	switch( m_ParticleType )
	{
	case A3DPARTICLE_STANDARD_PARTICLES:  
		return RenderStandardParticles(pCurrentViewport);
	case A3DPARTICLE_META_PARTICLES:
		g_A3DErrLog.Log("Not implemented now!");
		return false;
	case A3DPARTICLE_OBJECT_FRAGMENTS:
		return RenderObjectFragmentParticles(pCurrentViewport);
	case A3DPARTICLE_INSTANCED_GEOMETRY:
		// For instanced geometry will be rendered in a3dengine, so here we do not render;
		return true;
	}
	return true;
}

bool A3DParticleSystem::RenderStandardParticles(A3DViewport * pCurrentViewport)
{
	m_nVertCount = m_nIndexCount = 0;

	// first of all, we should use some bounding box to check
	// if some part of the particle system resides in the viewport;

	STANDARD_PARTICLE * pParticle = (STANDARD_PARTICLE *) m_pParticleBuffer;
	for(int id=0; id<m_nNumParticles; id++)
	{
		COMMON_PARTICLE * pCommonParticle = &pParticle->common;
		
		int			i;
		FLOAT		vSize;
		A3DVECTOR3	vecPos, vecCenter, vecScreenCenter;
		A3DVECTOR4	vecCorner[4];
		A3DVECTOR3	vecExt, vecDiff;
		A3DVECTOR3	vecStart, vecEnd, vecP;
		A3DCOLOR	diffuse, specular;

		//First determine the display size;
		if( pCommonParticle->nLive < m_nGrowFor || pCommonParticle->nLife < m_nFadeFor )
		{
			if( pCommonParticle->nLive < m_nGrowFor )
			{
				vSize = pCommonParticle->vSize * (pCommonParticle->nLive + 1) / m_nGrowFor;	
			}
			else if( pCommonParticle->nLife < m_nFadeFor )
			{
				vSize = pCommonParticle->vSize * (pCommonParticle->nLife + 1) / m_nFadeFor;	
			}
		}
		else
			vSize = pCommonParticle->vSize;

		switch( m_StandardParticleType )
		{
		case A3DPARTICLE_STANDARD_PARTICLE_FACING:
		case A3DPARTICLE_STANDARD_PARTICLE_CONSTANT:
			if( m_StandardParticleType == A3DPARTICLE_STANDARD_PARTICLE_FACING )
			{
				vecPos = pCommonParticle->vecPos;
				//Wobble the particle if needed;
				if( pCommonParticle->vBubbleAmplitude != 0.0f )
				{
					// The real bubble wobble effect is achieved by just moving in vecUp's plane;
					// Below can create an effect just like quake's rail gun;
					/*
					FLOAT vSin = g_pA3DMathUtility->SIN(pParticle->vBubblePhase);
					A3DMATRIX4 mat = RotateAxis(vecPos, pParticle->vecDir, DEG2RAD(pParticle->vBubblePhase));
					vecPos = mat * (vecPos + pParticle->vBubbleAmplitude * vSin * pParticle->vecUp);*/
					FLOAT vSin = g_A3DMathUtility.SIN(pCommonParticle->vBubblePhase);
					vecPos = vecPos + vSin * pCommonParticle->vecUp;
				}
				vecCenter	= vecPos;
				vecExt		= vecCenter + pCurrentViewport->GetCamera()->GetRight() * vSize;

				// Transform the center and if it is clipped, just continue;
				if( pCurrentViewport->Transform(vecCenter, vecCenter) )
					goto NEXTPARTICLE;
				// No check for whether extents are clipped;
				pCurrentViewport->Transform(vecExt, vecExt);
				vSize = vecExt.x - vecCenter.x;
			}
			else
			{
				vecCenter = pCommonParticle->vecPos;
				if( pCurrentViewport->Transform(vecCenter, vecCenter) )
					goto NEXTPARTICLE;
			}
			// Now use half size;
			vSize /= 2.0f;
			//Rotate the patch if needed;
			if( pCommonParticle->vSpinPhase != 0.0f )
			{
				FLOAT vSin, vCos;
				vSin = g_A3DMathUtility.SIN(pCommonParticle->vSpinPhase);
				vCos = g_A3DMathUtility.COS(pCommonParticle->vSpinPhase);

				vecCorner[0] = A3DVECTOR4(-vSize, -vSize, vecCenter.z, 1.0f);
				vecCorner[1] = A3DVECTOR4( vSize, -vSize, vecCenter.z, 1.0f);
				vecCorner[2] = A3DVECTOR4(-vSize,  vSize, vecCenter.z, 1.0f);
				vecCorner[3] = A3DVECTOR4( vSize,  vSize, vecCenter.z, 1.0f);
				for(i=0; i<4; i++)
				{
					FLOAT x = vecCenter.x + vecCorner[i].x * vCos + vecCorner[i].y * vSin;
					FLOAT y = vecCenter.y + vecCorner[i].y * vCos - vecCorner[i].x * vSin;
					vecCorner[i].x = x;
					vecCorner[i].y = y;
				}
			}
			else
			{
				vecCorner[0] = A3DVECTOR4(vecCenter.x - vSize, vecCenter.y - vSize, vecCenter.z, 1.0f);
				vecCorner[1] = A3DVECTOR4(vecCenter.x + vSize, vecCenter.y - vSize, vecCenter.z, 1.0f);
				vecCorner[2] = A3DVECTOR4(vecCenter.x - vSize, vecCenter.y + vSize, vecCenter.z, 1.0f);
				vecCorner[3] = A3DVECTOR4(vecCenter.x + vSize, vecCenter.y + vSize, vecCenter.z, 1.0f);
			}
			
			diffuse  = pCommonParticle->color;
			specular = A3DCOLORRGBA(0, 0, 0, 255); 

			m_pVertexBuffer[m_nVertCount + 0] = A3DTLVERTEX(vecCorner[0], diffuse, specular, 0.0f, 0.0f);
			m_pVertexBuffer[m_nVertCount + 1] = A3DTLVERTEX(vecCorner[1], diffuse, specular, 1.0f, 0.0f);
			m_pVertexBuffer[m_nVertCount + 2] = A3DTLVERTEX(vecCorner[2], diffuse, specular, 0.0f, 1.0f);
			m_pVertexBuffer[m_nVertCount + 3] = A3DTLVERTEX(vecCorner[3], diffuse, specular, 1.0f, 1.0f);

			m_nVertCount += 4;
			m_nIndexCount += 6;
			break;
		case A3DPARTICLE_STANDARD_PARTICLE_TETRA:
			vecPos = pCommonParticle->vecPos;

			vecStart	= vecPos;
			vecExt		= vecStart + pCurrentViewport->GetCamera()->GetRight() * vSize;

			// Transform the center and if it is clipped, just continue;
			if( pCurrentViewport->Transform(vecStart, vecStart) )
				goto NEXTPARTICLE;

			// No check for whether extents are clipped;
			pCurrentViewport->Transform(vecExt, vecExt);
			vSize = vecExt.x - vecStart.x;
			vSize /= 2.0f;

			vecEnd = vecPos + pCommonParticle->vecSpeed - m_EmitterAbsoluteVelocity;
			if( pCurrentViewport->Transform(vecEnd, vecEnd) )
				goto NEXTPARTICLE;

			vecDiff = vecEnd - vecStart;
			vecDiff.z = 0.0f;

			if( fabs(vecDiff.x) < vSize * 1.4f && fabs(vecDiff.y) < vSize * 1.4f )
			{
				// We should take some modification here to make the particle a decal;
				vecCenter = (vecStart + vecEnd) / 2.0f;
				vecCorner[0] = A3DVECTOR4(vecCenter.x - vSize, vecCenter.y - vSize, vecCenter.z, 1.0f);
				vecCorner[1] = A3DVECTOR4(vecCenter.x + vSize, vecCenter.y - vSize, vecCenter.z, 1.0f);
				vecCorner[2] = A3DVECTOR4(vecCenter.x - vSize, vecCenter.y + vSize, vecCenter.z, 1.0f);
				vecCorner[3] = A3DVECTOR4(vecCenter.x + vSize, vecCenter.y + vSize, vecCenter.z, 1.0f);
			}
			else
			{
				vecP = Normalize(CrossProduct(vecDiff, A3DVECTOR3(0.0f, 0.0f, 1.0f)));
				vecP.x *= vSize;
				vecP.y *= vSize;

				vecCorner[0] = A3DVECTOR4(vecEnd.x + vecP.x, vecEnd.y + vecP.y, vecEnd.z, 1.0f);
				vecCorner[1] = A3DVECTOR4(vecEnd.x - vecP.x, vecEnd.y - vecP.y, vecEnd.z, 1.0f);
				vecCorner[2] = A3DVECTOR4(vecStart.x + vecP.x, vecStart.y + vecP.y, vecStart.z, 1.0f);
				vecCorner[3] = A3DVECTOR4(vecStart.x - vecP.x, vecStart.y - vecP.y, vecStart.z, 1.0f);
			}
			
			diffuse  = pCommonParticle->color;
			specular = A3DCOLORRGBA(0, 0, 0, 255); 

			m_pVertexBuffer[m_nVertCount + 0] = A3DTLVERTEX(vecCorner[0], diffuse, specular, 0.0f, 0.0f);
			m_pVertexBuffer[m_nVertCount + 1] = A3DTLVERTEX(vecCorner[1], diffuse, specular, 1.0f, 0.0f);
			m_pVertexBuffer[m_nVertCount + 2] = A3DTLVERTEX(vecCorner[2], diffuse, specular, 0.0f, 1.0f);
			m_pVertexBuffer[m_nVertCount + 3] = A3DTLVERTEX(vecCorner[3], diffuse, specular, 1.0f, 1.0f);

			m_nVertCount += 4;
			m_nIndexCount += 6;
			break;
		default:
			g_A3DErrLog.Log("A3DSuperSpray::RenderParticles now you can only use A3DPARTICLE_STANDARD_PARTICLE_FACING or TETRA!");
			return false;
		}

		if( m_nIndexCount >= m_nMaxIndexCount || m_nVertCount >= m_nMaxVertCount )
		{
			m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
			m_pA3DDevice->SetDestAlpha(m_DestBlend);
			m_pA3DDevice->SetZWriteEnable(false);
			m_pA3DStream->SetVertsDynamic((LPBYTE)m_pVertexBuffer, m_nVertCount);
			m_pA3DStream->Appear();
			m_pA3DTexture->Appear(0);
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nIndexCount / 3);
			m_nVertCount = m_nIndexCount = 0;
			m_pA3DTexture->Disappear(0);
			m_pA3DDevice->SetZWriteEnable(true);
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		}

NEXTPARTICLE:
		++pParticle;
	}

	if( m_nVertCount )
	{
		m_pA3DDevice->SetZWriteEnable(false);
		m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
		m_pA3DDevice->SetDestAlpha(m_DestBlend);
		m_pA3DStream->SetVertsDynamic((LPBYTE)m_pVertexBuffer, m_nVertCount);
		m_pA3DStream->Appear();
		m_pA3DTexture->Appear(0);
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nIndexCount / 3);
		m_pA3DTexture->Disappear(0);
		m_nVertCount = m_nIndexCount = 0;
		m_pA3DDevice->SetZWriteEnable(true);
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}
	return true;
}

bool A3DParticleSystem::RenderMetaParticles(A3DViewport * pCurrentViewport)
{
	return true;
}

bool A3DParticleSystem::RenderObjectFragmentParticles(A3DViewport * pCurrentViewport)
{
	ZeroMemory(m_pnVertCount, sizeof(int) * m_nTextureNum);
	ZeroMemory(m_pnIndexCount, sizeof(int) * m_nTextureNum);

	int			i;
	int			nTextureID;
	A3DMATRIX4  matWorld = a3d_IdentityMatrix();

	m_pA3DDevice->SetWorldMatrix(matWorld);

	OBJECT_FRAGMENT_PARTICLE * pParticle = (OBJECT_FRAGMENT_PARTICLE *) m_pParticleBuffer;
	for(int id=0; id<m_nNumParticles; id++)
	{
		COMMON_PARTICLE * pCommonParticle = &pParticle->common;
		
		// If it is invisible, just continue with next particle;
		A3DVECTOR3 vecPos	 = pCommonParticle->vecPos;
		A3DVECTOR3 vecCenter = vecPos; // For object fragment particles, we use world space coordinate directly;

		A3DMATRIX4 matTM;

		if( pCurrentViewport->Transform(vecCenter, vecCenter) )
			goto NEXTPARTICLE;
		
		nTextureID = pParticle->nTextureID;

		// Exceed max vertex count? If so we have to render it now so to clear the buffer;
		if( m_pnVertCount[nTextureID] + pParticle->nVertCount > m_nMaxVertCount ||
			m_pnIndexCount[nTextureID] + pParticle->nIndexCount > m_nMaxIndexCount )
		{
			m_pA3DDevice->SetLightingEnable(false);
			m_pA3DStream->SetVertsDynamic((LPBYTE)m_ppVertexBuffers[nTextureID], m_pnVertCount[nTextureID]);
			m_pA3DStream->SetIndicesDynamic(m_ppIndexBuffers[nTextureID], m_pnIndexCount[nTextureID]);
			m_pA3DStream->Appear();
			m_ppA3DTextures[nTextureID]->Appear(0);

			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_pnVertCount[nTextureID], 0, m_pnIndexCount[nTextureID] / 3);

			m_ppA3DTextures[nTextureID]->Disappear(0);
			m_pA3DDevice->SetLightingEnable(true);
			m_pnVertCount[nTextureID]  = 0;
			m_pnIndexCount[nTextureID] = 0;
		}

		// Add this particle's vertex into the vertex buffer;
		// We should construct an transform matrix according to the vecPos
		// and the vSpinPhase, vecSpinAxis
		matTM.RotateAxis(pCommonParticle->vecSpinAxis, DEG2RAD(pCommonParticle->vSpinPhase));
		matTM *= a3d_Translate(pCommonParticle->vecPos.x, pCommonParticle->vecPos.y, pCommonParticle->vecPos.z);

		// First fill index buffer;
		for(i=0; i<pParticle->nIndexCount; i++)
		{
			m_ppIndexBuffers[nTextureID][m_pnIndexCount[nTextureID]] = pParticle->pIndex[i] + m_pnVertCount[nTextureID];
			m_pnIndexCount[nTextureID] ++;
		}

		// Then fill vertex buffer;
		for(i=0; i<pParticle->nVertCount; i++)
		{
			vecPos = A3DVECTOR3(pParticle->pVertex[i].x, pParticle->pVertex[i].y, pParticle->pVertex[i].z); 
			vecPos = vecPos * matTM;
			m_ppVertexBuffers[nTextureID][m_pnVertCount[nTextureID]] = 
				A3DLVERTEX(vecPos, pCommonParticle->color/*pParticle->pVertex[i].diffuse*/, pParticle->pVertex[i].specular, pParticle->pVertex[i].tu, pParticle->pVertex[i].tv);
			m_pnVertCount[nTextureID] ++;
		}

NEXTPARTICLE:
		++pParticle;
	}

	for(i=0; i<m_nTextureNum; i++)
	{
		if( m_pnVertCount[i] )
		{
			m_pA3DDevice->SetSourceAlpha(m_SrcBlend);
			m_pA3DDevice->SetDestAlpha(m_DestBlend);
			m_pA3DDevice->SetLightingEnable(false);
			m_pA3DStream->SetVertsDynamic((LPBYTE)m_ppVertexBuffers[i], m_pnVertCount[i]);
			m_pA3DStream->SetIndicesDynamic(m_ppIndexBuffers[i], m_pnIndexCount[i]);
			m_pA3DStream->Appear();
			m_ppA3DTextures[i]->Appear(0);

			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_pnVertCount[i], 0, m_pnIndexCount[i] / 3);

			m_ppA3DTextures[i]->Disappear(0);
			m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
			m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
			m_pA3DDevice->SetLightingEnable(true);
			m_pnVertCount[i] = 0;
			m_pnIndexCount[i] = 0;
		}
	}

	return true;
}

bool A3DParticleSystem::RenderInstancedGeometryParticles(A3DViewport * pCurrentViewport)
{
	return true;
}

bool A3DParticleSystem::Save(AFile * pFileToSave)
{
	int				i;

	if( pFileToSave->IsBinary() )
	{
		//Save Binary here;
	}
	else
	{
		//Save Text here;
		char		szLineBuffer[2048];

		sprintf(szLineBuffer, "ParticleType: %d", m_ParticleType);
		pFileToSave->WriteLine(szLineBuffer);

		if( m_ParticleType == A3DPARTICLE_INSTANCED_GEOMETRY )
		{
			sprintf(szLineBuffer, "InstancedModelNum: %d", m_nInstancedModelNum);
			pFileToSave->WriteLine(szLineBuffer);
			for(int i=0; i<m_nInstancedModelNum; i++)
				pFileToSave->WriteLine(m_szInstancedModelName[i]);
		}

		// Max visible particles are calculated internally;
		sprintf(szLineBuffer, "MaxVisibleParticles: %d", m_nMaxVisibleParticles);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "Position: (%f, %f, %f)", m_EmitterPos.x, m_EmitterPos.y, m_EmitterPos.z);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Dir: (%f, %f, %f)", m_EmitterDir.x, m_EmitterDir.y, m_EmitterDir.z);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Up: (%f, %f, %f)", m_EmitterUp.x, m_EmitterUp.y, m_EmitterUp.z);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "ColorNum: %d", m_nColorNum);
		pFileToSave->WriteLine(szLineBuffer);
		for(i=0; i<m_nColorNum; i++)
		{
			sprintf(szLineBuffer, "Color%d: %x", i, m_pColors[i]);
			pFileToSave->WriteLine(szLineBuffer);
		}
		sprintf(szLineBuffer, "ColorMapRange: %f", m_vColorMapRange);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "ColorSpace: %d", m_ColorSpace);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "ColorMapPlane: %d", m_ColorMapPlane);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "bUseRate: %d", m_bUseRate);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "UseRate: %d", m_nUseRate);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "bUseTotal: %d", m_bUseTotal);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "UseTotal: %d", m_nUseTotal);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "EmitStart: %d", m_nEmitStart);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "EmitEnd: %d", m_nEmitEnd);
		pFileToSave->WriteLine(szLineBuffer);
		
		sprintf(szLineBuffer, "DisplayUntil: %d", m_nDisplayUntil);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "Life: %d", m_nLife);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "LifeVariation: %d", m_nLifeVariation);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "Speed: %f", m_vSpeed);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpeedVariation: %f%%", m_vSpeedVariation);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Divergence: %f", m_vDivergence);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "Size: %f", m_vSize);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SizeVariation: %f%%", m_vSizeVariation);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "GrowFor: %d", m_nGrowFor);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "FadeFor: %d", m_nFadeFor);
		pFileToSave->WriteLine(szLineBuffer);
	
		sprintf(szLineBuffer, "Seed: %x", m_dwSeed);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "InheritInfluence: %f%%", m_vInheritInfluence);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "InheritInfluenceVariation: %f%%", m_vInheritInfluenceVariation);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "InheritMultiplier: %f", m_vInheritMultiplier);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Gravity: %f", m_vGravity);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "StandardParticleType: %d", m_StandardParticleType);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "TextureMap: %s", m_szTextureMap);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "SrcBlend: %d", m_SrcBlend);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "DestBlend: %d", m_DestBlend);
		pFileToSave->WriteLine(szLineBuffer);
		
		sprintf(szLineBuffer, "MetaParticleTension: %f", m_vMetaParticleTension);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "MetaParticleTensionVariation: %f%%", m_vMetaParticleTensionVariation);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "FragmentThickness: %f", m_vFragmentThickness);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "ObjectFragmentType: %d", m_ObjectFragmentType);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "ObjectFragmentMinimum: %d", m_nObjectFragmentMinimum);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "ObjectFragmentSmoothAngle: %f", m_vObjectFragmentSmoothAngle);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "SpawnType: %d", m_SpawnType);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "Spawns: %d", m_nSpawns);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpawnAffects: %f", m_vSpawnAffects);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpawnMultiplier: %d", m_nSpawnMultiplier);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpawnVariation: %f", m_vSpawnVariation);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "SpinTime: %d", m_nSpinTime);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpinTimeVariation: %f%%", m_vSpinTimeVariation);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpinPhase: %f", m_vSpinPhase);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpinPhaseVariation: %f%%", m_vSpinPhaseVariation);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpinAxis: %d", m_SpinAxis);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpinDefinedAxis: (%f, %f, %f)", m_vecSpinDefinedAxis.x, m_vecSpinDefinedAxis.y, m_vecSpinDefinedAxis.z);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "SpinAxisVariation: %f", m_vSpinAxisVariation);
		pFileToSave->WriteLine(szLineBuffer);

		sprintf(szLineBuffer, "BubbleAmplitude: %f", m_vBubbleAmplitude);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "BubbleAmplitudeVariation: %f%%", m_vBubbleAmplitudeVariation);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "BubblePeriod: %d", m_nBubblePeriod);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "BubblePeriodVariation: %f%%", m_vBubblePeriodVariation);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "BubblePhase: %f", m_vBubblePhase);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "BubblePhaseVariation: %f%%", m_vBubblePhaseVariation);
		pFileToSave->WriteLine(szLineBuffer);
	}
	return true;
}

bool A3DParticleSystem::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
{
	int			i;

	if( !Init(pA3DDevice) )
		return false;

	if( pFileToLoad->IsBinary() )
	{
		//Load Binary here;
	}
	else
	{
		//Load Text here;
		char			szLineBuffer[AFILE_LINEMAXLEN];
		char			szResult[AFILE_LINEMAXLEN];
		DWORD			dwReadLen;
		int				nval;

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ParticleType: %d", &m_ParticleType);
		
		if( m_ParticleType == A3DPARTICLE_INSTANCED_GEOMETRY )
		{
			int nInstancedModelNum; 
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "InstancedModelNum: %d", &nInstancedModelNum);
			for(int i=0; i<nInstancedModelNum; i++)
			{
				pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				AddInstancedModel(szLineBuffer);
			}
		}
		// Max visible particles are calculated internally;
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "MaxVisibleParticles: %d", &m_nMaxVisibleParticles);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Position: (%f, %f, %f)", &m_EmitterPos.x, &m_EmitterPos.y, &m_EmitterPos.z);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Dir: (%f, %f, %f)", &m_EmitterDir.x, &m_EmitterDir.y, &m_EmitterDir.z);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Up: (%f, %f, %f)", &m_EmitterUp.x, &m_EmitterUp.y, &m_EmitterUp.z);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ColorNum: %d", &m_nColorNum);
		for(i=0; i<m_nColorNum; i++)
		{
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "Color%d: %x", &nval, &m_pColors[i]);
			if( i != nval )
			{
				g_A3DErrLog.Log("A3DParticleSystem::Load() Parse file error!");
				return false;
			}
		}

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ColorMapRange: %f", &m_vColorMapRange);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ColorSpace: %d", &m_ColorSpace);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ColorMapPlane: %d", &m_ColorMapPlane);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "bUseRate: %d", &nval);
		m_bUseRate = nval ? true : false;
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "UseRate: %d", &m_nUseRate);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "bUseTotal: %d", &nval);
		m_bUseTotal = nval ? true : false;
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "UseTotal: %d", &m_nUseTotal);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "EmitStart: %d", &m_nEmitStart);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "EmitEnd: %d", &m_nEmitEnd);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "DisplayUntil: %d", &m_nDisplayUntil);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Life: %d", &m_nLife);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "LifeVariation: %d", &m_nLifeVariation);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Speed: %f", &m_vSpeed);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpeedVariation: %f%%", &m_vSpeedVariation);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Divergence: %f", &m_vDivergence);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Size: %f", &m_vSize);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SizeVariation: %f%%", &m_vSizeVariation);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "GrowFor: %d", &m_nGrowFor);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "FadeFor: %d", &m_nFadeFor);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Seed: %x", &m_dwSeed);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "InheritInfluence: %f%%", &m_vInheritInfluence);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "InheritInfluenceVariation: %f%%", &m_vInheritInfluenceVariation);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "InheritMultiplier: %f", &m_vInheritMultiplier);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Gravity: %f", &m_vGravity);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "StandardParticleType: %d", &m_StandardParticleType);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TextureMap: ", szResult);
		strncpy(m_szTextureMap, szResult, MAX_PATH);

		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strstr(szLineBuffer, "SrcBlend: ") == szLineBuffer )
		{
			sscanf(szLineBuffer, "SrcBlend: %d", &m_SrcBlend);
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLineBuffer, "DestBlend: %d", &m_DestBlend);
			pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		}
		
		sscanf(szLineBuffer, "MetaParticleTension: %f", &m_vMetaParticleTension);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "MetaParticleTensionVariation: %f%%", &m_vMetaParticleTensionVariation);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "FragmentThickness: %f", &m_vFragmentThickness);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ObjectFragmentType: %d", &m_ObjectFragmentType);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ObjectFragmentMinimum: %d", &m_nObjectFragmentMinimum);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "ObjectFragmentSmoothAngle: %f", &m_vObjectFragmentSmoothAngle);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpawnType: %d", &m_SpawnType);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "Spawns: %d", &m_nSpawns);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpawnAffects: %f", &m_vSpawnAffects);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpawnMultiplier: %d", &m_nSpawnMultiplier);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpawnVariation: %f", &m_vSpawnVariation);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpinTime: %d", &m_nSpinTime);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpinTimeVariation: %f%%", &m_vSpinTimeVariation);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpinPhase: %f", &m_vSpinPhase);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpinPhaseVariation: %f%%", &m_vSpinPhaseVariation);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpinAxis: %d", &m_SpinAxis);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpinDefinedAxis: (%f, %f, %f)", &m_vecSpinDefinedAxis.x, &m_vecSpinDefinedAxis.y, &m_vecSpinDefinedAxis.z);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "SpinAxisVariation: %f", &m_vSpinAxisVariation);
		
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "BubbleAmplitude: %f", &m_vBubbleAmplitude);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "BubbleAmplitudeVariation: %f%%", &m_vBubbleAmplitudeVariation);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "BubblePeriod: %d", &m_nBubblePeriod);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "BubblePeriodVariation: %f%%", &m_vBubblePeriodVariation);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "BubblePhase: %f", &m_vBubblePhase);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "BubblePhaseVariation: %f%%", &m_vBubblePhaseVariation);
	}

	UpdateRelativeTM();
	return true;
}

bool A3DParticleSystem::UpdateUp()
{
	A3DVECTOR3 vecLeft;
	if( fabs(m_EmitterDir.y) > 0.9f )
		m_EmitterUp = A3DVECTOR3(1.0f, 0.0f, 0.0f); // X-Axis;
	else
		m_EmitterUp = A3DVECTOR3(0.0f, 1.0f, 0.0f);
	vecLeft = Normalize(CrossProduct(m_EmitterDir, m_EmitterUp));
	m_EmitterUp = Normalize(CrossProduct(vecLeft, m_EmitterDir));
	return true;
}

bool A3DParticleSystem::UpdateParentTM(A3DMATRIX4& matParent)
{
	m_EmitterParentTM = matParent;

	return UpdateAbsoluteTM();
}

bool A3DParticleSystem::UpdateAbsoluteTM()
{
	A3DMATRIX4 matAbsoluteTM = m_EmitterRelativeTM * m_EmitterParentTM;

	A3DVECTOR3 vecPosOld = m_EmitterAbsoluteTM.GetRow(3);
	A3DVECTOR3 vecPosNew = matAbsoluteTM.GetRow(3);

	m_EmitterAbsoluteTM = matAbsoluteTM;
	m_EmitterAbsoluteVelocity = vecPosNew - vecPosOld;
	return true;
}

bool A3DParticleSystem::UpdateRelativeTM()
{
	m_EmitterRelativeTM = a3d_TransformMatrix(m_EmitterDir, m_EmitterUp, m_EmitterPos);
	return UpdateAbsoluteTM();
}

bool A3DParticleSystem::AddInstancedModel(char * szInstancedModel)
{
	if( !m_pA3DDevice )
		return true;

	if( m_nInstancedModelNum >= A3DPARTICLESYSTEM_INSTANCED_MAXMODEL )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DModelMan()->LoadModelFile(szInstancedModel, &m_pInstancedModels[m_nInstancedModelNum]) )
	{
		g_A3DErrLog.Log("A3DParticleSystem::AddInstancedModel(), Can not load model [%s]", szInstancedModel);
		return false;
	}
	strncpy(m_szInstancedModelName[m_nInstancedModelNum], szInstancedModel, MAX_PATH);

	m_nInstancedModelNum ++;
	return true;
}

bool A3DParticleSystem::RemoveInstancedModel(int index)
{
	Stop(true);

	if( index >= m_nInstancedModelNum )
		return true;

	A3DModel * pInstancedModel = m_pInstancedModels[index];
	if( pInstancedModel->GetContainer() == A3DCONTAINER_NULL )
	{
	}
	else
	{
		m_pA3DDevice->GetA3DEngine()->RemoveModel(pInstancedModel);
	}
	m_pA3DDevice->GetA3DEngine()->GetA3DModelMan()->ReleaseModel(pInstancedModel);	

	for(int i=index; i<m_nInstancedModelNum - 1; i++)
	{
		m_pInstancedModels[i] = m_pInstancedModels[i + 1];
		strncpy(m_szInstancedModelName[i], m_szInstancedModelName[i + 1], MAX_PATH);
	}

	m_nInstancedModelNum --;
	return true;
}

void A3DParticleSystem::SetParticleType(A3DPARTICLE_TYPE type)
{
	if( g_pA3DConfig->GetRunEnv() != A3DRUNENV_GFXEDITOR )
	{
		// We do not allow dynamic type change in game enviorament;
		m_ParticleType = type;
		return;
	}
	else
	{
		if( m_ParticleType != A3DPARTICLE_TYPE_UNKNOWN )
		{
			Release();
			m_ParticleType = type;
			switch(type)
			{
			case A3DPARTICLE_STANDARD_PARTICLES:
				CreateStandard();
				break;
			case A3DPARTICLE_META_PARTICLES:
				CreateMeta();
				break;
			case A3DPARTICLE_OBJECT_FRAGMENTS:
				g_A3DErrLog.Log("A3DParticleSystem::SetParticleType(), Can not change to object fragment dynamicly!");
				break;
			case A3DPARTICLE_INSTANCED_GEOMETRY:
				CreateInstancedGeometry();
				break;
			}
		}
		else
			m_ParticleType = type;
	}
}

A3DCOLOR A3DParticleSystem::GetParticleColor(FLOAT vHeight)
{
	if( vHeight < 0.0f ) vHeight = -vHeight;
	if( m_nColorNum == 0 ) return A3DCOLORRGBA(255, 255, 255, 255);
	else
	{
		if( m_nColorNum == 1 ) return m_pColors[0];
		else
		{
			FLOAT vIndex = vHeight / m_vColorMapRange * (m_nColorNum - 1);
			int   nIndex = (int) vIndex;

			if( nIndex >= m_nColorNum - 1 )
				return m_pColors[m_nColorNum - 1];
			else
			{
				FLOAT f = vIndex - nIndex;
				if( m_ColorSpace == A3DPARTICLE_COLORSPACE_RGB )
				{
					int r1, g1, b1, a1, r2, g2, b2, a2, r, g, b, a;	 
					r1 = A3DCOLOR_GETRED  (m_pColors[nIndex]);
					g1 = A3DCOLOR_GETGREEN(m_pColors[nIndex]);
					b1 = A3DCOLOR_GETBLUE (m_pColors[nIndex]);
					a1 = A3DCOLOR_GETALPHA(m_pColors[nIndex]);
					r2 = A3DCOLOR_GETRED  (m_pColors[nIndex + 1]);
					g2 = A3DCOLOR_GETGREEN(m_pColors[nIndex + 1]);
					b2 = A3DCOLOR_GETBLUE (m_pColors[nIndex + 1]);
					a2 = A3DCOLOR_GETALPHA(m_pColors[nIndex + 1]);
					r = (int)(r1 * (1.0f - f) + r2 * f);
					g = (int)(g1 * (1.0f - f) + g2 * f);
					b = (int)(b1 * (1.0f - f) + b2 * f);
					a = (int)(a1 * (1.0f - f) + a2 * f); 
					return A3DCOLORRGBA(r, g, b, a);
				}
				else
				{
					A3DHSVCOLORVALUE hsvColor;
					A3DCOLOR		 rgbColor;
					
					hsvColor.h = m_pHSVColors[nIndex].h * (1.0f - f) + m_pHSVColors[nIndex + 1].h * f;
					hsvColor.s = m_pHSVColors[nIndex].s * (1.0f - f) + m_pHSVColors[nIndex + 1].s * f;
					hsvColor.v = m_pHSVColors[nIndex].v * (1.0f - f) + m_pHSVColors[nIndex + 1].v * f;
					hsvColor.a = m_pHSVColors[nIndex].a * (1.0f - f) + m_pHSVColors[nIndex + 1].a * f;
					a3d_HSVToRGB(hsvColor, &rgbColor);
					return rgbColor;
				}
			}
		}
	}
}

bool A3DParticleSystem::UpdateCommonParticle(COMMON_PARTICLE * pCommonParticle)
{
	-- pCommonParticle->nLife;
	++ pCommonParticle->nLive;

	pCommonParticle->vecSpeed.y -= m_vGravity / 900;
	pCommonParticle->vecPos = pCommonParticle->vecPos + pCommonParticle->vecSpeed;

	//Current we use color map according to particle's life;
	if( (pCommonParticle->nLive % 4) == 1 )
		pCommonParticle->color = GetParticleColor((FLOAT)(pCommonParticle->nLive));

	// If m_nSpinTime is not zero, we need to spin the particle a little;
	if( pCommonParticle->nSpinTime )
		pCommonParticle->vSpinPhase += 360.0f / pCommonParticle->nSpinTime;
	if( pCommonParticle->nBubblePeriod )
		pCommonParticle->vBubblePhase += 360.0f / pCommonParticle->nBubblePeriod;

	return true;
}

bool A3DParticleSystem::FillCommonParticle(COMMON_PARTICLE * pCommonParticle)
{
	// The particle's speed and dir must be filled by derived class, not by base class;
	// And the speed and dir must be filled before this function call;
	pCommonParticle->nLife = GetNextParticleLife();
	pCommonParticle->vSize = GetNextParticleSize();
	pCommonParticle->nSpinTime = GetNextParticleSpinTime();
	pCommonParticle->vSpinPhase = GetNextParticleSpinPhase();
	pCommonParticle->vecSpinAxis = GetNextParticleSpinAxis(pCommonParticle->vecDir);
	pCommonParticle->vBubblePhase = GetNextParticleBubblePhase();
	pCommonParticle->nBubblePeriod = GetNextParticleBubblePeriod();
	pCommonParticle->vBubbleAmplitude = GetNextParticleBubbleAmplitude();

	if( pCommonParticle->vBubbleAmplitude != 0.0f )
	{
		// We need vecUp in calculating the wobbing effects, calculate it here;
		A3DVECTOR3 vecAxis, vecLeft, vecUp;
		if( pCommonParticle->vecDir.x == 0.0f && pCommonParticle->vecDir.y == 1.0f && pCommonParticle->vecDir.z == 0.0f )
			vecAxis = A3DVECTOR3(1.0f, 0.0f, 0.0f); // X-Axis;
		else
			vecAxis = A3DVECTOR3(0.0f, 1.0f, 0.0f);
		vecLeft = Normalize(CrossProduct(pCommonParticle->vecDir, vecAxis));
		vecUp   = Normalize(CrossProduct(vecLeft, pCommonParticle->vecDir));

		// Now use a random rotated vecUp;
		A3DMATRIX4 mat = a3d_RotateAxis(pCommonParticle->vecPos, pCommonParticle->vecDir, RandFloat(A3D_PI, A3D_PI));
		vecUp = mat * vecUp - mat * A3DVECTOR3(0.0f);
		pCommonParticle->vecUp = vecUp * pCommonParticle->vBubbleAmplitude;
	}
	return true;
}

// This inline function is for GFXEditor only;
inline bool A3DParticleSystem::ChangeTextureMap(char * pszTextureMap)
{
	if( m_pA3DTexture )
	{
		// First release old texture here;
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pA3DTexture);
		m_pA3DTexture = NULL;
	}
	if( pszTextureMap )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFileInFolder(pszTextureMap, m_pA3DDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(), &m_pA3DTexture) )
			return false;
		strncpy(m_szTextureMap, pszTextureMap, MAX_PATH);
	}
	return true;
}

bool A3DParticleSystem::AddParticle(LPBYTE pParticle)
{
	// If too many particles, just ignore this add operation;
	if( m_nNumParticles >= m_nMaxParticles )
		return true;

	memcpy(m_pParticleBuffer + (m_nNumParticles ++) * m_nParticleSize, pParticle, m_nParticleSize);
	return true;
}

bool A3DParticleSystem::DeleteParticle(int nIndex)
{
	if( nIndex == m_nNumParticles - 1 )
	{
		// this is the last particle, just get rid of it;
		m_nNumParticles --;
	}
	else
	{
		memcpy(m_pParticleBuffer + nIndex * m_nParticleSize, m_pParticleBuffer + (m_nNumParticles - 1) * m_nParticleSize, m_nParticleSize);
		m_nNumParticles --;
	}
	return true;
}
