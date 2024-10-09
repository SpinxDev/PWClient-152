// File		: DlgPetDye.h
// Creator	: Xu Wenbin
// Date		: 2010/3/8

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;
class CECPetData;

class CDlgPetDye : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgPetDye();
	virtual ~CDlgPetDye();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Dye(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetPet(int nSlot);
	void SetDye(CECIvtrItem *pItem, int nSlot);	

	int    GetPetSlot()const{ return m_nSlotItem; }
	int    GetDyeSlot()const { return m_nSlotDye; }

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE			m_pImg_Item;
	PAUIIMAGEPICTURE			m_pImg_Dye;
	PAUIOBJECT				m_pTxt_Num;
	PAUIOBJECT				m_pTxt_Dye;
	PAUIOBJECT				m_pTxt_Name;
	PAUILABEL				m_pTxt_Color;
	int						m_nSlotItem;
	int						m_nSlotDye;
	int						m_nRequireNum;

private:

	void ClearDialog();
	void ClearItem();
	void ClearDye();

	bool GetRequireDyeCount(const CECPetData *pPet, int &count);
	int		GetDyeCount();
	void UpdateRequireNum();
	void UpdateNumDisplay();

	void UpdateFittingRoom();
};
