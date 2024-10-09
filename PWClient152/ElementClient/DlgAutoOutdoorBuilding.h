/*
 * FILE: DlgAutoOutdoorTree.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_OUTDOOR_BUILDING_H_
#define _DLG_AUTO_OUTDOOR_BUILDING_H_

#include "DlgHomeBase.h"
#include "AutoBuilding.h"

class AUIListBox;

class CDlgAutoOutdoorBuilding: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoOutdoorBuilding();
	virtual ~CDlgAutoOutdoorBuilding();

	virtual AUIStillImageButton * GetSwithButton();
	void AddAllResTreeViewItem(unsigned int uUiqueID);
	
	

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	

	void LoadDefault();
	bool InitResource();
	
	bool FillBuildingListBox();

	void OnEventLBtnUpBuildingList(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void InValidateListIdx();
	
protected:
	AUIListBox* m_pListBoxBuildings;
	int m_nBuildingIdx;

	CAutoBuilding::MODELINFO* m_pModelInfos;
	int m_nNumModelInfos;
};

#endif