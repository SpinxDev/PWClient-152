#if !defined(AFX_TREASUREITEMDLG_H__BD4D6BBB_FCDB_4C31_8D63_A0D0F1215DFF__INCLUDED_)
#define AFX_TREASUREITEMDLG_H__BD4D6BBB_FCDB_4C31_8D63_A0D0F1215DFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreasureItemDlg.h : header file

#include "Treasure.h"
#include "AObject.h"
//
class CPropertyList;
class ADynPropertyObject;
class CTemplIDSel;



struct TIME_SET_FUNCS : public CUSTOM_FUNCS
{
	~TIME_SET_FUNCS() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

/////////////////////////////////////////////////////////////////////////////
// CTreasureItemDlg dialog

class CTreasureItemDlg : public CDialog
{
// Construction
public:
	CTreasureItemDlg(CWnd* pParent = NULL);   // standard constructor
	TREASURE_ITEM m_Item;
	void SetReadOnly(bool bReadOnly = true){ m_bReadOnly = bReadOnly; }
// Dialog Data
	//{{AFX_DATA(CTreasureItemDlg)
	enum { IDD = IDD_DIALOG_TREASURE_ITEM };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreasureItemDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	ADynPropertyObject *m_pProperty;
	CPropertyList *m_pList;
	CTemplIDSel* m_pListIDSel;
	TIME_SET_FUNCS* m_pTimeSet1;
	TIME_SET_FUNCS* m_pTimeSet2;
	TIME_SET_FUNCS* m_pTimeSet3;
	TIME_SET_FUNCS* m_pTimeSet4;
	ASetTemplate<int> sort_type;
	bool m_bReadOnly;
	void BuildProperty();
	void UpdateProperty(bool bGet);
	// Generated message map functions
	//{{AFX_MSG(CTreasureItemDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREASUREITEMDLG_H__BD4D6BBB_FCDB_4C31_8D63_A0D0F1215DFF__INCLUDED_)
