// Filename	: DlgWikiItemListBase.cpp
// Creator	: Feng Ning
// Date		: 2010/04/26

#include "DlgWikiItemListBase.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiItemListBase, CDlgWikiBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiItemListBase, CDlgWikiBase)
AUI_END_EVENT_MAP()

CDlgWikiItemListBase::CDlgWikiItemListBase()
:m_ObjIndex(0)
,m_ObjCount(0)
{
}

bool CDlgWikiItemListBase::OnInitDialog()
{
	m_ObjCount = 0;
	m_ObjIndex = 0;
	return CDlgWikiBase::OnInitDialog();
}

void CDlgWikiItemListBase::OnShowDialog()
{
	//
	CDlgWikiBase::OnShowDialog();
}

bool CDlgWikiItemListBase::Release(void)
{
	//
	return CDlgWikiBase::Release();
}

int CDlgWikiItemListBase::GetPageSize() const
{
	// lazy initialization
	if(!m_ObjCount)
	{
		int iObj = 1;
		PAUIOBJECT pObj = NULL;
		AString szName;
		while(true)
		{
			szName.Format("Txt_Link_%02d", iObj);
			pObj = const_cast<CDlgWikiItemListBase*>(this)->GetDlgItem(szName);
			if(!pObj)
			{
				szName.Format("Txt_%02d", iObj);
				pObj = const_cast<CDlgWikiItemListBase*>(this)->GetDlgItem(szName);
			}

			if (!pObj)
			{
				const_cast<CDlgWikiItemListBase*>(this)->m_ObjCount = iObj - 1;
				break;
			}

			++iObj;
		}
	}

	return m_ObjCount;
}

void CDlgWikiItemListBase::OnCommandCancel()
{
	//
	CDlgWikiBase::OnCommandCancel();
}

void CDlgWikiItemListBase::OnBeginSearch()
{
	m_ObjIndex = 1;
}

bool CDlgWikiItemListBase::OnAddSearch(WikiEntityPtr p)
{
	int iObj = m_ObjIndex;
	PAUIOBJECT pObj = NULL;
	AString szName;

	szName.Format("Txt_Link_%02d", iObj);
	pObj = GetDlgItem(szName);
	if(!pObj)
	{
		szName.Format("Txt_%02d", iObj);
		pObj = GetDlgItem(szName);
	}

	if (pObj)
	{
		szName.Format("Txt_Link_Name_%02d", iObj);
		PAUIOBJECT pName = GetDlgItem(szName);
		if(!pName)
		{
			szName.Format("Name_%02d", iObj);
			pName = GetDlgItem(szName);
		}

		if(!OnModityItem(p, pObj, pName))
		{
			return false;
		}

		// move to next index
		++m_ObjIndex;
	}

	// do not return false unless we want to drop this data
	return true;
}

void CDlgWikiItemListBase::OnEndSearch()
{
	CDlgWikiBase::OnEndSearch();

	if(!m_ObjIndex) return;
	int iObj = m_ObjIndex;
	m_ObjIndex = 0;

	PAUIOBJECT pObj = NULL;
	AString szName;
	while(true)
	{
		szName.Format("Txt_Link_%02d", iObj);
		pObj = GetDlgItem(szName);
		if (!pObj)
		{
			szName.Format("Txt_%02d", iObj);
			pObj = GetDlgItem(szName);
		}

		if(!pObj) break;
		pObj->Show(false);
		
		szName.Format("Txt_Link_Name_%02d", iObj);
		pObj = GetDlgItem(szName);
		if(!pObj)
		{
			szName.Format("Name_%02d", iObj);
			pObj = GetDlgItem(szName);
		}

		if(pObj)
		{
			pObj->Show(false);
		}
		
		iObj++;
	}
}
