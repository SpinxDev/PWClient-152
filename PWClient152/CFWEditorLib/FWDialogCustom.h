#ifndef _FWDIALOGCUSTOM_H_
#define _FWDIALOGCUSTOM_H_

#include "FWDialogPropBase.h"
#include "FWModifier.h"

class FWDialogCustom : public FWDialogPropBase  
{
	FW_DECLARE_DYNCREATE(FWDialogCustom)
	DECLARE_EVENT_MAP_FW()
protected:
	FWModifier * m_pModifier;
	AUIListBox * m_pListModifier;
	int m_nLastSelIndexModifier;
	AUIEditBox * m_pEditCount;
protected:
	virtual void OnShowDialog();
	virtual int GetProfileType();

	static void OnDoModalModifierCallback(void *pData);
	void InitModifierList();
	void OnSelChangeListModifier(int nSelIndex);
public:
	int m_nCount;
	const FWModifier * GetModifier();
	void SetModifier(const FWModifier * pModifier);
public:
	FWDialogCustom();
	virtual ~FWDialogCustom();
	
	virtual void OnOK();
	virtual bool Init(AUIDialog * pDlg);

	void OnClickBtnModifier(WPARAM wParam, LPARAM lParam);
	void OnLButtonDownListModifier(WPARAM wParam, LPARAM lParam);
};

#endif 