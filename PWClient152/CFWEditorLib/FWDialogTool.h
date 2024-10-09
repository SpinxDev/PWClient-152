#ifndef _FWDIALOGTOOL_H_
#define _FWDIALOGTOOL_H_

#include "FWDialogBase.h"

class AUIStillImageButton;


class FWDialogTool : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogTool)
	DECLARE_EVENT_MAP_FW()
protected:
	static void OnCloseDialogScale(void *pData);
protected:
	AUIStillImageButton * m_pBtnSelect;
	AUIStillImageButton * m_pBtnRotate;
	AUIStillImageButton * m_pBtnResize;
	AUIStillImageButton * m_pBtnGrid;

public:
	FWDialogTool();
	virtual ~FWDialogTool();

	void OnClickBtnGrid(WPARAM wParam, LPARAM lParam);
	void OnUpdateToolbar(WPARAM wParam, LPARAM lParam);
	void OnClickBtnResize(WPARAM wParam, LPARAM lParam);
	void OnClickBtnRotate(WPARAM wParam, LPARAM lParam);
	void OnClickBtnSelect(WPARAM wParam, LPARAM lParam);
	void OnClickBtnUnCombine(WPARAM wParam, LPARAM lParam);
	void OnClickBtnCombine(WPARAM wParam, LPARAM lParam);
	void OnClickBtnScale(WPARAM wParam, LPARAM lParam);

	// base class interface
	virtual bool Init(AUIDialog * pDlg);

};

#endif 