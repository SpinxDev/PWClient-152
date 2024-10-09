// Filename	: DlgFortressTechnology.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

class CDlgFortressTechnology : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressTechnology();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Upgrade(const char *szCommand);

	void UpdateInfo();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	
private:
	
	enum {TECHNOLOGY_COUNT = 5};			//	�Ƽ��츳����

	PAUIOBJECT					m_pLab_Technology;										//	ʣ��Ƽ�����
	PAUIOBJECT					m_pLab_Tech[TECHNOLOGY_COUNT];		//	���Ƽ��츳������ʾ
	PAUIIMAGEPICTURE	m_pImg_Tech[TECHNOLOGY_COUNT];		//	���Ƽ��츳ͼƬ��ʾ
};