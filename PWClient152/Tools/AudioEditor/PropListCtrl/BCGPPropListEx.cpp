#include "stdafx.h"
#include "BCGPPropListEx.h"
#include "BCGPPropertyListWrapper.h"
#include "..\Global.h"

BEGIN_MESSAGE_MAP(CBCGPPropListEx, CBCGPPropList)
	ON_MESSAGE(WM_KILLBCGPPROPSEL, OnKillBCGPPropSel)
END_MESSAGE_MAP()

CBCGPPropListEx::CBCGPPropListEx(CBCGPPropertyListWrapper* pWrapper)
: m_pWrapper(pWrapper)
, m_bEnable(true)
{


}

//	Enable property change message notify
void CBCGPPropListEx::EnablePropertyChanged(bool bEnable)
{
	m_bEnable = bEnable;
}

//	Wrapper for base_class's RemoveAll
void CBCGPPropListEx::RemoveAllItems()
{
	EnablePropertyChanged(false);
	RemoveAll();
	EnablePropertyChanged(true);
}

//	Commit the property change
void CBCGPPropListEx::OnPropertyChangedNoCommit (CBCGPProp* pProp) const
{
	CBCGPPropList::OnPropertyChanged(pProp);

	if (!m_bEnable)
		return;

	m_pWrapper->UpdateData(true, false);
}


void CBCGPPropListEx::OnPropertyChanged (CBCGPProp* pProp) const
{
	CBCGPPropList::OnPropertyChanged(pProp);
	
	if (!m_bEnable)
		return;

	m_pWrapper->UpdateData(true, true);
}

LRESULT CBCGPPropListEx::OnKillBCGPPropSel(WPARAM wParam, LPARAM lParam)
{
	SetCurSel(NULL);
	return 0;
}