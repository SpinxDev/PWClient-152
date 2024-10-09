// DataPathMan.h: interface for the DataPathMan class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _DATA_PATH_MAN_H_
#define _DATA_PATH_MAN_H_

#include "hashmap.h"
#include "AString.h"

class DataPathMan  
{
public:
	DataPathMan();
	virtual ~DataPathMan();

	typedef abase::hash_map<AString, unsigned long> PathIDMap;
	typedef abase::hash_map<unsigned long, AString> IDPathMap;

protected:
	PathIDMap m_PathIDMap;
	IDPathMap m_IDPathMap;

public:

	const char* GetPathByID(unsigned long ulID) const
	{
		IDPathMap::const_iterator it = m_IDPathMap.find(ulID);
		return it == m_IDPathMap.end() ? "" : it->second;
	}
	
	unsigned long GetIDByPath(const AString& strPath) const
	{
		PathIDMap::const_iterator it = m_PathIDMap.find(strPath);
		return it == m_PathIDMap.end() ? 0 : it->second;
	}
	
	unsigned long GetIdNum()
	{
		return m_PathIDMap.size();
	}
	
	IDPathMap::const_iterator begin_id()const
	{
		return m_IDPathMap.begin();
	}
	IDPathMap::const_iterator end_id()const
	{
		return m_IDPathMap.end();
	}

	PathIDMap::const_iterator begin_path()const
	{
		return m_PathIDMap.begin();
	}
	PathIDMap::const_iterator end_path()const
	{
		return m_PathIDMap.end();
	}

	void Release()
	{
		m_PathIDMap.clear();
		m_IDPathMap.clear();
	}

	//Return 0 if failed
	int Load(const char *szPath);
};

#endif
