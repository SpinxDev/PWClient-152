// TreeFlagMan.h: interface for the CTreeFlagMan class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TREE_FLAG_MAN_H_
#define _TREE_FLAG_MAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPtrList;
class AFile;

class CRecordData
{

public:
	CRecordData();
	~CRecordData();

	void GetTreeList(CTreeCtrl *pTreeCtrl);
	void Save();
	void Read();
	bool IsOpen(CString path);
	void Release();

private:
	void EnumTree(HTREEITEM root,CTreeCtrl *pTreeCtrl);
	CPtrList m_ListData;	
};

#endif // _TREE_FLAG_MAN_H_
