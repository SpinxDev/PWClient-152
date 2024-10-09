// Filename	: DlgLoginVersion.cpp
// Creator	: Xu Wenbin
// Date		: 2013/9/10

#include "DlgLoginVersion.h"
#include "EC_Global.h"
#include "EC_Game.h"

#include <AUIManager.h>

#include <cstdio>

AUI_BEGIN_COMMAND_MAP(CDlgLoginVersion, CDlgTheme)
AUI_END_COMMAND_MAP()

bool CDlgLoginVersion::OnInitDialog()
{
	PAUIOBJECT pObj = GetDlgItem("Txt_PatchVersion");
	if (pObj){
		FILE *f = fopen("..\\config\\element\\version.sw", "r");
		pObj->Show(false);
		if (f != NULL){
			char buf[256] = {0};
			fread(buf, sizeof(char), sizeof(buf)/sizeof(buf[0]), f);
			int version(-1);
			if (sscanf(buf, "%d", &version) == 1 && version >= 0){
				ACString str;
				str.Format(GetStringFromTable(282), version);
				pObj->SetText(str);
				pObj->Show(true);
			}
			fclose(f);
		}
	}
	pObj = GetDlgItem("Txt_ProgramVersion");
	if (pObj){
		
		DWORD dwVersion = g_pGame->GetGameVersion();
		DWORD dwVer1 = (dwVersion & 0x00FF0000) >> 16;
		DWORD dwVer2 = (dwVersion & 0x0000FF00) >> 8;
		DWORD dwVer3 = (dwVersion & 0x000000FF);
		ACString strVer;
		strVer.Format(_AL("%d.%d.%d"), dwVer1, dwVer2, dwVer3);
		ACString str;
		str.Format(GetStringFromTable(283), strVer, g_pGame->GetGameBuild());
		pObj->SetText(str);
	}
	SetCanEscClose(false);
	return true;
}