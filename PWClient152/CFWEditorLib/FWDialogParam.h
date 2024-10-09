#ifndef _FWDIALOGPARAM_H_
#define _FWDIALOGPARAM_H_

#include "FWDialogBase.h"
#include "FWParam.h"

class AUIListBox;
class AUIEditBox;
class AUILabel;

class FWDialogParam : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogParam)
	DECLARE_EVENT_MAP_FW()
protected:
	void UpdateParam();
	void ShowProp();

protected:
	int m_nLastSelInListParam;
	AUIListBox * m_pListParam;
	AUILabel * m_pLabelParam;
	AUIEditBox * m_pEditParam;
public:
	FWDialogParam();
	virtual ~FWDialogParam();

	void OnSelChangeListParam(int nSelIndex);
	void OnLButtonDownListParam(WPARAM wParam, LPARAM lParam);
	FWParamArray m_aryParam;
	virtual bool Init(AUIDialog * pDlg);
	virtual void OnOK();
	virtual void OnCancel();
	virtual void Show (bool bShow, bool bModal = false);
};

#endif 