#if !defined(AFX_FILTERDLG_H__84A4C63A_DF06_4C6B_AA99_5C3FD214B3E0__INCLUDED_)
#define AFX_FILTERDLG_H__84A4C63A_DF06_4C6B_AA99_5C3FD214B3E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterDlg.h : header file

#include "Resource.h"
//
#include <VECTOR>

typedef bool (WINAPI* pfnFilter)(const CString path, const CString name, bool bFolder);

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog
class CPathTreeNode
{
public:
	CPathTreeNode(){};
	~CPathTreeNode()
	{
		for( int i = 0; i < children.size(); ++i)
			delete children[i];
		children.clear();
	};

	void AddChild(CPathTreeNode* pChild)
	{
		if(pChild) children.push_back(pChild);
	}

	std::vector<CPathTreeNode* > children;
	CString text;
	BOOL    bCheck;
};

class CPathTree
{
public:
	CPathTree(){ mRoot = 0; }
	~CPathTree(){ if(mRoot) delete mRoot; }
	CPathTreeNode* InsertItem(CPathTreeNode *pParent, CString txt, BOOL bCheck)
	{
		if(pParent==0)
		{
			if(mRoot==0)
			{
				mRoot = new CPathTreeNode;
				mRoot->bCheck = bCheck;
				mRoot->text = txt;
				return mRoot;
			}else return 0;
		}else
		{
			CPathTreeNode *pNew = new CPathTreeNode;
			ASSERT(pNew);
			pNew->bCheck = bCheck;
			pNew->text = txt;
			pParent->AddChild(pNew);
			return pNew;
		}
	}

	CPathTreeNode* GetRoot(){ return mRoot; }
private:
	CPathTreeNode* mRoot;
};


class CFilterDlg : public CDialog
{
	enum
	{
		ICON_MYCOMPUTER,
		ICON_FOLDER,
		ICON_OPENFOLDER,
		ICON_FILE,
	};

	enum
	{
		BFD_A = 0,
		BFD_B,
		BFD_HARDDISKBEGIN, 
		BFD_C = BFD_HARDDISKBEGIN,
		BFD_D,
		BFD_E,
		BFD_F,
		BFD_G,
		BFD_H,
		BFD_I,
		BFD_J,
		BFD_K,
		BFD_L,
		BFD_M,
		BFD_N,
		BFD_O,
		BFD_P,
		BFD_Q,
		BFD_R,
		BFD_S,
		BFD_T,
		BFD_U,
		BFD_V,
		BFD_W,
		BFD_X,
		BFD_Y,
		BFD_Z,
		BFD_MAX,
		BFD_FOLDER = BFD_MAX,
		BFD_FOLDER_OPENED,
		BFD_MYCOMPUTER,
		BFD_FILE,
	};
	
	// Construction
public:
	CFilterDlg(CWnd* pParent = NULL);   // standard constructor
	
	//strInitPath: 设置初始化路径时后面一定要加"\\"
	//pfn:         如果你需要不让path出现在路径过滤器中，请指定该数pfn,该函数传进一个相对路径
	//             返回为TRUE时，将不出现在过滤器中
	void Init(CString strInitPath, pfnFilter pfn = NULL){ m_strInitPath = strInitPath; m_pfn = pfn; }

	//检测路径是否被过滤器过滤掉了，如果被过滤掉返回TRUE
	//strPath:     这儿也是相对路径，如果是文件夹后面不要带"\\"
	bool IsFiltered(CString strPath);

// Dialog Data
	//{{AFX_DATA(CFilterDlg)
	enum { IDD = IDD_DIALOG_FILTER };
	CTreeCtrl	m_PathTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void		ReadPathTree(CString path, HTREEITEM hParent, int iCount, bool bInit = false, bool bOnlyFirst = false);
	CString		GetItemPath(HTREEITEM hItem);
	void        SetChildItemCheck( HTREEITEM hRoot, bool bCheck);
	void        SetParentItemCheck( HTREEITEM hChild, bool bCheck);
	void        BuildTreeEx();
	void        EnumTree(HTREEITEM hRoot,CPathTreeNode *pParent);
	HICON       GetShellIcon(int nindex); 
	HTREEITEM   GetInsertPos(HTREEITEM hParent,CString name, bool bFolder = true);

	CImageList		m_ImageList;
	CString         m_strInitPath;
	CPathTree       m_PathTreeEx;
	pfnFilter       m_pfn;
	// Generated message map functions
	//{{AFX_MSG(CFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemexpandedTreeFilter(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeFilter(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectAll();
	afx_msg void OnSelectNone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDLG_H__84A4C63A_DF06_4C6B_AA99_5C3FD214B3E0__INCLUDED_)
