// File		: DlgPetList.h
// Creator	: Xiao Zhou
// Date		: 2005/12/26

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"

#define CDLGPETLIST_SLOT_MAX 10
#define CDLGPETLIST_PAGE_MAX 2

class CDlgPetList : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgPetList();
	virtual ~CDlgPetList();

	void OnCommandSummon(const char * szCommand);
	void OnCommandRecall(const char * szCommand);
	void OnCommandDetail(const char * szCommand);
	void OnCommandBanish(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommandEvolution(const char * szCommand);

	void OnCommandPage(const char* szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void UpdateList();
	int GetPetIndex(int nSlot);

	void OnPetDragDrop(int petIndex, PAUIOBJECT pObjOver);
	
protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	
	bool	IsPetDye(int nSlot);
	
	PAUIIMAGEPICTURE		m_pBtn_Summon[CDLGPETLIST_SLOT_MAX];
	PAUIIMAGEPICTURE		m_pBtn_Recall[CDLGPETLIST_SLOT_MAX];
	PAUIIMAGEPICTURE		m_pBtn_Detail[CDLGPETLIST_SLOT_MAX];
	PAUIIMAGEPICTURE		m_pBtn_Banish[CDLGPETLIST_SLOT_MAX];
	PAUIIMAGEPICTURE		m_pBtn_Evolution[CDLGPETLIST_SLOT_MAX];
	PAUIIMAGEPICTURE		m_pImg_Icon[CDLGPETLIST_SLOT_MAX];
	PAUILABEL				m_pLab_Name[CDLGPETLIST_SLOT_MAX];
	PAUILABEL				m_pLab_Level[CDLGPETLIST_SLOT_MAX];
	PAUILABEL				m_pTxt_Name[CDLGPETLIST_SLOT_MAX];
	PAUILABEL				m_pTxt_Level[CDLGPETLIST_SLOT_MAX];

	PAUISTILLIMAGEBUTTON	m_pBtn_Page[CDLGPETLIST_PAGE_MAX];

private:
	int m_nPageIndex;
};
