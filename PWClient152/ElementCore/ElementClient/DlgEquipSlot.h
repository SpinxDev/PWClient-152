// File		: DlgEquipSlot.h
// Creator	: Xiao Zhou
// Date		: 2008/6/3

#pragma once

#include "DlgBase.h"

class AUIImagePicture;
class AUIEditBox;
class CECIvtrItem;

class CDlgEquipSlot : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgEquipSlot();
	virtual ~CDlgEquipSlot();

	void OnCommandCANCEL(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetEquip(int iSrc);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	
protected:
	virtual bool OnInitDialog();
	
	AUIImagePicture *	m_pImg_Equip;
	PAUIOBJECT		m_pTxt_Name;
	PAUIOBJECT		m_pTxt_SlotNumber;
	PAUIOBJECT		m_pTxt_Number1;
};
