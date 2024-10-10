/*
 * FILE: AFI.h
 *
 * DESCRIPTION: headers for interface functions in Angelica File Lib;
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFI_H_
#define _AFI_H_

#include "ABaseDef.h"
#include "AString.h"

//	************************** Attention Please! *******************************
//
//	To Use File Module properly, you must first call af_Initialize() and 
//	then call af_SetBaseDir() to set a correct base dir, and at last you
//  should make a call to af_Finalize()
//
//	The default base dir is current directory, which is set when calling 
//	af_Initialize()
//
//  ****************************************************************************

bool af_Initialize();
bool af_Finalize();
bool af_SetBaseDir(const char* szBaseDir);
const char* af_GetBaseDir();

void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, char* szRelativepath);
void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, AString& strRelativePath);
void af_GetFullPathNoBase(char* szFullpath, const char* szBaseDir, const char* szFilename);
void af_GetFullPathNoBase(AString& strFullpath, const char* szBaseDir, const char* szFilename);

void af_GetFullPath(char* szFullPath, const char* szFolderName, const char* szFileName);
void af_GetFullPath(char* szFullPath, const char* szFileName);
void af_GetFullPath(AString& strFullPath, const char* szFolderName, const char* szFileName);
void af_GetFullPath(AString& strFullPath, const char* szFileName);

void af_GetRelativePath(const char* szFullPath, const char* szFolderName, char* szRelativePath);
void af_GetRelativePath(const char* szFullPath, char* szRelativePath);
void af_GetRelativePath(const char* szFullPath, const char* szFolderName, AString& strRelativePath);
void af_GetRelativePath(const char* szFullPath, AString& strRelativePath);

//	Get the file's title in the filename string;
//	Note: lpszFile and lpszTitle should be different buffer;
bool af_GetFileTitle(const char* lpszFile, char* lpszTitle, WORD cbBuf);
bool af_GetFileTitle(const char* lpszFile, AString& strTitle);

//	Get the file's path in the filename string;
//	Note: lpszFile and lpszPath should be different buffer;
bool af_GetFilePath(const char* lpszFile, char* lpszPath, WORD cbBuf);
bool af_GetFilePath(const char* lpszFile, AString& strPath);

//	Check file extension
bool af_CheckFileExt(const char* szFileName, const char* szExt, int iExtLen=-1, int iFileNameLen=-1);
//	Change file extension, szNewExt should contain it's own '.' before new extension, for example, '.bmp'
bool af_ChangeFileExt(char* szFileNameBuf, int iBufLen, const char* szNewExt);
bool af_ChangeFileExt(AString& strFileName, const char* szNewExt);

//	Check if file exist
bool af_IsFileExist(const char * szFileName);

#endif

