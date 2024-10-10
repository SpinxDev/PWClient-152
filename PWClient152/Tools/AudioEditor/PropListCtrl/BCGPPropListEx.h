#ifndef _BCGP_PROP_LIST_EX_H_
#define _BCGP_PROP_LIST_EX_H_

#include "BCGPPropList.h"

class CBCGPPropertyListWrapper;

// Define the custom version of CBCGPPropList
// In order to process the prop changed event
class CBCGPPropListEx : public CBCGPPropList
{
	DECLARE_MESSAGE_MAP();

public:
	typedef CBCGPPropList base_class;

	CBCGPPropListEx(CBCGPPropertyListWrapper* pWrapper);

	//	Public operations
public:

	//	Enable property change message notify
	void EnablePropertyChanged(bool bEnable);

	//	Wrapper for base_class's RemoveAll
	//	Use this instead of the base_class's RemoveAll
	//	Because the base_class's RemoveAll would destroy all sub-window during which the WM_KILLFOCUS message would be sent
	//	Then the CBCGPPropList's OnPropertyChanged would be called which has been overridden by this class
	//	We would update the PropList's data in the OnPropertyChanged callback, during which we would find the PropertyList is actually been destroying / destroyed
	void RemoveAllItems();
	//	property change no commit
	void OnPropertyChangedNoCommit (CBCGPProp* pProp) const;

	//	Override operations
protected:
	//	property changed with commit
	virtual void OnPropertyChanged (CBCGPProp* pProp) const;
	
	afx_msg LRESULT OnKillBCGPPropSel(WPARAM wParam, LPARAM lParam);

private:
	CBCGPPropertyListWrapper* m_pWrapper;
	bool m_bEnable;
};

#endif