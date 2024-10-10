/*
 * FILE: AFilePackBase.h
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

#ifndef _AFILEPACKBASE_H_
#define _AFILEPACKBASE_H_

#include "ABaseDef.h"
#include "ACPlatform.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MAX_FILE_PACKAGE	0x7fffff00U

//	#define AFPCK_VERSION	0x00010001
//	#define AFPCK_VERSION	0x00010002	//	Add compression
//  #define AFPCK_VERSION		0x00010003	//	The final release version on June 2002
//	#define AFPCK_VERSION		0x00020001	//	The version for element before Oct 2005
#define AFPCK_VERSION		0x00020002	//	The version with safe header

#define AFPCK_COPYRIGHT_TAG "Angelica File Package, Perfect World Co. Ltd. 2002~2008. All Rights Reserved. "

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
//	Class AFilePackBase
//
///////////////////////////////////////////////////////////////////////////

class AFilePackBase
{
public:		//	Types

	//	Package header flags
	enum
	{
		PACKFLAG_ENCRYPT = 0x80000000,
	};

	class CPackageFile
	{
	private:
		char		m_szPath[MAX_PATH];
		char		m_szPath2[MAX_PATH];
		char		m_szMode[32];

		FILE *		m_pFile1;
		FILE *		m_pFile2;

		__int64		m_size1;
		__int64		m_size2;

		__int64		m_filePos;

	public:
		inline DWORD GetPackageFileSize() { return (DWORD)(m_size1 + m_size2); }

	public:
		CPackageFile();
		~CPackageFile();
		bool Open(const char * szFileName, const char * szMode);
		bool Phase2Open(DWORD dwOffset);
		bool Close();

		size_t read(void *buffer, size_t size, size_t count);
		size_t write(const void *buffer, size_t size, size_t count);
		void seek(__int64 offset, int origin);
		DWORD tell();
		void SetPackageFileSize(DWORD dwFileSize);
	};

public:

	AFilePackBase() {}
	virtual ~AFilePackBase() {}

	virtual bool Close() { return true; }
	virtual const char* GetFolder() { return ""; }
	virtual bool IsFileExist(const char* szFileName) { return false; }

	//	Open a shared file
	virtual DWORD OpenSharedFile(const char* szFileName, BYTE** ppFileBuf, DWORD* pdwFileLen, bool bTempMem) { return 0; }
	//	Close a shared file
	virtual void CloseSharedFile(DWORD dwFileHandle) {}
};


#endif	//	_AFILEPACKBASE_H_
