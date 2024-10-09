// Filename	: DlgWikiEquipment.h
// Creator	: Feng Ning
// Date		: 2010/04/13

#pragma once

#include "DlgWikiByNameBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"

class WikiEquipmentProp;
class CDlgWikiEquipment : public CDlgWikiByNameBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:

	// click this link command will jump to current wiki
	class ShowSpecficLinkCommand : public CDlgNameLink::LinkCommand
	{
	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		ShowSpecficLinkCommand(unsigned int item_id, const ACString& name);
		
	protected:
		virtual ACString GetLinkText()  const;
		virtual LinkCommand* Clone() const;
		
	private:
		unsigned int m_ItemID;
		ACString m_Name;
	};
	
public:
	CDlgWikiEquipment();
	
	void OnCommand_Drop(const char *szCommand);
	void OnCommand_RandomProperty(const char *szCommand);
	
	void OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

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
	WikiEquipmentProp* GetSeletedProperty();

	ACString& GetItemName(unsigned int item_id, ACString& szTxt);
	ACString& GetAddonName(unsigned int item_id, unsigned int addon_id, ACString& szTxt);

	PAUILISTBOX m_pList;
	
	PAUISTILLIMAGEBUTTON m_pBtn_Drop;
	PAUISTILLIMAGEBUTTON m_pBtn_RandomProperty;
	WikiEquipmentProp* m_pEP;
};