// File		: DlgFriendGroup.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"
#include "AUIComboBox.h"

class CDlgFriendGroup : public CDlgBase  
{
	friend class CDlgFriendOptionName;

	AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandConfirm(const char *szCommand);

	CDlgFriendGroup();
	virtual ~CDlgFriendGroup();
	
protected:
	virtual void DoDataExchange(bool bSave);
	
	AUIComboBox *	m_pComboGroup;
};
