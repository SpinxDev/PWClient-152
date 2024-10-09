// ItemInfo.cpp: implementation of the CItemInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemInfo.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPckTreeNode::CPckTreeNode()
{
	m_pParent		= NULL;
	m_dwNodeData	= 0;
	m_bIsDirectory	= FALSE;
	m_bIsCompressed = false;
}

CPckTreeNode::~CPckTreeNode()
{
	FreeAllChilds();
}

VOID CPckTreeNode::FreeAllChilds()
{
	POSITION Pos = m_lstChilds.GetHeadPosition();
	while(Pos != NULL)
	{
		CPckTreeNode* pNode	= m_lstChilds.GetNext(Pos);
		if(pNode != NULL)
		{
			delete pNode;
		}
	}
	m_lstChilds.RemoveAll();
}

CString CPckTreeNode::GetPathName()
{
	CPckTreeNode* pParent	= m_pParent;
	CString strRet	= m_strName;
	if(pParent == NULL)
	{
		return strRet;
	}
	while(pParent->m_pParent != NULL)
	{
		strRet	= pParent->m_strName + CString("\\") + strRet;
		pParent	= pParent->m_pParent;
	}
	return strRet;
}

CPckTreeNode* CPckTreeNode::FindChild(LPCSTR pszName)
{
	POSITION Pos = m_lstChilds.GetHeadPosition();
	while(Pos != NULL)
	{
		CPckTreeNode* pNode	= m_lstChilds.GetNext(Pos);
		if(pNode->m_strName == pszName)
		{
			return pNode;
		}
	}
	return NULL;
}

CPckTreeNode* CPckTreeNode::AddChild(LPCSTR pszName, BOOL bIsDirectory)
{
	CPckTreeNode* pNode	= new CPckTreeNode;
	pNode->m_bIsDirectory	= bIsDirectory;
	pNode->m_strName		= pszName;
	pNode->m_pParent		= this;
	m_lstChilds.AddTail(pNode);
	return pNode;
}

CPckTreeNode* CPckTreeNode::UniqueAddChild(LPCSTR pszName, BOOL bIsDirectory)
{
	CPckTreeNode* pNode = FindChild(pszName);
	if(pNode != NULL)
	{
		if(pNode->m_bIsDirectory != bIsDirectory)
		{
			if(pNode->m_bIsDirectory)
			{
				pNode->FreeAllChilds();
			}
			pNode->m_bIsDirectory = bIsDirectory;
		}
		return pNode;
	}
	pNode =  AddChild(pszName, bIsDirectory);
	
	return pNode;
}

CPckTreeNode* CPckTreeNode::AddFullName(LPCSTR pszFullName)
{
	if(pszFullName == NULL)
	{
		return NULL;
	}
	int nTemp = 0, nFull = 0;
	char szTempName[MAX_PATH];
	CPckTreeNode* pNode = this;

	while(pszFullName[nFull] != '\0')
	{
		if(pszFullName[nFull] == '\\' || pszFullName[nFull] == '/')
		{
			if(nTemp > 0)
			{
				szTempName[nTemp] = '\0';
				pNode	= pNode->UniqueAddChild(szTempName, TRUE);
				if(pNode == NULL)
				{
					return NULL;
				}
			}
			nTemp = 0;
		}
		else
		{
			szTempName[nTemp] = pszFullName[nFull];
			nTemp ++;
		}
		nFull ++;
	}

	if(nTemp > 0)
	{
		szTempName[nTemp] = '\0';
		pNode->UniqueAddChild(szTempName, FALSE);
	}

	return pNode;
}
