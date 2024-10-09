// Filename	: DlgCongregate.h
// Creator	: Feng Ning
// Date		: 2011/04/14

#pragma once

#include "DlgBase.h"

class CDlgCongregate : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgCongregate();
	virtual ~CDlgCongregate();

	void OnCommand_Confrim(const char * szCommand);
	void OnCommand_Reject(const char * szCommand);
	void OnCommand_Later(const char * szCommand);

	void OnCongregateAction(int idAction, void* pData);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();

	ACString GetTypeName();
	bool IsAvailable() {return m_LastTime != 0;}
	void Reset();

	int m_Type;

	int m_Sponsor;
	int m_Timeout;
	int m_LastTime;

	abase::vector<int> m_RejectID;
	void RefreshPendingMsg();

	PAUIOBJECT m_pBtn_Later;
	PAUIOBJECT m_pTxt_Msg;
	PAUIOBJECT m_pBtn_Confirm;
};
