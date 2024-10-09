// File		: EC_Split.h
// Creator	: Xu Wenbin
// Date		: 2013/3/22

#pragma once
#include <CSplit.h>

class CECSplitHelperW
{
	CSplit::VectorAWString _strs;
public:
	CECSplitHelperW(const AWString &strValue, const wchar_t *szSplit=L",");
	int Count()const{ return (int)_strs.size(); }
	bool IsEmpty()const{ return Count() == 0; }
	AWString ToString(int index)const;
	int ToInt(int index)const;
};

class CECSplitHelperA
{
	CSplit::VectorAString _strs;
public:
	CECSplitHelperA(const AString &strValue, const char *szSplit=",");
	int Count()const{ return (int)_strs.size(); }
	bool IsEmpty()const{ return Count() == 0; }
	AString ToString(int index)const;
	int ToInt(int index)const;
};
