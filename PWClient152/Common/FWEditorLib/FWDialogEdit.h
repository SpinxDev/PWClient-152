#ifndef _FWDIALOGEDIT_H_
#define _FWDIALOGEDIT_H_

#include "FWDialogBase.h"

class AUIProgress;

class FWDialogEdit : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogEdit)
	DECLARE_EVENT_MAP_FW()
protected:
	AUIProgress * m_pPrgParticalQuota;
	AUIProgress * m_pPrgSoundQuota;

protected:
	void OnLButtonDownListGlyph(WPARAM wParam, LPARAM lParam);
	void UpdateGlyphListbox();
	void UpdateParticalQuotaProgress();
	void UpdateSoundQuotaProgress();

public:
	FWDialogEdit();
	virtual ~FWDialogEdit();

	void OnClickBtnInvertSelect(WPARAM wParam, LPARAM lParam);
	void OnUpdateToolbar(WPARAM wParam, LPARAM lParam);
	void OnClickBtnPaste(WPARAM wParam, LPARAM lParam);
	void OnClickBtnCut(WPARAM wParam, LPARAM lParam);
	void OnClickBtnCopy(WPARAM wParam, LPARAM lParam);
	void OnClickBtnCenter(WPARAM wParam, LPARAM lParam);
	void OnLButtonDblClickListGlyph(WPARAM wParam, LPARAM lParam);
	void OnClickBtnProp(WPARAM wParam, LPARAM lParam);
	void OnClickBtnSelectAll(WPARAM wParam, LPARAM lParam);
	void OnClickBtnRedo(WPARAM wParam, LPARAM lParam);
	void OnClickBtnUndo(WPARAM wParam, LPARAM lParam);
	void OnClickBtnDelete(WPARAM wParam, LPARAM lParam);

	// base class interface
	virtual bool Init(AUIDialog * pDlg);
};

#endif 