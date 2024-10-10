/*
 * FILE: DlgAutoChooseTexture.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_CHOOSE_TEXTURE_H_
#define _DLG_AUTO_CHOOSE_TEXTURE_H_

#include "DlgHomeBase.h"
#include "AutoTexture.h"
#include "AutoTerrainCommon.h"

#include "AutoParameters.h"

class AUISlider;
class AUILabel;
class AUIListBox;
class AUIImagePicture;
class AUIStillImageButton;



class CDlgAutoChooseTexture: public CDlgHomeBase
{
	
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoChooseTexture();
	virtual ~CDlgAutoChooseTexture();

	//Call it before Show()
	bool InitTextureParamInfo(const TextureParamInfo_t& param, 
							  ChooseTexInitParam_e texInitParam,
								bool		i_bSaveEnable = false,
								const char* i_szFileName = NULL);

	TextureParamInfo_t* GetTextureParam() { return &m_texParamInfo;} 

protected:


	AUISlider* m_pSliderMainTextureU;
	AUISlider* m_pSliderMainTextureV;
	AUISlider* m_pSliderPatchTextureU;
	AUISlider* m_pSliderPatchTextureV;
	
	
	int m_nMainTextureU;
	int m_nMainTextureV;
	int m_nPatchTextureU;
	int m_nPatchTextureV;

	
	
	AUILabel* m_pLabelMainTextureU;
	AUILabel* m_pLabelMainTextureV;
	AUILabel* m_pLabelPatchTextureU;
	AUILabel* m_pLabelPatchTextureV;

	AUIListBox* m_pListBoxMainTexture;
	AUIListBox* m_pListBoxPatchTexture;
	int m_nMainTexIdx;
	int m_nPatchTexIdx;

	AUIImagePicture* m_pImagePicMainTexture;
	AUIImagePicture* m_pImagePicPatchTexture;

	CAutoTexture::TEXTURETYPEINFO* m_pTexInfo;
	int m_nNumTexInfos;
	
	A2DSprite* m_pA2DSpriteMainTexture;
	A2DSprite* m_pA2DSpritePatchTexture;

	TextureParamInfo_t m_texParamInfo;
	int m_nTexInitParam;

protected:
	virtual bool OnInitContext();
	virtual void OnShowDialog();

	virtual void DoDataExchange(bool bSave);

	void SetSliderBound();
	void LoadDefault();
	void SetLabelsText();
	bool InitResource();
	void ClearListBox();
	void FillTextureParam(int nMainTexSel, int nPatchTexSel);

	

	void OnCommandMoveSlider(const char * szCommand);
	void OnCommandOK(const char* szCommand);
	void OnCommandCancel(const char* szCommand);

	void OnLButtonUpListBoxMainTexture(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonUpListBoxPatchTexture(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

 	//begin----------与保存数据和同步数据相关--------------
 	void OnCommandSave(const char* szCommand);
 	void OnCommandSaveAs(const char* szCommand);
	void OnCommandApply(const char* szCommand);
 
 	enum BTN_TYPE
 	{
 		BTN_SAVE,
 		BTN_SAVE_AS,
 		BTN_APPLY,
 		BTN_NUM
 	};
 	
 	AUIStillImageButton* m_pBtn[BTN_NUM];
 
 	char m_szFileName[MAX_PATH];
	//end------------与保存数据和同步数据相关-------------
};

#endif
