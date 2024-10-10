#include "Param.h"

//////////////////////////////////////////////////////////////////////////
// CParamArray
//////////////////////////////////////////////////////////////////////////

CParamArray::CParamArray() 
{
}

CParamArray::CParamArray(const CParamArray & src) 
{
	*this = src; 
}

CParamArray::~CParamArray()
{
	ClearParams();
}

void CParamArray::ClearParams()
{
	for (iterator iter = begin(); end() != iter; ++iter)
	{
		CParam *pParam = *iter;
		delete pParam;
	}
	clear();
}

CParamArray & CParamArray::operator = (const CParamArray & src)
{
	if (&src == this) return *this;

	ClearParams();
	for (const_iterator iter = src.begin(); src.end() != iter; ++iter)
	{
		push_back((*iter)->Clone());
	}
	return *this;		
}

//CParamArray * CParamArray::Clone() const
//{
//	CParamArray *pNewObj = new CParamArray(*this);
//	return pNewObj;
//}
//
//void CParamArray::Assign(const CParamArray & src)
//{
//	if (&src == this) return;
//
//	ClearParams();
//	for (const_iterator iter = src.begin(); src.end() != iter; ++iter)
//	{
//		push_back((*iter)->Clone());
//	}
//	return *this;		
//}

//////////////////////////////////////////////////////////////////////////
// CParam
//////////////////////////////////////////////////////////////////////////

CParam::CParam(ACString strName, int nDisplayType, const CParamArray *pDisplayValues) : 
	m_strName(strName), m_nDisplayType(nDisplayType)
{
	if (TYPE_LIST == nDisplayType || TYPE_CUSTOM == nDisplayType)
	{
		ASSERT(pDisplayValues);
		m_displayValues = *pDisplayValues;
	}
}

CParam::~CParam() 
{
	
}

ACString CParam::GetName() const 
{ 
	return m_strName; 
}

int CParam::GetDisplayType() const 
{ 
	return m_nDisplayType; 
}

void CParam::SetDisplayValues(const CParamArray& displayValues)
{
	m_displayValues = displayValues; 
}

const CParamArray & CParam::GetDisplayValues() const 
{ 
	return m_displayValues; 
}
	
CParamArray & CParam::GetDisplayValues()
{ 
	return m_displayValues; 
}

