// Filename	: DlgFortressInfo.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIProgress.h"
#include "AUIListbox.h"
#include "AUIImagePicture.h"

class CDlgFortressInfo : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	enum {RESETTECH_ITEM = 39202};		//  重置科技的道具

public:
	CDlgFortressInfo();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Levelup(const char *szCommand);
	void OnCommand_Upgrade(const char *szCommand);

	void OnEvent_RButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEvent_RButtonUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	void UpdateInfo();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void UpdateResetItem();
	
private:

	enum {BUILDING_MATERIAL_COUNT = 5};	//	可以处理的材料总数
	enum {TECHNOLOGY_COUNT = 5};		//	科技天赋总数

	PAUIOBJECT		m_pBtn_Levelup;		//	升级按钮
	PAUIOBJECT		m_pLab_GuildName;	//	帮派名称
	PAUIOBJECT		m_pTxt_Level;		//	基地等级
	PAUIPROGRESS	m_pPro_Exp;			//	基地经验条
	PAUIOBJECT		m_pLab_Exp;			//	基地经验文本
	PAUIOBJECT		m_Lab_Technology;	//	科技点数剩余值
	PAUIOBJECT		m_pLab_Tech[TECHNOLOGY_COUNT];		//	各科技天赋文字显示
	PAUIIMAGEPICTURE		m_pImg_Tech[TECHNOLOGY_COUNT];		//	各科技天赋图片显示
	PAUIOBJECT		m_pLab_M[BUILDING_MATERIAL_COUNT];		//	剩余材料数
	PAUIIMAGEPICTURE m_pImg_Reset;		//  重置道具
};