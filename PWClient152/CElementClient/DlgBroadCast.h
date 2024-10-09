/*
 * FILE: DlgBroadCast.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/9/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_BROAD_CAST_H_
#define _DLG_BROAD_CAST_H_

#include "DlgBase.h"

class AUIListBox;
class AWString;
class AString;

class CDlgBroadCast : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgBroadCast();
	virtual ~CDlgBroadCast();
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
protected:
	void OnCommandConfirm(const char* szCommand);
	void OnCommandCancel(const char* szCommand);

	void ClearListBox();

	AUIListBox* m_pListBoxChannel;
	abase::vector<ACString*> m_vecStringW;
	abase::vector<ACString*> m_vecStringA;
};

#endif