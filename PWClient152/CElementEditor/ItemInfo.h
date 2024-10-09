// ItemInfo.h: interface for the CItemInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMINFO_H__FABFAC7A_C382_4832_BB79_9380E7EE6E4E__INCLUDED_)
#define AFX_ITEMINFO_H__FABFAC7A_C382_4832_BB79_9380E7EE6E4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AFXTEMPL.H>
#include <af.h>

class CPckTreeNode
{
public:
	CList<CPckTreeNode*, CPckTreeNode*&> m_lstChilds;
	CPckTreeNode*	m_pParent;
	CString		m_strName;
	
	DWORD		m_dwNodeData;
	BOOL		m_bIsDirectory;
	bool		m_bIsCompressed;

	CPckTreeNode();
	~CPckTreeNode();

	CPckTreeNode* AddChild(LPCSTR pszName, BOOL bIsDirectory);
	CPckTreeNode* UniqueAddChild(LPCSTR pszName, BOOL bIsDirectory);
	CPckTreeNode* AddFullName(LPCSTR pszFullName);
	CPckTreeNode* FindChild(LPCSTR pszName);

	CString GetPathName();
	VOID FreeAllChilds();
};
#endif // !defined(AFX_ITEMINFO_H__FABFAC7A_C382_4832_BB79_9380E7EE6E4E__INCLUDED_)
