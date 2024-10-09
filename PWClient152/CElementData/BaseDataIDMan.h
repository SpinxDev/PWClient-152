#ifndef BASEDATAIDMAN_H_
#define BASEDATAIDMAN_H_

#include "hashmap.h"
#include "AString.h"

typedef abase::hash_map<AString, unsigned long> BaseDataPathIDMap;
typedef abase::hash_map<unsigned long, AString> BaseDataIDPathMap;

class BaseDataIDManBase
{
public:
	BaseDataIDManBase() : m_ulMaxID(0) {}
	virtual ~BaseDataIDManBase() {}

protected:
	BaseDataPathIDMap m_PathIDMap;
	BaseDataIDPathMap m_IDPathMap;
	AString m_strFile;
	unsigned long m_ulMaxID;

protected:
	static bool CheckOut(const char* szPath);
	static bool CheckIn(const char* szPath);
	static bool CanEdit(const char* szPath);
	virtual bool IsRightType(const char* szType) const;
	virtual const char* GetType() const;
	virtual bool Save();
	virtual unsigned long IncID(unsigned long ulID) { return ulID + 1; }
	int Load(
		const char* szPath,
		BaseDataPathIDMap& PathMap,
		BaseDataIDPathMap& IDMap,
		unsigned long& ulMaxID);

public:
	/*
	 *	return 0  成功
	 *		   -1 文件打开失败
	 *		   -2 有ID或Path重名
	 *		   -3 非此类型文件
	 */
	virtual int Load(const char* szPath);
	virtual void Release()
	{ 
		m_PathIDMap.clear();
		m_IDPathMap.clear();
		m_strFile.Empty();
	}
	virtual const char* GetPathByID(unsigned long ulID) const
	{
		BaseDataIDPathMap::const_iterator it = m_IDPathMap.find(ulID);
		return it == m_IDPathMap.end() ? "" : it->second;
	}
	
	virtual unsigned long GetIDByPath(const AString& strPath) const
	{
		BaseDataPathIDMap::const_iterator it = m_PathIDMap.find(strPath);
		return it == m_PathIDMap.end() ? 0 : it->second;
	}
	virtual unsigned long CreateID(const AString& strPath);
	void GeneratePathArray(abase::vector<AString>& PathArray);
	void WipeOffByPath(abase::vector<AString>& WipeOffArray);
	void WipeOffByID(abase::vector<unsigned long>& WipeOffArray);

	unsigned long GetIdNum()
	{
		return m_PathIDMap.size();
	};
	
	BaseDataIDPathMap::const_iterator begin_id()const
	{
		return m_IDPathMap.begin();
	}

	BaseDataIDPathMap::const_iterator end_id()const
	{
		return m_IDPathMap.end();
	}
};

class ExtDataIDMan : public BaseDataIDManBase
{
public:
	ExtDataIDMan() {}
	virtual ~ExtDataIDMan() {}

protected:
	virtual bool IsRightType(const char* szType) const;
	virtual const char* GetType() const;
	virtual unsigned long IncID(unsigned long ulID);
};

extern BaseDataIDManBase g_BaseIDMan;
extern BaseDataIDManBase g_TaskIDMan;
extern BaseDataIDManBase g_TalkIDMan;
extern BaseDataIDManBase g_PolicyIDMan;
extern BaseDataIDManBase g_FaceIDMan;
extern BaseDataIDManBase g_RecipeIDMan;
extern BaseDataIDManBase g_ConfigIDMan;
extern ExtDataIDMan g_ExtBaseIDMan;

#endif