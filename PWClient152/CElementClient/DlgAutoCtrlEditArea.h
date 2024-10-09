/*
 * FILE: DlgAutoCtrlEditArea.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_CTRL_EDIT_AREA_H_
#define _DLG_AUTO_CTRL_EDIT_AREA_H_

#include "DlgHomeBase.h"

class CDlgAutoCtrlEditArea : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	enum CtrlEditAreaOperation_e
	{
		CEAO_MOVEFRONT = 0,
		CEAO_MOVEBACK,
		CEAO_MOVELEFT,
		CEAO_MOVERIGHT,
		CEAO_TURNLEFT,
		CEAO_TURNRIGHT,
		CEAO_ZOOMOUT,
		CEAO_ZOOMIN,
		CEAO_NUM
	};

public:
	CDlgAutoCtrlEditArea();
	virtual ~CDlgAutoCtrlEditArea();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnTick();
	class AUIStillImageButton* m_pBtn[CEAO_NUM];
	
};
#endif