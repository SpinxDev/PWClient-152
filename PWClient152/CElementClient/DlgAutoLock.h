// Filename	: DlgAutoLock.h
// Creator	: Xiao Zhou
// Date		: 2007/10/17

#pragma once

#include "DlgBase.h"

class CDlgAutoLock : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgAutoLock();
	virtual ~CDlgAutoLock();

	void OnCommand_Confirm(const char * szCommand);

	void InitAutoLockInfo(void* pData);
	void SetAutoLockTime(void* pData);
	bool IsLocked()	{ return m_bLock; }
	bool IsPermanentLock() const { return m_bPermanentLock;}

protected:
	virtual void OnShowDialog();
	virtual void OnTick();

	int		m_nTotalTime;
	int		m_nEndTime;
	bool	m_bLock;
	bool	m_bPermanentLock; // ”¿æ√À¯∂®
};
