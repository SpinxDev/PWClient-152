/*
 * FILE: DlgAutoRoad.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_ROAD_H_
#define _DLG_AUTO_ROAD_H_

#include "DlgHomeBase.h"
#include "AutoParameters.h"

class AUILabel;
class AUISlider;

class CDlgAutoRoad : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:
	

	AUISlider* m_pSliderMaxRoadHeight;
	AUISlider* m_pSliderMainSmoothRoad;
	AUISlider* m_pSliderPartSmoothRoad;
	AUISlider* m_pSliderRoadWidth;
	AUISlider* m_pSliderSeedRoad;

	AUILabel* m_pLabelMaxRoadHeight;
	AUILabel* m_pLabelMainSmoothRoad;
	AUILabel* m_pLabelPartSmoothRoad;
	AUILabel* m_pLabelRoadWidth;
	AUILabel* m_pLabelSeedRoad;
	
	int m_nMaxRoadHeight;
	int m_nMainSmoothRoad;
	int m_nPartSmoothRoad;
	int m_nRoadWidth;
	int m_nSeedRoad;

	bool m_bCheckFlatRoad;

	TextureParamInfo_t m_texParamInfo;

	//bool m_bChangeRoad;
	
public:
	CDlgAutoRoad();
	virtual ~CDlgAutoRoad();


	TextureParamInfo_t* GetTextureParamInfo() { return &m_texParamInfo;}

	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();
	
	//begin----------与保存数据和同步数据相关--------------
	void InitSettings(ROADINFO &i_RoadInfo,
					  bool		i_bSaveEnable = false,
					  const char* i_szFileName = NULL);
	//end------------与保存数据和同步数据相关-------------

protected:

	virtual void DoDataExchange(bool bSave);
	void SetSliderBound();
	void LoadDefault();
	void SetLabelsText();

	virtual bool OnInitContext();
	virtual void OnShowDialog();

	void OnCommandMoveSlider(const char * szCommand);
	void OnCommandCreateRoad(const char* szCommand);
	void OnCommandTurnToBase(const char* szCommand);

	class AUIStillImageButton*  m_pBtnCreate;

	//begin----------与保存数据和同步数据相关--------------
	void OnCommandSave(const char* szCommand);
	void OnCommandSaveAs(const char* szCommand);

	enum BTN_TYPE
	{
		BTN_SAVE,
		BTN_SAVE_AS,
		BTN_NUM
	};
	
	AUIStillImageButton* m_pBtn[BTN_NUM];

	char m_szFileName[MAX_PATH];
	//end------------与保存数据和同步数据相关-------------


};

#endif