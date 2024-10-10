#ifndef A3DGFXMODELMAN_H_
#define A3DGFXMODELMAN_H_

#include <hashmap.h>
#include "AString.h"
#include "Windows.h"

class A3DSkinModel;
class A3DGFXModelMan;

class A3DGFXModelRef
{
protected:

	A3DGFXModelRef() : m_nRef(0), m_bShared(false), m_pModel(0) {}
	~A3DGFXModelRef() {}

	friend class A3DGFXModelMan;

protected:

	int m_nRef;
	bool m_bShared;
	A3DSkinModel* m_pModel;
	AString m_strFilePath;

	void AddRef() { m_nRef++; }
	int Release() { return --m_nRef; }
	void SetSharedFlag(bool b) { m_bShared = b; }
	void SetSkinModel(A3DSkinModel* pModel) { m_pModel = pModel; }
	void SetFilePath(const char* szFile) { m_strFilePath = szFile; }

public:

	bool IsShared() const { return m_bShared; }
	A3DSkinModel* GetSkinModel() { return m_pModel; }
};

typedef abase::hash_map<AString, A3DGFXModelRef*> A3DGFXModelRefMap;

class A3DGFXModelMan
{
public:

	A3DGFXModelMan() { ::InitializeCriticalSection(&m_cs); }
	~A3DGFXModelMan() { ::DeleteCriticalSection(&m_cs); }

protected:

	A3DGFXModelRefMap m_ModelRefMap;
	CRITICAL_SECTION m_cs;

public:

	void Release();
	A3DGFXModelRef* LoadModel(const char* szPath, bool bShared);
	void ReleaseModel(A3DGFXModelRef* pRef);
	void Lock() { ::EnterCriticalSection(&m_cs); }
	void Unlock() { ::LeaveCriticalSection(&m_cs); }
};

#endif
