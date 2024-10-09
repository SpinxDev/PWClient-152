#if !defined(AFX_DLGSETTIME_H__1C00CEEE_D7D0_4355_8DC6_7A18A78E9C60__INCLUDED_)
#define AFX_DLGSETTIME_H__1C00CEEE_D7D0_4355_8DC6_7A18A78E9C60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AObject.h"

//	设置时间长度
struct SetTimeLengthFunc : public CUSTOM_FUNCS
{
	~SetTimeLengthFunc() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	AVariant m_var;
	mutable AString  m_strShow;
};

//	设置过期时间
struct SetExpireTimeFunc : public CUSTOM_FUNCS
{
	~SetExpireTimeFunc() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	AVariant m_var;
	mutable AString  m_strShow;
};

// DlgSetTime.h : header file
//
#include "DlgExpireTime.h"
#include "DlgTimeLength.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSetTime dialog

class CDlgSetTime : public CDialog
{
public:	
	enum TimeType{
		AS_TIME_LENGTH,
		AS_EXPIRE_TIME,
	};

// Construction
public:
	CDlgSetTime(int value = 0, unsigned int type = AS_TIME_LENGTH, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetTime)
	enum { IDD = IDD_SET_TIME };
	CTabCtrl	m_tab;
	//}}AFX_DATA

	int GetTime();

private:
	unsigned int	m_type;
	int				m_value;
	CDlgTimeLength	m_item1;
	CDlgExpireTime	m_item2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetTime)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnSelchangeTabTime(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETTIME_H__1C00CEEE_D7D0_4355_8DC6_7A18A78E9C60__INCLUDED_)
