#ifndef _FWOBJECT_H_
#define _FWOBJECT_H_

#include "FWRuntimeClass.h"

class FWArchive;

class FWObject  
{
public:
	FWObject();
	virtual ~FWObject();	

	// dynamic
	static FWRuntimeClass classFWObject;
	virtual FWRuntimeClass * GetRuntimeClass() const;
	bool IsKindOf(FWRuntimeClass * pClass) const;
	// dyncrate
	static FWObject * CreateObject();

	virtual void Serialize(FWArchive & ar);
};

// just used for contruct RuntimeClass map
class FW_CLASS_INIT
{
public:
	FW_CLASS_INIT(FWRuntimeClass * pNewClass)
	{
		pNewClass->m_pNextClass = FWRuntimeClass::s_pFirstClass;
		FWRuntimeClass::s_pFirstClass = pNewClass;
	}
};

#define FW_RUNTIME_CLASS(className) \
	((FWRuntimeClass*)(&className::class##className))

// declare_xxx

#define FW_DECLARE_DYNAMIC(className) \
public: \
	static FWRuntimeClass class##className; \
	virtual FWRuntimeClass * GetRuntimeClass() const; 

#define FW_DECLARE_DYNCREATE(className) \
	FW_DECLARE_DYNAMIC(className) \
	static FWObject * CreateObject();

#define FW_DECLARE_SERIAL(className) \
	FW_DECLARE_DYNCREATE(className) \
	friend FWArchive & operator >> (FWArchive & ar, const className *& pObj);

// implement_xxx
#define FW_IMPLEMENT_RUNTIMECLASS(className, baseClassName, pfnNew) \
	FWRuntimeClass className::class##className = \
	{ 	#className, pfnNew, FW_RUNTIME_CLASS(baseClassName), NULL }; \
	FWRuntimeClass * className::GetRuntimeClass() const \
	{ return FW_RUNTIME_CLASS(className); }

#define FW_IMPLEMENT_DYNAMIC(className, baseClassName) \
	FW_IMPLEMENT_RUNTIMECLASS(className, baseClassName, NULL)

#define FW_IMPLEMENT_DYNCREATE(className, baseClassName) \
	FWObject * className::CreateObject() { return new className; } \
	FW_IMPLEMENT_RUNTIMECLASS(className, baseClassName, className::CreateObject) 

#define FW_IMPLEMENT_SERIAL(className, baseClassName) \
	FW_IMPLEMENT_DYNCREATE(className, baseClassName) \
	static FW_CLASS_INIT _init_##className(FW_RUNTIME_CLASS(className)); \
	FWArchive & operator >> (FWArchive & ar, const className *& pObj) \
	{ pObj = (className *) ar.ReadObject(); return ar; }

	

#endif 