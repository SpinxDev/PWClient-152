/*
 * FILE: DlgCustomizePre.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_CUSTOMIZE_PRE_H_
#define _DLG_CUSTOMIZE_PRE_H_

#ifdef _WIN32
	#pragma once
#endif

#include "DlgCustomizeBase.h"
#include "ExpTypes.h"
#include "AUI_ImageGrid.h"

class AUIListBox;

class CDlgCustomizePre : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

protected:
	//ddx control
	AUI_ImageGrid	m_ListBoxPredefined;	//	elementdata 中预定义的个性化数据
	
	//	个性化数据保存
	AUIListBox* m_pListBoxPreCustomize;
	AUIStillImageButton* m_pBtnDeleteCustomizeData;
	abase::vector<char*> m_vecCustomizeFileName;
	
	typedef abase::vector<A2DSprite*>::iterator VecA2DSpriteIter_t;
	abase::vector<A2DSprite*> m_vecA2DSprite;
	
	int		m_iLastStockFile;
	abase::vector<char*> m_vecStockFileName;

protected:
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual AUIStillImageButton * GetSwithButton();
	
	void DoCalculate();
	bool InitResource();
	
	// 载入形象的个性化数据
	bool LoadCustomizeData(const char* pszFile);

	void ApplyCustomizeData(const char *szFile);

	//确认个性化数据是否合法
	bool MakeSureCustomizeData( DATA_TYPE dt, unsigned int id, unsigned int part = 0);

	void ClearListCtrlAndA2DSprite();
	bool LoadUserCustomizeData(void);
	void DeleteFileName(char* szFileName);
	bool CheckFileName(char* szFileName);

public:
	CDlgCustomizePre();
	virtual ~CDlgCustomizePre();
	
	bool SaveUserCustomizeData(const wchar_t *szInputName);

	void OnCommandSaveCusData(const char* szCommand);
	void OnCommandDeleteCusData(const char* szCommand);
	void OnCommandRandomCusData(const char* szCommand);

	void OnLButtonUpListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnKeyDownListBoxPreCus(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	void OnLButtonUpListboxPredefined(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnMouseWheelListboxPredefined(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

};

#endif