// AWStringWithWildcard.cpp: implementation of the AWStringWithWildcard class.
//
//////////////////////////////////////////////////////////////////////

#include "AWStringWithWildcard.h"
#include <ABaseDef.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AWStringWithWildcard::AWStringWithWildcard()
{
	m_nPosInOriginal = 0;
}

AWStringWithWildcard::~AWStringWithWildcard()
{

}

bool AWStringWithWildcard::IsMatch(const AWString &strOriginal, const AWString &strWildcard)
{
	m_string.Format(L" %s ", (const wchar_t *)strOriginal);
	m_wildcard.Format(L" %s ", (const wchar_t *)strWildcard);
	m_nPosInOriginal = 0;

	bool bMatch = true;
	int segmentStart = 0;
	bool bLastEndWithAsterisk = false;
	while (segmentStart < m_wildcard.GetLength())
	{
		int asteriskPos = m_wildcard.Find(L'*', segmentStart);
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

bool AWStringWithWildcard::IsMatchSegment(int segmentStart, int segmentEnd, bool bLastEndWithAsterisk)
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

bool AWStringWithWildcard::MatchPureSegment(int segmentStart, int segmentEnd)
{
	if (m_string.GetLength() - m_nPosInOriginal < segmentEnd - segmentStart) 
		return false;

	int nPos = m_nPosInOriginal;
	while (segmentStart < segmentEnd)
	{
		if (m_wildcard[segmentStart] != L'?' &&
			m_wildcard[segmentStart] != m_string[nPos])
			return false;
		segmentStart++;
		nPos++;
	}
	return true;
}

bool IsMatch(AWString strOriginal, AWString strWildcard, bool bMatchCase)
{
	if (!bMatchCase)
	{
		strOriginal.MakeUpper();
		strWildcard.MakeUpper();
	}
	
	AWStringWithWildcard string;
	return string.IsMatch(strOriginal, strWildcard);
}
