/*
 * FILE: DlgAutoTools.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_AUTO_TOOLS_H_
#define _DLG_AUTO_TOOLS_H_

#include "DlgHomeBase.h"

class AUIStillImageButton;

class CDlgAutoTools : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoTools();
	virtual ~CDlgAutoTools();
	bool IsInTesting(void);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	void LoadDefault();

	void OnCommandOrthCamera(const char* szCommand);
	void OnCommandPersCamera(const char* szCommand);
	void OnCommandUndo(const char* szCommand);
	void OnCommandRedo(const char* szCommand);
	void OnCommandGenLightMap(const char* szCommand);

	void OnCommandStandardView(const char* szCommand);

	void OnCommandHideResDlg(const char* szCommand);
	
	void OnCommandTest(const char* szCommand);

	void OnCommandSoftGouraud(const char* szCommand);
	void OnCommandUpdateScene(const char* szCommand);

	//temp
	void OnCommandSaveScene(const char* szCommand);
	void OnCommandLoadScene(const char* szCommand);

	enum SHOWED_DLG
	{
		DLG_BASE_TERR,
		DLG_OUTDOOR,
		DLG_INDOOR,
		DLG_LIGHT
	};


	SHOWED_DLG m_BaseDlg;
	bool m_bShowNote2;
	bool m_bShowCtrlArea;
	bool m_bShowCtrl;

	void ShowUnTestDlgs(bool i_bShow);

	enum BTN_TYPE
	{
		BTN_TEST,
		BTN_ORTH_CAMERA,
		BTN_PERS_CAMERA,
		BTN_FREE_VIEW,
		BTN_GEN_LIGHTMAP,
		BTN_HIDE_RES,
		BTN_HIDE_CHAT,
		BTN_EXIT,
		BTN_NUM
	};
	
	AUIStillImageButton* m_pBtn[BTN_NUM];

};

#endif