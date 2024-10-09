// FWArchive.cpp: implementation of the FWArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "FWArchive.h"
#include <AFile.h>
#include "FWObject.h"
#include <ARect.h>
#include "CodeTemplate.h"




FWArchive::FWArchive(AFile *pFile, int nMode)
{
	m_pFile = pFile;
	m_nMode = nMode;
	m_nVersion = -1;
}

FWArchive::~FWArchive()
{

}

bool FWArchive::IsStoring()
{
	return m_nMode == MODE_STORE;
}

bool FWArchive::IsLoading()
{
	return m_nMode == MODE_LOAD;
}

UINT FWArchive::Read(void *lpBuf, UINT uMax)
{
	ASSERT(IsLoading());

	DWORD dwLen = 0;
	if (!m_pFile->Read(lpBuf, uMax, &dwLen))
		throw FWExceptionArchive(FWExceptionArchive::COMMON_READ_ERROR);

	return dwLen;
}

void FWArchive::Write(void *lpBuf, UINT uLen)
{
	ASSERT(IsStoring());

	DWORD dwLen = 0;
	if (!m_pFile->Write(lpBuf, uLen, &dwLen))
		throw FWExceptionArchive(FWExceptionArchive::COMMON_WRITE_ERROR);
}

FWObject* FWArchive::ReadObject()
{
	ASSERT(IsLoading());

	FWRuntimeClass *pClass = NULL;

	BYTE flag = 0;
	*this >> flag;
	if (flag == FLAG_RUNTIME_CLASS_INDEX)
	{
		short nIndex = 0;
		*this >> nIndex;
		if (m_aryRumtimeClass.GetSize() < nIndex + 1)
			throw FWExceptionArchive(FWExceptionArchive::INVALID_RUNTIME_CLASS_INDEX);
		pClass = m_aryRumtimeClass[nIndex];
	}
	else if (flag == FLAG_RUNTIME_CLASS_CONTENT)
	{
		pClass = FWRuntimeClass::Load(*this);
		if (!pClass)
			throw FWExceptionArchive(FWExceptionArchive::INVALID_RUNTIME_CLASS_CONTENT);
		m_aryRumtimeClass.Add(pClass);
	}
	else
	{
		throw FWExceptionArchive(FWExceptionArchive::UNKNOWN_RUNTIME_CLASS_FLAG);
	}
	
	FWObject *pObj = pClass->CreateObject();
	pObj->Serialize(*this);

	return pObj;
}

void FWArchive::WriteObject(const FWObject* pObj)
{
	ASSERT(IsStoring());

	FWRuntimeClass *pClass = pObj->GetRuntimeClass();

	int nIndex = FindInRuntimeClassArray(pClass);
	if (nIndex != -1)
	{
		*this << (BYTE) FLAG_RUNTIME_CLASS_INDEX;
		*this << (short) nIndex;
	}
	else
	{
		*this << (BYTE) FLAG_RUNTIME_CLASS_CONTENT;
		pClass->Store(*this);
		m_aryRumtimeClass.Add(pClass);
	}

	const_cast<FWObject *>(pObj)->Serialize(*this);
}

void FWArchive::WriteStringA(LPCSTR lpsz)
{
	// write string with terminating char
	Write((void *)lpsz, strlen(lpsz) + 1);
}

void FWArchive::WriteStringW(LPCWSTR lpwsz)
{
	if (m_nVersion == 0 || m_nVersion == 1)
		WriteStringA(WC2AS(lpwsz));
	else
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, lpwsz, -1, NULL, 0, NULL, NULL);
		char* o = new char[len];
		WideCharToMultiByte(CP_UTF8, 0, lpwsz, -1, o, len, NULL, NULL);
		WriteStringA(o);
		delete[] o;
	}
}

LPSTR FWArchive::ReadStringA(LPSTR lpsz, UINT uMax)
{

	int nRead = 0;
	do
	{
		try
		{
			Read(&lpsz[nRead++], 1);
		}
		catch (...)
		{
			// end of file or other critical error
			return NULL;
		}
	// exit loop until read terminating char 
	// or the buffer is full
	} while(lpsz[nRead - 1] && nRead < (int)uMax);

	// always fill last char with 0, so it is always
	// a valid string
	if (nRead == (int)uMax)
		lpsz[nRead - 1] = '\0';

	return lpsz;
}

LPWSTR FWArchive::ReadStringW(LPWSTR lpwsz, UINT uMax)
{
//	int nRead = 0;
//	do
//	{
//		try
//		{
//			Read(&lpwsz[nRead++], 2);
//		}
//		catch (...)
//		{
//			return NULL;
//		}  
//	} while(lpwsz[nRead - 1] && nRead < uMax);
//	
//	if (nRead == uMax)
//		lpwsz[nRead - 1] = '\0'; 
//
//	return lpwsz;
	UINT uMaxA = uMax * 2 - 1;
	LPSTR tmp = new char[uMaxA];
	if (!ReadStringA(tmp, uMaxA))
	{
		SAFE_DELETE_ARRAY(tmp);
		return NULL;
	}

	if (m_nVersion == 0 || m_nVersion == 1)
	{
		ACString str = AS2WC(tmp);
		wcscpy(lpwsz, str);
	}
	else
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, tmp, -1, NULL, 0);

		if (len <= (int)uMax)
			MultiByteToWideChar(CP_UTF8, 0, tmp, -1, lpwsz, len);
		else
			lpwsz[0] = 0;
	}

	SAFE_DELETE_ARRAY(tmp);
	return lpwsz;
}

bool FWArchive::ReadStringACString(ACString & strString)
{
	strString = _AL("");

	const int nMaxSizeOnce = 128;
	LPTSTR lpsz, lpszResult;
	int nLen;
	lpsz = strString.GetBuffer(nMaxSizeOnce);
	while (1)
	{
		lpszResult = ReadString(lpsz, nMaxSizeOnce + 1);
		strString.ReleaseBuffer();

		if (!lpszResult || (nLen = a_strlen(lpszResult) < nMaxSizeOnce))
			break;

		nLen = strString.GetLength();
		lpsz = strString.GetBuffer(nLen + nMaxSizeOnce)  + nLen;
	}

	return lpszResult != NULL;
}

bool FWArchive::ReadStringAString(AString & strString)
{
	strString = "";
	
	const int nMaxSizeOnce = 128;
	LPSTR lpsz, lpszResult;
	int nLen;
	lpsz = strString.GetBuffer(nMaxSizeOnce);
	while (1)
	{
		lpszResult = ReadStringA(lpsz, nMaxSizeOnce + 1);
		strString.ReleaseBuffer();
		
		if (!lpszResult || (nLen = strlen(lpszResult) < nMaxSizeOnce))
			break;
		
		nLen = strString.GetLength();
		lpsz = strString.GetBuffer(nLen + nMaxSizeOnce)  + nLen;
	}
	
	return lpszResult != NULL;
}

void FWArchive::Flush()
{
	// since we implement no cache for FWArchive
	// there's nothing Flush() should do
}


int FWArchive::FindInRuntimeClassArray(FWRuntimeClass *pClass)
{
	int nCount = m_aryRumtimeClass.GetSize();
	for (int i = 0; i < nCount; i++)
		if (m_aryRumtimeClass[i] == pClass)
			return i;
	return -1;
}
