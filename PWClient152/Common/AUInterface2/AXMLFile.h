#ifndef _AXMLFILE_H_
#define _AXMLFILE_H_

#include "AAssist.h"
#include "vector.h"

class AXMLItem
{
public:
	struct PROPERTY_DATA
	{
		ACString key;
		ACString value;
	};

	AXMLItem();
	~AXMLItem();

	bool Init(const ACHAR *pszText);
	const ACHAR* GetItemText(bool bEnd = false);
	
	const ACHAR* GetName();
	void SetName(const ACHAR* pszName);

	const ACHAR* GetValue(const ACHAR* pszKey);
	const ACHAR* GetValue(int nKey);
	void SetValue(const ACHAR* pszKey, const ACHAR* pszValue);
	bool SetValue(int nKey, const ACHAR* pszValue);
	int GetKeyNumber();

	AXMLItem* GetParentItem();
	AXMLItem* GetFirstChildItem();
	AXMLItem* GetPrevItem();
	AXMLItem* GetNextItem();
	AXMLItem* GetChildItemByName(const ACHAR *pszName);

	void InsertItem(AXMLItem* pParent, AXMLItem* pAfter = NULL);
	void RemoveItem();

	const abase::vector<PROPERTY_DATA>* GetProperties() const {return &m_vecProperty;}
	void ClearProperties() {m_vecProperty.clear();}

private:
	AXMLItem* m_pParentItem;
	AXMLItem* m_pFirstChildItem;
	AXMLItem* m_pPrevItem;
	AXMLItem* m_pNextItem;
	abase::vector<PROPERTY_DATA> m_vecProperty;
	ACString m_strName;
	ACString m_strItemText;
};

class AXMLFile
{
public:
	AXMLFile();
	~AXMLFile();

	bool LoadFromFile(const char * pszFile);
	bool LoadFromText(const ACHAR * pszText);
	bool SaveToFile(const char * pszFile);
	const ACHAR* GetText();

	AXMLItem* GetRootItem();

	void Release();

private:
	ACString BuildItemText(ACString strTab, AXMLItem *pItem);
	void ReleaseItem(AXMLItem* pItem);
	ACString GetNextXMLItemText();

	AXMLItem m_RootItem;
	ACString m_strText;
	int m_nPos;
};

#endif //	_AXMLFILE_H_