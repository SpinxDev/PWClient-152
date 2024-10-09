// File		: DlgSimpleCostItemService.h
// Creator	: Xu Wenbin
// Date		: 2015/1/16

#pragma once

#include "DlgBase.h"
#include "EC_CustomizePolicy.h"

//	class CDlgSimpleCostItemServicePolicy
class CDlgSimpleCostItemServicePolicy{
public:	
	class ProtocolParameterReadyPolicy{
	public:
		virtual ~ProtocolParameterReadyPolicy(){}
		virtual void OnProtocolParameterReady()=0;
		virtual void OnProtocolParameterPrepareFail()=0;
		virtual void OnProtocolParameterPrepareCancel()=0;
	};

private:
	typedef abase::vector<int>	CandidateIDs;
	CandidateIDs		m_candidateIDs;

	ProtocolParameterReadyPolicy*	m_pProtocolParameterReadyPolicy;

protected:
	void AddCandiateID(int id);

	virtual void PrepareProtocolParameterImpl();
	void OnProtocolParameterReady();
	void OnProtocolParameterPrepareFail();
	void OnProtocolParameterPrepareCancel();

public:
	CDlgSimpleCostItemServicePolicy();
	virtual ~CDlgSimpleCostItemServicePolicy(){}
	
	virtual int		GetTitleStringID()const=0;
	virtual bool	CheckCondition(ACString &strErrorMessage)const=0;

	void	PrepareProtocolParameter(ProtocolParameterReadyPolicy *p);

	virtual int		GetConfirmStringID()const=0;
	virtual const char *GetMessageBoxName()const=0;
	virtual void	SendProtocol(int id, int iSlot)=0;

	bool			GetCostItemID(int &id, int &iSlot)const;
};

//	class CDlgSimpleCostItemService
class CDlgSimpleCostItemService : public CDlgBase, public CDlgSimpleCostItemServicePolicy::ProtocolParameterReadyPolicy
{
	AUI_DECLARE_COMMAND_MAP()

protected:	
	int				m_index;
	int				m_id;
	
	AUIObject *		m_pLbl_Title;
	AUIObject *		m_pBtn_Confirm;
	
	CDlgSimpleCostItemServicePolicy *	m_pPolicy;
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();

public:
	CDlgSimpleCostItemService();

	void SetPolicy(CDlgSimpleCostItemServicePolicy *);

	//	派生自CDlgSimpleCostItemServicePolicy::ProtocolParameterReadyPolicy
	virtual void OnProtocolParameterReady();
	virtual void OnProtocolParameterPrepareFail();
	virtual void OnProtocolParameterPrepareCancel();

	const char * GetMessageBoxName()const;
	void OnMessageBox(int value);

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
};

//	class CDlgChangeGenderPolicy
class CECLoginPlayer;
class CDlgChangeGenderPolicy : public CDlgSimpleCostItemServicePolicy, public CECCustomizePolicy{
	CECLoginPlayer	*	m_pPlayerAfterChangeGender;
protected:
	virtual void PrepareProtocolParameterImpl();

public:
	CDlgChangeGenderPolicy();
	virtual ~CDlgChangeGenderPolicy();
	
	//	派生自 CDlgSimpleCostItemServicePolicy
	virtual int		GetTitleStringID()const;
	virtual bool	CheckCondition(ACString &strErrorMessage)const;
	virtual int		GetConfirmStringID()const;
	virtual const char *GetMessageBoxName()const;
	virtual void	SendProtocol(int id, int iSlot);

	//	派生自 CECCustomizePolicy	
	virtual bool ShouldHideAllDialog()const;
	virtual bool ShouldShowOKCancel()const;
	virtual void OnOK(CECCustomizeMgr &manager);
	virtual void OnCancel(CECCustomizeMgr &manager);

private:
	bool CanChangeGender(ACString &strMessage)const;

	bool CheckHasSpouse(ACString &strMessage)const;
	bool CheckIsEquippingAnyFashion(ACString &strMessage)const;
	bool CheckSetPlayerProfileMatch(ACString &strMessage)const;
	bool CheckHasGenderRelatedTask(ACString &strMessage)const;
	ACString ColorString(const ACString &strMessage)const;

	void ShowPlayer(bool bShow);
	void FinishCustomize(CECCustomizeMgr &manager);
};
