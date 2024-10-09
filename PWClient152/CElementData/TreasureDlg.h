#if !defined(AFX_TREASUREDLG_H__1DE55657_D1D2_4628_9C46_D66D413C9A52__INCLUDED_)
#define AFX_TREASUREDLG_H__1DE55657_D1D2_4628_9C46_D66D413C9A52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreasureDlg.h : header file
#include "Treasure.h"
#include "EditTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTreasureDlg dialog

class CTreasureDlg : public CDialog
{
// Construction
public:
	CTreasureDlg(CWnd* pParent = NULL);   // standard constructor
	
	void Init( CString strData ){ m_strData = strData; }
// Dialog Data
	//{{AFX_DATA(CTreasureDlg)
	enum { IDD = IDD_DIALOG_TREASURE };
	CStatic	m_staticIcon;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreasureDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTreasureMan m_TreasureMan;
	HICON m_IconReadOnly;
	HICON m_IconWriteable;
	void FreshList();
	bool  m_bReadOnly;
	bool  m_bChanged;
	CEditTreeCtrl m_treeTreasue;
	CImageList m_ImageList;
	bool ObjIsEqual(TREASURE_ITEM *pObj1, TREASURE_ITEM *pObj2);
	CString	   m_strData;
	// Generated message map functions
	//{{AFX_MSG(CTreasureDlg)
	afx_msg void OnAddItem();
	afx_msg void OnDelItem();
	afx_msg void OnDblclkListAllTreasure();
	afx_msg void OnExport();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckOut();
	afx_msg void OnCheckIn();
	afx_msg void OnFreshTreasure();
	afx_msg void OnTreasureMoveUp();
	afx_msg void OnTreasureMoveDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREASUREDLG_H__1DE55657_D1D2_4628_9C46_D66D413C9A52__INCLUDED_)
