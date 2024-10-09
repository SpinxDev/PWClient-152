// Filename	: DlgDamageRep.h
// Creator	: Xu Wenbin
// Date		: 2009/07/10

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"

class CECIvtrItem;
class CDlgDamageRep : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgDamageRep();
	virtual ~CDlgDamageRep();
	
	void OnCommand_Confirm(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void SetEquip(int nSlot);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	
private:
	void ClearAll();
	void UpdateUI();
	void UpdateDye();
	bool CanAcceptEquip(CECIvtrItem *pItem);
	bool GetRepairIDAndNum(CECIvtrItem *pItem, int &id, int &num);
	
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	PAUIIMAGEPICTURE		m_pImg_Equip;
	PAUIIMAGEPICTURE		m_pImg_Dye;
	PAUIOBJECT				m_pTxt_Num;
	int						m_nSlot;
};
