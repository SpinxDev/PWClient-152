// File		: DlgEquipDestroy.h
// Creator	: Xiao Zhou
// Date		: 2006/7/19

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgEquipDestroy : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgEquipDestroy();
	virtual ~CDlgEquipDestroy();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetItem(CECIvtrItem *pItem, int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUIOBJECT				m_pTxt_Name;
	int						m_nSolt;
};
