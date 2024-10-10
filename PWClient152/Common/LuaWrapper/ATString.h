#pragma once

#include <AString.h>
#include <AWString.h>

#ifdef _UNICODE

#define ATString AWString

#define sprintf_utf82w(ptch, n, pch) \
	{\
		CScriptString temp;\
		AWString _awstr;\
		temp.SetUtf8(pch, strlen(pch));\
		temp.RetrieveAWString(_awstr);\
		_sntprintf((ptch), (n), L"%s", _awstr);\
	}

#define sprintf2w(ptch, n, pch) \
	{\
		CScriptString temp;\
		AWString _awstr;\
		temp.SetAString(pch);\
		temp.RetrieveAWString(_awstr);\
		_sntprintf((ptch), (n), L"%s", _awstr);\
	}

#else

#define ATString AString

#define sprintf_utf82w(ptch, n, pch)\
	{\
		CScriptString temp;\
		AString _astr;\
		temp.SetUtf8(pch, strlen(pch));\
		temp.RetrieveAString(_astr);\
		_snprintf((ptch), (n), "%s", _astr);\
	}

#define sprintf2w(ptch, n, pch) \
	{\
		_snprintf((ptch), (n), "%s", pch);\
	}
#endif