#ifndef _A3DCOMBACTMAN_H_
#define _A3DCOMBACTMAN_H_

#include <hashmap.h>
#include "AString.h"

template<class T>
struct A3DGfxSharedObj
{
	A3DGfxSharedObj() :
	m_nRef(0),
	m_Data(0)
	{
	}

	~A3DGfxSharedObj() 
	{ 
		m_nRef = 0;
		m_Data = 0;
	}

protected:
	int		m_nRef;
	T*		m_Data;

public:
	void SetData(T* pData) { m_Data = pData; }
	T* GetData() { return m_Data; }

	void AddRef() { m_nRef++; }
	bool Release() { return --m_nRef == 0; }
};

template<class T>
class A3DGfxSharedObjMan
{
public:
	A3DGfxSharedObjMan() {}
	~A3DGfxSharedObjMan() {}

protected:
	typedef A3DGfxSharedObj<T> SharedItem;
	typedef abase::hash_map<AString, SharedItem*> SharedItemMap;

	SharedItemMap m_SharedItemMap;

public:
	void AddItem(const AString& strName, T* data)
	{
		assert(m_SharedItemMap.find(strName) == m_SharedItemMap.end());

		SharedItem* item = new SharedItem;
		item->SetData(data);
		item->AddRef();
		m_SharedItemMap[strName] = item;
	}

	T* RemoveItem(const AString& strName)
	{
		SharedItemMap::iterator it = m_SharedItemMap.find(strName);

		if (it == m_SharedItemMap.end())
			return NULL;

		SharedItem* item = it->second;

		if (item->Release())
		{
			T* pData = item->GetData();
			m_SharedItemMap.erase(strName);
			delete item;
			return pData;
		}
		return NULL;
	}

	T* GetAndRefItem(const AString& strName)
	{
		SharedItemMap::iterator it = m_SharedItemMap.find(strName);

		if (it == m_SharedItemMap.end())
			return NULL;

		it->second->AddRef();
		return it->second->GetData();
	}

	void Release()
	{
		SharedItemMap::iterator it = m_SharedItemMap.begin();

		for (; it != m_SharedItemMap.end(); ++it)
			delete it->second;

		m_SharedItemMap.clear();
	}

	void LogCurrentObjs()
	{
		SharedItemMap::iterator it = m_SharedItemMap.begin();

		for (; it != m_SharedItemMap.end(); ++it)
			a_LogOutput(1, "%s", it->first);
	}
};

#endif
