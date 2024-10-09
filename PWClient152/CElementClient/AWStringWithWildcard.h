
#pragma once

#include <AWString.h>

class AWStringWithWildcard
{
	AWString m_string;
	AWString m_wildcard;
	int m_nPosInOriginal;
protected:
	bool IsMatchSegment(int segmentStart, int segmentEnd, bool bLastEndWithAsterisk);
	bool MatchPureSegment(int segmentStart, int segmentEnd);
public:
	AWStringWithWildcard();
	~AWStringWithWildcard();
	bool IsMatch(const AWString &strOriginal, const AWString &strWildcard);
};

bool IsMatch(AWString strOriginal, AWString strWildcard, bool bMatchCase = false);

#ifdef UNICODE
#define ACStringWithWildcard AWStringWithWildcard
#else
#define ACStringWithWildcard AStringWithWildcard
#endif