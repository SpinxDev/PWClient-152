#include "AUI.h"

#include "AXMLFile.h"
#include "ACHAR.h"
#include "AFileImage.h"
#include "AMemory.h"

AXMLItem::AXMLItem()
{
	m_pParentItem = NULL;
	m_pFirstChildItem = NULL;
	m_pPrevItem = NULL;
	m_pNextItem = NULL;
	m_strName = _AL("");
	m_vecProperty.clear();
}

AXMLItem::~AXMLItem()
{
}

bool AXMLItem::Init(const ACHAR *pszText)
{
	ACString strText = pszText;
	strText.TrimLeft();
	strText.TrimRight();
	int nLength = strText.GetLength();
	if( strText[0] != '<' && strText[nLength - 1] != '>' )
		return false;

	int i, j;
	nLength--;
	if( strText[nLength - 1] == '/' )
		nLength--;
	for(i = 0; i < nLength; i++)
		if( strText[i] == '\t' || strText[i] == '\r' || strText[i] == '\n' )
			strText[i] = ' ';

	// get item name
	i = 1;
	while( i < nLength && strText[i] == ' ' )
		i++;
	j = i;
	while( i < nLength && strText[i] != ' ' )
		i++;
	m_strName = strText.Mid(j, i - j);

	// get key and value
	m_vecProperty.clear();
	while( i < nLength )
	{
		while( i < nLength && strText[i] == ' ' )
			i++;

		j = i;
		while( i < nLength && strText[i] != ' ' && strText[i] != '=' )
			i++;

		PROPERTY_DATA pdata;
		pdata.key = strText.Mid(j, i - j);
		while( i < nLength && strText[i] != '=' )
			i++;

		if( i == nLength )
			break;

		while( i < nLength && strText[i] != '"' )
			i++;
		i++;
		j = i;
		while( i < nLength )
		{
			if( strText[i] == '\\' )
				i += 2;
			else if( strText[i] != '"')
				i++;
			else
			{
				pdata.value = strText.Mid(j, i - j);
				m_vecProperty.push_back(pdata);
				i++;
				break;
			}
		}
	}
	return true;
}

const ACHAR* AXMLItem::GetItemText(bool bEnd)
{
	m_strItemText = _AL("<");
	m_strItemText += m_strName;
	ACString strTemp;
	for(int i = 0; i < m_vecProperty.size(); i++ )
	{
		strTemp.Format(_AL(" %s=\"%s\""), m_vecProperty[i].key, m_vecProperty[i].value);
		m_strItemText += strTemp;
	}
	if( bEnd )
		m_strItemText += _AL("/>");
	else
		m_strItemText += _AL(">");
	return m_strItemText;
}

const ACHAR* AXMLItem::GetName()
{
	return m_strName;
}

void AXMLItem::SetName(const ACHAR* pszName)
{
	m_strName = pszName;
}

const ACHAR* AXMLItem::GetValue(int nKey)
{
	if( nKey < m_vecProperty.size() )
		return m_vecProperty[nKey].value;
	else
		return NULL;
}

const ACHAR* AXMLItem::GetValue(const ACHAR* pszKey)
{
	for(int i = 0; i < m_vecProperty.size(); i++)
		if( a_stricmp(m_vecProperty[i].key, pszKey) == 0 )
			return m_vecProperty[i].value;
	return NULL;
}

bool AXMLItem::SetValue(int nKey, const ACHAR* pszValue)
{
	if( nKey < m_vecProperty.size() )
	{
		m_vecProperty[nKey].value = pszValue;
		return true;
	}
	return false;
}

void AXMLItem::SetValue(const ACHAR* pszKey, const ACHAR* pszValue)
{
	for(int i = 0; i < m_vecProperty.size(); i++)
		if( a_stricmp(m_vecProperty[i].key, pszKey) == 0 )
		{
			m_vecProperty[i].value = pszValue;
			return;
		}
	PROPERTY_DATA pdata;
	pdata.key = pszKey;
	pdata.value = pszValue;
	m_vecProperty.push_back(pdata);
}

int AXMLItem::GetKeyNumber()
{
	return m_vecProperty.size();
}

AXMLItem* AXMLItem::GetParentItem()
{
	return m_pParentItem;
}

AXMLItem* AXMLItem::GetFirstChildItem()
{
	return m_pFirstChildItem;
}

AXMLItem* AXMLItem::GetPrevItem()
{
	return m_pPrevItem;
}

AXMLItem* AXMLItem::GetNextItem()
{
	return m_pNextItem;
}

AXMLItem* AXMLItem::GetChildItemByName(const ACHAR *pszName)
{
	AXMLItem* pItem = m_pFirstChildItem;
	while( pItem )
	{
		if( a_stricmp(pItem->GetName(), pszName) == 0 )
			return pItem;
		pItem = pItem->GetNextItem();
	}
	return NULL;
}

void AXMLItem::InsertItem(AXMLItem* pParent, AXMLItem* pAfter)
{
	if( pAfter )
	{
		m_pParentItem = pAfter->GetParentItem();
		m_pNextItem = pAfter->GetNextItem();
		m_pPrevItem = pAfter;
		if( m_pNextItem )
			m_pNextItem->m_pPrevItem = this;
		pAfter->m_pNextItem = this;
		return;
	}
	if( pParent )
	{
		m_pParentItem = pParent;
		if( !pParent->GetFirstChildItem()  )
		{
			m_pNextItem = NULL;
			m_pPrevItem = NULL;
			pParent->m_pFirstChildItem = this;
		}
		else
		{
			AXMLItem* pItem = pParent->GetFirstChildItem();
			while( pItem->GetNextItem() )
				pItem = pItem->GetNextItem();
			m_pNextItem = NULL;
			pItem->m_pNextItem = this;
			m_pPrevItem = pItem;
		}
	}
}

void AXMLItem::RemoveItem()
{
	if( m_pPrevItem )
		m_pPrevItem->m_pNextItem = m_pNextItem;
	else
		m_pParentItem->m_pFirstChildItem = m_pNextItem;
	if( m_pNextItem )
		m_pNextItem->m_pPrevItem = m_pPrevItem;
	m_pParentItem = NULL;
}

AXMLFile::AXMLFile()
{
}

AXMLFile::~AXMLFile()
{
	Release();
}

bool AXMLFile::LoadFromFile(const char * pszFile)
{
#if defined(_ANGELICA2) || defined(_ANGELICA22)

	AFileImage aFile;
	if( !aFile.Open(pszFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
		return false;

	char *buf = (char*)a_malloctemp(aFile.GetFileLength() + 2);
	DWORD dwLen;
	bool bRet = true;
	if( aFile.Read(buf, aFile.GetFileLength(), &dwLen) )
	{
		buf[dwLen] = '\0';
		buf[dwLen + 1] = '\0';
		bRet = LoadFromText((ACHAR*)buf);
	}
	a_freetemp(buf);
	
	aFile.Close();
	return bRet;

#else

	AFileImage aFile;
	if( !aFile.Open(pszFile, AFILE_OPENEXIST | AFILE_BINARY) )
		return false;

	char *buf = (char*)a_malloc(aFile.GetFileLength() + 2);
	DWORD dwLen;
	bool bRet = true;
	if( aFile.Read(buf, aFile.GetFileLength(), &dwLen) )
	{
		buf[dwLen] = '\0';
		buf[dwLen + 1] = '\0';
		bRet = LoadFromText((ACHAR*)buf);
	}
	a_free(buf);
	
	aFile.Close();
	return bRet;

#endif
}

bool AXMLFile::LoadFromText(const ACHAR * pszText) 
{
	Release();

	m_strText = pszText;
	m_nPos = 0;
	ACString strText = GetNextXMLItemText();
	ACString strParentLabel = _AL("");
	AXMLItem *pNewItem, *pParentItem, *pPrevItem;
	pParentItem = &m_RootItem;
	pPrevItem = NULL;

	while( strText != _AL(""))
	{
		pNewItem = new AXMLItem;
		pNewItem->Init(strText);
		if( strParentLabel == pNewItem->GetName() )
		{
			pPrevItem = pParentItem;
			pParentItem = pParentItem->GetParentItem();
			strParentLabel = ACString(_AL("/")) + pParentItem->GetName();
			delete pNewItem;
		}
		else
		{
			pNewItem->InsertItem(pParentItem, pPrevItem);
			if( strText[strText.GetLength() - 2] == '/' )
			{
				pPrevItem = pNewItem;
			}
			else
			{
				pParentItem = pNewItem;
				pPrevItem = NULL;
				strParentLabel = ACString(_AL("/")) + pParentItem->GetName();
			}
		}

		strText = GetNextXMLItemText();
	}
	
	return true;
}

bool AXMLFile::SaveToFile(const char * pszFile)
{
	AFile aFile;
	if( !aFile.Open(pszFile, AFILE_CREATENEW | AFILE_BINARY | AFILE_NOHEAD) )
		return false;

	GetText();
	DWORD dwLen;
	if( !aFile.Write(m_strText.GetBuffer(0), m_strText.GetLength() * sizeof(ACHAR), &dwLen) )
		return false;

	aFile.Close();

	return true;
}

const ACHAR* AXMLFile::GetText()
{
	m_strText = (ACHAR)0xfeff;

	AXMLItem *pChildItem = m_RootItem.GetFirstChildItem();
	while(pChildItem)
	{
		m_strText += BuildItemText(_AL(""), pChildItem);
		pChildItem = pChildItem->GetNextItem();
	}

	return m_strText;
}
 
ACString AXMLFile::BuildItemText(ACString strTab, AXMLItem *pItem)
{
	ACString strItem = _AL("");
	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while(pChildItem)
	{
		strItem += BuildItemText(strTab + _AL("\t"), pChildItem);
		pChildItem = pChildItem->GetNextItem();
	}

	if( strItem.GetLength() == 0 && pItem->GetKeyNumber() == 0 )
		return _AL("");

	// modified by daixinyu 2011-07-12. To make empty AUIDialog load ok.
	if( strItem.GetLength() > 0 || pItem->GetParentItem() == &m_RootItem)
	{
		strItem = strTab + pItem->GetItemText() + _AL("\r\n") + strItem;
		ACString strText;
		strText.Format(_AL("</%s>\r\n"), pItem->GetName());
		strText = strTab + strText;
		strItem += strText;
	}
	else
	{
		strItem = strTab + pItem->GetItemText(true) + _AL("\r\n");
	}

	return strItem;
}

AXMLItem* AXMLFile::GetRootItem()
{
	return &m_RootItem;
}

void AXMLFile::Release()
{
	AXMLItem *pChildItem = m_RootItem.GetFirstChildItem();
	while( pChildItem )
	{
		AXMLItem *pNextItem = pChildItem->GetNextItem();
		ReleaseItem(pChildItem);
		pChildItem = pNextItem;
	}
}

void AXMLFile::ReleaseItem(AXMLItem* pItem)
{
	if( !pItem )
		return;
	
	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		AXMLItem *pNextItem = pChildItem->GetNextItem();
		ReleaseItem(pChildItem);
		pChildItem = pNextItem;
	}
	pItem->RemoveItem();
	delete pItem;
}

ACString AXMLFile::GetNextXMLItemText()
{
	int i = m_nPos;
	while( i < m_strText.GetLength() && m_strText[i] != '<' )
		i++;

	if( i == m_strText.GetLength() )
		return _AL("");

	int nStart = i;
	bool b = false;
	while( i < m_strText.GetLength() )
	{
		if( m_strText[i] == '"')
		{
			b = !b;
			i++;
		}
		else if( m_strText[i] == '\\' )
			i += 2;
		else if( !b && m_strText[i] == '>' )
		{
			m_nPos = i + 1;
			return m_strText.Mid(nStart, m_nPos - nStart);
		}
		else
			i++;
	}
	return _AL("");
}
