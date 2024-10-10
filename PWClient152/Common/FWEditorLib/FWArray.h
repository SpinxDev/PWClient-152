#ifndef _FWARRAY_H_
#define _FWARRAY_H_

#include <AArray.h>
#include "FWArchive.h"

// derived from AArray to support serialization
// note : to use this array
// type T must modified by FW_IMPLEMENT_SERIAL macro
template<class T, class ARG_T>
class FWArray : public AArray<T, ARG_T>
{
public:
	FWArray(){}
	FWArray(int iSize, int iGrowBy) : AArray(iSize, iGrowBy) {}
	virtual ~FWArray(){}
	virtual void Serialize(FWArchive &ar)
	{
		if (ar.IsStoring())
		{
			int nCount = GetSize();
			ar << nCount;
			for (int i = 0; i < nCount; i++)
			{
				ar << (*this)[i];
			}
		}	
		else
		{
			RemoveAll();
			int nCount = 0;
			ar >> nCount;
			SetSize(nCount, 0);
			for (int i = 0; i < nCount; i++)
			{
				ar >> (*this)[i];
			}
		}
	}
};

#endif 
