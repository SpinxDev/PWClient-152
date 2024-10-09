/*
 * FILE: DlgAutoSwitchDlg.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/4/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_SWITCH_DLG_H_
#define _DLG_AUTO_SWITCH_DLG_H_

#include "DlgHomeBase.h"

class AUIStillImageButton;

class CDlgAutoSwitchDlg : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	enum SwitchBtn_e
	{
		SB_SCENE = 0,
		SB_OUTDOOR,
		SB_INDOOR,
		SB_LIGHTING,
		SB_NUM
	};

	friend class DlgAutoAllRes;
public:
	CDlgAutoSwitchDlg();
	virtual ~CDlgAutoSwitchDlg();

	virtual bool OnInitContext();
	virtual void OnShowDialog();
	
	virtual void DoDataExchange(bool bSave);


	void OnCommandOnSBScene(const char* szCommand);
	void OnCommandOnSBOutdoor(const char* szCommand);
	void OnCommandOnSBIndoor(const char* szCommand);
	void OnCommandOnSBLighting(const char* szCommand);
protected:
	void SwitchBtn(SwitchBtn_e i_Btn);

protected:
	AUIStillImageButton* m_pBtn[SB_NUM];
	AUIStillImageButton* m_pBtnCur;

};

#endif