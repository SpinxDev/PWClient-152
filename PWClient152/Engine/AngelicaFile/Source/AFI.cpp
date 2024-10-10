/*
 * FILE: AFI.cpp
 *
 * DESCRIPTION: interface functions in Angelica File Module;
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AFI.h"
#include "ALog.h"
#include "AFilePackage.h"
#include "AFilePackMan.h"

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

ALog*	g_pAFErrLog = NULL;
char	g_szBaseDir[MAX_PATH] = "";

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

bool af_Initialize()
{
	if (g_pAFErrLog)
		af_Finalize();

	// By default, we should set the g_szBaseDir to current directory
	GetCurrentDirectoryA(MAX_PATH, g_szBaseDir);
	int iLen = strlen(g_szBaseDir);
	if (g_szBaseDir[iLen-1] == '\\')
		g_szBaseDir[iLen-1] = '\0';

	return true;
}

bool af_SetBaseDir(const char* szBaseDir)
{
	strncpy(g_szBaseDir, szBaseDir, MAX_PATH);

	// Get rid of last '\\'
	if( g_szBaseDir[0] && g_szBaseDir[strlen(g_szBaseDir) - 1] == '\\' )
		g_szBaseDir[strlen(g_szBaseDir) - 1] = '\0';

	if (!g_pAFErrLog)
	{
		if (!(g_pAFErrLog = new ALog))
			return false;

		g_pAFErrLog->Init("AF.log", "Angelica File Module Error Log");
	}

	return true;
}

const char* af_GetBaseDir()
{
	if (!g_szBaseDir[0])
		af_Initialize();

	return g_szBaseDir;
}

bool af_Finalize()
{
	if( g_pAFErrLog )
	{
		g_pAFErrLog->Release();
		delete g_pAFErrLog;
		g_pAFErrLog = NULL;
	}

	g_szBaseDir[0] = '\0';
	return true;
}

void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, char* szRelativepath)
{
	const char* p1 = szParentPath;
	const char* p2 = szFullpath;
	
	while( *p1 && *p2 && (*p1 == *p2 || 
		(*p1 >= 'A' && *p1 <= 'Z' && *p1 + 0x20 == *p2) || 
		(*p2 >= 'A' && *p2 <= 'Z' && *p1 == *p2 + 0x20)) )
	{
		p1 ++;
		p2 ++;
	}

	if( *p1 ) // not found;
	{
		strcpy(szRelativepath, szFullpath);
		return;
	}

	if( *p2 == '\\' )
		p2 ++;

	strcpy(szRelativepath, p2);
}

void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, AString& strRelativePath)
{
	char szRelativePath[MAX_PATH];
	af_GetRelativePathNoBase(szFullpath, szParentPath, szRelativePath);
	strRelativePath = szRelativePath;
}

void af_GetFullPathNoBase(char* szFullpath, const char* szBaseDir, const char* szFilename)
{
	szFullpath[0] = '\0';

	int nStrLenName = strlen(szFilename);
	if( nStrLenName == 0 )
		return;

	//See if it is a absolute path;
	if( nStrLenName > 3 )
	{
		if( (szFilename[1] == ':' && szFilename[2] == '\\') || (szFilename[0] == '\\' && szFilename[1] == '\\') )
		{
			strcpy(szFullpath, szFilename);
			return;
		}
	}
	else
	{
		if( nStrLenName > 1 && szFilename[0] == '\\' )
			return;
	}

	const char* pszRealfile = szFilename;
	// Get rid of prefix .\, so to make a clean relative file path
	if( nStrLenName > 2 && szFilename[0] == '.' && szFilename[1] == '\\' )
		pszRealfile = szFilename + 2;
	
	if( szBaseDir[0] == '\0' )
		strcpy(szFullpath, pszRealfile);
	else if( szBaseDir[strlen(szBaseDir) - 1] == '\\' )
		sprintf(szFullpath, "%s%s", szBaseDir, pszRealfile);
	else
		sprintf(szFullpath, "%s\\%s", szBaseDir, pszRealfile);
	return;
}

void af_GetFullPathNoBase(AString& strFullpath, const char* szBaseDir, const char* szFilename)
{
	char szFullPath[MAX_PATH];
	af_GetFullPathNoBase(szFullPath, szBaseDir, szFilename);
	strFullpath = szFullPath;	
}

void af_GetFullPath(char* szFullPath, const char* szFolderName, const char* szFileName)
{
	char szBaseDir[MAX_PATH];
	sprintf(szBaseDir, "%s\\%s", g_szBaseDir, szFolderName);
	af_GetFullPathNoBase(szFullPath, szBaseDir, szFileName);
}

void af_GetFullPath(char* szFullPath, const char* szFileName)
{
	af_GetFullPathNoBase(szFullPath, g_szBaseDir, szFileName);
}

void af_GetFullPath(AString& strFullPath, const char* szFolderName, const char* szFileName)
{
	char szBaseDir[MAX_PATH];
	sprintf(szBaseDir, "%s\\%s", g_szBaseDir, szFolderName);
	af_GetFullPathNoBase(strFullPath, szBaseDir, szFileName);
}

void af_GetFullPath(AString& strFullPath, const char* szFileName)
{
	af_GetFullPathNoBase(strFullPath, g_szBaseDir, szFileName);
}

void af_GetRelativePath(const char* szFullPath, const char* szFolderName, char* szRelativePath)
{
	char szBaseDir[MAX_PATH];
	sprintf(szBaseDir, "%s\\%s", g_szBaseDir, szFolderName);
	af_GetRelativePathNoBase(szFullPath, szBaseDir, szRelativePath);
}

void af_GetRelativePath(const char* szFullPath, char* szRelativePath)
{
	af_GetRelativePathNoBase(szFullPath, g_szBaseDir, szRelativePath);
}

void af_GetRelativePath(const char* szFullPath, const char* szFolderName, AString& strRelativePath)
{
	char szBaseDir[MAX_PATH];
	sprintf(szBaseDir, "%s\\%s", g_szBaseDir, szFolderName);
	af_GetRelativePathNoBase(szFullPath, szBaseDir, strRelativePath);
}

void af_GetRelativePath(const char* szFullPath, AString& strRelativePath)
{
	af_GetRelativePathNoBase(szFullPath, g_szBaseDir, strRelativePath);
}

bool af_GetFileTitle(const char* lpszFile, char* lpszTitle, WORD cbBuf)
{
	if (!lpszFile || !lpszTitle)
		return false;

	lpszTitle[0] = '\0';
	if (lpszFile[0] == '\0')
		return true;

	const char* pszTemp = lpszFile + strlen(lpszFile);

	--pszTemp;
    if ('\\' == (* pszTemp) || '/' == (* pszTemp)) return false;
    while (true) {
        if ('\\' == (* pszTemp) || '/' == (* pszTemp))
		{
            ++pszTemp;
            break;
        }
        if (pszTemp == lpszFile) break;
        --pszTemp;
    }
    strcpy(lpszTitle, pszTemp);
    return true;
}

bool af_GetFileTitle(const char* lpszFile, AString& strTitle)
{
	char szTitle[MAX_PATH];
	bool bRet = af_GetFileTitle(lpszFile, szTitle, MAX_PATH);
	if (bRet)
		strTitle = szTitle;

	return bRet;
}

bool af_GetFilePath(const char* lpszFile, char* lpszPath, WORD cbBuf)
{
	if (!lpszFile || !lpszPath)
		return false;

	lpszPath[0]= '\0';
	if (lpszFile[0] == '\0')
		return true;

	strncpy(lpszPath, lpszFile, cbBuf);
    char* pszTemp = (char *)lpszPath + strlen(lpszPath);

    --pszTemp;
    while (true) {
        if ('\\' == (* pszTemp) || '/' == (* pszTemp)) 
		{
            break;
        }
        if (pszTemp == lpszPath) break;
        --pszTemp;
    }
    *pszTemp = '\0';
    return true;
}

bool af_GetFilePath(const char* lpszFile, AString& strPath)
{
	char szPath[MAX_PATH];
	bool bRet = af_GetFilePath(lpszFile, szPath, MAX_PATH);
	if (bRet)
		strPath = szPath;

	return bRet;
}

//	Check file extension
bool af_CheckFileExt(const char* szFileName, const char* szExt, int iExtLen/* -1 */, int iFileNameLen/* -1 */)
{
	ASSERT(szFileName && szExt);

	if (iFileNameLen < 0)
		iFileNameLen = strlen(szFileName);

	if (iExtLen < 0)
		iExtLen = strlen(szExt);

	const char* p1 = szFileName + iFileNameLen - 1;
	const char* p2 = szExt + iExtLen - 1;

	bool bMatch = true;

	while (p2 >= szExt && p1 >= szFileName)
	{
		if (*p1 != *p2 && !(*p1 >= 'A' && *p1 <= 'Z' && *p2 == *p1 + 32) &&
			!(*p1 >= 'a' && *p1 <= 'z' && *p2 == *p1 - 32))
		{
			bMatch = false;
			break;
		}

		p1--;
		p2--;
	}

	return bMatch;
}

//	Change file extension
bool af_ChangeFileExt(char* szFileNameBuf, int iBufLen, const char* szNewExt)
{
	char szFile[MAX_PATH];
	strcpy(szFile, szFileNameBuf);

	char* pTemp = strrchr(szFile, '.');
	if (pTemp)
		strcpy(pTemp, szNewExt);
	else
		strcat(szFile, szNewExt);

	int iLen = strlen(szFile);
	if (iLen >= iBufLen)
	{
		ASSERT(iLen < iBufLen);
		return false;
	}

	strcpy(szFileNameBuf, szFile);
	return true;
}

bool af_ChangeFileExt(AString& strFileName, const char* szNewExt)
{
	char szFile[MAX_PATH];
	strcpy(szFile, strFileName);

	char* pTemp = strrchr(szFile, '.');
	if (pTemp)
		strcpy(pTemp, szNewExt);
	else
		strcat(szFile, szNewExt);

	strFileName = szFile;
	return true;
}

// Check if a specified file exist
bool af_IsFileExist(const char* szFileName)
{
	char	szRelativePath[MAX_PATH];
	af_GetRelativePath(szFileName, szRelativePath);

	// we must supply a relative path to GetFilePck function
	AFilePackBase * pPackage = g_AFilePackMan.GetFilePck(szRelativePath, true);
	if (pPackage)
	{
		if (pPackage->IsFileExist(szRelativePath))
			return true;
	}

	// not found in package, so test if exist on the disk, here we must use full path
	char	szFullPath[MAX_PATH];
	af_GetFullPath(szFullPath, szFileName);
	if( INVALID_FILE_ATTRIBUTES != GetFileAttributesA(szFullPath) )
		return true;

	return false;
}

