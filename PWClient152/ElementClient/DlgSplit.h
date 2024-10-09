// Filename	: DlgSplit.h
// Creator	: Tom Zhou
// Date		: October 12, 2005

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;
class CDlgSplit : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgSplit();
	virtual ~CDlgSplit();

	void OnCommand_begin(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	PAUIOBJECT m_pTxt_no1;
	PAUIOBJECT m_pTxt_no2;
	PAUIOBJECT m_pTxt_Gold;
	PAUIPROGRESS m_pPrgs_1;
	PAUIIMAGEPICTURE m_pImgItem_a;
	PAUIIMAGEPICTURE m_pImgItem_b;
	PAUISTILLIMAGEBUTTON m_pBtn_Start;
	PAUISTILLIMAGEBUTTON m_pBtn_Cancel;

	virtual bool OnInitDialog();
};
