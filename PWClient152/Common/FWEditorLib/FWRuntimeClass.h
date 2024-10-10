#ifndef _FWRUNTIMECLASS_H_
#define _FWRUNTIMECLASS_H_

class FWArchive;
class FWObject;


class FWRuntimeClass  
{
public:
	const char * m_lpszClassName;
	FWObject* (* m_pfnCreateObject)();
	FWRuntimeClass* m_pBaseClass;
	FWRuntimeClass* m_pNextClass;

	static FWRuntimeClass * s_pFirstClass;

	bool IsDerivedFrom(const FWRuntimeClass* pBaseClass) const;

	FWObject* CreateObject() const;
	
	void Store(FWArchive& ar) const;
	static FWRuntimeClass* Load(FWArchive& ar);
};

#endif 