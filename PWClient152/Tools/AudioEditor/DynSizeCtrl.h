#pragma once
#include <list>

struct DynSizeCtrl
{
public:
	DynSizeCtrl(CWnd* pCtrl, CWnd* pParent, bool bChangePosOnly = false);
	void ResizeCtrl(int cx, int cy);
protected:
	CWnd* pWnd;
	float fXPercent;		// x / dlg's length
	float fYPercent;		// y / dlg's height
	float fWidthPercent;	// width / dlg's length
	float fHeightPercent;	// height / dlg's height
	int nOriWidth;			//original control width
	int nOriHeight;			//original control height
	bool bChangePosOnly;
};

struct DynSizeCtrlMan
{
public:
	void Add(DynSizeCtrl dynSizeCtrl);
	void Resize(int cx, int cy);
	void Clear();
protected:
	std::list<DynSizeCtrl> m_dscList;
};