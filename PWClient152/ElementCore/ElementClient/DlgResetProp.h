// File		: DlgResetProp.h
// Creator	: Xiao Zhou
// Date		: 2006/1/12

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIRadioButton.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgResetProp : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgResetProp();
	virtual ~CDlgResetProp();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetScroll(CECIvtrItem *pItem);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	PAUILABEL				m_pTxt_Item[5];
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUIRADIOBUTTON			m_pRdo_Item[6];
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	
};
