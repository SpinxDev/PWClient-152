// FontEnum.cpp: implementation of the FWFontEnum class.
//
//////////////////////////////////////////////////////////////////////

#include "FWFontEnum.h"

#define new A_DEBUG_NEW



IMPLEMENT_SINGLETON(FWFontEnum)



FWFontEnum::FWFontEnum()
{
	m_bInited = false;
}

FWFontEnum::~FWFontEnum()
{

}

bool FWFontEnum::Init(HDC hDC)
{
	if (m_bInited) return true;
	
	LOGFONT logFont;
	ZeroMemory(&logFont, sizeof(LOGFONT));
	logFont.lfFaceName[0] = '\0';

	logFont.lfCharSet = GB2312_CHARSET;
	EnumFontFamiliesEx(
		hDC,
		&logFont,
		(FONTENUMPROC)EnumFontFamExProc,
		(LPARAM)this,
		0);

	logFont.lfCharSet = ANSI_CHARSET;
	EnumFontFamiliesEx(
		hDC,
		&logFont,
		(FONTENUMPROC)EnumFontFamExProc,
		(LPARAM)this,
		0);
	return true;
}

int CALLBACK FWFontEnum::EnumFontFamExProc(
	ENUMLOGFONTEX *lpelfe,    // logical-font data
	NEWTEXTMETRICEX *lpntme,  // physical-font data
	DWORD FontType,           // type of font
	LPARAM lParam             // application-defined data
	)
{
	if (FontType != TRUETYPE_FONTTYPE)
		return 1;

	FWFontEnum *pThis = (FWFontEnum *)(lParam);
	
	if (!pThis->m_lstFonts.Find(ACString(lpelfe->elfFullName)))
		pThis->m_lstFonts.AddTail(ACString(lpelfe->elfFullName));

	return 1;
}
