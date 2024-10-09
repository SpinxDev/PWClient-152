// File		: DlgLoginQueue.cpp
// Creator	: Xu Wenbin
// Date		: 2015/1/22

#include "DlgLoginQueue.h"
#include "EC_BaseUIMan.h"
#include "EC_LoginQueue.h"

//	class CDlgLoginQueue
AUI_BEGIN_COMMAND_MAP(CDlgLoginQueue, CDlgBase)
AUI_ON_COMMAND("Btn_Recharge", OnCommand_Recharge)
AUI_ON_COMMAND("Btn_Cancel", OnCommandCancel)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_END_COMMAND_MAP()

CDlgLoginQueue::CDlgLoginQueue()
: m_pTXT_CurrentQueuePosition(NULL)
, m_pTXT_VIPQueueNumber(NULL)
, m_pTXT_CurrentQueueLeftTime(NULL)
, m_pBtn_Cancel(NULL)
{
}

bool CDlgLoginQueue::OnInitDialog(){
	m_pTXT_CurrentQueuePosition = GetDlgItem("TXT_CurrentQueuePosition");
	m_pTXT_VIPQueueNumber = GetDlgItem("TXT_VIPQueueNumber");
	m_pTXT_CurrentQueueLeftTime = GetDlgItem("TXT_QueueTime");
	m_pBtn_Cancel = GetDlgItem("Btn_Cancel");
	return CDlgBase::OnInitDialog();
}

void CDlgLoginQueue::OnCommand_Recharge(const char *szCommand){
	GetBaseUIMan()->NavigateRechargeURL();
}

void CDlgLoginQueue::OnCommandCancel(const char * szCommand){
	if (CECLoginQueue::Instance().QuitQueue()){
		EnableQuitQueue(false);
	}
}

void CDlgLoginQueue::EnableQuitQueue(bool bEnable){
	if (m_pBtn_Cancel){
		m_pBtn_Cancel->Enable(bEnable);
	}
}

void CDlgLoginQueue::UpdateQueueNumber(AUIObject *pTxt_Number, int size){
	if (pTxt_Number){
		ACString strText;
		if (CECLoginQueue::Instance().InQueue()){
			strText.Format(_AL("%d"), size);
		}else{
			strText = _AL("-");
		}
		pTxt_Number->SetText(strText);
	}
}

void CDlgLoginQueue::UpdateCurrentQueuePosition(){
	UpdateQueueNumber(m_pTXT_CurrentQueuePosition, CECLoginQueue::Instance().CurrentQueuePosition());
}

void CDlgLoginQueue::UpdateVIPQueueNumber(){
	UpdateQueueNumber(m_pTXT_VIPQueueNumber, CECLoginQueue::Instance().VIPQueueSize());
}

void CDlgLoginQueue::UpdateLeftWaitTime(){
	if (m_pTXT_CurrentQueueLeftTime){
		ACString strText;
		if (CECLoginQueue::Instance().InQueue() && IsMatch(&CECLoginQueue::Instance())){
			int leftTime = CECLoginQueue::Instance().LeftEnterTime();
			if (leftTime <= 0 || leftTime > 4 * 3600 * 1000){
				strText = GetStringFromTable(1612);
			}else{
				strText = GetBaseUIMan()->GetFormatTimeSimple(a_Max(leftTime/1000, 61));	//	强制以分钟为最小刻录
			}
		}else{
			strText = _AL("-");
		}
		m_pTXT_CurrentQueueLeftTime->SetText(strText);
	}
}

void CDlgLoginQueue::UpdateALL(){
	UpdateCurrentQueuePosition();
	UpdateVIPQueueNumber();
	UpdateLeftWaitTime();
	EnableQuitQueue(CECLoginQueue::Instance().InQueue());
}

const char * CDlgLoginQueue::GetDialogNameForVIPQueue(){
	return "Win_LoginQueueVIP";
}

const char * CDlgLoginQueue::GetDialogNameForNormalQueue(){
	return "Win_LoginQueueNormal";
}

bool CDlgLoginQueue::IsMatch(const CECLoginQueue *p){
	if (!p->InQueue()){
		return false;
	}
	return m_szName == (p->InVIPQueue() ? GetDialogNameForVIPQueue() : GetDialogNameForNormalQueue());
}

void CDlgLoginQueue::EssentialShow(bool bShow){
	if (IsShow() == bShow){
		return;
	}
	if (IsShow()){
		Show(false);
	}else{
		Show(true, true);
	}
}

void CDlgLoginQueue::OnRegistered(const CECLoginQueue *p){
	UpdateALL();
}

void CDlgLoginQueue::OnModelChange(const CECLoginQueue *p, const CECObservableChange *q){
	const CECLoginQueueChange *pChange = dynamic_cast<const CECLoginQueueChange *>(q);
	if (!pChange){
		ASSERT(false);
		return;
	}
	if (pChange->EnterQueue()){
		if (IsMatch(p)){
			UpdateALL();
			EssentialShow(true);
		}else{
			EssentialShow(false);
		}
		return;
	}
	if (pChange->QuitQueue()){
		EssentialShow(false);
		return;
	}
	if (pChange->ChangeQueue()){
		if (!IsMatch(p)){
			EssentialShow(false);
		}else{
			UpdateALL();
			EssentialShow(true);
		}
		return;
	}
	if (pChange->VIPQueueSizeChange()){
		UpdateVIPQueueNumber();
	}
	if (pChange->CurrentQueuePositionChange()){
		UpdateCurrentQueuePosition();
	}
	if (pChange->EnterTimeChange()){
		UpdateLeftWaitTime();
	}
	if (pChange->QuitQueueFailed()){
		EnableQuitQueue(true);
	}
}
