// File		: DlgFriendColor.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUISlider.h"
#include "AUIImagePicture.h"

class CDlgFriendColor : public CDlgBase  
{
	friend class CDlgFriendOptionGroup;

	AUI_DECLARE_COMMAND_MAP()
public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandStart(const char *szCommand);
	void OnCommandColor(const char *szCommand);

	CDlgFriendColor();
	virtual ~CDlgFriendColor();

protected:
	virtual void DoDataExchange(bool bSave);
	
	AUISlider *			m_pSliderColorR;
	AUISlider *			m_pSliderColorG;
	AUISlider *			m_pSliderColorB;
	AUILabel *			m_pTxtColorR;
	AUILabel *			m_pTxtColorG;
	AUILabel *			m_pTxtColorB;
	AUIImagePicture *	m_pImgColor;
};
