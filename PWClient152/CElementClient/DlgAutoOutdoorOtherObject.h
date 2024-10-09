/*
 * FILE: DlgAutoOutdoorOtherObject.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/3/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_OUTDOOR_OTHEROBJECT_H_
#define _DLG_AUTO_OUTDOOR_OTHEROBJECT_H_

#include "DlgHomeBase.h"
#include "AutoBuilding.h"

class CDlgAutoOutdoorOtherObject: public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoOutdoorOtherObject();
	virtual ~CDlgAutoOutdoorOtherObject();

	virtual AUIStillImageButton * GetSwithButton();
	void AddAllResTreeViewItem(unsigned int uUiqueID);
	

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();

	bool FillOtherObjectListBox();

	void OnEventLBtnUpListOtherObject(WPARAM wParam, LPARAM lParam, AUIObject* pObject);
	void InValidateListIdx();

protected:
	AUIListBox* m_pListBoxOtherObject;
	int m_nOtherObjectIdx;

	CAutoBuilding::MODELINFO* m_pModelInfos;
	int m_nNumModelInfos;
};

#endif