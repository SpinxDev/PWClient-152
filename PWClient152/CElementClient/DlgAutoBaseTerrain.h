/*
 * FILE: DlgAutoBaseTerrain.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_BASE_TERRAIN_H_
#define _DLG_AUTO_BASE_TERRAIN_H_

#include "DlgHomeBase.h"
#include "AutoParameters.h"

class AUIStillImageButton;
class AUIListBox;
class CAutoParameters;

class CDlgAutoBaseTerrain : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoBaseTerrain();
	virtual ~CDlgAutoBaseTerrain();
	
	virtual AUIStillImageButton * GetSwithButton();

	inline int GetPreTerrainNum() { return m_iPreTerrainNum;}
	inline int GetPreTexNum() { return m_iPreTexNum;}


	

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void FillTerrainListBox();
	void FillTextureListBox();

	void OnCommandCreateTerrain(const char* szCommand);
	bool GetTerrainParameter(int a_nIdx, MAINTERRAINHEIGHT* a_pMainTerr);

	
	bool GetTextureParameter(int a_nIdx, TEXTUREINFO* a_pTexInfo);
	



protected:
	AUIListBox* m_pListBoxTerrain;
	AUIListBox* m_pListBoxTexture;
	int m_nTerrainIdx;
	int m_nTextureIdx;


	//begin----------与保存数据和同步数据相关--------------
public:
	void InsertTerrainSettings(const ACHAR* szFileName);
	void InsertTexSettings(const ACHAR* szFileName);
	
protected:
	int m_iPreTerrainNum;
	int m_iPreTexNum;

	enum BTN_TYPE
	{
		BTN_DEL_TERRAIN,
		BTN_ADV_TERRAIN,
		BTN_CREATE_TERRAIN,
		BTN_DEL_TEX,
		BTN_ADV_TEX,
		BTN_APPLY_TEX,
		BTN_NUM
	};

	AUIStillImageButton* m_pBtn[BTN_NUM];
	
	void OnCommandAdvance(const char* szCommand);
	void OnCommandDelPre(const char* szCommand);
	void OnLButtonUpListBoxTerrain(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnCommandAdvTex(const char* szCommand);
	void OnCommandDelTex(const char* szCommand);
	void OnCommandApplyTex(const char* szCommand);
	void OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	//end----------与保存数据和同步数据相关--------------
};

#endif