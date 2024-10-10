#include "StdAfx.h"
#include "ProjectList.h"
#include "..\AudioEngine\xml\xmlcommon.h"
#include "Global.h"

static const unsigned int version = 0x100001;

ProjectList::ProjectList(void)
{
}

ProjectList::~ProjectList(void)
{
	release();
}

bool ProjectList::LoadXML(const char* szPath)
{
	SetCurrentDirectory(g_Configs.szRootPath);
	if(!GF_IsExist(szPath))
	{
		if(!SaveXML(szPath))
			return false;
	}

	release();

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, szPath, "r"))
	{
		GF_Log(LOG_ERROR, "ProjectList::Load 无法打开文件%s", szPath);
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	long lLength = ftell(pFile);
	char* pBuf = new char[lLength + 1];
	memset(pBuf, 0, lLength + 1);
	fseek(pFile, 0, SEEK_SET);
	fread(pBuf, lLength, 1, pFile);	
	fclose(pFile);
	pBuf[lLength] = 0;

	TiXmlDocument doc;
	doc.Parse(pBuf);
	delete [] pBuf;

	TiXmlNode* pNode = doc.FirstChild("ProjectList");
	if (!pNode)
		return false;

	unsigned int ver = 0;

	TiXmlElement* root = pNode->ToElement();
	QueryElement(root, "version", ver);
	int iProjectNum = 0;
	QueryElement(root, "ProjectNum", iProjectNum);

	TiXmlNode* pProjectNode = root->FirstChild("Project");
	for (int i=0; i<iProjectNum; ++i)
	{
		std::string strName;
		QueryElement(pProjectNode, "Name", strName);
		m_arrProjectName.push_back(strName.c_str());
		pProjectNode = pProjectNode->NextSibling("Project");
	}

	return true;
}

bool ProjectList::SaveXML(const char* szPath)
{
	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("ProjectList");
	doc.LinkEndChild(root);

	AddElement(root, "version", version);
	int iProjectNum = GetNum();
	AddElement(root, "ProjectNum", iProjectNum);
	for (int i=0; i<iProjectNum; ++i)
	{
		TiXmlElement* ele = new TiXmlElement("Project");
		root->LinkEndChild(ele);
		AddElement(ele, "Name", std::string(m_arrProjectName[i]));
	}
	TiXmlPrinter printer;
	doc.Accept(&printer);

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, szPath, "w"))
	{
		GF_Log(LOG_ERROR, "ProjectList::Save 无法打开文件%s", szPath);
		return false;
	}
	fwrite((LPVOID)printer.CStr(), printer.Size(), 1, pFile);
	fclose(pFile);
	return true;
}

bool ProjectList::IsExist(const char* szName)
{
	CString strName(szName);
	for (size_t i=0; i<m_arrProjectName.size(); ++i)
	{
		if (0 == strName.CollateNoCase(m_arrProjectName[i]))
			return true;
	}
	return false;
}

ProjectList& ProjectList::GetInstance()
{
	static ProjectList instance;
	return instance;
}

bool ProjectList::Add(const char* szName)
{
	m_arrProjectName.push_back(szName);
	return true;
}

bool ProjectList::Delete(const char* szName)
{
	CString strName(szName);
	for (size_t i=0; i<m_arrProjectName.size(); ++i)
	{
		if(0 == strName.CollateNoCase(m_arrProjectName[i]))
		{
			m_arrProjectName.erase(m_arrProjectName.begin() + i);
			return true;
		}
	}
	return false;
}

void ProjectList::release()
{
	m_arrProjectName.clear();
}