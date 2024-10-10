// File		: DlgLoginPage.h
// Creator	: Xiao Zhou
// Date		: 2005/11/23

#pragma once

#include "DlgBase.h"

class AUIObject;
class AUIComboBox;
class AUIStillImageButton;
class AUILabel;

class CDlgLoginPage : public CDlgBase  
{
	friend class ObjectList;
	
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgLoginPage();
	virtual ~CDlgLoginPage();
	
	void ChangeSize(int nWidth,int nHeight);

	void OnCommand_Cancel(const char * szCommand);

	void OnEvent_mousewheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEvent_lbuttonup(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual bool Release();
	virtual void OnTick();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
	virtual void OnChangeLayoutEnd(bool bAllDone);
	
	void SetBBSPagePos();

	bool					m_bUseNewFontSize;
	AString					m_aInitUrl;
	bool					m_bFirstOpen;
	bool					m_bLoaded;
};
