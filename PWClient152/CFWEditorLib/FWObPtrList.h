#ifndef _FWOBPTRLIST_H_
#define _FWOBPTRLIST_H_

#include <AList2.h>
#include "FWObject.h"
#include "FWArchive.h"

// note : this list can not serialize itself's pointer
// but can do with its member
template<class T>
class FWObPtrList : public APtrList<T>
{
public:
	FWObPtrList() {}
	virtual ~FWObPtrList(){}
	virtual void Serialize(FWArchive & ar)
	{
		if (ar.IsStoring())
		{
			ar << GetCount();
			ALISTPOSITION pos = GetHeadPosition();
			while (pos)
			{
				T p = GetNext(pos);
				ar.WriteObject(p);
			}
		}
		else
		{
			DeleteAll();	
			int nCount = 0;
			ar >> nCount;
			T p = NULL;
			while (nCount--)
			{
				p = static_cast<T>(ar.ReadObject());
				AddTail(p);
			}
		}
	}
	virtual void DeleteAll()
	{
		ALISTPOSITION pos = GetHeadPosition();
		while (pos)
		{
			T p = GetNext(pos);
			delete p;
		}
		RemoveAll();
	}
	bool IsEmpty()
	{
		return GetHeadPosition() != NULL;
	}
	void AddTail(const FWObPtrList<T> *pListSrc)
	{
		ALISTPOSITION pos = pListSrc->GetHeadPosition();
		while (pos)
		{
			AddTail(pListSrc->GetNext(pos));
		}
	}
	ALISTPOSITION AddTail(T data)
	{
		return APtrList<T>::AddTail(data);
	}
};

#endif 
