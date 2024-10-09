/*
 * FILE: EL_Archive.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EL_Archive.h"
#include "AMemory.h"
#include "AFile.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CELFileArchive
//	
///////////////////////////////////////////////////////////////////////////

CELFileArchive::CELFileArchive(AFile* pFile, bool bSave)
{
	m_pFile		= pFile;
	m_bSaving	= bSave;
	m_pszTemp	= NULL;
	m_pwszTemp	= NULL;
}

CELFileArchive::~CELFileArchive()
{
	if (m_pszTemp)
		delete [] m_pszTemp;

	if (m_pwszTemp)
		delete [] m_pwszTemp;
}

const char* CELFileArchive::ReadString(int* piLen/* NULL */)
{
	DWORD dwRead;
	int iLen;

	m_pFile->Read(&iLen, sizeof (int), &dwRead);
	if (iLen < 0)
		return NULL;

	if (m_pszTemp) 
		delete [] m_pszTemp;

	if (!(m_pszTemp = new char [iLen + 1]))
		return NULL;

	if (iLen > 0)
		m_pFile->Read(m_pszTemp, iLen, &dwRead);
	
	m_pszTemp[iLen] = '\0';

	if (piLen) 
		*piLen = iLen;
	
	return m_pszTemp;
}

int CELFileArchive::WriteString(const char* str)
{
	DWORD dwWrite;

	//	Write length of string
	int iLen = strlen(str);
	m_pFile->Write(&iLen, sizeof (int), &dwWrite);

	//	Write string data
	if (iLen)
		m_pFile->Write((void*)str, iLen, &dwWrite);

	return iLen;
}

const wchar_t* CELFileArchive::ReadWString(int* piLen/* NULL */)
{
	DWORD dwRead;
	int iLen;

	m_pFile->Read(&iLen, sizeof (int), &dwRead);
	if (iLen < 0)
		return NULL;

	if (m_pwszTemp) 
		delete [] m_pwszTemp;

	if (!(m_pwszTemp = new wchar_t [iLen + 1]))
		return NULL;

	if (iLen > 0)
		m_pFile->Read(m_pwszTemp, iLen * sizeof (wchar_t), &dwRead);
	
	m_pwszTemp[iLen] = '\0';

	if (piLen) 
		*piLen = iLen;
	
	return m_pwszTemp;
}

int CELFileArchive::WriteWString(const wchar_t* str)
{
	DWORD dwWrite;

	//	Write length of string
	int iLen = wcslen(str);
	m_pFile->Write(&iLen, sizeof (int), &dwWrite);

	//	Write string data
	if (iLen)
		m_pFile->Write((void*)str, iLen * sizeof (wchar_t), &dwWrite);

	return iLen;
}

bool CELFileArchive::Write(const void* pBuf, int iSize)
{
	DWORD dwWrite;
	return m_pFile->Write((void*)pBuf, iSize, &dwWrite);
}

bool CELFileArchive::Read(void* pBuf, int iSize)
{
	DWORD dwRead;
	if (!m_pFile->Read(pBuf, iSize, &dwRead) || (int)dwRead != iSize)
		return false;

	return true;
}

unsigned int CELFileArchive::GetPos()
{
	return m_pFile->GetPos();
}

bool CELFileArchive::Seek(int iOffset, int iOrigin)
{
	return m_pFile->Seek(iOffset, (AFILE_SEEK)iOrigin);
}

