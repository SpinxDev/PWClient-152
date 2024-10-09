// Filename	: DlgFortressStatus.h
// Creator	: Xu Wenbin
// Date		: 2010/11/18

#pragma once

#include "DlgBase.h"
#include <AUIProgress.h>

class CDlgFortressStatus : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressStatus();

	void UpdateInfo();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	
private:
	PAUIOBJECT	m_pLab_GuildName;		//	������������
	PAUIOBJECT	m_pTxt_Level;					//	��������ȼ�

	PAUIOBJECT			m_pLab_Exp;				//	���ؾ���������ʾ
	PAUIPROGRESS	m_pPro_Exp;				//	���ؾ��������
	PAUIOBJECT			m_pLab_Health;			//	������������ʾ
	PAUIPROGRESS	m_pPro_Health;			//	�����Ƚ�����
	PAUIOBJECT			m_pTxt_FortressLevel;	//	���صȼ�

	PAUIOBJECT	m_pLab_CumulateContrib;	//	�ۻ����׶�
	PAUIOBJECT	m_pLab_ConsumeContrib;	//	�����ѹ��׶�
	PAUIOBJECT	m_pLab_ExpContrib;			//	��ת��Ϊ���ɾ��鹱�׶�
};