#include "vector_string.h"
#include <memory.h>


CVectorString::CVectorString()
{
	m_pData = NULL;
}

CVectorString::~CVectorString()
{
	Release();
}

void CVectorString::Release()
{
	for(size_t i = 0; i < m_listChar.size(); ++i)
	{
		int len = m_listChar[i]->m_nPathNum;
		for( int j = 0; j < len; ++j)
		{
			delete []m_listChar[i]->m_pPathData[j]->m_pPtData;
			delete []m_listChar[i]->m_pPathData[j];
		}
		delete []m_listChar[i]->m_pPathData;
		delete m_listChar[i];
	}

	if(m_pData) delete[] m_pData;
	m_pData = NULL;
}

bool CVectorString::Save(FILE *pFile)
{ 
	int n = m_listChar.size();
	fwrite(&n,sizeof(int),1,pFile);
	for( int i = 0; i < n; ++i)
	{
		fwrite(&m_listChar[i]->m_nPathNum,sizeof(int),1,pFile);
		for( int j = 0; j < m_listChar[i]->m_nPathNum; ++j)
		{
			int len = m_listChar[i]->m_pPathData[j]->m_nPtNum;
			fwrite(&len,sizeof(int),1,pFile);
			fwrite(m_listChar[i]->m_pPathData[j]->m_pPtData,len,1,pFile);
		}
	}
	
	return true; 
}

bool CVectorString::Load(FILE *pFile)
{ 
	Release();
	
	int n;
	fread(&n,sizeof(int),1,pFile);
	for( int i = 0; i < n; ++i)
	{
		int len;
		VECTOR_CHAR *pChar = NULL;
		fread(&len,sizeof(int),1,pFile);
		if(len > 0) 
		{
			pChar = new VECTOR_CHAR;
			if(pChar==NULL) return false;
			pChar->m_nPathNum = len;
			pChar->m_pPathData = (VECTOR_PATH**)new unsigned int[len];
			if(pChar->m_pPathData==NULL) return false; 
		}

		for( int j = 0; j < len; ++j)
		{
			VECTOR_PATH* pNewPath = new VECTOR_PATH;
			fread(&pNewPath->m_nPtNum,sizeof(int),1,pFile);
			pNewPath->m_pPtData = new char[pNewPath->m_nPtNum];
			fread(pNewPath->m_pPtData,pNewPath->m_nPtNum,1,pFile);
			pChar->m_pPathData[j] = pNewPath;
		}
		if(pChar) m_listChar.push_back(pChar);
	}
	
	return true; 
}

const void* CVectorString::SaveToMem(int& rs)
{
	rs = 0;
	if(m_pData) delete[] m_pData;
	int size = GetSize();
	if(size < 0) return NULL;

	m_pData = new char[size];
	char *pMem = m_pData;
	if(m_pData)
	{
		int n = m_listChar.size();
		(*(int*)pMem) = n;
		pMem += sizeof(int);
		for( int i = 0; i < n; ++i)
		{
			(*(int*)pMem) = m_listChar[i]->m_nPathNum;
			pMem += sizeof(int);
			for( int j = 0; j < m_listChar[i]->m_nPathNum; ++j)
			{
				int len = m_listChar[i]->m_pPathData[j]->m_nPtNum;
				(*(int*)pMem) = len;
				pMem += sizeof(int);
				memcpy(pMem,m_listChar[i]->m_pPathData[j]->m_pPtData,len);
				pMem += len;
			}
		}
	}
	rs = size;
	return m_pData;
}

bool CVectorString::LoadFromMem(const void *pData)
{
	Release();
	if(pData==NULL) return false;
	char *pMem = (char*)pData;
	int n = (*(int*)pMem);
	pMem += sizeof(int);
	for( int i = 0; i < n; ++i)
	{
		VECTOR_CHAR *pChar = NULL;
		int len = (*(int*)pMem);
		pMem += sizeof(int);
		if(len > 0) 
		{
			pChar = new VECTOR_CHAR;
			if(pChar==NULL) return false;
			pChar->m_nPathNum = len;
			pChar->m_pPathData = (VECTOR_PATH**)new unsigned int[len];
			if(pChar->m_pPathData==NULL) return false; 
		}

		for( int j = 0; j < len; ++j)
		{
			VECTOR_PATH* pNewPath = new VECTOR_PATH;
			pNewPath->m_nPtNum = (*(int*)pMem);
			pMem += sizeof(int);
			pNewPath->m_pPtData = new char[pNewPath->m_nPtNum];
			memcpy(pNewPath->m_pPtData,pMem,pNewPath->m_nPtNum);
			pMem += pNewPath->m_nPtNum;
			pChar->m_pPathData[j] = pNewPath;
		}
		if(pChar) m_listChar.push_back(pChar);
	}
	
	return true; 
}

CVectorString* CVectorString::Disorder()
{
	int n = m_listChar.size();
	if(n < 1) return NULL;
	CVectorString* pNewString = new CVectorString;
	if(pNewString==NULL) return NULL;

	for( int i = 0; i < n; ++i)
	{
		int len = m_listChar[i]->m_nPathNum;
		VECTOR_CHAR *pNewChar = new VECTOR_CHAR;
		pNewChar->m_nPathNum = len;
		pNewChar->m_pPathData = (VECTOR_PATH**)new unsigned int[len];
		for( int j = 0; j < len; ++j)
		{
			VECTOR_PATH *pOldPath = m_listChar[i]->m_pPathData[j];
			if(pOldPath->m_nPtNum > 0)
			{
				VECTOR_PATH *pNewPath = new VECTOR_PATH;;
				pNewPath->m_nPtNum = pOldPath->m_nPtNum;
				pNewPath->m_pPtData = new char[pNewPath->m_nPtNum];
				memcpy(pNewPath->m_pPtData,pOldPath->m_pPtData,pNewPath->m_nPtNum);
				ProcessData(pNewPath->m_pPtData,pNewPath->m_nPtNum);
				pNewChar->m_pPathData[j] = pNewPath;
			}
		}
		pNewString->AddChar(pNewChar);
	}

	return pNewString;
}

void CVectorString::ProcessData(char *pBuffer, int& len)
{
	for( int i = 0; i < len; ++i)
	{
		int x = pBuffer[i] >> 4 & 0xf;
		int y = pBuffer[i] & 0xf;

		int rd[3] = {-1,0,1};

		int offsetx = rand() % 3;
		int offsety = rand() % 3;

		x += rd[offsetx];
		y += rd[offsety];

		if( x > 15 ) x = 15;
		if( y > 15 ) y = 15;

		if( x < 0 ) x = 0;
		if( y < 0 ) y = 0;

		pBuffer[i] = (char)x<<4 | (char)y;
	}
}

int  CVectorString::GetSize()
{
	int count = sizeof(int);
	int n = m_listChar.size();
	for( int i = 0; i < n; i++)
	{
		VECTOR_CHAR *pChar = m_listChar[i];
		int len = pChar->m_nPathNum;
		count += sizeof(int);
		for( int j = 0; j < len; ++j)
		{
			count += sizeof(int);
			count += pChar->m_pPathData[j]->m_nPtNum;
		}
	}

	return count;
}

