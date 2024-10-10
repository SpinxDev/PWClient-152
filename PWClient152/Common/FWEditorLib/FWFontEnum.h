#ifndef _FWFONTENUM_H_
#define _FWFONTENUM_H_

#include <AList2.h>
#include <AString.h>
#include <AAssist.h>
#include "CodeTemplate.h"
#include "FWConfig.h"

// enum system font
// client can access m_lstFonts to get the list all installed
// font name, but must after the call to Init()
class FWFontEnum
{
	DECLARE_SINGLETON(FWFontEnum)
protected:
	bool m_bInited;
	static int CALLBACK EnumFontFamExProc(
		ENUMLOGFONTEX *lpelfe,    // logical-font data
		NEWTEXTMETRICEX *lpntme,  // physical-font data
		DWORD FontType,           // type of font
		LPARAM lParam             // application-defined data
		);
	FWFontEnum();
	virtual ~FWFontEnum();
	AList2<ACString, ACString &> m_lstFonts;
public:
	const AList2<ACString, ACString &> & GetFontList()	{ return m_lstFonts; }
public:
	bool Init(HDC hDC);

};

#endif 