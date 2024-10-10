#ifndef _FWASSEMBLYCACHE_H_
#define _FWASSEMBLYCACHE_H_

#include "FWInfoSet.h"
#include "CodeTemplate.h"
#include "FWConfig.h"

class A3DGFXEx;
typedef A3DGFXEx FWAssembly;
class FWAssemblyProfile;
class FWFormulaList;
class A3DGFXElement;
class A3DDevice;

// note : the ID of Assembly is generated depending on
// the sequence of items in file "ÑÌ»¨.txt"
// for each type TYPE_xxx, its ID start from (TYPE_xxx * 1000)
// and increase by 1 for each assembly read after


// this is the data item managed by FWAssemblyCache.
// each item is associated with a .gfx and a .ini file
struct FWAssemblyInfo : public FWInfo
{
	friend class FWAssemblyCache;
public:
	// see enum TYPE_xxx defined in FWAssemblyCache
	int Type;	

	// if ParticalCount == -1, the partical count of the assembly
	// is dynamic, that means user can modify some outproperty which
	// may change the partical count
	// if ParticalCount != -1, the partical count of the assembly
	// is constant, it will remain unchange all the time
	int ParticalCount; 

	// SoundCount is always static
	int SoundCount;

protected:
	A3DDevice * m_pDevice;
	AString strFileName;
#ifdef _ENABLE_ASSEMBLY_CACHE
	// cached data
	FWAssembly * m_pAssembly;
	FWAssemblyProfile * m_pProfile;
	FWFormulaList * m_pFormulaList;
#endif

public:
	FWAssemblyInfo();
	virtual ~FWAssemblyInfo();

	// create new object, using cache if posssile
	// when _ENABLE_ASSEMBLY_CACHE is set, the function 
	// will copy object from cache
	// when _ENABLE_ASSEMBLY_CACHE is not set, the
	// function is same as Loadxxx()
	FWAssembly * CreateAssembly();
	FWAssemblyProfile * CreateProfile();
	FWFormulaList * CreateFormulaList();

	// read directly from file
	FWAssembly * LoadAssembly();
	FWAssemblyProfile * LoadProfile();
	FWFormulaList * LoadFormulaList();
	bool LoadIsParticalCountConstant();
	
	// property-get
#ifdef _ENABLE_ASSEMBLY_CACHE
	const FWAssembly * GetAssembly() const;
	const FWAssemblyProfile * GetProfile() const;
	const FWFormulaList * GetFormulaList() const;
#endif
};

// this is the manager of all assembly used in fweditor
// it is a container of FWAssemblyInfo object, and have
// the ownership of those object
// note : now the manager is designed to load all file 
// when been inited
class FWAssemblyCache : public FWInfoSet<FWAssemblyInfo>
{
	DECLARE_SINGLETON(FWAssemblyCache)
protected:
	FWAssemblyCache();
protected:
	bool IsValidType(int nType);
	// convert type from string form to int form
	int GetIntTypeFromStrType(const AString &strType);

	// return value is always (nType * 1000)
	int GetMinIDOfType(int nType);

	// if no assembly of given nType exists
	// return (GetMinIDOfType() - 1)
	int GetMaxIDOfType(int nType);
	
	bool m_bInited;
public:
	enum 
	{
		TYPE_UNKNOWN = -1, 
		TYPE_SIMPLE = 0,
		TYPE_COMPONENT,
		TYPE_TEXT,
		TYPE_LANCH,
		TYPE_COUNT,
		TYPE_DEFAULT = TYPE_SIMPLE,
	};

	// return the default data item of a given type
	int GetDefaultProfileIDByType(int nType);

	// init the cache
	// read all gfx in listfile to memory
	// note : corrupted gfx will be ignored
	virtual bool Init(A3DDevice *pDevice);

};

#endif 
