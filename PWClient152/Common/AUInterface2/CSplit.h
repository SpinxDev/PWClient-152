// Filename	: CSplit.h
// Creator	: Tom Zhou
// Date		: July 8, 2004
// Desc		: CSplit is like the "split" command of Perl.

#ifndef _CSPLIT_H_
#define _CSPLIT_H_

#include "vector.h"

#include "AString.h"
#include "AWString.h"

class CSplit
{
public:
	typedef abase::vector<AString> VectorAString;
	typedef abase::vector<AWString> VectorAWString;

public:
	CSplit(const char *str);
	CSplit(const wchar_t *str);
	~CSplit();
	
	VectorAString Split(const char *split);
	VectorAWString Split(const wchar_t *split);
	
	static void SetReserveEmptyStr(bool bReserve);

protected:
	AString m_AString;
	AWString m_AWString;

	VectorAString m_vectorAString;
	VectorAWString m_vectorAWString;

	static bool m_bReserveEmptyStr;
};

#ifdef UNICODE
#define CSPLIT_STRING_VECTOR	CSplit::VectorAWString
#else
#define CSPLIT_STRING_VECTOR	CSplit::VectorAString
#endif

#endif //_CSPLIT_H_