#include "Global.h"
#include "MD5Collector.h"
#include "EC_MD5Hash.h"

#include <algorithm>

FILE* OpenFile(const char *name, const char * param)
{
	FILE* f=NULL;
	int i=0;
	while(i<=10&&(f=fopen(name, param))==NULL)
	{
		Sleep(30);
		i++;
	}
	return f;
}

int GetFileSize(LPCSTR lFileName)
{
	FILE *f;
	if((f=OpenFile(lFileName,"rb"))!=NULL)
	{
		unsigned int maxsize = fseek(f,0, SEEK_END);
		maxsize = ftell(f);
		fclose(f);
		return maxsize;
	}
	else
		return -1;
}

static const int FileSizeOnDraw=8192;
BOOL CalFileMd5(LPCSTR lName,char *md5)
{
	FILE *f;
	PATCH::MD5Hash m;
	char buf[FileSizeOnDraw+1];
	int iRead;
	UINT iSize=GetFileSize(lName);
	if((f=OpenFile(lName,"rb"))!=NULL)
	{
		UINT iReadSize=0;
		do
		{
			iRead=fread(buf,sizeof(char),FileSizeOnDraw,f);
			m.Update(buf,iRead);
			iReadSize+=iRead;
		}while(iRead==FileSizeOnDraw);
		fclose(f);
	}
	UINT i=33;
	m.Final();
	m.GetString(md5,i);
	return TRUE;
}

//*********************************
//	MD5Collector Implementation
//*********************************

void MD5Collector::Clear()
{
	m_md5.clear();
}

void MD5Collector::Add(const char *szFileName)
{
	char md5[33] = {0};
	CalFileMd5(szFileName, md5);
	m_md5.push_back(md5);
}

void MD5Collector::Sort()
{
	std::sort(m_md5.begin(), m_md5.end());
}

void MD5Collector::Output(const char *szFileName)
{
	FILE *f = fopen(szFileName, "w");
	if (f){
		for (MD5Array::const_iterator cit = m_md5.begin(); cit != m_md5.end(); ++ cit)
		{
			const std::string &s = *cit;
			fprintf(f, "%s\n", s.c_str());
		}
		fflush(f);
		fclose(f);
	}
}