/*
 * FILE: A3DSnow.cpp
 *
 * DESCRIPTION: Snow system
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DSnow.h"
#include "A3DDevice.h"
#include "A3DCamera.h"
#include "AFile.h"

A3DSnow::A3DSnow()
{
	m_dwClassID			= A3D_CID_SNOW;
	m_pA3DDevice		= NULL;
	m_pHostCamera		= NULL;

	m_vEmitterCenterX	= 0.0f;
	m_vEmitterCenterZ	= 0.0f;
	m_vEmitterWidth		= 150.0f;
	m_vEmitterLength	= 150.0f;
	m_vEmitterHeight	= 60.0f;
}

A3DSnow::~A3DSnow()
{
}

bool A3DSnow::EmitParticles()
{
	// First follow my host camera;
	A3DVECTOR3 vecCamPos(0.0f);

	if( m_pHostCamera )
	{
		vecCamPos = m_pHostCamera->GetPos();
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

		FLOAT vX, vY, vZ;
		vX = GetNextParticleXOffset() + vecCamPos.x;
		vY = vecCamPos.y + m_vEmitterHeight;
		vZ = GetNextParticleZOffset() + vecCamPos.z;

		// We let each particle has its own position in world space;
		pNewCommonParticle->vecPos	 = A3DVECTOR3(vX, vY, vZ);
		pNewCommonParticle->vecDir   = A3DVECTOR3(0.0f, -1.0f, 0.0f);
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

bool A3DSnow::MakeDead(LPVOID pParticle)
{
	return true;
}

bool A3DSnow::Save(AFile * pFileToSave)
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

bool A3DSnow::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
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

bool A3DSnow::Init(A3DDevice * pA3DDevice, FLOAT vAmplitude)
{
	m_noise.Init(256, vAmplitude, 64, 0.3f, 2, 0x12345);
	return A3DParticleSystem::Init(pA3DDevice);
}

bool A3DSnow::Release()
{
	m_noise.Release();
	return A3DParticleSystem::Release();
}

bool A3DSnow::CreateSnow()
{
	return CreateStandard();
}

bool A3DSnow::StartSnow()
{
	return Start();
}

bool A3DSnow::UpdateStandardParticles()
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
			A3DVECTOR3 vecNoise;
			m_noise.GetValue(i + m_nTicks * 1.0f, vecNoise.m, 3);
			vecNoise.y = 0.0f;
			
			pParticle->common.vecPos = pParticle->common.vecPos + vecNoise * 0.5f;
			UpdateCommonParticle(&pParticle->common);
			++pParticle;
		}
	}

	return true;
}