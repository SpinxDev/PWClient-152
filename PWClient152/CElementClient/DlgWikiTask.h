// Filename	: DlgWikiTask.h
// Creator	: Feng Ning
// Date		: 2010/05/11

#pragma once

#include "DlgWikiByNameBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"

class ATaskTempl;
class CDlgWikiTask : public CDlgWikiByNameBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:

	// click this link command will jump to a wikipage showing
	// specific task by input task_id
	class ShowSpecficLinkCommand : public CDlgNameLink::LinkCommand
	{
	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		ShowSpecficLinkCommand(unsigned int task_id, const ACString& name);
		
	protected:
		virtual ACString GetLinkText() const;
		virtual LinkCommand* Clone() const;
		
	private:
		unsigned int m_TaskID;
		ACString m_Name;
		ACString m_Color;
	};
	
public:
	CDlgWikiTask();
	
	void OnCommand_Detail(const char *szCommand);
	
	void OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	virtual void ResetSearchCommand();

protected:
	virtual bool OnInitDialog();
	virtual bool Release(void);

protected:
	
	// derived from wiki base
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual void OnEndSearch();
	virtual void OnCommandCancel();

private:
	ATaskTempl* GetSeletedTask();

	ACString& GetClassName(unsigned long ulOccupation, unsigned long* pOccupations, ACString& szTxt);
	ACString& GetNPCName(unsigned long id, ACString& szTxt);
	ACString& GetLevelRequire(unsigned long minLevel, unsigned long maxLevel, ACString& szTxt);
	bool SetItemIcon(PAUIIMAGEPICTURE pIcon, unsigned int item_id, unsigned int count);
	ACString GetColorName(int idType);

	PAUILISTBOX m_pList;
	PAUISTILLIMAGEBUTTON m_pBtn_Detail;
};