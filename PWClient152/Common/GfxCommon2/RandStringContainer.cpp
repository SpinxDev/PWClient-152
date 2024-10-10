/*
* FILE: RandStringContainer.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/05/13
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "RandStringContainer.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement RandStringContainer
//	
///////////////////////////////////////////////////////////////////////////

RandStringContainer::RandStringContainer(int iMaxNum)
: m_iMaxNum(iMaxNum)
{
}

RandStringContainer::RandStringContainer(const RandStringContainer& rhs)
: m_iMaxNum(rhs.m_iMaxNum)
{
	for (size_t iIdx = 0; iIdx < rhs.m_aItems.size(); ++iIdx)
	{
		if (iIdx >= static_cast<int>(m_aItems.size()))
			m_aItems.push_back(rhs.m_aItems[iIdx]);
		else
			m_aItems[iIdx] = rhs.m_aItems[iIdx];
	}
}

void RandStringContainer::swap(RandStringContainer& rhs)
{
	a_Swap(m_iMaxNum, rhs.m_iMaxNum);
	m_aItems.swap(rhs.m_aItems);
}

RandStringContainer& RandStringContainer::operator = (const RandStringContainer& rhs)
{
	if (&rhs == this)
		return *this;

	RandStringContainer(rhs).swap(*this);
	return *this;
}

RandStringContainer::~RandStringContainer(void)
{
}

bool RandStringContainer::UniqueAdd(const char* szString)
{
	if (!szString || !szString[0])
		return false;

	if (static_cast<int>(m_aItems.size()) >= m_iMaxNum)
		return false;

	if (std::find(m_aItems.begin(), m_aItems.end(), AString(szString)) != m_aItems.end())
		return false;

	m_aItems.push_back(szString);
	return true;
}

const char* RandStringContainer::GetRandString() const
{
	if (m_aItems.empty())
		return NULL;

	return m_aItems[a_Random(0, m_aItems.size() - 1)];
}

bool RandStringContainer::SetString(int iIdx, const char* szString)
{
	if (!szString || !szString[0])
		return false;

	if (std::find(m_aItems.begin(), m_aItems.end(), AString(szString)) != m_aItems.end())
		return false;

	m_aItems[iIdx] = szString;
	return true;
}

bool RandStringContainer::IsExist(const AString& strFind) 
{
	return std::find(m_aItems.begin(), m_aItems.end(), strFind) != m_aItems.end();
}

bool RandStringContainer::operator == (const RandStringContainer& rhs) const
{
	if (m_iMaxNum != rhs.m_iMaxNum)
		return false;

	if (m_aItems.size() != rhs.m_aItems.size())
		return false;

	for (size_t iIdx = 0; iIdx < m_aItems.size(); ++iIdx)
	{
		if (m_aItems[iIdx] != rhs.m_aItems[iIdx])
			return false;
	}

	return true;
}