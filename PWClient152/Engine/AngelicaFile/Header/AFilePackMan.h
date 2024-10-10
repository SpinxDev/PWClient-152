/*
 * FILE: AFilePackMan.h
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

#ifndef _AFILEPACKMAN_H_
#define _AFILEPACKMAN_H_

#include <string.h>
#include <vector.h>
#include "AFilePackBase.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	#define AFPCK_VERSION	0x00010001
//	#define AFPCK_VERSION	0x00010002	//	Add compression
//  #define AFPCK_VERSION		0x00010003	//	The final release version on June 2002
//	#define AFPCK_VERSION		0x00020001	//	The version for element before Oct 2005
#define AFPCK_VERSION		0x00020002	//	The version with safe header

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
//	Class AFilePackMan
//
///////////////////////////////////////////////////////////////////////////

class AFilePackMan
{
public:

	//	Package open flags
	enum
	{
		OPEN_ENCRYPT		= 0x0001,	//	Encrypt flag
		OPEN_LOCALIZATION	= 0x0002,	//	Localization flag
	};

public:

	AFilePackMan();
	~AFilePackMan();

	//	Set algorithm id
	bool SetAlgorithmID(int id);

	//	Open an exist file package
	bool OpenFilePackage(const char* szPckFile, DWORD dwOpenFlags=0);
	bool OpenFilePackage(const char* szPckFile, const char* szFolder, DWORD dwOpenFlags=0);
	bool OpenFilePackageInGame(const char* szPckFile, DWORD dwOpenFlags=0);
	bool OpenFilePackageInGame(const char* szPckFile, const char* szFolder, DWORD dwOpenFlags=0);
	bool CreateFilePackage(const char* szPckFile, const char* szFolder, DWORD dwOpenFlags=0);

	//	Close an opening file package
	bool CloseFilePackage(AFilePackBase* pFilePck);
	//	Close all opening file packages
	bool CloseAllPackages();

	//	Get the number of file packages
	int GetNumPackages() { return m_FilePcks.size(); }
	//	Get the specified file package by index
	AFilePackBase* GetFilePck(int index) { return m_FilePcks[index]; }
	//	Get the package using a path, this function can be used to find specified file should
	//	be in which package. But note, this function never check localization package, because
	//	in theory every file can possiblely be in it.
	AFilePackBase* GetFilePck(const char* szPath) { return GetFilePck(szPath, false); }
	//	Get the package using a path, this function do the same thing as GetFilePck(const char* szPath)
	//	when bCheckLocalization = false. If bCheckLocalization is set to true, this function check if
	//	specified file has been existed in localization package at first, and localization package will
	//	be returned if it does.
	AFilePackBase* GetFilePck(const char* szPath, bool bCheckLocalization);
	//	Get localization package
	AFilePackBase* GetLocalizationPack() { return m_pLocalizePack; }

private:

	abase::vector<AFilePackBase*>	m_FilePcks;
	AFilePackBase*					m_pLocalizePack;	//	Localization package
};

extern AFilePackMan		g_AFilePackMan;


#endif	//	_AFILEPACKMAN_H_
