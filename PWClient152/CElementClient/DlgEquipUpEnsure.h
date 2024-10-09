#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIRadioButton.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"
#include "DlgEquipUp.h"

#define INHERIT_PROPERTYNUM 5

class CDlgEquipUpEnsure : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
		
public:
	
	CDlgEquipUpEnsure();

	void OnCommand_Confirm(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void SetInheritStatus(char status, char activestatus, int Inheritfee);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();	

	PAUISTILLIMAGEBUTTON m_pBtn_Confirm;
	PAUILABEL m_pTxt_Num;
	PAUITEXTAREA m_pTxt_Inherit;
	PAUITEXTAREA m_pTxt_NoInherit;

	char m_inheriStatus;
	char m_activeStatus;
	int m_iTotalFee;
};
