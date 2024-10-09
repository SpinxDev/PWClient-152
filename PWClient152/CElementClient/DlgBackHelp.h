// Filename	: DlgBackHelp.h
// Creator	: Xu Wenbin
// Date		: 2009/08/27

#pragma once

#include "DlgTextHelp.h"
#include "AUIListBox.h"

class CDlgBackHelp : public CDlgTextHelp
{
	AUI_DECLARE_COMMAND_MAP();
	
public:
	CDlgBackHelp();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	
protected:	
	PAUILISTBOX m_pLst_Member;
};