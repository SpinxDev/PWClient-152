#include "StdAfx.h"
#include "DynSizeCtrl.h"

DynSizeCtrl::DynSizeCtrl(CWnd* pCtrl, CWnd* pParent, bool bChangePosOnly)
: pWnd(pCtrl),bChangePosOnly(bChangePosOnly)
{
	ASSERT(pCtrl && pParent);
	CRect rcCtrl;
	pCtrl->GetWindowRect(&rcCtrl);
	CRect rcParent;
	pParent->GetClientRect(&rcParent);
	pParent->ScreenToClient(&rcCtrl);
	float fDlgWidth = static_cast<float>(rcParent.Width());
	float fDlgHeight = static_cast<float>(rcParent.Height());
	fXPercent = rcCtrl.left / fDlgWidth;
	fYPercent = rcCtrl.top / fDlgHeight;
	nOriWidth = rcCtrl.Width();
	nOriHeight = rcCtrl.Height();
	fWidthPercent = rcCtrl.Width() / fDlgWidth;
	fHeightPercent = rcCtrl.Height() / fDlgHeight;
}

void DynSizeCtrl::ResizeCtrl(int cx, int cy)
{
	if(!pWnd)
		return;
	if (bChangePosOnly)
	{
		pWnd->MoveWindow((int)(fXPercent * cx), (int)(fYPercent * cy), nOriWidth, nOriHeight);
	}
	else
	{
		pWnd->MoveWindow((int)(fXPercent * cx), (int)(fYPercent * cy), (int)(fWidthPercent * cx), (int)(fHeightPercent * cy));
	}		
}

void DynSizeCtrlMan::Add(DynSizeCtrl dynSizeCtrl)
{
	m_dscList.push_back(dynSizeCtrl);
}

void DynSizeCtrlMan::Clear()
{
	m_dscList.clear();
}

void DynSizeCtrlMan::Resize(int cx, int cy)
{
	typedef std::list<DynSizeCtrl>::iterator DynSizeCtrlIterator;
	for (DynSizeCtrlIterator itr = m_dscList.begin(); itr != m_dscList.end();	++itr)
	{	
		(*itr).ResizeCtrl(cx, cy);
	}
}