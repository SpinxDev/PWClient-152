/*
 * FILE: DlgAutoBaseWater.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_BASE_WATER_H_
#define _DLG_AUTO_BASE_WATER_H_


#include "DlgHomeBase.h"
#include "AutoParameters.h"

class AUIStillImageButton;
class AUIListBox;
class AUISlider;
class AUILabel;

class CDlgAutoBaseWater : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoBaseWater();
	virtual ~CDlgAutoBaseWater();
	
	virtual AUIStillImageButton * GetSwithButton();
protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void SetLabelsText();
	void SetSliderBound();

	void FillTextureListBox();

	bool GetTextureParameter(int a_nIdx, CAutoParameters::TEXTUREINFO* a_pTexInfo);

	void UpdateTerrainHeight();
	void OnCommandCreateWater(const char* szCommand);
	void OnCommandMoveSlider(const char * szCommand);


	AUIListBox* m_pListBoxTexture;
	int m_nTextureIdx;

	AUISlider* m_pSliderWaterHeight;
	AUISlider* m_pSliderSandWidth;

	AUILabel* m_pLabelWaterHeight;
	AUILabel* m_pLabelSandWidth;

	int m_nWaterHeight;
	int m_nSandWidth;
	
	//begin----------与保存数据和同步数据相关--------------
public:
	void InsertTexSettings(const ACHAR* szFileName);
	
protected:
	enum BTN_TYPE
	{
		BTN_DEL_TEX,
		BTN_ADV_TEX,
		BTN_APPLY_TEX,
		BTN_NUM
	};
	
	int m_iPreTexNum;
	AUIStillImageButton* m_pBtn[BTN_NUM];

	void OnCommandAdvTex(const char* szCommand);
	void OnCommandDelTex(const char* szCommand);
	void OnCommandApplyTex(const char* szCommand);
	void OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	//end----------与保存数据和同步数据相关--------------
};

#endif