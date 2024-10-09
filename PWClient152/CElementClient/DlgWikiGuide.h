// Filename	: DlgWikiGuide.h
// Creator	: Feng Ning
// Date		: 2010/09/25

#pragma once

#include "DlgWikiBase.h"
#include "AUIStillImageButton.h"
#include "AUIComboBox.h"
#include "AUICheckBox.h"

class WikiEntityPtrProvider;
class CDlgWikiGuide : public CDlgWikiBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWikiGuide();

protected:
	virtual bool OnInitDialog();
	virtual bool Release(void);
	virtual void OnShowDialog();

	void OnCommand_DetailButton(const char* szCommand);
	void OnCommand_SelectLevel(const char* szCommand);
	void OnCommand_AutoHide(const char* szCommand);

protected:
	
	// derived from wiki base
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual void OnEndSearch();
	virtual void OnCommandCancel();
	virtual int GetPageSize() const;

	void ConfirmSearch();

	WikiEntityPtrProvider* GetWikiData(PAUISTILLIMAGEBUTTON pBtn);
	WikiEntityPtrProvider* SetWikiData(PAUISTILLIMAGEBUTTON pBtn, WikiEntityPtrProvider* ptr);

	WikiEntityPtrProvider* m_pCurData;
	bool TryAddEquipGuide(WikiEntityPtr p);

	PAUISTILLIMAGEBUTTON m_pBtn_Skill;
	PAUISTILLIMAGEBUTTON m_pBtn_Task;
	PAUISTILLIMAGEBUTTON m_pBtn_Feature;
	PAUISTILLIMAGEBUTTON m_pBtn_Equ;
	PAUISTILLIMAGEBUTTON m_pBtn_Area;

	PAUICOMBOBOX m_pCombo_Level;
	PAUICHECKBOX m_pChk_AutoHide;
};