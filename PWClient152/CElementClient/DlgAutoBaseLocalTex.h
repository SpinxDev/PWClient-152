/*
 * FILE: DlgAutoBaseLocalTex.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_BASE_LOCAL_TEX_H_
#define _DLG_AUTO_BASE_LOCAL_TEX_H_

#include "DlgHomeBase.h"
#include "EC_HomeDlgsMgr.h"
#include "AutoParameters.h"

class AUIStillImageButton;
class AUIListBox;

class CDlgAutoBaseLocalTex : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoBaseLocalTex();
	virtual ~CDlgAutoBaseLocalTex();
	
	virtual AUIStillImageButton * GetSwithButton();
	void SetBtnCreateToModify();
	void SetBtnModifyToCreate();


	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	
	
	void OnCommandCreateLocalTex(const char* szCommand);
	

	AUIListBox* m_pListBoxTexture;
	int m_nTextureIdx;
	
	AUIListBox* m_pListBoxLocalInfo;
	int m_nLocalInfoIdx;

	

	//begin----------与保存数据和同步数据相关--------------
public:
	void InsertLocalTexSettings(const ACHAR* szFileName);
	void InsertTexSettings(const ACHAR* szFileName);
	void SetTexOpEnable(bool i_bEnable);
	
protected:
	int m_iPreLocalTexNum;
	int m_iPreTexNum;

	enum BTN_TYPE
	{
		BTN_DEL_LOCAL_TEX,
		BTN_ADV_LOCAL_TEX,
		BTN_CREATE_LOCAL_TEX,
		BTN_DEL_TEX,
		BTN_ADV_TEX,
		BTN_APPLY_TEX,
		BTN_NUM
	};

	AUIStillImageButton* m_pBtn[BTN_NUM];
	
	void FillTextureListBox();
	void FillLocalInfoListBox();
	bool GetTextureParameter(int a_nIdx, TEXTUREINFO* a_pTexInfo);
	bool GetLocalInfoParameter(int a_nIdx, PARTTEXTUREINFO* a_pLocalInfo);

	void OnCommandAdvance(const char* szCommand);
	void OnCommandDelPre(const char* szCommand);
	void OnLButtonUpListBoxLocalTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnCommandAdvTex(const char* szCommand);
	void OnCommandDelTex(const char* szCommand);
	void OnCommandApplyTex(const char* szCommand);
	void OnLButtonUpListBoxTex(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	//end----------与保存数据和同步数据相关--------------
};

#endif