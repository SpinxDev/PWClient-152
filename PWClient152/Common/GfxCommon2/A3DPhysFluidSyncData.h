/*
* FILE: A3DPhysFluidSyncData.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/04/27
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DPhysFluidSyncData_H_
#define _A3DPhysFluidSyncData_H_

#include <AString.h>
#include <A3DObject.h>
#include "A3DGFXPhysDataMan.h"
#include "A3DGFXEditorInterface.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class APhysXFluidEmitterAttacherDesc;
class APhysXFluidObjectDesc;
struct APhysXFluidParameters;
struct APhysXFluidEmitterParameters;

enum FluidObjectDescTypes
{
	FLUIDOBJ_NORMAL = 0,	// No phys orientation
	FLUIDOBJ_ORIENT,		// Fluid with phys orientation
	FLUIDOBJ_NUMBER,		// Total number
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class IPhysXFluidObjectDescCreator
//	
///////////////////////////////////////////////////////////////////////////

class IPhysXFluidObjectDescCreator
{
protected:
	explicit IPhysXFluidObjectDescCreator(FluidObjectDescTypes iType);
	virtual ~IPhysXFluidObjectDescCreator() = 0 {}

public:
	// Public interfaces
	static IPhysXFluidObjectDescCreator* CreateFluidObjDescCreatorByType(FluidObjectDescTypes iType);
	static IPhysXFluidObjectDescCreator* CreateFluidObjDescCreatorFromStream(NxStream* ps, DWORD dwVersion);
	static void StorePhysRBInfoToStream(NxStream* ps, FluidObjectDescTypes iType);
	static void DestroyFluidObjDescCreator(IPhysXFluidObjectDescCreator* pCreator);
	FluidObjectDescTypes GetFluidObjectType() const { return m_iFluidObjectDescType; }
	
public:
	// Override functions
	virtual int GetPropertyCount() const = 0;
	virtual GFX_PROPERTY GetProperty(int iIdx) const = 0;
	virtual AString GetPropertyName(int iIdx) const = 0;
	virtual void SetProperty(int iIdx, const GFX_PROPERTY& var) = 0;
	virtual APhysXFluidObjectDesc* Create() const = 0;
	virtual bool Load(NxStream* ps, DWORD dwVersion) = 0;
	virtual bool Save(NxStream* ps) = 0;
	virtual IPhysXFluidObjectDescCreator* Clone() const = 0;
private:
	FluidObjectDescTypes m_iFluidObjectDescType;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DNormalFluidObjectDescCreator
//	
///////////////////////////////////////////////////////////////////////////

class A3DNormalFluidObjectDescCreator : public IPhysXFluidObjectDescCreator
{
public:
	explicit A3DNormalFluidObjectDescCreator(FluidObjectDescTypes iType);
	~A3DNormalFluidObjectDescCreator() {}
protected:
	virtual int GetPropertyCount() const;
	virtual GFX_PROPERTY GetProperty(int iIdx) const;
	virtual AString GetPropertyName(int iIdx) const;
	virtual void SetProperty(int iIdx, const GFX_PROPERTY& var);
	virtual APhysXFluidObjectDesc* Create() const;
	virtual bool Load(NxStream* ps, DWORD dwVersion);
	virtual bool Save(NxStream* ps);
	virtual IPhysXFluidObjectDescCreator* Clone() const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DOrientFluidObjectDescCreator
//	
///////////////////////////////////////////////////////////////////////////

class A3DOrientFluidObjectDescCreator : public IPhysXFluidObjectDescCreator
{
public:
	explicit A3DOrientFluidObjectDescCreator(FluidObjectDescTypes iType);
	~A3DOrientFluidObjectDescCreator() {}
	A3DOrientFluidObjectDescCreator(const A3DOrientFluidObjectDescCreator& rhs);

protected:
	virtual int GetPropertyCount() const;
	virtual GFX_PROPERTY GetProperty(int iIdx) const;
	virtual AString GetPropertyName(int iIdx) const;
	virtual void SetProperty(int iIdx, const GFX_PROPERTY& var);
	virtual APhysXFluidObjectDesc* Create() const;
	virtual bool Load(NxStream* ps, DWORD dwVersion);
	virtual bool Save(NxStream* ps);
	virtual IPhysXFluidObjectDescCreator* Clone() const;

private:
	// For parameter: APhysXOrientedParticleSpinType mSpinType;
	int m_iSpinType;
	// For parameter: AphysXOrientedParticleInitialOrt mInitialOrt;
	int m_iInitialOrt;
	// max allowed angular velocity of particle, unit is degree.
	float m_fMaxAngularVel;
	// the spin radius of particle, only for case of APX_OP_SPINTYPE_SPHERE
	float m_fSpinRadius;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DPhysFluidSyncData
//	
///////////////////////////////////////////////////////////////////////////

class A3DPhysFluidSyncData : public IGFXPhysSyncData
{

public:		//	Types

	friend class A3DPhysFluidEmitter;

public:		//	Constructor and Destructor

	A3DPhysFluidSyncData(void);
	A3DPhysFluidSyncData(const A3DPhysFluidSyncData&);
	virtual ~A3DPhysFluidSyncData(void);

public:		//	Attributes

public:		//	Operations

	// Create the actual phys object desc from all editable data
	// In game, we call InitBind 
	bool InitBind();

	APhysXFluidParameters* GetFluidParameters() const { return m_pFluidParameters; }
	APhysXFluidEmitterParameters* GetFluidEmitterParameters() const { return m_pFluidEmitterParameters; }

	void SetFluidObjectDescCreator(IPhysXFluidObjectDescCreator* pCreator);
	IPhysXFluidObjectDescCreator* GetFluidObjectDescCreator() const { return m_pFluidObjectDescCreator; }
	FluidObjectDescTypes GetFluidObjectDescType() const { return m_pFluidObjectDescCreator->GetFluidObjectType(); }

protected:	//	Attributes

	APhysXFluidParameters* m_pFluidParameters;
	APhysXFluidEmitterParameters* m_pFluidEmitterParameters;
	IPhysXFluidObjectDescCreator* m_pFluidObjectDescCreator;
	APhysXFluidEmitterAttacherDesc* m_pAttacherDesc;

protected:	//	Operations

	void SetInitParameters();
	//	Override functions
	virtual IGFXPhysSyncData* Clone() const;
	virtual bool Load(NxStream* ps, DWORD dwVersion);
	virtual bool Save(NxStream* ps);

	APhysXFluidEmitterAttacherDesc* GetFluidEmitterAttacherDesc() const { return m_pAttacherDesc; }

	//	Disable operations
	A3DPhysFluidSyncData& operator = (const A3DPhysFluidSyncData&);

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DPhysFluidSyncData_H_


