// Filename	: DlgTargetItem.h
// Creator	: Shizhenhua
// Date		: 2014/2/12

#pragma once

#include "DlgBase.h"
#include "EC_Counter.h"
#include <AUILabel.h>
#include <AUIImagePicture.h>

class CECIvtrTargetItem;
class CDlgTargetItem : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgTargetItem();

	void OnEvent_LButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	void UseItem();

protected:
	PAUILABEL m_pLbl_Name;
	PAUIIMAGEPICTURE m_pImg_Item;
	CECCounter m_cntCheck;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void UpdateIconState();

	CECIvtrTargetItem *GetItem();
};