/*
 * FILE: DlgAutoIndoorToy.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_OUTDOOR_TOY_H_
#define _DLG_AUTO_OUTDOOR_TOY_H_

#include "DlgHomeBase.h"
#include "AutoBuilding.h"

class CDlgAutoIndoorToy: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoIndoorToy();
	virtual ~CDlgAutoIndoorToy();

	virtual AUIStillImageButton * GetSwithButton();
	void AddAllResTreeViewItem(unsigned int uUiqueID);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	
	bool FillListBox();

	void OnEventLBtnUpListToy(WPARAM wParam, LPARAM lParam, AUIObject* pObject);
	void InValidateListIdx();

protected:
	AUIListBox* m_pListBoxToy;
	int m_nToyIdx;

	CAutoBuilding::MODELINFO* m_pModelInfos;
	int m_nNumModelInfos;
};

#endif