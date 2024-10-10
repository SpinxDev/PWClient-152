/*
 * FILE: A3DPhysActor.h
 *
 * DESCRIPTION: 物理Actor数据基类，提取出了
 * A3DModelPhysSyncData::ACTOR_DATA和A3DSkinPhysSyncData::ACTOR_DATA共有的数据
 *
 * CREATED BY: Duyuxin, 2008/6/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */


#ifndef _A3DPHYSACTOR_H_
#define _A3DPHYSACTOR_H_

#include <AArray.h>
#include <AString.h>

class APhysXShapeDesc;
class APhysXActorDesc;
class APhysXMaterialDesc;

struct A3DPhysActor
{
protected:
	
	AString				strName;		//	Actor name
	APhysXActorDesc*	pActorDesc;		//	Actor desc
	APhysXMaterialDesc*	pMaterial;		//	Actor material

	APtrArray<APhysXShapeDesc*>		aShapes;	//	Actor shape array

public:
	
	A3DPhysActor();
	A3DPhysActor(const A3DPhysActor& rhs);
	virtual ~A3DPhysActor();

	//	Clone Actor Data
	virtual A3DPhysActor* Clone() const;
	A3DPhysActor& operator = (const A3DPhysActor& rhs);

	//	Get actor name
	const char* GetName() const { return strName; }
	//	Add shape to a actor.
	int AddShape(const APhysXShapeDesc* pShapeDesc);
	//	Remove a actor shape
	void RemoveShape(int iIndex);
	//	Get shape number
	int GetShapeNum() const { return aShapes.GetSize(); }
	//	Get shape desc
	APhysXShapeDesc* GetShapeDesc(int n) const { return aShapes[n]; }

	//	Get actor desc
	APhysXActorDesc* GetActorDesc() const { return pActorDesc; }
	//	Get material desc
	APhysXMaterialDesc* GetMaterialDesc() const { return pMaterial; }
};


#endif