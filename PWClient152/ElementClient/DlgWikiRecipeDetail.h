// Filename	: DlgWikiRecipeDetail.h
// Creator	: Feng Ning
// Date		: 2010/07/26

#pragma once

#include "DlgNameLink.h"
#include "DlgWikiRecipe.h"
#include "AUIImagePicture.h"

class CDlgWikiRecipeDetail : public CDlgNameLink
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	// click this link command will jump to current wiki
	class ShowSpecficLinkCommand : public CDlgNameLink::LinkCommand
	{
	public:
		typedef WikiNPCMakingCache::CachePair CachePair;

		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		ShowSpecficLinkCommand(const CachePair& data, const ACString& name);
		
	protected:
		virtual ACString GetLinkText()  const;
		virtual CDlgNameLink::LinkCommand* Clone() const;
		
	private:
		CachePair m_Data;
		ACString m_Name;
	};

	CDlgWikiRecipeDetail();
	void OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release(void);

private:
	bool SetItemInfo(int id, int count, PAUIIMAGEPICTURE pIcon);
};