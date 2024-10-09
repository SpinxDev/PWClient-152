#if !defined(AFX_CUSTOMCOLORDIALOG_H__DC613BBC_BD88_41D9_95F8_233EA842EAC9__INCLUDED_)
#define AFX_CUSTOMCOLORDIALOG_H__DC613BBC_BD88_41D9_95F8_233EA842EAC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomColorDialog.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CCustomColorDialog dialog

class CCustomColorDialog : public CColorDialog
{
	DECLARE_DYNAMIC(CCustomColorDialog)

public:

	DWORD GetColor() const;
	CCustomColorDialog(DWORD clrInit = 0, DWORD dwFlags = 0,
			CWnd* pParentWnd = NULL);

protected:

	UINT m_byte_alpha;
	//{{AFX_MSG(CCustomColorDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAlpha();
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMCOLORDIALOG_H__DC613BBC_BD88_41D9_95F8_233EA842EAC9__INCLUDED_)
