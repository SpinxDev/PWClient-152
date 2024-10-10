 /*
 * FILE: AFilePackage.cpp
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
#include "AFilePackMan.h"
#include "AScriptFile.h"
#include "AMemory.h"
#include "AAssist.h"
#include "ACSWrapper.h"
#include "ATempMemMan.h"
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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

int _CacheFileNameCompare(const void *arg1, const void *arg2)
{
	AFilePackage::CACHEFILENAME* pFile1 = *(AFilePackage::CACHEFILENAME**)arg1;
	AFilePackage::CACHEFILENAME* pFile2 = *(AFilePackage::CACHEFILENAME**)arg2;
	
	if (pFile1->dwFileID > pFile2->dwFileID)
		return 1;
	else if (pFile1->dwFileID < pFile2->dwFileID)
		return -1;
	else
		return 0;
}

void* Zlib_User_Alloc(void* opaque,unsigned int items,unsigned int size)
{
	return a_malloctemp(size * items);
}

void Zlib_User_Free(void* opaque,void* ptr)
{
	a_freetemp(ptr);
}

int Zlib_Compress(Bytef *dest,uLongf *destLen,const Bytef *source,uLong sourceLen,int level = Z_BEST_SPEED)
{
	z_stream stream;
	int err;
	
	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;
	
	stream.zalloc = &Zlib_User_Alloc;	//0;
	stream.zfree  = &Zlib_User_Free;		//0;
	stream.opaque = (voidpf)0;
	
	err = deflateInit(&stream, level);
	if (err != Z_OK) return err;
	
	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		deflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*destLen = stream.total_out;
	
	err = deflateEnd(&stream);
	return err;
}

int Zlib_UnCompress(Bytef *dest,uLongf *destLen,const Bytef *source,uLong sourceLen)
{
	z_stream stream;
	int err;
	
	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
	
	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;
	
	stream.zalloc = &Zlib_User_Alloc;  //0;
	stream.zfree  = &Zlib_User_Free;   //0;
	
	err = inflateInit(&stream);
	if (err != Z_OK) return err;
	
	err = inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		inflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*destLen = stream.total_out;
	
	err = inflateEnd(&stream);
	return err;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of AFilePackage::directory
//
///////////////////////////////////////////////////////////////////////////

int 
AFilePackage::directory::searchItemIndex(const char * name,int * pos)
{
	int left,right,mid;
	left = 0;
	right = _list.size() -1;
	mid = 0;
	while(left <= right)
	{
		mid = (left + right) / 2;
		int rst = stricmp(name,_list[mid]->_name);
		if(rst < 0)
		{
			right = mid - 1;
		}
		else if(rst > 0)
		{
			left = mid + 1;
		}
		else
		{
			return mid;
		}
	}
	if(pos) *pos = mid;
	return -1;
}

AFilePackage::entry* 
AFilePackage::directory::SearchItem(const char * name)
{
	int idx = searchItemIndex(name,NULL);
	if(idx < 0) 
		return NULL;
	else
		return _list[idx];
}


int 
AFilePackage::directory::RemoveItem(const char * name)
{
	int rst;
	rst = searchItemIndex(name,NULL);
	if(rst < 0) return -1;
	delete _list[rst];
	_list.erase(_list.begin() + rst);
	return 0;
}

AFilePackage::entry* 
AFilePackage::directory::GetItem(int index)
{
	if(index <0 || index >=(int)(_list.size())) return NULL;
	return _list[index];
}


int
AFilePackage::directory::AppendEntry(entry * item)
{
	int pos;
	if(searchItemIndex(item->_name,&pos) >=0)
	{
		//名字重复
		return -1;
	}

	if(pos >= (int)(_list.size()))
	{
		_list.push_back(item);
	}
	else
	{
		int rst = stricmp(item->_name,_list[pos]->_name);
		if(rst < 0)
		{
			_list.insert(_list.begin() + pos,item);
		}
		else
		{
			_list.insert(_list.begin() + (pos + 1),item);
		}
	}
	return 0;
}

int
AFilePackage::directory::SearchEntry(const char * filename)
{
	char name[MAX_PATH];
	strcpy(name,filename);
	char * tok;
	tok = strtok(name,"\\");
	entry * ent = this;
	while(tok)
	{
		char * next = strtok(NULL,"\\");
		entry * tmp = ent->SearchItem(tok);
		if(tmp == NULL) return -1;
		if(next)
		{
			if(tmp->IsContainer())
			{
				ent = tmp;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return tmp->GetIndex();
		}
		tok = next;
	}
	return -1;
}


int 
AFilePackage::directory::clear()
{
	size_t i;
	for(i = 0; i< _list.size(); i++)
	{
		delete _list[i];
	}
	_list.clear();
	return 0;
}


AFilePackage::directory::~directory()
{
	clear();
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of AFilePackage
//
///////////////////////////////////////////////////////////////////////////

AFilePackage::AFilePackage() : 
m_aFileEntries(0, 100),
m_aFileEntryCache(0, 100),
m_CachedFileTab(128),
m_SharedFileTab(128)
{
	m_bHasChanged	= false;
	m_bReadOnly		= false;
	m_bUseShortName	= false;
	m_fpPackageFile = NULL;
	m_dwSharedSize	= 0;
	m_dwCacheSize	= 0;
	m_szPckFileName[0] = '\0';

	InitializeCriticalSection(&m_csFR);
	InitializeCriticalSection(&m_csSF);

	m_bHasSafeHeader = false;
}

AFilePackage::~AFilePackage()
{
	DeleteCriticalSection(&m_csFR);
	DeleteCriticalSection(&m_csSF);
}

bool AFilePackage::InnerOpen(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt, bool bShortName)
{
	char szFullPckPath[MAX_PATH];
	af_GetFullPath(szFullPckPath, szPckPath);

	m_bUseShortName = bShortName;

	//	Save folder name
	ASSERT(szFolder);
	int iFolderLen = strlen(szFolder);
	memset(m_szFolder, 0, sizeof (m_szFolder));

	if (iFolderLen > 0)
	{
		strncpy(m_szFolder, szFolder, MAX_PATH);
		strlwr(m_szFolder);
		NormalizeFileName(m_szFolder);

		//	Add '\' at folder tail
		if (m_szFolder[iFolderLen-1] != '\\')
		{
			m_szFolder[iFolderLen] = '\\';
			m_szFolder[iFolderLen+1] = '\0';
		}
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

				AFERRLOG(("AFilePackage::Open(), Can not open file [%s]", szFullPckPath));
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
				AFERRLOG(("AFilePackage::Open(), GuardBytes corrupted [%s]", szPckPath));
				return false;
			}

			//	Seek to entry list;
			m_fpPackageFile->seek(m_header.dwEntryOffset, SEEK_SET); 

			//	Create entries
			m_aFileEntries.SetSize(iNumFile, 100);
			m_aFileEntryCache.SetSize(iNumFile, 100);

			for (i=0; i < iNumFile; i++)
			{
				FILEENTRY* pEntry = new FILEENTRY;
				if (!pEntry)
				{
					AFERRLOG(("AFilePackage::Open(), Not enough memory!"));
					return false;
				}
				pEntry->iAccessCnt = 0;

				FILEENTRYCACHE* pEntryCache = new FILEENTRYCACHE;
				if (!pEntryCache)
				{
					AFERRLOG(("AFilePackage::Open(), Not enough memory!"));
					return false;
				}
				memset(pEntryCache, 0, sizeof(FILEENTRYCACHE));

				// first read the entry size after compressed
				int nCompressedSize;
				m_fpPackageFile->read(&nCompressedSize, sizeof(int), 1);
				nCompressedSize ^= AFPCK_MASKDWORD;

				int nCheckSize;
				m_fpPackageFile->read(&nCheckSize, sizeof(int), 1);
				nCheckSize = nCheckSize ^ AFPCK_CHECKMASK ^ AFPCK_MASKDWORD;

				if( nCompressedSize != nCheckSize )
				{	
					AFERRLOG(("AFilePackage::Open(), Check Byte Error!"));
					return false;
				}

				pEntryCache->dwCompressedLength = nCompressedSize;
				pEntryCache->pEntryCompressed = (BYTE*)a_malloc(nCompressedSize);
				if (!pEntryCache->pEntryCompressed)
				{
					AFERRLOG(("AFilePackage::Open(), Not enough memory !"));
					return false;
				}

				m_fpPackageFile->read(pEntryCache->pEntryCompressed, nCompressedSize, 1);
				DWORD dwEntrySize = sizeof(FILEENTRY);

				if( dwEntrySize == nCompressedSize )
				{
					memcpy(pEntry, pEntryCache->pEntryCompressed, sizeof(FILEENTRY));

					// maybe the original package fileentry has not been compressed
					DWORD dwCompressedSize = sizeof(FILEENTRY);
					BYTE * pBuffer = (BYTE *)a_malloc(sizeof(FILEENTRY));
					int nRet = Compress((LPBYTE)pEntry, sizeof(FILEENTRY), pBuffer, &dwCompressedSize);
					if( nRet != 0 || dwCompressedSize >= sizeof(FILEENTRY) )
					{
						dwCompressedSize = sizeof(FILEENTRY);
						memcpy(pBuffer, pEntry, sizeof(FILEENTRY));
					}
					pEntryCache->dwCompressedLength = dwCompressedSize;
					pEntryCache->pEntryCompressed = (BYTE *)a_realloc(pEntryCache->pEntryCompressed, dwCompressedSize);
					memcpy(pEntryCache->pEntryCompressed, pBuffer, dwCompressedSize);
					a_free(pBuffer);
				}
				else
				{
					if( 0 != Uncompress(pEntryCache->pEntryCompressed, nCompressedSize, (LPBYTE)pEntry, &dwEntrySize) )
					{
						AFERRLOG(("AFilePackage::Open(), decode file entry fail!"));
						return false;
					}

					ASSERT(dwEntrySize == sizeof(FILEENTRY));
				}

				//	Note: A bug existed in AppendFileCompressed() after m_bUseShortName was introduced. The bug
				//		didn't normalize file name when new file is added to package, so that the szFileName of
				//		FILEENTRY may contain '/' character. The bug wasn't fixed until 2013.3.18, many 'new' files
				//		have been added to package, so NormalizeFileName is inserted here to ensure all szFileName
				//		of FILEENTRY uses '\' instead of '/', at least in memory.
				NormalizeFileName(pEntry->szFileName, false);

				m_aFileEntries[i] = pEntry;
				m_aFileEntryCache[i] = pEntryCache;
			}

			ResortEntries();

			// now we move entry point to the end of the file so to keep old entries here
			if( m_bHasSafeHeader )
				m_header.dwEntryOffset = nOffset;
		}
		else
		{
			AFERRLOG(("AFilePackage::Open(), Incorrect version!"));
			return false;
		}

		break;

	case CREATENEW:
		m_bReadOnly = false;
		m_fpPackageFile = new CPackageFile();
		
		if (!m_fpPackageFile->Open(szFullPckPath, "wb"))
		{
			delete m_fpPackageFile;
			m_fpPackageFile = NULL;

			AFERRLOG(("AFilePackage::Open(), Can not create file [%s]", szFullPckPath));
			return false;
		}
		strncpy(m_szPckFileName, szPckPath, MAX_PATH);

		CreateSafeHeader();

		// Init header;
		ZeroMemory(&m_header, sizeof (FILEHEADER));
		m_header.guardByte0 = AFPCK_GUARDBYTE0;
		m_header.dwEntryOffset = sizeof(SAFEFILEHEADER);
		m_header.dwVersion = AFPCK_VERSION;
		m_header.dwFlags = bEncrypt ? PACKFLAG_ENCRYPT : 0;
		m_header.guardByte1 = AFPCK_GUARDBYTE1;
		strncpy(m_header.szDescription, AFPCK_COPYRIGHT_TAG, sizeof(m_header.szDescription));

		m_aFileEntries.RemoveAll();
		m_aFileEntryCache.RemoveAll();
		break;

	default:

		AFERRLOG(("AFilePackage::Open(), Unknown open mode [%d]!", mode));
		return false;
	}

	m_mode			= mode;
	m_bHasChanged	= false;
	m_dwSharedSize	= 0;
	m_dwCacheSize	= 0;

	return true;
}

bool AFilePackage::Open(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt/* false */)
{
	return InnerOpen(szPckPath, szFolder, mode, bEncrypt, true);
}

bool AFilePackage::Open(const char* szPckPath, OPENMODE mode, bool bEncrypt)
{
	char szFolder[MAX_PATH];

	strncpy(szFolder, szPckPath, MAX_PATH);
	if( szFolder[0] == '\0' )
	{
		AFERRLOG(("AFilePackage::Open(), can not open a null or empty file name!"));
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
		AFERRLOG(("AFilePackage::Open(), only file with extension can be opened!"));
		return false;
	}

	*pext++ = '\\';
	*pext = '\0';

	return InnerOpen(szPckPath, szFolder, mode, bEncrypt, false);
}

bool AFilePackage::Close()
{
	switch( m_mode )
	{
	case OPENEXIST:
		
		if (m_bHasChanged)
		{
			DWORD dwFileSize = m_header.dwEntryOffset;

			DWORD dwEntrySize = 0;
			if( !SaveEntries(&dwEntrySize) )
				return false;
			dwFileSize += dwEntrySize;

			// Write file header here;
			m_header.dwEntryOffset ^= AFPCK_MASKDWORD;
			m_fpPackageFile->write(&m_header, sizeof (FILEHEADER), 1);
			m_header.dwEntryOffset ^= AFPCK_MASKDWORD;
			dwFileSize += sizeof (FILEHEADER);

			int iNumFile = m_aFileEntries.GetSize();
			m_fpPackageFile->write(&iNumFile, sizeof (int), 1);
			dwFileSize += sizeof (int);
			m_fpPackageFile->write(&m_header.dwVersion, sizeof (DWORD), 1);
			dwFileSize += sizeof (DWORD);

			m_fpPackageFile->SetPackageFileSize(dwFileSize);
			
			SaveSafeHeader();
			m_bHasChanged = false;
		}

		break;

	case CREATENEW:
		{
		if( !SaveEntries() )
			return false;

		int iNumFile = m_aFileEntries.GetSize();

		// Write file header here;
		m_header.dwEntryOffset ^= AFPCK_MASKDWORD;
		m_fpPackageFile->write(&m_header, sizeof (FILEHEADER), 1);
		m_header.dwEntryOffset ^= AFPCK_MASKDWORD;

		m_fpPackageFile->write(&iNumFile, sizeof (int), 1);
		m_fpPackageFile->write(&m_header.dwVersion, sizeof (DWORD), 1);

		SaveSafeHeader();
		break;
		}
	}

	if (m_fpPackageFile)
	{
		m_fpPackageFile->Close();
		delete m_fpPackageFile;
		m_fpPackageFile = NULL;
	}

	int i, iUnClosed=0;

	//	Release entries
	for (i=0; i < m_aFileEntries.GetSize(); i++)
	{
		if (m_aFileEntries[i])
		{
			delete m_aFileEntries[i];
			m_aFileEntries[i] = NULL;
		}
	}

	//	Release entries cache
	for (i=0; i < m_aFileEntryCache.GetSize(); i++)
	{
		FILEENTRYCACHE* pCache = m_aFileEntryCache[i];
		if (pCache)
		{
			if (pCache->pEntryCompressed)
			{
				a_free(pCache->pEntryCompressed);
				pCache->pEntryCompressed = NULL;
			}

			delete pCache;
		}

		m_aFileEntryCache[i] = NULL;
	}

	m_aFileEntries.RemoveAll();
	m_aFileEntryCache.RemoveAll();

	//	Release all shared files
	SharedTable::iterator it1 = m_SharedFileTab.begin();
	for (; it1 != m_SharedFileTab.end(); ++it1)
	{
		SHAREDFILE* pFileItem = *it1.value();
		if (pFileItem->iRefCnt)
			iUnClosed++;

		a_free(pFileItem->pFileData);
		delete pFileItem;
	}

	m_SharedFileTab.clear();

	if (iUnClosed)
		AFERRLOG(("AFilePackage::Close(), %d file in package weren't closed !", iUnClosed));

	//	Release cache file list
	CachedTable::iterator it2 = m_CachedFileTab.begin();
	for (; it2 != m_CachedFileTab.end(); ++it2)
	{
		CACHEFILENAME* pItem = *it2.value();
		delete pItem;
	}

	m_CachedFileTab.clear();

	return true;
}

//	Get rid of folder from file
void AFilePackage::GetRidOfFolder(const char* szInName, char* szOutName)
{
	af_GetRelativePathNoBase(szInName, m_szFolder, szOutName);
}

bool AFilePackage::NormalizeFileName(char* szFileName)
{
	int i, nLength;

	nLength = strlen(szFileName);
	
	//	First we should unite the path seperator to '\'
	for (i=0; i < nLength; i++)
	{
		if (szFileName[i] == '/')
			szFileName[i] = '\\';
	}

	//	Remove multi '\'
	for (i=0; i < nLength-1; )
	{
		if (szFileName[i] == '\\' && szFileName[i+1] == '\\')
		{
			int j;
			for (j=i; j < nLength-1; j++)
				szFileName[j] = szFileName[j+1];

			szFileName[j] = '\0';
		}
		else
		{
			i++;
		}
	}

	//	Get rid of the preceding .\ string
	if (nLength > 2 && szFileName[0] == '.' && szFileName[1] == '\\')
	{
		for (i=0; i < nLength - 2; i++)
			szFileName[i] = szFileName[i + 2];

		szFileName[i] = '\0';
	}

	//	Get rid of extra space at the tail of the string;
	nLength = strlen(szFileName);

	for (i=nLength-1; i >= 0; i--)
	{
		if (szFileName[i] != ' ')
			break;
		else
			szFileName[i] = '\0';
	}

	return true;
}

//	Normalize file name
bool AFilePackage::NormalizeFileName(char* szFileName, bool bUseShortName)
{
	if (!NormalizeFileName(szFileName))
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

static bool CheckFileEntryValid(AFilePackage::FILEENTRY* pFileEntry)
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

bool AFilePackage::IsFileExist(const char* szFileName)
{
	FILEENTRY FileEntry;
	int iEntryIndex;
	return GetFileEntry(szFileName, &FileEntry, &iEntryIndex);
}

bool AFilePackage::GetFileEntry(const char* szFileName, FILEENTRY* pFileEntry, int* pnIndex)
{
	char szFindName[MAX_PATH];

	//	Normalize file name
	strncpy(szFindName, szFileName, MAX_PATH);
	NormalizeFileName(szFindName, m_bUseShortName);

	ZeroMemory(pFileEntry, sizeof (FILEENTRY));

	int iEntry = m_directory.SearchEntry(szFindName);
	if (iEntry < 0)
		return false;

	if (!m_aFileEntries[iEntry])
		return false;

	*pFileEntry = *m_aFileEntries[iEntry];

	if (!CheckFileEntryValid(pFileEntry))
	{
		// 如果该File Entry出错了，我们返回其长度为0
		pFileEntry->dwLength = 0;
		pFileEntry->dwCompressedLength = 0;
	}

	if (pnIndex)
		*pnIndex = iEntry;

	return true;
}

void AFilePackage::Encrypt(LPBYTE pBuffer, DWORD dwLength)
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

void AFilePackage::Decrypt(LPBYTE pBuffer, DWORD dwLength)
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

bool AFilePackage::ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen)
{
	FILEENTRY fileEntry;

	if( !GetFileEntry(szFileName, &fileEntry) )
	{
		AFERRLOG(("AFilePackage::ReadFile(), Can not find file entry [%s]!", szFileName));
		return false;
	}

	return ReadFile(fileEntry, pFileBuffer, pdwBufferLen);
}

bool AFilePackage::ReadFile(FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD * pdwBufferLen)
{
	if( *pdwBufferLen < fileEntry.dwLength )
	{
		AFERRLOG(("AFilePackage::ReadFile(), Buffer is too small!"));
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

		if (0 != Uncompress(pBuffer, fileEntry.dwCompressedLength, pFileBuffer, &dwFileLength))
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

bool AFilePackage::ReadCompressedFile(const char* szFileName, LPBYTE pCompressedBuffer, DWORD* pdwBufferLen)
{
	FILEENTRY fileEntry;

	if( !GetFileEntry(szFileName, &fileEntry) )
	{
		AFERRLOG(("AFilePackage::ReadCompressedFile(), Can not find file entry [%s]!", szFileName));
		return false;
	}

	return ReadCompressedFile(fileEntry, pCompressedBuffer, pdwBufferLen);
}

bool AFilePackage::ReadCompressedFile(FILEENTRY& fileEntry, LPBYTE pCompressedBuffer, DWORD * pdwBufferLen)
{
	if( *pdwBufferLen < fileEntry.dwCompressedLength )
	{
		AFERRLOG(("AFilePackage::ReadCompressedFile(), Buffer is too small!"));
		return false;
	}

	ACSWrapper csa(&m_csFR);

	m_fpPackageFile->seek(fileEntry.dwOffset, SEEK_SET);
	*pdwBufferLen = m_fpPackageFile->read(pCompressedBuffer, 1, fileEntry.dwCompressedLength);
	Decrypt(pCompressedBuffer, fileEntry.dwCompressedLength);

	return true;
}

bool AFilePackage::AppendFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress)
{
	// We should use a function to check whether szFileName has been added into the package;
	if( m_bReadOnly )
	{
		AFERRLOG(("AFilePackage::AppendFile(), Read only package, can not append!"));
		return false;
	}

	FILEENTRY fileEntry;
	if (GetFileEntry(szFileName, &fileEntry))
	{
		AFERRLOG(("AFilePackage::AppendFile(), file entry [%s] already exist!", szFileName));
		return false;
	}

	DWORD dwCompressedLength = dwFileLength;
	if (bCompress)
	{
		//	Compress the file
		BYTE* pBuffer = (BYTE*)a_malloc(dwFileLength);
		if (!pBuffer)
			return false;

		if( 0 != Compress(pFileBuffer, dwFileLength, pBuffer, &dwCompressedLength) )
		{
			//compress error, so use uncompressed format
			dwCompressedLength = dwFileLength;
		}

		//compress2(m_pBuffer, &dwCompressedLength, pFileBuffer, dwFileLength, 1);

		if( dwCompressedLength < dwFileLength )
		{
			if( !AppendFileCompressed(szFileName, pBuffer, dwFileLength, dwCompressedLength) )
			{
				a_free(pBuffer);
				return false;
			}
		}
		else
		{
			if( !AppendFileCompressed(szFileName, pFileBuffer, dwFileLength, dwFileLength) )
			{
				a_free(pBuffer);
				return false;
			}
		}

		a_free(pBuffer);
	}
	else
	{
		if( !AppendFileCompressed(szFileName, pFileBuffer, dwFileLength, dwFileLength) )
			return false;
	}

	return true;
}

bool AFilePackage::AppendFileCompressed(const char* szFileName, LPBYTE pCompressedFileBuffer, DWORD dwFileLength, DWORD dwCompressedLength)
{
	FILEENTRY* pEntry = new FILEENTRY;
	if (!pEntry)
	{
		AFERRLOG(("AFilePackage::AppendFile(), Not enough memory!"));
		return false;
	}

	//	Normalize file name
	char szSavedFileName[MAX_PATH];
	strcpy(szSavedFileName, szFileName);
	NormalizeFileName(szSavedFileName, m_bUseShortName);
	szFileName = szSavedFileName;

	//	Store this file;
	strncpy(pEntry->szFileName, szFileName, MAX_PATH);
	pEntry->dwOffset = m_header.dwEntryOffset;
	pEntry->dwLength = dwFileLength;
	pEntry->dwCompressedLength = dwCompressedLength;
	pEntry->iAccessCnt = 0;
	if (!CheckFileEntryValid(pEntry))
	{
		delete pEntry;
		AFERRLOG(("AFilePackage::AppendFile(), Invalid File Entry!"));
		return false;
	}

	m_aFileEntries.Add(pEntry);

	FILEENTRYCACHE* pEntryCache = new FILEENTRYCACHE;
	if (!pEntryCache)
	{
		AFERRLOG(("AFilePackage::AppendFile(), Not enough memory!"));
		return false;
	}
	DWORD dwCompressedSize = sizeof(FILEENTRY);
	BYTE * pBuffer = (BYTE *)a_malloc(sizeof(FILEENTRY));
	int nRet = Compress((LPBYTE)pEntry, sizeof(FILEENTRY), pBuffer, &dwCompressedSize);
	if( nRet != 0 || dwCompressedSize >= sizeof(FILEENTRY) )
	{
		dwCompressedSize = sizeof(FILEENTRY);
		memcpy(pBuffer, pEntry, sizeof(FILEENTRY));
	}
	pEntryCache->dwCompressedLength = dwCompressedSize;
	pEntryCache->pEntryCompressed = (BYTE *)a_malloc(dwCompressedSize);
	memcpy(pEntryCache->pEntryCompressed, pBuffer, dwCompressedSize);
	m_aFileEntryCache.Add(pEntryCache);
	a_free(pBuffer);

	m_fpPackageFile->seek(m_header.dwEntryOffset, SEEK_SET);

	//	We write the compressed buffer into the disk;
	Encrypt(pCompressedFileBuffer, dwCompressedLength);
	m_fpPackageFile->write(pCompressedFileBuffer, dwCompressedLength, 1);
	Decrypt(pCompressedFileBuffer, dwCompressedLength);
	m_header.dwEntryOffset += dwCompressedLength;
	
	InsertFileToDir(szFileName, m_aFileEntries.GetSize()-1);
	m_bHasChanged = true;

	return true;
}

bool AFilePackage::RemoveFile(const char* szFileName)
{
	if( m_bReadOnly )
	{
		AFERRLOG(("AFilePackage::RemoveFile(), Read only package, can not remove file!"));
		return false;
	}
	
	FILEENTRY Entry;
	int	nIndex;

	if (!GetFileEntry(szFileName, &Entry, &nIndex))
	{
		AFERRLOG(("AFilePackage::RemoveFile(), Can not find file %s", szFileName));
		return false;
	}
	
	FILEENTRY* pEntry = m_aFileEntries[nIndex];
	RemoveFileFromDir(pEntry->szFileName);

	//	Added by dyx on 2013.10.14. Now we only delete entry object and leave a NULL at it's position
	//	in m_aFileEntries, this is in order that the entry indices recoreded in file items of m_directory 
	//	can still be valid and needn't updating.
	delete pEntry;
	m_aFileEntries[nIndex] = NULL;

	FILEENTRYCACHE* pEntryCache = m_aFileEntryCache[nIndex];
	if (pEntryCache)
	{
		if (pEntryCache->pEntryCompressed)
			a_free(pEntryCache->pEntryCompressed);

		delete pEntryCache;
		m_aFileEntryCache[nIndex] = NULL;
	}

//	ResortEntries();

	m_bHasChanged = true;
	return true;
}

bool AFilePackage::ReplaceFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress)
{
	//	We only add a new file copy at the end of the file part, and modify the 
	//	file entry point to that file body;
	DWORD dwCompressedLength = dwFileLength;

	if (bCompress)
	{
		//	Try to compress the file
		BYTE* pBuffer = (BYTE*)a_malloc(dwFileLength);
		if (!pBuffer)
			return false;

		if( 0 != Compress(pFileBuffer, dwFileLength, pBuffer, &dwCompressedLength) )
		{
			//compress error, so use uncompressed format
			dwCompressedLength = dwFileLength;
		}

		//compress2(m_pBuffer, &dwCompressedLength, pFileBuffer, dwFileLength, 1);

		if( dwCompressedLength < dwFileLength )
		{
			if( !ReplaceFileCompressed(szFileName, pBuffer, dwFileLength, dwCompressedLength) )
			{
				a_free(pBuffer);
				return false;
			}
		}
		else
		{
			if( !ReplaceFileCompressed(szFileName, pFileBuffer, dwFileLength, dwFileLength) )
			{
				a_free(pBuffer);
				return false;
			}
		}

		a_free(pBuffer);
	}
	else
	{
		if( !ReplaceFileCompressed(szFileName, pFileBuffer, dwFileLength, dwFileLength) )
			return false;
	}


	return true;
}

bool AFilePackage::ReplaceFileCompressed(const char * szFileName, LPBYTE pCompressedBuffer, DWORD dwFileLength, DWORD dwCompressedLength)
{
	if( m_bReadOnly )
	{
		AFERRLOG(("AFilePackage::ReplaceFileCompressed(), Read only package, can not replace!"));
		return false;
	}

	FILEENTRY Entry;
	int	nIndex;

	if (!GetFileEntry(szFileName, &Entry, &nIndex))
	{
		AFERRLOG(("AFilePackage::ReplaceFile(), Can not find file %s", szFileName));
		return false;
	}

	Entry.dwOffset = m_header.dwEntryOffset;
	Entry.dwLength = dwFileLength;
	Entry.dwCompressedLength = dwCompressedLength;
	if (!CheckFileEntryValid(&Entry))
	{
		// 先行检查输入参数是否合法，如果不合法，则提前返回，不做任何修改
		AFERRLOG(("AFilePackage::ReplaceFile(), Invalid File Entry"));
		return false;
	}

	FILEENTRY* pEntry = m_aFileEntries[nIndex];
	ASSERT(pEntry);
	// modify this file entry to point to the new file body;			
	pEntry->dwOffset = m_header.dwEntryOffset;
	pEntry->dwLength = dwFileLength;
	pEntry->dwCompressedLength = dwCompressedLength;

	FILEENTRYCACHE* pEntryCache = m_aFileEntryCache[nIndex];
	DWORD dwCompressedSize = sizeof(FILEENTRY);
	BYTE * pBuffer = (BYTE *)a_malloc(sizeof(FILEENTRY));
	int nRet = Compress((LPBYTE)pEntry, sizeof(FILEENTRY), pBuffer, &dwCompressedSize);
	if( nRet != 0 || dwCompressedSize >= sizeof(FILEENTRY) )
	{
		dwCompressedSize = sizeof(FILEENTRY);
		memcpy(pBuffer, pEntry, sizeof(FILEENTRY));
	}
	pEntryCache->dwCompressedLength = dwCompressedSize;
	pEntryCache->pEntryCompressed = (BYTE *)a_realloc(pEntryCache->pEntryCompressed, dwCompressedSize);
	memcpy(pEntryCache->pEntryCompressed, pBuffer, dwCompressedSize);
	a_free(pBuffer);
	
	m_fpPackageFile->seek(m_header.dwEntryOffset, SEEK_SET);

	//	We write the compressed buffer into the disk;
	Encrypt(pCompressedBuffer, dwCompressedLength);
	m_fpPackageFile->write(pCompressedBuffer, dwCompressedLength, 1);
	Decrypt(pCompressedBuffer, dwCompressedLength);
	m_header.dwEntryOffset += dwCompressedLength;

	m_bHasChanged = true;
	return true;
}

bool AFilePackage::RemoveFileFromDir(const char * filename)
{
	char szFindName[MAX_PATH];
	int nLength,i;
	char *name,*tok;
	
	strncpy(szFindName, filename, MAX_PATH);
	strlwr(szFindName);
	name = szFindName;
	nLength = strlen(szFindName);
	for(i=0; i<nLength; i++)
	{
		if( szFindName[i] == '/' )
			szFindName[i] = '\\';
	}
	tok = strtok(name,"\\");
	directory * dir = &m_directory;
	while(tok)
	{
		entry * ent = dir->SearchItem(tok);
		if(ent == NULL) return false; //entry not found
		char * next = strtok(NULL,"\\");
		if(next == NULL)
		{
			if(!ent->IsContainer())
			{
				dir->RemoveItem(tok);
				return true;
			}
			return false;
		}
		else
		{
			if(ent->IsContainer())
				dir = (directory *) ent;
			else
				return false;
		}
		tok = next;
	}
	return false;
}

AFilePackage::directory * AFilePackage::GetDirEntry(const char * szPath)
{
	char szFindName[MAX_PATH];
	int nLength,i;
	char *name,*tok;
	
	strncpy(szFindName, szPath, MAX_PATH);
	strlwr(szFindName);
	name = szFindName;
	nLength = strlen(szFindName);
	for(i=0; i<nLength; i++)
	{
		if( szFindName[i] == '/' )
			szFindName[i] = '\\';
	}
	tok = strtok(name,"\\");
	directory * dir = &m_directory;
	while(tok && *tok)
	{
		entry * ent = dir->SearchItem(tok);
		if(ent == NULL) return NULL; //entry not found
		if(!ent->IsContainer()) return NULL;
		tok = strtok(NULL,"\\");
		dir = (directory*)ent;
	}
	return dir;
}
			    
bool AFilePackage::InsertFileToDir(const char * filename,int index)
{
	char szFindName[MAX_PATH];
	int nLength,i;
	char *name,*tok;
	
	strncpy(szFindName, filename, MAX_PATH);
	strlwr(szFindName);
	name = szFindName;
	nLength = strlen(szFindName);
	for(i=0; i<nLength; i++)
	{
		if( szFindName[i] == '/' )
			szFindName[i] = '\\';
	}
	tok = strtok(name,"\\");
	directory * dir = &m_directory;
	while(tok)
	{
		char * next = strtok(NULL,"\\");
		entry * ent = dir->SearchItem(tok);
		if(next)
		{
			if(ent == NULL)
			{
				directory *tmp = new directory(tok);
				dir->AppendEntry(tmp);
				dir = tmp;
			}
			else
			{
				assert(ent->IsContainer());
				if(!ent->IsContainer())
				{
					AFERRLOG(("AFilePackage::InsertFileToDir(), Directory conflict:%s", filename));
					return false;
				}
				dir = (directory*)ent;
			}
		}
		else
		{
			if(ent == NULL)
			{
				dir->AppendEntry(new file(tok,index));
			}
			else
			{
				assert(!ent->IsContainer());
				if(ent->IsContainer())
					return false;
				else
					((file*)ent)->SetIndex(index);
				break;
			}
		}		
		tok = next;
	}
	return true;
}


// 已经添加了目录结构
bool AFilePackage::ResortEntries()
{
	m_directory.clear();
	for (int i=0; i < m_aFileEntries.GetSize(); i++)
	{
		if (m_aFileEntries[i])
		{
			InsertFileToDir(m_aFileEntries[i]->szFileName, i);
		}
	}
	return true;
}

//	Add a group of file names to file cache list
bool AFilePackage::AddCacheFileNameList(const char* szDescFile)
{
	AScriptFile ScriptFile;
	if (!ScriptFile.Open(szDescFile))
	{
		AFERRLOG(("AFilePackage::ReadCacheFileNameList, Failed to open file %s !", szDescFile));
		return false;
	}

	while (ScriptFile.GetNextToken(true))
	{
		AddCacheFileName(ScriptFile.m_szToken);
	}

	ScriptFile.Close();

	return true;
}

//	Add a file name to file cache list
bool AFilePackage::AddCacheFileName(const char* szFile)
{
	CACHEFILENAME* pCacheFile = new CACHEFILENAME;
	if (!pCacheFile)
	{
		AFERRLOG(("AFilePackage::ReadCacheFileNameList, Not enough memory !"));
		return false;
	}

	pCacheFile->strFileName = szFile;
	pCacheFile->dwFileID	= a_MakeIDFromLowString(szFile);

	if (!m_CachedFileTab.put((int)pCacheFile->dwFileID, pCacheFile))
	{
		//	Failed to put item into table, this maybe caused by file name collision
		delete pCacheFile;
		return false;
	}

	return true;
}

//	Clear file cache
void AFilePackage::ClearFileCache()
{
	SharedTable::iterator it = m_SharedFileTab.begin();
	for (; it != m_SharedFileTab.end(); )
	{
		SHAREDFILE* pFileItem = *it.value();

		//	Don't release file which is still referenced
		if (!pFileItem->iRefCnt)
		{
			a_free(pFileItem->pFileData);
			delete pFileItem;

			it = m_SharedFileTab.erase(it);
		}
		else
			++it;
	}
}

/*	Open a shared file

	Return file handle address for success, otherwise return NULL for failure

	szFileName: name of specified file
	ppFileBuf (out): receive file data buffer
	dwFileLen (out): receive file data length
*//*
DWORD AFilePackage::OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen)
{
	char szFile[MAX_PATH];
	
	//	Normalize file name
	strncpy(szFile, szFileName, MAX_PATH);
	NormalizeFileName(szFile, m_bUseShortName);
	DWORD dwFileID = a_MakeIDFromLowString(szFile);
	if (!dwFileID)
	{
		ASSERT(dwFileID);
		return 0;
	}

	ACSWrapper csa(&m_csSF);

	//	Search it in opened shared file table
	SharedTable::pair_type Pair = m_SharedFileTab.get((int)dwFileID);
	if (Pair.second)
	{
		//	This file exists in shared file table
		SHAREDFILE* pFileItem = *Pair.first;

		if (stricmp(szFileName, pFileItem->pFileEntry->szFileName))
		{
			ASSERT(0);
			return 0;
		}

		pFileItem->iRefCnt++;
		pFileItem->pFileEntry->iAccessCnt++;

		*ppFileBuf	= pFileItem->pFileData;
		*pdwFileLen	= pFileItem->dwFileLen;

		return (DWORD)pFileItem;
	}

	//	Get file entry
	FILEENTRY FileEntry;
	int iEntryIndex;
	if (!GetFileEntry(szFile, &FileEntry, &iEntryIndex))
	{
		AFERRLOG(("AFilePackage::OpenSharedFile, Failed to find file [%s] in package !", szFile));
		return false;
	}

	//	Allocate file data buffer
	BYTE* pFileData = (BYTE*)a_malloc(FileEntry.dwLength);
	if (!pFileData)
	{
		AFERRLOG(("AFilePackage::OpenSharedFile, Not enough memory!"));
		return false;
	}

	//	Read file data
	DWORD dwFileLen = FileEntry.dwLength;
	if (!ReadFile(FileEntry, pFileData, &dwFileLen))
	{
		AFERRLOG(("AFilePackage::OpenSharedFile, Failed to read file data [%s] !", szFile));
		return false;
	}

	//	The file is in cache file list ?
	CACHEFILENAME* pCachedFile = SearchCacheFileName(dwFileID);

	//	Add it to shared file arrey
	SHAREDFILE* pFileItem = new SHAREDFILE;
	if (!pFileItem)
	{
		a_free(pFileData);
		AFERRLOG(("AFilePackage::OpenSharedFile, Not enough memory!"));
		return false;
	}

	pFileItem->bCached		= pCachedFile ? true : false;
	pFileItem->dwFileID		= dwFileID;
	pFileItem->dwFileLen	= dwFileLen;
	pFileItem->iRefCnt		= 1;
	pFileItem->pFileData	= pFileData;
	pFileItem->pFileEntry	= m_aFileEntries[iEntryIndex];

	pFileItem->pFileEntry->iAccessCnt++;

	m_SharedFileTab.put((int)dwFileID, pFileItem);

	//	Change size counter
	m_dwSharedSize += dwFileLen;
	if (pFileItem->bCached)
		m_dwCacheSize += dwFileLen;

	*ppFileBuf	= pFileData;
	*pdwFileLen	= dwFileLen;

	return (DWORD)pFileItem;
}
*/
DWORD AFilePackage::OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem)
{
	//	Get file entry
	FILEENTRY FileEntry;
	int iEntryIndex;
	if (!GetFileEntry(szFileName, &FileEntry, &iEntryIndex))
	{
		if( !strstr(szFileName, "Textures") && !strstr(szFileName, "Tex_") )
		{
			AFERRLOG(("AFilePackage::OpenSharedFile, Failed to find file [%s] in package !", szFileName));
		}
		return false;
	}

	ASSERT(m_aFileEntries[iEntryIndex]);

	//	Allocate file data buffer
	BYTE* pFileData = NULL;
	if (bTempMem)
		pFileData = (BYTE*)a_malloctemp(FileEntry.dwLength);
	else
		pFileData = (BYTE*)a_malloc(FileEntry.dwLength);

	if (!pFileData)
	{
		AFERRLOG(("AFilePackage::OpenSharedFile, Not enough memory!"));
		return false;
	}

	//	Read file data
	DWORD dwFileLen = FileEntry.dwLength;
	if (!ReadFile(FileEntry, pFileData, &dwFileLen))
	{
		if (bTempMem)
			a_freetemp(pFileData);
		else
			a_free(pFileData);

		AFERRLOG(("AFilePackage::OpenSharedFile, Failed to read file data [%s] !", szFileName));
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

		AFERRLOG(("AFilePackage::OpenSharedFile, Not enough memory!"));
		return false;
	}

	pFileItem->bCached		= false;
	pFileItem->bTempMem		= bTempMem;
	pFileItem->dwFileID		= 0;
	pFileItem->dwFileLen	= dwFileLen;
	pFileItem->iRefCnt		= 1;
	pFileItem->pFileData	= pFileData;
	pFileItem->pFileEntry	= m_aFileEntries[iEntryIndex];

//	pFileItem->pFileEntry->iAccessCnt++;

	*ppFileBuf	= pFileData;
	*pdwFileLen	= dwFileLen;

	return (DWORD)pFileItem;
}

/*
//	Close a shared file
void AFilePackage::CloseSharedFile(DWORD dwFileHandle)
{
	SHAREDFILE* pFileItem = (SHAREDFILE*)dwFileHandle;
	ASSERT(pFileItem && pFileItem->iRefCnt > 0);

	ACSWrapper csa(&m_csSF);

	SharedTable::pair_type Pair = m_SharedFileTab.get((int)pFileItem->dwFileID);
	if (!Pair.second)
	{
		ASSERT(0);
		return;
	}

	pFileItem->iRefCnt--;

	if (!pFileItem->iRefCnt && !pFileItem->bCached)
	{
		//	Change size counter
		m_dwSharedSize -= pFileItem->dwFileLen;

		//	No cache file, release it
		a_free(pFileItem->pFileData);
		delete pFileItem;

		m_SharedFileTab.erase((int)pFileItem->dwFileID);
	}
}
*/

//	Close a shared file
void AFilePackage::CloseSharedFile(DWORD dwFileHandle)
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

//	Search a cache file name from list
//	Return index of file name for success, otherwise return -1
AFilePackage::CACHEFILENAME* AFilePackage::SearchCacheFileName(const char* szFileName)
{
	DWORD dwFileID = a_MakeIDFromLowString(szFileName);
	return SearchCacheFileName(dwFileID);
}

//	Search a cache file name from list
//	Return index of file name for success, otherwise return -1
AFilePackage::CACHEFILENAME* AFilePackage::SearchCacheFileName(DWORD dwFileID)
{
	CachedTable::pair_type Pair = m_CachedFileTab.get((int)dwFileID);
	if (!Pair.second)
		return NULL;

	return *Pair.first;
}

/*
	Compress a data buffer
	pFileBuffer			IN		buffer contains data to be compressed
	dwFileLength		IN		the bytes in buffer to be compressed
	pCompressedBuffer	OUT		the buffer to hold the compressed data
	pdwCompressedLength IN/OUT	the compressed buffer size when used as input
								when out, it contains the real compressed length

	RETURN: 0,		ok
			-1,		dest buffer is too small
			-2,		unknown error
*/
int AFilePackage::Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD * pdwCompressedLength)
{
	int nRet = compress2(pCompressedBuffer, pdwCompressedLength, pFileBuffer, dwFileLength, 1);
	if( Z_OK == nRet )
		return 0;

	if( Z_BUF_ERROR == nRet )
		return -1;
	else
		return -2;
}

/* 
	Uncompress a data buffer
	pCompressedBuffer	IN		buffer contains compressed data to be uncompressed
	dwCompressedLength	IN		the compressed data size
	pFileBuffer			OUT		the uncompressed data buffer
	pdwFileLength		IN/OUT	the uncompressed data buffer size as input
								when out, it is the real uncompressed data length

	RETURN: 0,		ok
			-1,		dest buffer is too small
			-2,		unknown error
*/
int AFilePackage::Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD * pdwFileLength)
{
	int nRet = Zlib_UnCompress(pFileBuffer, pdwFileLength, pCompressedBuffer, dwCompressedLength);
	if( Z_OK == nRet )
		return 0;

	if( Z_BUF_ERROR == nRet )
		return -1;
	else
		return -2;
}


/*
	Safe Header section	
*/
bool AFilePackage::LoadSafeHeader()
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

bool AFilePackage::CreateSafeHeader()
{
	m_bHasSafeHeader = true;

	m_safeHeader.tag1 = 0x4DCA23EF;
	m_safeHeader.tag2 = 0x56a089b7;
	m_safeHeader.offset = 0;

	return true;
}

bool AFilePackage::SaveSafeHeader()
{
	if( m_bHasSafeHeader )
	{
		m_fpPackageFile->seek(0, SEEK_END);
		m_safeHeader.offset = m_fpPackageFile->tell();

		m_fpPackageFile->seek(0, SEEK_SET);
		m_fpPackageFile->write(&m_safeHeader, sizeof(SAFEFILEHEADER), 1);
		m_fpPackageFile->seek(0, SEEK_SET);
	}

	return true;
}

#define ENTRY_BUFFER_SIZE		(1024 * 1024)

bool AFilePackage::SaveEntries(DWORD * pdwEntrySize)
{
	DWORD dwTotalSize = 0;

	int iNumFile = m_aFileEntries.GetSize();
	int i;

	//	Added by dyx, 2013.10.14. Remove NULL entries at first, see RemoveFile for detail.
	for (i=iNumFile-1; i >= 0; i--)
	{
		FILEENTRY* pEntry = m_aFileEntries[i];
		if (!pEntry)
		{
			m_aFileEntries.RemoveAt(i);
			ASSERT(!m_aFileEntryCache[i]);
			m_aFileEntryCache.RemoveAt(i);
		}
	}

	iNumFile = m_aFileEntries.GetSize();

	DWORD dwBufferUsed = 0;
	BYTE * pEntryBuffer = new BYTE[ENTRY_BUFFER_SIZE];
	if( NULL == pEntryBuffer )
		return false;

	// Rewrite file entries and file header here;
	m_fpPackageFile->seek(m_header.dwEntryOffset, SEEK_SET);
	for(i=0; i < iNumFile; i++)
	{
		FILEENTRY* pEntry = m_aFileEntries[i];
		FILEENTRYCACHE* pEntryCache = m_aFileEntryCache[i];

		if( dwBufferUsed + sizeof(FILEENTRY) + sizeof(DWORD) + sizeof(DWORD) > ENTRY_BUFFER_SIZE )
		{
			// flush entry buffer;
			m_fpPackageFile->write(pEntryBuffer, dwBufferUsed, 1);
			dwBufferUsed = 0;
		}

		DWORD dwCompressedSize = pEntryCache->dwCompressedLength;

		dwCompressedSize ^= AFPCK_MASKDWORD;
		memcpy(&pEntryBuffer[dwBufferUsed], &dwCompressedSize, sizeof(DWORD));
		dwBufferUsed += sizeof(DWORD);

		dwCompressedSize ^= AFPCK_CHECKMASK;
		memcpy(&pEntryBuffer[dwBufferUsed], &dwCompressedSize, sizeof(DWORD));
		dwBufferUsed += sizeof(DWORD);

		memcpy(&pEntryBuffer[dwBufferUsed], pEntryCache->pEntryCompressed, pEntryCache->dwCompressedLength);
		dwBufferUsed += pEntryCache->dwCompressedLength;
		
		dwTotalSize += sizeof(DWORD) + sizeof(DWORD) + pEntryCache->dwCompressedLength;
	}

	if( dwBufferUsed )
	{
		// flush entry buffer;
		m_fpPackageFile->write(pEntryBuffer, dwBufferUsed, 1);
		dwBufferUsed = 0;
	}		 

	delete [] pEntryBuffer;
	pEntryBuffer = NULL;

	if( pdwEntrySize )
		*pdwEntrySize = dwTotalSize;
	return true;
}


