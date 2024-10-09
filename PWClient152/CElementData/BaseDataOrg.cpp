#include "BaseDataOrg.h"
#include "AFile.h"

static const char* _format_version	= "Version: %u";
static const char* _format_num		= "ItemNum: %d";
static const char* _format_name		= "ItemName: %s";
static const char* _format_type		= "ItemType: %s";

BaseDataOrg::BaseDataOrg()
{
}

BaseDataOrg::~BaseDataOrg()
{
}

int BaseDataOrg::GetItemNum() const
{
	return static_cast<int>(m_ItemArray.size());
}

const AString& BaseDataOrg::GetItemName(int nIndex) const
{
	return m_ItemArray[nIndex]->m_strItemName;
}

const AString& BaseDataOrg::GetItemType(int nIndex) const
{
	return m_ItemArray[nIndex]->m_strItemType;
}

int BaseDataOrg::Load(const char* szPath)
{
	AFile file;

	if (!file.Open(szPath, AFILE_OPENEXIST | AFILE_TEXT))
		return -1;

	char	szLine[AFILE_LINEMAXLEN];
	char	szBuf[AFILE_LINEMAXLEN];
	DWORD	dwRead;
	DWORD	dwVersion = 0;
	int		nItemNum = 0;

	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_version, &dwVersion);

	file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_num, &nItemNum);

	for (int i = 0; i < nItemNum; i++)
	{
		AString strName, strType;

		szBuf[0] = '\0';
		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_name, szBuf);
		strName = szBuf;

		szBuf[0] = '\0';
		file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_type, szBuf);
		strType = szBuf;
	
		AString strKey = strName + strType;
		if (m_ItemMap.find(strKey) != m_ItemMap.end())
		{
			file.Close();
			Release();
			return -2;
		}

		DATA_ITEM* pItem = new DATA_ITEM;
		pItem->m_strItemName = strName;
		pItem->m_strItemType = strType;
		m_ItemArray.push_back(pItem);
		m_ItemMap[strKey] = pItem;
	}

	file.Close();
	return 0;
}

void BaseDataOrg::Release()
{
	for (size_t i = 0; i < m_ItemArray.size(); i++)
		delete m_ItemArray[i];
	m_ItemArray.clear();
	m_ItemMap.clear();
}