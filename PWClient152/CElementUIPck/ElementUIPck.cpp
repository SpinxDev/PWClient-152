// ElementUIPck.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ElementUIPck.h"
#include <conio.h>
#include <io.h>
#include "AF.h"
#include "elementpckdir.h"
#include "FilterDlg.h"
#include <ABaseDef.h>
#include <AAssist.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAJOR_VERSION			1
#define MINOR_VERSION			1

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;

CFilterDlg g_filgerDlg;

typedef struct _AFPCK_OPTION
{
	char		szSrcFile[MAX_PATH];
	char		szPckFile[MAX_PATH];
	bool		bVerbose;
	bool		bUseCompress;
} AFPCK_OPTION;

const wchar_t * g_wszForbidFiles[] =
{
	L".psd",
	L".db",
	L".scc",
	L"version.sw",
	L"_desktop.ini",
	L".bak",
	L".sth",
	L".tch",
	L".max",
	L".phy",
	L".log",
	L".mphy",
	L".rar",
	L".zip",
	L".tmp",
	L".suo",
	L".bin",
};

const wchar_t * g_wszInvalidChar[] =
{
	L" ",
	L"　",
};

DWORD	g_dwTotalFileSize = 0;
char	g_szDestPath[MAX_PATH];
char	g_szPckPath[MAX_PATH];
bool	g_bNoToAll = false;
bool	g_bYesToAll = false;
bool	g_bCopyYesToAll = false;
bool	g_bCopyNoToAll = false;

ALog	g_logFile;

int createdir(char * szDir)
{
    int     nlen;
    char    full_path[MAX_PATH + 1];
    char    *p_path_to_make;
    char    *pch;

    full_path[MAX_PATH] = '\0';
    pch = p_path_to_make = NULL;
    strncpy(full_path, szDir, MAX_PATH);

    /*get rid of the first and the last slash*/
    nlen = strlen(full_path);
    if(full_path[nlen - 1] == '\\')
		full_path[nlen - 1] = 0;

    pch = p_path_to_make = full_path;
    if(*pch == '\\') pch++;

    /*begin mkdir*/
    while(pch)
    {
            pch = strstr(pch, "\\");
            if( pch )
				*pch = 0;
			if( _access(p_path_to_make, 0) == -1 )
			{
				if( !CreateDirectory(p_path_to_make, NULL) )
				{
					goto FAILURE;
				}
			}
            if( pch )
            {
				*pch = '\\';
                pch++;
            }
    }
    return 0;

FAILURE:
    return -1;
}

void SafeCreateDir(char * szDir)
{
	if( _access(szDir, 0) == -1 )
	{
		createdir(szDir);
	}
}

void show_version()
{
	printf("Element File Pack Tool - V%d.%d\n\n", MAJOR_VERSION, MINOR_VERSION);
}

bool IsForbidFile(const char * szFileName)
{
	AWString strName = AS2WC(szFileName);
	strName.MakeLower();
	for(int i=0; i<sizeof(g_wszForbidFiles) / sizeof(g_wszForbidFiles[0]); i++)
	{
		const wchar_t * wszForbid = g_wszForbidFiles[i];
		if (strName.Right(wcslen(wszForbid)) == wszForbid)
			return true;
	}

	return false;
}

bool HasInvalidChar(const char * szFileName)
{
	AWString strName = AS2WC(szFileName);
	strName.MakeLower();
	for(int i=0; i<sizeof(g_wszInvalidChar) / sizeof(g_wszInvalidChar[0]); i++)
	{
		const wchar_t * pszFound = wcsstr(strName, g_wszInvalidChar[i]);
		if( pszFound )
			return true;
	}

	return false;
}

bool PackInDir(const char * pszPath, AFPCK_OPTION * pOption, AFilePackage * pPackage)
{
	char		szSrcFile[MAX_PATH];
	char		szPath[MAX_PATH];

	strcpy(szPath, pszPath);

	if( szPath[0] && szPath[strlen(szPath) - 1] != '\\' )
	{
		strcat(szPath, "\\");
	}

	sprintf(szSrcFile, "%s%s", szPath, pOption->szSrcFile);
	
	WIN32_FIND_DATA fd;
	// We have to find the source file here;
	HANDLE hFind = FindFirstFile(szSrcFile, &fd);
	if( INVALID_HANDLE_VALUE == hFind )
		return true;

	do
	{
		char szFileName[MAX_PATH];
		char szDestFileFolder[MAX_PATH];
		char szDestFileName[MAX_PATH];
		
		sprintf(szFileName, "%s%s", szPath, fd.cFileName);
		strlwr(szFileName);

		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( 0 == stricmp(fd.cFileName, ".") ||
				0 == stricmp(fd.cFileName, "..") )
				continue;

			AFPCK_OPTION newOption = *pOption;
			strcpy(newOption.szSrcFile, "*.*");
			PackInDir(szFileName, &newOption, pPackage);
			continue;
		}
		else
		{
			if( 0 == stricmp(szFileName, pOption->szPckFile) )
				continue;
			if( IsForbidFile(szFileName) )
				continue;

			if( g_filgerDlg.IsFiltered(szFileName) )
				continue;

			printf("Packing %s......", szFileName);
			if( HasInvalidChar(szFileName) )
			{
				printf("failed! invalid char encounter!\n");
				g_logFile.Log("文件[%s]中含有非法字符（空格）!", szFileName);
				continue;
			}

			FILE * file = fopen(szFileName, "rb");
			if( NULL == file )
			{
				printf("Error Open");
				continue;
			}
			fseek(file, 0, SEEK_END);
			DWORD dwFileSize = ftell(file);
			DWORD dwCompressedSize = dwFileSize;
			LPBYTE pFileContent = (LPBYTE) malloc(dwFileSize);
			if( NULL == pFileContent )
			{
				printf("Not enough memory!\n\n");
				return false;
			}
			LPBYTE pFileCompressed = (LPBYTE) malloc(dwCompressedSize);
			if( NULL == pFileCompressed )
			{
				printf("Not enough memory!\n\n");
				return false;
			}

			fseek(file, 0, SEEK_SET);
			fread(pFileContent, dwFileSize, 1, file);
			fclose(file);

			if( 0 != AFilePackage::Compress(pFileContent, dwFileSize, pFileCompressed, &dwCompressedSize) )
			{
				dwCompressedSize = dwFileSize;
			}

			if( dwCompressedSize < dwFileSize )
			{
				if( !pPackage->AppendFileCompressed(szFileName, pFileCompressed, dwFileSize, dwCompressedSize) )
					return false;
			}
			else
			{
				if( !pPackage->AppendFileCompressed(szFileName, pFileContent, dwFileSize, dwFileSize) )
					return false;
			}
			
			if( g_szDestPath[0] )
			{
				sprintf(szDestFileName, "%s%s", g_szDestPath, szFileName);
				af_GetFilePath(szDestFileName, szDestFileFolder, MAX_PATH);

				SafeCreateDir(szDestFileFolder);
				file = fopen(szDestFileName, "wb");
				if( !file )
				{
					MessageBox(NULL, "Can not create dest file, check the dest disk!", "ERROR", MB_ICONSTOP);
					exit(0);
				}

				if( dwCompressedSize < dwFileSize )
				{
					fwrite(&dwFileSize, sizeof(DWORD), 1, file);
					fwrite(pFileCompressed, 1, dwCompressedSize, file);
				}
				else
				{
					fwrite(&dwFileSize, sizeof(DWORD), 1, file);
					fwrite(pFileContent, 1, dwFileSize, file);
				}
				fclose(file);
			}

			free(pFileContent);
			free(pFileCompressed);

			g_dwTotalFileSize += dwFileSize;
			printf("Done\n");
		}

	} while( FindNextFile(hFind, &fd) );

	return true;
}

bool IsInSepFileList(const char * szFileName)
{
	for(int i=0; i<sizeof(g_szSepFiles) / sizeof(const char *); i++)
	{
		if( stricmp(szFileName, g_szSepFiles[i]) == 0 )
			return true;
	}
	return false;
}

bool CopyDir(const char * pszPath, const char * pszOption)
{
	char		szSrcFile[MAX_PATH];
	char		szPath[MAX_PATH];

	strcpy(szPath, pszPath);

	if( szPath[0] && szPath[strlen(szPath) - 1] != '\\' )
	{
		strcat(szPath, "\\");
	}

	sprintf(szSrcFile, "%s%s", szPath, "*.*");
	
	WIN32_FIND_DATA fd;
	// We have to find the source file here;
	HANDLE hFind = FindFirstFile(szSrcFile, &fd);
	if( INVALID_HANDLE_VALUE == hFind )
		return true;

	do
	{
		char szFileName[MAX_PATH];
		char szDestFileFolder[MAX_PATH];
		char szDestFileName[MAX_PATH];
		
		sprintf(szFileName, "%s%s", szPath, fd.cFileName);
		strlwr(szFileName);
		
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( 0 == stricmp(fd.cFileName, ".") ||
				0 == stricmp(fd.cFileName, "..") )
				continue;

			if( stricmp(pszOption, "r") == 0 )
			{
				CopyDir(szFileName, pszOption);
			}
			continue;
		}
		else
		{
			if( stricmp(pszOption, "s") == 0 )
			{
				// test if the file in sep files list
				if( !IsInSepFileList(fd.cFileName) )
					continue;
			}
			if( IsForbidFile(szFileName) )
				continue;

			if( g_filgerDlg.IsFiltered(szFileName) )
				continue;

			printf("Copying %s......", szFileName);
			if( HasInvalidChar(szFileName) )
			{
				printf("failed! invalid char encounter!\n");
				g_logFile.Log("文件[%s]中含有非法字符（空格）!", szFileName);
				continue;
			}

			FILE * file = fopen(szFileName, "rb");
			if( NULL == file )
			{
				printf("Error Open");
				continue;
			}
			fseek(file, 0, SEEK_END);
			DWORD dwFileSize = ftell(file);
			LPBYTE pFileContent = (LPBYTE) malloc(dwFileSize);
			if( NULL == pFileContent )
			{
				printf("Not enough memory!\n\n");
				return false;
			}
			
			fseek(file, 0, SEEK_SET);
			fread(pFileContent, dwFileSize, 1, file);
			fclose(file);

			if( g_szPckPath[0] )
			{
				sprintf(szDestFileName, "%s%s", g_szPckPath, szFileName);
				af_GetFilePath(szDestFileName, szDestFileFolder, MAX_PATH);

				SafeCreateDir(szDestFileFolder);
				file = fopen(szDestFileName, "wb");

				fwrite(pFileContent, 1, dwFileSize, file);
				
				fclose(file);
			}

			if( g_szDestPath[0] )
			{
				sprintf(szDestFileName, "%s%s", g_szDestPath, szFileName);
				af_GetFilePath(szDestFileName, szDestFileFolder, MAX_PATH);

				SafeCreateDir(szDestFileFolder);
				file = fopen(szDestFileName, "wb");

				// we comopress all files for release
				DWORD dwCompressedSize = dwFileSize;
				LPBYTE pFileCompressed = (LPBYTE) malloc(dwCompressedSize);
				if( NULL == pFileCompressed )
				{
					printf("Not enough memory!\n\n");
					return false;
				}

				if( 0 != AFilePackage::Compress(pFileContent, dwFileSize, pFileCompressed, &dwCompressedSize) )
				{
					dwCompressedSize = dwFileSize;
				}

				if( dwCompressedSize < dwFileSize )
				{
					fwrite(&dwFileSize, 1, sizeof(DWORD), file);
					fwrite(pFileCompressed, 1, dwCompressedSize, file);
				}
				else
				{
					fwrite(&dwFileSize, 1, sizeof(DWORD), file);
					fwrite(pFileContent, 1, dwFileSize, file);
				}
				free(pFileCompressed);

				fclose(file);
			}

			free(pFileContent);
			
			g_dwTotalFileSize += dwFileSize;
			printf("Done\n");
		}

	} while( FindNextFile(hFind, &fd) );

	return true;
}

bool HasBasePath(const char * path1, const char * path2)
{
	char szPath1[MAX_PATH];
	char szPath2[MAX_PATH];

	strcpy(szPath1, path1);
	strcpy(szPath2, path2);

	strlwr(szPath1);
	strlwr(szPath2);

	if( strlen(szPath2) == 0 )
		return false;

	if( strcmp(szPath2, ".") == 0 )
		return true;

	if( strlen(szPath1) && szPath1[strlen(szPath1) - 1] != '\\' )
		strcat(szPath1, "\\");
	if( strlen(szPath2) && szPath2[strlen(szPath2) - 1] != '\\' )
		strcat(szPath2, "\\");

	if( strstr(szPath1, szPath2) == szPath1 )
		return true;

	return false;
}

bool WINAPI IsPathIgnored(const CString filepath, const CString filename, bool bDir)
{
	int i;

	char	szFilePath[MAX_PATH];
	char	szFileName[MAX_PATH];
	char	szRelativePath[MAX_PATH];

	af_GetRelativePath((const char*)filepath, szFilePath);
	af_GetRelativePath((const char*)filename, szFileName);

	strlwr(szFilePath);
	strlwr(szFileName);

	if( bDir )
	{
	}
	else
	{
		char szPath[MAX_PATH];
		af_GetFilePath(szFilePath, szPath, MAX_PATH);
		strcpy(szFilePath, szPath);
	}

	for(i=0; i<sizeof(g_szPckDir)/sizeof(const char *); i++)
	{
		if( HasBasePath(szFilePath, g_szPckDir[i]) )
			return false;
	}

	for(i=0; i<sizeof(g_szNoPckDir)/sizeof(const char *)/2; i++)
	{
		if( bDir )
		{
			if( HasBasePath(szFilePath, g_szNoPckDir[i][0]) )
			{
				af_GetRelativePathNoBase(szFilePath, g_szNoPckDir[i][0], szRelativePath);
				if( strlen(szRelativePath) == 0 )
					return false;
				else
				{
					switch(g_szNoPckDir[i][1][0])
					{
					case 'r':
						return false;

					case 's':
						break;

					default:
						break;
					}
				}
			}
			else if( HasBasePath(g_szNoPckDir[i][0], szFilePath) )
			{
				return false;
			}
		}
		else
		{
			if( HasBasePath(szFilePath, g_szNoPckDir[i][0]) && !IsForbidFile(szFileName) && !HasInvalidChar(szFileName) )
			{
				af_GetRelativePathNoBase(szFilePath, g_szNoPckDir[i][0], szRelativePath);
				switch(g_szNoPckDir[i][1][0])
				{
				case 'r':
					return false;

				case 's':
					if( strlen(szRelativePath) == 0 && IsInSepFileList(szFileName) )
						return  false;
					break;

				default:
					if( strlen(szRelativePath) == 0 )
						return  false;
					break;
				}
			}
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	int		i;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		printf(_T("Fatal Error: MFC initialization failed"));
		return 1;
	}

	g_szDestPath[0] = '\0';
	g_szPckPath[0] = '\0';

	if( argc != 3 )
	{
		printf("usage: elementpck.exe [destdir] [pckdir]\n");
		return false;
	}

	g_logFile.Init("error.log", "element pack error message");

	strncpy(g_szDestPath, argv[1], MAX_PATH);
	if( g_szDestPath[0] && g_szDestPath[strlen(g_szDestPath) - 1] != '\\' )
		strcat(g_szDestPath, "\\");
	strlwr(g_szDestPath);

	strncpy(g_szPckPath, argv[2], MAX_PATH);
	if( g_szPckPath[0] && g_szPckPath[strlen(g_szPckPath) - 1] != '\\' )
		strcat(g_szPckPath, "\\");
	strlwr(g_szPckPath);
	
	g_bYesToAll	= false;
	g_bNoToAll = false;

	DWORD tickStart = GetTickCount();

	show_version();

	AFPCK_OPTION option;

	af_Initialize();

	char szWorkDir[MAX_PATH];
	//	Set current directory as work directory
	GetCurrentDirectoryA(MAX_PATH, szWorkDir);
	af_SetBaseDir(szWorkDir);

	if( szWorkDir[strlen(szWorkDir) - 1] != '\\' )
		strcat(szWorkDir, "\\");
	g_filgerDlg.Init(szWorkDir, IsPathIgnored);
	if( IDOK != g_filgerDlg.DoModal() )
	{
		printf("You have canceled the operation!\n");
		return 0;
	}

	// restore current directory, because filter dlg will change current directory.
	SetCurrentDirectory(szWorkDir);

	for(i=0; i<sizeof(g_szPckDir) / sizeof(const char *); i++)
	{
		sprintf(option.szPckFile, "%s.pck", g_szPckDir[i]);
		strcpy(option.szSrcFile, "*.*");
		option.bVerbose = true;
		option.bUseCompress = true;

		char szPckFile[MAX_PATH];
		sprintf(szPckFile, "%s%s", g_szPckPath, option.szPckFile);

		char szPckFilePath[MAX_PATH];
		af_GetFilePath(szPckFile, szPckFilePath, MAX_PATH);
		SafeCreateDir(szPckFilePath);
				
		g_dwTotalFileSize = 0;
		
		printf("----Creating [%s]----\n", option.szPckFile);
		// First we should create the package file;
		AFilePackage pckFile;

		if( !pckFile.Open(szPckFile, AFilePackage::CREATENEW) )
		{
			printf("Can not create %s\n\n", option.szPckFile);
			return -1;
		}

		PackInDir(g_szPckDir[i], &option, &pckFile);

		if( option.bUseCompress )
			printf("\n\nTotal %d files, %d bytes after compressed, with ratio %5.2f%%\n", pckFile.GetFileNumber(), pckFile.GetFileHeader().dwEntryOffset, 100.0f - pckFile.GetFileHeader().dwEntryOffset * 100.0f / g_dwTotalFileSize);
		else
			printf("\n\nTotal %d files, %d bytes\n", pckFile.GetFileNumber(), pckFile.GetFileHeader().dwEntryOffset);

		// Last we close and save the package file.
		pckFile.Close();

		// Succeed!
		printf("----Finished pack [%s]!----\n\n", option.szPckFile);
	}

	printf("Time used: %dms", GetTickCount() - tickStart);
	af_Finalize();

	printf("now copy unpacked files\n");
	for(i=0; i<sizeof(g_szNoPckDir) / sizeof(const char *) / 2; i++)
	{
		printf("----Copy dir [%s]----\n", g_szNoPckDir[i][0]);
		// First we should create the package file;
		
		CopyDir(g_szNoPckDir[i][0], g_szNoPckDir[i][1]);

		// Succeed!
		printf("----Finish copy [%s]!----\n\n", g_szNoPckDir[i][0]);
	}

	g_logFile.Release();
	return 0;
}
