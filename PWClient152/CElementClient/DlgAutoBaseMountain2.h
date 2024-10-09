/*
 * FILE: DlgAutoBaseMountain2.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_BASE_MOUNTAIN2_H_
#define _DLG_AUTO_BASE_MOUNTAIN2_H_

#include "DlgHomeBase.h"
#include "AutoParameters.h"
#include "AutoHillLine.h"

class AUIStillImageButton;
class AUIListBox;

class CDlgAutoBaseMountain2 : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoBaseMountain2();
	virtual ~CDlgAutoBaseMountain2();
	
	virtual AUIStillImageButton * GetSwithButton();


	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();



protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void FillMountain2ListBox();
	void FillTextureListBox(); 

	
	bool GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo);
	bool GetMountainLineParameter(int a_nIdx, CAutoParameters::HILLINFO* a_pMounLineInfo);
	void OnCommandCreateMountainLine(const char* szCommand);
	

protected:
	AUIListBox* m_pListBoxMountain2;
	AUIListBox* m_pListBoxTexture;
	int m_nMountain2Idx;
	int m_nTextureIdx;

	
	//begin----------与保存数据和同步数据相关--------------
public:
	void InsertHillLineSettings(const ACHAR* szFileName);
	void InsertTexSettings(const ACHAR* szFileName);
	void SetTexOpEnable(bool i_bEnable);
	
protected:
	int m_iPreHillLineNum;
	int m_iPreTexNum;

	enum BTN_TYPE
	{
		BTN_DEL_HILL_LINE,
		BTN_ADV_HILL_LINE,
		BTN_CREATE_HILL_LINE,
		BTN_DEL_TEX,
		BTN_ADV_TEX,
		BTN_APPLY_TEX,
		BTN_NUM
	};

	AUIStillImageButton* m_pBtn[BTN_NUM];
	
	void OnCommandAdvance(const char* szCommand);
	void OnCommandDelPre(const char* szCommand);
	void OnLButtonUpListBoxHillLine(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnCommandAdvTex(const char* szCommand);
	void OnCommandDelTex(const char* szCommand);
	void OnCommandApplyTex(const char* szCommand);
	void OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	//end----------与保存数据和同步数据相关--------------
	
};

#endif