// File		: DlgEquipMark.h
// Creator	: Xu Wenbin
// Date		: 2012/11/7

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;
class CECIvtrEquip;
class CECIvtrDyeTicket;

class CDlgEquipMark : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgEquipMark();
	virtual ~CDlgEquipMark();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	void OnCommandLook(const char * szCommand);

	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Dye(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetEquip(int nSlot);
	void SetDye(int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	CECIvtrEquip *GetEquip(int nSlot);
	CECIvtrDyeTicket *GetDye(int nSlot);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUISTILLIMAGEBUTTON	m_pBtn_Look;

	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUIEDITBOX				m_pTxt_Name;
	PAUIIMAGEPICTURE		m_pImg_Dye;
	PAUILABEL				m_pTxt_Color;
	PAUIEDITBOX				m_pTxt_Num;
	PAUIEDITBOX				m_pEdt_Mark;

	int						m_nSlot;
	int						m_nSlotDye;

private:
	void ClearDialog();

	ACString GetNewMark();
	A3DCOLOR GetMarkColor();

	int  GetRequireDyeCount();
	int  GetDyeCount();
	void UpdateNumDisplay();

	bool CanConfirm();
	bool CanLook();
};
