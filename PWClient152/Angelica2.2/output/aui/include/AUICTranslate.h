// Filename	: AUICTranslate.h
// Creator	: Tom Zhou
// Date		: October 20, 2004
// Desc		: AUICTranslate is like the "s" command of Perl.

#ifndef _AUICTRANSLATE_H_
#define _AUICTRANSLATE_H_

#include "AString.h"
#include "AWString.h"

class AUICTranslate
{
public:
	AUICTranslate();
	~AUICTranslate();
	
	const char * Translate(const char *str);
	const wchar_t * Translate(const wchar_t *str);
	const char * ReverseTranslate(const char *str);
	const wchar_t * ReverseTranslate(const wchar_t *str);

protected:
	AString m_AString;
	AWString m_AWString;
};

#endif //_AUICTRANSLATE_H_