#ifndef _ASSEMBLYPROFILE_H_
#define _ASSEMBLYPROFILE_H_


#include "CodeTemplate.h"
#include "FWParam.h"
#include "FWAssemblyCache.h"
#include "FWConfig.h"



class A3DGFXEx;
typedef A3DGFXEx FWAssembly;
class FWOutterPropertyList;
class ADynPropertyObject;
class AScriptFile;
class FWArchive;

// this is the list of outproperties of a gfx
class FWAssemblyProfile
{
	// assiciated ID in assemblycache
	PROPERTY_INT(AssemblyID);	

	// list of outproperties
	PROPERTY_POINTER(FWOutterPropertyList *, OutterPropertyList);
	
	// this is the outproperty each gfx should have and that 
	// can not be implement in .ini, so we put it here
	float m_fScale;	
public:
	float GetScale() const { return m_fScale; }
public:
	FWAssemblyProfile();
	virtual ~FWAssemblyProfile();
	FWAssemblyProfile & operator = (const FWAssemblyProfile & src);
	IMPLEMENT_CLONE_INLINE(FWAssemblyProfile, FWAssemblyProfile);

	// get partical count of associated assembly
	int GetParticalCount() const;
	// get Sound count of associated assembly
	int GetSoundCount() const;

	// load list from .ini
	// should only be called by FWAssemblyCache
	bool Load(AScriptFile *pFile);

	// copy content from cache
	bool Init(int nID);

	// create assembly by current content
	FWAssembly * CreateAssembly() const;

	// fill an array of params which is used by UI
	void FillParamArray(FWParamArray &arrayParam) const;
	// upate content from an array of params that passed by UI
	void UpdateFromParamArray(const FWParamArray &arrayParam);

	// serialization
	virtual void Serialize(FWArchive &ar);
};

#endif 
