/*
 * FILE: DlgCustomizePreForFree.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/9/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_CUSTOMIZE_PRE_FOR_FREE_H_
#define _DLG_CUSTOMIZE_PRE_FOR_FREE_H_

#ifdef _WIN32
	#pragma once
#endif

#include "DlgCustomizeBase.h"
#include "ExpTypes.h"

class AUIListBox;

class CDlgCustomizePreForFree : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

protected:
	//ddx control
	AUIListBox* m_pListBoxPreCustomize;
	
protected:
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	void Clear();


 	bool InitResource();
 
	// 载入形象的个性化数据
	bool LoadCustomizeData(char* pszFile);

	//确认个性化数据是否合法
	bool MakeSureCustomizeData( DATA_TYPE dt, unsigned int id, unsigned int part = 0);



public:
	CDlgCustomizePreForFree();
	~CDlgCustomizePreForFree();

	void OnLButtonUpListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnKeyDownListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj);


};

#endif // _DLG_CUSTOMIZE_PRE_FOR_FREE_H_