// EnumTypes.h: interface for the CEnumTypes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENUMTYPES_H__1395FAA0_1600_40E3_96A8_82794C8A2D2B__INCLUDED_)
#define AFX_ENUMTYPES_H__1395FAA0_1600_40E3_96A8_82794C8A2D2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AString.h"
class CPtrList;

struct ENUM_TYPE  
{
	AString strTypeName;
	DWORD   dwItemNum;
	AString *listItemName;
};


class CEnumTypes  
{
public:
	CEnumTypes();
	virtual ~CEnumTypes();

public:
	bool ReadTypes();
	void Release();
	
	bool Find(AString str);
	
	ENUM_TYPE *GetType(AString str);
	
	
private:
	CPtrList m_listTypes;	
};

#endif // !defined(AFX_ENUMTYPES_H__1395FAA0_1600_40E3_96A8_82794C8A2D2B__INCLUDED_)
