#ifndef _FWDIALOGPROPBASE_H_
#define _FWDIALOGPROPBASE_H_

#include "FWDialogBase.h"
#include "FWAssemblyProfile.h"

class AUIListBox;
class AUIEditBox;

class FWDialogPropBase : public FWDialogBase  
{
	FW_DECLARE_DYNAMIC(FWDialogPropBase)
	DECLARE_EVENT_MAP_FW()
protected:
	// helper for generate onselchange event for listbox
	int m_nLastSelIndexProfile;
	
	AUIEditBox * m_pEditName;
	AUIEditBox * m_pEditTime;
	AUIListBox * m_pListProfile;
	
	// whether the data is modified by user
	// before the dialog close
	bool m_bModified;
protected:
	// this is the base/intermedia class that should not be instantiation
	FWDialogPropBase();
	virtual ~FWDialogPropBase();
	
	// this function will be called before
	// the dialog is made visible
	virtual void OnShowDialog();

	// fill profilelist
	virtual void InitProfileList();

	// this function will be called by default InitProfileList()
	// to get the type of profile to fill in the list
	// derived class should override this function to return 
	// the property type
	virtual int GetProfileType();

	// select first item in listbox if no item is selected currently
	void CheckListbox(AUIListBox * pListbox, FuncOnSelChangeListbox pFunc);

	static void OnDoModalProfileCallback(void *pData);
	void OnSelChangeListProfile(int nSelIndex);
public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual bool Init(AUIDialog * pDlg);
	virtual void Show (bool bShow, bool bModal = false);
	
	bool IsModified();
	
	void OnClickBtnProfile(WPARAM wParam, LPARAM lParam);
	void OnLButtonDownListProfile(WPARAM wParam, LPARAM lParam);

public:
	ACString m_strName;
	float m_fTime;
	FWAssemblyProfile m_profile;
};

#endif 