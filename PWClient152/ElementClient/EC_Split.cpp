// File		: EC_Split.cpp
// Creator	: Xu Wenbin
// Date		: 2013/3/22

#include "EC_Split.h"

//	CECSplitHelperW
CECSplitHelperW::CECSplitHelperW(const AWString &strValue, const wchar_t *szSplit)
{
	CSplit splitter(strValue);
	_strs = splitter.Split(szSplit);
}

AWString CECSplitHelperW::ToString(int index)const
{
	AWString strTemp = _strs[index];
	strTemp.TrimLeft();
	strTemp.TrimRight();
	return strTemp;
}

int CECSplitHelperW::ToInt(int index)const
{
	AWString strTemp = _strs[index];
	strTemp.TrimLeft();
	strTemp.TrimRight();
	return strTemp.ToInt();
}

//	CECSplitHelperA
CECSplitHelperA::CECSplitHelperA(const AString &strValue, const char *szSplit)
{
	CSplit splitter(strValue);
	_strs = splitter.Split(szSplit);
}

AString CECSplitHelperA::ToString(int index)const
{
	AString strTemp = _strs[index];
	strTemp.TrimLeft();
	strTemp.TrimRight();
	return strTemp;
}

int CECSplitHelperA::ToInt(int index)const
{
	AString strTemp = _strs[index];
	strTemp.TrimLeft();
	strTemp.TrimRight();
	return strTemp.ToInt();
}