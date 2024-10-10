/*
 * FILE: DlgAutoNote2.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_NOTE2_H_
#define _DLG_AUTO_NOTE2_H_

#include "DlgHomeBase.h"

class AUILabel;
class AUIStillImageButton;

class CDlgAutoNote2 : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	enum AreaType
	{
		AT_LINE,
		AT_POLY,
		AT_MODEL
	};


	CDlgAutoNote2();
	virtual ~CDlgAutoNote2();
	void SetNote2(AreaType a_areaType);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	void DoDataExchange(bool bSave);
	
	void OnCommandCloseArea(const char* szCommand);
	void OnCommandDeleteEditPoint(const char* szCommand);

protected:
	AUILabel* m_pLabelNote2;
	AUIStillImageButton* m_pBtnCloseArea;
	AUIStillImageButton* m_pBtnDelPoint;

};

#endif