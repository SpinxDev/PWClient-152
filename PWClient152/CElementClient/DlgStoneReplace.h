// File		: DlgStoneReplace.h 
// Creator	: Wang And Shi
// Date		: 2013/01/14

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"


class CDlgStoneReplace : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgStoneReplace();
	virtual ~CDlgStoneReplace();

	void OnCommandCANCEL(const char * szCommand);
	void OnCommandReplace(const char * szCommand);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	void DoReplace();

protected:
	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	
	PAUIIMAGEPICTURE m_pItem;
	PAUILABEL m_pStoneName;
	PAUILABEL m_pStoneAttri;
	PAUISTILLIMAGEBUTTON m_pReaplceBtn;
};
