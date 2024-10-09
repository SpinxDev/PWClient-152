// Filename	: DlgAutoPolicy.h
// Creator	: Shizhenhua
// Date		: 2013/8/30

#pragma once

#include "DlgBase.h"
#include <AUIImagePicture.h>
#include <AUIEditBox.h>
#include <AUICheckBox.h>
#include <AUIStillImageButton.h>
#include <AUILabel.h>
#include <AUIComboBox.h>


class CDlgAutoPolicy : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoPolicy();

	void OnCommand_Confirm(const char * szCommand);
	void OnCommand_StartOrStop(const char * szCommand);
	void OnCommand_SetAutoHPMP(const char * szCommand);
	void OnCommand_AutoPickup(const char * szCommand);

	void OnEvent_LButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void DragDropItem(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUIIMAGEPICTURE m_pImg_Skill1;
	PAUIIMAGEPICTURE m_pImg_Skill2;
	PAUIEDITBOX m_pTxt_Interval;
	PAUIEDITBOX m_pTxt_Time;
	PAUIEDITBOX m_pTxt_Range;
	PAUICOMBOBOX m_pCmb_AutoPick;
	PAUISTILLIMAGEBUTTON m_pBtn_Switch;
	PAUILABEL m_pLbl_RemainTime;

private:
	void SaveConfigData();
	void SetSkillImage(PAUIIMAGEPICTURE pSkillImg, int skill_id, bool bCombo);
	ACString GetRemainTimeTxt();
};