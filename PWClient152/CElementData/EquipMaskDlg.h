#if !defined(AFX_EQUIPMASKDLG_H__90782262_C755_4978_84FD_E487E56F52C6__INCLUDED_)
#define AFX_EQUIPMASKDLG_H__90782262_C755_4978_84FD_E487E56F52C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EquipMaskDlg.h : header file
//

#include "AObject.h"

struct EquipMask : public CUSTOM_FUNCS
{
	~EquipMask() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	AVariant m_var;
	LPCTSTR* m_pMaskNames;
	mutable AString  m_strShow;
	int m_nNameCount;
};

/////////////////////////////////////////////////////////////////////////////
// CEquipMaskDlg dialog

class CEquipMaskDlg : public CDialog
{
// Construction
public:
	CEquipMaskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEquipMaskDlg)
	enum { IDD = IDD_EQUIP_MASK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	unsigned long m_ulEquipMask;
	LPCTSTR* m_pMaskNames;
	int m_nNameCount;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEquipMaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEquipMaskDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQUIPMASKDLG_H__90782262_C755_4978_84FD_E487E56F52C6__INCLUDED_)
