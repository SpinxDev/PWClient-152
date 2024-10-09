/*
 * FILE: DlgAutoLocalTexture.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_LOCAL_TEXTURE_H_
#define _DLG_AUTO_LOCAL_TEXTURE_H_

#include "DlgHomeBase.h"

class AUISlider;
class AUILabel;

class CDlgAutoLocalTexture : public CDlgHomeBase
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
	
	AUISlider* m_pSliderLocalTexBorderScope;
	AUISlider* m_pSliderSeedLocalTexture;

	AUILabel* m_pLabelLocalTexBorderScope;
	AUILabel* m_pLabelSeedLocalTexture;

	int m_nLocalTexBorderScope;
	float m_fLocalTexBorderScope;

	int m_nSeedLocalTexture;

	int m_nRadioTexture;

	TextureParamInfo_t m_texParamInfo;
	
	
	class AUIStillImageButton* m_pBtnCreate;
	
public:
	CDlgAutoLocalTexture();
	virtual ~CDlgAutoLocalTexture();



	TextureParamInfo_t* GetTextureParamInfo() { return &m_texParamInfo;}

	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();
	

	//begin----------与保存数据和同步数据相关--------------
	void InitSettings(float		i_fBorderScope,
					  int		i_iLocalTexSeed, 
					  int		i_iRadioTex, 
					  float		i_fTextureRatio,
					  float		i_fMainTexture,
					  float		i_fPatchTexture,
					  float		i_fPatchDivision,
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
	void OnCommandCreateLocalTex(const char* szCommand);

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