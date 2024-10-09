// File		: DlgPetRename.h
// Creator	: Xiao Zhou
// Date		: 2006/6/22

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"

class CECIvtrItem;

class CDlgPetRename : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgPetRename();
	virtual ~CDlgPetRename();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetPet(int nSlot);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUILABEL			m_pTxt_PetName;
	PAUILABEL			m_pTxt_PetLevel;
	PAUIOBJECT			m_pTxt_PetNewName;
	PAUIIMAGEPICTURE	m_pImg_Item;
	int					m_nSlot;
};
