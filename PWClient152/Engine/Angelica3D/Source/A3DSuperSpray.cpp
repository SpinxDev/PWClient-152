/*
 * FILE: A3DSuperSpray.cpp
 *
 * DESCRIPTION: Particle System, which use off axis and off plance
		to determine the start position and speed of the particle, this is infact a 
		SUPER SPRY Particle System in 3DS MAX for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DSuperSpray.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DModel.h"
#include "AFile.h"

A3DSuperSpray::A3DSuperSpray()
{
	m_dwClassID			= A3D_CID_SUPERSPRAY;
	m_pA3DDevice		= NULL;
	m_vOffAxis			= 0.0f;
	m_vOffAxisSpread	= 10.0f;
	m_vOffPlane			= 0.0f;
	m_vOffPlaneSpread	= 180.0f;
}

A3DSuperSpray::~A3DSuperSpray()
{
}

bool A3DSuperSpray::Release()
{
	return A3DParticleSystem::Release();
}

bool A3DSuperSpray::EmitParticles()
{
	int		n;
	STANDARD_PARTICLE StandardParticle;
	INSTANCED_GEOMETRY_PARTICLE InstancedParticle;

	switch(m_ParticleType)
	{
	case A3DPARTICLE_STANDARD_PARTICLES:
		int nToNew;
		nToNew = GetNumberToNew();
		
		if( m_vInheritInfluence != 0.0f && m_nTicks <= 1 )
			break;

		for(n=0; n<nToNew; n++)
		{
			COMMON_PARTICLE * pNewCommonParticle;

			ZeroMemory(&StandardParticle, sizeof(STANDARD_PARTICLE));
			pNewCommonParticle = &StandardParticle.common;

			FLOAT vOffAxis, vOffPlane;
			vOffAxis = GetNextParticleOffAxis();
			vOffPlane = GetNextParticleOffPlane();
			//Construct the rotation matrix here;
			A3DMATRIX4 matEmitDir = a3d_RotateX(DEG2RAD(vOffAxis - m_vOffAxis)) * a3d_RotateZ(DEG2RAD(vOffPlane)) * a3d_RotateX(DEG2RAD(m_vOffAxis));
			
			// We let each particle has its own position in world space;
			pNewCommonParticle->vecPos	 = m_EmitterAbsoluteTM.GetRow(3) - m_EmitterAbsoluteVelocity * (1.0f * n / nToNew);
			pNewCommonParticle->vecDir   = A3DParticleSystem::m_EmittingAxis * matEmitDir * m_EmitterAbsoluteTM - m_EmitterAbsoluteTM.GetRow(3);
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
		break;

	case A3DPARTICLE_META_PARTICLES:
		g_A3DErrLog.Log("Not implemented now!");
		return false;

	case A3DPARTICLE_OBJECT_FRAGMENTS:
		g_A3DErrLog.Log("Super spray can not work with object fragments!");
		return false;

	case A3DPARTICLE_INSTANCED_GEOMETRY:
		// We only emit at the very start frame;
		if( m_nTicks != m_nEmitStart )
			return true;

		for(n=0; n<m_nInstancedModelNum; n++)
		{
			COMMON_PARTICLE * pNewCommonParticle;

			ZeroMemory(&InstancedParticle, sizeof(INSTANCED_GEOMETRY_PARTICLE));
			pNewCommonParticle = &InstancedParticle.common;

			FLOAT vOffAxis, vOffPlane;
			vOffAxis = GetNextParticleOffAxis();
			vOffPlane = GetNextParticleOffPlane();
			//Construct the rotation matrix here;
			A3DMATRIX4 matEmitDir = a3d_RotateX(DEG2RAD(vOffAxis - m_vOffAxis)) * a3d_RotateZ(DEG2RAD(vOffPlane)) * a3d_RotateX(DEG2RAD(m_vOffAxis));
			
			// We let each particle has its own position in world space;
			pNewCommonParticle->vecPos	 = m_EmitterAbsoluteTM.GetRow(3) - m_EmitterAbsoluteVelocity;
			pNewCommonParticle->vecDir   = A3DParticleSystem::m_EmittingAxis * matEmitDir * m_EmitterAbsoluteTM - m_EmitterAbsoluteTM.GetRow(3);
			pNewCommonParticle->vecSpeed = GetNextParticleSpeedValue() * pNewCommonParticle->vecDir;

			if( m_vInheritInfluence != 0.0f && m_nTicks > 1 )
			{
				pNewCommonParticle->vecSpeed = pNewCommonParticle->vecSpeed + 
					GetNextParticleInheritInfluence() * 0.01f * m_vInheritMultiplier * m_EmitterAbsoluteVelocity;
			}

			// Fill these common parameters;
			FillCommonParticle(pNewCommonParticle);

			if( m_pInstancedModels[n]->GetContainer() == A3DCONTAINER_NULL )
			{
				m_pA3DDevice->GetA3DEngine()->AddModel(m_pInstancedModels[n], NULL);
			}

			InstancedParticle.pModel = m_pInstancedModels[n];
			m_pInstancedModels[n]->SetPos(pNewCommonParticle->vecPos);
			
			m_pInstancedModels[n]->RestartGFXEvents();
			m_pInstancedModels[n]->RestartSFXEvents();
			AddParticle((LPBYTE)&InstancedParticle);
		}
		break;
	}
	return true;
}

bool A3DSuperSpray::MakeDead(LPVOID pParticle)
{
	switch( m_ParticleType )
	{
	case A3DPARTICLE_STANDARD_PARTICLES:
		break;
	case A3DPARTICLE_META_PARTICLES:
		g_A3DErrLog.Log("Not implemented now!");
		return false;
	case A3DPARTICLE_OBJECT_FRAGMENTS:
		g_A3DErrLog.Log("Super spray can not work with object fragments!");
		return false;
	case A3DPARTICLE_INSTANCED_GEOMETRY:
		INSTANCED_GEOMETRY_PARTICLE * pInstanced = (INSTANCED_GEOMETRY_PARTICLE *) pParticle;
		A3DModel * pInstancedModel = pInstanced->pModel;
		pInstancedModel->StopAllGFX();
		pInstancedModel->StopAllSFX();
		if( pInstancedModel->GetContainer() == A3DCONTAINER_NULL )
		{
		}
		else
		{
			m_pA3DDevice->GetA3DEngine()->RemoveModel(pInstancedModel);
		}
		break;
	}
	return true;
}

bool A3DSuperSpray::Save(AFile * pFileToSave)
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

		sprintf(szLineBuffer, "OffAxis: %f", m_vOffAxis);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "OffAxisSpread: %f", m_vOffAxisSpread);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "OffPlane: %f", m_vOffPlane);
		pFileToSave->WriteLine(szLineBuffer);
		sprintf(szLineBuffer, "OffPlaneSpread: %f", m_vOffPlaneSpread);
		pFileToSave->WriteLine(szLineBuffer);
	}

	return true;
}

bool A3DSuperSpray::Load(A3DDevice * pA3DDevice, AFile * pFileToLoad)
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
		sscanf(szLineBuffer, "OffAxis: %f", &m_vOffAxis);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "OffAxisSpread: %f", &m_vOffAxisSpread);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "OffPlane: %f", &m_vOffPlane);
		pFileToLoad->ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLineBuffer, "OffPlaneSpread: %f", &m_vOffPlaneSpread);
	}

	return true;	
}