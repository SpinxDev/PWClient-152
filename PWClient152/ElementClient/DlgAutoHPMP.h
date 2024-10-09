// Filename	: DlgAutoHPMP.h
// Creator	: Xu Wenbin
// Date		: 2012/8/8

#pragma once

#include "DlgBase.h"
#include "EC_Configs.h"
#include <AUICheckbox.h>
#include <AUIImagePicture.h>
#include <AUIEditBox.h>

class CDlgAutoHPMP : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgAutoHPMP();
	virtual ~CDlgAutoHPMP(){}

	virtual void SwitchShow();

	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnCommand_Confirm(const char *szCommand);
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Enable(const char *szCommand);

	void Load();
	void UpdateUI();

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	
protected:	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

private:
	int  GetItemID(int group);
	short GetItemSlot(int group);
	void SetItemID(int group, int item_id);
	void SetItemSlot(int group, short slot);
	char GetPercent(int group);
	bool GetEnable(int group);
	void SetEnable(int group, bool bEnable);
	void ClearItem(int group);
	void EnableItem(int group, bool bEnable);
	void SetItem(int group, short slot);

private:
	enum {COUNT = EC_COMPUTER_AIDED_SETTING::GROUP_COUNT};
	PAUICHECKBOX		m_pChk[COUNT];
	PAUIIMAGEPICTURE	m_pImg[COUNT];
	PAUIEDITBOX			m_pEdt[COUNT];
};
