// Filename	: DlgELFEquipRemove.h
// Creator	: Chen Zhixin
// Date		: November 27, 2008

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "AUIStillImageButton.h"
#include "AUITextArea.h"
#include "AUIEditBox.h"
#include "AUICheckBox.h"
#include "EC_IvtrItem.h"

class CDlgELFEquipRemove : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFEquipRemove();
	virtual ~CDlgELFEquipRemove();
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Confirm(const char * szCommand);
	void OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_NewItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetELF(CECIvtrItem *pItem, int nSlot);
	void SetItem(CECIvtrItem *pItem, int nSlot, int index);
	void ConfirmOK(bool bConfirm);
	void RefineResult(int result);
	
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	
protected:
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE		m_pImg_ELF;
	PAUIOBJECT				m_pTxt_Item;
	PAUICHECKBOX			m_pChk_Equip[4];
	PAUIIMAGEPICTURE		m_pImg_OldItem[4];
	PAUIIMAGEPICTURE		m_pImg_NewItem[4];
	PAUIIMAGEPICTURE		m_pImg_Item[4];
	PAUIIMAGEPICTURE		m_pImg_Arrow[4];
	PAUILABEL				m_pLab_NoItem[4];
	int						m_nSolt;
	int						m_nLastSolt;
	int						m_dwEnalbeTick;
	int						m_nSlotNewItem[4];
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void RefreshHostDetails();
};
