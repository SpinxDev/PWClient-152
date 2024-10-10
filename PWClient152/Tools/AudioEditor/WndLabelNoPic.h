#pragma once
#include "wndbase.h"

class CWndLabelNoPic : public CWndBase
{
public:
	CWndLabelNoPic(void);
	~CWndLabelNoPic(void);
public:
	void SetLabel(const CString& csLabel) { m_csLabel = csLabel; }

protected:
	virtual void DrawRealize(CDC* pDC);
protected:
	CString m_csLabel;
};
