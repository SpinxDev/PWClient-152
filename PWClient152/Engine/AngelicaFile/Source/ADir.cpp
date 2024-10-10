  #include "AFPI.h"
#include "AFile.h"
#include "AFI.h"
#include "ADir.h"

#define new A_DEBUG_NEW


ADir::ADir()
{
	memset(m_szFileName,0,sizeof(m_szFileName));
	m_hDirHandle = NULL;
	m_bHasOpened = false;
}


ADir::~ADir()
{
	Close();
}

bool ADir::Open(const char* szFullPath)
{
	if(m_bHasOpened) Close();
	char name[MAX_PATH+16];
	strncpy(name,szFullPath,MAX_PATH);
	name[MAX_PATH] = '\0';
	int len = strlen(name);
	if(len > 0 && name[len-1] == '\\')
	{	
		name[len-1] = 0;
	}	
	strcat(name,"\\*.*");
	WIN32_FIND_DATAA findData;
	HANDLE handle = FindFirstFileA(name,&findData);
	if(handle == NULL)
	{
		return false;
	}
	FindClose(handle);
	strcpy(m_szFileName,name);
	m_bHasOpened  = true;
	return true;
}

bool ADir::Close()
{
	if(m_bHasOpened)
	{
		m_szFileName[0] = 0;
		if(m_hDirHandle)
		{
			FindClose(m_hDirHandle);
			m_hDirHandle = NULL;
		}
	}
	return true;
}

bool ADir::Read(AFileStat & fileData)
{
	if(!m_bHasOpened) return false;
	WIN32_FIND_DATAA findData;
	if(m_hDirHandle != NULL)
	{
		if(!FindNextFileA(m_hDirHandle,&findData)) return false;
	}
	else
	{
		m_hDirHandle = FindFirstFileA(m_szFileName,&findData);
		if(m_hDirHandle == NULL) return false;
	}
	fileData.m_nLength = findData.nFileSizeLow;
	fileData.m_bIsDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!= 0;
	strncpy(fileData.m_szName,findData.cFileName,sizeof(fileData.m_szName));
	return true;
}
