/*
 * FILE: DlgAutoIndoorVirtu.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_OUTDOOR_VIRTU_H_
#define _DLG_AUTO_OUTDOOR_VIRTU_H_

#include "DlgHomeBase.h"
#include "AutoBuilding.h"

class CDlgAutoIndoorVirtu: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoIndoorVirtu();
	virtual ~CDlgAutoIndoorVirtu();

	virtual AUIStillImageButton * GetSwithButton();
	void AddAllResTreeViewItem(unsigned int uUiqueID);

protected:
	virtual void OnShowDialog() ;
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();

	bool FillListBox();
	
	void OnEventLBtnUpListVirtu(WPARAM wParam, LPARAM lParam, AUIObject* pObject);
	void InValidateListIdx();

protected:
	AUIListBox* m_pListBoxVirtu;
	int m_nVirtuIdx;

	CAutoBuilding::MODELINFO* m_pModelInfos;
	int m_nNumModelInfos;
};

#endif