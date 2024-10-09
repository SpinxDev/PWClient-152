// Filename	: DlgWeddingInvite.h
// Creator	: Xu Wenbin
// Date		: 2010/07/08

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgWeddingInvite : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWeddingInvite();
	
	void OnCommand_Confirm(const char *szCommand);
	void OnCommand_Cancel(const char *szCommand);
	
	void OnEventLButtonDown_Img(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_Name(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void SetItem(int iSlot = -1);
	
	void WriteCard(bool bOK, bool bForAnyOne = true);
	
	enum {INVITE_TICKET_ID = 28400};

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
		
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void Rewrite(bool bClearName);

	bool CanConfirm();
	ACString GetTimeString(int start_time, int end_time);
	bool IsImageItemValid(int iSlot);
	bool IsInviteItemValid();

	PAUIIMAGEPICTURE m_pImg_01;	//	场地租用证明
	PAUIOBJECT m_pTxt_01;
	PAUIOBJECT m_pTxt_Name;

	PAUISTILLIMAGEBUTTON m_pBtn_Confirm;
	PAUISTILLIMAGEBUTTON m_pBtn_Cancel;

	int  m_iSlot;
	bool m_bConfirmed;	//	是否已经点击确定
	time_t m_sendTime;	//	点击确定的时间
};