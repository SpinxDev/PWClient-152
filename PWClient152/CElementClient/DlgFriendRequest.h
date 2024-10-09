// File		: DlgFriendRequest.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"

class CDlgFriendRequest : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandAccept(const char *szCommand);
	void OnCommandRefuse(const char *szCommand);
	void OnCommandAdd(const char *szCommand);
	void OnCommandBlackList(const char *szCommand);

	CDlgFriendRequest();
	virtual ~CDlgFriendRequest();

protected:
	virtual void DoDataExchange(bool bSave);
	
	AUIStillImageButton *	m_pBtnApply;
	AUIStillImageButton *	m_pBtnRefuse;
	AUIStillImageButton *	m_pBtnAdd;
	AUIStillImageButton *	m_pBtnBlackList;
};
