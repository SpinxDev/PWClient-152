#ifndef _A3DPHYSELEMENT_H_
#define _A3DPHYSELEMENT_H_

#include "A3DGFXElement.h"
#include "A3DGFXEx.h"

class A3DGFXPhysDataMan;
class A3DGFXEx;
class APhysXObjectDescManager;
class APhysXScene;
class APhysXFluidEmitterAttacher;
class APhysXFluidEmitterAttacherDesc;
class APhysXFluidObject;
class APhysXUserGraphicsSync;
class APhysXObjectDesc;
class APhysXEngine;
struct APhysXFluidEmitterParameters;
struct APhysXFluidParameters;
class IGFXPhysSyncData;


class A3DPhysElement : public A3DGFXElement
{
private:
	// Pointer to the PhysX Object Desc Manager (which is contained in the A3DGFXEx)
	A3DGFXPhysDataMan* m_pPhysDataMan;

protected:
	// Only should be Used / Override by Real GFX Phys Elements
	A3DGFXPhysDataMan* GetPhysObjDescMan() const { return m_pPhysDataMan; }
// 	virtual bool CreateGFXPhysObject() = 0;
// 	virtual APhysXObjectDesc* Inner_GetObjectDesc() = 0;
// 	virtual void Innder_SetObjectDesc(const APhysXObjectDesc* pDesc) = 0;
	virtual bool SetPhysSyncData(const IGFXPhysSyncData* pPhysSyncData) = 0;

public:
	explicit A3DPhysElement(A3DGFXEx* pGfx);

public:
//	APhysXObjectDesc* GetObjectDesc() { return Inner_GetObjectDesc(); }
	
	// Subclass need to override this function to provide physics sync data
	virtual IGFXPhysSyncData* GetPhysSyncData() const = 0;
	
	// Check the phys engine and decide whether to create the phys element
	static A3DPhysElement* CreatePhysElement(A3DGFXEx* pGfx, int nEleId);

	// Register the PhysX Desc Objects for load and save
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion) = 0;
	virtual bool Save(AFile* pFile) = 0;

	virtual bool InitBind() = 0;
	virtual bool Bind(APhysXScene* pScene, const A3DMATRIX4& mtInitPose) = 0;
	virtual bool ReleasePhysXObject(APhysXScene* pScene) = 0;

// 	// Called by User After Each time Finished Editing
// 	// Create Phys Sync Data
// 	bool InitBind() { return CreateGFXPhysObject(); }
	
};

#endif