// Filename	: DlgIdentify.h
// Creator	: Tom Zhou
// Date		: October 12, 2005

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;
class CDlgIdentify : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgIdentify();
	virtual ~CDlgIdentify();

	void OnCommand_start(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	PAUIIMAGEPICTURE m_pImgItem;
	PAUISTILLIMAGEBUTTON m_pBtn_Confirm;

	virtual bool OnInitDialog();
};
