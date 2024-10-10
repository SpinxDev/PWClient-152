#pragma once

#include "treectrlex.h"
#include "EventDisplay.h"
#include "DynSizeCtrl.h"
// CDlgSelectEvent dialog
using AudioEngine::EventList;

class CDlgSelectEvent : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSelectEvent)

public:
	CDlgSelectEvent(bool bMultiSelect, const char* szTitle = "", CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectEvent();

// Dialog Data
	enum { IDD = IDD_SELECT_EVENT };
public:
	EventList GetSelectEventList() const { return m_listSelectEvent; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
protected:
	CImageList m_ImageList;
	CTreeCtrlEx m_wndTree;
	CEventDisplay m_eventDisplay;
	EventList m_listSelectEvent;
	DynSizeCtrlMan m_dscMan;
	bool m_bMultiSelect;
	CString m_csTitle;
};
