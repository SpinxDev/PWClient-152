/*
 * FILE: AM3DSoundDevice.cpp
 *
 * DESCRIPTION: 3d sound listener class
 *
 * CREATED BY: Hedi, 2002/1/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AM3DSoundDevice.h"
#include "AMPI.h"
#include "AMSoundEngine.h"
#include "AMSoundDevice.h"

AM3DSoundDevice::AM3DSoundDevice()
{
	m_pAMSoundEngine	= NULL;
	m_pDS3DListener8	= NULL;

	m_dwFlags			= DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;		
}

AM3DSoundDevice::~AM3DSoundDevice()
{
}

bool AM3DSoundDevice::Init(PAMSoundEngine pAMSoundEngine, AMSOUNDDEVFORMAT * pDevFormat)
{
	m_pAMSoundEngine = pAMSoundEngine;

	pDevFormat->algorithm = AMS3DALG_DEFAULT;
	if( !AMSoundDevice::Init(pAMSoundEngine, pDevFormat) )
	{
		AMERRLOG(("AM3DSoundDevice::Init, Call AMSoundDevice::Init Fail!"));
		return false;
	}

	// If we did not create the direct sound object;
	if( NULL == pAMSoundEngine->GetDS() )
		return true;

	IDirectSoundBuffer * pPrimaryBuffer = GetPrimaryBuffer();

	HRESULT hval;
	hval = pPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID *)&m_pDS3DListener8);
	if (FAILED(hval))
	{
		AMERRLOG(("AM3DSoundDevice::Init, Query IID_IDirect3DListener8 Interface Fail!"));
		return false;
	}

	if( !RetrieveParameters() )
	{
		AMERRLOG(("AM3DSoundDevice::Init, Call RetrieveParameters Fail!"));
		return false;
	}
	return true;
}

bool AM3DSoundDevice::Release()
{
	if( m_pDS3DListener8 )
	{
		m_pDS3DListener8->Release();
		m_pDS3DListener8 = NULL;
	}

	return AMSoundDevice::Release();
}

bool AM3DSoundDevice::UpdateChanges()
{
	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	HRESULT hval;

	DS3DLISTENER ds3dlistener;
	ZeroMemory(&ds3dlistener, sizeof(ds3dlistener));
	ds3dlistener.dwSize				= sizeof(ds3dlistener);
	ds3dlistener.vPosition			= *(D3DVECTOR *)&m_3dlistenerDesc.vecPos; 
    ds3dlistener.vVelocity			= *(D3DVECTOR *)&m_3dlistenerDesc.vecVel; 
    ds3dlistener.vOrientFront		= *(D3DVECTOR *)&m_3dlistenerDesc.vecOrientFront; 
    ds3dlistener.vOrientTop			= *(D3DVECTOR *)&m_3dlistenerDesc.vecOrientTop; 
    ds3dlistener.flDistanceFactor	= m_3dlistenerDesc.vDistanceFactor; 
    ds3dlistener.flRolloffFactor	= m_3dlistenerDesc.vRolloffFactor; 
    ds3dlistener.flDopplerFactor	= m_3dlistenerDesc.vDopplerFactor;
	
	hval = m_pDS3DListener8->SetAllParameters(&ds3dlistener, DS3D_IMMEDIATE);
	if( hval != DS_OK )
	{
		AMERRLOG(("AM3DSoundDevice::UpdateChanges, Call DS3DListener's SetAllParameters fail!"));
		return false;
	}
	return true;
}

bool AM3DSoundDevice::RetrieveParameters()
{
	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	HRESULT hval;
	DS3DLISTENER	ds3dlistener;
	ZeroMemory(&ds3dlistener, sizeof(ds3dlistener));
	ds3dlistener.dwSize = sizeof(ds3dlistener);

	hval = m_pDS3DListener8->GetAllParameters(&ds3dlistener);
	if( hval != DS_OK )
	{
		AMERRLOG(("AM3DSoundDevice::RetrieveParameters, Get DS Listener's parameter fail!"));
		return false;
	}

	m_3dlistenerDesc.vecPos = *(A3DVECTOR3 *)&ds3dlistener.vPosition;
    m_3dlistenerDesc.vecVel = *(A3DVECTOR3 *)&ds3dlistener.vVelocity;
    m_3dlistenerDesc.vecOrientFront = *(A3DVECTOR3 *)&ds3dlistener.vOrientFront;
    m_3dlistenerDesc.vecOrientTop = *(A3DVECTOR3 *)&ds3dlistener.vOrientTop;
    m_3dlistenerDesc.vDistanceFactor = ds3dlistener.flDistanceFactor;
    m_3dlistenerDesc.vRolloffFactor = ds3dlistener.flRolloffFactor;
    m_3dlistenerDesc.vDopplerFactor = ds3dlistener.flDopplerFactor;
	return true;
}
