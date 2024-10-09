#ifndef _EXTENDDATATEMPL_H_
#define _EXTENDDATATEMPL_H_

#include "BaseDataTemplate.h"

class ExtendDataTempl : public BaseDataTempl
{
public:
	ExtendDataTempl() {}
	virtual ~ExtendDataTempl() {}

public:
	virtual bool CreateTempl(const char* szPath, const char* szParentPath, BaseDataIDManBase* pIDMan, bool bNew = true);
	bool AddItem(const AString& strName, const AString& strType);
	void RemoveAt(int nIndex);
};

#endif