 /*
 * FILE: AFilePackGame.cpp
 *
 * DESCRIPTION: A File Package Class for Angelica Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AFPI.h"
#include "AFI.h"
#include "AFilePackage.h"
#include "AFilePackGame.h"
#include "AScriptFile.h"
#include "AMemory.h"
#include "AAssist.h"
#include "ATempMemMan.h"
#include "ACSWrapper.h"
#include <io.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

extern int	AFPCK_GUARDBYTE0;
extern int	AFPCK_GUARDBYTE1;
extern int	AFPCK_MASKDWORD;
extern int	AFPCK_CHECKMASK;

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct FILEENTRY_INFILE
{
	char	szFileName[MAX_PATH];	//	The file name of this entry; this may contain a path;
	DWORD	dwOffset;				//	The offset from the beginning of the package file;
	DWORD	dwLength;				//	The length of this file;
	DWORD	dwCompressedLength;		//	The compressed data length;
	int		iAccessCnt;				//	Access counter used by OpenSharedFile
};

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

extern int _CacheFileNameCompare(const void *arg1, const void *arg2);
extern void* Zlib_User_Alloc(void* opaque,unsigned int items, unsigned int size);
extern void Zlib_User_Free(void* opaque,void* ptr);
extern int Zlib_Compress(Bytef *dest,uLongf *destLen,const Bytef *source,uLong sourceLen, int level);
extern int Zlib_UnCompress(Bytef *dest,uLongf *destLen,const Bytef *source,uLong sourceLen);

///////////////////////////////////////////////////////////////////////////
//
//	Implement of AFilePackGame
//
///////////////////////////////////////////////////////////////////////////

AFilePackGame::AFilePackGame() : 
m_FileQuickSearchTab(2048)
{
	m_aFileEntries	= NULL;
	m_iNumEntry		= 0;
	m_bReadOnly		= false;
	m_bUseShortName	= false;
	m_fpPackageFile = NULL;
	m_szPckFileName[0] = '\0';

	InitializeCriticalSection(&m_csFR);

	m_bHasSafeHeader = false;
}

AFilePackGame::~AFilePackGame()
{
	DeleteCriticalSection(&m_csFR);
}

bool AFilePackGame::InnerOpen(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt, bool bShortName)
{
	char szFullPckPath[MAX_PATH];
	af_GetFullPath(szFullPckPath, szPckPath);

	m_bUseShortName = bShortName;

	//	Save folder name
	ASSERT(szFolder);
	strncpy(m_szFolder, szFolder, MAX_PATH);
	strlwr(m_szFolder);
	AFilePackage::NormalizeFileName(m_szFolder);

	//	Add '//' at folder tail
	int iFolderLen = strlen(m_szFolder);
	if (m_szFolder[iFolderLen-1] != '\\')
	{
		m_szFolder[iFolderLen] = '\\';
		m_szFolder[iFolderLen+1] = '\0';
	}

	switch (mode)
	{
	case OPENEXIST:
		m_bReadOnly = false;
		m_fpPackageFile = new CPackageFile();
		
		if( !m_fpPackageFile->Open(szFullPckPath, "r+b") )
		{
			if( !m_fpPackageFile->Open(szFullPckPath, "rb") )
			{
				delete m_fpPackageFile;
				m_fpPackageFile = NULL;

				AFERRLOG(("AFilePackGame::Open(), Can not open file [%s]", szFullPckPath));
				return false;
			}
			m_bReadOnly = true;
		}

		strncpy(m_szPckFileName, szPckPath, MAX_PATH);

		LoadSafeHeader();

		int nOffset;
		m_fpPackageFile->seek(0, SEEK_END);
		nOffset = m_fpPackageFile->tell();
		m_fpPackageFile->seek(0, SEEK_SET);

		if( m_bHasSafeHeader )
			nOffset = (int)m_safeHeader.offset;

		// Now analyse the file entries of the package;
		DWORD dwVersion;

		// First version;
		m_fpPackageFile->seek(nOffset - sizeof(DWORD), SEEK_SET);
		m_fpPackageFile->read(&dwVersion, sizeof(DWORD), 1);
	
		if (dwVersion == 0x00020002 || dwVersion == 0x00020001)
		{
			int i, iNumFile;

			// Now read file number;
			m_fpPackageFile->seek(nOffset - (sizeof (int) + sizeof (DWORD)), SEEK_SET);
			m_fpPackageFile->read(&iNumFile, sizeof (int), 1);
			m_fpPackageFile->seek(nOffset - (sizeof (FILEHEADER) + sizeof (DWORD) + sizeof(int)), SEEK_SET);
			m_fpPackageFile->read(&m_header, sizeof (FILEHEADER), 1);
			if( strstr(m_header.szDescription, "lica File Package") == NULL )
				return false;
			strncpy(m_header.szDescription, AFPCK_COPYRIGHT_TAG, sizeof(m_header.szDescription));

			// if we don't expect one encrypt package, we will let the error come out.
			// make sure the encrypt flag is correct
			bool bPackIsEncrypt = (m_header.dwFlags & PACKFLAG_ENCRYPT) != 0;
			if (bEncrypt != bPackIsEncrypt)
			{
				AFERRLOG(("AFilePackage::Open(), wrong encrypt flag"));
				return false;
			}

			m_header.dwEntryOffset ^= AFPCK_MASKDWORD;
			
			if( m_header.guardByte0 != AFPCK_GUARDBYTE0 ||
				m_header.guardByte1 != AFPCK_GUARDBYTE1 )
			{
				// corrput file
				AFERRLOG(("AFilePackGame::Open(), GuardBytes corrupted [%s]", szPckPath));
				return false;
			}

			//	Seek to entry list;
			m_fpPackageFile->seek(m_header.dwEntryOffset, SEEK_SET); 

			//	Create entries
			m_aFileEntries = new FILEENTRY[iNumFile];
			if (!m_aFileEntries)
			{
				AFERRLOG(("AFilePackGame::Open(), Not enough memory for entries [%s]", szPckPath));
				return false;
			}

			memset(m_aFileEntries, 0, sizeof (FILEENTRY) * iNumFile);

			m_iNumEntry = iNumFile;

			for (i=0; i < iNumFile; i++)
			{
				FILEENTRY* pEntry = &m_aFileEntries[i];

				FILEENTRY_INFILE tempEntry;

				// first read the entry size after compressed
				int nCompressedSize;
				m_fpPackageFile->read(&nCompressedSize, sizeof(int), 1);
				nCompressedSize ^= AFPCK_MASKDWORD;

				int nCheckSize;
				m_fpPackageFile->read(&nCheckSize, sizeof(int), 1);
				nCheckSize = nCheckSize ^ AFPCK_CHECKMASK ^ AFPCK_MASKDWORD;

				if( nCompressedSize != nCheckSize )
				{	
					AFERRLOG(("AFilePackGame::Open(), Check Byte Error!"));
					return false;
				}

				BYTE* pEntryCompressed = (BYTE*)a_malloctemp(nCompressedSize);
				if (!pEntryCompressed)
				{
					AFERRLOG(("AFilePackGame::Open(), Not enough memory !"));
					return false;
				}

				m_fpPackageFile->read(pEntryCompressed, nCompressedSize, 1);
				DWORD dwEntrySize = sizeof (FILEENTRY_INFILE);

				if (dwEntrySize == nCompressedSize)
				{
					memcpy(&tempEntry, pEntryCompressed, sizeof (FILEENTRY_INFILE));
				}
				else
				{
					if (0 != AFilePackage::Uncompress(pEntryCompressed, nCompressedSize, (LPBYTE) &tempEntry, &dwEntrySize))
					{
						a_freetemp(pEntryCompressed);
						AFERRLOG(("AFilePackGame::Open(), decode file entry fail!"));
						return false;
					}

					ASSERT(dwEntrySize == sizeof (FILEENTRY_INFILE));
				}

				a_freetemp(pEntryCompressed);

				//	Note: A bug existed in AppendFileCompressed() after m_bUseShortName was introduced. The bug
				//		didn't normalize file name when new file is added to package, so that the szFileName of
				//		FILEENTRY may contain '/' character. The bug wasn't fixed until 2013.3.18, many 'new' files
				//		have been added to package, so NormalizeFileName is inserted here to ensure all szFileName
				//		of FILEENTRY uses '\' instead of '/', at least in memory.
				NormalizeFileName(tempEntry.szFileName, false);

				//	Duplicate entry info
				pEntry->szFileName = AllocFileName(tempEntry.szFileName, i, iNumFile);
				pEntry->dwLength = tempEntry.dwLength;
				pEntry->dwCompressedLength = tempEntry.dwCompressedLength;
				pEntry->dwOffset = tempEntry.dwOffset;
			}

			ResortEntries();

			// now we move entry point to the end of the file so to keep old entries here
			if( m_bHasSafeHeader )
				m_header.dwEntryOffset = nOffset;
		}
		else
		{
			AFERRLOG(("AFilePackGame::Open(), Incorrect version!"));
			return false;
		}

		break;

	default:

		AFERRLOG(("AFilePackGame::Open(), Unknown open mode [%d]!", mode));
		return false;
	}

	m_mode = mode;

	return true;
}

bool AFilePackGame::Open(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt/* false */)
{
	return InnerOpen(szPckPath, szFolder, mode, bEncrypt, true);
}

bool AFilePackGame::Open(const char* szPckPath, OPENMODE mode, bool bEncrypt)
{
	char szFolder[MAX_PATH];

	strncpy(szFolder, szPckPath, MAX_PATH);
	if( szFolder[0] == '\0' )
	{
		AFERRLOG(("AFilePackGame::Open(), can not open a null or empty file name!"));
		return false;
	}
	char* pext = szFolder + strlen(szFolder) - 1;
	while(pext != szFolder)
	{
		if( *pext == '.' )
			break;

		pext --;
	}

	if( pext == szFolder )
	{
		AFERRLOG(("AFilePackGame::Open(), only file with extension can be opened!"));
		return false;
	}

	*pext++ = '\\';
	*pext = '\0';

	return InnerOpen(szPckPath, szFolder, mode, bEncrypt, false);
}

bool AFilePackGame::Close()
{
	if (m_fpPackageFile)
	{
		m_fpPackageFile->Close();
		delete m_fpPackageFile;
		m_fpPackageFile = NULL;
	}

	int i;

	//	Release entries
	if (m_aFileEntries)
	{
		delete [] m_aFileEntries;
		m_aFileEntries = NULL;
	}

	m_FileQuickSearchTab.clear();
	m_aIDCollisionFiles.RemoveAll();

	//	Release file names
	for (i=0; i < m_aNameBufs.GetSize(); i++)
	{
		NAMEBUFFER& info = m_aNameBufs[i];
		a_free(info.pBuffer);
	}

	m_aNameBufs.RemoveAll();

	return true;
}

//	Allocate new name
char* AFilePackGame::AllocFileName(const char* szFile, int iEntryCnt, int iEntryTotalNum)
{
	ASSERT(szFile);

	DWORD dwNameLen = strlen(szFile) + 1;
	ASSERT(dwNameLen < MAX_PATH);

	bool bAllocNewBuffer = false;
	NAMEBUFFER* pBufInfo = NULL;

	if (!m_aNameBufs.GetSize())
	{
		bAllocNewBuffer = true;
	}
	else
	{
		pBufInfo = &m_aNameBufs[m_aNameBufs.GetSize()-1];
		if (pBufInfo->dwOffset + dwNameLen > pBufInfo->dwLength)
			bAllocNewBuffer = true;
	}

	if (bAllocNewBuffer)
	{
		DWORD dwSize = (iEntryTotalNum - iEntryCnt) * 32;
		if (dwSize < dwNameLen)
			dwSize = dwNameLen * 10;

		char* pBuffer = (char*)a_malloc(dwSize);
		if (!pBuffer)
			return NULL;

		NAMEBUFFER info;
		info.dwLength = dwSize;
		info.pBuffer = pBuffer;
		info.dwOffset = 0;
		m_aNameBufs.Add(info);

		pBufInfo = &m_aNameBufs[m_aNameBufs.GetSize()-1];
	}

	ASSERT(pBufInfo);

	char* pCurPos = &(pBufInfo->pBuffer[pBufInfo->dwOffset]);
	strncpy(pCurPos, szFile, dwNameLen);
	pCurPos[dwNameLen-1] = '\0';
	pBufInfo->dwOffset += dwNameLen;

	return pCurPos;
}

//	Get rid of folder from file
void AFilePackGame::GetRidOfFolder(const char* szInName, char* szOutName)
{
	af_GetRelativePathNoBase(szInName, m_szFolder, szOutName);
}

//	Normalize file name
bool AFilePackGame::NormalizeFileName(char* szFileName, bool bUseShortName)
{
	if (!AFilePackage::NormalizeFileName(szFileName))
		return false;

	//	Get rid of folder from file name
	if (bUseShortName)
	{
		char szFullName[MAX_PATH];
		strcpy(szFullName, szFileName);
		GetRidOfFolder(szFullName, szFileName);
	}

	return true;
}

static bool CheckFileEntryValid(AFilePackGame::FILEENTRY* pFileEntry)
{
	if (pFileEntry->dwCompressedLength > MAX_FILE_PACKAGE)
	{
		// patcher在更新pck的时候，如果遇到了文件长度为0的文件，会给进一个错误的pFileEntry->dwCompressedLength
		// 通常情况下该值为0xFFFFFFFC，但是也可能有其他情况被写成了其他的值，正常情况下一个包里的小文件不应该太大
		// 我们认为过大的FileEntry是错的 
		// 标准：大于MAX_FILE_PACKAGE的情况都认为是过大的，因为pck文件大于该值时会自动拆包，理论上不会有任何一个文件大于这个值
		AFERRLOG(("CheckFileEntryValid, file entry [%s]'s length is not correct!", pFileEntry->szFileName));
		return false;
	}
	
	return true;
}

bool AFilePackGame::IsFileExist(const char* szFileName)
{
	return GetFileEntry(szFileName) ? true : false;
}

AFilePackGame::FILEENTRY* AFilePackGame::GetFileEntry(const char* szFileName)
{
	ASSERT((int)m_FileQuickSearchTab.size() + m_aIDCollisionFiles.GetSize() == m_iNumEntry);

	//	Normalize file name
	char szFindName[MAX_PATH];
	strncpy(szFindName, szFileName, MAX_PATH);
	NormalizeFileName(szFindName, m_bUseShortName);

	DWORD idFile = a_MakeIDFromLowString(szFindName);
	if (!idFile)
	{
		ASSERT(idFile && "Failed to generate file id");
		AFERRLOG(("AFilePackGame::GetFileEntry, failed to generate file id for [%s%s] !", m_szFolder, szFindName));
		return 0;
	}

	FileEntryTable::pair_type pair = m_FileQuickSearchTab.get((int)idFile);
	if (pair.second)
	{
		FILEENTRY* pFileEntry = *pair.first;

		//	Check file name again to avoid id collision problem
		if (0 != _stricmp(szFindName, pFileEntry->szFileName))
		{
			//	There is id collision occurs, search file in candidate array
		//	ASSERT(0 && "file name collision");
			pFileEntry = FindIDCollisionFile(szFindName);
			return pFileEntry;
		}
		else
			return pFileEntry;
	}
	else
		return NULL;
}

//	Find a file in ID collision candidate array
AFilePackGame::FILEENTRY* AFilePackGame::FindIDCollisionFile(const char* szFileName)
{
	int i;
	for (i=0; i < m_aIDCollisionFiles.GetSize(); i++)
	{
		FILEENTRY* pFileEntry = m_aIDCollisionFiles[i];
		if (!_stricmp(szFileName, pFileEntry->szFileName))
			return pFileEntry;
	}

	return NULL;
}

void AFilePackGame::Encrypt(LPBYTE pBuffer, DWORD dwLength)
{
	if ((m_header.dwFlags & PACKFLAG_ENCRYPT) == 0)
		return;

	DWORD dwMask = dwLength + 0x739802ab;

	for(int i=0; i<dwLength; i+=4)
	{
		if( i + 3 < dwLength )
		{
			DWORD data = (pBuffer[i] << 24) | (pBuffer[i+1] << 16) | (pBuffer[i+2] << 8) | pBuffer[i+3];
			data ^= dwMask;
			data = (data << 16) | ((data >> 16) & 0xffff);
			pBuffer[i] = (data >> 24) & 0xff;
			pBuffer[i + 1] = (data >> 16) & 0xff;
			pBuffer[i + 2] = (data >> 8) & 0xff;
			pBuffer[i + 3] = data & 0xff;
		}
	}
}

void AFilePackGame::Decrypt(LPBYTE pBuffer, DWORD dwLength)
{
	if ((m_header.dwFlags & PACKFLAG_ENCRYPT) == 0)
		return;

	DWORD dwMask = dwLength + 0x739802ab;

	for(int i=0; i<dwLength; i+=4)
	{
		if( i + 3 < dwLength )
		{
			DWORD data = (pBuffer[i] << 24) | (pBuffer[i+1] << 16) | (pBuffer[i+2] << 8) | pBuffer[i+3];
			data = (data << 16) | ((data >> 16) & 0xffff);
			data ^= dwMask;
			pBuffer[i] = (data >> 24) & 0xff;
			pBuffer[i + 1] = (data >> 16) & 0xff;
			pBuffer[i + 2] = (data >> 8) & 0xff;
			pBuffer[i + 3] = data & 0xff;
		}
	}
}

bool AFilePackGame::ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
{
	FILEENTRY* pFileEntry = GetFileEntry(szFileName);
	if (!pFileEntry)
	{
		AFERRLOG(("AFilePackage::ReadFile(), Can not find file entry [%s]!", szFileName));
		return false;
	}

	return ReadFile(*pFileEntry, pFileBuffer, pdwBufferLen);
}

bool AFilePackGame::ReadFile(FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD * pdwBufferLen)
{
	if( *pdwBufferLen < fileEntry.dwLength )
	{
		AFERRLOG(("AFilePackGame::ReadFile(), Buffer is too small!"));
		return false;
	}

	// We can automaticly determine whether compression has been used;
	if( fileEntry.dwLength > fileEntry.dwCompressedLength )
	{
		DWORD dwFileLength = fileEntry.dwLength;
		BYTE* pBuffer = (BYTE*)a_malloctemp(fileEntry.dwCompressedLength);
		if (!pBuffer)
			return false;

		ACSWrapper csa(&m_csFR);
		m_fpPackageFile->seek(fileEntry.dwOffset, SEEK_SET);
		m_fpPackageFile->read(pBuffer, fileEntry.dwCompressedLength, 1);
		Decrypt(pBuffer, fileEntry.dwCompressedLength);

		csa.Detach(true);

		if (0 != AFilePackage::Uncompress(pBuffer, fileEntry.dwCompressedLength, pFileBuffer, &dwFileLength))
		{
			FILE * fp = fopen("logs\\bad.dat", "wb");
			if (fp)
			{
				fwrite(pBuffer, fileEntry.dwCompressedLength, 1, fp);
				fclose(fp);
			}

			a_freetemp(pBuffer);
			return false;
		}
			
		//uncompress(pFileBuffer, &dwFileLength, m_pBuffer, fileEntry.dwCompressedLength);

		*pdwBufferLen = dwFileLength;

		a_freetemp(pBuffer);
	}
	else
	{
		ACSWrapper csa(&m_csFR);
		m_fpPackageFile->seek(fileEntry.dwOffset, SEEK_SET);
		m_fpPackageFile->read(pFileBuffer, fileEntry.dwLength, 1);
		Decrypt(pFileBuffer, fileEntry.dwLength);
		csa.Detach(true);

		*pdwBufferLen = fileEntry.dwLength;
	}

	return true;
}

bool AFilePackGame::ReadCompressedFile(FILEENTRY& fileEntry, LPBYTE pCompressedBuffer, DWORD * pdwBufferLen)
{
	if( *pdwBufferLen < fileEntry.dwCompressedLength )
	{
		AFERRLOG(("AFilePackGame::ReadCompressedFile(), Buffer is too small!"));
		return false;
	}

	ACSWrapper csa(&m_csFR);

	m_fpPackageFile->seek(fileEntry.dwOffset, SEEK_SET);
	*pdwBufferLen = m_fpPackageFile->read(pCompressedBuffer, 1, fileEntry.dwCompressedLength);
	Decrypt(pCompressedBuffer, fileEntry.dwCompressedLength);

	return true;
}

// 已经添加了目录结构
bool AFilePackGame::ResortEntries()
{
	int i;

	//	Build quick search table
	m_FileQuickSearchTab.clear();

	for (i=0; i < m_iNumEntry; i++)
	{
		FILEENTRY* pFileEntry = &m_aFileEntries[i];
		DWORD idFile = a_MakeIDFromLowString(pFileEntry->szFileName);

		bool val = m_FileQuickSearchTab.put((int)idFile, pFileEntry);
		if (!val)
		{
			//	ID already exist, is there a ID collision ?
			FileEntryTable::pair_type pair = m_FileQuickSearchTab.get((int)idFile);
			if (pair.second)
			{
				FILEENTRY* pCheckEntry = *pair.first;
				if (0 != _stricmp(pCheckEntry->szFileName, pFileEntry->szFileName))
				{
					//	id collision, add file to candidate array
				//	ASSERT(0 && "ID collision");
					m_aIDCollisionFiles.Add(pFileEntry);
				}
				else
				{
					//	Same file was added twice ?!! Shouldn't happen !!
					ASSERT(0 && "Same file was added twice !!");
					return false;
				}
			}
			else
			{
				//	Unknown error !!
				ASSERT(0 && "Unknown error !");
				return false;
			}
		}
	}

	return true;
}

DWORD AFilePackGame::OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem)
{
	//	Get file entry
	FILEENTRY* pFileEntry = GetFileEntry(szFileName);
	if (!pFileEntry)
	{
		if( !strstr(szFileName, "Textures") && !strstr(szFileName, "Tex_") )
		{
			AFERRLOG(("AFilePackGame::OpenSharedFile, Failed to find file [%s] in package !", szFileName));
		}

		return false;
	}

	//	Allocate file data buffer
	BYTE* pFileData = NULL;
	if (bTempMem)
		pFileData = (BYTE*)a_malloctemp(pFileEntry->dwLength);
	else
		pFileData = (BYTE*)a_malloc(pFileEntry->dwLength);

	if (!pFileData)
	{
		AFERRLOG(("AFilePackGame::OpenSharedFile, Not enough memory!"));
		return false;
	}

	//	Read file data
	DWORD dwFileLen = pFileEntry->dwLength;
	if (!ReadFile(*pFileEntry, pFileData, &dwFileLen))
	{
		if (bTempMem)
			a_freetemp(pFileData);
		else
			a_free(pFileData);

		AFERRLOG(("AFilePackGame::OpenSharedFile, Failed to read file data [%s] !", szFileName));
		return false;
	}

	//	Add it to shared file arrey
	SHAREDFILE* pFileItem = new SHAREDFILE;
	if (!pFileItem)
	{
		if (bTempMem)
			a_freetemp(pFileData);
		else
			a_free(pFileData);

		AFERRLOG(("AFilePackGame::OpenSharedFile, Not enough memory!"));
		return false;
	}

	pFileItem->bCached		= false;
	pFileItem->bTempMem		= bTempMem;
	pFileItem->dwFileID		= 0;
	pFileItem->dwFileLen	= dwFileLen;
	pFileItem->iRefCnt		= 1;
	pFileItem->pFileData	= pFileData;
	pFileItem->pFileEntry	= pFileEntry;

	*ppFileBuf	= pFileData;
	*pdwFileLen	= dwFileLen;

	return (DWORD)pFileItem;
}

//	Close a shared file
void AFilePackGame::CloseSharedFile(DWORD dwFileHandle)
{
	SHAREDFILE* pFileItem = (SHAREDFILE*)dwFileHandle;
	ASSERT(pFileItem && pFileItem->iRefCnt > 0);

	//	No cache file, release it
	if (pFileItem->bTempMem)
		a_freetemp(pFileItem->pFileData);
	else
		a_free(pFileItem->pFileData);
	
	delete pFileItem;
}

/*
	Safe Header section	
*/
bool AFilePackGame::LoadSafeHeader()
{
	m_fpPackageFile->seek(0, SEEK_SET);

	m_fpPackageFile->read(&m_safeHeader, sizeof(SAFEFILEHEADER), 1);
	if( m_safeHeader.tag1 == 0x4DCA23EF && m_safeHeader.tag2 == 0x56a089b7 )
		m_bHasSafeHeader = true;
	else
		m_bHasSafeHeader = false;

	if( m_bHasSafeHeader )
		m_fpPackageFile->Phase2Open(m_safeHeader.offset);

	m_fpPackageFile->seek(0, SEEK_SET);
	return true;
}

