#include "StdAfx.h"




#ifdef A3D_PHYSX

#include "A3DPhysActor.h"
#include <APhysX.h>


A3DPhysActor::A3DPhysActor()
{
	pActorDesc	= new APhysXActorDesc;
	pMaterial	= new APhysXMaterialDesc;
}

A3DPhysActor::A3DPhysActor(const A3DPhysActor& rhs)
{
	pActorDesc	= rhs.pActorDesc->Clone();
	pMaterial	= rhs.pMaterial->Clone();

	int i, nShapesNum = rhs.GetShapeNum();
	for (i = 0 ; i < nShapesNum ; ++i) {
		AddShape(rhs.GetShapeDesc(i));
	}
}

A3DPhysActor::~A3DPhysActor()
{
	int j;

	for (j=0; j < aShapes.GetSize(); j++)
		delete aShapes[j];

	if (pActorDesc)
		delete pActorDesc;

	if (pMaterial)
		delete pMaterial;
}

A3DPhysActor* A3DPhysActor::Clone() const
{
	return new A3DPhysActor(*this);
}

A3DPhysActor& A3DPhysActor::operator = (const A3DPhysActor& rhs)
{
	if (&rhs == this)
		return *this;

	*pActorDesc = *rhs.pActorDesc;
	*pMaterial	= *rhs.pMaterial;

	int i, nShapesNum = aShapes.GetSize();
	for (i = 0 ; i < nShapesNum ; ++i) {
		delete aShapes[i];
	}
	aShapes.RemoveAll();

	int nDestShapesNum = rhs.GetShapeNum();
	for (i = 0 ; i < nDestShapesNum ; ++i) {
		AddShape(rhs.GetShapeDesc(i));
	}

	return *this;
}

int A3DPhysActor::AddShape(const APhysXShapeDesc* pShapeDesc)
{
	APhysXShapeDesc* pDst = pShapeDesc->Clone();
	if (!pDst)
		return -1;

	return aShapes.Add(pDst);
}

void A3DPhysActor::RemoveShape(int iIndex)
{
	if (iIndex >= 0 && iIndex < aShapes.GetSize())
	{
		delete aShapes[iIndex];
		aShapes.RemoveAt(iIndex);
	}
}


#endif	// end of A3D_PHYSX