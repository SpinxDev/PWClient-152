/*
 * FILE: Global.h
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#pragma once
#include "Resource.h"
#include "ALog.h"
#include "AMemory.h"
#include "AString.h"
#include "A3DTypes.h"
#include "hashmap.h"

class BaseDataOrg;
class BaseDataTempl;
class CEnumTypes;
class CVssOperation;
class CRecordData;

//extern BaseDataOrg		g_BaseDataOrg;
//extern BaseDataTempl	    g_BaseDataTempl;
extern CEnumTypes g_EnumTypes;
extern HCURSOR    g_hNoDrag;

extern ALog			g_Log;
extern char			g_szWorkDir[];
extern char         g_szUserName[];
extern char         g_szPassword[];
extern char         g_szServerPath[];
extern char         g_szProject[];
extern char		    g_szIniFile[];
extern bool         g_bConfigInit;
extern bool			g_bAutoUpdate;
extern bool         g_bPolicyModifyed;
extern const char * g_szVersion;
extern bool         g_bLinkVss;


#define MAX_LINE_LEN	1024
typedef abase::hash_map<AString, int> ORG_PATHS;
extern ORG_PATHS _org_paths;
extern const char* _org_config_path;
extern bool CanExport(const char *szTemplateFilePath, AString &nameInOrgConfig);

extern CVssOperation g_VSS;
extern CRecordData   g_Record; 

inline bool ReadLine(FILE* fp, char* szLine)
{
	if (fgets(szLine, MAX_LINE_LEN, fp) == NULL) return false;
	szLine[strcspn(szLine, "\r\n")] = '\0';
	return true;
}

void SaveConfigs();
void LoadConfigs();
bool FileIsReadOnly(AString szFilePathName);
bool FileIsExist(AString szFilePathName);
void LoadAllCursor();

unsigned long glb_HandleException(LPEXCEPTION_POINTERS pExceptionPointers);

AString glb_WideCharToMultiByte(const wchar_t *wszData, int count);