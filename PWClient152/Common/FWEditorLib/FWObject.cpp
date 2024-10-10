// FWObject.cpp: implementation of the FWObject class.
//
//////////////////////////////////////////////////////////////////////

#include "FWObject.h"
#include "Global.h"


FWObject::FWObject()
{

}

FWObject::~FWObject()
{

}

FWRuntimeClass FWObject::classFWObject = 
{
	"FWObject", FWObject::CreateObject,	NULL, NULL
};
static FW_CLASS_INIT _init_FWObject(FW_RUNTIME_CLASS(FWObject));

FWRuntimeClass * FWObject::GetRuntimeClass() const
{
	return FW_RUNTIME_CLASS(FWObject);
}

FWObject * FWObject::CreateObject()
{
	return new FWObject;
}

bool FWObject::IsKindOf(FWRuntimeClass * pClass) const
{
	return GetRuntimeClass()->IsDerivedFrom(pClass);
}

void FWObject::Serialize(FWArchive & ar)
{

}
