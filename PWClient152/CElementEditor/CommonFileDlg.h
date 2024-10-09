#if !defined(AFX_COMMONFILEDLG_H__A89FDAF9_1CF3_4564_B43B_D31BFFC9E205__INCLUDED_)
#define AFX_COMMONFILEDLG_H__A89FDAF9_1CF3_4564_B43B_D31BFFC9E205__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommonFileDlg.h : header file
//

#include <A3D.h>
#include <af.h>
#include <am.h>

#include <d3dx8.h>
#include <d3d8.h>

#include "ItemInfo.h"

#define AFILE_ALLOWMULFILES		1
/////////////////////////////////////////////////////////////////////////////
// CCommonFileDlg dialog

class CCommonFileDlg : public CDialog
{
// Construction
public:
	CCommonFileDlg(CWnd* pParent = NULL);   // standard constructor
	CCommonFileDlg(A3DEngine* pA3DEngine,CString sRelPath = "", CString sExt = "*.*", CWnd* pParent = NULL);

private:
	
	CString m_curDir;				// current directory path
	CString m_curRoot;				// current root path
	CString m_relRoot;				// relative root path
	CString m_ext;					// extension of file
	CString m_workRoot;				// work root path
	CString m_relworkRoot;

	CPckTreeNode  m_tnRoot;			// package tree
	CPckTreeNode* m_pCurNode;		// current package node
	CPckTreeNode* m_pRootNode;		// package root node

	CBitmap bmp[2];
	CBitmap m_bmpPrev;				// bitmap for preview

	CImageList m_imageList;			// icon of lookin and file list
	CImageList m_dirImageList;		// icon of dir list

	CStringArray m_arrDir;				// folder array for disk files

	CStringArray m_arrFilelist;		// for multiple selected files
	CStringArray m_backDir;			// restore past directory
	CStringArray m_arrCurDir;		// save the decomposed current directory

	bool bChanged;					
	int OpenMode;					// from harddisk or from package
	DWORD flags;					// multiple selection or single selection 

	A3DEngine*		m_pA3DEngine;
	AFilePackage*	m_pPackage;
	AMSoundBufferMan* m_pSoundBufMan;
	AMSoundBuffer*	m_pSoundBuf;

	// Dialog Data
	//{{AFX_DATA(CCommonFileDlg)
	enum { IDD = IDD_COMMONFILE_DIALOG };
	CStatic	m_sPrev;
	CButton	m_upBut;
	CStatic	m_strImageInfo;
	CStatic	m_strFileInfo;
	CListCtrl	m_lstFile;
	CListCtrl	m_lstDir;
	CComboBoxEx	m_cboLookIn;
	CComboBox	m_cboFileType;
	CButton	m_preview;
	CButton	m_prevBut;
	CString	m_fileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	
	bool InitDir();
	void InitExt();

	bool OpenFromDisk(CString sPath);
	bool UpdateDiskDirCtrl(CString sPath);
	bool UpdateDiskListCtrl(CString sPath);

	bool OpenFromPackage(CString sPath);
	bool UpdatePckListCtrl(CPckTreeNode* pNode);
	bool UpdatePckDirCtrl(CPckTreeNode* pNode);
	void ClosePckListCtrl();

	void UpdateLookinCombo();
	void UpdateArrCurDir(CString sDir);
	void UpdatePrevRegion();
	
	bool LoadPrev(CString sFile);
	bool DisplayBmp();

	bool GetMulFiles();

	CString GetCurDir()		{ return m_curDir; }
	CString GetRootDir()	{ return m_curRoot;	}
	void GetRelCurRoot()
	{
		DWORD nCount = m_curDir.GetLength()-m_curRoot.GetLength()-1;
		m_relRoot = m_curDir.Right(nCount);
	}
	void GetRelWorkRoot()
	{
		DWORD nCount = m_curDir.GetLength()-m_workRoot.GetLength()-1;
		m_relworkRoot = m_curDir.Right(nCount);
	}
	
// Implementation
public:

	CString GetFullFileName()
	{
		GetRelCurRoot();
		
		if(m_relRoot == "")
			return m_fileName;
		else
			return (m_relRoot+"\\"+m_fileName);
	}

	CString GetRelativeFileName()
	{
		GetRelWorkRoot();

		if(m_relworkRoot == "")
			return m_fileName;
		else
			return (m_relworkRoot+"\\"+m_fileName);
	}

	CString GetFirstSelectedFullName()
	{
		GetRelCurRoot();
		
		if(m_arrFilelist.GetSize() == 0)
			return "";
		else
		{
			CString pItem = m_arrFilelist.GetAt(0);

			if(m_relRoot == "")
				return pItem;
			else
				return (m_relRoot+"\\"+pItem);
		}		
	}

	CString GetFirstSelectedRelName()
	{
		GetRelWorkRoot();

		if(m_arrFilelist.GetSize() == 0)
			return "";
		else
		{
			CString pItem = m_arrFilelist.GetAt(0);

			if(m_relworkRoot == "")
				return pItem;
			else
				return (m_relworkRoot+"\\"+pItem);
		}
	}

	int GetSelectedNum()
	{
		return m_arrFilelist.GetSize();
	}

	CString GetNextSelectedFullName(int nIndex)
	{
		GetRelCurRoot();

		if(GetSelectedNum() != 0 && nIndex <= GetSelectedNum())
		{
			if(m_relRoot == "")
				return m_arrFilelist.GetAt(nIndex-1);
			else
				return (m_relRoot+"\\"+m_arrFilelist.GetAt(nIndex-1));
		}
		else
			return "";
	}
	
	CString GetNextSelectedRelName(int nIndex)
	{
		GetRelWorkRoot();

		if(GetSelectedNum() != 0 && nIndex <= GetSelectedNum())
		{
			if(m_relworkRoot == "")
				return m_arrFilelist.GetAt(nIndex-1);
			else
				return (m_relworkRoot+"\\"+m_arrFilelist.GetAt(nIndex-1));
		}
		else
			return "";
	}

	// Generated message map functions
	//{{AFX_MSG(CCommonFileDlg)
	afx_msg void OnChangeFilename();
	afx_msg void OnClickDirlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	afx_msg void OnSelchangeComboFiletype();
	afx_msg void OnSelchangeComboboxexLookin();
	afx_msg void OnbtnUp();
	afx_msg void OnbtnBack();
	afx_msg void OnPaint();
	afx_msg void OnOpen();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMONFILEDLG_H__A89FDAF9_1CF3_4564_B43B_D31BFFC9E205__INCLUDED_)
