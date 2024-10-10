/*
 * FILE: DlgAutoIndoorFurniture.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_OUTDOOR_FURNITURE_H_
#define _DLG_AUTO_OUTDOOR_FURNITURE_H_

#include "DlgHomeBase.h"
#include "AutoBuilding.h"

class CDlgAutoIndoorFurniture: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoIndoorFurniture();
	virtual ~CDlgAutoIndoorFurniture();

	virtual AUIStillImageButton * GetSwithButton();
	void AddAllResTreeViewItem(unsigned int uUiqueID);
	

protected:
	virtual void OnShowDialog() ;
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();

	bool FillListBox();

	void OnEventLBtnUpListFuniture(WPARAM wParam, LPARAM lParam, AUIObject* pObject);
	void InValidateListIdx();

protected:
	AUIListBox* m_pListBoxFurniture;
	int m_nFurnitureIdx;

	CAutoBuilding::MODELINFO* m_pModelInfos;
	int m_nNumModelInfos;
};

#endif