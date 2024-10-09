// File		: DlgLoginQueue.h
// Creator	: Xu Wenbin
// Date		: 2015/1/22

#pragma once

#include "DlgBase.h"
#include "EC_Observer.h"

class AUIObject;

class CECLoginQueue;
typedef CECObserver<CECLoginQueue>	CECLoginQueueObserver;

//	class CDlgLoginQueue
class CDlgLoginQueue : public CDlgBase, public CECLoginQueueObserver
{
	AUI_DECLARE_COMMAND_MAP()

	AUIObject *		m_pTXT_CurrentQueuePosition;
	AUIObject *		m_pTXT_VIPQueueNumber;
	AUIObject *		m_pTXT_CurrentQueueLeftTime;
	AUIObject *		m_pBtn_Cancel;

	void UpdateQueueNumber(AUIObject *pTxt_Number, int size);

	void UpdateCurrentQueuePosition();
	void UpdateVIPQueueNumber();
	void UpdateLeftWaitTime();
	void EnableQuitQueue(bool bEnable);
	void UpdateALL();

	void EssentialShow(bool bShow);

	bool IsMatch(const CECLoginQueue *p);
	
protected:
	virtual bool OnInitDialog();

public:
	CDlgLoginQueue();
	
	static const char * GetDialogNameForVIPQueue();
	static const char * GetDialogNameForNormalQueue();

	//	ÅÉÉú×Ô	CECLoginQueueObserver
	virtual void OnRegistered(const CECLoginQueue *p);
	virtual void OnModelChange(const CECLoginQueue *p, const CECObservableChange *q);

	void OnCommand_Recharge(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
};