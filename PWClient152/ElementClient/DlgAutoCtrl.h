/*
 * FILE: DlgAutoCtrl.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_CTRL_H_
#define _DLG_AUTO_CTRL_H_

#include "DlgHomeBase.h"

class AUIStillImageButton;

class CDlgAutoCtrl: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	enum CtrlModelOperation_e
	{
		CMO_MOVEFRONT = 0,
		CMO_MOVEBACK,
		CMO_MOVELEFT,
		CMO_MOVERIGHT,
		CMO_MOVEUP,
		CMO_MOVEDOWN,
		CMO_TURNLEFT,
		CMO_TURNRIGHT,
		CMO_NUM
	};
public:
	CDlgAutoCtrl();
	virtual ~CDlgAutoCtrl();
	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnTick();
	void OnCommandDropModel(const char* szCommand);
	void OnEventLButtonUpOnTurnLeftAndRight(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
protected:
	AUIStillImageButton* m_pBtn[CMO_NUM];
};

#endif