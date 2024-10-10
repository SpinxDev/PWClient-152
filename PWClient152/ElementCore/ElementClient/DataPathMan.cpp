// DataPathMan.cpp: implementation of the DataPathMan class.
//
//////////////////////////////////////////////////////////////////////


#include "DataPathMan.h"
#include <AFileImage.h>

#define	PATHMAP_IDENTIFY		(('P'<<24) | ('M'<<16) | ('I'<<8) | 'D')

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DataPathMan::DataPathMan()
{

}

DataPathMan::~DataPathMan()
{
	Release();
}

int DataPathMan::Load(const char *szPath)
{
	Release();
	
	AFileImage file;
	if( !file.Open(szPath, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
		return 0;
	
	DWORD dwReadLen;
	unsigned long Identify;
	if( !file.Read(&Identify, sizeof(unsigned long), &dwReadLen) )
		return 0;
	if(Identify != PATHMAP_IDENTIFY) return 0;

	unsigned long id, usize;
	char szBuffer[256];
	if( !file.Read(&usize, sizeof(unsigned long), &dwReadLen) )
		return 0;

	for( unsigned long i = 0; i < usize; ++i)
	{
		unsigned int len = 0;
		memset(szBuffer,'\0',256);
		if( !file.Read(&id, sizeof(unsigned long), &dwReadLen) )
			return 0;
		if( !file.Read(&len, sizeof(unsigned int), &dwReadLen) )
			return 0;
		if( !file.Read(szBuffer, sizeof(char)*len, &dwReadLen) )
			return 0;;

		AString strPath = szBuffer;
		if (m_PathIDMap.find(strPath) != m_PathIDMap.end()
		 || m_IDPathMap.find(id) != m_IDPathMap.end())
		{
			return 0;
		}
		m_IDPathMap[id] = strPath;
		m_PathIDMap[strPath] = id;
	}
	
	file.Close();
	return 1;
}
