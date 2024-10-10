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
	PAUIOBJECT	m_pLab_GuildName;		//	帮派自身名称
	PAUIOBJECT	m_pTxt_Level;					//	帮派自身等级

	PAUIOBJECT			m_pLab_Exp;				//	基地经验文字显示
	PAUIPROGRESS	m_pPro_Exp;				//	基地经验进度条
	PAUIOBJECT			m_pLab_Health;			//	健康度文字显示
	PAUIPROGRESS	m_pPro_Health;			//	健康度进度条
	PAUIOBJECT			m_pTxt_FortressLevel;	//	基地等级

	PAUIOBJECT	m_pLab_CumulateContrib;	//	累积贡献度
	PAUIOBJECT	m_pLab_ConsumeContrib;	//	可消费贡献度
	PAUIOBJECT	m_pLab_ExpContrib;			//	可转换为帮派经验贡献度
};