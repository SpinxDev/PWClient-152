// File		: DlgPetHatch.h
// Creator	: Xiao Zhou
// Date		: 2005/12/29

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"

class CECIvtrItem;

class CDlgPetHatch : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgPetHatch();
	virtual ~CDlgPetHatch();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetEggs(CECIvtrItem *pItem, int nSlot);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	PAUIIMAGEPICTURE	m_pImg_Item;
	PAUIOBJECT			m_pTxt_Gold;
	PAUIOBJECT			m_pTxt_Name;
	int					m_nSolt;
};
