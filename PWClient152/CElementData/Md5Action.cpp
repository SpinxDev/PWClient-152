#include "StdAfx.h"
#include "Md5Action.h"
#include "EC_MD5Hash.h"

#include <AFile.h>
#include <vector.h>

const char* _format_md5_ = "Md: %s";

CMd5Action::CMd5Action(void)
{
}

CMd5Action::~CMd5Action(void)
{
}

bool CMd5Action::Check(const char* szPath)
{
	bool bRet = false;
	PATCH::MD5Hash md5;
	AFile file;
	if (!file.Open(szPath, AFILE_OPENEXIST | AFILE_TEXT))
		return false;
	char szBuffer[AFILE_LINEMAXLEN];
	char szLine[AFILE_LINEMAXLEN];
	DWORD dwReadLen;
	while (file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen))
	{
		szBuffer[0] = 0;
		sscanf(szLine, _format_md5_, szBuffer);
		if (0 == strlen(szBuffer))
		{
			md5.Update(szLine, dwReadLen);
			continue;
		}
		md5.Final();
		unsigned int uLen;
		md5.GetString(szLine, uLen);
		AString strMd5(szLine);
		if(0 == stricmp(szLine, szBuffer))
			bRet = true;
	}
	file.Close();
	return bRet;
}

bool CMd5Action::Generate(const char* szPath)
{
	abase::vector<AString> vecLines;
	PATCH::MD5Hash md5;

	AFile file;
	if (!file.Open(szPath, AFILE_OPENEXIST | AFILE_TEXT))
		return false;
	char szBuffer[AFILE_LINEMAXLEN];
	char szLine[AFILE_LINEMAXLEN];
	DWORD dwReadLen;
	while (file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen))
	{		
		szBuffer[0] = 0;
		sscanf(szLine, _format_md5_, szBuffer);
		if (0 == strlen(szBuffer))
		{
			md5.Update(szLine, dwReadLen);
			vecLines.push_back(szLine);
		}			
	}
	file.Close();
	md5.Final();

	unsigned int uLen;
	md5.GetString(szLine, uLen);
	AString strMd5("Md: ");
	strMd5 += szLine;
	vecLines.push_back(strMd5);

	if(!SetFileAttributes(szPath, FILE_ATTRIBUTE_NORMAL))
		return false;

	if (!file.Open(szPath, AFILE_CREATENEW | AFILE_TEXT))
		return false;

	for (size_t i=0;i<vecLines.size();++i)
	{
		file.WriteLine(vecLines[i]);
	}

	file.Close();
	return true;
}