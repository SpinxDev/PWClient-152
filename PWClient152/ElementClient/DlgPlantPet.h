// File		: DlgPlantPet.h
// Creator	: Xu Wenbin
// Date		: 2011/6/15

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include <vector.h>

class CDlgPlantPet : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	void OnCommand_CANCEL(const char * szCommand);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnTick();

	ACString GetTimeString(int timeLength, int timeLeft);

	typedef abase::vector<PAUIIMAGEPICTURE>	ImgList;
	ImgList	m_ImgList;

	typedef abase::vector<PAUILABEL>	LabelList;
	LabelList m_LabelList;
};
