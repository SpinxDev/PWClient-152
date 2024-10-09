/*
 * FILE: DlgAutoBaseRoad.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_BASE_ROAD_H_
#define _DLG_AUTO_BASE_ROAD_H_

#include "DlgHomeBase.h"
#include "AutoParameters.h"

class AUIStillImageButton;
class AUIListBox;

class CDlgAutoBaseRoad : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoBaseRoad();
	virtual ~CDlgAutoBaseRoad();
	
	virtual AUIStillImageButton * GetSwithButton();
	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void FillTextureListBox();
	void FillRoadInfoListBox();
	bool GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo);
	bool GetRoadInfoParameter(int a_nIdx, CAutoParameters::ROADINFO* a_pRoadInfo);
	void OnCommandCreateRoad(const char* szCommand);

	AUIListBox* m_pListBoxTexture;
	int m_nTextureIdx;

	AUIListBox* m_pListBoxRoadInfo;
	int m_nRoadInfoIdx;
    

	//begin----------与保存数据和同步数据相关--------------
public:
	void InsertRoadSettings(const ACHAR* szFileName);
	void InsertTexSettings(const ACHAR* szFileName);
	void SetTexOpEnable(bool i_bEnable);
	
protected:
	int m_iPreRoadNum;
	int m_iPreTexNum;

	enum BTN_TYPE
	{
		BTN_DEL_ROAD,
		BTN_ADV_ROAD,
		BTN_CREATE_ROAD,
		BTN_DEL_TEX,
		BTN_ADV_TEX,
		BTN_APPLY_TEX,
		BTN_NUM
	};

	AUIStillImageButton* m_pBtn[BTN_NUM];
	
	void OnCommandAdvance(const char* szCommand);
	void OnCommandDelPre(const char* szCommand);
	void OnLButtonUpListBoxRoad(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnCommandAdvTex(const char* szCommand);
	void OnCommandDelTex(const char* szCommand);
	void OnCommandApplyTex(const char* szCommand);
	void OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	

	//end----------与保存数据和同步数据相关--------------

};

#endif