#include "StdAfx.h"
#include "BaseDataIDMan.h"
#include "AFile.h"
#include "WinBase.h"
#include "Global.h"
#include "VssOperation.h"

const char* _format_id_type = "IDType = %s";
const char* _format_id = "ID = %u, Path = %s";

const char* _base_type	= "BaseType";
const char* _ext_type	= "ExtType";

BaseDataIDManBase g_BaseIDMan;
BaseDataIDManBase g_TaskIDMan;
BaseDataIDManBase g_TalkIDMan;
BaseDataIDManBase g_PolicyIDMan;
BaseDataIDManBase g_FaceIDMan;
BaseDataIDManBase g_RecipeIDMan;
BaseDataIDManBase g_ConfigIDMan;
ExtDataIDMan g_ExtBaseIDMan;


bool BaseDataIDManBase::IsRightType(const char* szType) const
{
	return strcmp(szType, _base_type) == 0;
}

const char* BaseDataIDManBase::GetType() const
{
	return _base_type;
}

int BaseDataIDManBase::Load(
	const char* szPath,
	BaseDataPathIDMap& PathMap,
	BaseDataIDPathMap& IDMap,
	unsigned long& ulMaxID)
{
	AFile file;
	if (!file.Open(szPath, AFILE_OPENEXIST | AFILE_TEXT))
		return -1;

	ulMaxID = 0;
	char	szLine[AFILE_LINEMAXLEN];
	char	szBuf[AFILE_LINEMAXLEN];
	DWORD	dwRead;

	szLine[0] = '\0';
	if (!file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead))
	{
		file.Close();
		return -1;
	}

	szBuf[0] = '\0';
	sscanf(szLine, _format_id_type, szBuf);

	if (!IsRightType(szBuf))
	{
		file.Close();
		return -1;
	}

	while (true)
	{
		szLine[0] = '\0';
		if (!file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead)
		|| strlen(szLine) == 0) break;

		unsigned long ulID = 0;
		szBuf[0] = '\0';

		sscanf(szLine, _format_id, &ulID, szBuf);
		if (ulID == 0 || strlen(szBuf) == 0) continue;
		
		AString strPath = szBuf;
		if (IDMap.find(ulID) != IDMap.end()
		 || PathMap.find(strPath) != PathMap.end())
		{
			file.Close();
			return -2;
		}

		IDMap[ulID] = strPath;
		PathMap[strPath] = ulID;

		if (ulID > ulMaxID) ulMaxID = ulID;
	}

	file.Close();
	return 0;
}

int BaseDataIDManBase::Load(const char* szPath)
{
	unsigned long ulMaxID;
	int nRet = Load(szPath, m_PathIDMap, m_IDPathMap, ulMaxID);
	if (nRet != 0) return nRet;
	m_ulMaxID = ulMaxID;
	m_strFile = szPath;
	return 0;
}

unsigned long BaseDataIDManBase::CreateID(const AString& strPath)
{
	if (strPath.FindOneOf("\n ") >= 0) return 0;
	if (!CheckOut(m_strFile)) return 0;
	
	if(!g_VSS.IsCheckOutStatus(CString(m_strFile))) return 0;

	BaseDataPathIDMap PathMap;
	BaseDataIDPathMap IDMap;
	unsigned long ulMaxID;

	if (Load(m_strFile, PathMap, IDMap, ulMaxID) != 0
	 || PathMap.find(strPath) != PathMap.end())
	{
		CheckIn(m_strFile);
		return 0;
	}

	ulMaxID = IncID(ulMaxID);
	IDMap[ulMaxID] = strPath;
	PathMap[strPath] = ulMaxID;

	m_PathIDMap.clear();
	m_IDPathMap.clear();

	for (BaseDataPathIDMap::iterator iPath = PathMap.begin(); iPath != PathMap.end(); ++iPath)
		m_PathIDMap[iPath->first] = iPath->second;

	for (BaseDataIDPathMap::iterator iID = IDMap.begin(); iID != IDMap.end(); ++iID)
		m_IDPathMap[iID->first] = iID->second;

	m_ulMaxID = ulMaxID;

	Save();
	CheckIn(m_strFile);
	if(g_VSS.IsCheckOutStatus(CString(m_strFile))) return 0;

	return ulMaxID;
}

void BaseDataIDManBase::WipeOffByPath(abase::vector<AString>& WipeOffArray)
{
	if (!CheckOut(m_strFile)) return;

	BaseDataPathIDMap PathMap;
	BaseDataIDPathMap IDMap;
	unsigned long ulMaxID;

	if (Load(m_strFile, PathMap, IDMap, ulMaxID) != 0) return;

	for (size_t i = 0; i < WipeOffArray.size(); i++)
	{
		BaseDataPathIDMap::iterator it = PathMap.find(WipeOffArray[i]);
		if (it == PathMap.end()) continue;

		ASSERT(IDMap.find(it->second) != IDMap.end());
		IDMap.erase(it->second);
		PathMap.erase(WipeOffArray[i]);
	}

	m_ulMaxID = 0;
	m_PathIDMap.clear();
	m_IDPathMap.clear();

	for (BaseDataPathIDMap::iterator iPath = PathMap.begin(); iPath != PathMap.end(); ++iPath)
		m_PathIDMap[iPath->first] = iPath->second;

	for (BaseDataIDPathMap::iterator iID = IDMap.begin(); iID != IDMap.end(); ++iID)
	{
		if (iID->first > m_ulMaxID) m_ulMaxID = iID->first;
		m_IDPathMap[iID->first] = iID->second;
	}

	Save();
	CheckIn(m_strFile);
}

void BaseDataIDManBase::WipeOffByID(abase::vector<unsigned long>& WipeOffArray)
{
	if (!CheckOut(m_strFile)) return;

	BaseDataPathIDMap PathMap;
	BaseDataIDPathMap IDMap;
	unsigned long ulMaxID;

	if (Load(m_strFile, PathMap, IDMap, ulMaxID) != 0) return;

	for (size_t i = 0; i < WipeOffArray.size(); i++)
	{
		BaseDataIDPathMap::iterator it = IDMap.find(WipeOffArray[i]);
		if (it == IDMap.end()) continue;

		ASSERT(PathMap.find(it->second) != PathMap.end());
		PathMap.erase(it->second);
		IDMap.erase(WipeOffArray[i]);
	}

	m_ulMaxID = 0;
	m_PathIDMap.clear();
	m_IDPathMap.clear();

	for (BaseDataPathIDMap::iterator iPath = PathMap.begin(); iPath != PathMap.end(); ++iPath)
		m_PathIDMap[iPath->first] = iPath->second;

	for (BaseDataIDPathMap::iterator iID = IDMap.begin(); iID != IDMap.end(); ++iID)
	{
		if (iID->first > m_ulMaxID) m_ulMaxID = iID->first;
		m_IDPathMap[iID->first] = iID->second;
	}

	Save();
	CheckIn(m_strFile);
}

bool BaseDataIDManBase::CanEdit(const char* szPath)
{
	if (strlen(szPath) == 0) return false;
	if(g_bLinkVss)
		return g_VSS.IsCheckOutStatus(szPath);
	else
	{
		DWORD dwAttr = GetFileAttributes(szPath);
		return (dwAttr != -1 && !(dwAttr & FILE_ATTRIBUTE_READONLY));
	}
}

bool BaseDataIDManBase::CheckOut(const char* szPath)
{
	CString str(szPath);
	int n = str.ReverseFind('\\');
	if (n == -1) return false;
	g_VSS.SetProjectPath(str.Left(n));
	
	g_VSS.CheckOutFile(szPath);
	if (CanEdit(szPath)) return true;

	int nTryTimes = 3;
	while (nTryTimes--)
	{
		Sleep(1000);
		if (CanEdit(szPath)) return true;
	}
	return false;
}

bool BaseDataIDManBase::CheckIn(const char* szPath)
{
	CString str(szPath);
	int n = str.ReverseFind('\\');
	if (n == -1) return false;
	g_VSS.SetProjectPath(str.Left(n));

	g_VSS.CheckInFile(str.Mid(n+1));
	if (!CanEdit(szPath)) return true;

	int nTryTimes = 3;
	while (nTryTimes--)
	{
		Sleep(1000);
		if (!CanEdit(szPath)) return true;
	}
	return false;
}

bool BaseDataIDManBase::Save()
{
	AFile file;
	if (!file.Open(m_strFile, AFILE_CREATENEW | AFILE_TEXT))
		return false;

	char	szLine[AFILE_LINEMAXLEN];

	sprintf(szLine, _format_id_type, GetType());
	file.WriteLine(szLine);

	for (unsigned long i = 1; i <= m_ulMaxID; i = IncID(i))
	{
		BaseDataIDPathMap::iterator it = m_IDPathMap.find(i);
		if (it != m_IDPathMap.end())
		{
			sprintf(szLine, _format_id, i, it->second);
			file.WriteLine(szLine);
		}
	}

	file.Close();
	return true;
}

void BaseDataIDManBase::GeneratePathArray(abase::vector<AString>& PathArray)
{
	for (BaseDataIDPathMap::iterator it = m_IDPathMap.begin(); it != m_IDPathMap.end(); ++it)
		PathArray.push_back(it->second);
}

//////////////////////////////////////////////////////

const unsigned long _high_mask	= 0xffff8000;
const unsigned long _low_mask	= 0x00000fff;
const unsigned long _high_unit	= (~_high_mask + 1);

unsigned long ExtDataIDMan::IncID(unsigned long ulID)
{
	unsigned long ulHighPart = ulID & _high_mask;
	unsigned long ulLowPart = ulID & _low_mask;

	ulLowPart++;
	if (ulLowPart > _low_mask)
		ulHighPart += _high_unit;

	return (ulHighPart & _high_mask) + (ulLowPart & _low_mask);
}

bool ExtDataIDMan::IsRightType(const char* szType) const
{
	return strcmp(szType, _ext_type) == 0;
}

const char* ExtDataIDMan::GetType() const
{
	return _ext_type;
}