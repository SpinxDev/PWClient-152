// Filename	: DlgCWindow.h
// Creator	: Xiao Zhou
// Date		: 2008/11/27

#pragma once

#include "DlgBase.h"
#include "AUIWindowPicture.h"
#include "AUICheckBox.h"
#include "AUIScroll.h"

class CDlgCWindow : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
public:
	CDlgCWindow();
	virtual ~CDlgCWindow();
	void WindowResize(int width, int height);

	void OnCommandSelect(const char *szCommand);
	void OnCommandClear(const char *szCommand);
	void OnCommandRefresh(const char *szCommand);
	void OnCommandCancel(const char *szCommand);

	void OnEventLButtonDown_Img_ZoomIn(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Img_ZoomIn(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Img_ZoomIn(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	PAUIWINDOWPICTURE m_pWPic;
	static int m_nCWindowNum;

	int m_nMouseLastX;
	int m_nMouseLastY;
	SIZE m_nOldSize;
	PAUIOBJECT m_pImg_ZoomIn;
	PAUIOBJECT m_pImg_Title;
//	PAUICHECKBOX m_pChk_IsWindow;
	PAUISCROLL m_pScl_Horizon;
	PAUISCROLL m_pScl_Vertical;

	virtual void OnTick();
	virtual bool Render();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
