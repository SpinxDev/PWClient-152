
#ifndef __VECTOR_STRING__
#define __VECTOR_STRING__

#include <vector.h>
#include <stdio.h>

struct VECTOR_PATH
{
	int   m_nPtNum;
	char* m_pPtData;
};

struct VECTOR_CHAR
{
	int m_nPathNum;
	VECTOR_PATH** m_pPathData;
};

class CVectorString
{
public:
	CVectorString();
	~CVectorString();

public:
	void AddChar(VECTOR_CHAR* pChar){ m_listChar.push_back(pChar); }
	int GetCharNum(){ return m_listChar.size(); }
	VECTOR_CHAR* GetChar(int idx){ return ((size_t)idx < m_listChar.size() && idx >=0) ? m_listChar[idx] : NULL;}
	CVectorString* Disorder();
	bool Save(FILE *pFile);
	bool Load(FILE *pFile);
	int  GetSize();

	const void* SaveToMem(int& rs);
	bool LoadFromMem(const void *pData);
	
protected:
	void Release();
	void ProcessData(char *pBuffer, int& len);
	
	char* m_pData;
	abase::vector<VECTOR_CHAR*> m_listChar;
};



#endif