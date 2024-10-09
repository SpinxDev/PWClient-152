#ifndef _FWINFOSET_H_
#define _FWINFOSET_H_

#include "CodeTemplate.h"
#include "FWConfig.h"
#include <AString.h>
#include <AList2.h>


// this is the base class of data item to be managed by FWInfoSet class. 
// each item have at least two property : ID and Name. derive from 
// FWInfo class to create your own data item type. 

struct FWInfo
{
	int ID;
	AString Name;

	FWInfo() :	ID(-1),	Name("") {}
	FWInfo(const FWInfo & src) { *this = src; }
	virtual ~FWInfo(){}
	FWInfo & operator = (const FWInfo & src) 
	{
		ID = src.ID;
		Name = src.Name;
		return *this;
	}
	IMPLEMENT_CLONE_INLINE(FWInfo, FWInfo);
};


// this the base class to manage a table consist of data item of 
// FWInfo type, providing serach-method by ID and Name.

template<class T>
class FWInfoSet : public APtrList<T *>
{
public:
	FWInfoSet()
	{
	}
	virtual ~FWInfoSet()
	{
		DeleteAll();
	}

	void DeleteAll()
	{
		ALISTPOSITION pos = GetHeadPosition();
		while (pos)
		{
			T *p = GetNext(pos);
			SAFE_DELETE(p);
		}
		RemoveAll();
	}

	T * FindByID(int nID)
	{
		ALISTPOSITION pos = GetHeadPosition();
		while (pos)
		{
			T *p = GetNext(pos);
			if (p->ID == nID)
				return p;
		}
		return NULL;
	}
	T * FindByName(const AString & strName)
	{
		ALISTPOSITION pos = GetHeadPosition();
		while (pos)
		{
			T *p = GetNext(pos);
			if (p->Name == strName)
				return p;
		}
		return NULL;
	}

	const T * FindByID(int nID) const
	{
		ALISTPOSITION pos = GetHeadPosition();
		while (pos)
		{
			T *p = GetNext(pos);
			if (p->ID == nID)
				return p;
		}
		return NULL;
	}
	const T * FindByName(const AString & strName) const
	{
		ALISTPOSITION pos = GetHeadPosition();
		while (pos)
		{
			T *p = GetNext(pos);
			if (p->Name == strName)
				return p;
		}
		return NULL;
	}
};

#endif 