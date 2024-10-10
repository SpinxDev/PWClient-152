#ifndef _ASTRINGWITHWIDECARD_H_
#define _ASTRINGWITHWIDECARD_H_

#include <AString.h>

class AStringWithWildcard
{
	AString m_string;
	AString m_wildcard;
	int m_nPosInOriginal;
protected:
	bool IsMatchSegment(int segmentStart, int segmentEnd, bool bLastEndWithAsterisk);
	bool MatchPureSegment(int segmentStart, int segmentEnd);
public:
	AStringWithWildcard();
	~AStringWithWildcard();
	bool IsMatch(AString strOriginal, AString strWildcard);
};

bool IsMatch(AString strOriginal, AString strWildcard, bool bMatchCase = false);

#endif //_ASTRINGWITHWIDECARD_H_