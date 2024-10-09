/********************************************************************
	created:	2012/1/8
	created:	12:11:2012   16:31
	file base:	DlgPlayerRename
	file ext:	h
	author:		zhougaomin01305
	
	purpose:	½ÇÉ«¸ÄÃû
*********************************************************************/


#pragma once

#include "DlgBase.h"

//	class CDlgModifyNamePolicy
class CDlgModifyNamePolicy{
	typedef abase::vector<int>	CandidateIDs;
	CandidateIDs		m_candidateIDs;
protected:
	void AddCandiateID(int id);
public:
	virtual ~CDlgModifyNamePolicy(){}

	bool GetCostItemID(int &id, int &iSlot)const;

	virtual int GetTitleStringID()const=0;
	virtual int GetInputHintStringID()const=0;
	virtual int GetConfirmStringID()const=0;
	virtual const char *GetMessageBoxName()const=0;
	virtual void SendProtocol(const ACString &strNewName, int id, int iSlot)=0;
	virtual int GetMaxNameLength()const=0;
};

//	class CDlgModifyName
class AUIEditBox;
class AUIStillImageButton;
class CDlgModifyName : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

protected:	
	int						m_index;
	int						m_id;

	AUIObject *				m_pLbl_Title;
	AUIObject *				m_pLbl_Msg;
	AUIEditBox *			m_pEdt_Name;
	AUIStillImageButton *	m_pBtn_Confirm;

	CDlgModifyNamePolicy *	m_pPolicy;
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();

public:
	CDlgModifyName();

	void SetPolicy(CDlgModifyNamePolicy *);

	const char * GetMessageBoxName()const;
	void SendRenameProtocol();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

	void OnEventKeyUp_Edt_Name(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

};

//	class CDlgPlayerRenamePolicy
class CDlgPlayerRenamePolicy : public CDlgModifyNamePolicy{
public:
	CDlgPlayerRenamePolicy();
	virtual int GetTitleStringID()const;
	virtual int GetInputHintStringID()const;
	virtual int GetConfirmStringID()const;
	virtual const char *GetMessageBoxName()const;
	virtual void SendProtocol(const ACString &strNewName, int id, int iSlot);
	virtual int GetMaxNameLength()const;
};

//	class CDlgFactionRenamePolicy
class CDlgFactionRenamePolicy : public CDlgModifyNamePolicy{
public:
	CDlgFactionRenamePolicy();
	virtual int GetTitleStringID()const;
	virtual int GetInputHintStringID()const;
	virtual int GetConfirmStringID()const;
	virtual const char *GetMessageBoxName()const;
	virtual void SendProtocol(const ACString &strNewName, int id, int iSlot);
	virtual int GetMaxNameLength()const;
};