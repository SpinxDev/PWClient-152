/*
 * FILE: AFilePackGame.h
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

#ifndef _AFILEPACKGAME_H_
#define _AFILEPACKGAME_H_

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
//	Class AFilePackGame
//
///////////////////////////////////////////////////////////////////////////

class AFilePackGame : public AFilePackBase
{
public:		//	Types

	enum OPENMODE
	{
		OPENEXIST = 0,
		CREATENEW = 1
	};
	
	struct FILEENTRY
	{
		char*	szFileName;				//	The file name of this entry; this may contain a path;
		DWORD	dwOffset;				//	The offset from the beginning of the package file;
		DWORD	dwLength;				//	The length of this file;
		DWORD	dwCompressedLength;		//	The compressed data length;
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

	//	Name buffer info
	struct NAMEBUFFER
	{
		char*	pBuffer;
		DWORD	dwLength;		//	Buffer length
		DWORD	dwOffset;		//	Current offset
	};

	friend class AFilePackMan;

	typedef abase::hashtab<FILEENTRY*, int, abase::_hash_function> FileEntryTable;

private:

	bool		m_bReadOnly;
	bool		m_bUseShortName;	//	Get rid of m_szFolder in each file name
	
	FILEHEADER	m_header;
	OPENMODE	m_mode;

	FILEENTRY*				m_aFileEntries;			//	File entries
	int						m_iNumEntry;			//	Number of file entry
	FileEntryTable			m_FileQuickSearchTab;	//	Quick search table when OM_ID_SEARCH is set
	APtrArray<FILEENTRY*>	m_aIDCollisionFiles;	//	ID collision file
	AArray<NAMEBUFFER>		m_aNameBufs;			//	Entry file name buffer
	CRITICAL_SECTION		m_csFR;					//	File Read lock

	CPackageFile* m_fpPackageFile;
	char		m_szPckFileName[MAX_PATH];	// the package file path
	char		m_szFolder[MAX_PATH];	// the folder path (in lowercase) the package packs.

	bool			m_bHasSafeHeader;	// flag indicates whether the package contains a safe header
	SAFEFILEHEADER	m_safeHeader;		// Safe file header

public:

	AFilePackGame();
	virtual ~AFilePackGame();

	bool Open(const char* szPckPath, OPENMODE mode, bool bEncrypt=false);
	bool Open(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt=false);
	virtual bool Close();

	//	Sort the file entry list;
	bool ResortEntries();

	bool ReadFile(const char* szFileName, LPBYTE pFileBuffer, DWORD* pdwBufferLen);
	bool ReadFile(FILEENTRY& fileEntry, LPBYTE pFileBuffer, DWORD* pdwBufferLen);
	bool ReadCompressedFile(FILEENTRY& fileEntry, LPBYTE pCompressedBuffer, DWORD * pdwBufferLen);

	// Find a file entry;
	// return true if found, false if not found;
	FILEENTRY* GetFileEntry(const char* szFileName);
	const FILEENTRY* GetFileEntryByIndex(int nIndex) const { return &m_aFileEntries[nIndex]; }
	//	Find a file in ID collision candidate array
	FILEENTRY* FindIDCollisionFile(const char* szFileName);
	
	//	Open a shared file
	virtual DWORD OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem);
	//	Close a shared file
	virtual void CloseSharedFile(DWORD dwFileHandle);

	int GetFileNumber() const { return m_iNumEntry; }
	FILEHEADER GetFileHeader() const { return m_header; }
	virtual const char* GetFolder() { return m_szFolder; }
	const char* GetPckFileName() { return m_szPckFileName; }
	virtual bool IsFileExist(const char* szFileName);

	DWORD GetPackageFileSize() { return m_fpPackageFile->GetPackageFileSize(); }

protected:	//	Attributes

protected:	//	Operations

	//	Normalize file name
	bool NormalizeFileName(char* szFileName, bool bUseShortName);
	//	Get rid of folder from file
	void GetRidOfFolder(const char* szInName, char* szOutName);

	bool InnerOpen(const char* szPckPath, const char* szFolder, OPENMODE mode, bool bEncrypt, bool bShortName);

	//	Save file entries
	void Encrypt(LPBYTE pBuffer, DWORD dwLength);
	void Decrypt(LPBYTE pBuffer, DWORD dwLength);
	//	Safe header
	bool LoadSafeHeader();

	//	Allocate new name
	char* AllocFileName(const char* szFile, int iEntryCnt, int iEntryTotalNum);
};


#endif	//	_AFILEPACKGAME_H_
