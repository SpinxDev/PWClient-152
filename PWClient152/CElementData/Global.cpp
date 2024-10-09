   /*
 * FILE: Global.cpp
 *
 * DESCRIPTION: Global variables and definitions
 *
 * CREATED BY: Duyuxin, 2002/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include "EnumTypes.h"
#include "Global.h"
#include "ElementData.h"
#include "BaseDataOrg.h"
#include "BaseDataTemplate.h"
#include "VssOperation.h"
#include "TreeFlagMan.h"
#include "../CElementClient/AMiniDump.h"

#include "WinBase.h"
#include <hashtab.h>


#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

ALog		  g_Log;
char		  g_szWorkDir[MAX_PATH];

char          g_szUserName[MAX_PATH];
char          g_szPassword[MAX_PATH];
char          g_szServerPath[MAX_PATH];
char		  g_szIniFile[MAX_PATH];		//	.ini file name
char          g_szProject[256];
CEnumTypes    g_EnumTypes;
CVssOperation g_VSS;
CRecordData   g_Record;;
bool          g_bConfigInit = false;
bool          g_bAutoUpdate = true;
bool          g_bPolicyModifyed = false;
HCURSOR       g_hNoDrag;
bool          g_bLinkVss = true;

const char* _settings	= "Settings";
const char* _database	= "VSS_DATABASE";
const char* _user_name	= "VSS_USERNAME";
const char* _password	= "VSS_PASSWORD_BIN";
const char* _update		= "VSS_AUTO_UPDATE";
const char* _linkvss	= "VSS_LINK";

const char* g_szVersion = "3.0.2.5";

void Encrypt(char *strPass)
{
	int len = strlen(strPass);
	int pass = 2004;
	for(int i = 0; i< len; i++)
	{
		strPass[i] ^= pass;
	}
}

void LoadConfigs()
{
	struct abase::_hash_function	strHash;
	DWORD dwWorkID = (DWORD)strHash(g_szWorkDir);

	char	regPath[256];
	sprintf(regPath, "%s\\%ud", _settings, dwWorkID);

	g_bConfigInit = true;
	strcpy(g_szServerPath, AfxGetApp()->GetProfileString(regPath, _database, ""));
	if( strlen(g_szServerPath) == 0 )
		g_bConfigInit = false;

	strcpy(g_szUserName, AfxGetApp()->GetProfileString(regPath, _user_name, ""));
	if( strlen(g_szUserName) == 0 )
		g_bConfigInit = false;

	memset(g_szPassword, 0, sizeof(g_szPassword));
	UINT n;
	BYTE* p;
	AfxGetApp()->GetProfileBinary(regPath, _password, &p, &n);
	if (p && n) strncpy(g_szPassword, (const char*)p, n);
	Encrypt(g_szPassword);
	if( strlen(g_szPassword) == 0 )
		g_bConfigInit = false;

	g_bAutoUpdate = (AfxGetApp()->GetProfileInt(regPath, _update, 0) != 0);
	g_bLinkVss = (AfxGetApp()->GetProfileInt(regPath, _linkvss, 0) != 0);
}

void SaveConfigs()
{
	struct abase::_hash_function	strHash;
	DWORD dwWorkID = (DWORD) strHash(g_szWorkDir);

	char	regPath[256];
	sprintf(regPath, "%s\\%ud", _settings, dwWorkID);

	AfxGetApp()->WriteProfileString(regPath, _database, g_szServerPath);
	AfxGetApp()->WriteProfileString(regPath, _user_name, g_szUserName);
	char b[MAX_PATH];
	strcpy(b, g_szPassword);
	int nLen = strlen(g_szPassword);
	Encrypt(b);
	AfxGetApp()->WriteProfileBinary(regPath, _password, (unsigned char*)b, nLen);
	AfxGetApp()->WriteProfileInt(regPath, _update, g_bAutoUpdate);
	AfxGetApp()->WriteProfileInt(regPath, _linkvss, g_bLinkVss);
}

bool FileIsReadOnly(AString szFilePathName)
{
	DWORD flag = GetFileAttributes(szFilePathName);
	if(flag==-1)
	{
		//AfxMessageBox("不正确的文件附加属性,不能判断文件是否是只读");
		return true;
	}
	if (!(flag & FILE_ATTRIBUTE_READONLY)) 
	{
		return false;
	}
	return true;
}

bool FileIsExist(AString szFilePathName)
{
	FILE *file;
	file = fopen(szFilePathName,"r");
	if(file) 
	{
		fclose(file);
		return true;
	}
	else return false;
}

void LoadAllCursor()
{
	g_hNoDrag = LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_NODRAG));
}

unsigned long glb_HandleException(LPEXCEPTION_POINTERS pExceptionPointers)
{	
	if( IsDebuggerPresent() )
		return EXCEPTION_CONTINUE_SEARCH;

	char szFile[MAX_PATH];
	extern DWORD GAME_BUILD;
	sprintf(szFile, "%s\\Logs\\ec_build.dmp", g_szWorkDir);
	
	if( GetVersion() < 0x80000000 )
	{
		// WinNT or Win2000
		AMiniDump::Create(NULL, pExceptionPointers, szFile, NULL);
	}
	else
	{
		// Win95 or Win98 or Win32		
	}
	
	return EXCEPTION_EXECUTE_HANDLER;
}

AString glb_WideCharToMultiByte(const wchar_t *wszData, int count)
{
	AString strRet;
	int len = WideCharToMultiByte(CP_ACP, 0, wszData, count, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wszData, count, strRet.GetBuffer(len), len, NULL, NULL);
	strRet.ReleaseBuffer();
	return strRet;
}