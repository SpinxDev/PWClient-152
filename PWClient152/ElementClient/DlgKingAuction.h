// Filename	: DlgKingAuction.h
// Creator	: Shi && Wang
// Date		: 2013/1/29

#pragma once

#include "DlgBase.h"
#include <AUIStillImageButton.h>
#include <AUIEditBox.h>
#include "EC_RoleTypes.h"


class CDlgKingAuction : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgKingAuction();
	virtual ~CDlgKingAuction();

	void OnCommand_Confirm(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEvent_MoneyKeyUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	// ��Ӧ������Э��
	void OnKECandidateApply_Re(void* pData);

	// �ж�����Ƿ�ﵽ����
	static bool CheckPlayerCondition(bool bAuction, ACString& strMsg);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);

	PAUISTILLIMAGEBUTTON m_pBtnConfirm;
	int m_iMoneyCount;
};