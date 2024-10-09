// File		: DlgPetRec.h
// Creator	: Xiao Zhou
// Date		: 2005/12/29

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"

class CDlgPetRec : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgPetRec();
	virtual ~CDlgPetRec();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetPet(int nSlot);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	
	PAUIIMAGEPICTURE	m_pImg_Item;
	PAUIOBJECT			m_pTxt_Gold;
	PAUIOBJECT			m_pTxt_Name;
	int					m_nSlot;
};
