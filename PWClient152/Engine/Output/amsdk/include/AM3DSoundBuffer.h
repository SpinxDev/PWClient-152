/*
 * FILE: AM3DSoundBuffer.h
 *
 * DESCRIPTION: 3d sound buffer class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AM3DSOUNDBUFFER_H_
#define _AM3DSOUNDBUFFER_H_

#include "AMPlatform.h"
#include "AMTypes.h"
#include "AMSoundBuffer.h"

class AM3DSoundBuffer : public AMSoundBuffer
{
	friend class AMSoundBufferMan;

private:
	AMS3DBUFFERDESC				m_3dbufferDesc;	
	IDirectSound3DBuffer8*		m_pDS3DBuffer8;
	bool						m_bForce2D;

protected:
	bool RetrieveParamsFromDS3DBuffer();

public:
	AM3DSoundBuffer();
	~AM3DSoundBuffer();

protected:
	virtual bool Create(AMSoundEngine* pAMSoundEngine, AMSBUFFERDESC * pDesc);
	virtual bool Release();

public:
	
	// Use this method to free the sound resource when it will not used for sometime;
	virtual bool UnloadSoundData();

	virtual bool Play(bool bLoop);
	virtual bool Stop();
	bool SetForce2D(bool bForce2D);

	bool CanHear();

	virtual bool Load(AMSoundEngine* pAMSoundEngine, AFile* pFileToLoad, const char* szFileName);
	virtual bool Reload(AFile * pFileToLoad);

	// Functions used to set parameters of the buffer;
	inline void SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle);
	inline void SetConeOrientation(A3DVECTOR3& vecConeOrientation);
	inline void SetConeOutsideVolume(LONG lConeOutsideVolume);
	inline void SetMaxDistance(FLOAT vMaxDistance);
	inline void SetMinDistance(FLOAT vMinDistance);
	inline void SetMode(DWORD dwMode);
	inline void SetPosition(const A3DVECTOR3& vecPos);
	inline void SetVelocity(const A3DVECTOR3& vecVel);

	// Make the changes to the buffer's params be activated;
	bool UpdateChanges();
	
	// inline functions to get parameters of the buffer;
	// ...
	inline bool IsForce2D() { return m_bForce2D; }
	inline IDirectSound3DBuffer8* GetDSound3DBuffer() { return m_pDS3DBuffer8; }
	AMS3DBUFFERDESC* Get3DBufferDesc() { return &m_3dbufferDesc; }
};

void AM3DSoundBuffer::SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle)
{
	m_3dbufferDesc.dwInsideConeAngle = dwInsideConeAngle;
	m_3dbufferDesc.dwOutsideConeAngle = dwOutsideConeAngle;
}

void AM3DSoundBuffer::SetConeOrientation(A3DVECTOR3& vecConeOrientation)
{
	m_3dbufferDesc.vecConeOrientation = vecConeOrientation;
}

void AM3DSoundBuffer::SetConeOutsideVolume(LONG lConeOutsideVolume)
{
	m_3dbufferDesc.lConeOutsideVolume = lConeOutsideVolume;
}

void AM3DSoundBuffer::SetMaxDistance(FLOAT vMaxDistance)
{
	if( vMaxDistance < m_3dbufferDesc.vMinDistance + 0.1f )
		vMaxDistance = m_3dbufferDesc.vMinDistance + 0.1f;
	m_3dbufferDesc.vMaxDistance = vMaxDistance;
}

void AM3DSoundBuffer::SetMinDistance(FLOAT vMinDistance)
{
	if( vMinDistance < 0.1f )
		vMinDistance = 0.1f;
	if( vMinDistance > m_3dbufferDesc.vMaxDistance - 0.1f )
		vMinDistance = m_3dbufferDesc.vMaxDistance - 0.1f;
	m_3dbufferDesc.vMinDistance = vMinDistance;
}

void AM3DSoundBuffer::SetMode(DWORD dwMode)
{
	m_3dbufferDesc.dwMode = dwMode;
}

void AM3DSoundBuffer::SetPosition(const A3DVECTOR3& vecPos)
{
	m_3dbufferDesc.vecPos = vecPos;
}

void AM3DSoundBuffer::SetVelocity(const A3DVECTOR3& vecVel)
{
	m_3dbufferDesc.vecVel = vecVel;
}

typedef class AM3DSoundBuffer * PAM3DSoundBuffer;
#endif//_AM3DSOUNDBUFFER_H_