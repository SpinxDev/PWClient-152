#include "AFile.h"
#include "AFPI.h"
#include "AFI.h"
#include "ADirImage.h"
#include "AFilePackMan.h"

#define new A_DEBUG_NEW

ADirImage::ADirImage()
{
	m_pck = NULL;
	m_dir = NULL;
	m_flag = 1;
	m_dirIdx = 0;
}
ADirImage::~ADirImage()
{
}

bool ADirImage::Open(const char* szFullPath)
{
	ASSERT(0 && "Don't use this class anymore !!!");
	return false;

	char szRelativePath[MAX_PATH];

	af_GetRelativePath(szFullPath, szRelativePath);
	m_pck = (AFilePackage*)g_AFilePackMan.GetFilePck(szRelativePath);

	if(m_pck)
		m_flag = 0;
	else
		m_flag = 1;
	
	if(m_flag)
	{		
		return ADir::Open(szRelativePath);
	}
	if(m_bHasOpened) Close();
	assert(m_pck);
	m_dir = m_pck->GetDirEntry(szRelativePath);
	m_dirIdx = 0;
	if(!m_dir) return false;
	m_bHasOpened = true;
	return true;
}

bool ADirImage::Close()
{
	if(m_flag)
	{		
		return ADir::Close();
	}
	if(!m_bHasOpened) return false;
	m_dir = NULL;
	m_bHasOpened = false;
	m_dirIdx = 0;
	return true;
}

bool ADirImage::Read(AFileStat & fileData)
{
	if(m_flag)
	{		
		return ADir::Read(fileData);
	}
	if(!m_bHasOpened) return false;
	AFilePackage::entry * ent = m_dir->GetItem(m_dirIdx);
	if(ent) 
		m_dirIdx ++;
	else 
		return false;
	strncpy(fileData.m_szName,ent->_name,sizeof(fileData.m_szName));
	fileData.m_bIsDir = ent->IsContainer();
	fileData.m_nLength = 0;
	if(!fileData.m_bIsDir)
	{
		int index = ent->GetIndex();
		const AFilePackage::FILEENTRY* pEntry = m_pck->GetFileEntryByIndex(index);
		fileData.m_nLength = pEntry->dwLength;
	}
	return true;
}
