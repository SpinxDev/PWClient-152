// File		: DlgSkillEdit.h
// Creator	: Xiao Zhou
// Date		: 2005/12/20

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"

class CDlgSkillEdit : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgSkillEdit();
	virtual ~CDlgSkillEdit();

	void DragDropItem(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void SelectSkill(int n);

	void OnCommandConfirm(const char * szCommand);
	void OnCommandMoveLeft(const char * szCommand);
	void OnCommandMoveRight(const char * szCommand);

	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Icon(WPARAM wParam, LPARAM lParam, AUIObject * pObj);


protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	void SetSpecialIcon(PAUIIMAGEPICTURE pImage, int iType);

	PAUIIMAGEPICTURE	m_pImg_Item;
	PAUIOBJECT			m_pTxt_Name;
	int					m_nItemSelect;
	int					m_nIcon;
};
