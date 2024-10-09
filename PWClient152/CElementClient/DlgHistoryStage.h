// Filename	: DlgHistoryStage.h 
// Creator	: WYD


#pragma once

#include "DlgBase.h"
#include "AUITextArea.h"

class CDlgHistoryStage : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	
public:
	CDlgHistoryStage();
	virtual ~CDlgHistoryStage();

	void GetProgressData();
	void OnRefreshPos();

protected:
	virtual void OnTick();
	virtual void OnShowDialog();
	
protected:
	int m_stage_idx;
	int m_query_counter;
};
