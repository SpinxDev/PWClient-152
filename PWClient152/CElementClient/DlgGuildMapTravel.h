// Filename	: DlgGuildMapTravel.h
// Creator	: Xiao Zhou
// Date		: 2005/2/7

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"

class CDlgGuildMapTravel : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgGuildMapTravel();
	virtual ~CDlgGuildMapTravel();

	void OnCommandCancel(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);

	void SetDomainId(int n);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();

	PAUILISTBOX				m_pLst_Target;
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
};
