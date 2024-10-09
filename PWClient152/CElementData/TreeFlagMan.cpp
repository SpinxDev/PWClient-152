// TreeFlagMan.cpp: implementation of the CTreeFlagMan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ElementData.h"
#include "AF.h"
#include "A3D.h"

#include "TreeFlagMan.h"

#define new A_DEBUG_NEW

void CRecordData::EnumTree(HTREEITEM root,CTreeCtrl *pTreeCtrl)
{
	
	if(!root) return;
	if(pTreeCtrl->GetItemState(root,TVIF_STATE) & TVIS_EXPANDED )
	{
		CString txt = pTreeCtrl->GetItemText(root);
		AString *pNewStr = new AString(txt);
		m_ListData.AddTail(pNewStr);
		
		HTREEITEM child = pTreeCtrl->GetChildItem(root);
		EnumTree(child,pTreeCtrl);
		while(child)
		{
			child = pTreeCtrl->GetNextItem(child,TVGN_NEXT);
			EnumTree(child,pTreeCtrl);
		}
	}
}

void CRecordData::GetTreeList(CTreeCtrl *pTreeCtrl)
{
	ASSERT(pTreeCtrl);
	Release();
	HTREEITEM root = pTreeCtrl->GetRootItem();
	EnumTree(root,pTreeCtrl);
	while(root)
	{
		root = pTreeCtrl->GetNextItem(root,TVGN_NEXT);
		EnumTree(root,pTreeCtrl);
	}
}

void CRecordData::Save()
{
	DWORD result;
	AFile file;
	int num = m_ListData.GetCount();
	if(file.Open("ElementData.sdt",AFILE_CREATENEW))
	{
		file.Write(&num,sizeof(int),&result);
		POSITION pos = m_ListData.GetHeadPosition();
		while(pos)
		{
			AString *pstr = (AString*)m_ListData.GetNext(pos);
			file.WriteString(*pstr);
		}
		file.Close();
	}
	
}

void CRecordData::Read()
{
	DWORD result;
	AFile file;
	int num;
	if(file.Open("ElementData.sdt",AFILE_OPENEXIST))
	{
		file.Read(&num,sizeof(int),&result);
		for(int i = 0; i< num; i++)
		{
			AString *pNewStr = new AString;
			file.ReadString(*pNewStr);
			m_ListData.AddTail(pNewStr);
		}
		file.Close();
	}
}

bool CRecordData::IsOpen(CString path)
{
	
	POSITION pos = m_ListData.GetHeadPosition();
	CString ExtPath = path; 
	ExtPath.Replace("BaseData","");
	ExtPath.Delete(0,1);
	bool bOpen = true;
	int n = 0;
	while(n!=-1)
	{
		n = ExtPath.FindOneOf("\\");
		CString temp;
		if(n!=-1)
			temp = ExtPath.Left(n);
		else temp = ExtPath;
		ExtPath.Delete(0,n+1);
		bool bSkip = false;
		while(pos)
		{
			AString * pPt = (AString *)m_ListData.GetNext(pos);
			if(stricmp(*pPt,temp)==0)
			{
				bSkip = true;
				break;
			}
		}
		if(!bSkip) 
		{
			bOpen = false;
			break;
		}

	}
	
	return bOpen;
}

void CRecordData::Release()
{
	POSITION pos = m_ListData.GetHeadPosition();
	while(pos)
	{
		AString * pPt = (AString *)m_ListData.GetNext(pos);
		delete pPt;
	}
	m_ListData.RemoveAll();
}
	
CRecordData::CRecordData()
{

}
CRecordData::~CRecordData()
{
	Release();
}


