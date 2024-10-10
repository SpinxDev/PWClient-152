// File		: DlgWarTower.h
// Creator	: Xiao Zhou
// Date		: 2006/1/11

#pragma once

#include "DlgBase.h"
#include "AUIRadioButton.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"

#define WAR_TOWER_MAX 4

class CDlgWarTower : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWarTower();
	virtual ~CDlgWarTower();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	PAUILABEL				m_pTxt_Money[WAR_TOWER_MAX];
	PAUILABEL				m_pTxt_Item[WAR_TOWER_MAX];
	PAUIIMAGEPICTURE		m_pImg_Tower[WAR_TOWER_MAX];
	PAUIRADIOBUTTON			m_pRdo_Tower[WAR_TOWER_MAX];
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	
};
