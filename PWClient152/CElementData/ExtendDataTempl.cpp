#include "ExtendDataTempl.h"
#include "AFile.h"
#include "Global.h"

bool ExtendDataTempl::CreateTempl(const char* szPath, const char* szExtName, BaseDataIDManBase* pIDMan, bool bNew)
{
	if (bNew)
	{
		m_strName = szExtName;
		unsigned long ulID = pIDMan->CreateID(szPath);
		if (ulID == 0)
		{
			Release();
			return false;
		}
		m_ulID = ulID;
	}

	return true;
}

bool ExtendDataTempl::AddItem(const AString& strName, const AString& strType)
{
	if (m_ItemMap.find(strName + strType) != m_ItemMap.end()) return false;

	ITEM_DATA* pItem = new ITEM_DATA;
	pItem->m_strName = strName;
	pItem->m_strType = strType;
	m_ItemArray.push_back(pItem);
	m_ItemMap[strName + strType] = pItem;
	return true;
}

void ExtendDataTempl::RemoveAt(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_ItemArray.size())
		return;

	ITEM_DATA* pItem = m_ItemArray[nIndex];
	m_ItemArray.erase(&m_ItemArray[nIndex]);
	m_ItemMap.erase(pItem->m_strName + pItem->m_strType);
	delete pItem;
}