/*
 * FILE: DlgAutoMountain.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_MOUNTAIN_H_
#define _DLG_AUTO_MOUNTAIN_H_

#include "DlgHomeBase.h"
#include "AutoParameters.h"

class AUISlider;
class AUILabel;
class CCurvePoint;

class CDlgAutoMountain : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

protected:
	AUISlider* m_pSliderTextureRatio;
	AUISlider* m_pSliderMainTexture;
	AUISlider* m_pSliderPatchTexture;
	AUISlider* m_pSliderPatchDivision;
	
	int m_nTextureRatio;
	int m_nMainTexture;
	int m_nPatchTexture;
	int m_nPatchDivision;

	float m_fTextureRatio;
	float m_fMainTexture;
	float m_fPatchTexture;
	float m_fPatchDivision;

	AUILabel* m_pLabelTextureRatio;
	AUILabel* m_pLabelMainTexture;
	AUILabel* m_pLabelPatchTexture;
	AUILabel* m_pLabelPatchDivision;
	
	AUISlider* m_pSliderMaxHeightHill;
	AUISlider* m_pSliderMainSmoothHill;
	AUISlider* m_pSliderPartSmoothHill;
	AUISlider* m_pSliderTerrainSeedHill;
	AUISlider* m_pSliderBridgeScope;
	AUISlider* m_pSliderBridgeDepth;
	AUISlider* m_pSliderBorderErode;
	

	AUILabel* m_pLabelMaxHeightHill;
	AUILabel* m_pLabelMainSmoothHill;
	AUILabel* m_pLabelPartSmoothHill;
	AUILabel* m_pLabelTerrainSeedHill;
	AUILabel* m_pLabelBridgeScope;
	AUILabel* m_pLabelBridgeDepth;
	AUILabel* m_pLabelBorderErode;
	AUILabel* m_pLabelLineHillWidth;
	
	int m_nMaxHeightHill;
	int m_nMainSmoothHill;
	int m_nPartSmoothHill;
	int m_nTerrainSeedHill;
	int m_nBridgeScope;
	int m_nBridgeDepth;
	int m_nBorderErode;

	float m_fMainSmoothHill;
	float m_fPartSmoothHill;	
	float m_fBridgeScope;
	float m_fBridgeDepth;
	float m_fBorderErode;
	

	bool m_bCheckHillUp;
	int m_nUpDownPri;

	CCurvePoint* m_pCurveHeight;

	TextureParamInfo_t m_texParamInfo;

	
	class AUIStillImageButton* m_pBtnCreate;
	
public:
	CDlgAutoMountain();
	virtual ~CDlgAutoMountain();

	inline TextureParamInfo_t* GetTextureParamInfo() { return &m_texParamInfo;}

	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();

	//begin----------与保存数据和同步数据相关--------------
	void InitSettings(HILLINFO &i_HillInfo,
					  bool		i_bSaveEnable = false,
					  const char* i_szFileName = NULL);
	//end------------与保存数据和同步数据相关-------------
	
protected:


	virtual bool OnInitContext();
	virtual void OnShowDialog();
	
	virtual void DoDataExchange(bool bSave);

	void SetSliderBound();
	void LoadDefault();
	void SetLabelsText();

	void OnCommandMoveSlider(const char * szCommand);
	void OnCommandCreateHillPoly(const char* szCommand);
	void OnCommandOpenHeightCurveDlg(const char* szCommand);
	void OnCommandOnRadio(const char* szCommand);
	void OnCommandTurnToBase(const char* szCommand);

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