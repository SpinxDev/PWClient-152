                     /*
 * FILE: AFilePackage.h
 *
 * DESCRIPTION: A File Package Class for Angelica Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
   2002/5/30 --- Add compression ability
				struct FILEENTRY add the 4th element: dwOriginLength
				RemoveFile, ReplaceFile added
				version upgraded to 1.2
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFILEPACKAGE_H_
#define _AFILEPACKAGE_H_

#include <string.h>
#include <vector.h>
#include "AArray.h"
#include "AString.h"
#include "hashtab.h"
#include "AFilePackBase.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//	Class AFilePackage
//
///////////////////////////////////////////////////////////////////////////

class AFilePackage : public AFilePackBase
{
public:

	class entry
	{
	public:
		char *_name;
		entry():_name(NULL){}
		entry(const char * name){_name = new char[strlen(name)+1];strcpy(_name,name);}
		virtual ~entry() {if(_name) delete[]_name;}
		virtual bool IsContainer() = 0;
		virtual int GetIndex() = 0;
		virtual entry * SearchItem(const char *) = 0;
	};
	class directory : public entry
	{
		abase::vector<entry *> _list;
		int searchItemIndex(const char * name,int * pos);
	public:
		directory(const char * name):entry(name){}
		directory(){}
		~directory();	
		int clear();
		virtual bool IsContainer() {return true;}
		virtual int GetIndex() {return -1;}
		virtual entry * SearchItem(const char *);
	public:
		entry * GetItem(int index);
		int GetEntryCount() { return _list.size();}
		int RemoveItem(const char * name);
		int AppendEntry(entry * );
		int SearchEntry(const char * filename);
	};
	class file : public entry
	{
		int _index;
	public:
		file(const char * name,int index):entry(name),_index(index){}
		virtual bool IsContainer() {return false;}
		virtual entry * SearchItem(const char *) {return NULL;}
		virtual int GetIndex() { return _index;}
		void SetIndex(int index) {_index = index;}
	};

public:		//	Types

	enum OPENMODE
	{
		OPENEXIST = 0,
		CREATENEW = 1
	};

	struct FILEENTRYCACHE
	{
		DWORD		dwCompressedLength;	//	The compressed file entry length
		BYTE *		pEntryCompressed;	//	The compressed file entry data
	};

	struct FILEENTRY
	{
		char	szFileName[MAX_PATH];	//	The file name of this entry; this may contain a path;
		DWORD	dwOffset;				//	The offset from the beginning of the package file;
		DWORD	dwLength;				//	The length of this file;
		DWORD	dwCompressedLength;		//	The compressed data length;

		int		iAccessCnt;				//	Access counter used by OpenSharedFile
	};

	struct FILEHEADER
	{
		DWORD	guardByte0;				//	0xabcdefab
		DWORD	dwVersion;				//	Composed by two word version, major part and minor part;
		DWORD	dwEntryOffset;			//	The entry list offset from the beginning;
		DWORD	dwFlags;				//	package flags. the highest bit means the encrypt state;
		char	szDescription[252];		//	Description
		DWORD	guardByte1;				//	0xffeeffee
	};

	//	Share read file item
	struct SHAREDFILE
	{
		DWORD	dwFileID;		//	File ID
		bool	bCached;		//	Cached flag
		bool	bTempMem;		//	true, use temporary memory alloctor
		int		iRefCnt;		//	Reference counter
		BYTE*	pFileData;		//	File data buffer
		DWORD	dwFileLen;		//	File data length

		FILEENTRY*	pFileEntry;	//	Point to file entry
	};

	//	Cache file name
	struct CACHEFILENAME
	{
		AString	strFileName;	//	File name
		DWORD	dwFileID;		//	File ID
	};

	//	Safe Header
	struct SAFEFILEHEADER
	{
		DWORD	tag1;			//	tag1 of safe header, current it is 0x4DCA23EF
		DWORD	offset;			//	offset of real entries
		DWORD	tag2;			//	tag2 of safe header, current it is 0x56a089b7
	};

	typedef abase::hashtab<SHAREDFILE*, int, abase::_hash_function> SharedTable;
	typedef abase::hashtab<CACHEFILENAME*, int, abase::_hash_function> CachedTable;

	friend class AFilePackMan;
	friend class AFilePackGame;

private:

	bool		m_bHasChanged;
	bool		m_bReadOnly;
	bool		m_bUseShortName;	//	Get rid of m_szFolder in each file name
	
	FILEHEADER	m_header;
	OPENMODE	m_mode;

	APtrArray<FILEENTRY*>	m_aFileEntries;		//	File entries
	APtrArray<FILEENTRYCACHE*>	m_aFileEntryCache;	// File entries cache
	CachedTable				m_CachedFileTab;	//	Cached file table
	SharedTable				m_SharedFileTab;	//	Shared file table
	CRITICAL_SECTION		m_csFR;				//	File Read lock
	CRITICAL_SECTION		m_csSF;				//	Shared File lock

	CPackageFile* m_fpPackageFile;
	char		m_szPckFileName[MAX_PATH];	// the package file path
	char		m_szFolder[MAX_PATH];	// the folder path (in lowercase) the package packs.

	directory	m_directory;	//	the ROOT of directory tree. 
	DWORD		m_dwCacheSize;	//	Size counter of all cached files
	DWORD		m_dwSharedSize;	//	Size counter of all shared files

	bool			m_bHasSafeHeader;	// flag indicates whether the package contains a safe header
	SAFEFILEHEADER	m_safeHeader;		// Safe file header

public:

	AFilePackage();
	virtual ~AFilePackage();

	bool Open(const char* szPckPath, OPENMODE mode, bool bEncrypt=false);
	bool Open(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt=false);
	virtual bool Close();

	/*	Append a file into the package
		parameter:
			IN: szFileName		file name
			IN: pFileBuffer		the buffer containing file content
			IN: dwFileLength	the length of the buffer
			IN: bCompress		true, compress file
	*/
	bool AppendFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress);
	bool AppendFileCompressed(const char * szFileName, LPBYTE pCompressedBuffer, DWORD dwFileLength, DWORD dwCompressedLength);

	/*
		Remove a file from the package, we will only remove the file entry from the package;
		the file's data will remain in the package
		parameter:
			IN: szFileName		file name
	*/
	bool RemoveFile(const char* szFileName);

	/*
		Replace a file in the package, we will only replace the file entry in the package;
		the old file's data will remain in the package
		parameter:
			IN: szFileName		file name
			IN: pFileBuffer		the buffer containing file content
			IN: dwFileLength	the length of the buffer
			IN: bCompress		true, compress file
	*/
	bool ReplaceFile(const char* szFileName, LPBYTE pFileBuffer, DWORD dwFileLength, bool bCompress);
	bool ReplaceFileCompressed(const char * szFileName, LPBYTE pCompressedBuffer, DWORD dwFileLength, DWORD dwCompressedLength);

	// Sort the file entry list;
	bool ResortEntries();

	bool ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen);
	bool ReadFile(FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD* pdwBufferLen);

	bool ReadCompressedFile(const char * szFileName, LPBYTE pCompressedBuffer, DWORD * pdwBufferLen);
	bool ReadCompressedFile(FILEENTRY& fileEntry, LPBYTE pCompressedBuffer, DWORD * pdwBufferLen);

	// Find a file entry;
	// return true if found, false if not found;
	bool GetFileEntry(const char* szFileName, FILEENTRY* pFileEntry, int* pnIndex=NULL);
	const FILEENTRY* GetFileEntryByIndex(int nIndex) const { return m_aFileEntries[nIndex]; }
	
	directory* GetDirEntry(const char* szPath);

	//	Clear file cache
	void ClearFileCache();
	//	Open a shared file
	virtual DWORD OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem);
	//	Close a shared file
	virtual void CloseSharedFile(DWORD dwFileHandle);

	//	Get current cached file total size
	DWORD GetCachedFileSize() const { return m_dwCacheSize; }
	//	Get current shared file total size
	DWORD GetSharedFileSize() const { return m_dwSharedSize; }

	int GetFileNumber() const { return m_aFileEntries.GetSize(); }
	FILEHEADER GetFileHeader() const { return m_header; }
	virtual const char * GetFolder() { return m_szFolder; }
	const char* GetPckFileName() { return m_szPckFileName; }
	virtual bool IsFileExist(const char* szFileName);

	DWORD GetPackageFileSize() { return m_fpPackageFile->GetPackageFileSize(); }

public:
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
	static int Compress(LPBYTE pFileBuffer, DWORD dwFileLength, LPBYTE pCompressedBuffer, DWORD * pdwCompressedLength);

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
	static int Uncompress(LPBYTE pCompressedBuffer, DWORD dwCompressedLength, LPBYTE pFileBuffer, DWORD * pdwFileLength);

	//	Add a group of file names to file cache list
	bool AddCacheFileNameList(const char* szDescFile);
	//	Add a file name to file cache list
	bool AddCacheFileName(const char* szFile);

protected:	//	Attributes

protected:	//	Operations

	//	Normalize file name
	static bool NormalizeFileName(char* szFileName);
	bool NormalizeFileName(char* szFileName, bool bUseShortName);
	//	Get rid of folder from file
	void GetRidOfFolder(const char* szInName, char* szOutName);

	bool InnerOpen(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt, bool bShortName);

	//	Append a file into directroy
	bool RemoveFileFromDir(const char * filename);
	bool InsertFileToDir(const char * filename,int index);

	//	Search a cache file name from table
	CACHEFILENAME* SearchCacheFileName(const char* szFileName);
	CACHEFILENAME* SearchCacheFileName(DWORD dwFileID);

	//	Save file entries
	bool SaveEntries(DWORD * pdwEntrySize=NULL);
	void Encrypt(LPBYTE pBuffer, DWORD dwLength);
	void Decrypt(LPBYTE pBuffer, DWORD dwLength);

	//	Safe header
	bool LoadSafeHeader();
	bool SaveSafeHeader();
	bool CreateSafeHeader();
};

extern AFilePackMan		g_AFilePackMan;

#endif	//	AFILEPACKAGE_H_
