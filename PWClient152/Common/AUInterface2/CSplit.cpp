// Filename	: CSplit.cpp
// Creator	: Tom Zhou
// Date		: July 8, 2004
// Desc		: CSplit is like the "split" command of Perl.

#include "AUI.h"

#include "CSplit.h"
#include "AUICommon.h"

bool CSplit::m_bReserveEmptyStr = false;

CSplit::CSplit(const char *str)
{
	m_AString = str;
}

CSplit::CSplit(const wchar_t *str)
{
	m_AWString = str;
}

CSplit::~CSplit()
{
}

CSplit::VectorAString CSplit::Split(const char *split)
{
	m_vectorAString.clear();

	char *str = new char[m_AString.GetLength() + 1];
	ASSERT(str);
	strcpy(str, m_AString);

	char *pchStart = str, *pch = NULL;
	while( true )
	{
		pch = strstr(pchStart, split);
		if( pch ) *pch = '\0';

		if( m_bReserveEmptyStr || strlen(pchStart) > 0 )
			m_vectorAString.push_back(pchStart);

		if( !pch ) break;

		pchStart = pch + strlen(split);
	}

	delete[] str;

	return m_vectorAString;
}

CSplit::VectorAWString CSplit::Split(const wchar_t *split)
{
	m_vectorAWString.clear();

	wchar_t *str = new wchar_t[m_AWString.GetLength() + 1];
	ASSERT(str);
	wcscpy(str, m_AWString);

	wchar_t *pchStart = str, *pch = NULL;
	while( true )
	{
		pch = wcsstr(pchStart, split);
		if( pch ) *pch = '\0';

		if( m_bReserveEmptyStr || wcslen(pchStart) > 0 )
			m_vectorAWString.push_back(pchStart);

		if( !pch ) break;

		pchStart = pch + wcslen(split);
	}

	delete[] str;

	return m_vectorAWString;
}

void CSplit::SetReserveEmptyStr(bool bReserve)
{
	m_bReserveEmptyStr = bReserve;
}