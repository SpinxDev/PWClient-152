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

#include "AFilePackMan.h"
#include "AFilePackage.h"
#include "AFilePackGame.h"
#include "AFI.h"
#include "AFPI.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

int	AFPCK_GUARDBYTE0 = 0xfdfdfeee;
int	AFPCK_GUARDBYTE1 = 0xf00dbeef;
int AFPCK_MASKDWORD  = 0xa8937462;
int	AFPCK_CHECKMASK	 = 0x59374231;

AFilePackMan g_AFilePackMan;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	AFilePackMan
//
///////////////////////////////////////////////////////////////////////////

AFilePackMan::AFilePackMan() : 
m_FilePcks(),
m_pLocalizePack(NULL)
{
}

AFilePackMan::~AFilePackMan()
{
	CloseAllPackages();
}

bool AFilePackMan::CreateFilePackage(const char* szPckFile, const char* szFolder, DWORD dwOpenFlags/* 0 */)
{
	bool bEncrypt = (dwOpenFlags & OPEN_ENCRYPT) ? true : false;
	bool bLocalization = (dwOpenFlags & OPEN_LOCALIZATION) ? true : false;

	if (bLocalization && m_pLocalizePack)
	{
		//	Only one localization package can exist at one moment
		AFERRLOG(("AFilePackMan::CreateFilePackage(), Localization package has existed!"));
		return false;
	}

	AFilePackage * pFilePackage;
	if (!(pFilePackage = new AFilePackage))
	{
		AFERRLOG(("AFilePackMan::CreateFilePackage(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, szFolder, AFilePackage::CREATENEW, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::CreateFilePackage(), Can not open package [%s]", szPckFile));
		return false;
	}

	if (bLocalization)
		m_pLocalizePack = pFilePackage;
	else
		m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackage(const char* szPckFile, DWORD dwOpenFlags/* 0 */)
{
	bool bEncrypt = (dwOpenFlags & OPEN_ENCRYPT) ? true : false;
	bool bLocalization = (dwOpenFlags & OPEN_LOCALIZATION) ? true : false;

	if (bLocalization && m_pLocalizePack)
	{
		//	Only one localization package can exist at one moment
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Localization package has existed!"));
		return false;
	}

	AFilePackage* pFilePackage;
	if (!(pFilePackage = new AFilePackage))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
		return false;
	}

	if (bLocalization)
	{
		if (!pFilePackage->Open(szPckFile, "", AFilePackage::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
			return false;
		}

		m_pLocalizePack = pFilePackage;
	}
	else
	{
		if (!pFilePackage->Open(szPckFile, AFilePackage::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
			return false;
		}

		m_FilePcks.push_back(pFilePackage);
	}

	return true;
}

bool AFilePackMan::OpenFilePackage(const char* szPckFile, const char* szFolder, DWORD dwOpenFlags/* 0 */)
{
	bool bEncrypt = (dwOpenFlags & OPEN_ENCRYPT) ? true : false;
	bool bLocalization = (dwOpenFlags & OPEN_LOCALIZATION) ? true : false;

	if (bLocalization && m_pLocalizePack)
	{
		//	Only one localization package can exist at one moment
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Localization package has existed!"));
		return false;
	}

	AFilePackage* pFilePackage;
	if (!(pFilePackage = new AFilePackage))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, szFolder, AFilePackage::OPENEXIST, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackage(), Can not open package [%s]", szPckFile));
		return false;
	}

	if (bLocalization)
		m_pLocalizePack = pFilePackage;
	else
		m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::OpenFilePackageInGame(const char* szPckFile, DWORD dwOpenFlags/* 0 */)
{
	bool bEncrypt = (dwOpenFlags & OPEN_ENCRYPT) ? true : false;
	bool bLocalization = (dwOpenFlags & OPEN_LOCALIZATION) ? true : false;

	if (bLocalization && m_pLocalizePack)
	{
		//	Only one localization package can exist at one moment
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Localization package has existed!"));
		return false;
	}

	AFilePackGame* pFilePackage;
	if (!(pFilePackage = new AFilePackGame))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Not enough memory!"));
		return false;
	}

	if (bLocalization)
	{
		if (!pFilePackage->Open(szPckFile, "", AFilePackGame::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Can not open package [%s]", szPckFile));
			return false;
		}

		m_pLocalizePack = pFilePackage;
	}
	else
	{
		if (!pFilePackage->Open(szPckFile, AFilePackGame::OPENEXIST, bEncrypt))
		{
			delete pFilePackage;
			AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Can not open package [%s]", szPckFile));
			return false;
		}

		m_FilePcks.push_back(pFilePackage);
	}

	return true;
}

bool AFilePackMan::OpenFilePackageInGame(const char* szPckFile, const char* szFolder, DWORD dwOpenFlags/* 0 */)
{
	bool bEncrypt = (dwOpenFlags & OPEN_ENCRYPT) ? true : false;
	bool bLocalization = (dwOpenFlags & OPEN_LOCALIZATION) ? true : false;

	if (bLocalization && m_pLocalizePack)
	{
		//	Only one localization package can exist at one moment
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Localization package has existed!"));
		return false;
	}

	AFilePackGame* pFilePackage;
	if (!(pFilePackage = new AFilePackGame))
	{
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Not enough memory!"));
		return false;
	}

	if (!pFilePackage->Open(szPckFile, szFolder, AFilePackGame::OPENEXIST, bEncrypt))
	{
		delete pFilePackage;
		AFERRLOG(("AFilePackMan::OpenFilePackageInGame(), Can not open package [%s]", szPckFile));
		return false;
	}

	if (bLocalization)
		m_pLocalizePack = pFilePackage;
	else
		m_FilePcks.push_back(pFilePackage);

	return true;
}

bool AFilePackMan::CloseFilePackage(AFilePackBase* pFilePck)
{
	if (!pFilePck)
		return true;

	if (m_pLocalizePack == pFilePck)
	{
		pFilePck->Close();
		delete pFilePck;
		m_pLocalizePack = NULL;
		return true;
	}
	else
	{
		int nCount = m_FilePcks.size();
		for (int i=0; i < nCount; i++)
		{
			if (m_FilePcks[i] == pFilePck)
			{
				m_FilePcks.erase(&m_FilePcks[i]);

				pFilePck->Close();
				delete pFilePck;
				return true;
			}
		}
	}

	return true;
}

bool AFilePackMan::CloseAllPackages()
{
	if (m_pLocalizePack)
	{
		m_pLocalizePack->Close();
		delete m_pLocalizePack;
		m_pLocalizePack = NULL;
	}

	int nCount = m_FilePcks.size();
	for (int i=0; i < nCount; i++)
	{
		AFilePackBase* pFilePck = m_FilePcks[i];
		pFilePck->Close();
		delete pFilePck;
	}

	m_FilePcks.clear();
	
	return true;
}

AFilePackBase* AFilePackMan::GetFilePck(const char* szPath, bool bCheckLocalization)
{
	if (bCheckLocalization && m_pLocalizePack)
	{
		//	Check if specified file 'has existed' in localization package or not. 'Has existed'
		//	means that we can't decide if a new file should be added to localization package, because
		//	in theory every file can possiblely be added to the package.
		if (m_pLocalizePack->IsFileExist(szPath))
			return m_pLocalizePack;
	}

	//	Normalize file name
	char szLowPath[MAX_PATH];
	strncpy(szLowPath, szPath, MAX_PATH);
	_strlwr(szLowPath);
	AFilePackage::NormalizeFileName(szLowPath);

	//	Check other packages through file path
	size_t nCount = m_FilePcks.size();
	for (size_t i=0; i < nCount; i++)
	{
		AFilePackBase* pPack = m_FilePcks[i];
		
		if (strstr(szLowPath, pPack->GetFolder()) == szLowPath)
		{
			return pPack;
		}
	}

	return NULL;
}

bool AFilePackMan::SetAlgorithmID(int id)
{
	switch(id)
	{
	case 111:
		AFPCK_GUARDBYTE0 = 0xab12908f;
		AFPCK_GUARDBYTE1 = 0xb3231902;
		AFPCK_MASKDWORD  = 0x2a63810e;
		AFPCK_CHECKMASK	 = 0x18734563;
		break;

	default:
		AFPCK_GUARDBYTE0 = 0xfdfdfeee + id * 0x72341f2;
		AFPCK_GUARDBYTE1 = 0xf00dbeef + id * 0x1237a73;
		AFPCK_MASKDWORD  = 0xa8937462 + id * 0xab2321f;
		AFPCK_CHECKMASK	 = 0x59374231 + id * 0x987a223;
		break;
	}

	return true;
}

