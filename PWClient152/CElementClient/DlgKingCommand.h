// Filename	: DlgKingCommand.h
// Creator	: Shi && Wang
// Date		: 2013/1/29

#pragma once

#include "DlgBase.h"
#include <AUIStillImageButton.h>
#include <AUILabel.h>
#include "EC_RoleTypes.h"
#include <vector.h>

class CDlgKingCommand : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgKingCommand();
	virtual ~CDlgKingCommand();

	void OnCommand_Order(const char * szCommand);

	int GetDomainID() const { return m_iDomainID; }
	void SetDomainID(int iDomainID) { m_iDomainID = iDomainID; }

	void DoAssignAssault( bool bBig );
	void DoCancelDomainLimit();

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual void OnShowDialog();

	int m_iDomainID;	// 命令所作用的区域ID
	PAUILABEL m_pLblScore;
	PAUISTILLIMAGEBUTTON m_pBtnUse1;
	PAUISTILLIMAGEBUTTON m_pBtnUse2;
	PAUISTILLIMAGEBUTTON m_pBtnCancel;
};