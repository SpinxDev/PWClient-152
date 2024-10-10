#ifndef _A3DGFXCACHEDITEMMAN_H_
#define _A3DGFXCACHEDITEMMAN_H_

template<class T>
class CachedItem
{
public:
	CachedItem() : m_nLastAccess(0) {}
	virtual ~CachedItem() {}

protected:
	T		m_Data;
	int		m_nLastAccess;

public:
	int GetLastAccess() const { return m_nLastAccess; }
	void SetLastAccess(int nLast) { m_nLastAccess = nLast; }
	void SetData(T _Data) { m_Data = _Data; }
	T GetData() { return m_Data; }
};

template<class T>
class CachedItemMan
{
public:
	CachedItemMan() : m_nHitCount(0) {}
	virtual ~CachedItemMan() {}

protected:
	typedef CachedItem<T> _ITEM;
	typedef _ITEM* PITEM;
	typedef abase::hash_map<AString, PITEM> CachedItemMap;

	CachedItemMap m_ItemMap;
	int m_nHitCount;

public:
	void AddItem(const AString& strName, T ItemData)
	{
		PITEM pItem = new _ITEM;
		pItem->SetData(ItemData);
		pItem->SetLastAccess(++m_nHitCount);
		m_ItemMap[strName] = pItem;
	}
	bool HasItem(const AString& strName)
	{
		return m_ItemMap.find(strName) != m_ItemMap.end();
	}
	T GetItemData(const AString& strName)
	{
		CachedItemMap::iterator it = m_ItemMap.find(strName);
		if (it == m_ItemMap.end()) return T();
		return it->second->GetData();
	}
	T TouchItem(const AString& strName)
	{
		CachedItemMap::iterator it = m_ItemMap.find(strName);
		if (it == m_ItemMap.end()) return T();
		it->second->SetLastAccess(++m_nHitCount);
		return it->second->GetData();
	}
	T RemoveLRUItem()
	{
		const char* pstrLRU = NULL;
		PITEM pItem = NULL;
		int nLastAccess = m_nHitCount;
		T _ret = 0;

		for (CachedItemMap::iterator it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it)
		{
			if (it->second->GetLastAccess() < nLastAccess)
			{
				pstrLRU = it->first;
				pItem = it->second;
				nLastAccess = it->second->GetLastAccess();
			}
		}

		if (pstrLRU && pItem)
		{
			_ret = pItem->GetData();
			delete pItem;
			m_ItemMap.erase(pstrLRU);
		}

		return _ret;
	}
	int GetItemCount() const { return (int)m_ItemMap.size(); }
	T RemoveHead()
	{
		CachedItemMap::iterator it = m_ItemMap.begin();
		T _ret = it->second->GetData();
		delete it->second;
		m_ItemMap.erase(it->first);
		return _ret;
	}
};

#endif