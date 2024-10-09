#ifndef BASEDATAEXP_H_
#define BASEDATAEXP_H_

class AString;

void ConvertMultilineText(char* szText, bool bToMultiline);

enum enumBaseDataExportType
{
	BDET_BASE,		//	来自 g_BaseIDMan
	BDET_EXT,		//	来自 g_ExtBaseIDMan
	BDET_TALK,		//	来自 g_TalkIDMan
	BDET_FACE,		//	来自 g_FaceIDMan
	BDET_RECIPE,	//	来自 g_RecipeIDMan
	BDET_CONFIG,	//	来自 g_ConfigIDMan 
	BDET_NUM,
};
struct BaseDataExportConfig
{
	CString					exportFullPath;
	abase::vector<AString>	pathArray[BDET_NUM];

	int	ExportCount()const{
		int count(0);
		for (int i(0); i < BDET_NUM; ++ i){
			count += (int)pathArray[i].size();
		}
		return count;
	}
};
bool ExportBaseData(const BaseDataExportConfig *sourcePaths);

bool ExportPolicyData( const char* szPath);
bool CheckMd5CodeForTemplate(char *szPathName, AString *pMD5=NULL);
void TempTransVersion();
void TempExtendFullProfession();
bool HasMd5(const char* szPath);

#endif