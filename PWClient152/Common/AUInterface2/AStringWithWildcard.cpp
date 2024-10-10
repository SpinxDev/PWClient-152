// AStringWithWildcard.cpp: implementation of the AStringWithWildcard class.
//
//////////////////////////////////////////////////////////////////////

#include "AUI.h"

#include "AStringWithWildcard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AStringWithWildcard::AStringWithWildcard()
{
	m_nPosInOriginal = 0;
}

AStringWithWildcard::~AStringWithWildcard()
{

}

bool AStringWithWildcard::IsMatch(AString strOriginal, AString strWildcard)
{
	m_string.Format(" %s ", strOriginal);
	m_wildcard.Format(" %s ", strWildcard);
	m_nPosInOriginal = 0;

	bool bMatch = true;
	int segmentStart = 0;
	bool bLastEndWithAsterisk = false;
	while (segmentStart < m_wildcard.GetLength())
	{
		int asteriskPos = m_wildcard.Find('*', segmentStart);
		int segmentEnd = (asteriskPos == -1) ? m_wildcard.GetLength(): asteriskPos;

		if (!IsMatchSegment(segmentStart, segmentEnd, bLastEndWithAsterisk))
		{
			bMatch = false;
			break;
		}

		bLastEndWithAsterisk = (asteriskPos != -1);
		segmentStart = segmentEnd + 1;
	}

	return bMatch;
}

bool AStringWithWildcard::IsMatchSegment(int segmentStart, int segmentEnd, bool bLastEndWithAsterisk)
{
	// null substring always match
	if (segmentStart >= segmentEnd) return true;

	if (bLastEndWithAsterisk)
	{
		while (m_nPosInOriginal < m_string.GetLength())
		{
			if (MatchPureSegment(segmentStart, segmentEnd))
			{
				m_nPosInOriginal += segmentEnd - segmentStart;
				return true;
			}
			m_nPosInOriginal++;
		}
	}
	else
	{
		if (MatchPureSegment(segmentStart, segmentEnd))
		{
			m_nPosInOriginal += segmentEnd - segmentStart;
			return true;
		}
	}
	return false;
}

bool AStringWithWildcard::MatchPureSegment(int segmentStart, int segmentEnd)
{
	if (m_string.GetLength() - m_nPosInOriginal < segmentEnd - segmentStart) 
		return false;

	int nPos = m_nPosInOriginal;
	while (segmentStart < segmentEnd)
	{
		if (m_wildcard[segmentStart] != '?' &&
			m_wildcard[segmentStart] != m_string[nPos])
			return false;
		segmentStart++;
		nPos++;
	}
	return true;
}

bool IsMatch(AString strOriginal, AString strWildcard, bool bMatchCase)
{
	if (!bMatchCase)
	{
		strOriginal.MakeUpper();
		strWildcard.MakeUpper();
	}
	
	AStringWithWildcard string;
	return string.IsMatch(strOriginal, strWildcard);
}

//////////////////////////////////////////////////////////////////////////
// test case
//////////////////////////////////////////////////////////////////////////

void TestIsMatch()
{
	assert(IsMatch("abc", "abc"));
	assert(!IsMatch("abc", ""));
	assert(!IsMatch("abc", "a"));
	assert(!IsMatch("abc", "abcc"));
	
	assert(IsMatch("abc", "*c"));
	assert(IsMatch("abc", "a*"));
	assert(IsMatch("abc", "a*c"));
	assert(IsMatch("abc", "abc*"));
	assert(IsMatch("abc", "*abc"));
	assert(IsMatch("abc", "*a*b*c"));
	assert(IsMatch("abc", "*"));
	assert(IsMatch("abc", "**"));
	assert(IsMatch("abc", "*b*"));
	assert(!IsMatch("abc", "*bd*"));
	assert(!IsMatch("abc", "bs*"));
	
	assert(IsMatch("abc", "?bc"));
	assert(IsMatch("abc", "a?c"));
	assert(IsMatch("abc", "ab?"));
	assert(IsMatch("abc", "?b?"));
	assert(IsMatch("abc", "???"));
	assert(!IsMatch("abc", "?"));
	assert(!IsMatch("abc", "??"));
	assert(!IsMatch("abc", "????"));

	assert(IsMatch("abc", "?*"));
	assert(IsMatch("abc", "*?"));
	assert(IsMatch("abc", "?*?"));
	assert(IsMatch("abc", "?*??"));
	assert(IsMatch("abc", "???*"));
	
	assert(IsMatch("abc", "a*?"));
	assert(IsMatch("abc", "?*c"));
	assert(IsMatch("abc", "*b?"));
	assert(IsMatch("abc", "a?*"));
}