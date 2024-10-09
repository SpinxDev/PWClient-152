// Filename	: DlgDomainCondition.h
// Creator	: Shi && Wang
// Date		: 2013/1/29

#pragma once

#include "DlgBase.h"
#include <AUIStillImageButton.h>
#include "EC_RoleTypes.h"

class CDlgDomainCondition : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

	enum
	{
		MAX_PLAYER_COUNT = 99,
	};

public:
	CDlgDomainCondition();
	virtual ~CDlgDomainCondition();

	void OnCommand_Confirm(const char * szCommand);

	void SetLimitData(const abase::vector<int>& player_cnt, const abase::vector<int>& ghost);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);

	abase::vector<int> m_iPlayerLimit;
	abase::vector<int> m_iGhostLimit;
};