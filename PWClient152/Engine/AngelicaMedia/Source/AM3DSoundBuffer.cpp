/*
 * FILE: AM3DSoundBuffer.cpp
 *
 * DESCRIPTION: 3d sound buffer class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AM3DSoundBuffer.h"
#include "AMPI.h"
#include "AMSoundEngine.h"
#include "AMSoundBufferMan.h"
#include "AM3DSoundDevice.h"
#include "A3DFuncs.h"

#define MAX_ATTENUATION		10

AM3DSoundBuffer::AM3DSoundBuffer()
{
	m_dwTypeID		= AMSNDBUF_3D;
	m_pDS3DBuffer8	= NULL;
	m_bForce2D		= false;
}

AM3DSoundBuffer::~AM3DSoundBuffer()
{
}

bool AM3DSoundBuffer::Create(AMSoundEngine* pAMSoundEngine, AMSBUFFERDESC * pDesc)
{
	pDesc->dwFlags |= DSBCAPS_CTRL3D;
	pDesc->algorithm = AMS3DALG_DEFAULT;
	
	if( !AMSoundBuffer::Create(pAMSoundEngine, pDesc) )
	{		  
		AMERRLOG(("AM3DSoundBuffer::Create, Create the sound buffer fail!"));
		return false;
	}

	// If we did not create the direct sound object;
	if( NULL == pAMSoundEngine->GetDS() )
		return true;

	HRESULT hval;

	hval = m_pDSBuffer8->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID *) &m_pDS3DBuffer8);
	if (FAILED(hval))
	{
		AMERRLOG(("AM3DSoundBuffer::Create, Query 3D Buffer interface fail!"));
		return false;
	}

	if( !RetrieveParamsFromDS3DBuffer() )
	{
		AMERRLOG(("AM3DSoundBuffer::Create, Call RetrieveParamsFromDS3DBuffer fail!"));
		return false;
	}
	return true;
}

bool AM3DSoundBuffer::Release()
{
	if( m_pDS3DBuffer8 )
	{
		m_pDS3DBuffer8->Release();
		m_pDS3DBuffer8 = NULL;
	}

	return AMSoundBuffer::Release();
}

bool AM3DSoundBuffer::UnloadSoundData()
{
	if( m_pDS3DBuffer8 )
	{
		m_pDS3DBuffer8->Release();
		m_pDS3DBuffer8 = NULL;
	}

	return AMSoundBuffer::UnloadSoundData();
}

bool AM3DSoundBuffer::UpdateChanges()
{
	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	/*
	if( GetAsyncKeyState(VK_SPACE) & 0x8000 )
	{
		DWORD dwFrequency;
		m_pDSBuffer8->SetFrequency(DSBFREQUENCY_ORIGINAL);
		m_pDSBuffer8->GetFrequency(&dwFrequency);
		m_pDSBuffer8->SetFrequency(dwFrequency / 4);
	}
	else if( GetAsyncKeyState(VK_RETURN) & 0x8000 )
	{
		m_pDSBuffer8->SetFrequency(DSBFREQUENCY_ORIGINAL);
	}*/

	if( m_bForce2D )
	{
		// We shall not adjust the sound buffer's properties, because it is a 2D buffer now
		return true;
	}

	if( !m_pDS3DBuffer8 )
		return true;

	DS3DBUFFER	ds3dbuffer;

	ZeroMemory(&ds3dbuffer, sizeof(ds3dbuffer));
	ds3dbuffer.dwSize				= sizeof(ds3dbuffer);
	ds3dbuffer.vPosition			= *(D3DVECTOR *)&m_3dbufferDesc.vecPos;
	ds3dbuffer.vVelocity			= *(D3DVECTOR *)&m_3dbufferDesc.vecVel;
	ds3dbuffer.dwInsideConeAngle	= m_3dbufferDesc.dwInsideConeAngle; 
	ds3dbuffer.dwOutsideConeAngle	= m_3dbufferDesc.dwOutsideConeAngle;
	ds3dbuffer.vConeOrientation		= *(D3DVECTOR *)&m_3dbufferDesc.vecConeOrientation; 
    ds3dbuffer.lConeOutsideVolume	= m_3dbufferDesc.lConeOutsideVolume; 
    ds3dbuffer.flMinDistance		= m_3dbufferDesc.vMinDistance; 
    ds3dbuffer.flMaxDistance		= m_3dbufferDesc.vMaxDistance; 
    ds3dbuffer.dwMode				= m_3dbufferDesc.dwMode; 

	HRESULT hval;
	// Here we should use DS3D_DEFFERED for setting the params, and
	// use listener's commitdeferredsettings to let the changes appear;
	hval = m_pDS3DBuffer8->SetAllParameters(&ds3dbuffer, DS3D_IMMEDIATE);
	if( DS_OK != hval )
	{
		AMERRLOG(("AM3DSoundBuffer::UpdateChanges(), Set DS3DBuffer's parameters fail!"));
		return false;
	}

	return true;
}

bool AM3DSoundBuffer::RetrieveParamsFromDS3DBuffer()
{
	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	HRESULT			hval;
	DS3DBUFFER		ds3dbuffer;

	ZeroMemory(&ds3dbuffer, sizeof(ds3dbuffer));
	ds3dbuffer.dwSize = sizeof(ds3dbuffer);

	hval = m_pDS3DBuffer8->GetAllParameters(&ds3dbuffer);
	if( DS_OK != hval )
	{
		AMERRLOG(("AM3DSoundBuffer::RetrieveParamsFromDS3DBuffer(), Get DS3DBuffer's parameters fail!"));
		return false;
	}

	m_3dbufferDesc.vecPos				= *(A3DVECTOR3 *)&ds3dbuffer.vPosition;
	m_3dbufferDesc.vecVel				= *(A3DVECTOR3 *)&ds3dbuffer.vVelocity;
	m_3dbufferDesc.dwInsideConeAngle	= ds3dbuffer.dwInsideConeAngle;
	m_3dbufferDesc.dwOutsideConeAngle	= ds3dbuffer.dwOutsideConeAngle;
	m_3dbufferDesc.vecConeOrientation	= *(A3DVECTOR3 *)&ds3dbuffer.vConeOrientation; 
    m_3dbufferDesc.lConeOutsideVolume	= ds3dbuffer.lConeOutsideVolume;
    m_3dbufferDesc.vMinDistance			= ds3dbuffer.flMinDistance; 
    m_3dbufferDesc.vMaxDistance			= ds3dbuffer.flMaxDistance; 
    m_3dbufferDesc.dwMode				= ds3dbuffer.dwMode; 
	return true;
}

bool AM3DSoundBuffer::Play(bool bLoop)
{
	return AMSoundBuffer::Play(bLoop);
}

bool AM3DSoundBuffer::Stop()
{
	return AMSoundBuffer::Stop();
}

bool AM3DSoundBuffer::Load(AMSoundEngine* pAMSoundEngine, AFile* pFileToLoad, const char* szFileName)
{
	return AMSoundBuffer::Load(pAMSoundEngine, pFileToLoad, szFileName);
}

bool AM3DSoundBuffer::SetForce2D(bool bForce2D)
{
	// See if the flag has changed
	if( m_bForce2D == bForce2D )
		return true;

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
	{
		m_bForce2D = bForce2D;
		return true;
	}

	HRESULT		hval;

	if( !bForce2D )
	{
		// Now we should restore the 3d properties;
		hval = m_pDS3DBuffer8->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);
		if (FAILED(hval)) 
		{
			AMERRLOG(("AM3DSoundBuffer::SetForce2D(), SetMode Normal fail!"));
			return false;
		}
		m_bForce2D = false;
		UpdateChanges();
	}
	else
	{
		hval = m_pDS3DBuffer8->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);
		if (FAILED(hval)) 
		{
			AMERRLOG(("AM3DSoundBuffer::SetForce2D(), SetMode Disable fail!"));
			return false;
		}
		m_bForce2D = true;
	}

	return true;
}

bool AM3DSoundBuffer::CanHear()
{
	if( !m_pAMSoundEngine->GetAM3DSoundDevice() )
		return true;

	A3DVECTOR3 vecEarPos = m_pAMSoundEngine->GetAM3DSoundDevice()->GetPosition();

	A3DVECTOR3 vecDis = m_3dbufferDesc.vecPos - vecEarPos;
	FLOAT vDis = vecDis.Magnitude();

	if( vDis > m_3dbufferDesc.vMaxDistance )
	{
		// For we will not hear some sound if it has been attenuated to 1/10 
		// of the full volume;
		if( m_3dbufferDesc.vMaxDistance > m_3dbufferDesc.vMinDistance * MAX_ATTENUATION )
			return false;
	}
	else
		return true;
	return true;
}

bool AM3DSoundBuffer::Reload(AFile * pFileToLoad)
{
	// first of all, we must keep the old buffer desc
	AMS3DBUFFERDESC cur_desc = m_3dbufferDesc;

	// first call baseclass's reload function;
	AMSoundBuffer::Reload(pFileToLoad);

	if( m_bForce2D )
	{
		m_bForce2D = false;
		SetForce2D(true);
	}

	m_3dbufferDesc = cur_desc;
	// now set the 3d parameters again
	UpdateChanges();

	return true;
}
