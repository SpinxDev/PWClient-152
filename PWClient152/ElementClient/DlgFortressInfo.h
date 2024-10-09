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
	enum {RESETTECH_ITEM = 39202};		//  ���ÿƼ��ĵ���

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

	enum {BUILDING_MATERIAL_COUNT = 5};	//	���Դ���Ĳ�������
	enum {TECHNOLOGY_COUNT = 5};		//	�Ƽ��츳����

	PAUIOBJECT		m_pBtn_Levelup;		//	������ť
	PAUIOBJECT		m_pLab_GuildName;	//	��������
	PAUIOBJECT		m_pTxt_Level;		//	���صȼ�
	PAUIPROGRESS	m_pPro_Exp;			//	���ؾ�����
	PAUIOBJECT		m_pLab_Exp;			//	���ؾ����ı�
	PAUIOBJECT		m_Lab_Technology;	//	�Ƽ�����ʣ��ֵ
	PAUIOBJECT		m_pLab_Tech[TECHNOLOGY_COUNT];		//	���Ƽ��츳������ʾ
	PAUIIMAGEPICTURE		m_pImg_Tech[TECHNOLOGY_COUNT];		//	���Ƽ��츳ͼƬ��ʾ
	PAUIOBJECT		m_pLab_M[BUILDING_MATERIAL_COUNT];		//	ʣ�������
	PAUIIMAGEPICTURE m_pImg_Reset;		//  ���õ���
};