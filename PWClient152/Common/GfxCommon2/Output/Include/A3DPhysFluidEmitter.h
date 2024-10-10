/*
 * FILE: A3DPhysFluidEmitter.h
 *
 * DESCRIPTION: Physical Fluid Emitter Implemented by Phys Fluid Emitter
 *
 * CREATED BY: ZhangYachuan, 2008/11/21
 *
 * HISTORY:
 *
 */

#ifndef _A3DPHYSFLUIDEMITTER_H_
#define _A3DPHYSFLUIDEMITTER_H_

#include "A3DPhysElement.h"

class NxVec3;
class NxQuat;
class APhysXFluidObject;
class APhysXObjectDesc;
class APhysXFluidObjectDesc;
class IGFXPhysSyncData;
class A3DPhysFluidSyncData;

class APhysXFluidObjectWrapper
{

public:
	explicit APhysXFluidObjectWrapper(APhysXFluidObject* pFluidObj) : 
	m_pFluidObj(pFluidObj) {}

public:

	int GetParticleNum() const;
	const NxVec3* GetParticlePositions() const;
	const NxVec3* GetParticleVelocities() const;
	const NxQuat* GetParticleDirections() const;
	const float* GetParticleLives() const;
	const float* GetParticleDensities() const;
	const unsigned int* GetParticleIDs() const;
	const unsigned int* GetParticleFlags() const;

	const NxVec3* GetParticleCollisionNormals() const;

	int GetCreatedIdNum() const;
	const unsigned int* GetCreatedParticleIds() const;
	int GetDeletedIdNum() const;
	const unsigned int* GetDeletedParticleIds() const;

	bool IsValid() const { return m_pFluidObj != NULL; }

private:
	APhysXFluidObject* m_pFluidObj;
};

class A3DPhysFluidEmitter : public A3DPhysElement
{
	// Types
	enum
	{
		FS_BEFORESTART,
		FS_PLAYING,
		FS_STOPPED,
	};

	// enable A3DPhysElement::CreatePhysElement to create this class's instance
	friend class A3DPhysElement;
	// Constructor / destructor
protected:
	// private ctor, so this class could only be constructed in A3DPhysElement::CreatePhysElement
	// and itself's clone member function
	explicit A3DPhysFluidEmitter(A3DGFXEx* pGfx);
	~A3DPhysFluidEmitter();

public:
	
	APhysXFluidEmitterParameters* GetFluidEmitterParam() const;
	APhysXFluidParameters* GetFluidParam() const;
	A3DPhysFluidSyncData* GetFluidPhysSyncData() const { return m_pFluidSyncData; }

	// Override operations
protected:
	// clone & operator =
	A3DGFXElement* Clone(A3DGFXEx* pGfx) const = 0;
	A3DPhysFluidEmitter& operator = (const A3DPhysFluidEmitter& src);

	// this pair of function is used for saving / loading the gfx properties
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual bool Init(A3DDevice* pDevice);

	// GFX element tick state
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool Play();
	virtual bool Stop();
	virtual void DummyTick(DWORD dwTick);
	virtual bool IsFinished() const;
	virtual bool CanRender() const;
	virtual bool StopParticleEmit();

	void OnActive(const A3DMATRIX4& mtInitPose);
	void OnFinish();
	void OnPhysEmitterFinish();
	bool PhysGetCurKeyPoint(KEY_POINT& kp);
		
	// Override when renaming the phys element
	virtual void SetName(const char* szName);

	// Create physics object desc
	virtual bool InitBind();
	// Create physics object
	virtual bool Bind(APhysXScene* pScene, const A3DMATRIX4& mtInitPose);
	// Release physics object
	virtual bool ReleasePhysXObject(APhysXScene* pScene);

	// Override (this interface is used by A3DGFXPhysDataMan to load / save data)
	virtual IGFXPhysSyncData* GetPhysSyncData() const;
	virtual bool SetPhysSyncData(const IGFXPhysSyncData* pPhysSyncData);

	// Private operations
private:
	
	void ReleaseOldPhys(APhysXScene* pScene);
	
	// Protected operations
protected:

	APhysXFluidEmitterAttacher* GetFluidEmitterAttacher() const { return m_pAttacher; }
	APhysXFluidObjectWrapper GetFluidObject() const;

protected:
	//	Phys Fluid Sync Data Used For Edit / Save / Load
	A3DPhysFluidSyncData* m_pFluidSyncData;

	// Attributes
private:
	APhysXFluidEmitterAttacher* m_pAttacher;
	int m_iState;
	KEY_POINT m_kpLast;

	// Phys Real data, Need to save
	// Will be saved by A3DGFXPhysDataMan not by Element self
	APhysXFluidObjectWrapper* m_pFluidObjWrapper;
};

unsigned char calcAlphaByTime(unsigned char ucBaseAlpha, float fCur, float fLife, float fFadeTime);

#endif
