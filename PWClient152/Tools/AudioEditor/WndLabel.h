#pragma once
#include "wndbase.h"

class CWndLabel : public CWndBase
{
public:
	CWndLabel(void);
	~CWndLabel(void);
public:
	void SetLabel(const CString& csLabel, UINT uiIconID);
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void DrawRealize(CDC* pDC);
	virtual void EndDraw();
protected:
	CString m_csLabel;
	CStatic m_wndIcon;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
