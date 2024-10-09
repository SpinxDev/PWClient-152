// Filename	: DlgFaceName.h
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#pragma once
#include "DlgCustomizeBase.h"

class AUIEditBox;
class AUIStillImageButton;

class CDlgFaceName : public CDlgCustomizeBase  
{
	
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgFaceName();
	virtual ~CDlgFaceName();
	
	void OnCommand_confirm(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	const wchar_t* GetInput(void);
	virtual bool OnInitContext();
	virtual void OnTick(void);
	
protected:
	virtual void DoDataExchange(bool bSave);
	bool CDlgFaceName::CheckValid(void);
	ACString m_AStrName;
	AUIStillImageButton* m_pBtnConfirm;
	ACHAR m_wLawlessWChar[10]; 	
	PAUIOBJECT	m_pTxtInput;
};
