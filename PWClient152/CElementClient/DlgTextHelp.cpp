// File		: DlgTextHelp.cpp
// Creator	: Feng Ning
// Date		: 2010/11/18

#include "DlgTextHelp.h"
#include "AUITextArea.h"

#include <AFI.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTextHelp, CDlgBase)

AUI_END_COMMAND_MAP()

CDlgTextHelp::CDlgTextHelp()
{
}

CDlgTextHelp::~CDlgTextHelp()
{
}

bool CDlgTextHelp::OnInitDialog()
{
	if(!CDlgBase::OnInitDialog())
	{
		return false;
	}

	//
	return true;
}

void CDlgTextHelp::OnShowDialog()
{
	RefreshHelpContent();
	CDlgBase::OnShowDialog();
}

void CDlgTextHelp::RefreshHelpContent()
{
	// get the prefix
	AString prefix = GetName();
	prefix.MakeLower();
	if( strstr(prefix, "win_") )
	{
		prefix = prefix.Mid(strlen("win_"));
	}
	
	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	while(pElement)
	{
		PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pElement->pThis);
		if(pText)
		{
			// get the suffix
			AString suffix = pText->GetName();
			suffix.MakeLower();
			if( strstr(suffix, "help_") )
			{
				suffix = suffix.Mid(strlen("help_"));
				
				AString filename;
				LoadText(filename.Format("%s_%s.txt", prefix, suffix), pText);
			}
		}
		
		pElement = pElement->pNext;
	}
}

bool CDlgTextHelp::LoadText(const char* pFile, AUITextArea* pText)
{
	if(!pText || !pFile) return false;

	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\Info\\%s", af_GetBaseDir(), pFile);
	FILE *file = fopen(szFile, "rb");
	if( !file ) return AUI_ReportError(__LINE__, __FILE__);

	BYTE szHeader[2] = { 0, 0 };
	fread(szHeader, sizeof(BYTE), 2, file);
	if( szHeader[0] != 0xFF || szHeader[1] != 0xFE )
		return AUI_ReportError(__LINE__, __FILE__);

	fseek(file, 0, SEEK_END);
	int nLen = ftell(file) - 2;
	if( nLen <= 0 ) return AUI_ReportError(__LINE__, __FILE__);

	ACHAR *pszText = (ACHAR*)a_malloctemp(sizeof(ACHAR)*(nLen / 2 + 1));
	fseek(file, 2, SEEK_SET);
	fread(pszText, sizeof(ACHAR), nLen / 2, file);
	pszText[nLen / 2] = 0;
	pText->SetText(pszText);

	a_freetemp(pszText);

	fclose(file);

	return true;
}