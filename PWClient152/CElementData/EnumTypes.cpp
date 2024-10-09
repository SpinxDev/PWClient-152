// EnumTypes.cpp: implementation of the CEnumTypes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ElementData.h"
#include "Global.h"
#include "A3D.h"
#include "AF.h"

#include "EnumTypes.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnumTypes::CEnumTypes()
{

}

CEnumTypes::~CEnumTypes()
{
	Release();
}
//****************
//TYPE NUM

//TYPE NAME
//ITEM NUM
//ITEM NAME
//****************

bool CEnumTypes::ReadTypes()
{
	ACHAR buffer[64];
	DWORD dwReadLen;
	AFile file;
	AString path = g_szWorkDir;
	path = path + "BaseData\\EnumTypes.type";
	if(!file.Open(path,AFILE_OPENEXIST))
	{
		AfxMessageBox("警告: 没有用户自定义的数据文件! BaseData\\EnumTypes.type");
		return true;
	}
	file.ReadLine(buffer,64*sizeof(ACHAR),&dwReadLen);
	int numType = atoi(buffer);
	for(int i=0; i<numType; i++)
	{
		ENUM_TYPE *type = new ENUM_TYPE;

		file.ReadLine(buffer,64*sizeof(ACHAR),&dwReadLen);
		type->strTypeName = buffer;
		file.ReadLine(buffer,64*sizeof(ACHAR),&dwReadLen);
		type->dwItemNum = (int)atoi(buffer);
		type->listItemName = new AString[type->dwItemNum];
		for(DWORD j=0; j<type->dwItemNum; j++)
		{
			file.ReadLine(buffer,64*sizeof(ACHAR),&dwReadLen);
			type->listItemName[j] = buffer;
		}
		m_listTypes.AddTail(type);
	}
	return true;
}

void CEnumTypes::Release()
{
	POSITION pos = m_listTypes.GetHeadPosition();
	while(pos)
	{
		ENUM_TYPE * pPt = (ENUM_TYPE *)m_listTypes.GetNext(pos);
		delete []pPt->listItemName;
		delete pPt;
	}
	
	m_listTypes.RemoveAll();
}

bool CEnumTypes::Find(AString str)
{
	POSITION pos = m_listTypes.GetHeadPosition();
	while(pos)
	{
		ENUM_TYPE * pPt = (ENUM_TYPE *)m_listTypes.GetNext(pos);
		if(strcmp(pPt->strTypeName,str)==0)
			return true;
	}
	return false;
}

ENUM_TYPE *CEnumTypes::GetType(AString str)
{
	POSITION pos = m_listTypes.GetHeadPosition();
	while(pos)
	{
		ENUM_TYPE * pPt = (ENUM_TYPE *)m_listTypes.GetNext(pos);
		if(strcmp(pPt->strTypeName,str)==0)
			return pPt;
	}
	return NULL;
}
