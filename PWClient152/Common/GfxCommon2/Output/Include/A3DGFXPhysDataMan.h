/*
 * FILE: A3DGFXPhysDataMan.h
 *
 * DESCRIPTION: Description Data Manager for Phys
 *
 * CREATED BY: 
 *
 * HISTORY:
 *
 */

#ifndef _A3DGFXPHYSDATAMAN_H_
#define _A3DGFXPHYSDATAMAN_H_

#include <AString.h>
#include <hashmap.h>
#include <vector.h>
#include <algorithm>

class A3DGFXEx;
class A3DPhysElement;
class A3DGFXElement;
class APhysXScene;
class APhysXObjectDesc;
class A3DPhysFluidSyncData;
class NxStream;

enum 
{
	GFXPHYSSYNCDAT_FLUID = 0,
	GFXPHYSSYNCDAT_NUM, 
};

//////////////////////////////////////////////////////////////////////////
//
//	Global Functions
//
//////////////////////////////////////////////////////////////////////////

//	Write string to stream
bool WriteString(NxStream* ps, const AString& str);
//	Read string from stream
bool ReadString(NxStream* ps, AString& str);

//////////////////////////////////////////////////////////////////////////
//
//	class IGFXPhysSyncData
//
//////////////////////////////////////////////////////////////////////////

class IGFXPhysSyncData
{
protected:
	IGFXPhysSyncData(int iPhysSyncDataType);
	virtual ~IGFXPhysSyncData() = 0 {}
public:
	static IGFXPhysSyncData* CreateFromStream(NxStream* ps);
	static void StorePhysSyncDataTypeToStream(NxStream* ps, const int iDataType);
	static void DestroyPhysSyncData(IGFXPhysSyncData* pData);
	int GetPhysSyncDataType() const { return m_iDataType; }

	virtual IGFXPhysSyncData* Clone() const = 0;
	virtual bool Load(NxStream* ps, DWORD dwVersion) = 0;
	virtual bool Save(NxStream* ps) = 0;
	virtual void SetName(const char * szName);
	const char* GetName() const { return m_strName; }
protected:
	AString m_strName;
	const int m_iDataType;
};

//////////////////////////////////////////////////////////////////////////
//
//	class A3DGFXPhysDataMan
//
//////////////////////////////////////////////////////////////////////////

// Each A3DGFXEx has one A3DGfxPhysMan
class A3DGFXPhysDataMan
{
	typedef abase::vector<APhysXObjectDesc*> A3DGFXPhysDescArray;
	typedef abase::vector<A3DPhysElement*> PhysEleList;
	typedef abase::hash_map<AString, IGFXPhysSyncData*> A3DPhysSyncDataMap;

public:

	// Interfaces for A3DPhysElement

	// Register phys element, only add a new element in
	void RegisterElement(A3DGFXElement* pEle);

	// remove element
	void RemoveElement(A3DGFXElement* pEle);

// 	// Get object desc from loaded data, used for PhysElements' construction
// 	const APhysXObjectDesc* GetObjectDesc(const char * szObjName) const;

	// Get fluid sync data by name
	IGFXPhysSyncData* GetPhysSyncData(const char * szDataName);

public:

	// A3DGFXEx and A3DGFXPhysDataMan has a 1 - 1 relationship
	// created when loading ".gfx" file and find necessary for loading a phys desc file
	explicit A3DGFXPhysDataMan(A3DGFXEx* pGFXEx);
	virtual ~A3DGFXPhysDataMan();

	// load / save phys data
	// load data to A3DGFXPhysDescArray and used for construction
	bool LoadPhys(const char* szPhysGfxFile);
	// save data from each PhysElement
	bool SavePhys(const char* szPhysGfxFile);
	
	// Get Phys Scene Object
	APhysXScene* GetPhysScene() const { return m_pPhysXScene; }
	void Init(APhysXScene* pScene) { m_pPhysXScene = pScene; }

	size_t GetPhysEleNum() const { return m_physEleLst.size(); }
	
	void Release();

public:

	// static functions
	// Input file name may be full path / relative path / single file name
	// Return the single file name for phys
	static AString GfxFileName2PhysFileName(const char * szGfxFileName);
	// Change the input filename's Ext
	static AString GfxFileExt2PhysFileExt(const char * szGfxFileName);

private:

	// currently uncopyable
	A3DGFXPhysDataMan& operator = (const A3DGFXPhysDataMan&);
	A3DGFXPhysDataMan(const A3DGFXPhysDataMan&);

private:
	// phys elements' list
	PhysEleList m_physEleLst;
	// Loaded Fluid Sync Datas
	A3DPhysSyncDataMap m_mapPhysSyncData;
	// Phys Scene Object
	APhysXScene*	m_pPhysXScene;
	// A3DGFXEx Object
	A3DGFXEx* m_pGfxEx;
	// Counter for desc
	int m_nCounter;
};





#endif